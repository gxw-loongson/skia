/*
 ============================================================================
 Name        : loongson-mmintrin.h
 Author      : Heiher <r@hev.cc>
 Version     : 0.0.1
 Copyright   : Copyright (c) 2017 everyone.
 Description : The helpers for x86 SSE to Loongson MMI.
 ============================================================================
 */

#ifndef __LOONGSON_MMINTRIN_H__
#define __LOONGSON_MMINTRIN_H__

#include <stdint.h>

/* Note:
 *   From llvm-clang-8.0, llvm-clang supporting loongson's specific instructions
 *   by Loongson's ISA-spec.
 *   But for history gcc supporting, here using the macro define.
 */
#ifdef __clang__
#define  PXOR       "pxor"
#define  PAND       "pand"
#define  POR        "por"
#define  PNOR       "pnor"
#define  SADDW       "saddw"
#define  SADDUW      "sadduw"
#define  SADDD      "saddd"
#define  SSUBW       "ssubw"
#define  SSUBUW      "ssubuw"
#define  SSUBD      "ssubd"
#define  SSLLW       "ssllw"
#define  SSLLD      "sslld"
#define  SSRAD      "ssrad"
#define  SSRLD      "ssrld"
#define  SSRAW       "ssraw"
#define  SSRLW       "ssrlw"
#define  SSEQD       "sseqd"
#define  SSEQUD      "ssequd"
#define  SSLED       "ssled"
#define  SSLEUD      "ssleud"
#define  SSLTD       "ssltd"
#define  SSLTUD      "ssltud"
#else
//gnuc
#define  PXOR       "xor"
#define  PAND       "and"
#define  POR        "or"
#define  PNOR       "nor"
#define  SADDW       "add"
#define  SADDUW      "addu"
#define  SADDD      "dadd"
#define  SSUBW       "sub"
#define  SSUBUW      "subu"
#define  SSUBD      "dsub"
#define  SSLLW       "sll"
#define  SSLLD      "dsll"
#define  SSRAD      "dsra"
#define  SSRLD      "dsrl"
#define  SSRAW       "sra"
#define  SSRLW       "srl"
#define  SSEQD       "seq"
#define  SSEQUD      "sequ"
#define  SSLED       "sle"
#define  SSLEUD      "sleu"
#define  SSLTD       "slt"
#define  SSLTUD      "sltu"
#endif

typedef long long __m64i     __attribute__ ((__vector_size__ (8),  __may_alias__));
typedef long long __m128i    __attribute__ ((__vector_size__ (16), __may_alias__));

