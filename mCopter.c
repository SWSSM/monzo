#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "mCopter.h"

void Return_into_Map(struct Copter* cop)
{
	int x = (*cop).rx;
	int y = (*cop).ry;
	if(x<80 || x>1840 || y>1000 || y<80)
	{
		(*cop).roll = R_DEF;
		(*cop).pitch = P_DEF;
		(*cop).yaw = Y_DEF;
		(*cop).throttle = Th_DEF-50;
	}
}


void Operate_Break(struct Copter* cop)
{
	int des_vector		= cop->vector;
	int current_vector	= cop->dir;
	int speed_X		= cop->vx;
	int speed_Y		= cop->vy;
	

	int Roll_need=0;
	int Pitch_need=0;
	
	


	if(des_vector != current_vector)//목표지점 방향과 콥터의 이동방향이 다름
	{
		printf("BREAK_TEST : %d\n",des_vector);
		
		if(abs(speed_X)<3) speed_X =0;
		if(abs(speed_Y)<3) speed_Y =0;

		switch(des_vector)
		{
			//case 11:Roll_need = -speed_Y;
			//	Pitch_need = -speed_X;
			//	break;
			case 1:if(speed_Y != 0)
			       {
				       Roll_need = -speed_Y;
			       }//Y축이 다를때

			       if(speed_X <=0)
			       {
				       Pitch_need = -speed_X;
			       }//X축방향이 반대일때
					/*
			       if(speed_X>LIMIT_SPEED)
			       {
				       Pitch_need = -(speed_X - LIMIT_SPEED);
			       }//속도가 너무 빠를때
*/
			       break;
			
			case 2:
				  /* if(speed_Y<0 && speed_X>0)
			       {
					if(speed_X > LIMIT_SPEED)
						Pitch_need = -(speed_X -LIMIT_SPEED);
					if(speed_Y < -LIMIT_SPEED)
						Roll_need = (-speed_Y)-LIMIT_SPEED;
			       }//방향은 비슷하나 틀어져있음
			       //속도가 너무 빠를때
				*/
			       if(speed_Y>0)
			       {
						Roll_need = -speed_Y;		
			       }//Y축 방향이 반대
			       if(speed_X<0)
			       {
				       Pitch_need = -speed_X;
			       }//X축 방향이 반대
	
			       break;

			case 3:/*if(speed_Y < (-LIMIT_SPEED))
			       {
					Roll_need = (-speed_Y)-LIMIT_SPEED;
			       }//Y축은 올바르나 속도가 빠를때*/
			       if(speed_Y >=0)
			       {
					Roll_need = -speed_Y;
			       }//Y축이 반대
			       if(speed_X != 0)
			       {
				       Pitch_need = -speed_X;
			       }//X가 0이 아닐때
			       break;

			case 4:/*if(speed_Y<0 && speed_X<0)
			       {
					if(speed_X < -LIMIT_SPEED)
						Pitch_need = (-speed_X)-LIMIT_SPEED;
					if(speed_Y < -LIMIT_SPEED)
						Roll_need = (-speed_Y)-LIMIT_SPEED;
			       }//방향은 올바르나 속도가 빠를때*/
			       if(speed_Y>0)
			       {
				       Roll_need = -speed_Y;
			       }//Y가 반대일때
			       if(speed_X>0)
			       {
				       Pitch_need = -speed_X;
			       }//X가 반대일때
			       break;

			case 5:/*if(speed_X<0)
			       {
				       if(speed_X < -LIMIT_SPEED)
				       {
					       Pitch_need = (-speed_X)-LIMIT_SPEED;
				       }
			       }*/
			       if(speed_X>0)
			       {
				       Pitch_need = -speed_X;
			       }
			       if(speed_Y !=0)
			       {
				       Roll_need = -speed_Y;
			       }
			       break;

			case 6:/*if(speed_X<0 && speed_Y >0)
			       {
					if(speed_X < -LIMIT_SPEED)
						Pitch_need = (-speed_X)-LIMIT_SPEED;
					if(speed_Y > LIMIT_SPEED)
						Roll_need = -(speed_Y - LIMIT_SPEED);
			       }*/
			       if(speed_X>0)
			       {
				       Pitch_need = -speed_X;
			       }
			       if(speed_Y<0)
			       {
				       Roll_need = -speed_Y;
			       }
				break;
			
			case 7:/*if(speed_Y >0)
			       {
				       if(speed_Y > LIMIT_SPEED)
				       {
					       Roll_need = -(speed_Y - LIMIT_SPEED);
				       }
			       }*/
			       if(speed_X != 0)
			       {
				       Pitch_need = -speed_X;
			       }
			       if(speed_Y <0)
			       {
				       Roll_need = -speed_Y;
			       }
			       break;

			case 8:/*if(speed_Y>0 && speed_X >0)
			       {
				       if(speed_Y > LIMIT_SPEED)
					       Roll_need = -(speed_Y - LIMIT_SPEED);
				       if(speed_X > LIMIT_SPEED)
					       Pitch_need = -(speed_X - LIMIT_SPEED);
				
			       }*/
			       if(speed_Y <0)
			       {
				       Roll_need = -speed_Y;
			       }
			       if(speed_X <0)
			       {
				       Pitch_need = -speed_X;
			       }
			       break;
			default:Pitch_need =-speed_X;
					Roll_need =-speed_Y;
					break;
		}
	}
	if(Roll_need > 0){
		if(cop->Break_cnt_r<=0) cop->Break_cnt_r =0;
		cop->Break_cnt_r++;
		if(cop->Break_cnt_r>BREAK_THRESHOLD) Roll_need=0;	
	}
	else if(Roll_need <0){
		if(cop->Break_cnt_r>=0) cop->Break_cnt_r =0;
		cop->Break_cnt_r--;
		if(cop->Break_cnt_r<-BREAK_THRESHOLD) Roll_need=0;
	}
	else
	{
		cop->Break_cnt_r =0;
	}
	
	if(cop->Break_cnt_r>MAX_BREAK_CNT || cop->Break_cnt_r<(-MAX_BREAK_CNT))
	{
		Roll_need =-speed_Y;
		printf("Break of Break\n");
		cop->Break_cnt_r=0;
	}
	
	if(Pitch_need > 0){
		if(cop->Break_cnt_p<=0) cop->Break_cnt_p =0;
		cop->Break_cnt_p++;
		if(cop->Break_cnt_p > BREAK_THRESHOLD) Pitch_need=0;	
		if(cop->Break_cnt_p > MAX_BREAK_CNT) cop->Break_cnt_p=0;
	}
	else if(Pitch_need <0){
		if(cop->Break_cnt_p>=0) cop->Break_cnt_p =0;
		cop->Break_cnt_p--;
		if(cop->Break_cnt_p < -BREAK_THRESHOLD) Pitch_need=0;
		if(cop->Break_cnt_p < -MAX_BREAK_CNT) cop->Break_cnt_p=0;
	}
	else
	{
		cop->Break_cnt_p =0;
	}
	if(cop->Break_cnt_p>MAX_BREAK_CNT || cop->Break_cnt_p<(-MAX_BREAK_CNT))
	{
		Pitch_need =-speed_X;
		printf("Break of Break\n");
		cop->Break_cnt_p=0;
	}



	
	cop->roll_Break  = Roll_need	* ROLL_BREAK_POWER;
	cop->pitch_Break = Pitch_need	* PITCH_BREAK_POWER;
	
	if(cop->roll_Break	<	 15) cop->roll_Break += 10;	
	if(cop->roll_Break	>	-15) cop->roll_Break -= 10;	
	if(cop->pitch_Break <	 15) cop->pitch_Break += 10;	
	if(cop->pitch_Break >	-15) cop->pitch_Break -= 10;	


	if(cop->roll_Break >	 90)	cop->roll_Break		=  90;
	if(cop->roll_Break <	-90)	cop->roll_Break		= -90;
	if(cop->pitch_Break >	 90)	cop->pitch_Break	=  90;
	if(cop->pitch_Break <	-90)	cop->pitch_Break	= -90;

	//cop->roll += cop->roll_Break;
	//cop->pitch += cop->pitch_Break;
	

	cop->roll =	R_DEF	+ cop->roll_Break;
	cop->pitch = P_DEF	+ cop->pitch_Break;
	printf("SPEED_X/Y :  %d, %d\n", speed_X,speed_Y);
	printf("Break ROLL / PITCH :  %d, %d\n", cop->roll_Break, cop->pitch_Break);

}

