#include <stdio.h>
#include <threads.h>

#include "config.h"
#include "sleigh.h"

static mtx_t lock;
static cnd_t cond;
static once_flag flag = ONCE_FLAG_INIT;
static bool corrupted;
static int count;

static void
init()
{
	int res = mtx_init(&lock, mtx_plain);
	if (res == thrd_error)
		goto fail;
	if ((res = cnd_init(&cond)) != thrd_success) {
		mtx_destroy(&lock);
		goto fail;
	}
	return;
fail:
	corrupted = true;
	switch (res) {
	case thrd_nomem:
		fputs("Not enough memory to initialize sleigh.\n", stderr);
		return;
	case thrd_error:
		fputs("Could not initialize sleigh.\n", stderr);
	}
}

[[nodiscard]]
int
harness()
{
	call_once(&flag, init);
	if (corrupted)
		return thrd_error;
	if (mtx_lock(&lock) == thrd_error)
		return thrd_error;
	const bool wake_up = ++count == n_rndrs;
	while (mtx_unlock(&lock) == thrd_error)
		thrd_yield();
	if (wake_up)
		cnd_signal(&cond);
	return thrd_success;
}

[[nodiscard]]
int
wait_for_sleigh()
{
	call_once(&flag, init);
	if (corrupted)
		return thrd_error;
	if (mtx_lock(&lock) == thrd_error)
		return thrd_error;
	while (count < n_rndrs)
		if (cnd_wait(&cond, &lock) == thrd_error) {
			while (mtx_unlock(&lock) == thrd_error)
				thrd_yield();
			return thrd_error;
		}
	count = 0;
	while (mtx_unlock(&lock) == thrd_error)
		thrd_yield();
	return thrd_success;
}
