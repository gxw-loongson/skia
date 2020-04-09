/*
 * Copyright 2020 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "SkOpts.h"
#define SK_OPTS_NS msa
#include "SkBlitRow_opts.h"

namespace SkOpts {
    void Init_msa() {
        blit_row_s32a_opaque = msa::blit_row_s32a_opaque;
    }
}