int find_vector(struct Copter* cop, int d_x, int d_y, int d_Z)
{
	int diff_x = (*cop).rx - d_x;
	int diff_y = (*cop).ry - d_y;	
	int diff_z = (int)(*cop).az - d_Z;

	apply_threshold(&diff_x, &diff_y, &diff_z);


	//오른쪽방향이 vector = 1, 시계반대방향 순서로 8까지
	if(diff_z >0) {
		(*cop).throttle = FLYING_Th_DEF;// + 20;//(*cop).vector = 9; //상승
	}
	else if(diff_z <0) {
		(*cop).throttle = FLYING_Th_DEF;// -20;//(*cop).vector = 10; //하강
	}
	else cop->throttle = FLYING_Th_DEF;
	
	if(diff_x < 0 && diff_y == 0) (*cop).vector = 1;
	else if(diff_x <0 && diff_y >0) (*cop).vector = 2;
	else if(diff_x == 0 && diff_y >0) (*cop).vector = 3;
	else if(diff_x >0 && diff_y >0) (*cop).vector = 4;
	else if(diff_x >0 && diff_y == 0) (*cop).vector = 5;
	else if(diff_x >0 && diff_y <0) (*cop).vector = 6;
	else if(diff_x ==0 && diff_y <0) (*cop).vector = 7;
	else if(diff_x <0 && diff_y <0) (*cop).vector = 8;
	else cop->vector == 11;

	switch((*cop).vector)
	{
		case 1:	(*cop).pitch	= P_DEF +SPEED;
				
				if(cop->vx <MIN_SPEED) cop->pitch_flag=0;
				break;
		
		case 2:	(*cop).pitch	= P_DEF +SPEED;
				(*cop).roll		= R_DEF -ROLL_LEFT_SPEED;
				
				if(cop->vx <MIN_SPEED) cop->pitch_flag=0;
				if(cop->vy > (-MIN_SPEED)) cop->roll_flag=0;
				break;
		
		case 3:	(*cop).roll	= R_DEF -ROLL_LEFT_SPEED;
				
				if(cop->vy > (-MIN_SPEED)) cop->roll_flag=0;
				break;
		
		case 4:	(*cop).pitch	= P_DEF-SPEED;
				(*cop).roll		= R_DEF-ROLL_LEFT_SPEED;
				
				if(cop->vx > (-MIN_SPEED)) cop->pitch_flag=0;
				if(cop->vy > (-MIN_SPEED)) cop->roll_flag=0;
				break;
		
		case 5:	(*cop).pitch	= P_DEF-SPEED;
				
				if(cop->vx > (-MIN_SPEED)) cop->pitch_flag=0;
				break;
		
		case 6:	(*cop).pitch	= P_DEF-SPEED;
				(*cop).roll		= R_DEF+ROLL_RIGHT_SPEED;
				
				if(cop->vx > (-MIN_SPEED)) cop->pitch_flag=0;
				if(cop->vy < MIN_SPEED) cop->roll_flag=0;
				break;
		
		case 7:	(*cop).roll	= R_DEF+ROLL_RIGHT_SPEED;
				
				if(cop->vy < MIN_SPEED) cop->roll_flag=0;
				break;
		
		case 8:	(*cop).pitch	= P_DEF+SPEED;
				(*cop).roll		= R_DEF+ROLL_RIGHT_SPEED;
				if(cop->vy < MIN_SPEED) cop->roll_flag=0;
				if(cop->vx <MIN_SPEED) cop->pitch_flag=0;
				break;
		case 11: return 1;
				 break;
		
		default: printf("right_POSITION!!\n"); 
				break;

	}
	


	//Reduce_Accelation(&(cop->pitch),&(cop->roll),&(cop->pitch_flag),&(cop->roll_flag));

	return 0;
}

