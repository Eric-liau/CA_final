#include "stdio.h"
#include "limits.h"
#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"
#include "stdlib.h"
#include "assert.h"
#include "twin.h"

/* find last (most-significant) bit set */
#define fls32(x) ((x) ? sizeof(int) * 8 - __builtin_clz(x) : 0)

void fastdiv_prepare(uint32_t div, uint32_t *m, uint8_t *s1, uint8_t *s2)
{
    const int l = fls32(div - 1);
    const uint64_t nextpow2 = UINT64_C(1) << l;
    uint32_t magic = ((nextpow2 - div) << 32) / div;

    *m = magic + 1;
    *s1 = (l > 1) ? 1 : l;
    *s2 = (l == 0) ? 0 : l - 1;

    if (div == 1)
        return;

    if (magic == 0) { /* div == nextpow2 */
        *s1 -= 1, *s2 += 1;
    } else {
        magic = (magic + UINT64_C(0x100000000)) / 2;
        uint32_t rem = (nextpow2 << 31) - magic * div; 
        assert(rem > 0 && rem < div);
        if (rem > div - (nextpow2 >> 1)) {
            *m = magic + 1;
            *s1 -= 1;
        }
    }
}

static uint32_t fastdiv(uint32_t val, uint64_t mul, uint8_t s1, uint8_t s2)
{   
    const uint32_t hi = (val * mul) >> 32;
    return (hi + ((val - hi) >> s1)) >> s2;
}

static inline twin_fixed_t sin_poly(twin_angle_t x)
{
    /* S(x) = x * 2^(-n) * (A1 - 2 ^ (q-p) * x * (2^-n) * x * 2^(-n) * (B1 - 2 ^
     * (-r) * x * 2 ^ (-n) * C1 * x)) * 2 ^ (a-q)
     * @n: the angle scale
     * @A: the amplitude
     * @p,q,r: the scaling factor
     *
     * A1 = 2^q * a5, B1 = 2 ^ p * b5, C1 = 2 ^ (r+p-n) * c5
     * where a5, b5, c5 are the coefficients for 5th-order polynomial
     * a5 = 4 * (3 / pi - 9 / 16)
     * b5 = 2 * a5 - 5 / 2
     * c5 = a5 - 3 / 2
     */
    const uint64_t A = 16, n = 10, p = 32, q = 31, r = 3;
    const uint64_t A1 = 3370945099, B1 = 2746362156, C1 = 2339369;
    uint64_t y = (C1 * x) >> n;
    y = B1 - ((x * y) >> r);
    y = x * (y >> n);
    y = x * (y >> n);
    y = A1 - (y >> (p - q));
    y = x * (y >> n);
    y = (y + (1UL << (q - A - 1))) >> (q - A); /* Rounding */
    return y;
}

void twin_sincos(twin_angle_t a, twin_fixed_t *sin, twin_fixed_t *cos)
{
    twin_fixed_t sin_val = 0, cos_val = 0;

    /* limit to [0..360) */
    a = a & (TWIN_ANGLE_360 - 1);
    int c = a > TWIN_ANGLE_90 && a < TWIN_ANGLE_270;
    /* special case for 90 degrees */
    if ((a & ~(TWIN_ANGLE_180)) == TWIN_ANGLE_90) {
        sin_val = TWIN_FIXED_ONE;
        cos_val = 0;
    } else {
        /* mirror second and third quadrant values across y axis */
        if (a & TWIN_ANGLE_90)
            a = TWIN_ANGLE_180 - a;
        twin_angle_t x = a & (TWIN_ANGLE_90 - 1);
        if (sin)
            sin_val = sin_poly(x);
        if (cos)
            cos_val = sin_poly(TWIN_ANGLE_90 - x);
    }
    if (sin) {
        /* mirror third and fourth quadrant values across x axis */
        if (a & TWIN_ANGLE_180)
            sin_val = -sin_val;
        *sin = sin_val;
    }
    if (cos) {
        /* mirror first and fourth quadrant values across y axis */
        if (c)
            cos_val = -cos_val;
        *cos = cos_val;
    }
}
twin_fixed_t twin_tan(twin_angle_t a)
{
    twin_fixed_t s, c;
    twin_sincos(a, &s, &c);

    if (c == 0) {
        if (s > 0)
            return TWIN_FIXED_MAX;
        return TWIN_FIXED_MIN;
    }
    if (s == 0)
        return 0;
    uint8_t s1, s2;
    uint32_t mul;
    fastdiv_prepare(c, &mul, &s1, &s2);
    return fastdiv(s << 15, mul, s1, s2) << 1;
}