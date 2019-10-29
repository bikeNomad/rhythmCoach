/**
 * @file main.cpp
 */

#include "aubio_cpp.h"
#include "delay_line.h"
#include <iostream>
#include <cstdio>

int main(int argc, char const *argv[]) {
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << " filename\n";
    exit(1);
  }
  AubioOnsetDetector onsetDetector(argv[1]);
  while (onsetDetector.process_frames()) {
    if (onsetDetector.is_onset()) {
      printf("%.6f\n", onsetDetector.last_s());
    }
  }
}
