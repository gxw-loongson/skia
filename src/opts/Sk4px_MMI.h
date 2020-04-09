/*
 * Copyright 2015 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

namespace { // NOLINT(google-build-namespaces)

#include "loongson-mmintrin.h"

inline Sk4px::Wide Sk4px::widen() const {
    return Sk16h(_mm_unpacklo_epi8(this->fVec, _mm_setzero_si128()),
                 _mm_unpackhi_epi8(this->fVec, _mm_setzero_si128()));
}

inline Sk4px::Wide Sk4px::mulWiden(const Sk16b& other) const {
    return this->widen() * Sk4px(other).widen();
}

inline Sk4px Sk4px::Wide::addNarrowHi(const Sk16h& other) const {
    Sk4px::Wide r = (*this + other) >> 8;
    return Sk4px(_mm_packus_epi16(r.fLo.fVec, r.fHi.fVec));
}

inline Sk4px Sk4px::Wide::div255() const {
    // (x + 127) / 255 == ((x+128) * 257)>>16,
    // and _mm_mulhi_epu16 makes the (_ * 257)>>16 part very convenient.
    const __m128i _128 = _mm_set1_epi16(128),
                  _257 = _mm_set1_epi16(257);
    return Sk4px(_mm_packus_epi16(_mm_mulhi_epu16(_mm_add_epi16(fLo.fVec, _128), _257),
                                  _mm_mulhi_epu16(_mm_add_epi16(fHi.fVec, _128), _257)));
}

// Load4Alphas and Load2Alphas use possibly-unaligned loads (SkAlpha[] -> uint16_t or uint32_t).
// These are safe on x86, often with no speed penalty.

inline Sk4px Sk4px::alphas() const {
    static_assert(SK_A32_SHIFT == 24, "Intel's always little-endian.");
    // We exploit that A >= rgb for any premul pixel.
    __m128i as = fVec;                             // 3xxx 2xxx 1xxx 0xxx
    as = _mm_max_epu8(as, _mm_srli_epi32(as,  8)); // 33xx 22xx 11xx 00xx
    as = _mm_max_epu8(as, _mm_srli_epi32(as, 16)); // 3333 2222 1111 0000
    return Sk16b(as);
}

inline Sk4px Sk4px::Load4Alphas(const SkAlpha a[4]) {
    __m128i as = _mm_loadu_cvtsi32_si128((const int32_t*)a);  // ____ ____ ____ 3210
    as = _mm_unpacklo_epi8 (as, as);                          // ____ ____ 3322 1100
    as = _mm_unpacklo_epi16(as, as);                          // 3333 2222 1111 0000
    return Sk16b(as);
}

inline Sk4px Sk4px::Load2Alphas(const SkAlpha a[2]) {
    __m128i as = _mm_loadu_cvtsi16_si128((const int16_t*)a);  // ____ ____ ____ 0010
    as = _mm_unpacklo_epi8 (as, as);                          // ____ ____ 0000 1100
    as = _mm_unpacklo_epi16(as, as);                          // 0000 0000 1111 0000
    return Sk16b(as);
}

}  // namespace