void Reduce_Accelation(int* pitch, int* roll, int* pitch_flag, int* roll_flag)
{
	printf("pitch_flag : %d\n",(*pitch_flag));
	printf("roll_flag : %d\n",(*roll_flag));
	

	
	if((*pitch)>P_DEF)
	{
		if((*pitch_flag) <0) (*pitch_flag)=0;	
		(*pitch_flag)++;
	}
    else if((*pitch)<P_DEF) 
	{
		if((*pitch_flag)>0) (*pitch_flag) =0;
		(*pitch_flag)--;
	}
	else
	{
		(*pitch_flag) =0;
	}
	
	if((*roll)>R_DEF) 
	{
		if((*roll_flag)<0) (*roll_flag) =0;
		(*roll_flag)++;
	}
	else if((*roll)<R_DEF) 
	{
		if((*roll_flag)>0) (*roll_flag) =0;
		(*roll_flag)--;
	}
	else
	{
		(*roll_flag) =0;
	}
	
	if((*pitch_flag)>=10)
	{
		(*pitch) = P_DEF;
		(*pitch_flag) = 3;
//		(*pitch_flag)=0;
	}

	else if((*pitch_flag)>=5)
	{
		(*pitch) = P_DEF;// + ((SPEED));	
	}
	else if((*pitch_flag)>=4)
	{
		(*pitch) = P_DEF + ((SPEED)/2);
	}
	else if((*pitch_flag)>=2)
	{
		(*pitch) = P_DEF + ((SPEED)*3/4);
	}
	if((*pitch_flag)<=-10)
	{
		(*pitch) = P_DEF;
		(*pitch_flag) = -3;
//		(*pitch_flag)=0;
	}
	else if((*pitch_flag)<=-5)
	{
		(*pitch) = P_DEF;// - ((SPEED)/5);	
	}
	else if((*pitch_flag)<=-4)
	{
		(*pitch) = P_DEF - ((SPEED)/2);
	}
	else if((*pitch_flag)<=-2)
	{
		(*pitch) = P_DEF - ((SPEED)*3/4);
	}


	if((*roll_flag)>=10)
	{
		(*roll) = R_DEF;// + ((SPEED)/5);	
		(*roll_flag) = 3;
//		(*roll_flag)=0;
	}
	else if((*roll_flag)>=5)
	{
		(*roll) = R_DEF;// + ((SPEED)/5);	
	}
	else if((*roll_flag)>=4)
	{
		(*roll) = R_DEF + ((ROLL_RIGHT_SPEED)/2);
	}
	else if((*roll_flag)>=2)
	{
		(*roll) = R_DEF + ((SPEED)*3/4);
	}
	
	if((*roll_flag)<=-10)
	{
		(*roll) = R_DEF;// - ((SPEED)/5);	
		(*roll_flag) =-3;
//		(*roll_flag)=0;
	}
	else if((*roll_flag)<=-5)
	{
		(*roll) = R_DEF;// - ((SPEED)3/4);	
	}
	else if((*roll_flag)<=-4)
	{
		(*roll) = R_DEF - ((ROLL_LEFT_SPEED)/2);
	}
	else if((*roll_flag)<=-2)
	{
		(*roll) = R_DEF - ((SPEED)*3/4);
	}
}	


