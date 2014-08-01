#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h> 
#include <sys/syscall.h>
#include <sys/socket.h> 
#include <sys/un.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <stdbool.h>
#include <signal.h>
#include <sched.h>
#include <errno.h>

#include "mCopter.h"
#include "mSignal.h"

#define PROCESS_SCHED
#define SIGNAL_WAIT

#define TASK_NUMBER		10
#define PACKET_NUMBER		20
#define BUF_SIZE		255

#define THREAD_MODE		-1

#define CPU_NUMBER		4
#define CPU_NO_COM		0		// Communication CPU Affinity No
#define CPU_NO_CAM		0
#define handle_error_en(en, msg) \
	        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

struct taskInfo{
	pid_t pid;
	char name[100];
}taskList[TASK_NUMBER];

enum TASK_TYPE {P_MAIN, P_CAMERA, TW_COPTER, TR_CAMERA, TR_ANDROID, TW_ANDROID};
char camera_buf[PACKET_NUMBER][BUF_SIZE];
char android_buf[PACKET_NUMBER][BUF_SIZE];

char *copterZigbeePath = "/dev/ttyUSB0";
char *cameraPipePath = "./monzoPipe";
char *cameraProcessPath = "./cameraProcess";
int androidSocketPort = 9999;

int parsing(char buf[][BUF_SIZE], char *str, char tok);
void saveAndroidData(char buf[][BUF_SIZE], struct Copter copter[]);
void saveCameraData(char buf[][BUF_SIZE], struct Copter copter[]);
bool setCPUAffinity(pid_t pid, int cpuNo);
int packetNumber;

/*** Process Management Function***/
bool createProcess(int index, char *pn, int cpuNo){
	char pwd[BUF_SIZE];
	pid_t pid;

	pid = fork();
	if(pid < 0){
		printf("Create Child Process Failed\n");
		return false;
	}
	else if(pid == 0) ;
	else{
		taskList[index].pid = pid;
		strcpy(taskList[index].name, pn);
		setCPUAffinity(pid, cpuNo);
		return true;
	}

	if(pn[0] == '.'){			// Relative Path
		getcwd(pwd, BUF_SIZE);
		strcat(pwd, pn+1);
	}
	else						// Absolute Path
		strcpy(pwd, pn);

	execl(pwd, pwd, NULL);
	return true;
}

static void signalCameraRestart(int sig){
	int status = -999;
	pid_t pid = taskList[P_CAMERA].pid;

	if(pid == -1){
		printf("Setting Camera Process PID\n");
		return;
	}

	while(pid != wait(&status));
	createProcess(P_CAMERA, cameraProcessPath, CPU_NO_CAM);
}

void signalHandler(void *func){
	signal(SIGCHLD, func);
}

/*** CPU Affinity Function***/
inline pid_t gettid(){
	return syscall(__NR_gettid);
}

inline void setTaskInfo(int index, char *name){
	taskList[index].pid = gettid();
	strcpy(taskList[index].name, name);
}

int getTaskInfoIndex(pid_t pid){
	int i;
	for(i=0; i<TASK_NUMBER; i++)
		if(taskList[i].pid == pid)
			return i;
	return -1;
}

bool setCPUAffinity(pid_t pid, int cpuNo){
#ifdef PROCESS_SCHED
	cpu_set_t mask;
	int mode, err, cnt=0;

	CPU_ZERO(&mask);
	CPU_SET(cpuNo, &mask);

	if( (mode = (int)pid) == THREAD_MODE) pid = gettid();

	// pid is -1: Thread CPU Affinity, else Process CPU Affinity
	while(1) { 
		if( (err = pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask)) == 0 ) break;
		/*
		if(mode == THREAD_MODE)
			if( (err = pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask)) == 0 ) break;
		else
			if( (err = sched_setaffinity(pid, sizeof(mask), &mask)) == 0){ printf("suceess\n"); break;}
*/
		if(cnt++ > 10000){
			char buf[BUF_SIZE];
			sprintf(buf, "%s Thread CPU Set Affinity Failed", taskList[getTaskInfoIndex(pid)].name);
			handle_error_en(err, buf); 
			return false;
		}
	}

	printf("%s Thread CPU:%d Affinity Succeed\n", taskList[getTaskInfoIndex(pid)].name, cpuNo);
#endif
	return true;
}

/*** copter Write Thread Function
 *
 */
void *t_CopterWriteThread(void *fd){
	char buf[BUF_SIZE];
	int sockfd = *((int*)fd);

	setTaskInfo(TW_COPTER, "CopterWriteThread");
	setCPUAffinity(THREAD_MODE, CPU_NO_COM);

	while (1){
		waitSignal_Copter();
		//pthread_cond_wait(&gCopterCond, &gCopterMutex);
		print_copter_info(copter[0]);
		sendPacketToCopter(sockfd, copter);
	}
	close(sockfd);
	printf("Exit %s\n", taskList[TW_COPTER].name);
}

