all: shared/a.out static/a.out



shared/revert_string.so: ../revert_string/revert_string.c
	gcc -shared ../revert_string/revert_string.c -o shared/revert_string.so

shared/a.out: ../revert_string/main.c shared/revert_string.so
	gcc ../revert_string/main.c shared/revert_string.so -o shared/a.out



static/librevert_string.a: ../revert_string/revert_string.c
	gcc -c ../revert_string/revert_string.c -o static/revert_string.o
	ar rcs static/librevert_string.a static/revert_string.o
	rm static/revert_string.o

static/a.out: ../revert_string/main.c static/librevert_string.a
	gcc ../revert_string/main.c -Lstatic/ -lrevert_string -o static/a.out



clean:
	rm -f shared/* static/*