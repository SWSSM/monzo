#include"mCopter.h"
#define FORM1_POINT 6

int Formation_num=0;
int test1 =0;
Formation form1;
void example_formation1_init(Formation* form)
{
	//vector = 11;
	(*form).moving_index = 0;
	
	int index=0;
	
	(*form).size = FORM1_POINT;
/*
	(*form).flag1 = (int)malloc(FORM1_POINT);
	(*form).real_cx1 = (int)malloc(FORM1_POINT);
	(*form).real_cy1 = (int)malloc(FORM1_POINT);
	(*form).Z1 = (int)malloc(FORM1_POINT);
	
	(*form).flag2 = (int)malloc(FORM1_POINT);
	(*form).real_cx2 = (int)malloc(FORM1_POINT);
	(*form).real_cy2 = (int)malloc(FORM1_POINT);
	(*form).Z2 = (int)malloc(FORM1_POINT);
*/	
	for(index =0; index<FORM1_POINT; index++)
	{
		(*form).flag1[index] = 0;
		(*form).flag2[index] = 0;
		
		(*form).real_cy1[index] = 370;
		(*form).real_cy2[index] = 710;

		(*form).Z1[index] = 120;
		(*form).Z2[index] = 120;
	}

	(*form).real_cx1[0] = 400;
	(*form).real_cx1[1] = 600;
	(*form).real_cx1[2] = 800;
	(*form).real_cx1[3] = 1000;
	(*form).real_cx1[4] = 1200;
	(*form).real_cx1[5] = 1400;
	
	(*form).real_cx2[0] = 400;
	(*form).real_cx2[1] = 600;
	(*form).real_cx2[2] = 800;
	(*form).real_cx2[3] = 1000;
	(*form).real_cx2[4] = 1200;
	(*form).real_cx2[5] = 1400;


}
