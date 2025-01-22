#include "stdio.h"
#include "limits.h"
#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"
#include "stdlib.h"
#include "assert.h"
#include "math.h"
#include "time.h"

#define TWIN_FIXED_ONE (0x10000)
#define TWIN_FIXED_HALF (0x08000)
#define TWIN_FIXED_MAX (0x7fffffff)
#define TWIN_FIXED_MIN (-0x7fffffff)

typedef uint8_t twin_a8_t;
typedef uint16_t twin_a16_t;
typedef uint16_t twin_rgb16_t;
typedef uint32_t twin_argb32_t;
typedef uint32_t twin_ucs4_t;
typedef int16_t twin_coord_t;
typedef int16_t twin_count_t;
typedef int16_t twin_keysym_t;
typedef uint8_t twin_js_number_t;
typedef int16_t twin_js_value_t;
typedef int32_t twin_area_t;
typedef int32_t twin_time_t;
typedef int16_t twin_stretch_t;
typedef int32_t twin_fixed_t; /* 16.16 format */
typedef int16_t twin_angle_t; /* -2048 .. 2048 for -180 .. 180 */

#define TWIN_ANGLE_360 4096
#define TWIN_ANGLE_180 (TWIN_ANGLE_360 >> 1)
#define TWIN_ANGLE_90 (TWIN_ANGLE_360 >> 2)
#define TWIN_ANGLE_45 (TWIN_ANGLE_360 >> 3)
#define TWIN_ANGLE_22_5 (TWIN_ANGLE_360 >> 4)
#define TWIN_ANGLE_11_25 (TWIN_ANGLE_360 >> 5)

#define TWIN_ANGLE_270 (TWIN_ANGLE_180 + TWIN_ANGLE_90)
#define TWIN_ANGLE_0 (0)

#define twin_degrees_to_angle(d) \
    ((twin_angle_t) ((((int32_t) (d)) * TWIN_ANGLE_360 / 360)))

static const uint32_t m[] = {1, 1, 2863311531, 1, 3435973837, 2863311531, 613566757, 1, 3817748708, 3435973837, 3123612579, 2863311531, 
    2643056798, 613566757, 2290649225, 1, 4042322161, 3817748708, 2938661835, 3435973837, 2249744775, 3123612579, 2987803337, 2863311531, 
    2748779070, 2643056798, 795364315, 613566757, 2369637129, 2290649225, 138547333, 1, 4164816772, 4042322161, 3558687189, 3817748708, 
    3134165325, 2938661835, 2753184165, 3435973837, 3352169597, 2249744775, 3196254732, 3123612579, 1813430637, 2987803337, 2924233053, 
    2863311531, 2804876602, 2748779070, 2694881441, 2643056798, 891408307, 795364315, 702812831, 613566757, 527452125, 2369637129, 2329473788, 
    2290649225, 2253097598, 138547333, 68174085, 1, 4228890877, 4164816772, 4102655328, 4042322161, 3983737782, 3558687189, 3871519817, 
    3817748708, 3235934265, 3134165325, 3665038760, 2938661835, 3569842948, 2753184165, 3479467177, 3435973837, 3393554407, 3352169597, 
    3311782012, 2249744775, 3233857729, 3196254732, 3159516172, 3123612579, 3088515809, 1813430637, 1746305385, 2987803337, 2955676419, 
    2924233053, 1491936009, 2863311531, 1372618415, 2804876602, 2776544515, 2748779070, 1148159575, 2694881441, 1042467791, 2643056798, 
    940802361, 891408307, 842937507, 795364315, 748664025, 702812831, 657787785, 613566757, 570128403, 527452125, 485518043, 2369637129, 
    403800345, 2329473788, 2309898378, 2290649225, 248469183, 2253097598, 174592167, 138547333, 2199023256, 68174085, 33818641};

static const uint8_t s1[] = {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 
    1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0,
     1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 
     1, 0, 1, 1, 0, 1, 1};

static const uint8_t s2[] = {9, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 
    13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 
    14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15};
    
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
twin_fixed_t twin_tan_my(twin_angle_t a)
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
    int sign = s >> 31 ^ c >> 31;
    printf("%-8d%-8d%-8d", s, c, sign);
    if(c < 0)
        c = -c;
    if(s < 0)
        s = -s;
    //printf("%-8d%-8d", s, c);
    int c1 = (c >> 9) + ((c & 256) >> 8);
    c = c >> 9;

    printf("%-8d%-8d",c, c1);
    if(c == 127)
        return sign ? -s : s;
    else{
        twin_fixed_t res = fastdiv(s << 15, m[c], s1[c], s2[c]) << 1;
        return sign ? -res : res;
    }
}
twin_fixed_t twin_tan_fast(twin_angle_t a)
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
    int sign = s >> 31 ^ c >> 31 ? -1 : 1;
    //printf("%-8d%-8d%-8d", s, c, sign);
    if(c < 0)
        c = -c;
    if(s < 0)
        s = -s;
    uint32_t mul;
    uint8_t s1, s2;

    fastdiv_prepare(c, &mul, &s1, &s2); 
    return sign * fastdiv(s << 15, mul, s1, s2) << 1;
    
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
    return ((s << 15) / c) << 1;
}
int main(){
    //srand(time(NULL));
    //int16_t test[20];
    //for(int i = 0; i < 20; i++){
    //    test[i] = (rand() + i) % 4048;
    //}
    FILE *f = fopen("test_fastdiv.txt", "w");
    fprintf(f, "| angle | answer | myanswer | fast_div | error rate | fast_div error rate |\n");
    fprintf(f, "| ----- | ------ | -------- | -------- | ---------- | ------------------- |\n");
    int16_t test[20] = {14659, 41967, 21825, 41634, 20873, 36471, 41746, 30660, 60680, 17029, 28151, 22910, 40912, 64147, 10802, 2959, 34279, 21628, 12564, 3480};
    float error_rate_avg = 0;
    for(int i = 0; i < 20; i++){
        float error_rate, error_rate_fast;
        twin_fixed_t my = twin_tan_my(test[i]);
        twin_fixed_t golden = twin_tan(test[i]);
        twin_fixed_t fast = twin_tan_fast(test[i]);
        int d = abs(my - golden), dfast = abs(fast - golden);
        error_rate = (float)d / abs(golden);
        error_rate_fast = (float)dfast / abs(golden);
        fprintf(f, "| %d | %d | %d | %d | %f | %f |\n", test[i], golden, my, fast, error_rate, error_rate_fast);
        error_rate_avg += error_rate;
        printf("%-8d%-8d%-8d%-16f\n", my, golden, fast, error_rate);
    }
    fclose(f);
    error_rate_avg = error_rate_avg / 20.0;
    printf("\n%f", error_rate_avg);
}