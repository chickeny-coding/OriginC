all: ogc

ogc: main.c errors.h tokenizer.c treeizer.c assembler.c
	gcc $< -o $@ -std=c2x

clean:
	rm ogc
