#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>

#include "config.h"
#include "fail.h"
#include "santa.h"
#include "sem.h"
#include "sleigh.h"

[[nodiscard]] int hang_enter();

static struct semaphore dismiss[n_rndrs];
static once_flag flag = ONCE_FLAG_INIT;
static bool dismiss_corrupted;

static void
init()
{
	int result;
	for (size_t i = 0; i < n_rndrs; i++)
		if ((result = sem_init(&dismiss[i], 0)) != thrd_success) {
			dismiss_corrupted = true;
			for (size_t j = 0; j < i; j++)
				sem_destroy(&dismiss[j]);
			break;
		}
	switch (result) {
	case thrd_nomem:
		fputs("Not enough memory to initialize reindeers.\n", stderr);
		return;
	case thrd_error:
		fputs("Could not initialize reindeers.\n", stderr);
	}
}

[[nodiscard]]
int
deer_clear()
{
	call_once(&flag, init);
	for (size_t i = 0; i < n_rndrs; i++)
		if (sem_increment(&dismiss[i], 1) < 0) {
			fprintf(stderr,
			        "Could not dismiss reindeer %zu.\n",
			        i);
			return thrd_error;
		}
	return thrd_success;
}

int
deer_start(void * const restrict param)
{
	const size_t id = *(const size_t *) param;
	call_once(&flag, init);
	for (;;) {
		const int result = hang_enter();
		#ifdef __STDC_NO_ATOMICS__
		if (result < 0)
			fail("Reindeer %zu could not enter the hangar.\n", id);
		#endif
		if (result && wake_up_santa() == thrd_error)
			fail("Reindeer %zu could not wake Santa up.\n", id);
		if (harness() == thrd_error)
			fail("Reindeer %zu could not harness.\n", id);
		if (sem_decrement(&dismiss[id]) < 0)
			fail("Reindeer %zu could not be dismissed.\n", id);
		thrd_yield();
	}
}
