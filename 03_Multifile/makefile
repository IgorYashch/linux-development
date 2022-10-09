GENERATES = prog prog-a prog-so README liboutput_static.a liboutput.so
TRASH = *.o *~ o.* tests
ARFLAGS = -rcs
CFLAGS = -fPIC

all:    README prog

prog:   const.o fun.o prog.o

prog-a: prog.o liboutput_static.a
	cc -L. $< -loutput_static -o $@

prog-so: prog.o liboutput.so
# 	cc -L. $< -loutput -o $@
	cc $(CFLAGS) -L. prog.o -loutput -o $@

README: prog
	./$< 2> $@

fun.o:  outlib.h

liboutput_static.a: fun.o const.o
	ar $(ARFLAGS) $@ $^

liboutput.so: fun.o const.o
	cc -shared $(CFLAGS) $^ -o $@


clean: distclean
	rm -rf $(TRASH)

distclean:
	rm -rf $(GENERATES)


test: prog prog-a prog-so
	mkdir -p tests

	for prog in $^; do\
		LD_LIBRARY_PATH=`pwd` ./$$prog > tests/$$prog-0.txt;\
	done

	for prog in $^; do\
		LD_LIBRARY_PATH=`pwd` ./$$prog hello > tests/$$prog-1.txt;\
	done

	for prog in $^; do\
		LD_LIBRARY_PATH=`pwd` ./$$prog hello beautiful world  > tests/$$prog-3.txt;\
	done


	for num in 0 1 3; do\
		cmp tests/prog-$$num.txt tests/prog-a-$$num.txt &&\
		cmp tests/prog-$$num.txt tests/prog-so-$$num.txt;\
	done
