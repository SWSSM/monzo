#include "Basic.h"



#define COLOR(mat1,j,i,YMAX,YMIN,CbMAX,CbMIN,CrMAX,CrMIN) (mat1.at<Vec3b>(i,j)[0]<YMAX && mat1.at<Vec3b>(i,j)[1]>CbMIN && mat1.at<Vec3b>(i,j)[1] <CbMAX && mat1.at<Vec3b>(i,j)[2] >CrMIN && mat1.at<Vec3b>(i,j)[2]<CrMAX)


#define BUFFER_SIZE	30
#define Z_BUFFER_SIZE 15
#define MOVE_THRESHOLD 0
#define V_BUFFER_SIZE 10

#define V_MIN 2
void Copter1_init();
void Copter2_init();

struct Copter
{
	int number;
	int real_cx; int real_cy;
	int space_X; int space_Y;
	float Z_avr;
    int X_v;     int Y_v;
	//Member instances for sending to Main Process
	
    
    
    float Z;		

    int x;      int y;
    int width;  int height;
    int radius_X; float radX_avr;
	int radius_Y; float radY_avr;
	int center_x; int center_y;
	int prev_cx[V_BUFFER_SIZE];
    int prev_cy[V_BUFFER_SIZE];
    
    int Y_MAX;  int Y_MIN;
    int Cb_MAX; int Cb_MIN;
    int Cr_MAX; int Cr_MIN;

    int Y_MAX2;  int Y_MIN2;
    int Cb_MAX2; int Cb_MIN2;
    int Cr_MAX2; int Cr_MIN2;

    bool Flag;
    
    unsigned int find_count;
    unsigned int not_found_count;
	
	int radX_buff[BUFFER_SIZE];
	int radY_buff[BUFFER_SIZE];
	float Z_Buffer[Z_BUFFER_SIZE];

    Mat Picture;
    Mat Picture2;
    string Pic2_name;
    Mat Picture3;
    string Pic3_name;
    

    Mat Pic_Binary1;
    Mat Pic_Binary2;


    Mat Copter_Layer;
	
    int Head_vector;
	int vector;
	bool find_mode;
	bool highenX_flag;
	bool highenY_flag;
};

void Take_Copter(Mat Cam, Mat Cam2,  Copter& cop);
void Take_Copter_Init(Mat Cam, Mat Cam2, Mat Pic, Mat Pic2, Copter& cop);
void Copter_Color(Mat YUV, Mat* Binary1, Mat* Binary2,Copter copter1);
void Check_Copter(Copter& copter);
void Reposition_minus(Copter& cop);
void Take_Picture(Copter& copter);
void print_info(Copter cop);
void rad_buffer_init(Copter& cop);
void buffer_setting(Copter& cop, int& index);
void Return_True_Position(Copter& cop);
void Zposition(Copter& cop);
void Return_Ratio(double& ratio_X, double& ratio_Y, int cx, int cy, double X_th, double Y_th);
int Return_Corner(int cx, int cy);
double Return_Threshold_between_altitude(float rad_avr, int Corner, bool X);
void Z_buffer_init(Copter& cop);
void Z_buffer_setting(Copter& cop, int& index);
bool In_CrashDanger(Copter cop1, Copter cop2);
int Return_HeadWay(Rect box3, Rect box4);
void Save_prev(int pcx[], int pcy[]);



//////////////////////////////////////////////////////////////////////
struct Copter_Packet
{
	int number;
	int flag;
	int real_cx; 
	int real_cy;
	int space_X; 
	int space_Y;
	int Z_avr;
	int Head_Vector;
    int X_v;
    int Y_v;
    int vector;
};
void cop_packet_init(Copter_Packet& CP, Copter cop);
void Send_Packet(Copter_Packet cp1, Copter_Packet cp2, int client_sockfd);
void Socket_Data(Copter cop, Copter_Packet& cp);
