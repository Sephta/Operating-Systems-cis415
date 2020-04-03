sources = $(wildcard *.c)
objects = $(addsuffix .o, $(basname $(sources)))

target = psuedo-shell

$(target) : $(objects)
	gcc -o $(target) $(objects)

%.o : %.c
	gcc -c $, -o $@
clean:
	rm $(target) $(objects)
