all:
	@ gcc my_malloc.c -o my_malloc
run: all
	@ ./my_malloc
clean:
	@ rm my_malloc
