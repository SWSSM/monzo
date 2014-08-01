#ifndef __M_COPTER_H__
#define __M_COPTER_H__

#define DIFF_THRESHOLD 15
#define Z_THRESHOLD 6
#define INTERVAL_LIMIT 55
#define Z_INTERVAL_LIMIT 35
#define FORMS_COUNT 1
#define ZIGBEE 1

#define COPTER1_MOTION 1
#define COPTER2_MOTION 3

#define Y_DEF 1500
#define R_DEF 1506
#define P_DEF 1514
#define Th_DEF 1000
#define FLYING_Th_DEF 1312
#define HB 15

/*
#define Y_DEF 1500
#define R_DEF 1495
#define P_DEF 1512
#define Th_DEF 1000
#define FLYING_Th_DEF 1305
*/

//배터리 꽉차있을때 1310    !!!
//+- 30~40

#define Y_DEF2 1500
#define R_DEF2 1510
#define P_DEF2 1505
#define Th_DEF2 1000
#define FLYING_Th_DEF2 1250


#define SPEED 7 //pitch나 roll의 변화량 find_vector()
#define ROLL_LEFT_SPEED 6   //find_vector()
#define ROLL_RIGHT_SPEED 5  //find_vector()
#define LIMIT_SPEED 25 //카메라에서보이는 콥터의 제한속도
/*** ***/
#define COPTER_NUMBER   1

#define ROLL_BREAK_POWER  2//Operate_Break()
#define PITCH_BREAK_POWER 2 //Operate_Break()
#define HOVERING_RATE 4     //Hovering()

#define DEFAULT_Z_POSITION 140
#define MIN_SPEED 2
#define BREAK_THRESHOLD 2
#define MAX_BREAK_CNT   5

struct Copter{
    int id;
	int seen;       // be seen copter: on / off
    int height;     // copter height: on / off
	int activate;   // copter activate: turn on / off

	int head;
	int vector;

	int roll;
	int pitch;
	int yaw;
	int throttle;
	int func1;
	int func2;

	int rx;         // real x
	int ry;         // real y
	int sx;         // space x
	int sy;         // space y
	float az;       // average z

    int vx;         // velocity x
    int vy;         // velocity y
    int dir;        // direction
    
    int spx;        // start_position_x
    int spy;        // start_position_y
    
    int roll_Break;
    int pitch_Break;
    int Break_cnt_p;
    int Break_cnt_r;
    int pitch_flag;
    int roll_flag;
}copter[COPTER_NUMBER];

typedef struct Form
{
	int moving_index;
	int size;

//copter1
	int flag1[15];
	int real_cx1[15];
	int real_cy1[15];
	int Z1[15];

//copter2
	int flag2[15];
	int real_cx2[15];
	int real_cy2[15];
	int Z2[15];
}Formation;

void Calibration_Head(struct Copter* copter);
void Prevent_Crash(struct Copter* copter1, struct Copter* copter2);
void apply_threshold(int* diff_x, int* diff_y, int* diff_z);
int find_vector(struct Copter* cop,int d_x, int d_y, int d_Z);
void Send_Packet(int R1, int P1, int Y1, int Th1, int F1, int F2, int R2, int P2, int Y2, int Th2, int F3, int F4, int fd);
void initCopter(struct Copter* cop);
void example_formation1_init(Formation* form1);
void Return_into_Map(struct Copter* cop);
void Reduce_Accelation(int* pitch, int* roll, int* pitch_flag, int* roll_flag);
void Operate_Break(struct Copter* cop);
void Hovering(struct Copter* cop);
void startFormation(struct Copter copter[]);
void setCopterCommand(struct Copter copter[], int copterID, int cmd);
//Z값을 우선적으로 확인 후 움직임
//d_x, d_y 값은 lookup_table을 통한 calibrated된 값을 사용
//cop.vector 정의
#endif
