#include <stdio.h>
#include <stdlib.h>
#include <threads.h>

#include "elves.h"
#include "fail.h"
#include "santa.h"
#include "sem.h"
#include "sleigh.h"

[[nodiscard]] int deer_clear();
[[nodiscard]] int hang_check();

static struct semaphore wakeup_signal;
static once_flag init_flag = ONCE_FLAG_INIT;
static bool wakeup_signal_corrupted;

static void
init_wakeup_signal()
{
	switch (sem_init(&wakeup_signal, 0)) {
	case thrd_nomem:
		fputs("Not enough memory to initialize Santa.\n", stderr);
		wakeup_signal_corrupted = true;
		return;
	case thrd_error:
		fputs("Could not initialize Santa.\n", stderr);
		wakeup_signal_corrupted = true;
	}
}

[[nodiscard]]
int
wake_up_santa()
{
	call_once(&init_flag, init_wakeup_signal);
	if (wakeup_signal_corrupted || sem_increment(&wakeup_signal, 1) < 0)
		return thrd_error;
	return thrd_success;
}

static void
count_action()
{
	static unsigned long long count = 0;
	if (++count % 1000000 == 0)
		printf("%llu actions.\n", count);
}

int
start_santa(void *)
{
	call_once(&init_flag, init_wakeup_signal);
	if (wakeup_signal_corrupted)
		thrd_exit(EXIT_FAILURE);
	for (;;) {
		while (sem_decrement(&wakeup_signal) < 0)
			thrd_yield();
		const int full = hang_check();
		#if __STDC_NO_ATOMICS__ == 1
		if (full < 0)
			fail("Santa could not check hangar fullness.\n");
		#endif
		if (full) {
			while (wait_for_sleigh() == thrd_error)
				thrd_yield();
			count_action();
			if (deer_clear() == thrd_error)
				thrd_exit(EXIT_FAILURE);
		} else {
			count_action();
			if (dismiss_elves() == thrd_error)
				thrd_exit(EXIT_FAILURE);
		}
		thrd_yield();
	}
}
