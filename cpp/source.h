/**
 * @file source.h
 */

#pragma once
#include "rhythm_coach.h"

typedef CombFilterbank<unsigned, uint8_t, MaxDelay> filter_type;

struct DetectorSettings {
  uint_t hop_size;     // -h
  smpl_t minioi_ms;    // -i
  smpl_t silence;      // -s
  smpl_t threshold;    // -t
  smpl_t compression;  // -c
  char const *method;  // -m
  char const *source_path;
  char const *png_path;  // -o
  uint_t window_size;
  uint_t smoothing_window;  // -x
};

constexpr DetectorSettings defaultSettings = {
    256, defaultMinIOI, -90.0, 0.3, 0.0, "default", nullptr, nullptr, 1024, 11};

template <unsigned MaxFramesDelay, unsigned ImageWidth>
class Source {
 public:
  struct HistoryItem {
    filter_type::smoothed_type smoothed;
    unsigned num_onsets;
  };

  Source()
      : detector(nullptr),
        history(new HistoryItem[ImageWidth]),
        png_name(nullptr),
        input_name(nullptr),
        frames_since_last_onset(0),
        had_onset(false),
        latest_onset(0.0),
        processed_frames(0),
        skip(0) {}

  ~Source() { delete[] history; }

  void initialize() { skip = totalFrames() / ImageWidth; }

  bool applySettings(DetectorSettings const &settings);

  bool processNextFrame();

  void writeImage();

  unsigned totalFrames() const { return detector->total_hops(); }

  static bool getNextSource(Source &s, int &lastarg, int argc,
                            char const *argv[], const char *default_outputname);

  bool hadOnset() const { return had_onset; }

  void addToHistory(unsigned index) {
    history[index].num_onsets = comb_filter.num_items();
    comb_filter.smooth(history[index].smoothed, smoothing_window);
  }

 protected:
  filter_type comb_filter;
  AubioOnsetDetector *detector;
  HistoryItem *history;
  char const *png_name;
  char const *input_name;

  unsigned frames_since_last_onset;
  bool had_onset;
  float latest_onset;
  unsigned processed_frames;
  unsigned skip;
  unsigned smoothing_window;
};

static bool getFloat(char const *arg, float &dest) {
  char *str_end;
  dest = std::strtof(arg, &str_end);
  return *str_end == '\0';
}

static bool getUint(char const *arg, uint_t &dest) {
  char *str_end;
  dest = static_cast<uint_t>(std::strtoul(arg, &str_end, 10));
  return *str_end == '\0';
}

static void printSettings(DetectorSettings const &settings) {
  /* clang-format off */
    std::cerr << "settings: "
              << " -f " << settings.hop_size
              << " -i " << settings.minioi_ms
              << " -s " << settings.silence
              << " -t " << settings.threshold
              << " -c " << settings.compression
              << " -m " << settings.method
              << " -x " << settings.smoothing_window
              << " -o " << settings.png_path
              << " -w " << settings.window_size
              << " " << settings.source_path
              << "\n";
  /* clang-format on */
}

template <unsigned MaxFramesDelay, unsigned ImageWidth>
bool Source<MaxFramesDelay, ImageWidth>::processNextFrame() {
  if (!detector->process_samples()) {
    return false;
  }

  if ((had_onset = detector->is_onset())) {
    latest_onset = detector->last_ms();
    frames_since_last_onset = 0;
  } else {
    frames_since_last_onset++;
  }

  // add to delay line and update comb filter
  comb_filter.add_item(had_onset);
  if (processed_frames % skip == 0) {
    unsigned index = processed_frames / skip;
    if (index < ImageWidth) {
      addToHistory(index);
    }
  }
  processed_frames++;
  return true;
}

template <unsigned MaxFramesDelay, unsigned ImageWidth>
void Source<MaxFramesDelay, ImageWidth>::writeImage() {
  /* scale history into image pixels */
  image_type image(ImageWidth, MaxFramesDelay);

  for (unsigned x = 0; x < ImageWidth; x++) {
    HistoryItem &column_history(history[x]);
    for (int i = 0; i < 20; i++) {
      column_history.smoothed[i] = 0;
    }
    auto maxelem = std::max_element(column_history.smoothed.cbegin(),
                                    column_history.smoothed.cend());
    assert(maxelem);
    unsigned maxpos = maxelem - column_history.smoothed.cbegin();
    assert(maxpos < MaxFramesDelay);
    for (unsigned y = 0; y < MaxFramesDelay; y++) {
      float normalized = static_cast<float>(column_history.smoothed[y]) /
                         static_cast<float>(*maxelem);
      uint8_t val = std::round(normalized * 255);
      png::rgb_pixel pixel(val, val, val);
      image.set_pixel(x, y, pixel);
    }
    image.set_pixel(x, maxpos,
                    png::rgb_pixel(255, 0, 0));  // mark the maximum with red
    image.set_pixel(x, maxpos / 2, png::rgb_pixel(0, 255, 0));
    if (maxpos * 2 < MaxFramesDelay) {
      image.set_pixel(x, maxpos * 2, png::rgb_pixel(0, 0, 255));
    }
  }
  image.write(png_name);
}

template <unsigned M, unsigned I>
bool Source<M, I>::applySettings(DetectorSettings const &settings) {
  detector = new AubioOnsetDetector(settings.source_path, 0, settings.hop_size,
                                    settings.method, settings.window_size);
  if (!detector) {
    return false;
  }
  detector->set_threshold(settings.threshold);
  detector->set_minioi_ms(settings.minioi_ms);
  detector->set_silence(settings.silence);
  if (settings.compression != 0.0) {
    detector->set_compression(settings.compression);
  }
  input_name = settings.source_path;
  png_name = settings.png_path;
  skip = totalFrames() / I;
  smoothing_window = settings.smoothing_window;
  printSettings(settings);
  return true;
}

// class static
template <unsigned M, unsigned I>
bool Source<M, I>::getNextSource(Source &s, int &lastarg, int argc,
                                 char const *argv[],
                                 const char *default_outputname) {
  int i;
  DetectorSettings settings = defaultSettings;
  /* clang-format off */
    for (i = lastarg; i < argc; i++)
    {
        if (!strcmp("-h", argv[i])) return false;

        if (!strcmp("-f", argv[i])) { if (!getUint(argv[++i], settings.hop_size)) return false; continue; }
        if (!strcmp("-x", argv[i])) { if (!getUint(argv[++i], settings.smoothing_window)) return false; continue; }
        if (!strcmp("-i", argv[i])) { if (!getFloat(argv[++i], settings.minioi_ms)) return false; continue; }
        if (!strcmp("-s", argv[i])) { if (!getFloat(argv[++i], settings.silence)) return false; continue; }
        if (!strcmp("-t", argv[i])) { if (!getFloat(argv[++i], settings.threshold)) return false; continue; }
        if (!strcmp("-c", argv[i])) { if (!getFloat(argv[++i], settings.compression)) return false; continue; }
        if (!strcmp("-m", argv[i])) { settings.method = argv[++i]; continue; }
        if (!strcmp("-o", argv[i])) { settings.png_path = argv[++i]; continue; }
        if (!strcmp("-w", argv[i])) { if (!getUint(argv[++i], settings.window_size)) return false; continue; }
        // else
        settings.source_path = argv[i];
        break;
    }
  /* clang-format on */
  if (i >= argc) {
    return false;
  }

  if (!settings.png_path) {
    settings.png_path = default_outputname;
  }

  lastarg = i + 1;
  s.applySettings(settings);

  return true;
}
