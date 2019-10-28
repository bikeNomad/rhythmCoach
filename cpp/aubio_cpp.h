#pragma once

extern "C" {
#include <aubio.h>
#include "utils_tests.h"
}

#include <cassert>

class AubioProcessor {
 public:
  AubioProcessor(char const *source_path, uint_t samplerate_ = 0,
                 uint_t hop_size_ = 256)
      : samplerate(samplerate_),
        win_s(1024),
        hop_size(hop_size_),
        n_frames(0) {
    source = new_aubio_source(source_path, samplerate, hop_size);
    assert(source != nullptr);
    if (!samplerate) {
      samplerate = aubio_source_get_samplerate(source);
    }
    in = new_fvec(hop_size);
    assert(in != nullptr);
  }

  ~AubioProcessor() {
    del_aubio_source(source);
    del_fvec(in);
  }

  /** answer true until the end of the input */
  bool read_frames() {
    uint_t read;
    aubio_source_do(source, in, &read);
    n_frames += read;
    return read == hop_size;
  }

 protected:
  uint_t samplerate;  // sample rate; 0 for automatic
  uint_t win_s;  // window size
  uint_t hop_size;
  aubio_source_t *source;
  uint_t n_frames;
  fvec_t *in;  // audio input buffer
};

class AubioOnsetDetector : public AubioProcessor {
  typedef AubioProcessor super;
 public:
  /** @param method one of
   * energy, hfc, complexdomain, complex, phase, wphase,
   * mkl, kl, specflux, specdiff, old_default
   */
  AubioOnsetDetector(char const *source_path, char const *method = "default",
                     uint_t samplerate_ = 0, uint_t hop_size_ = 256)
      : AubioProcessor(source_path, samplerate_, hop_size_) {
    o = new_aubio_onset(method, win_s, hop_size, samplerate);
    out = new_fvec(2);
    assert(out != nullptr);
  }

  ~AubioOnsetDetector() {
    del_aubio_onset(o);
    del_fvec(out);
  }

  /** answer true until the end of the input */
  bool process_frames() {
    bool retval = super::read_frames();
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
  uint_t last_frame() const {
    return aubio_onset_get_last(o);
  }

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