/*** camera Read Thread Function
 *
 */
void *t_CameraReadThread(void *fd)
{
	char buf[BUF_SIZE];
	int pipeFD = *((int*)fd);

	setTaskInfo(TR_CAMERA, "CameraReadThread");
	setCPUAffinity(THREAD_MODE, CPU_NO_COM);

	while (1){
		memset(buf, 0, BUF_SIZE);
		if (read(pipeFD, buf, BUF_SIZE) <= 0) break;

		parsing(camera_buf, buf, '&');
		saveCameraData(camera_buf, copter);
	
	}

	//cameraExit(pipeFD, cameraPipePath);
	printf("Exit %s\n", taskList[TR_CAMERA].name);
}

/*** android Read Thread Function
 *
 */
void *t_AndroidReadThread(void *fd){
	char buf[BUF_SIZE];
	int client_sockfd = *((int*)fd);

	setTaskInfo(TR_ANDROID, "AndroidReadThread");
	setCPUAffinity(THREAD_MODE, CPU_NO_COM);

	while (1){
		memset(buf, 0, BUF_SIZE);
		if (read(client_sockfd, buf, BUF_SIZE) <= 0) break;
		printf("Recv from Android Data: %s", buf);

		parsing(android_buf, buf, '&');
		//saveAndroidData(android_buf, copter);
		setCopterCommand(copter, atoi(android_buf[0])-1, atoi(android_buf[1]));
	}

	close(client_sockfd);
	printf("Exit %s\n", taskList[TR_ANDROID].name);

}

/*** android Write Thread Function
 *
 */
void *t_AndroidWriteThread(void *fd){
	char buf[BUF_SIZE];
	int client_sockfd = *((int*)fd);
	pid_t pid = taskList[TW_ANDROID].pid;

	setTaskInfo(TW_ANDROID, "AndroidWriteThread");
	setCPUAffinity(THREAD_MODE, CPU_NO_COM);

	while (1){
		memset(buf, 0, BUF_SIZE);
		write(client_sockfd, buf, sizeof(buf));
		printf("Write to Android Data: %s\n", buf);
	}

	close(client_sockfd);
	printf("Exit %s\n", taskList[TW_ANDROID].name);
}

/*** android Server2Client Connection Function
 *
 * return client socket File Descriptor
 */
int AndroidClientConnection(int server_sockfd){
	struct sockaddr_in clientaddr;
	int client_sockfd, client_len;

	client_len = sizeof(clientaddr);

	client_sockfd = accept(server_sockfd, (struct sockaddr *)&clientaddr, &client_len);
	if (client_sockfd == -1){
		perror("Accept error: ");
		exit(0);
	}
	else
		printf("Android Connection OK, Start Android Thread!!\n");

	return client_sockfd;
}

/*** copter Connection Zigbee Initial Function
 *
 * return copter zigbee File Descriptor
 */
int copterInit(char *path){
	int fd;

	fd = open(copterZigbeePath, O_RDWR);
	if (fd < 0){
		printf("%s Connection Error!!\n", copterZigbeePath);
		exit(0);
	}
	return fd;
}

/*** android Communication Socket Initial Function
 *
 * return android server Socket Descriptor
 */
