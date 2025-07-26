CFLAGS=-std=c23 -pedantic -Wall -Wextra -O3 -flto
OBJ=deer.o elves.o fail.o hangar.o program.o santa.o sem.o sleigh.o

program: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o program $(OBJ)

deer.o: deer.c config.h fail.h santa.h sem.h sleigh.h
elves.o: elves.c config.h fail.h elves.h santa.h sem.h
fail.o: fail.c
hangar.o: hangar.c config.h
help.o: help.c config.h sem.h
program.o: program.c config.h elves.h santa.h
santa.o: santa.c elves.h fail.h santa.h sem.h sleigh.h
sem.o: sem.c sem.h
sleigh.o: sleigh.c config.h sleigh.h

clean:
	rm -f program $(OBJ)

.PHONY: clean
