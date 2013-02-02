default:	compile

compile:
	gcc test_syscall.c -o syscall.o
	gcc test_quad.c -o quad.o
	gcc test_steal.c -o steal.o
	gcc test_swipe.c -o swipe.o
	gcc test_forcewrite.c -o forcewrite.o
	gcc test_zombify.c -o zombify.o
	gcc test_myjoin.c -o myjoin.o

clean :
	rm -f *.o
