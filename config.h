#include <stddef.h>

constexpr size_t n_rndrs = 9;
constexpr size_t n_elves = 10;
constexpr size_t n_group = 3;

_Static_assert(n_rndrs <= 15000, "Too many reindeers");
_Static_assert(n_elves <= 15000, "Too many elves");
_Static_assert(n_group <= n_elves, "Elf group too large");
