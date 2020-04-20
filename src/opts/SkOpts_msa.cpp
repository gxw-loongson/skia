/*
 * Copyright 2020 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "SkOpts.h"
#define SK_OPTS_NS msa
#include "SkBlitRow_opts.h"
#include "SkSwizzler_opts.h"

namespace SkOpts {
    void Init_msa() {
        blit_row_s32a_opaque = msa::blit_row_s32a_opaque;

        RGBA_to_BGRA          = msa::RGBA_to_BGRA;
        RGBA_to_rgbA          = msa::RGBA_to_rgbA;
        RGBA_to_bgrA          = msa::RGBA_to_bgrA;
        RGB_to_RGB1           = msa::RGB_to_RGB1;
        RGB_to_BGR1           = msa::RGB_to_BGR1;
        gray_to_RGB1          = msa::gray_to_RGB1;
        grayA_to_RGBA         = msa::grayA_to_RGBA;
        grayA_to_rgbA         = msa::grayA_to_rgbA;
        inverted_CMYK_to_RGB1 = msa::inverted_CMYK_to_RGB1;
        inverted_CMYK_to_BGR1 = msa::inverted_CMYK_to_BGR1;
    }
}