void apply_threshold(int* diff_x, int* diff_y, int* diff_z)
{
	if((*diff_x)<DIFF_THRESHOLD && (*diff_x)>-DIFF_THRESHOLD)
	{	
		(*diff_x) = 0;	
	}
	if((*diff_y)<DIFF_THRESHOLD && (*diff_y)>-DIFF_THRESHOLD)
	{
		(*diff_y) = 0;
	}
	if((*diff_z)<Z_THRESHOLD && (*diff_z)>-Z_THRESHOLD)
	{
		(*diff_z) = 0;
	}
}

void Prevent_Crash(struct Copter* copter1, struct Copter* copter2)
{
	int interval_X = abs((*copter1).rx - (*copter2).rx);
	int interval_Y = abs((*copter1).ry - (*copter2).ry);
	int interval_Z = abs((*copter1).az - (*copter2).az);
	if(interval_X <=  INTERVAL_LIMIT || interval_Y <= INTERVAL_LIMIT )
	{
		if(interval_Z <= Z_INTERVAL_LIMIT)
		{	printf("PREVENTING_CRASH!!!!\n");
			(*copter2).vector = 11;
			(*copter2).roll 	= 1500;
			(*copter2).pitch 	= 1500;
			(*copter2).yaw 		= 1500;
			(*copter2).throttle = 1000;

		}
	}
	// 1차적으로는 콥터2를 멈추게 해놓음!
	// 필수개선점 : 정확하게 하려면 공간좌표를 이용해서 해야할듯!
	// 추가개선점 : 가까이있는 방향에따라 다른 루트를 찾도록??
}

