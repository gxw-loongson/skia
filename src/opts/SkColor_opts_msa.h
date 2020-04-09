/*
 * Copyright 2014 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkColor_opts_msa_DEFINED
#define SkColor_opts_msa_DEFINED

#include <msa.h>

//#define ASSERT_EQ(a,b) SkASSERT(0xffff == _mm_movemask_epi8(_mm_cmpeq_epi8((a), (b))))//Comment this Macro for MSA Optimize

static inline v4i32 SkAlpha255To256_MSA(const v4i32& alpha) {
    v4i32 set1 = {1, 1, 1, 1};
    return __msa_addv_w((v4i32)alpha, set1);
}

// See #define SkAlphaMulAlpha(a, b)  SkMulDiv255Round(a, b) in SkXfermode.cpp.
static inline v8i16 SkAlphaMulAlpha_MSA(const v8i16& a,
                                           const v8i16& b) {
    v4i32 __128 = {128, 128, 128, 128};
    v8i16 prod = __msa_mulv_h(a, b);
    prod = (v8i16)__msa_addv_w((v4i32)prod, __128);
    prod = (v8i16)__msa_addv_w((v4i32)prod, __msa_srli_w((v4i32)prod, 8));
    prod = (v8i16)__msa_srli_w((v4i32)prod, 8);
    return prod;
}

// Portable version SkAlphaMulQ is in SkColorData.h.
static inline v16u8 SkAlphaMulQ_MSA(const v16u8& c, const v4i32& scale) {
    const v4u32 mask = {0xFF00FF, 0xFF00FF, 0xFF00FF, 0xFF00FF};
    const v4u32 mask_not = {0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00};
    v8i16 s = (v8i16)__msa_or_v((v16u8)__msa_slli_w(scale, 16), (v16u8)scale);

    // uint32_t rb = ((c & mask) * scale) >> 8
    v4u32 rb = (v4u32)__msa_and_v((v16u8)mask, (v16u8)c);
    rb = (v4u32)__msa_mulv_h((v8i16)rb, (v8i16)s);
    rb = (v4u32)__msa_srli_h((v8i16)rb, 8);

    // uint32_t ag = ((c >> 8) & mask) * scale
    v4u32 ag = (v4u32)__msa_srli_h((v8i16)c, 8);
    ag = (v4u32)__msa_and_v((v16u8)mask, (v16u8)ag);
    ag = (v4u32)__msa_mulv_h((v8i16)ag, (v8i16)s);

    // (rb & mask) | (ag & ~mask)
    rb = (v4u32)__msa_and_v((v16u8)mask, (v16u8)rb);
    ag = (v4u32)__msa_and_v((v16u8)mask_not, (v16u8)ag);
    return __msa_or_v((v16u8)rb, (v16u8)ag);
}

// Fast path for SkAlphaMulQ_SSE2 with a constant scale factor.
static inline v16u8 SkAlphaMulQ_MSA(const v16u8& c, const unsigned scale) {
    const v4u32 mask = {0xFF00FF, 0xFF00FF, 0xFF00FF, 0xFF00FF};
    const v4u32 mask_not = {0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00};
    v16u8 s = (v16u8)__msa_fill_h(scale << 8); // Move scale factor to upper byte of word.

    // With mulhi, red and blue values are already in the right place and
    // don't need to be divided by 256.
    v16u8 rb = __msa_and_v((v16u8)mask, (v16u8)c);
    rb = (v16u8)__msa_mul_q_h((v8i16)rb, (v8i16)s);
    rb = (v16u8)__msa_srli_h((v8i16)rb,1);

    v16u8 ag = __msa_and_v((v16u8)mask_not, (v16u8)c);
    ag = (v16u8)__msa_mul_q_h((v8i16)ag, (v8i16)s);     // Alpha and green values are in the higher byte of each word.
    ag = (v16u8)__msa_srli_h((v8i16)ag,1);
    ag = (v16u8)__msa_and_v((v16u8)mask_not, (v16u8)ag);

    return __msa_or_v((v16u8)rb, (v16u8)ag);
}

// Portable version SkFastFourByteInterp256 is in SkColorData.h.
static inline v16i8 SkFastFourByteInterp256_MSA(const v16i8& src, const v16i8& dst, const unsigned src_scale) {
    const v4u32 mask = {0x00FF00FF, 0x00FF00FF, 0x00FF00FF, 0x00FF00FF};
    const v4u32 mask_not = {0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00};
    v16u8 src_rb = __msa_and_v((v16u8)mask, (v16u8)src);
    v8i16 src_ag = __msa_srli_h((v8i16)src, 8);
    v16u8 dst_rb = __msa_and_v((v16u8)mask, (v16u8)dst);
    v8i16 dst_ag = __msa_srli_h((v8i16)dst, 8);

    // Compute scaled differences.
    v8i16 diff_rb = __msa_subv_h((v8i16)src_rb, (v8i16)dst_rb);
    v8i16 diff_ag = __msa_subv_h((v8i16)src_ag, (v8i16)dst_ag);
    v8i16 s = __msa_fill_h(src_scale);
    diff_rb = __msa_mulv_h((v8i16)diff_rb, (v8i16)s);
    diff_ag = __msa_mulv_h((v8i16)diff_ag, (v8i16)s);

    // Pack the differences back together.
    diff_rb = __msa_srli_h((v8i16)diff_rb, 8);
    diff_ag = (v8i16)__msa_and_v((v16u8)mask_not, (v16u8)diff_ag);
    v16u8 diff = __msa_or_v((v16u8)diff_rb, (v16u8)diff_ag);

    // Add difference to destination.
    return __msa_addv_b((v16i8)dst, (v16i8)diff);
}

// Portable version SkPMLerp is in SkColorData.h
static inline v16i8 SkPMLerp_MSA(const v16i8& src, const v16i8& dst, const unsigned scale) {
    return SkFastFourByteInterp256_MSA((v16i8)src, (v16i8)dst, scale);
}

static inline v4i32 SkGetPackedA32_MSA(const v4i32& src) {
#if SK_A32_SHIFT == 24                // It's very common (universal?) that alpha is the top byte.
    return __msa_srli_w((v4i32)src, 24);   // You'd hope the compiler would remove the left shift then,
#else                                 // but I've seen Clang just do a dumb left shift of zero. :(
    v4i32 a = __msa_slli_w((v4i32)src, (24 - SK_A32_SHIFT));
    return __msa_srli_w((v4i32)a, 24);
#endif
}

static inline v4i32 SkGetPackedR32_MSA(const v4i32& src) {
    v4i32 r = __msa_slli_w(src, (24 - SK_R32_SHIFT));
    return __msa_srli_w(r, 24);
}

static inline v4i32 SkGetPackedG32_MSA(const v4i32& src) {
    v4i32 g = __msa_slli_w(src, (24 - SK_G32_SHIFT));
    return __msa_srli_w(g, 24);
}

static inline v4i32 SkGetPackedB32_MSA(const v4i32& src) {
    v4i32 b = __msa_slli_w(src, (24 - SK_B32_SHIFT));
    return __msa_srli_w(b, 24);
}

static inline v8i16 SkMul16ShiftRound_MSA(const v8i16& a,
                                             const v8i16& b, int shift) {
    v8i16 prod = __msa_mulv_h((v8i16)a, (v8i16)b);
    v8i16 shift_vec = __msa_fill_h(shift);
    prod = __msa_addv_h(prod, __msa_fill_h(1 << (shift - 1)));
    prod = __msa_addv_h(prod, __msa_srl_h(prod, shift_vec));
    prod = __msa_srl_h(prod, shift_vec);
    return prod;
}

static inline v16u8 SkPackRGB16_MSA(const v8i16& r,
                                       const v8i16& g, const v8i16& b) {
    v8i16 dr = __msa_slli_h(r, SK_R16_SHIFT);
    v8i16 dg = __msa_slli_h(g, SK_G16_SHIFT);
    v8i16 db = __msa_slli_h(b, SK_B16_SHIFT);
    v16u8 c = __msa_or_v((v16u8)dr, (v16u8)dg);
    return    __msa_or_v((v16u8)c,  (v16u8)db);
}

static inline v16u8 SkPackARGB32_MSA(const v4i32& a, const v4i32& r,
                                        const v4i32& g, const v4i32& b) {
    v4i32 da = __msa_slli_w(a, SK_A32_SHIFT);
    v4i32 dr = __msa_slli_w(r, SK_R32_SHIFT);
    v4i32 dg = __msa_slli_w(g, SK_G32_SHIFT);
    v4i32 db = __msa_slli_w(b, SK_B32_SHIFT);
    v16u8 c = __msa_or_v((v16u8)da, (v16u8)dr);
    c = __msa_or_v((v16u8)c, (v16u8)dg);
    return __msa_or_v((v16u8)c, (v16u8)db);
}

static inline v4i32 SkPacked16ToR32_MSA(const v4i32& src) {
    v4i32 r = __msa_srli_w(src, SK_R16_SHIFT);
    r = (v4i32)__msa_and_v((v16u8)r, (v16u8)__msa_fill_w(SK_R16_MASK));
    r = (v4i32)__msa_or_v((v16u8)__msa_slli_w((v4i32)r, (8 - SK_R16_BITS)),
                     (v16u8)__msa_srli_w((v4i32)r, (2 * SK_R16_BITS - 8)));
    return r;
}

static inline v4i32 SkPacked16ToG32_MSA(const v4i32& src) {
    v4i32 g = __msa_srli_w(src, SK_G16_SHIFT);
    g = (v4i32)__msa_and_v((v16u8)g, (v16u8)__msa_fill_w(SK_G16_MASK));
    g = (v4i32)__msa_or_v((v16u8)__msa_slli_w((v4i32)g, (8 - SK_G16_BITS)),
                     (v16u8)__msa_srli_w((v4i32)g, (2 * SK_G16_BITS - 8)));
    return g;
}

static inline v4i32 SkPacked16ToB32_MSA(const v4i32& src) {
    v4i32 b = __msa_srli_w(src, SK_B16_SHIFT);
    b = (v4i32)__msa_and_v((v16u8)b, (v16u8)__msa_fill_w(SK_B16_MASK));
    b = (v4i32)__msa_or_v((v16u8)__msa_slli_w(b, (8 - SK_B16_BITS)),
                     (v16u8)__msa_srli_w(b, (2 * SK_B16_BITS - 8)));
    return b;
}

static inline v16u8 SkPixel16ToPixel32_MSA(const v4i32& src) {
    v4i32 r = SkPacked16ToR32_MSA(src);
    v4i32 g = SkPacked16ToG32_MSA(src);
    v4i32 b = SkPacked16ToB32_MSA(src);
    return SkPackARGB32_MSA(__msa_fill_w(0xFF), r, g, b);
}

static inline v8u16 packsMsa(v4u32 a, v4u32 b){
    v8i16 shuffle_vec = {0,2,4,6,8,10,12,14};
    v4u32 temp_a = __msa_sat_u_w(a, 15);
    v4u32 temp_b = __msa_sat_u_w(b, 15);
    temp_a = (v4u32)__msa_vshf_h(shuffle_vec, (v8i16)temp_a, (v8i16)temp_a);
    temp_b = (v4u32)__msa_vshf_h(shuffle_vec, (v8i16)temp_b, (v8i16)temp_b);
    v8u16 value = (v8u16)__msa_ilvr_d((v2i64)temp_b, (v2i64)temp_a);
    return value;
}
static inline v16u8 SkPixel32ToPixel16_ToU16_MSA(const v4i32& src_pixel1,
                                                    const v4i32& src_pixel2) {
    // Calculate result r.
    v4i32 r1 = __msa_srli_w(src_pixel1,
                            SK_R32_SHIFT + (8 - SK_R16_BITS));
    r1 = (v4i32)__msa_and_v((v16u8)r1, (v16u8)__msa_fill_w(SK_R16_MASK));
    v4i32 r2 = __msa_srli_w(src_pixel2,
                            SK_R32_SHIFT + (8 - SK_R16_BITS));
    r2 = (v4i32)__msa_and_v((v16u8)r2, (v16u8)__msa_fill_w(SK_R16_MASK));
    v8u16 r = packsMsa((v4u32)r1, (v4u32)r2);

    // Calculate result g.
    v4i32 g1 = __msa_srli_w(src_pixel1,
                            SK_G32_SHIFT + (8 - SK_G16_BITS));
    g1 = (v4i32)__msa_and_v((v16u8)g1, (v16u8)__msa_fill_w(SK_G16_MASK));
    v4i32 g2 = __msa_srli_w(src_pixel2,
                            SK_G32_SHIFT + (8 - SK_G16_BITS));
    g2 = (v4i32)__msa_and_v((v16u8)g2, (v16u8)__msa_fill_w(SK_G16_MASK));
    v8u16 g = packsMsa((v4u32)g1, (v4u32)g2);

    // Calculate result b.
    v4i32 b1 = __msa_srli_w(src_pixel1,
                            SK_B32_SHIFT + (8 - SK_B16_BITS));
    b1 = (v4i32)__msa_and_v((v16u8)b1, (v16u8)__msa_fill_w(SK_B16_MASK));
    v4i32 b2 = __msa_srli_w(src_pixel2,
                            SK_B32_SHIFT + (8 - SK_B16_BITS));
    b2 = (v4i32)__msa_and_v((v16u8)b2, (v16u8)__msa_fill_w(SK_B16_MASK));
    v8u16 b = packsMsa((v4u32)b1, (v4u32)b2);

    // Store 8 16-bit colors in dst.
    v16u8 d_pixel = SkPackRGB16_MSA((v8i16)r, (v8i16)g, (v8i16)b);

    return d_pixel;
}

// Portable version is SkPMSrcOver in SkColorData.h.
static inline v4i32 SkPMSrcOver_MSA(const v4i32& src, const v16u8& dst) {
    return __msa_addv_w((v4i32)src,
                         (v4i32)SkAlphaMulQ_MSA((v16u8)dst, (v4i32)__msa_subv_w(__msa_fill_w(256),
                                                             SkGetPackedA32_MSA(src))));
}

// Fast path for SkBlendARGB32_SSE2 with a constant alpha factor.
static inline v16u8 SkBlendARGB32_MSA(const v4i32& src, const v4i32& dst,
                                         const unsigned aa) {
    unsigned alpha = SkAlpha255To256(aa);
    v8i16 src_scale = __msa_fill_h(alpha);
    // SkAlphaMulInv256(SkGetPackedA32(src), src_scale)
    v4i32 dst_scale = SkGetPackedA32_MSA(src);
    // High words in dst_scale are 0, so it's safe to multiply with 16-bit src_scale.
    dst_scale = (v4i32)__msa_mulv_h((v8i16)dst_scale, (v8i16)src_scale);
    dst_scale = (v4i32)__msa_subv_w(__msa_fill_w(0xFFFF), dst_scale);
    dst_scale = (v4i32)__msa_addv_w(dst_scale, __msa_srli_w(dst_scale, 8));
    dst_scale = (v4i32)__msa_srli_w(dst_scale, 8);
    // Duplicate scales into 2x16-bit pattern per pixel.
    dst_scale = (v4i32)__msa_shf_h((v8i16)dst_scale, 0xa0);

    const v4u32 mask     = {0x00FF00FF, 0x00FF00FF, 0x00FF00FF, 0x00FF00FF};
    const v4u32 mask_not = {0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00};

    // Unpack the 16x8-bit source/destination into 2 8x16-bit splayed halves.
    v16u8 src_rb = __msa_and_v((v16u8)mask, (v16u8)src);
    v8i16 src_ag = __msa_srli_h((v8i16)src, 8);
    v16u8 dst_rb = __msa_and_v((v16u8)mask, (v16u8)dst);
    v8i16 dst_ag = __msa_srli_h((v8i16)dst, 8);

    // Scale them.
    src_rb = (v16u8)__msa_mulv_h((v8i16)src_rb, (v8i16)src_scale);
    src_ag = (v8i16)__msa_mulv_h((v8i16)src_ag, (v8i16)src_scale);
    dst_rb = (v16u8)__msa_mulv_h((v8i16)dst_rb, (v8i16)dst_scale);
    dst_ag = (v8i16)__msa_mulv_h((v8i16)dst_ag, (v8i16)dst_scale);

    // Add the scaled source and destination.
    dst_rb = (v16u8)__msa_addv_h((v8i16)src_rb, (v8i16)dst_rb);
    dst_ag = (v8i16)__msa_addv_h((v8i16)src_ag, (v8i16)dst_ag);

    // Unsplay the halves back together.
    dst_rb = (v16u8)__msa_srli_h((v8i16)dst_rb, 8);
    dst_ag = (v8i16)__msa_and_v((v16u8)mask_not, (v16u8)dst_ag);
    return __msa_or_v((v16u8)dst_rb, (v16u8)dst_ag);
}

//#undef ASSERT_EQ
#endif // SkColor_opts_MSA_DEFINED
