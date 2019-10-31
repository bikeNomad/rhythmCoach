/**
 * @file main.cpp
 */

#include "aubio_cpp.h"
#include "delay_line.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>

struct DetectorSettings {
  uint_t hop_size;    // -h
  smpl_t minioi_ms;   // -i
  smpl_t silence;     // -s
  smpl_t threshold;   // -t
  char const *method;  // -m
  char const *source_path;
};

static char const *progname = nullptr;

static void usage() {
  std::cerr
      << "usage:\n"
      << progname
      << "[file1options] file1 [file2options] file2\noptions:\n"
      << "\t[-h hopsize]    hop size (default=256)\n"
      << "\t[-m method]     detection method (default|hfc|energy|complex|complexdomain|phase|wphase|mkl|kl|specflux|specdiff)\n"
      << "\t[-i minioi_ms]  minimum inter-onset interval in ms (default=12)\n"
      << "\t[-s silence]    silence threshold in dB (default=-90)\n"
      << "\t[-t threshold]  detection threshold (0-1) (default=0.3)\n";
}

// 21.3 ms minioi = 4 frames of 256 samples at 48ksps
static float constexpr defaultMinIOI = 21.3;

DetectorSettings constexpr defaultSettings = { 256, defaultMinIOI, -90.0, 0.3, "default", nullptr };

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
          << " -h " << settings.hop_size
          << " -i " << settings.minioi_ms
          << " -s " << settings.silence
          << " -t " << settings.threshold
          << " -m " << settings.method
          << " " << settings.source_path << "\n";
}

static bool getNextOnsetDetector(AubioOnsetDetector *&dest, int &lastarg, int argc,
                          char const *argv[]) {
  int i;
  DetectorSettings settings = defaultSettings;
  for (i = lastarg; i < argc; i++) {
    if (!strcmp("-h", argv[i])) {
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
  printSettings(settings);
  return true;
}

int main(int argc, char const *argv[]) {
  progname = argv[0];
  AubioOnsetDetector *detectors[2];

  int lastarg = 1;
  for (int i = 0; i < 2; i++) {
    if (!getNextOnsetDetector(detectors[i], lastarg, argc, argv)) {
      usage();
      exit(1);
    }
  }

  while (detectors[0]->process_samples() && detectors[1]->process_samples()) {
    if (detectors[0]->is_onset()) {
      printf("1 %.6f\n", detectors[0]->last_s());
    }
    if (detectors[1]->is_onset()) {
      printf("2 %.6f\n", detectors[1]->last_s());
    }
  }
}
