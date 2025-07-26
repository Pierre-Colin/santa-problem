#if __STDC_NO_ATOMICS__ != 1
#include <stdatomic.h>
#else
#include <stdio.h>
#include <threads.h>
#endif

#include "config.h"

#if __STDC_NO_ATOMICS__ == 1
static mtx_t mtx;
static size_t count;
static bool corrupted;
static once_flag flag = ONCE_FLAG_INIT;
#else
static atomic_size_t count;
#endif

#if __STDC_NO_ATOMICS__ == 1
static void
init()
{
	if (mtx_init(&mtx, mtx_plain) == thrd_error) {
		fputs("Could not initialize hangar.\n", stderr);
		corrupted = true;
	}
}
#endif

[[nodiscard]]
int
hang_enter()
{
	#if __STDC_NO_ATOMICS__ == 1
	call_once(&flag, init);
	if (corrupted || mtx_lock(&mtx) == thrd_error)
		return -1;
	const bool result = ++count == n_rndrs;
	while (mtx_unlock(&mtx) == thrd_error)
		thrd_yield();
	return result;
	#else
	const size_t n = atomic_fetch_add_explicit(&count, 1,
	                                           memory_order_relaxed);
	return n + 1 == n_rndrs;
	#endif
}

[[nodiscard]]
int
hang_check()
{
	#if __STDC_NO_ATOMICS__ == 1
	call_once(&flag, init);
	if (corrupted || mtx_lock(&mtx) == thrd_error)
		return -1;
	const bool result = count == n_rndrs;
	count = 0;
	while (mtx_unlock(&mtx) == thrd_error)
		thrd_yield();
	return result;
	#else
	size_t exp = n_rndrs;
	return atomic_compare_exchange_strong_explicit(&count, &exp, 0,
	                                               memory_order_relaxed,
	                                               memory_order_relaxed);
	#endif
}
