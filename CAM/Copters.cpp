#include"Copters.h"


Copter copter1;
Copter copter2;

void Copter1_init()
{

	copter1.number = 1;
    copter1.find_count =0;
    copter1.not_found_count =0;
    copter1.Head_vector =1;

    copter1.Y_MAX = 130;
    copter1.Y_MIN = 0;
    copter1.Cb_MIN = 100;
    copter1.Cb_MAX = 127;
    copter1.Cr_MIN = 159;
    copter1.Cr_MAX = 255;

    copter1.Y_MAX2 = 190;
    copter1.Y_MIN2 = 80;
    copter1.Cb_MIN2 = 90;
    copter1.Cb_MAX2 = 129;
    copter1.Cr_MIN2 = 86;
    copter1.Cr_MAX2 = 118;

    copter1.Flag = false;
    copter1.Picture =imread("copter1.png",CV_LOAD_IMAGE_COLOR);
    copter1.Picture2 = imread("copter1.png",CV_LOAD_IMAGE_COLOR);
    copter1.Picture3 = imread("copter1.png",CV_LOAD_IMAGE_COLOR);

	Mat YUV;
    cvtColor(copter1.Picture,YUV,CV_RGB2YCrCb);
	copter1.Pic_Binary1 = Mat(copter1.Picture.rows,copter1.Picture.cols,CV_8UC1);
	copter1.Pic_Binary2 = Mat(copter1.Picture.rows,copter1.Picture.cols,CV_8UC1);
	
	Mat Pic_Binary3(copter1.Picture.rows,copter1.Picture.cols,CV_8UC1);
	Mat Pic_Binary4(copter1.Picture.rows,copter1.Picture.cols,CV_8UC1);
	Mat Pic_Binary5(copter1.Picture.rows,copter1.Picture.cols,CV_8UC1);
	Mat Pic_Binary6(copter1.Picture.rows,copter1.Picture.cols,CV_8UC1);
	
	vector<Mat> yuv;
	split(YUV,yuv);
	threshold(yuv[0],copter1.Pic_Binary1,copter1.Y_MAX,255,THRESH_BINARY_INV);
	threshold(yuv[1],yuv[1],copter1.Cb_MAX,255,THRESH_TOZERO_INV);
	threshold(yuv[1],Pic_Binary3,copter1.Cb_MIN,255,THRESH_BINARY);
	threshold(yuv[2],yuv[2],copter1.Cr_MAX,255,THRESH_TOZERO_INV);
	threshold(yuv[2],Pic_Binary4,copter1.Cr_MIN,255,THRESH_BINARY);
	
	bitwise_and(copter1.Pic_Binary1,Pic_Binary3,Pic_Binary3);
	bitwise_and(Pic_Binary3,Pic_Binary4,copter1.Pic_Binary1);

	split(YUV,yuv);
	threshold(yuv[0],yuv[0],copter1.Y_MAX2,255,THRESH_TOZERO_INV);
	threshold(yuv[0],copter1.Pic_Binary2,copter1.Y_MIN2,255,THRESH_BINARY);
	threshold(yuv[1],yuv[1],copter1.Cb_MAX2,255,THRESH_TOZERO_INV);
	threshold(yuv[1],Pic_Binary5,copter1.Cb_MIN2,255,THRESH_BINARY);
	threshold(yuv[2],yuv[2],copter1.Cr_MAX2,255,THRESH_TOZERO_INV);
	threshold(yuv[2],Pic_Binary6,copter1.Cr_MIN2,255,THRESH_BINARY);
	
	bitwise_and(copter1.Pic_Binary2,Pic_Binary5,Pic_Binary5);
	bitwise_and(Pic_Binary5,Pic_Binary6,copter1.Pic_Binary2);

	copter1.vector = 0;
	
	copter1.Z = 0;
	copter1.Z_avr =0;
	copter1.find_mode = false;
	copter1.highenX_flag = false;
	copter1.highenY_flag = false;
    //imshow("3",copter1.Pic_Binary3);
}

