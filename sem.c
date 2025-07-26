#include <threads.h>

#include "sem.h"

[[nodiscard]]
int
sem_init(struct semaphore sem[const restrict static 1], const int count)
{
	if (mtx_init(&sem->mtx, mtx_plain) == thrd_error)
		return thrd_error;
	const int result = cnd_init(&sem->cond);
	if (result != thrd_success) {
		mtx_destroy(&sem->mtx);
		return result;
	}
	sem->count = count;
	return thrd_success;
}

void
sem_destroy(struct semaphore sem[const restrict static 1])
{
	cnd_destroy(&sem->cond);
	mtx_destroy(&sem->mtx);
}

[[nodiscard]]
int
sem_increment(struct semaphore sem[const restrict static 1], const unsigned n)
{
	if (mtx_lock(&sem->mtx) == thrd_error)
		return thrd_error;
	sem->count += n;
	while (mtx_unlock(&sem->mtx) == thrd_error)
		thrd_yield();
	cnd_signal(&sem->cond);
	return thrd_success;
}

[[nodiscard]]
int
sem_decrement(struct semaphore sem[const restrict static 1])
{
	if (mtx_lock(&sem->mtx) == thrd_error)
		return -1;
	while (!sem->count)
		if (cnd_wait(&sem->cond, &sem->mtx) == thrd_error) {
			while (mtx_unlock(&sem->mtx) == thrd_error)
				thrd_yield();
			return -1;
		}
	const unsigned new_count = --sem->count;
	while (mtx_unlock(&sem->mtx) == thrd_error)
		thrd_yield();
	if (new_count)
		cnd_signal(&sem->cond);
	return new_count;
}
