symnmf: symnmf.o symnmf.h
	gcc -o symnmf symnmf.o

symnmf.o: symnmf.c
	gcc -c -ansi -Wall -Wextra -Werror -pedantic-errors symnmf.c