/**
 * @file main.cpp
 */

#include "rhythm_coach.h"

static char const *progname = nullptr;

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
          "|phase|wphase|mkl|kl|specflux|specdiff)\n"
      << "\t[-o filename]   output filename for PNG (default=comb_filter[12].png)\n";
}

std::vector<source_type *> sources;

static void processCommandLine(int argc, char const *argv[]) {
  int lastarg = 1;
  for (unsigned source_num = 0; source_num < 2; source_num++) {
    source_type *source = new source_type();
    const char *default_output_name =
        (source_num == 0) ? "comb_filter1.png" : "comb_filter2.png";
    if (source_type::getNextSource(*source, lastarg, argc, argv,
                                   default_output_name)) {
      sources.push_back(source);
    } else {
      break;
    }
  }

  if (sources.size() < 2) {
    usage();
    exit(1);
  }
}

static uint_t processFiles() {
  uint_t retval(0);
  bool done(false);
  uint_t total_onsets(0);

  while (!done) {
    for (auto source : sources) {
      if (source->processNextFrame()) {
        total_onsets += source->hadOnset() ? 1 : 0;
      } else {
        retval = total_onsets;
        done = true;
      }
    }
  }

  for (auto source : sources) {
    source->writeImage();
    delete source;
  }

  return retval;
}

int main(int argc, char const *argv[]) {
  progname = argv[0];
  processCommandLine(argc, argv);
  processFiles();
  return 0;
}
