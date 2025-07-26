Santa Claus Problem
===================

This is an ISO C23 solution to John Trono's [Santa Claus
Problem](https://doi.org/10.1145/187387.187391).

The problem basically goes as follows.  Santa Claus spends most time sleeping
and may be woken up by two things:

* all reindeers are back from vacation, in which case the last reindeer to come
  back wakes him up *before harnessing*, and Santa delivers presents after all
  reindeers are harnessed;

* enough elves have issues building toys to form a help group of a fixed size
  waking Santa up and then going back to work before another help group can
  form.

If both conditions arise at the same time, the reindeers take priority.  The
crux of the problem is to avoid deadlocks despite the problem being full of
agents waiting for each other.

Building
--------

This program needs a conforming hosted C23 implementation that doesn't define
`__STDC_NO_THREADS__`.  GCC 13 should support all C23 features used in the
program.

Though the program doesn't require a POSIX environment, the included `Makefile`
is the preferred way to build it.  It calls the compiler as `cc -std=c23`.
Simply use something like:

```shell
$ make -j4
```

The file `config.h` contains some integer constants you can change.

Possible improvements
---------------------

This solution makes heavy use of custom-made semaphores.  It might be possible
to replace some of these semaphores with more-efficient synchronization
devices.

The main motivator for this project was to train myself to reason about the
concurrent C execution model.  Due to its complexity, this is advancing at
snail pace, and I am not even happy with its current structure (it's full of
pompous tables).  This is why no draft of it is currently included.
