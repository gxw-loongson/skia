/*
 * Copyright 2016 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "SkCpu.h"
#include "SkOnce.h"

#if defined(SK_CPU_X86)
    #if defined(SK_BUILD_FOR_WIN)
        #include <intrin.h>
        static void cpuid (uint32_t abcd[4]) { __cpuid  ((int*)abcd, 1);    }
        static void cpuid7(uint32_t abcd[4]) { __cpuidex((int*)abcd, 7, 0); }
        static uint64_t xgetbv(uint32_t xcr) { return _xgetbv(xcr); }
    #else
        #include <cpuid.h>
        #if !defined(__cpuid_count)  // Old Mac Clang doesn't have this defined.
            #define  __cpuid_count(eax, ecx, a, b, c, d) \
                __asm__("cpuid" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "0"(eax), "2"(ecx))
        #endif
        static void cpuid (uint32_t abcd[4]) { __get_cpuid(1, abcd+0, abcd+1, abcd+2, abcd+3); }
        static void cpuid7(uint32_t abcd[4]) {
            __cpuid_count(7, 0, abcd[0], abcd[1], abcd[2], abcd[3]);
        }
        static uint64_t xgetbv(uint32_t xcr) {
            uint32_t eax, edx;
            __asm__ __volatile__ ( "xgetbv" : "=a"(eax), "=d"(edx) : "c"(xcr));
            return (uint64_t)(edx) << 32 | eax;
        }
    #endif

    static uint32_t read_cpu_features() {
        uint32_t features = 0;
        uint32_t abcd[4] = {0,0,0,0};

        // You might want to refer to http://www.sandpile.org/x86/cpuid.htm

        cpuid(abcd);
        if (abcd[3] & (1<<25)) { features |= SkCpu:: SSE1; }
        if (abcd[3] & (1<<26)) { features |= SkCpu:: SSE2; }
        if (abcd[2] & (1<< 0)) { features |= SkCpu:: SSE3; }
        if (abcd[2] & (1<< 9)) { features |= SkCpu::SSSE3; }
        if (abcd[2] & (1<<19)) { features |= SkCpu::SSE41; }
        if (abcd[2] & (1<<20)) { features |= SkCpu::SSE42; }

        if ((abcd[2] & (3<<26)) == (3<<26)         // XSAVE + OSXSAVE
             && (xgetbv(0) & (3<<1)) == (3<<1)) {  // XMM and YMM state enabled.
            if (abcd[2] & (1<<28)) { features |= SkCpu:: AVX; }
            if (abcd[2] & (1<<29)) { features |= SkCpu::F16C; }
            if (abcd[2] & (1<<12)) { features |= SkCpu:: FMA; }

            cpuid7(abcd);
            if (abcd[1] & (1<<5)) { features |= SkCpu::AVX2; }
            if (abcd[1] & (1<<3)) { features |= SkCpu::BMI1; }
            if (abcd[1] & (1<<8)) { features |= SkCpu::BMI2; }

            if ((xgetbv(0) & (7<<5)) == (7<<5)) {  // All ZMM state bits enabled too.
                if (abcd[1] & (1<<16)) { features |= SkCpu::AVX512F; }
                if (abcd[1] & (1<<17)) { features |= SkCpu::AVX512DQ; }
                if (abcd[1] & (1<<21)) { features |= SkCpu::AVX512IFMA; }
                if (abcd[1] & (1<<26)) { features |= SkCpu::AVX512PF; }
                if (abcd[1] & (1<<27)) { features |= SkCpu::AVX512ER; }
                if (abcd[1] & (1<<28)) { features |= SkCpu::AVX512CD; }
                if (abcd[1] & (1<<30)) { features |= SkCpu::AVX512BW; }
                if (abcd[1] & (1<<31)) { features |= SkCpu::AVX512VL; }
            }
        }
        return features;
    }

#elif defined(SK_CPU_ARM64) && __has_include(<sys/auxv.h>)
    #include <sys/auxv.h>

    static uint32_t read_cpu_features() {
        const uint32_t kHWCAP_CRC32   = (1<< 7),
                       kHWCAP_ASIMDHP = (1<<10);

        uint32_t features = 0;
        uint32_t hwcaps = getauxval(AT_HWCAP);
        if (hwcaps & kHWCAP_CRC32  ) { features |= SkCpu::CRC32; }
        if (hwcaps & kHWCAP_ASIMDHP) { features |= SkCpu::ASIMDHP; }
        return features;
    }

#elif defined(SK_CPU_ARM32) && __has_include(<sys/auxv.h>) && \
    (!defined(__ANDROID_API__) || __ANDROID_API__ >= 18)
    // sys/auxv.h will always be present in the Android NDK due to unified
    //headers, but getauxval is only defined for API >= 18.
    #include <sys/auxv.h>

    static uint32_t read_cpu_features() {
        const uint32_t kHWCAP_NEON  = (1<<12);
        const uint32_t kHWCAP_VFPv4 = (1<<16);

        uint32_t features = 0;
        uint32_t hwcaps = getauxval(AT_HWCAP);
        if (hwcaps & kHWCAP_NEON ) {
            features |= SkCpu::NEON;
            if (hwcaps & kHWCAP_VFPv4) { features |= SkCpu::NEON_FMA|SkCpu::VFP_FP16; }
        }
        return features;
    }

#elif defined(SK_CPU_ARM32) && __has_include(<cpu-features.h>)
    #include <cpu-features.h>

    static uint32_t read_cpu_features() {
        uint32_t features = 0;
        uint64_t cpu_features = android_getCpuFeatures();
        if (cpu_features & ANDROID_CPU_ARM_FEATURE_NEON)     { features |= SkCpu::NEON; }
        if (cpu_features & ANDROID_CPU_ARM_FEATURE_NEON_FMA) { features |= SkCpu::NEON_FMA; }
        if (cpu_features & ANDROID_CPU_ARM_FEATURE_VFP_FP16) { features |= SkCpu::VFP_FP16; }
        return features;
    }

#elif defined(SK_CPU_MIPS) || defined(SK_CPU_MIPS64)
    #include <sys/wait.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <signal.h>
    #include <fcntl.h>
    #include <cstring>

    #define MMI_MASK    0x00000010
    #define MSA1_MASK   0x00000020
    #define MSA2_MASK   0x00000040
    #define LSX1_MASK   0x00000200
    #define LSX2_MASK   0x00000400
    #define LASX_MASK   0x00000800
    int fd[2];
    int support_cpucfg;

    static void handler(int signum)
    {
        close(fd[1]);
        exit(1);
    }

    /* Brief :  Function to check if cpucfg supported.
     * Return:  1   supported
     *          0   not supported
     */
    static int cpucfg_test(void)
    {
        pid_t pid;
        int status = 0;

        support_cpucfg = 0;
        pipe(fd);
        pid = fork();
        if (pid == 0) { /* Subprocess */
            struct sigaction act;
            close(fd[0]);
            /* Set signal action for SIGILL. */
            act.sa_handler = handler;
            sigaction(SIGILL,&act,NULL);

            /* Execute cpucfg in subprocess. */
            __asm__ volatile(
                ".insn              \n\t"
                ".word (0xc8080118) \n\t"
                :::
            );
            support_cpucfg = 1;
            write(fd[1],&support_cpucfg,sizeof(support_cpucfg));
            close(fd[1]);
            exit(0);
        } else if (pid > 0){ /* Parent process*/
            close(fd[1]);
            if ((waitpid(pid,&status,0) <= 0) ||
                (read(fd[0],&support_cpucfg,sizeof(support_cpucfg)) <= 0))
                support_cpucfg = 0;
            close(fd[0]);
        } else {
            support_cpucfg = 0;
        }

        return support_cpucfg;
    }

    static int get_cpu_flags_from_cpucfg(void)
    {
        int flags = 0;
        int flag = 0;
        __asm__ volatile(
            ".insn                     \n\t"
            "dli    $8,    0x01        \n\t"
            ".word (0xc9084918)        \n\t"
            "usw    $9,    0x00(%0)    \n\t"
            :
            : "r"(&flag)
            : "memory"
        );
        if (flag & MMI_MASK)
            flags |= SkCpu::MMI;
        if (flag & MSA1_MASK)
            flags |= SkCpu::MSA;
        if (flag & MSA2_MASK)
            flags |= SkCpu::MSA2;
        if (flag & LSX1_MASK && flag & LSX2_MASK)
            flags |= SkCpu::LSX;
        if (flag & LASX_MASK)
            flags |= SkCpu::LASX;
        return flags;
    }

    static int get_cpu_flags_from_cpuinfo(void)
    {
        uint32_t flags = 0;

# ifdef __linux__
        FILE* fp = fopen("/proc/cpuinfo", "r");
        if (!fp)
            return flags;

        char buf[200];
        memset(buf, 0, sizeof(buf));
        while (fgets(buf, sizeof(buf), fp)) {
            if (!strncmp(buf, "cpu model", strlen("cpu model"))) {
                if (strstr(buf, "Loongson-3") || strstr(buf, "Loongson-2K")) {
                    flags |= SkCpu::MMI;
                }
                break;
            }
        }
        while (fgets(buf, sizeof(buf), fp)) {
            if(!strncmp(buf, "ASEs implemented", strlen("ASEs implemented"))) {
                if (strstr(buf, "loongson-mmi") && strstr(buf, "loongson-ext")) {
                    flags |= SkCpu::MMI;
                }
                if (strstr(buf, "msa")) {
                    flags |= SkCpu::MSA;
                }
                break;
            }
        }
        fclose(fp);
# endif

        return flags;
    }

    static uint32_t read_cpu_features() {
        int flags = 0;
        int ret = 0;

        ret = cpucfg_test();
        if (ret == 1)
            flags = get_cpu_flags_from_cpucfg();
        else
            flags = get_cpu_flags_from_cpuinfo();
        return flags;
    }

#else
    static uint32_t read_cpu_features() {
        return 0;
    }

#endif

uint32_t SkCpu::gCachedFeatures = 0;

void SkCpu::CacheRuntimeFeatures() {
    static SkOnce once;
    once([] { gCachedFeatures = read_cpu_features(); });
}