typedef union {
  __m128i mv;
  double  dv[2];
} u__m128i;

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_set1_epi8(int8_t __A)
{
    u__m128i v;
    double z;

    __asm__ volatile (
        "ins    %[v], %[v], 8, 8 \n\t"
        PXOR  "    %[z], %[z], %[z] \n\t"
        "mtc1   %[v], %[l]       \n\t"
        "pshufh %[l], %[l], %[z] \n\t"
        "mov.d  %[h], %[l]       \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1]), [z]"=&f"(z), [v]"+&r"(__A)
    );

    return v.mv;
}
static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_set1_epi16(int16_t __A)
{
    u__m128i v;
    double z;

    __asm__ volatile (
        PXOR  "    %[z], %[z], %[z] \n\t"
        "mtc1   %[v], %[l]       \n\t"
        "pshufh %[l], %[l], %[z] \n\t"
        "mov.d  %[h], %[l]       \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1]), [z]"=&f"(z)
        : [v]"r"(__A)
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_set1_epi32(int32_t __A)
{
    u__m128i v;

    __asm__ volatile (
        "mtc1      %[v], %[l]       \n\t"
        "punpcklwd %[l], %[l], %[l] \n\t"
        "mov.d     %[h], %[l]       \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [v]"r"(__A)
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_setr_epi8(int8_t __A, int8_t __B, int8_t __C, int8_t __D, int8_t __E, int8_t __F, int8_t __G,
              int8_t __H, int8_t __I, int8_t __J, int8_t __K, int8_t __L, int8_t __M, int8_t __N,
              int8_t __O, int8_t __P)
{
    u__m128i v;

    __asm__ volatile (
        "dins  %[a], %[b], 8,  8 \n\t"
        "dins  %[a], %[c], 16, 8 \n\t"
        "dins  %[a], %[d], 24, 8 \n\t"
        "dins  %[a], %[e], 32, 8 \n\t"
        "dins  %[a], %[f], 40, 8 \n\t"
        "dins  %[a], %[g], 48, 8 \n\t"
        "dins  %[a], %[h], 56, 8 \n\t"
        "dins  %[i], %[j], 8, 8  \n\t"
        "dins  %[i], %[k], 16, 8 \n\t"
        "dins  %[i], %[l], 24, 8 \n\t"
        "dins  %[i], %[m], 32, 8 \n\t"
        "dins  %[i], %[n], 40, 8 \n\t"
        "dins  %[i], %[o], 48, 8 \n\t"
        "dins  %[i], %[p], 56, 8 \n\t"
        "dmtc1 %[a], %[vl]       \n\t"
        "dmtc1 %[i], %[vh]       \n\t"
        : [vl]"=&f"(v.dv[0]), [vh]"=&f"(v.dv[1]), [a]"+&r"(__A), [i]"+&r"(__I)
        : [b]"r"(__B), [c]"r"(__C), [d]"r"(__D), [e]"r"(__E), [f]"r"(__F), [g]"r"(__G),
          [h]"r"(__H), [j]"r"(__J), [k]"r"(__K), [l]"r"(__L), [m]"r"(__M), [n]"r"(__N),
          [o]"r"(__O), [p]"r"(__P)
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_set_epi8(int8_t __A, int8_t __B, int8_t __C, int8_t __D, int8_t __E, int8_t __F, int8_t __G,
             int8_t __H, int8_t __I, int8_t __J, int8_t __K, int8_t __L, int8_t __M, int8_t __N,
             int8_t __O, int8_t __P)
{
    return _mm_setr_epi8(__P, __O, __N, __M, __L, __K, __J, __I,
                         __H, __G, __F, __E, __D, __C, __B, __A);
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_setr_epi16(int16_t __A, int16_t __B, int16_t __C, int16_t __D, int16_t __E, int16_t __F,
               int16_t __G, int16_t __H)
{
    u__m128i v;

    __asm__ volatile (
            "dins  %[a], %[b], 16, 16 \n\t"
            "dins  %[a], %[c], 32, 16 \n\t"
            "dins  %[a], %[d], 48, 16 \n\t"
            "dins  %[e], %[f], 16, 16 \n\t"
            "dins  %[e], %[g], 32, 16 \n\t"
            "dins  %[e], %[h], 48, 16 \n\t"
            "dmtc1 %[a], %[vl]        \n\t"
            "dmtc1 %[e], %[vh]        \n\t"
            : [vl]"=&f"(v.dv[0]), [vh]"=&f"(v.dv[1]), [a]"+&r"(__A), [e]"+&r"(__E)
            : [b]"r"(__B), [c]"r"(__C), [d]"r"(__D), [f]"r"(__F), [g]"r"(__G), [h]"r"(__H)
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_set_epi16(int16_t __A, int16_t __B, int16_t __C, int16_t __D,
              int16_t __E, int16_t __F, int16_t __G, int16_t __H)
{
    return _mm_setr_epi16(__H, __G, __F, __E, __D, __C, __B, __A);
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_setr_epi32(int32_t __A, int32_t __B, int32_t __C, int32_t __D)
{
    u__m128i v;

    __asm__ volatile (
        "mtc1  %[a], %[l] \n\t"
        "mthc1 %[b], %[l] \n\t"
        "mtc1  %[c], %[h] \n\t"
        "mthc1 %[d], %[h] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [a]"r"(__A), [b]"r"(__B), [c]"r"(__C), [d]"r"(__D)
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_set_epi32(int32_t __A, int32_t __B, int32_t __C, int32_t __D)
{
    return _mm_setr_epi32(__D, __C, __B, __A);
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_loadu_cvtsi16_si128(int16_t const *__P)
{
    u__m128i v;
    int32_t x;

    __asm__ volatile (
        "ulhu %[x], %[p]       \n\t"
        "mtc1 %[x], %[l]       \n\t"
        PXOR  "  %[h], %[h], %[h] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1]), [x]"=&r"(x)
        : [p]"m"(*__P)
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_loadu_cvtsi32_si128(int32_t const *__P)
{
    u__m128i v;

    __asm__ volatile (
        "gslwlc1 %[l], 0x3(%[p])  \n\t"
        "gslwrc1 %[l], 0x0(%[p])  \n\t"
        PXOR  "     %[h], %[h], %[h] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [p]"r"(__P)
        : "memory"
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtsi32_si128(int32_t __A)
{
    u__m128i v;

    __asm__ volatile (
        "mtc1 %[a], %[l]       \n\t"
        PXOR  "  %[h], %[h], %[h] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [a]"r"(__A)
    );

    return v.mv;
}

static inline int32_t __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtsi128_si32(__m128i __A)
{
    int32_t v;//qiao
    u__m128i A;
    A.mv[0] = __A[0];
    A.mv[1] = __A[1];

    __asm__ volatile (
        "mfc1 %[v], %[l] \n\t"
        : [v]"=&r"(v)
        : [l]"f"(A.dv[0])
    );

    return v;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_setzero_si128(void)
{
    u__m128i v;

    __asm__ volatile (
        PXOR  " %[l], %[l], %[l] \n\t"
        PXOR  " %[h], %[h], %[h] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_loadl_epi64(__m128i const *__P)
{
    u__m128i v;

    __asm__ volatile (
        "gsldlc1 %[l], 0x7(%[p])  \n\t"
        "gsldrc1 %[l], 0x0(%[p])  \n\t"
        PXOR  "     %[h], %[h], %[h] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [p]"r"(__P)
        : "memory"
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_load_si128(__m128i const *__P)
{
    u__m128i v;

    __asm__ volatile (
        "gslqc1 %[h], %[l], (%[p]) \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [p]"r"(__P)
        : "memory"
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_loadu_si128(__m128i const *__P)
{
    u__m128i v;

    __asm__ volatile (
        "gsldlc1 %[l], 0x7(%[p]) \n\t"
        "gsldrc1 %[l], 0x0(%[p]) \n\t"
        "gsldlc1 %[h], 0xf(%[p]) \n\t"
        "gsldrc1 %[h], 0x8(%[p]) \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [p]"r"(__P)
        : "memory"
    );

    return v.mv;
}

static inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_storel_epi64(__m128i *__P, __m128i __A)
{
    __asm__ volatile (
        "sdl %[l], 0x7(%[p]) \n\t"
        "sdr %[l], 0x0(%[p]) \n\t"
        :: [l]"r"(__A[0]), [p]"r"(__P)
        : "memory"
    );
}

static inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_store_si128(__m128i *__P, __m128i __A)
{
    u__m128i v;
    v.mv[0] = __A[0];
    v.mv[1] = __A[1];

    __asm__ volatile (
        "gssqc1 %[h], %[l], (%[p]) \n\t"
        :: [l]"f"(v.dv[0]), [h]"f"(v.dv[1]), [p]"r"(__P)
        : "memory"
    );
}

static inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_storeu_si128(__m128i* __P, __m128i __A)
{
    u__m128i v;
    v.mv[0] = __A[0];
    v.mv[1] = __A[1];

    __asm__ volatile (
        "gssdlc1 %[l], 0x7(%[p]) \n\t"
        "gssdrc1 %[l], 0x0(%[p]) \n\t"
        "gssdlc1 %[h], 0xf(%[p]) \n\t"
        "gssdrc1 %[h], 0x8(%[p]) \n\t"
        :: [l]"f"(v.dv[0]), [h]"f"(v.dv[1]), [p]"r"(__P)
        : "memory"
    );
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_add_epi8(__m128i __A, __m128i __B)
{
    u__m128i v;
    u__m128i A;
    u__m128i B;
    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    B.mv[0] = __B[0];
    B.mv[1] = __B[1];

    __asm__ volatile (
        "paddb %[l], %[al], %[bl] \n\t"
        "paddb %[h], %[ah], %[bh] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1]), [bl]"f"(B.dv[0]), [bh]"f"(B.dv[1])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_adds_epu8(__m128i __A, __m128i __B)
{
    u__m128i v;
    u__m128i A;
    u__m128i B;
    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    B.mv[0] = __B[0];
    B.mv[1] = __B[1];

    __asm__ volatile (
        "paddusb %[l], %[al], %[bl] \n\t"
        "paddusb %[h], %[ah], %[bh] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1]), [bl]"f"(B.dv[0]), [bh]"f"(B.dv[1])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_add_epi16(__m128i __A, __m128i __B)
{
    u__m128i v;
    u__m128i A;
    u__m128i B;
    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    B.mv[0] = __B[0];
    B.mv[1] = __B[1];

    __asm__ volatile (
        "paddh %[l], %[al], %[bl] \n\t"
        "paddh %[h], %[ah], %[bh] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1]), [bl]"f"(B.dv[0]), [bh]"f"(B.dv[1])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_add_epi32(__m128i __A, __m128i __B)
{
    u__m128i v;
    u__m128i A;
    u__m128i B;
    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    B.mv[0] = __B[0];
    B.mv[1] = __B[1];

    __asm__ volatile (
        "paddw %[l], %[al], %[bl] \n\t"
        "paddw %[h], %[ah], %[bh] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1]), [bl]"f"(B.dv[0]), [bh]"f"(B.dv[1])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sub_epi8(__m128i __A, __m128i __B)
{
    u__m128i v;
    u__m128i A;
    u__m128i B;
    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    B.mv[0] = __B[0];
    B.mv[1] = __B[1];

    __asm__ volatile (
        "psubb %[l], %[al], %[bl] \n\t"
        "psubb %[h], %[ah], %[bh] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1]), [bl]"f"(B.dv[0]), [bh]"f"(B.dv[1])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sub_epi16(__m128i __A, __m128i __B)
{
    u__m128i v;
    u__m128i A;
    u__m128i B;
    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    B.mv[0] = __B[0];
    B.mv[1] = __B[1];

    __asm__ volatile (
        "psubh %[l], %[al], %[bl] \n\t"
        "psubh %[h], %[ah], %[bh] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1]), [bl]"f"(B.dv[0]), [bh]"f"(B.dv[1])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sub_epi32(__m128i __A, __m128i __B)
{
    u__m128i v;
    u__m128i A;
    u__m128i B;
    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    B.mv[0] = __B[0];
    B.mv[1] = __B[1];

    __asm__ volatile (
        "psubw %[l], %[al], %[bl] \n\t"
        "psubw %[h], %[ah], %[bh] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1]), [bl]"f"(B.dv[0]), [bh]"f"(B.dv[1])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_mullo_epi16(__m128i __A, __m128i __B)
{
    u__m128i v;
    u__m128i A;
    u__m128i B;
    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    B.mv[0] = __B[0];
    B.mv[1] = __B[1];

    __asm__ volatile (
        "pmullh %[l], %[al], %[bl] \n\t"
        "pmullh %[h], %[ah], %[bh] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1]), [bl]"f"(B.dv[0]), [bh]"f"(B.dv[1])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_mulhi_epi16(__m128i __A, __m128i __B)
{
    u__m128i v;
    u__m128i A;
    u__m128i B;
    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    B.mv[0] = __B[0];
    B.mv[1] = __B[1];

    __asm__ volatile (
        "pmulhh %[l], %[al], %[bl] \n\t"
        "pmulhh %[h], %[ah], %[bh] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1]), [bl]"f"(B.dv[0]), [bh]"f"(B.dv[1])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_mulhi_epu16(__m128i __A, __m128i __B)
{
    u__m128i v;
    u__m128i A;
    u__m128i B;
    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    B.mv[0] = __B[0];
    B.mv[1] = __B[1];

    __asm__ volatile (
        "pmulhuh %[l], %[al], %[bl] \n\t"
        "pmulhuh %[h], %[ah], %[bh] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1]), [bl]"f"(B.dv[0]), [bh]"f"(B.dv[1])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_mul_epu32(__m128i __A, __m128i __B)
{
    u__m128i v;
    u__m128i A;
    u__m128i B;
    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    B.mv[0] = __B[0];
    B.mv[1] = __B[1];

    __asm__ volatile (
        "pmuluw %[l], %[al], %[bl] \n\t"
        "pmuluw %[h], %[ah], %[bh] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1]), [bl]"f"(B.dv[0]), [bh]"f"(B.dv[1])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_and_si128(__m128i __A, __m128i __B)
{
    u__m128i v;
    u__m128i A;
    u__m128i B;
    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    B.mv[0] = __B[0];
    B.mv[1] = __B[1];

    __asm__ volatile (
        PAND  " %[l], %[al], %[bl] \n\t"
        PAND  " %[h], %[ah], %[bh] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1]), [bl]"f"(B.dv[0]), [bh]"f"(B.dv[1])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_andnot_si128(__m128i __A, __m128i __B)
{
    u__m128i v;
    u__m128i A;
    u__m128i B;
    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    B.mv[0] = __B[0];
    B.mv[1] = __B[1];

    __asm__ volatile (
        "pandn %[l], %[al], %[bl] \n\t"
        "pandn %[h], %[ah], %[bh] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1]), [bl]"f"(B.dv[0]), [bh]"f"(B.dv[1])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_or_si128(__m128i __A, __m128i __B)
{
    u__m128i v;
    u__m128i A;
    u__m128i B;
    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    B.mv[0] = __B[0];
    B.mv[1] = __B[1];

    __asm__ volatile (
        POR  " %[l], %[al], %[bl] \n\t"
        POR  " %[h], %[ah], %[bh] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1]), [bl]"f"(B.dv[0]), [bh]"f"(B.dv[1])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_xor_si128(__m128i __A, __m128i __B)
{
    u__m128i v;
    u__m128i A;
    u__m128i B;
    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    B.mv[0] = __B[0];
    B.mv[1] = __B[1];

    __asm__ volatile (
        PXOR  " %[l], %[al], %[bl] \n\t"
        PXOR  " %[h], %[ah], %[bh] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1]), [bl]"f"(B.dv[0]), [bh]"f"(B.dv[1])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_slli_epi16(__m128i __A, int32_t __S)
{
    u__m128i v;
    u__m128i A;
    union {
      int32_t i;
      float   f;
    } S;

    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    S.i = __S;

    __asm__ volatile (
        "psllh %[l], %[al], %[s] \n\t"
        "psllh %[h], %[ah], %[s] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1]), [s]"f"(S.f)
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_slli_epi32(__m128i __A, int32_t __S)
{
    u__m128i v;
    u__m128i A;
    union {
      int32_t i;
      float   f;
    } S;

    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    S.i = __S;

    __asm__ volatile (
        "psllw %[l], %[al], %[s] \n\t"
        "psllw %[h], %[ah], %[s] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1]), [s]"f"(S.f)
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_srli_epi16(__m128i __A, int32_t __S)
{
    u__m128i v;
    u__m128i A;
    union {
      int32_t i;
      float   f;
    } S;

    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    S.i = __S;

    __asm__ volatile (
        "psrlh %[l], %[al], %[s] \n\t"
        "psrlh %[h], %[ah], %[s] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1]), [s]"f"(S.f)
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_srli_epi32(__m128i __A, int32_t __S)
{
    u__m128i v;
    u__m128i A;
    union {
      int32_t i;
      float   f;
    } S;

    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    S.i = __S;

    __asm__ volatile (
        "psrlw %[l], %[al], %[s] \n\t"
        "psrlw %[h], %[ah], %[s] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1]), [s]"f"(S.f)
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_srli_si128(__m128i __A, int32_t __S)
{
    u__m128i v;
    u__m128i A;
    union {
      int32_t i;
      float   f;
    } S, T;

    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    //uint64_t s, t;

    __S = __S > 15 ? 16 : __S;

    if (__S > 8) {
        S.i = (__S - 8) << 3;

        __asm__ volatile (
            PXOR  "  %[h], %[h],  %[h] \n\t"
            SSRLD  " %[l], %[ah], %[s] \n\t"
            : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
            : [ah]"f"(A.dv[1]), [s]"f"(S.f)
        );
    } else {
        S.i = __S << 3;
        T.i = 64 - S.i;

        __asm__ volatile (
            SSLLD  " %[t], %[ah], %[t] \n\t"
            SSRLD  " %[h], %[ah], %[s] \n\t"
            SSRLD  " %[l], %[al], %[s] \n\t"
            POR  "   %[l], %[l],  %[t] \n\t"
            : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1]), [t]"+&f"(T.f)
            : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1]), [s]"f"(S.f)
        );
    }

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_srai_epi16(__m128i __A, int32_t __S)
{
    u__m128i v;
    u__m128i A;
    union {
      int32_t i;
      float   f;
    } S;

    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    S.i = __S;

    __asm__ volatile (
        "psrah %[l], %[al], %[s] \n\t"
        "psrah %[h], %[ah], %[s] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1]), [s]"f"(S.f)
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_srai_epi32(__m128i __A, int32_t __S)
{
    u__m128i v;
    u__m128i A;
    union {
      int32_t i;
      float   f;
    } S;

    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    S.i = __S;

    __asm__ volatile (
        "psraw %[l], %[al], %[s] \n\t"
        "psraw %[h], %[ah], %[s] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1]), [s]"f"(S.f)
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_shufflelo_epi16(__m128i __A, int32_t __mask)
{
    u__m128i v;
    u__m128i A;
    union {
      int32_t i;
      float   f;
    } S;

    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    S.i = __mask;

    __asm__ volatile (
        "pshufh %[l], %[al], %[s] \n\t"
        "mov.d  %[h], %[ah]       \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1]), [s]"f"(S.f)
    );

    return v.mv;
}

#define _MM_SHUFFLE(z, y, x, w) ((z << 6) | (y << 4) | (x << 2) | w)

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_shufflehi_epi16(__m128i __A, int32_t __mask)
{
    u__m128i v;
    u__m128i A;
    union {
      int32_t i;
      float   f;
    } S;

    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    S.i = __mask;

    __asm__ volatile (
        "mov.d  %[l], %[al]       \n\t"
        "pshufh %[h], %[ah], %[s] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1]), [s]"f"(S.f)
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_shuffle_epi32_0000(__m128i __A)
{
    uint64_t t;
    u__m128i v;
    u__m128i A;

    A.mv[0] = __A[0];
    A.mv[1] = __A[1];

    __asm__ volatile (
        "mfc1  %[t], %[al] \n\t"
        "mov.d %[l], %[al] \n\t"
        "mthc1 %[t], %[l]  \n\t"
        "mov.d %[h], %[l]  \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1]), [t]"=&r"(t)
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_shuffle_epi32_0020(__m128i __A)
{
    uint64_t t;
    u__m128i v;
    u__m128i A;

    A.mv[0] = __A[0];
    A.mv[1] = __A[1];

    __asm__ volatile (
        "mfc1  %[t], %[ah] \n\t"
        "mov.d %[l], %[al] \n\t"
        "mov.d %[h], %[al] \n\t"
        "mthc1 %[t], %[l]  \n\t"
        "mfc1  %[t], %[al] \n\t"
        "mthc1 %[t], %[h]  \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1]), [t]"=&r"(t)
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_shuffle_epi32_3232(__m128i __A)
{
    u__m128i v;
    u__m128i A;

    A.mv[0] = __A[0];
    A.mv[1] = __A[1];

    __asm__ volatile (
        "mov.d %[l], %[ah] \n\t"
        "mov.d %[h], %[ah] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_min_epu8(__m128i __A, __m128i __B)
{
    u__m128i v;
    u__m128i A;
    u__m128i B;

    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    B.mv[0] = __B[0];
    B.mv[1] = __B[1];

    __asm__ volatile (
        "pminub %[l], %[al], %[bl] \n\t"
        "pminub %[h], %[ah], %[bh] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1]), [bl]"f"(B.dv[0]), [bh]"f"(B.dv[1])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_min_epi16(__m128i __A, __m128i __B)
{
    u__m128i v;
    u__m128i A;
    u__m128i B;

    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    B.mv[0] = __B[0];
    B.mv[1] = __B[1];

    __asm__ volatile (
        "pminsh %[l], %[al], %[bl] \n\t"
        "pminsh %[h], %[ah], %[bh] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1]), [bl]"f"(B.dv[0]), [bh]"f"(B.dv[1])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_max_epu8(__m128i __A, __m128i __B)
{
    u__m128i v;
    u__m128i A;
    u__m128i B;

    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    B.mv[0] = __B[0];
    B.mv[1] = __B[1];

    __asm__ volatile (
        "pmaxub %[l], %[al], %[bl] \n\t"
        "pmaxub %[h], %[ah], %[bh] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1]), [bl]"f"(B.dv[0]), [bh]"f"(B.dv[1])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_max_epi16(__m128i __A, __m128i __B)
{
    u__m128i v;
    u__m128i A;
    u__m128i B;

    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    B.mv[0] = __B[0];
    B.mv[1] = __B[1];

    __asm__ volatile (
        "pmaxsh %[l], %[al], %[bl] \n\t"
        "pmaxsh %[h], %[ah], %[bh] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1]), [bl]"f"(B.dv[0]), [bh]"f"(B.dv[1])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpeq_epi8(__m128i __A, __m128i __B)
{
    u__m128i v;
    u__m128i A;
    u__m128i B;

    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    B.mv[0] = __B[0];
    B.mv[1] = __B[1];

    __asm__ volatile (
        "pcmpeqb %[l], %[al], %[bl] \n\t"
        "pcmpeqb %[h], %[ah], %[bh] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1]), [bl]"f"(B.dv[0]), [bh]"f"(B.dv[1])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpeq_epi16(__m128i __A, __m128i __B)
{
    u__m128i v;
    u__m128i A;
    u__m128i B;

    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    B.mv[0] = __B[0];
    B.mv[1] = __B[1];

    __asm__ volatile (
        "pcmpeqh %[l], %[al], %[bl] \n\t"
        "pcmpeqh %[h], %[ah], %[bh] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1]), [bl]"f"(B.dv[0]), [bh]"f"(B.dv[1])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpeq_epi32(__m128i __A, __m128i __B)
{
    u__m128i v;
    u__m128i A;
    u__m128i B;

    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    B.mv[0] = __B[0];
    B.mv[1] = __B[1];

    __asm__ volatile (
        "pcmpeqw %[l], %[al], %[bl] \n\t"
        "pcmpeqw %[h], %[ah], %[bh] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1]), [bl]"f"(B.dv[0]), [bh]"f"(B.dv[1])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmplt_epi8(__m128i __A, __m128i __B)
{
    u__m128i v;
    u__m128i A;
    u__m128i B;

    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    B.mv[0] = __B[0];
    B.mv[1] = __B[1];

    __asm__ volatile (
        "pcmpgtb %[l], %[bl], %[al] \n\t"
        "pcmpgtb %[h], %[bh], %[ah] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1]), [bl]"f"(B.dv[0]), [bh]"f"(B.dv[1])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmplt_epi32(__m128i __A, __m128i __B)
{
    u__m128i v;
    u__m128i A;
    u__m128i B;

    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    B.mv[0] = __B[0];
    B.mv[1] = __B[1];

    __asm__ volatile (
        "pcmpgtw %[l], %[bl], %[al] \n\t"
        "pcmpgtw %[h], %[bh], %[ah] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1]), [bl]"f"(B.dv[0]), [bh]"f"(B.dv[1])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpgt_epi32(__m128i __A, __m128i __B)
{
    u__m128i v;
    u__m128i A;
    u__m128i B;

    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    B.mv[0] = __B[0];
    B.mv[1] = __B[1];

    __asm__ volatile (
        "pcmpgtw %[l], %[al], %[bl] \n\t"
        "pcmpgtw %[h], %[ah], %[bh] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1]), [bl]"f"(B.dv[0]), [bh]"f"(B.dv[1])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_packus_epi16(__m128i __A, __m128i __B)
{
    double t;
    u__m128i v;
    u__m128i A;
    u__m128i B;

    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    B.mv[0] = __B[0];
    B.mv[1] = __B[1];

    __asm__ volatile (
        "packushb  %[t], %[ah], %[bh] \n\t"
        "packushb  %[l], %[al], %[bl] \n\t"
        "punpckhwd %[h], %[l],  %[t]  \n\t"
        "punpcklwd %[l], %[l],  %[t]  \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1]), [t]"=&f"(t)
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1]), [bl]"f"(B.dv[0]), [bh]"f"(B.dv[1])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_packs_epi32(__m128i __A, __m128i __B)
{
    double t;
    u__m128i v;
    u__m128i A;
    u__m128i B;

    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    B.mv[0] = __B[0];
    B.mv[1] = __B[1];

    __asm__ volatile (
        "packsswh  %[t], %[ah], %[bh] \n\t"
        "packsswh  %[l], %[al], %[bl] \n\t"
        "punpckhwd %[h], %[l],  %[t]  \n\t"
        "punpcklwd %[l], %[l],  %[t]  \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1]), [t]"=&f"(t)
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1]), [bl]"f"(B.dv[0]), [bh]"f"(B.dv[1])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_unpacklo_epi8(__m128i __A, __m128i __B)
{
    u__m128i v;
    u__m128i A;
    u__m128i B;

    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    B.mv[0] = __B[0];
    B.mv[1] = __B[1];

    __asm__ volatile (
        "punpckhbh %[h], %[al], %[bl] \n\t"
        "punpcklbh %[l], %[al], %[bl] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [bl]"f"(B.dv[0])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_unpacklo_epi16(__m128i __A, __m128i __B)
{
    u__m128i v;
    u__m128i A;
    u__m128i B;

    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    B.mv[0] = __B[0];
    B.mv[1] = __B[1];

    __asm__ volatile (
        "punpckhhw %[h], %[al], %[bl] \n\t"
        "punpcklhw %[l], %[al], %[bl] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [bl]"f"(B.dv[0])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_unpacklo_epi32(__m128i __A, __m128i __B)
{
    u__m128i v;
    u__m128i A;
    u__m128i B;

    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    B.mv[0] = __B[0];
    B.mv[1] = __B[1];

    __asm__ volatile (
        "punpckhwd %[h], %[al], %[bl] \n\t"
        "punpcklwd %[l], %[al], %[bl] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [bl]"f"(B.dv[0])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_unpacklo_epi64(__m128i __A, __m128i __B)
{
    u__m128i v;
    u__m128i A;
    u__m128i B;

    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    B.mv[0] = __B[0];
    B.mv[1] = __B[1];

    __asm__ volatile (
        "mov.d %[l], %[al] \n\t"
        "mov.d %[h], %[bl] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [al]"f"(A.dv[0]), [bl]"f"(B.dv[0])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_unpackhi_epi8(__m128i __A, __m128i __B)
{
    u__m128i v;
    u__m128i A;
    u__m128i B;

    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    B.mv[0] = __B[0];
    B.mv[1] = __B[1];

    __asm__ volatile (
        "punpcklbh %[l], %[ah], %[bh] \n\t"
        "punpckhbh %[h], %[ah], %[bh] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [ah]"f"(A.dv[1]), [bh]"f"(B.dv[1])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_unpackhi_epi16(__m128i __A, __m128i __B)
{
    u__m128i v;
    u__m128i A;
    u__m128i B;

    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    B.mv[0] = __B[0];
    B.mv[1] = __B[1];

    __asm__ volatile (
        "punpcklhw %[l], %[ah], %[bh] \n\t"
        "punpckhhw %[h], %[ah], %[bh] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [ah]"f"(A.dv[1]), [bh]"f"(B.dv[1])
    );

    return v.mv;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_unpackhi_epi32(__m128i __A, __m128i __B)
{
    u__m128i v;
    u__m128i A;
    u__m128i B;

    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    B.mv[0] = __B[0];
    B.mv[1] = __B[1];

    __asm__ volatile (
        "punpcklwd %[l], %[ah], %[bh] \n\t"
        "punpckhwd %[h], %[ah], %[bh] \n\t"
        : [l]"=&f"(v.dv[0]), [h]"=&f"(v.dv[1])
        : [ah]"f"(A.dv[1]), [bh]"f"(B.dv[1])
    );

    return v.mv;
}

static inline int32_t __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_movemask_epi8(__m128i __A)
{
    int32_t v;
    double h;
    double l;
    u__m128i A;

    A.mv[0] = __A[0];
    A.mv[1] = __A[1];

    __asm__ volatile (
        "pmovmskb  %[l], %[al]      \n\t"
        "pmovmskb  %[h], %[ah]      \n\t"
        "punpcklbh %[l], %[l], %[h] \n\t"
        "mfc1      %[v], %[l]       \n\t"
        : [v]"=&r"(v), [l]"=&f"(l), [h]"=&f"(h)
        : [al]"f"(A.dv[0]), [ah]"f"(A.dv[1])
    );

    return v;
}

static inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_unpackhi_epi64(__m128i __A, __m128i __B)
{
    u__m128i v;
    u__m128i A;
    u__m128i B;

    A.mv[0] = __A[0];
    A.mv[1] = __A[1];
    B.mv[0] = __B[0];
    B.mv[1] = __B[1];

    __asm__ volatile (
        "mov.d  %[vl], %[ah] \n\t"
        "mov.d  %[vh], %[bh] \n\t"
        : [vh]"=&f"(v.dv[1]), [vl]"=&f"(v.dv[0])
        : [ah]"f"(A.dv[1]), [bh]"f"(B.dv[1])
    );

    return v.mv;
}

#endif /* __LOONGSON_MMINTRIN_H__ */

