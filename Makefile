all:
	gcc mainProcess.c mCopter.c mSignal.c -lpthread

clean:
	rm a.out monzoPipe tags
