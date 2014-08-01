#include<iostream>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/socket.h>
#include<unistd.h>
#include<sys/un.h>
#include<string.h>
#include<stdlib.h>
#include<opencv/cv.h>
#include<opencv/highgui.h>
#include<opencv2/opencv.hpp>
//#include<opencv2/ocl/ocl.hpp>
#include<pthread.h>
using namespace std;
using namespace cv;

//include




#define CAMSIZE_X 1920
#define CAMSIZE_Y 1080
#define FRAME_SPEED	8
//캠 설정


#define TEMPLATE_WIDTH 180
#define TEMPLATE_HEIGHT 180
//콥터 영역

#define ROW_CUT1 150
#define ROW_CUT2 960
#define COL_CUT1 150
#define COL_CUT2 1770
#define X_VALUE true
#define Y_VALUE false
//Values in Return_Corner Function.



#define NOTFOUND false
#define TRACKING true

#define CAPTURE_FRAME 20
#define FIND_THRESHOLD 10 

#define DEBUG 0
#define IMSHOW 0 
#define RADIUS 0

#define COPTER2 1
//Debuging tools
