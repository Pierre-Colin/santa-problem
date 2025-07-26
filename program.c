#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>

#include "config.h"
#include "elves.h"
#include "santa.h"

int deer_start(void *);

constexpr size_t n_ids = n_rndrs > n_elves ? n_rndrs : n_elves;

static void
start_thread(thrd_t *thr, thrd_start_t func, void *arg, const char *name)
{
	switch (thrd_create(thr, func, arg)) {
	case thrd_nomem:
		fprintf(stderr, "Not enough memory to create %s.\n", name);
		abort();
	case thrd_error:
		fprintf(stderr, "Could not create %s.\n", name);
		abort();
	}
}

int
main()
{
	setlocale(LC_ALL, "");
	thrd_t thr[1 + n_rndrs + n_elves];
	start_thread(&thr[0], start_santa, nullptr, "Santa");
	size_t ids[n_ids];
	for (size_t i = 0; i < n_ids; i++)
		ids[i] = i;
	for (size_t i = 0; i < n_rndrs; i++)
		start_thread(&thr[i + 1], deer_start, &ids[i], "Reindeer");
	for (size_t i = 0; i < n_elves; i++)
		start_thread(&thr[i + n_rndrs + 1], start_elf, &ids[i], "Elf");
	for (size_t i = 0; i < n_rndrs + n_elves + 1; i++) {
		int result;
		thrd_join(thr[i], &result);
		if (result == EXIT_FAILURE) {
			fprintf(stderr, "Thread %zu failed.\n", i);
			return EXIT_FAILURE;
		}
	}
}
