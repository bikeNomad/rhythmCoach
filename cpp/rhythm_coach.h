/**
 * @file rhythm_coach.h
 */

#pragma once

#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <png++/png.hpp>
#include <vector>

#include "aubio_cpp.h"
#include "comb_filter.h"

// 21.3 ms minioi = 4 frames of 256 samples at 48ksps
constexpr float defaultMinIOI = 21.3;
constexpr unsigned MaxImageWidth = 1024U;

/** save up to 3 seconds for rhythm analysis */
constexpr unsigned MaxDelay = {48000 / 256 * 3};

typedef png::image<png::rgb_pixel> image_type;

#include "source.h"
typedef Source<MaxDelay, MaxImageWidth> source_type;

template class Source<MaxDelay, MaxImageWidth>;
