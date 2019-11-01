/**
 * @file main.cpp
 */

#include "aubio_cpp.h"
#include "comb_filter.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <array>
#include <fstream>
#include <png++/png.hpp>

struct DetectorSettings {
  uint_t hop_size;    // -h
  smpl_t minioi_ms;   // -i
  smpl_t silence;     // -s
  smpl_t threshold;   // -t
  smpl_t compression; // -c
  char const *method;  // -m
  char const *source_path;
};

// 21.3 ms minioi = 4 frames of 256 samples at 48ksps
static float constexpr defaultMinIOI = 21.3;

DetectorSettings constexpr defaultSettings = { 256, defaultMinIOI, -90.0, 0.3, 0.0, "default", nullptr };

static char const *progname = nullptr;

static float minimumWindow = 10.0;  // ms; smallest perceptible difference (?)
static float maximumWindow = 40.0;  // ms; largest difference that isn't likely to be musically intended

static void usage() {
  std::cerr
      << "usage:\n"
      << progname
      << "[file1options] file1 [file2options] file2\noptions:\n"
      << "\t[-f hopsize]    hop (frame) size (default=256)\n"
      << "\t[-i minioi_ms]  minimum inter-onset interval in ms (default=12)\n"
      << "\t[-s silence]    silence threshold in dB (default=-90)\n"
      << "\t[-t threshold]  detection threshold (0-1) (default=0.3)\n"
      << "\t[-c compression]  log compression lambda (default=off)\n"
      << "\t[-m method]     detection method (default|hfc|energy|complex|complexdomain"
         "|phase|wphase|mkl|kl|specflux|specdiff)\n";
}

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
  std::cerr << "settings: "
          << " -f " << settings.hop_size
          << " -i " << settings.minioi_ms
          << " -s " << settings.silence
          << " -t " << settings.threshold
          << " -c " << settings.compression
          << " -m " << settings.method
          << " " << settings.source_path << "\n";
}

static bool getNextOnsetDetector(AubioOnsetDetector *&dest, int &lastarg, int argc,
                          char const *argv[]) {
  int i;
  DetectorSettings settings = defaultSettings;
  for (i = lastarg; i < argc; i++) {
    if (!strcmp("-h", argv[i])) {
      return false;
    }
    if (!strcmp("-f", argv[i])) {
      if (!getUint(argv[++i], settings.hop_size)) {
        return false;
      }
      continue;
    }
    if (!strcmp("-i", argv[i])) {
      if (!getFloat(argv[++i], settings.minioi_ms)) {
        return false;
      }
      continue;
    }
    if (!strcmp("-s", argv[i])) {
      if (!getFloat(argv[++i], settings.silence)) {
        return false;
      }
      continue;
    }
    if (!strcmp("-t", argv[i])) {
      if (!getFloat(argv[++i], settings.threshold)) {
        return false;
      }
      continue;
    }
    if (!strcmp("-c", argv[i])) {
      if (!getFloat(argv[++i], settings.compression)) {
        return false;
      }
      continue;
    }
    if (!strcmp("-m", argv[i])) {
      settings.method = argv[++i];
      continue;
    }
    settings.source_path = argv[i];
    break;
  }
  if (i >= argc || !settings.source_path) {
    return false;
  }
  lastarg = i + 1;
  dest = new AubioOnsetDetector(settings.source_path, 0, settings.hop_size,
                                settings.method);
  if (!dest)
    return false;
  dest->set_threshold(settings.threshold);
  dest->set_minioi_ms(settings.minioi_ms);
  dest->set_silence(settings.silence);
  if (settings.compression != 0.0) {
    dest->set_compression(settings.compression);
  }
  printSettings(settings);
  return true;
}

struct Source {
  AubioOnsetDetector *detector;
  bool had_onset;
  float onset_quality;
  float latest_onset;
};

Source sources[2];

/** save up to 3 seconds for rhythm analysis */
constexpr unsigned MaxDelay = { 48000 / 256 * 3 };

/** this stores the number of onsets for each frame back 2 seconds */
/** this is the comb filterbank for each frame of periodicity */
CombFilterbank<unsigned, uint8_t, MaxDelay> comb_filter;

static void initializeDetectors(int argc, char const *argv[]) {
  int lastarg = 1;
  for (int i = 0; i < 2; i++) {
    if (!getNextOnsetDetector(sources[i].detector, lastarg, argc, argv)) {
      usage();
      exit(1);
    }
  }
}

static uint_t processFiles(uint_t &num_windowed) {
  uint_t total_onsets = 0;
  num_windowed = 0;
  unsigned max_frames = std::max(sources[0].detector->total_hops(), sources[1].detector->total_hops());
  unsigned num_frames = 0;
  unsigned every_n = max_frames / 1024;
  uint_t retval;
  const char sep = ',';
  png::image<png::rgb_pixel> image(max_frames / every_n, MaxDelay);

  std::cout << "chan,had_onset,latest_onset,quality,diff\n";

  for (;;) {
    uint_t num_onsets = 0;

    for (int i = 0; i < 2; i++) {
      if (!sources[i].detector->process_samples()) { // end of file?
        retval = total_onsets;
        goto done;
      }
    }

    for (int i = 0; i < 2; i++) {
      if ((sources[i].had_onset = sources[i].detector->is_onset())) {
        sources[i].latest_onset = sources[i].detector->last_ms();
        num_onsets++;
      }
    }

    float quality = comb_filter.add_item(num_onsets);  // add to delay line and update comb filter

    for (int i = 0; i < 2; i++) {
      if (sources[i].had_onset) {
        sources[i].onset_quality = quality;
      }
    }

    if (num_onsets) {
      total_onsets++;
      float diff = sources[0].latest_onset - sources[1].latest_onset;
      float absdiff = std::abs(diff);
      if (minimumWindow < absdiff && absdiff < maximumWindow) {
        num_windowed++;
        // output both onsets
        for (int i = 0; i < 2; i++) {
          Source &s = sources[i];
          std::cout
            << i << sep
            << s.had_onset << sep
            << s.latest_onset / 1000.0 << sep
            << s.onset_quality << sep
            << ((i==1) ? -diff : diff)
            << "\n";
        }
      }
    }

    if (num_frames % every_n == 0) {
      for (unsigned i = 0; i < MaxDelay; i++) {
        uint8_t value = comb_filter.normalized_at(i) * 1024;
        image[i][num_frames / every_n] = png::rgb_pixel(value, value, value);
      }
    }

    num_frames++;
  }

done:
  image.write("combFilter.png");
  return retval;
}

int main(int argc, char const *argv[]) {
  progname = argv[0];
  initializeDetectors(argc, argv);
  uint_t num_windowed;
  uint_t total_onsets = processFiles(num_windowed);
  float percent = 100.0 * static_cast<float>(num_windowed) / total_onsets;
  std::cout << "# " << num_windowed << "/" << total_onsets
      << " (" << percent << "%) onsets in " << sources[0].detector->position_s() << " seconds\n";
}
