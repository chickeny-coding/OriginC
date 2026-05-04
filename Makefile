all: ./dst

./ogc: ./main.c ./errors.h ./tokenizer.c ./treeizer.c ./assembler.c
	gcc $< -o $@ -std=c2x -Wall -Wextra -Werror

./dst.s: ./src.og ./ogc
	./ogc $< $@

./dst: ./dst.s
	gcc $^ -o $@

clean:
	rm ./ogc ./dst.s ./dst
