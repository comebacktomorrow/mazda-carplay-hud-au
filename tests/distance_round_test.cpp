// Host unit test for distance_round.h:  g++ -std=c++11 -Wall -I mazda/patches/blmjcicarplay tests/distance_round_test.cpp && ./a.out
#include "distance_round.h"
#include <cstdio>

static int fails = 0;
static void expect(int32_t in, int32_t disp, uint32_t unit)
{
    RoundedDistance r = carplay_round_distance(in);
    if (r.disp != disp || r.unit != unit) {
        std::printf("FAIL %6d m -> disp=%d unit=%u, want disp=%d unit=%u\n", in, r.disp, r.unit, disp, unit);
        ++fails;
    }
}

int main()
{
    // < 30 m: 5 m steps
    expect(2,     0,       1);
    expect(3,     5000,    1);
    expect(13,    15000,   1);
    expect(12,    10000,   1);
    expect(27,    25000,   1);
    expect(29,    30000,   1);
    // 30..99 m: 10 m steps
    expect(30,    30000,   1);
    expect(34,    30000,   1);
    expect(35,    40000,   1);
    expect(64,    60000,   1);
    expect(94,    90000,   1);
    expect(95,    100000,  1);   // rounds up to 100
    // 100..299 m: 50 m steps
    expect(100,   100000,  1);
    expect(124,   100000,  1);
    expect(125,   150000,  1);
    expect(260,   250000,  1);
    expect(274,   250000,  1);
    expect(275,   300000,  1);
    // 300..999 m: 100 m steps
    expect(300,   300000,  1);
    expect(349,   300000,  1);
    expect(350,   400000,  1);
    expect(640,   600000,  1);
    expect(949,   900000,  1);
    expect(950,   1000,    3);   // 1000 m -> shown as 1.0 km
    expect(999,   1000,    3);
    // 1..10 km: 0.1 km
    expect(1000,  1000,    3);
    expect(1260,  1300,    3);
    expect(1249,  1200,    3);
    expect(9949,  9900,    3);
    expect(9950,  10000,   3);
    // >= 10 km: whole km
    expect(12400, 12000,   3);
    expect(12500, 13000,   3);
    expect(-5,    0,       1);
    if (fails) { std::printf("%d failure(s)\n", fails); return 1; }
    std::printf("distance_round: all cases pass\n");
    return 0;
}
