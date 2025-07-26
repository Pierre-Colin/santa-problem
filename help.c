#include <stddef.h>
#include <threads.h>

#include "config.h"
#include "sem.h"

static struct semaphore enter, ready;
static once_flag flag;
static bool corrupted;
static size_t members[n_group];

static void
init()
{
	switch (sem_init(&enter, n_group)) {
	case thrd_nomem:
		goto fail_nomem;
	case thrd_error:
		goto fail_error;
	}
	switch (sem_init(&ready, 0)) {
	case thrd_nomem:
		sem_destroy(&enter);
		goto fail_nomem;
	case thrd_error:
		sem_destroy(&enter);
		goto fail_error;
	}
	return;
fail_nomem:
	fputs("Not enough memory to initialize the help group.\n", stderr);
	corrupted = true;
	return;
fail_error:
	fputs("Could not initialize the help group.\n", stderr);
	corrupted = true;
}

/* TODO
   help_join() must wake Santa up, use an atomic counter to detect last elf
   ready to wait for help with aquire-release semantics.

   help_all() must call elf_help() on all members. */

[[nodiscard]]
int
help_join(const size_t id)
{
	const int count = sem_decrement(&enter);
	if (count < 0)
		return -1;
	members[count] = id;
	if (count) {
		if (sem_increment(&ready, 1) < 0)
			return -1;
}

[[nodiscard]]
int
help_clear()
{
	return sem_increment(&enter, n_group);
}