void Calibration_Head(struct Copter* copter)
{
	int yaw_unit=0;
	switch((*copter).head)
	{
		case 1: yaw_unit =0; break;
		case 2: yaw_unit =3; break;
		case 3: yaw_unit =3; break;
		case 4: yaw_unit =3; break;
		case 5: yaw_unit =3; break;
		case 6: yaw_unit =-3; break;
		case 7: yaw_unit =-3; break;
		case 8: yaw_unit =-3; break;
		default: yaw_unit =0;
				 printf("Error in head");
				 break;
	}
	(*copter).yaw = 1500+yaw_unit;
}

void print_copter_info(struct Copter copter)
{
	/*
	printf("              ");
	printf("|NUMBER : %d\n",copter.id);
	printf("|FLAG : %d\n",copter.seen);
	printf("                                         |R_CX : %d\n",copter.rx);
	printf("                             |R_CY : %d\n",copter.ry);
	printf("|SP_X : %d\n",copter.sx);
	printf("|SP_Y : %d\n",copter.sy);
	printf("               |Z_AVR : %f\n",copter.az);
	printf("|C_HEAD: %d\n",copter.head);
	*/
	printf("%d, %d, %d, %d, %d", copter.head, copter.roll, copter.pitch, copter.yaw, copter.throttle);
	printf("\trx,ry(%d,%d)\tsx,sy(%d,%d)\tvx,vy(%d,%d)\n",
			copter.rx, copter.ry, copter.sx, copter.sy, copter.vx, copter.vy); 
}


void initCopter(struct Copter copter[])
{
	int i;
	for(i=0; i<COPTER_NUMBER; i++){
		copter[i].head		= 1;
		copter[i].vector 	= 11;
		copter[i].roll 		= R_DEF;
		copter[i].pitch		= P_DEF;
		copter[i].yaw		= Y_DEF;
		copter[i].throttle	= Th_DEF;
		copter[i].func1		= 1500;
		copter[i].func2		= 1500;
		copter[i].activate 	= 0;
		copter[i].Break_cnt_p	= 0;
		copter[i].Break_cnt_r	= 0;
		copter[i].roll_Break	= 0;
		copter[i].pitch_Break	= 0;
		copter[i].pitch_flag	= 0;
		copter[i].roll_flag	= 0;
	}
}

/*** opt ***/
void setCopterRPYT(struct Copter copter[], int copterID, int roll, int pitch, int yaw, int throttle){
	copter[copterID].roll		= roll;
	copter[copterID].pitch		= pitch;
	copter[copterID].yaw		= yaw;
	copter[copterID].throttle	= throttle;
}

void setCopterCommand(struct Copter copter[], int copterID, int cmd){
	switch(cmd){
		// START COPTER
		case 1:
			setCopterRPYT(copter, copterID, R_DEF, P_DEF, 2000, Th_DEF);
			sendSignal_Copter(2000);
			setCopterRPYT(copter, copterID, R_DEF, P_DEF, Y_DEF, Th_DEF);
			sendSignal_Copter(1000);

			// 수렴
			copter[copterID].spx = copter[copterID].rx;
			copter[copterID].spy = copter[copterID].ry;
			copter[copterID].activate = 1;
			break;

		// STOP COPTER
		case 2:
			copter[copterID].activate = copter[copterID].height = 0;
			setCopterRPYT(copter, copterID, R_DEF, P_DEF, 1000, Th_DEF);
			sendSignal_Copter(0);
			break;
		
		// CALIB1
		case 3:
			setCopterRPYT(copter, copterID, R_DEF, 1000, 1000, 1000);
			sendSignal_Copter(2000);
			setCopterRPYT(copter, copterID, R_DEF, P_DEF, Y_DEF, Th_DEF);
			sendSignal_Copter(0);
			break;
		
		// CALIB2
		case 4:
			setCopterRPYT(copter, copterID, R_DEF, 1000, 1000, 2000);
			sendSignal_Copter(2000);
			setCopterRPYT(copter, copterID, R_DEF, P_DEF, Y_DEF, Th_DEF);
			sendSignal_Copter(0);
			break;

		case 5:
			setCopterRPYT(copter, copterID, R_DEF, 1000, Y_DEF, 2000);
			sendSignal_Copter(600);
			setCopterRPYT(copter, copterID, R_DEF, P_DEF, Y_DEF, Th_DEF);
			sendSignal_Copter(0);
			break;

		case 6:
			setCopterRPYT(copter, copterID, R_DEF, 2000, Y_DEF, 2000);
			sendSignal_Copter(600);
			setCopterRPYT(copter, copterID, R_DEF, P_DEF, Y_DEF, Th_DEF);
			sendSignal_Copter(0);
			break;

		case 7:
			setCopterRPYT(copter, copterID, 1000, P_DEF, Y_DEF, 2000);
			sendSignal_Copter(600);
			setCopterRPYT(copter, copterID, R_DEF, P_DEF, Y_DEF, Th_DEF);
			sendSignal_Copter(0);
			break;

		case 8:
			setCopterRPYT(copter, copterID, 2000, P_DEF, Y_DEF, 2000);
			sendSignal_Copter(600);
			setCopterRPYT(copter, copterID, R_DEF, P_DEF, Y_DEF, Th_DEF);
			sendSignal_Copter(0);
			break;

		case 9:
			break;

		case 10:
			//setCopterRPYT(copter, copterID, 1301, 1502, 1703, 1904);
			setCopterRPYT(copter, copterID, 1505, 1506, 1507, 1508);
			//setCopterRPYT(copter, copterID, 1511, 1012, 1013, 1014);
			sendSignal_Copter(0);
			break;
	}
}

