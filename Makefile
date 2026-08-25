GCC := gcc
CFLAGS := -Wall -Wextra -g

todo: main.c todo_list.c
	$(GCC) -o $@ $^

clean:
	rm -rf todo 