int androidInit(int port){
	struct sockaddr_in serveraddr;
	int serverSocketFD, state;

	if ((serverSocketFD = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("socket error : ");
		exit(0);
	}

	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(port);

	state = bind(serverSocketFD, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if (state == -1){
		perror("bind error : ");
		exit(0);
	}

	state = listen(serverSocketFD, 5);
	if (state == -1){
		perror("listen error : ");
		exit(0);
	}

	return serverSocketFD;
}


/*** camera Communication Socket Initial Function
 *
 * return camera server Socket Descriptor
 */
int cameraInit(char *path){
	int fd;
	if (mkfifo(cameraPipePath, 0666) == -1){
		printf("fail to make fifo()\n");
		return -1;
	}

	if ((fd = open(cameraPipePath, O_RDWR)) < 0){
		printf("fail to open fifo()\n");
		return -1;
	}

	return fd;

#if 0
	/*** Unix Domain Socket ***/
	struct sockaddr_un serveraddr;
	int serverSocketFD, state;

	if (access(path, F_OK) == 0){
		printf("%s Access Error!!\n", path);
		unlink(path);
		exit(0);
	}

	if ((serverSocketFD = socket(AF_UNIX, SOCK_STREAM, 0)) < 0){
		perror("socket error : ");
		exit(0);
	}

	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sun_family = AF_UNIX;
	strcpy(serveraddr.sun_path, path);

	state = bind(serverSocketFD, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if (state == -1){
		perror("bind error : ");
		exit(0);
	}

	state = listen(serverSocketFD, 5);
	if (state == -1){
		perror("listen error : ");
		exit(0);
	}

	return serverSocketFD;
#endif
}

/*** copter Communication Socket Exit Function
 *
 */
void copterExit(int fd){
	close(fd);
}
/*** android Communication Socket Exit Function
 *
 */
void androidExit(int fd){
	close(fd);
}

/*** camera Communication Socket Exit Function
 *
 */
void cameraExit(int fd, char *path){
	close(fd);
	unlink(path);
}


int sendPacketToCopter(int fd, struct Copter copter[]){
	char buf[BUF_SIZE] = {0,};
	int i;

	for(i=0; i<COPTER_NUMBER; i++){
		//sprintf(buf, "[%d&%d&%d&%d&%d&%d]", copter[i].id, copter[i].roll, copter[i].pitch, copter[i].yaw, copter[i].throttle, copter[i].func1, copter[i].func2);
		sprintf(buf, "$M<%c%c%c%c%c%c%c%c%c%c%c", 8, 210, 
				(copter[i].roll)&0xFF, (copter[i].roll>>8)&0xFF, 
				(copter[i].pitch)&0xFF, (copter[i].pitch>>8)&0xFF, 
				(copter[i].yaw)&0xFF, (copter[i].yaw>>8)&0xFF, 
				(copter[i].throttle)&0xFF, (copter[i].throttle>>8)&0xFF, 210);
		write(fd, buf, strlen(buf));
	}
}

void saveAndroidData(char buf[][BUF_SIZE], struct Copter copter[]){
	copter[0].id = atoi(buf[0]);
	copter[0].yaw = atoi(buf[1]);
	copter[0].roll = atoi(buf[2]);
	copter[0].pitch = atoi(buf[3]);
	copter[0].throttle = atoi(buf[4]);
	copter[0].func1 = atoi(buf[5]);
}

void saveCameraData(char buf[][BUF_SIZE], struct Copter copter[]){
	int i, j, copterPacketNumber = 11; //sizeof(struct Copter)/sizeof(int);

	for(i=0; i<COPTER_NUMBER; i++){
		copter[i].id	 = atoi(buf[i*copterPacketNumber + 0]);
		copter[i].seen	 = atoi(buf[i*copterPacketNumber + 1]);
		copter[i].rx	 = atoi(buf[i*copterPacketNumber + 2]);
		copter[i].ry	 = atoi(buf[i*copterPacketNumber + 3]);
		copter[i].sx	 = atoi(buf[i*copterPacketNumber + 4]);
		copter[i].sy	 = atoi(buf[i*copterPacketNumber + 5]);
		copter[i].az	 = atoi(buf[i*copterPacketNumber + 6]);
		copter[i].head	 = atoi(buf[i*copterPacketNumber + 7]);
		copter[i].vx	 = atoi(buf[i*copterPacketNumber + 8]);
		copter[i].vy	 = atoi(buf[i*copterPacketNumber + 9]);
		copter[i].dir	 = atoi(buf[i*copterPacketNumber + 10]);
	}
}

int parsing(char buf[][BUF_SIZE], char *str, char tok){
	char *startPoint;
	int start, len, buf_index, i;

	start = len = buf_index = i = 0;
	startPoint = str;

	while (1){
		if (*str == tok || *str == (int)NULL){
			strncpy(buf[buf_index], startPoint + start, len);
			buf[buf_index][len] = 0;
			start = i + 1;
			len = -1;
			buf_index++;
		}
		if (*str == (int)NULL) break;
		str++, len++, i++;
	}
	return buf_index;
}

/*** Work
 * 1. Process WriteThread -> Down CPU Usage
 *
 */

int main()
{
	pthread_t pCopterThread, pCameraThread, pAndroidReadThread, pAndroidWriteThread;
	int iCopterFD, iCameraFD, iAndroidFD, iAndroidClientFD;
	int status;

	strcpy(taskList[P_MAIN].name, "MainProcess");

	unlink(cameraPipePath);
	initCopter(copter);

	signalHandler(signalCameraRestart);

	iCopterFD = copterInit(copterZigbeePath);
	iCameraFD = cameraInit(cameraPipePath);
	iAndroidFD = androidInit(androidSocketPort);
	iAndroidClientFD = AndroidClientConnection(iAndroidFD);

	pthread_create(&pCopterThread, NULL, t_CopterWriteThread, (void*)&iCopterFD);
	pthread_create(&pCameraThread, NULL, t_CameraReadThread, (void*)&iCameraFD);
	pthread_create(&pAndroidReadThread, NULL, t_AndroidReadThread, (void*)&iAndroidClientFD);
	//pthread_create(&pAndroidWriteThread, NULL, t_AndroidWriteThread, (void*) &iAndroidClientFD);
	setCPUAffinity(P_MAIN, CPU_NO_COM);
	//createProcess(P_CAMERA, cameraProcessPath, CPU_NO_CAM);

	// while is OS: setAffinity & process management
	while (1) {
		startFormation(copter);
	}
	return 0;
}

