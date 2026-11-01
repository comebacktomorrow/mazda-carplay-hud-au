// SPDX-License-Identifier: AGPL-3.0-or-later
//
// CarPlay -> HUD bridge for Mazda Connect (CMU150).
// Part of an AGPL-3.0 work - see NOTICE.md for full attribution.
//
// [ROUNDING] Round the guidance distance the way the CarPlay banner reads it, so the HUD and the
// centre screen agree instead of the HUD ticking down metre by metre. Header-only and free of any
// OEM dependency so it can be unit-tested on the host (see tests/distance_round_test.cpp).
//
// Bands (metric), round-half-up to the nearest step:
//   >= 10 km          whole km            12.4 km -> 12 km
//   1 km .. < 10 km   0.1 km              1.26 km -> 1.3 km
//   500 m .. < 1 km   100 m               640 m   -> 600 m
//   100 m .. < 500 m  50 m                130 m   -> 150 m
//   < 100 m           5 m                 13 m    -> 15 m, 2 m -> 0 m (shown as "now")
// A value that rounds up across a band edge is re-expressed in the larger unit
// (e.g. 960 m -> 1000 m -> "1.0 km"), matching how the CarPlay banner switches over.
//
// The thresholds are #defines so they can be tuned from the Makefile without editing code.

#ifndef LIBPATCH_BLMJCICARPLAY_DISTANCE_ROUND_H
#define LIBPATCH_BLMJCICARPLAY_DISTANCE_ROUND_H

#include <stdint.h>

#ifndef CARPLAY_ROUND_KM_WHOLE_FROM
#define CARPLAY_ROUND_KM_WHOLE_FROM 10000   // metres: at/above this, whole km
#endif
#ifndef CARPLAY_ROUND_KM_FROM
#define CARPLAY_ROUND_KM_FROM       1000    // metres: at/above this, km with one decimal
#endif
#ifndef CARPLAY_ROUND_100_FROM
#define CARPLAY_ROUND_100_FROM      500     // metres: at/above this, 100 m steps
#endif
#ifndef CARPLAY_ROUND_50_FROM
#define CARPLAY_ROUND_50_FROM       100     // metres: at/above this, 50 m steps
#endif
#ifndef CARPLAY_ROUND_SMALL_STEP
#define CARPLAY_ROUND_SMALL_STEP    5       // metres: step below CARPLAY_ROUND_50_FROM
#endif

// Unit codes are the proto DISPLAY_DISTANCE_UNIT values hud_on_distance() expects:
// 1 = METERS, 3 = KILOMETERS. `disp` is "raw unit * 1000", the same scale nav.cpp already passes.
struct RoundedDistance {
    int32_t  disp;   // raw unit * 1000
    uint32_t unit;   // 1 = metres, 3 = km
};

static inline int32_t round_to_step(int32_t v, int32_t step)
{
    return ((v + step / 2) / step) * step;
}

static inline RoundedDistance carplay_round_distance(int32_t meters)
{
    RoundedDistance r;
    if (meters < 0) meters = 0;

    int32_t m;
    if (meters >= CARPLAY_ROUND_KM_WHOLE_FROM)      m = round_to_step(meters, 1000);
    else if (meters >= CARPLAY_ROUND_KM_FROM)       m = round_to_step(meters, 100);
    else if (meters >= CARPLAY_ROUND_100_FROM)      m = round_to_step(meters, 100);
    else if (meters >= CARPLAY_ROUND_50_FROM)       m = round_to_step(meters, 50);
    else                                            m = round_to_step(meters, CARPLAY_ROUND_SMALL_STEP);

    if (m >= CARPLAY_ROUND_KM_FROM) {
        r.unit = 3;          // km: disp = metres (km * 1000), HUD shows tenths
        r.disp = m;
    } else {
        r.unit = 1;          // metres: disp = metres * 1000
        r.disp = m * 1000;
    }
    return r;
}

#endif // LIBPATCH_BLMJCICARPLAY_DISTANCE_ROUND_H
