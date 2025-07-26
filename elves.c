#include <stdio.h>
#include <stdlib.h>
#include <threads.h>

#include "config.h"
#include "fail.h"
#include "elves.h"
#include "santa.h"
#include "sem.h"

static struct semaphore dismiss[n_elves], panic, ready;
static once_flag dismiss_init_flag = ONCE_FLAG_INIT;
static once_flag panic_init_flag = ONCE_FLAG_INIT;
static bool dismiss_corrupted, panic_corrupted;
static size_t to_be_dismissed[n_group];

static void
init_dismiss()
{
	int result;
	for (size_t i = 0; i < n_elves; i++)
		if ((result = sem_init(&dismiss[i], 0)) != thrd_success) {
			dismiss_corrupted = true;
			for (size_t j = 0; j < i; j++)
				sem_destroy(&dismiss[j]);
			break;
		}
	switch (result) {
	case thrd_nomem:
		fputs("Not enough memory to initialize elves.\n", stderr);
		return;
	case thrd_error:
		fputs("Could not initialize elves.\n", stderr);
	}
}

[[nodiscard]]
int
dismiss_elves()
{
	call_once(&dismiss_init_flag, init_dismiss);
	if (dismiss_corrupted)
		return thrd_error;
	for (unsigned i = 0; i < n_group; i++) {
		const size_t id = to_be_dismissed[i];
		if (sem_increment(&dismiss[id], 1) < 0) {
			fprintf(stderr, "Could not dismiss elf %zu.\n", id);
			return thrd_error;
		}
	}
	return thrd_success;
}

static void
init_panic()
{
	switch (sem_init(&panic, n_group)) {
	case thrd_nomem:
		fputs("Not enough memory to initialize elves.\n", stderr);
		panic_corrupted = true;
		return;
	case thrd_error:
		fputs("Could not initialize elves.\n", stderr);
		panic_corrupted = true;
		return;
	}
	switch (sem_init(&ready, 0)) {
	case thrd_nomem:
		sem_destroy(&panic);
		fputs("Not enough memory to initialize elves.\n", stderr);
		panic_corrupted = true;
		return;
	case thrd_error:
		sem_destroy(&panic);
		fputs("Could not initialize elves.\n", stderr);
		panic_corrupted = true;
	}
}

static void
wait_and_wake_up(const size_t id)
{
	if (sem_decrement(&ready) < 0 || sem_decrement(&ready) < 0)
		fail("Elf %zu could not wait for other elves.\n", id);
	if (wake_up_santa() == thrd_error)
		fail("Elf %zu could not wake up Santa.\n", id);
}

static void
signal_ready(const size_t id)
{
	if (sem_increment(&ready, 1) < 0)
		fail("Elf %zu could not signal readiness.\n", id);
}

static void
wait_dismissal(const size_t id)
{
	if (sem_decrement(&dismiss[id]) < 0)
		fail("Elf %zu could not be dismissed.\n", id);
}

static void
clear_panic_room(const size_t id)
{
	if (sem_increment(&panic, n_group) < 0)
		fail("Elf %zu could not clear the help group.\n", id);
}

int
start_elf(void *param)
{
	const size_t id = *(const size_t *) param;
	call_once(&dismiss_init_flag, init_dismiss);
	if (dismiss_corrupted)
		thrd_exit(EXIT_FAILURE);
	call_once(&panic_init_flag, init_panic);
	if (panic_corrupted)
		thrd_exit(EXIT_FAILURE);
	for (;;) {
		const int count = sem_decrement(&panic);
		if (count < 0)
			fail("Elf %zu could not join a help group.\n", id);
		to_be_dismissed[count] = id;
		if (!count)
			wait_and_wake_up(id);
		else
			signal_ready(id);
		wait_dismissal(id);
		if (!count)
			clear_panic_room(id);
		thrd_yield();
	}
}
