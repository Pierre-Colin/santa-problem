#include <threads.h>

struct semaphore
{
	mtx_t mtx;
	cnd_t cond;
	unsigned count;
};

[[__nodiscard__]] int sem_init(struct semaphore sem[static 1], int count);
void sem_destroy(struct semaphore sem[static 1]);

[[__nodiscard__]]
int sem_increment(struct semaphore sem[static 1], unsigned n);

[[__nodiscard__]] int sem_decrement(struct semaphore sem[static 1]);