void Hovering(struct Copter* cop)
{
	int speed_X = cop->vx;
	int speed_Y = cop->vy;
	int addition_X = 0;
	int addition_Y = 0;

	if(abs(speed_X)<9) addition_X =1;
	if(abs(speed_Y)<9) addition_Y =1;


	int Roll_Rate  = ((-speed_Y)    * (HOVERING_RATE+addition_Y));
	int Pitch_Rate = ((-speed_X)    * (HOVERING_RATE+addition_X));
	


	if(Roll_Rate > 0)
	{
		if(cop->Break_cnt_r <0) cop->Break_cnt_r =0;
		cop->Break_cnt_r++;
	}
	else if(Roll_Rate <0)
	{
		if(cop->Break_cnt_r >0) cop->Break_cnt_r =0;
		cop->Break_cnt_r--;
	}
	if(Pitch_Rate >0)
	{
		if(cop->Break_cnt_p <0) cop->Break_cnt_p =0;
		cop->Break_cnt_p++;
	}
	else if(Pitch_Rate <0)
	{
		if(cop->Break_cnt_p >0) cop->Break_cnt_p =0;
		cop->Break_cnt_p--;
	}

	/*
	if(cop->Break_cnt_r <3 && cop->Break_cnt_r >1)
	{
		Roll_Rate = HB;
	}
	if(cop->Break_cnt_r >-3 && cop->Break_cnt_r <-1)
	{
		Roll_Rate = -HB;
	}

	if(cop->Break_cnt_p <3 && cop->Break_cnt_p >1)
	{
		Pitch_Rate = HB;
	}
	if(cop->Break_cnt_p >-3 && cop->Break_cnt_p <-1)
	{
		Pitch_Rate = -HB;
	}*/

	if(cop->Break_cnt_r > 2 || cop->Break_cnt_r < -2) Roll_Rate=0;
	if(cop->Break_cnt_p > 2 || cop->Break_cnt_p < -2) Pitch_Rate=0;

	if(cop->Break_cnt_r > 3 || cop->Break_cnt_r < -3) cop->Break_cnt_r =0;
	if(cop->Break_cnt_p > 3 || cop->Break_cnt_p < -3) cop->Break_cnt_p =0;
	
	if(Roll_Rate > 100) Roll_Rate = 100;
	if(Roll_Rate < -100) Roll_Rate = -100;
	if(Pitch_Rate > 100) Pitch_Rate = 100;
	if(Pitch_Rate < -100) Pitch_Rate = -100;
	
	
	cop->roll   = R_DEF + Roll_Rate;
	cop->pitch  = P_DEF + Pitch_Rate;

	printf("PITCH / ROLL RATE : %d / %d\n", Pitch_Rate, Roll_Rate);

}

void movePP(int val){
	copter[0].pitch = P_DEF + val;
}

void moveMP(int val){
	copter[0].pitch = P_DEF - val;
}

void movePR(int val){
	copter[0].roll = R_DEF + val;
}

