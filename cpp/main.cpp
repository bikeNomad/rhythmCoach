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

AubioOnsetDetector *detectors[2];

/** save up to 2 seconds for rhythm analysis */
constexpr unsigned MaxDelay = { 48000 / 256 * 2 };

/** this stores the number of onsets for each frame back 2 seconds */
/** this is the comb filterbank for each frame of periodicity */
CombFilterbank<unsigned, uint8_t, MaxDelay> comb_filter;

/** call this for each frame */
static void updateCombFilter(uint8_t nOnsets) {
  comb_filter.add_item(nOnsets);  // add to delay line and update comb filter
}

static void printCombFilter(FILE *file) {
  for (unsigned i = 0; i < MaxDelay; i++) {
    fprintf(file, "%.3f ", comb_filter.normalized_at(i));
  }
  fprintf(file, "\n");
}

static void initializeDetectors(int argc, char const *argv[]) {
  int lastarg = 1;
  for (int i = 0; i < 2; i++) {
    if (!getNextOnsetDetector(detectors[i], lastarg, argc, argv)) {
      usage();
      exit(1);
    }
  }
}

static uint_t processFiles(uint_t &num_windowed, const char *combfilename) {
  uint_t total_onsets = 0;
  float recent_onsets[2] = { 0.0, 0.0 };
  num_windowed = 0;
  FILE *comb_output = fopen(combfilename, "w");

  for (;;) {
    uint_t num_onsets = 0;

    for (int i = 0; i < 2; i++) {
      if (!detectors[i]->process_samples()) {
        return total_onsets;
      }
    }

    for (int i = 0; i < 2; i++) {
      if (detectors[i]->is_onset()) {
        recent_onsets[i] = detectors[i]->last_ms();
        num_onsets++;
      }
    }

    if (num_onsets) {
      total_onsets++;
      float diff = recent_onsets[0] - recent_onsets[1];
      float absdiff = std::abs(diff);
      if (minimumWindow < absdiff && absdiff < maximumWindow) {
        num_windowed++;
        float most_recent = diff < 0.0 ? recent_onsets[1] : recent_onsets[0];
        std::cout << most_recent / 1000.0 << "\t" << diff << "\t" << num_onsets << "\n" ;
      }
    }

    updateCombFilter(num_onsets);
    printCombFilter(comb_output);
  }
}

int main(int argc, char const *argv[]) {
  progname = argv[0];
  initializeDetectors(argc, argv);
  uint_t num_windowed;
  uint_t total_onsets = processFiles(num_windowed, "combFilter.txt");
  float percent = 100.0 * static_cast<float>(num_windowed) / total_onsets;
  std::cout << "# " << num_windowed << "/" << total_onsets
      << " (" << percent << "%) onsets in " << detectors[0]->position_s() << " seconds\n";
}
