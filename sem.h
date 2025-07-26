#include <threads.h>

struct semaphore
{
	mtx_t mtx;
	cnd_t cond;
	unsigned count;
};

[[nodiscard]] int sem_init(struct semaphore sem[static 1], int count);
void sem_destroy(struct semaphore sem[static 1]);
[[nodiscard]] int sem_increment(struct semaphore sem[static 1], unsigned n);
[[nodiscard]] int sem_decrement(struct semaphore sem[static 1]);