void Copter2_init()
{
	
	copter2.number = 2;
    copter2.find_count =0;
    copter2.not_found_count =0;
    copter2.Head_vector =1;
/*
    copter2.Y_MAX = 140;
    copter2.Y_MIN = 0;
    copter2.Cb_MIN = 100;
    copter2.Cb_MAX = 178;
    copter2.Cr_MIN = 151;
    copter2.Cr_MAX = 255;
*/
    copter2.Y_MAX = 130;
    copter2.Y_MIN = 0;
    copter2.Cb_MIN = 100;
    copter2.Cb_MAX = 153;
    copter2.Cr_MIN = 147;
    copter2.Cr_MAX = 255;
	
	copter2.Y_MAX2 = 124;
    copter2.Y_MIN2 = 0;
    copter2.Cb_MIN2 = 133;
    copter2.Cb_MAX2 = 255;
    copter2.Cr_MIN2 = 55;
    copter2.Cr_MAX2 = 90;	//blue

/*    copter2.Y_MAX2 = 190;
    copter2.Y_MIN2 = 80;
    copter2.Cb_MIN2 = 90;
    copter2.Cb_MAX2 = 129;
    copter2.Cr_MIN2 = 86;
    copter2.Cr_MAX2 = 118;
*/
    copter2.Flag = false;
    copter2.Picture =imread("copter2.png",CV_LOAD_IMAGE_COLOR);
    copter2.Picture2 = imread("copter2.png",CV_LOAD_IMAGE_COLOR);
    copter2.Picture3 = imread("copter2.png",CV_LOAD_IMAGE_COLOR);

	Mat YUV;
    cvtColor(copter2.Picture,YUV,CV_RGB2YCrCb);
	copter2.Pic_Binary1 = Mat(copter2.Picture.rows,copter2.Picture.cols,CV_8UC1);
	copter2.Pic_Binary2 = Mat(copter2.Picture.rows,copter2.Picture.cols,CV_8UC1);
	
	Mat Pic_Binary3(copter2.Picture.rows,copter2.Picture.cols,CV_8UC1);
	Mat Pic_Binary4(copter2.Picture.rows,copter2.Picture.cols,CV_8UC1);
	Mat Pic_Binary5(copter2.Picture.rows,copter2.Picture.cols,CV_8UC1);
	Mat Pic_Binary6(copter2.Picture.rows,copter2.Picture.cols,CV_8UC1);
	
	vector<Mat> yuv;
	split(YUV,yuv);
	threshold(yuv[0],yuv[0],copter2.Y_MAX,255,THRESH_TOZERO_INV);
	threshold(yuv[0],copter2.Pic_Binary1,copter2.Y_MIN,255,THRESH_BINARY);
	threshold(yuv[1],yuv[1],copter2.Cb_MAX,255,THRESH_TOZERO_INV);
	threshold(yuv[1],Pic_Binary3,copter2.Cb_MIN,255,THRESH_BINARY);
	threshold(yuv[2],yuv[2],copter2.Cr_MAX,255,THRESH_TOZERO_INV);
	threshold(yuv[2],Pic_Binary4,copter2.Cr_MIN,255,THRESH_BINARY);

	bitwise_and(copter2.Pic_Binary1,Pic_Binary3,Pic_Binary3);
	bitwise_and(Pic_Binary3,Pic_Binary4,copter2.Pic_Binary1);
	
	split(YUV,yuv);
	threshold(yuv[0],yuv[0],copter2.Y_MAX2,255,THRESH_TOZERO_INV);
	threshold(yuv[0],copter2.Pic_Binary2,copter2.Y_MIN2,255,THRESH_BINARY);
	threshold(yuv[1],yuv[1],copter2.Cb_MAX2,255,THRESH_TOZERO_INV);
	threshold(yuv[1],Pic_Binary5,copter2.Cb_MIN2,255,THRESH_BINARY);
	threshold(yuv[2],yuv[2],copter2.Cr_MAX2,255,THRESH_TOZERO_INV);
	threshold(yuv[2],Pic_Binary6,copter2.Cr_MIN2,255,THRESH_BINARY);
	
	bitwise_and(copter2.Pic_Binary2,Pic_Binary5,Pic_Binary5);
	bitwise_and(Pic_Binary5,Pic_Binary6,copter2.Pic_Binary2);
	
	copter2.vector = 0;
	
	copter2.Z=0;
	copter2.Z_avr=0;
	copter2.find_mode = false;
	copter2.highenX_flag = false;
	copter2.highenY_flag = false;
    //imshow("3",copter1.Pic_Binary3);
}


void cop_packet_init(Copter_Packet& cp, Copter cop)
{
	cp.number = cop.number;
	cp.real_cx = cop.real_cx;
	cp.real_cy = cop.real_cy;
	cp.space_X = cop.space_X;
    cp.space_Y = cop.space_Y;
    cp.Z_avr   = cop.Z_avr;
	cp.Head_Vector  = cop.Head_vector;
    cp.X_v = cop.X_v;
    cp.Y_v = cop.Y_v;
    cp.vector = cop.vector;
    if(cop.Flag) cp.flag = 1;
    else cp.flag = 0;

}
