efm: efmsource/*.c
	gcc -g -Wall -m64 -Iefminclude $^ -lm -o $@