void moveMR(int val){
	copter[0].roll = R_DEF - val;
}

#define ABS(x) ((x)>0 ? (x) : -(x))
#define ISPLUS(x) ((x)>0 ? 1 : 0)

void setVector(struct Copter copter[], int copterID, int x, int y){
	int dx = (x - copter[copterID].rx);
	int dy = (y - copter[copterID].ry);

	if(dx <3 && dx >-3) dx=0;
	if(dy <3 && dy >-3) dy=0;

	if(dx > 0 && dy == 0) (copter[0]).vector = 1;
	else if(dx >0 && dy <0) (copter[0]).vector = 2;
	else if(dx == 0 && dy <0) (copter[0]).vector = 3;
	else if(dx <0 && dy <0) (copter[0]).vector = 4;
	else if(dx <0 && dy == 0) (copter[0]).vector = 5;
	else if(dx <0 && dy >0) (copter[0]).vector = 6;
	else if(dx ==0 && dy >0) (copter[0]).vector = 7;
	else if(dx >0 && dy >0) (copter[0]).vector = 8;
}

bool moveDestinationX(struct Copter copter[], int copterID, int x){
	int dx = (x - copter[copterID].rx);
	const int xDiff = 50;
	const int vxStopValue = 20;
	const int pitchMaxValue = 16;

	if( ABS(dx) < xDiff )
		return true;
	else{
		dx = (dx>0) ? (dx-xDiff) : (dx+xDiff);
		if( ABS(copter[copterID].vx) > vxStopValue )
			copter[copterID].pitch = P_DEF;
		else if( ABS(dx) > 200)
			copter[copterID].pitch = ISPLUS(dx) ? (P_DEF + pitchMaxValue) : (P_DEF - pitchMaxValue);
		else if( ABS(dx) >= 0)
			copter[copterID].pitch = ISPLUS(dx) ? (P_DEF + pitchMaxValue / 2) : (P_DEF - pitchMaxValue / 2);
		else printf("moveDestination Exception\n");
	}
	return false;
}

bool moveDestinationY(struct Copter copter[], int copterID, int y){
	int dy = (y - copter[copterID].ry);
	const int yDiff = 5;
	const int vyStopValue = 20;
	const int rollMaxValue = 10;
	int roll = R_DEF;

	if( ABS(dy) < yDiff )
		return true;
	else if( copter[copterID].roll == R_DEF );
	else{
		dy = (dy>0) ? (dy-yDiff) : (dy+yDiff);
		if( ABS(copter[copterID].vy) > vyStopValue )
			roll = R_DEF;
		else if( ABS(dy) > 50)
			roll = ISPLUS(dy) ? (R_DEF + rollMaxValue) : (R_DEF - rollMaxValue);
		else if( ABS(dy) >= 0)
			roll = ISPLUS(dy) ? (R_DEF + rollMaxValue / 2) : (R_DEF - rollMaxValue / 2);
		else printf("moveDestination Exception\n");
	}

	if( ABS(roll - R_DEF) > ABS(copter[copterID].roll - R_DEF)) {
		copter[copterID].roll = roll;
	}
	return false;
}

void moveHeight(struct Copter copter[], int copterID){
	int devication = FLYING_Th_DEF - copter[copterID].throttle;
	int divide, d = 2;
	if((divide = devication / 100) > 0)
		copter[copterID].throttle += (devication / (divide + d));
	else if(devication < 0){
		copter[copterID].throttle = FLYING_Th_DEF;
		copter[copterID].height = 1;
	}
	else if(devication < 50)
		copter[copterID].throttle += 10;
	else if(devication < 100)
		copter[copterID].throttle += 20;
	else
		printf("moveHeight Exception!!\n");
}

void startFormation(struct Copter copter[]){
	int i;
	for(i=0; i<COPTER_NUMBER; i++){
		if(copter[i].seen && copter[i].activate && copter[i].height ){
			setVector(copter, i, 1200, 600);
//			Hovering(&copter[i]);
			Operate_Break(&copter[i]);
			moveDestinationX(copter, i, 1200);
			moveDestinationY(copter, i, 600);
			sendSignal_Copter(150);
		}
		else if(copter[i].seen && copter[i].activate){
			moveHeight(copter, i);
			sendSignal_Copter(150);
		}
	}
}
