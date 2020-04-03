sources = $(wildcard *.c)
objects = $(addsuffix .o, $(basename $(sources)))
target = lab2

$(target) : $(objects)
	gcc -o $(target) $(objects)

%.o : %.c
	gcc -c $< -o $@
clean:
	rm $(target) $(objects)
