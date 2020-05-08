# look into gdb flags
# sources = $(wildcard *.c)
# objects = $(addsuffix .o, $(basename $(sources)))
# flags = -g -W -Wall
# target = pseudo-shell

# $(target) : $(objects)
# 	gcc -o $(target) $(objects)

# %.o : %.c
# 	gcc -c $< -o $@
# clean:
# 	rm $(target) $(objects


flags = -g -W -Wall

all: part2.out

part2.out: part2.c
	gcc $(flags) -o part2.out part2.c

# part2: part2.c
# 	gcc -o part2 part2.c

clean:
	rm part2.out