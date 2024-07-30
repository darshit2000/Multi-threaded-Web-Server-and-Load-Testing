all: load_generator

load_generator: load_generator.c
	gcc load_generator.c -o load_generator -g -lpthread

.PHONY: clean

clean:
	rm -rf load_generator a.out load_gen.log
