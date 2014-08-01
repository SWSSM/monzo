#include "mSignal.h"
//#define SIGNAL_USE

pthread_cond_t gCopterCond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t gCopterMutex = PTHREAD_MUTEX_INITIALIZER;
int packetNumber;

void waitSignal_Copter(){
#ifdef SIGNAL_USE
	pthread_cond_wait(&gCopterCond, &gCopterMutex);
#else
	while(packetNumber <= 0);
	packetNumber--;
#endif
}

/***
 * delay: ms
 */

void sendSignal_Copter(int delay){
#ifdef SIGNAL_USE
	pthread_cond_signal(&gCopterCond);
#else
	packetNumber++;
#endif
	if(delay > 0) usleep(delay * 1000);
}
