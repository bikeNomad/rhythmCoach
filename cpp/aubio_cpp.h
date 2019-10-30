#pragma once

extern "C" {
#include <aubio.h>
#include "utils_tests.h"
}
#include <iostream>

#include <cassert>

class AubioProcessor {
 public:
  AubioProcessor(char const *source_path, uint_t samplerate = 0,
                 uint_t hop_size = 256)
      : samplerate_(samplerate),
        win_s_(1024),
        hop_size_(hop_size),
        n_samples_(0) {
    source = new_aubio_source(source_path, samplerate_, hop_size_);
    if (!source) {
      std::cerr << "Can't open " << source_path << "\n";
    }
    if (!samplerate_) {
      samplerate_ = aubio_source_get_samplerate(source);
    }
    std::cerr << "Sample rate: " << samplerate_ << "\n";
    in = new_fvec(hop_size_);
    assert(in != nullptr);
  }

  ~AubioProcessor() {
    del_aubio_source(source);
    del_fvec(in);
  }

  /** answer true until the end of the input */
  bool read_samples() {
    uint_t read;
    aubio_source_do(source, in, &read);
    n_samples_ += read;
    return read == hop_size_;
  }

 protected:
  uint_t samplerate_;  // sample rate; 0 for automatic
  uint_t win_s_;  // window size
  uint_t hop_size_;
  aubio_source_t *source;
  uint_t n_samples_;
  fvec_t *in;  // audio input buffer
};

class AubioOnsetDetector : public AubioProcessor {
  typedef AubioProcessor super;
 public:
  /** @param method one of
   * energy, hfc, complexdomain, complex, phase, wphase,
   * mkl, kl, specflux, specdiff, old_default
   */
  AubioOnsetDetector(char const *source_path,
                     uint_t samplerate = 0,
                     uint_t hop_size = 256,
                     char const *method = "default")
      : AubioProcessor(source_path, samplerate, hop_size) {
    o = new_aubio_onset(method, win_s_, hop_size_, samplerate_);
    out = new_fvec(2);
    assert(out != nullptr);
  }

  ~AubioOnsetDetector() {
    del_aubio_onset(o);
    del_fvec(out);
  }

  /** answer true until the end of the input */
  bool process_samples() {
    bool retval = super::read_samples();
    aubio_onset_do(o, in, out);
    return retval;
  }

  bool is_onset() const {
    return out->data[0] != 0;
  }

  smpl_t last_ms() const {
    return aubio_onset_get_last_ms(o);
  }
  smpl_t last_s() const {
    return aubio_onset_get_last_s(o);
  }

  /** answer the location of the last onset in samples */
  uint_t last_sample() const {
    return aubio_onset_get_last(o);
  }

  /** answer the current delay in samples */
  uint_t delay() const { return aubio_onset_get_delay(o); }
 
  void set_threshold(smpl_t threshold) {
    aubio_onset_set_threshold(o, threshold);
  }
  void set_delay(smpl_t delay) {
    aubio_onset_set_delay(o, delay);
  }
  void set_minioi_ms(smpl_t minioi_ms) {
    aubio_onset_set_minioi_ms(o, minioi_ms);
  }
  void set_silence(smpl_t silence) {
    aubio_onset_set_silence(o, silence);
  }
  void set_awhitening(bool enable) {
    aubio_onset_set_awhitening(o, enable);
  }

 protected:
  aubio_onset_t *o;
  fvec_t *out;  // output position
};

