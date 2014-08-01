#include "Copters.h"
#include "Calib_Arr.h"
extern Mat Cam_Image;
//extern pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
bool In_CrashDanger(Copter cop1, Copter cop2)
{
	if(abs(cop1.Z_avr-cop2.Z_avr) <27)
	{
		if(abs(cop1.space_X - cop2.space_X) <43 && abs(cop1.space_Y - cop2.space_Y)<43)
		{
			return true;
		}
	}
	return false;
}

void Socket_Data(Copter cop, Copter_Packet& cp)
{
	 cp.number = cop.number;
	 cp.real_cx = cop.real_cx;
	 cp.real_cy = cop.real_cy;
	 cp.space_X = cop.space_X;
	 cp.space_Y = cop.space_Y;
	 cp.Z_avr   = cop.Z_avr;
	 cp.Head_Vector = cop.Head_vector;    
     cp.X_v = cop.X_v;
     cp.Y_v = cop.Y_v;
     cp.vector = cop.vector;
     if(cop.Flag) cp.flag = 1;
     else cp.flag = 0;

	//write(client_sockfd,buf,sizeof(buf));
}

void Send_Packet(Copter_Packet cp1, Copter_Packet cp2, int filedes)
{
	char buf[255];
	sprintf(buf,"%d&%d&%d&%d&%d&%d&%d&%d&%d&%d&%d&%d&%d&%d&%d&%d&%d&%d&%d&%d&%d&%d&\r\n", cp1.number, cp1.flag, cp1.real_cx, cp1.real_cy, cp1.space_X, cp1.space_Y, cp1.Z_avr,cp1.Head_Vector,cp1.X_v, cp1.Y_v, cp1.vector, cp2.number, cp2.flag, cp2.real_cx, cp2.real_cy, cp2.space_X, cp2.space_Y, cp2.Z_avr, cp2.Head_Vector, cp2.X_v, cp2.Y_v, cp2.vector);
	write(filedes,buf,sizeof(buf));
}




/*
void Socket_Data(Copter cop, char* buf[])
{
	int buf_idx=0;
	char num =0x00;
	char flag=0x00;

	
	if(cop.number == 1)
	{
		buf_idx = 43;
		num = 0x01;
	}
	else if(cop.number == 2)
	{
		buf_idx = 21;
		num = 0x02;
	}
	if(cop.Flag)
		flag = 0x01;

	int Z_DATA = cop.Z_avr*10;

	(*buf)[buf_idx] = num;
	(*buf)[--buf_idx] = flag;
	(*buf)[--buf_idx] = 0xff & (cop.real_cx>>(8*3));
	(*buf)[--buf_idx] = 0xff & (cop.real_cx>>(8*2));
	(*buf)[--buf_idx] = 0xff & (cop.real_cx>>8);
	(*buf)[--buf_idx] = 0xff & (cop.real_cx);

	(*buf)[--buf_idx] = 0xff & (cop.real_cy>>(8*3));
	(*buf)[--buf_idx] = 0xff & (cop.real_cy>>(8*2));
	(*buf)[--buf_idx] = 0xff & (cop.real_cy>>8);
	(*buf)[--buf_idx] = 0xff & (cop.real_cy);

	(*buf)[--buf_idx] = 0xff & (cop.space_X>>(8*3));
	(*buf)[--buf_idx] = 0xff & (cop.space_X>>(8*2));
	(*buf)[--buf_idx] = 0xff & (cop.space_X>>8);
	(*buf)[--buf_idx] = 0xff & (cop.space_X);

	(*buf)[--buf_idx] = 0xff & (cop.space_Y>>(8*3));
	(*buf)[--buf_idx] = 0xff & (cop.space_Y>>(8*2));
	(*buf)[--buf_idx] = 0xff & (cop.space_Y>>8);
	(*buf)[--buf_idx] = 0xff & (cop.space_Y);

	(*buf)[--buf_idx] = 0xff & (Z_DATA>>(8*3));
	(*buf)[--buf_idx] = 0xff & (Z_DATA>>(8*2));
	(*buf)[--buf_idx] = 0xff & (Z_DATA>>8);
	(*buf)[--buf_idx] = 0xff & (Z_DATA);

	
	(*buf)[44] = 0xff;
}
*/
void Take_Copter_Init(Mat Cam, Mat Cam2, Mat Pic, Mat Pic2, Copter& cop)
{
    Mat Corr_1;
    Mat Corr_2;
    Mat Result;
    double min,max;
    Point Copter_point;
	
	matchTemplate(Cam,Pic,Corr_1,CV_TM_CCOEFF_NORMED);
	matchTemplate(Cam2,Pic2,Corr_2,CV_TM_CCOEFF_NORMED);
    Result = (0.5*Corr_1) + (0.5*Corr_2);

    minMaxLoc(Result,&min,&max,NULL,&Copter_point);
    cop.x = Copter_point.x-(TEMPLATE_WIDTH/3);
    cop.y = Copter_point.y-(TEMPLATE_HEIGHT/3);
    cop.width = TEMPLATE_WIDTH;
    cop.height = TEMPLATE_HEIGHT;

    if(cop.x<0) cop.x = 0;
    if(cop.y<0) cop.y = 0;
    if(1920-cop.x <=TEMPLATE_WIDTH){ cop.x = 1920-TEMPLATE_WIDTH-1; }
    if(1080-cop.y <=TEMPLATE_HEIGHT){ cop.y = 1080-TEMPLATE_HEIGHT-1; }
	cop.center_x = cop.x+(cop.width/2);
	cop.center_y = cop.y+(cop.height/2);
	cop.prev_cx[0] = cop.center_x;
	cop.prev_cy[0] = cop.center_y;
	for(int i=1; i<V_BUFFER_SIZE; i++)
	{
		cop.prev_cx[i] = cop.center_x;
		cop.prev_cy[i] = cop.center_y;
	}
    if(cop.number==1)
	cop.Copter_Layer = Cam_Image(Rect(cop.x , cop.y , cop.width , cop.height ));
	else if(cop.number==2)
	cop.Copter_Layer = Cam_Image(Rect(cop.x , cop.y , cop.width , cop.height));

}
void Z_buffer_init(Copter& cop)
{
	for(int i=0; i<Z_BUFFER_SIZE; i++)
	{
		cop.Z_Buffer[i] = cop.Z;
	}
	cop.Z_avr = cop.Z;
}
void rad_buffer_init(Copter& cop)
{
	for(int i=0; i<BUFFER_SIZE; i++)
	{
		cop.radX_buff[i] = cop.radius_X;
		cop.radY_buff[i] = cop.radius_Y;
	}
	cop.radX_avr = cop.radius_X;
	cop.radY_avr = cop.radius_Y;
}

void Z_buffer_setting(Copter& cop, int& index)
{
	//cop.find_mode = false;
#if DEBUG
	cout << "Z:";
	for(int m=0; m<Z_BUFFER_SIZE; m++){ cout <<" [ "<<cop.Z_Buffer[m]<<" ]";
		if(index == m) cout << "<";
		else cout <<" ";
	}
	cout << endl;
#endif
	int sum=0;
	
	if(cop.Z > (cop.Z_avr+5))
	{
		cop.Z_Buffer[index] = cop.Z_avr+3;
	}
	else if(cop.Z> (cop.Z_avr+3))
	{
		cop.Z_Buffer[index] = cop.Z_avr+2;
	}
	else if(cop.Z > cop.Z_avr)
	{
		cop.Z_Buffer[index] = cop.Z_avr+1;
	}
	else if(cop.Z < (cop.Z_avr-5))
	{
		cop.Z_Buffer[index] = cop.Z_avr - 3;
	}
	else if(cop.Z < (cop.Z_avr-3))
	{
		cop.Z_Buffer[index] = cop.Z_avr -2;
	}
	else if(cop.Z < cop.Z_avr)
	{
		cop.Z_Buffer[index] = cop.Z_avr-1;
	}
	else
	{ 
		cop.Z_Buffer[index] = cop.Z_avr; 
	}
	if(index == (Z_BUFFER_SIZE-1))
	{
		index = 0;
	}
	else
	{	index++; }
	
	for(int j=0; j<Z_BUFFER_SIZE; j++)
	{
		sum += cop.Z_Buffer[j];
	}
	cop.Z_avr = ((float)sum / Z_BUFFER_SIZE);

}

void buffer_setting(Copter& cop, int& index)
{
	//cop.find_mode = false;
#if DEBUG 
	cout << "radX:";
	for(int m=0; m<BUFFER_SIZE; m++){ cout <<" [ "<<cop.radX_buff[m]<<" ]";
		if(index == m) cout << "<";
		else cout <<" ";
	}
	cout << endl;
	cout << "radY:";
	for(int m=0; m<BUFFER_SIZE; m++){ cout <<" [ "<<cop.radY_buff[m]<<" ]";
		if(index == m) cout << "<";
		else cout <<" ";
	}
	cout << endl;
#endif

	int sum=0;
	
	if(cop.radius_X > (cop.radX_avr+20))
	{
		cop.radX_buff[index] = cop.radX_avr+10;
	}
	else if(cop.radius_X > (cop.radX_avr+10))
	{
		cop.radX_buff[index] = cop.radX_avr+3;
	}
	else if(cop.radius_X > (cop.radX_avr+5))
	{
		cop.radX_buff[index] = cop.radX_avr+2;
	}
	else if(cop.radius_X > cop.radX_avr)
	{
		cop.radX_buff[index] = cop.radX_avr+1;
	}
	else if(cop.radius_X < (cop.radX_avr-20))
	{
		cop.radX_buff[index] = cop.radX_avr - 10;
	}
	else if(cop.radius_X < (cop.radX_avr-10))
	{
		cop.radX_buff[index] = cop.radX_avr - 3;
	}
	else if(cop.radius_X < (cop.radX_avr-5))
	{
		cop.radX_buff[index] = cop.radX_avr -2;
	}
	else if(cop.radius_X < cop.radX_avr)
	{
		cop.radX_buff[index] = cop.radX_avr-1;
	}
	else
	{ 
		cop.radX_buff[index] = cop.radX_avr; 
	} 
	
	
	for(int j=0; j<BUFFER_SIZE; j++)
	{
		sum += cop.radX_buff[j];
	}
	cop.radX_avr = ((float)sum / BUFFER_SIZE);
	
	sum=0;
	if(cop.radius_Y > (cop.radY_avr+20))
	{
		cop.radY_buff[index] = cop.radY_avr+10;
	}
	else if(cop.radius_Y > (cop.radY_avr+10))
	{
		cop.radY_buff[index] = cop.radY_avr+3;
	}
	else if(cop.radius_Y > (cop.radY_avr+5))
	{
		cop.radY_buff[index] = cop.radY_avr+2;
	}
	else if(cop.radius_Y > cop.radY_avr)
	{
		cop.radY_buff[index] = cop.radY_avr+1;
	}
	else if(cop.radius_Y < (cop.radY_avr-20))
	{
		cop.radY_buff[index] = cop.radY_avr - 10;
	}
	else if(cop.radius_Y < (cop.radY_avr-10))
	{
		cop.radY_buff[index] = cop.radY_avr - 3;
	}
	else if(cop.radius_Y < (cop.radY_avr-5))
	{
		cop.radY_buff[index] = cop.radY_avr -2;
	}
	else if(cop.radius_Y < cop.radY_avr)
	{
		cop.radY_buff[index] = cop.radY_avr-1;
	}
	else
	{
		cop.radY_buff[index] = cop.radY_avr; 
	} 
	if(index == (BUFFER_SIZE-1))
	{
		index = 0;
	}
	else
	{	index++; }
	
	for(int j=0; j<BUFFER_SIZE; j++)
	{
		sum += cop.radY_buff[j];
	}
	cop.radY_avr = ((float)sum / BUFFER_SIZE);
}

void print_info(Copter cop)
{

	cout << endl << endl << endl;
    cout << "******** Copter Info ***********" << endl;
    cout << "================================" << endl;
    cout << "| number  : " << cop.number << endl;
    cout << "| x       : " << cop.x << endl;
    cout << "| y       : " << cop.y << endl;
    cout << "| width   : " << cop.width << endl;
    cout << "| height  : " << cop.height << endl;
    cout << "| averageX: " << cop.radX_avr << endl;
    cout << "| averageY: " << cop.radY_avr << endl;
	cout << "| ==========================>averageZ: " << cop.Z_avr << endl;
	cout << "================================" << endl;
//	system("clear");
	/*    cout << "| <picture1> " << endl;
    cout << "| width     : " << cop.Picture.cols << endl;
    cout << "| height    : " << cop.Picture.rows << endl;
    cout << "| b1_width  : " << cop.Pic_Binary1.cols << endl;
    cout << "| b1_height : " << cop.Pic_Binary1.rows << endl;
    cout << "| b2_width  : " << cop.Pic_Binary2.cols << endl;
    cout << "| b2_height : " << cop.Pic_Binary2.rows << endl;
    cout << "================================" << endl;
    cout << "| <picture2> " << endl;
    cout << "| width     : " << cop.Picture2.cols << endl;
    cout << "| height    : " << cop.Picture2.rows << endl;
    cout << "================================" << endl;
    cout << "| <picture3> " << endl;
    cout << "| width     : " << cop.Picture3.cols << endl;
    cout << "| height    : " << cop.Picture3.rows << endl;
    cout << "================================" << endl;
*/
}

//                  색1       색2            색1            색2           색1              색2
void Take_Copter(Mat Cam, Mat Cam2,  Copter& cop)
{
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    Mat YUV_PIC3;
    
    Mat Pic2_col1(cop.Picture3.rows,cop.Picture3.cols,CV_8UC1);
    Mat Pic2_col2(cop.Picture3.rows,cop.Picture3.cols,CV_8UC1);
   	Mat temp3(cop.Picture3.rows,cop.Picture3.cols,CV_8UC1);
   	Mat temp4(cop.Picture3.rows,cop.Picture3.cols,CV_8UC1);
   	Mat temp5(cop.Picture3.rows,cop.Picture3.cols,CV_8UC1);
   	Mat temp6(cop.Picture3.rows,cop.Picture3.cols,CV_8UC1);
	
    cvtColor(cop.Picture3,YUV_PIC3,CV_RGB2YCrCb);
	
	vector<Mat> yuv;
		Mat Corr_1_1;//색1
    Mat Corr_1_2;//색1
    Mat Corr_2_1;//색2
    Mat Corr_2_2;//색2
    int corr_size_X =0;
    int corr_size_Y =0;


    Mat Result;
    double min,max;
    Point Copter_point;
   
    for(int i=0; i<V_BUFFER_SIZE; i++)
    {
        cop.prev_cx[i] = -5000;
        cop.prev_cy[i] = -5000;
    }

	if(cop.find_mode == true){
		cop.find_mode = false;
		matchTemplate(Cam,cop.Pic_Binary1,Corr_1_1,CV_TM_CCOEFF_NORMED);
    	matchTemplate(Cam2,cop.Pic_Binary2,Corr_1_2,CV_TM_CCOEFF_NORMED);
		if(Corr_1_1.rows<Corr_1_2.rows)
    	{ 
       		corr_size_Y = Corr_1_1.rows; 
    	}
    	else
    	{
        	corr_size_Y = Corr_1_2.rows;
    	}
    	if(Corr_1_1.cols<Corr_1_2.cols)
    	{
        	corr_size_X = Corr_1_1.cols;
    	}
    	else
    	{
        	corr_size_X = Corr_1_2.cols;
    	}
    	resize(Corr_1_1,Corr_1_1,Size(corr_size_X,corr_size_Y),0,0,INTER_CUBIC);
    	resize(Corr_1_2,Corr_1_2,Size(corr_size_X,corr_size_Y),0,0,INTER_CUBIC);
		Result = (0.6*Corr_1_1)+(0.6*Corr_1_2);
	}
	else
	{
		cop.find_mode = true;
		split(YUV_PIC3,yuv);
		threshold(yuv[0],yuv[0],cop.Y_MAX,255,THRESH_TOZERO_INV);
		threshold(yuv[0],Pic2_col1,cop.Y_MIN,255,THRESH_BINARY);
		threshold(yuv[1],yuv[1],cop.Cb_MAX,255,THRESH_TOZERO_INV);
		threshold(yuv[1],temp3,cop.Cb_MIN,255,THRESH_BINARY);
		threshold(yuv[2],yuv[2],cop.Cr_MAX,255,THRESH_TOZERO_INV);
		threshold(yuv[2],temp4,cop.Cr_MIN,255,THRESH_BINARY);
	
		bitwise_and(Pic2_col1,temp3,temp3);
		bitwise_and(temp3,temp4,Pic2_col1);

		split(YUV_PIC3,yuv);
	
		threshold(yuv[0],yuv[0],cop.Y_MAX2,255,THRESH_TOZERO_INV);
		threshold(yuv[0],Pic2_col2,cop.Y_MIN2,255,THRESH_BINARY);
		threshold(yuv[1],yuv[1],cop.Cb_MAX2,255,THRESH_TOZERO_INV);
		threshold(yuv[1],temp5,cop.Cb_MIN2,255,THRESH_BINARY);
		threshold(yuv[2],yuv[2],cop.Cr_MAX2,255,THRESH_TOZERO_INV);
		threshold(yuv[2],temp6,cop.Cr_MIN2,255,THRESH_BINARY);
    
		bitwise_and(Pic2_col2,temp5,temp5);
		bitwise_and(temp5,temp6,Pic2_col2);
	

		
		matchTemplate(Cam,Pic2_col1,Corr_2_1,CV_TM_CCOEFF_NORMED);
    	matchTemplate(Cam2,Pic2_col2,Corr_2_2,CV_TM_CCOEFF_NORMED);
		if(Corr_2_1.rows<Corr_2_2.rows)
    	{ 
       		corr_size_Y = Corr_2_1.rows; 
    	}
    	else
    	{
        	corr_size_Y = Corr_2_2.rows;
    	}
    	if(Corr_2_1.cols<Corr_2_2.cols)
    	{
        	corr_size_X = Corr_2_1.cols;
    	}
    	else
    	{
        	corr_size_X = Corr_2_2.cols;
    	}
    	resize(Corr_2_1,Corr_2_1,Size(corr_size_X,corr_size_Y),0,0,INTER_CUBIC);
   		resize(Corr_2_2,Corr_2_2,Size(corr_size_X,corr_size_Y),0,0,INTER_CUBIC);
		Result = (0.6*Corr_2_1)+(0.6*Corr_2_2);
	}
//	imshow("3",Pic2_col2);
//	imshow("2",Pic2_col1);
//	imshow("1",Result);
    minMaxLoc(Result,&min,&max,NULL,&Copter_point);

    cop.width = 3*(cop.Pic_Binary2.cols);
    cop.height =3*(cop.Pic_Binary2.rows);
    cop.x = Copter_point.x-(cop.width/3);
    cop.y = Copter_point.y-(cop.height/3);
    Reposition_minus(cop);
   	pthread_mutex_lock(&mutex);// =========* 
    resize(cop.Copter_Layer,cop.Copter_Layer,Size(cop.width,cop.height),0,0,INTER_NEAREST);
    if(cop.number == 1)
		cop.Copter_Layer = Cam_Image(Rect(cop.x , cop.y , cop.width , cop.height ) );
	else if(cop.number == 2)
		cop.Copter_Layer = Cam_Image(Rect(cop.x,cop.y,cop.width,cop.height));
	   pthread_mutex_unlock(&mutex);// ==========*

}

void Copter_Color(Mat YUV, Mat* Binary1, Mat* Binary2, Copter copter)
{
	Mat Binary3((*Binary1).rows,(*Binary1).cols,CV_8UC1);
	Mat Binary4((*Binary1).rows,(*Binary1).cols,CV_8UC1);
	Mat Binary5((*Binary2).rows,(*Binary2).cols,CV_8UC1);
	Mat Binary6((*Binary2).rows,(*Binary2).cols,CV_8UC1);

	vector<Mat> yuv;
	vector<Mat> yuv2;
	split(YUV,yuv);
	threshold(yuv[0],yuv[0],copter.Y_MAX,255,THRESH_TOZERO_INV);
	threshold(yuv[0],(*Binary1),copter.Y_MIN,255,THRESH_BINARY);
	threshold(yuv[1],yuv[1],copter.Cb_MAX,255,THRESH_TOZERO_INV);
	threshold(yuv[1],(Binary3),copter.Cb_MIN,255,THRESH_BINARY);
	threshold(yuv[2],yuv[2],copter.Cr_MAX,255,THRESH_TOZERO_INV);
	threshold(yuv[2],(Binary4),copter.Cr_MIN,255,THRESH_BINARY);

	bitwise_and((*Binary1),Binary3,Binary3);
	bitwise_and(Binary3,Binary4,(*Binary1));

	split(YUV,yuv2);
	
	threshold(yuv2[0],yuv2[0],copter.Y_MAX2,255,THRESH_TOZERO_INV);
	threshold(yuv2[0],(*Binary2),copter.Y_MIN2,255,THRESH_BINARY);
	threshold(yuv2[1],yuv2[1],copter.Cb_MAX2,255,THRESH_TOZERO_INV);
	threshold(yuv2[1],(Binary5),copter.Cb_MIN2,255,THRESH_BINARY);
	threshold(yuv2[2],yuv2[2],copter.Cr_MAX2,255,THRESH_TOZERO_INV);
	threshold(yuv2[2],(Binary6),copter.Cr_MIN2,255,THRESH_BINARY);

	bitwise_and((*Binary2),Binary5,Binary5);
	bitwise_and(Binary5,Binary6,(*Binary2));
    
	//vector<Vec4i> hierarchy1;
    //vector<vector<Point> > contours1;
/*
    findContours((*Binary1),contours1,hierarchy1,RETR_CCOMP, CHAIN_APPROX_TC89_KCOS);
    for(size_t idx=0; idx<contours1.size(); idx++)
    {
        drawContours((*Binary1),contours1,idx,Scalar(255,255,255),CV_FILLED,8);
    }
    bitwise_and((*Binary1),(*Binary2),(*Binary2));*/
	
}

void Check_Copter(Copter& copter)
{
    bool Flag_1 = false;
    bool Flag_2 = false;
    Mat YUV;
    Mat color1(copter.height , copter.width , CV_8UC1);
    Mat color2(copter.height , copter.width , CV_8UC1);
    Mat color3(copter.height , copter.width , CV_8UC1);
    
    Mat color1_2(copter.height , copter.width , CV_8UC1);
    Mat color1_3(copter.height , copter.width , CV_8UC1);
    
    Mat color2_2(copter.height , copter.width , CV_8UC1);
    Mat color2_3(copter.height , copter.width , CV_8UC1);
    



	Mat Temp_roi;

    vector<vector<Point> > contours1;
    vector<vector<Point> > contours2;
    vector<vector<Point> > contours3;

    vector<Vec4i> hierarchy1;
    vector<Vec4i> hierarchy2;
    vector<Vec4i> hierarchy3;
    
    Rect copter_box1;
    Rect copter_box2;
    Rect copter_box3;
    Rect copter_box4;

    int max_contour1=0;
    int max_contour2=0;
    int max_contour3=0;

    int min_num1 =0x7fffff;
    int min_num2 =0x7fffff;
    int min_num3 =0x7fffff;
    int x1, x2, y1, y2;
    int min_diff =0x7fffff;
    int min_diff2 =0x7fffff; 
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	double start,end,t1;
	
	pthread_mutex_lock(&mutex);
	cvtColor(copter.Copter_Layer,YUV,CV_RGB2YCrCb);
	pthread_mutex_unlock(&mutex);
	
	vector<Mat> yuv;
	split(YUV,yuv);

//	start=(double)cvGetTickCount();
	
	threshold(yuv[0],yuv[0],copter.Y_MAX,255,THRESH_TOZERO_INV);
	threshold(yuv[0],(color1),copter.Y_MIN,255,THRESH_BINARY);
	threshold(yuv[1],yuv[1],copter.Cb_MAX,255,THRESH_TOZERO_INV);
	threshold(yuv[1],(color1_2),copter.Cb_MIN,255,THRESH_BINARY);
	threshold(yuv[2],yuv[2],copter.Cr_MAX,255,THRESH_TOZERO_INV);
	threshold(yuv[2],(color1_3),copter.Cr_MIN,255,THRESH_BINARY);
	
	bitwise_and(color1,color1_2,color1_2);
	bitwise_and(color1_2,color1_3,color1);

	split(YUV,yuv);
	
	threshold(yuv[0],yuv[0],copter.Y_MAX2,255,THRESH_TOZERO_INV);
	threshold(yuv[0],(color2),copter.Y_MIN2,255,THRESH_BINARY);
	threshold(yuv[1],yuv[1],copter.Cb_MAX2,255,THRESH_TOZERO_INV);
	threshold(yuv[1],(color2_2),copter.Cb_MIN2,255,THRESH_BINARY);
	threshold(yuv[2],yuv[2],copter.Cr_MAX2,255,THRESH_TOZERO_INV);
	threshold(yuv[2],(color2_3),copter.Cr_MIN2,255,THRESH_BINARY);

	bitwise_and(color2,color2_2,color2_2);
	bitwise_and(color2_2,color2_3,color3);
	
//    end=(double)cvGetTickCount();
//	t1 = (end-start)/((double)cvGetTickFrequency()*1000.);
//	printf("Threshold : %g ms\n",t1);
#if DEBUG	
	start=(double)cvGetTickCount();
#endif
	findContours(color1, contours1, hierarchy1, RETR_CCOMP, CHAIN_APPROX_TC89_KCOS);

    if(contours1.size() == 0 ) { cout <<copter.number <<":  no color1 ! " << endl;
    }

    for(size_t idx = 0; idx < contours1.size(); idx++)
    {
        drawContours(color1,contours1,idx,Scalar(255,255,255),CV_FILLED,8);
    }
    bitwise_and(color1,color3,color2);
    
    findContours(color2, contours2,hierarchy2, RETR_CCOMP, CHAIN_APPROX_TC89_KCOS);
    if(contours2.size() == 0 ) { cout <<copter.number << ":  no color2 ! " << endl;
	}
#if DEBUG
    end=(double)cvGetTickCount();
	t1 = (end-start)/((double)cvGetTickFrequency()*1000.);
	printf("findcountours : %g ms\n",t1);

	start=(double)cvGetTickCount();
#endif
	for(int m=0; m<contours1.size(); m++)
    {
        for(int n=0; n<contours2.size(); n++)
        {//
            copter_box1 = boundingRect(contours1[m]);
            copter_box2 = boundingRect(contours2[n]);
            Point box2_center = Point(copter_box2.x+(copter_box2.width/2),copter_box2.y+(copter_box2.height/2));
            if(box2_center.x >=copter_box1.x-copter_box1.width && box2_center.x <= copter_box1.x+copter_box1.width*2 && box2_center.y >=copter_box1.y-copter_box1.height && box2_center.y <=copter_box1.y+(copter_box1.height)*2)
            {

            	int diff = abs(copter_box2.x+(copter_box2.width/2) - copter_box1.x+(copter_box1.width/2)) + abs(copter_box2.y+(copter_box2.height/2) - copter_box1.y+(copter_box1.height/2));
            	if(diff < min_diff){
                	min_diff = diff;
                	min_num1 = m;
                	min_num2 = n;
            	}

            }

        }//
    }
	absdiff(color3,color2,color3);
	findContours(color3, contours3, hierarchy3, RETR_CCOMP, CHAIN_APPROX_TC89_KCOS);
	if(contours1.size()>0 && contours3.size()>0 && min_num1 < contours1.size())
	{
    copter_box4 = boundingRect(contours1[min_num1]);
    int centerof_box4_x = copter_box4.x + (copter_box4.width/2);
    int centerof_box4_y = copter_box4.y + (copter_box4.height/2);
    int centerof_box3_x=0;
    int centerof_box3_y=0;
    for(int t=0; t<contours3.size(); t++)
    {
        bool check_exception=true;
        copter_box3 = boundingRect(contours3[t]);
        centerof_box3_x = copter_box3.x + (copter_box3.width/2);
        centerof_box3_y = copter_box3.y + (copter_box3.height/2);
        if(centerof_box3_x >= copter_box4.x && centerof_box3_x <= copter_box4.x+copter_box4.width && centerof_box3_y >= copter_box4.y && centerof_box3_y <= copter_box4.y+copter_box4.height)
        {
            check_exception = false;
        }
        if(check_exception  && contours3[t].size() < contours1[min_num1].size() && t != min_num2 )
        {
            int diff = abs(centerof_box4_x - centerof_box3_x) + abs(centerof_box4_y - centerof_box3_y);
            if(diff<min_diff2){
                min_diff2 = diff;
                min_num3 = t;
            }        
        }
    }
    if(min_num3 <contours3.size())
    {
        copter_box3 = boundingRect(contours3[min_num3]);
        copter.Head_vector =  Return_HeadWay(copter_box3, copter_box4);
    }
	}
#if DEBUG
    cout << "Copter.Head_Vector : " << copter.Head_vector << endl;
#endif


#if DEBUG
    end=(double)cvGetTickCount();
	t1 = (end-start)/((double)cvGetTickFrequency()*1000.);
	printf("for loop : %g ms\n",t1);

	start =(double)cvGetTickCount();
#endif
	if(min_diff > 10000) {
        copter.not_found_count++;	
		switch(copter.vector)
		{
				case 1 : copter.width += (copter.width>>1); 
						 //cout << copter.number << " :   1 " << endl;
						 break;
				case 2 : copter.width += (copter.width>>1);
						 copter.y -= (copter.height>>1); 
						 copter.height += (copter.height>>1); 
						 //cout << copter.number << " :   2 " << endl;
						 break;
				case 3 : copter.y -= (copter.height>>1); 
						 copter.height += (copter.height>>1); 
						 //cout << copter.number << " :   3 "<< endl;
						 break;
				case 4 : copter.x -= (copter.width>>1);
						 copter.width +=(copter.width>>1);
						 copter.y -= (copter.height>>1);
						 copter.height += (copter.height>>1);
						 //cout << copter.number << " :   4 "<< endl;
						 break;
				case 5 : copter.x -= (copter.width>>1);
						 copter.width += (copter.width>>1);
						 //cout << copter.number << " :   5 "<< endl;
						 break;
				case 6 : copter.x -= (copter.width>>1);
						 copter.width += (copter.width>>1);
						 copter.height += (copter.height>>1); 
						 //cout << copter.number << " :   6 "<< endl;
						 break;
				case 7 : copter.height += (copter.height>>1); 
						 //cout << copter.number << " :   7 "<< endl;
						 break;
				case 8 : copter.height += (copter.height>>1);
						 copter.width += (copter.width>>1); 
						 //cout << copter.number << " :   8 "<< endl;
						 break;
				default : break;
		}
		Reposition_minus(copter);
        if(copter.not_found_count >=FIND_THRESHOLD)
        { 
            copter.not_found_count = 0 ;
            copter.Flag = false; 
        }
        return;
    }//확인요망
    else { copter.not_found_count = 0; }
    copter_box1 = boundingRect(contours1[min_num1]);
#if DEBUG
	cout << " *************************Width/Height :>  " << copter_box1.width << " / " <<copter_box1.height<<endl;
#endif
	if(min_diff<7*(copter_box1.width + copter_box1.height))
    {
      //  cout << "Found!"<<endl; 
		x1 = Calib_X[copter_box1.y][copter_box1.x];	
		x2 = Calib_X[copter_box1.y][copter_box1.x+copter_box1.width-1];
		y1 = Calib_Y[copter_box1.y][copter_box1.x];
		y2 = Calib_Y[copter_box1.y+copter_box1.height-1][copter_box1.x];
		
		copter.radius_X = x2-x1;
		copter.radius_Y = y2-y1+1;

		Save_prev(copter.prev_cx,copter.prev_cy);

        copter.prev_cx[0] = copter.center_x;
		copter.prev_cy[0] = copter.center_y;

        if(copter.prev_cx[V_BUFFER_SIZE-1] == -5000 && copter.prev_cy[V_BUFFER_SIZE-1] == -5000)
        {
            for(int i=1; i<V_BUFFER_SIZE; i++)
            {
                copter.prev_cx[i] = copter.prev_cx[0];
                copter.prev_cy[i] = copter.prev_cy[0];
            }
        }


        copter.x = copter_box1.x+copter.x - (copter_box1.width<<2);
        copter.y = copter_box1.y+copter.y - (copter_box1.height<<2);
        copter.width = copter_box1.width*9;
        copter.height = copter_box1.height*9;
		

        Reposition_minus(copter);
	pthread_mutex_lock(&mutex);
        Take_Picture(copter);
	pthread_mutex_unlock(&mutex);
		
		copter.center_x = copter.x+(copter.width/2);
		copter.center_y = copter.y+(copter.height/2);
     /*   
       cout<<"X : ["<<copter.center_x<<"] "; 
        for(int ii=0; ii<V_BUFFER_SIZE;ii++)
            cout <<"[" <<copter.prev_cx[ii]<<"] ";
        cout << endl;
       cout<<"Y : ["<<copter.center_y<<"] "; 
        for(int ii=0; ii<V_BUFFER_SIZE;ii++)
            cout <<"[" <<copter.prev_cy[ii]<<"] ";
        cout << endl;
*/
		int diff_x = copter.center_x - copter.prev_cx[V_BUFFER_SIZE-1];
		int diff_y = copter.center_y - copter.prev_cy[V_BUFFER_SIZE-1];
        copter.X_v = diff_x;
        copter.Y_v = diff_y;


        

		if(diff_x >MOVE_THRESHOLD && diff_y >MOVE_THRESHOLD) copter.vector =8;
		else if(diff_x>MOVE_THRESHOLD && diff_y<=MOVE_THRESHOLD && diff_y>-MOVE_THRESHOLD) copter.vector =1;
		else if(diff_x>MOVE_THRESHOLD && diff_y<=-MOVE_THRESHOLD) copter.vector =2;
		else if(diff_x<=MOVE_THRESHOLD && diff_x>-MOVE_THRESHOLD && diff_y<=-MOVE_THRESHOLD) copter.vector =3;
		else if(diff_x<=-MOVE_THRESHOLD && diff_y <= -MOVE_THRESHOLD) copter.vector =4;
		else if(diff_x<=-MOVE_THRESHOLD && diff_y >-MOVE_THRESHOLD && diff_y<=MOVE_THRESHOLD) copter.vector =5;
		else if(diff_x<=-MOVE_THRESHOLD && diff_y >MOVE_THRESHOLD) copter.vector =6;
		else if(diff_x<=MOVE_THRESHOLD && diff_x >-MOVE_THRESHOLD && diff_y>MOVE_THRESHOLD) copter.vector =7;
		else copter.vector=0;
		
		switch(copter.vector)
		{
				case 1 : copter.width += (copter.width>>1); 
						 //cout << copter.number << " :   1 " << endl;
						 break;
				case 2 : copter.width += (copter.width>>1);
						 copter.y -= (copter.height>>1); 
						 copter.height += (copter.height>>1); 
						 //cout << copter.number << " :   2 " << endl;
						 break;
				case 3 : copter.y -= (copter.height>>1); 
						 copter.height += (copter.height>>1); 
						 //cout << copter.number << " :   3 " << endl;
						 break;
				case 4 : copter.x -= (copter.width>>1);
						 copter.width +=(copter.width>>1);
						 copter.y -= (copter.height>>1);
						 copter.height += (copter.height>>1);
						 //cout << copter.number << " :   4 " << endl;
						 break;
				case 5 : copter.x -= (copter.width>>1);
						 copter.width += (copter.width>>1);
						 //cout << copter.number << " :   5 " << endl;
						 break;
				case 6 :  copter.x -= (copter.width>>1);
						  copter.width += (copter.width>>1);
						  copter.height += (copter.height>>1); 
						 //cout << copter.number << " :   6 " << endl;
						  break;
				case 7 : copter.height += (copter.height>>1); 
						 //cout << copter.number << " :   7 " << endl;
						 break;
				case 8 : copter.height += (copter.height>>1);
						 copter.width += (copter.width>>1); 
						 //cout << copter.number << " :   8 " << endl;
						 break;
						 
				default : break;
		}
#if RADIUS
	if(copter.find_count==CAPTURE_FRAME-1){
	if(copter.number == 1) cout << "Copter1 : " << copter.rad_avr << endl;
	else if(copter.number == 2) cout << "Copter2 : " << copter.rad_avr << endl;
	}
#endif
		Reposition_minus(copter);
       	//pthread_mutex_lock(&mutex); 
        resize(copter.Copter_Layer,copter.Copter_Layer,Size(copter.width,copter.height),0,0,INTER_NEAREST); 
	   //pthread_mutex_unlock(&mutex);
	   if(copter.number == 1){
       		pthread_mutex_lock(&mutex); 
	   		Temp_roi =  Cam_Image(Rect(copter.x, copter.y , copter.width , copter.height));
	   		pthread_mutex_unlock(&mutex);
	   }
	   else if(copter.number == 2){  
       		pthread_mutex_lock(&mutex); 
	   		Temp_roi = Cam_Image(Rect(copter.x, copter.y , copter.width , copter.height));
	   		pthread_mutex_unlock(&mutex);
	   }
	   copter.Copter_Layer = Temp_roi.clone();
	   copter.Flag = true;
       copter.find_count++;
#if DEBUG
	end=(double)cvGetTickCount();
	t1 = (end-start)/((double)cvGetTickFrequency()*1000.);
	printf("etc : %g ms\n",t1);
	start=(double)cvGetTickCount();
	//drawContours(copter.Copter_Layer,contours1,min_num1,Scalar(255,0,0),2,8);
	//drawContours(copter.Copter_Layer,contours2,min_num2,Scalar(255,0,0),1,8);
    end=(double)cvGetTickCount();
	t1 = (end-start)/((double)cvGetTickFrequency()*1000.);
	printf("draw_contour : %g ms\n",t1);
#endif
	}
    else
    {
        copter.Flag = false;
    }
}

void Save_prev(int pcx[], int pcy[])
{
    for(int i=(V_BUFFER_SIZE-2); i>=0; i--)
    {
        pcx[i+1] = pcx[i];
        pcy[i+1] = pcy[i];
    }
}
int Return_HeadWay(Rect box3, Rect box4)
{
    int center_head_X = box3.x + (box3.width/2);
    int center_head_Y = box3.y + (box3.height/2);
    int center_body_X = box4.x + (box4.width/2);
    int center_body_Y = box4.y + (box4.height/2);

    int diff_X = center_head_X - center_body_X;
    int diff_Y = center_head_Y - center_body_Y;

    if(diff_X <2 && diff_X > -2) diff_X =0;
    if(diff_Y <2 && diff_Y > -2) diff_Y =0;

    if(diff_X > 0 && diff_Y == 0) return 1;
    else if(diff_X>0 && diff_Y <0) return 2;
    else if(diff_X == 0 && diff_Y <0) return 3;
    else if(diff_X <0 && diff_Y <0) return 4;
    else if(diff_X <0 && diff_Y ==0) return 5;
    else if(diff_X <0 && diff_Y >0) return 6;
    else if(diff_X == 0 && diff_Y >0) return 7;
    else if(diff_X >0 && diff_Y >0) return 8;
    else
    {
        cout << "Error in Return HeadWay" << endl;
        return -1;
    }
}



void Reposition_minus(Copter& cop)
{
    if(cop.width > 1918) cop.width = 1918;
    if(cop.height > 1078) cop.height = 1078;
    if(cop.x<0) cop.x = 0;
    if(cop.y<0) cop.y = 0;
    if(cop.x>(1920-cop.width-1)) cop.x = (1920-cop.width-2);
    if(cop.y>(1080-cop.height-1)) cop.y = (1080-cop.height-2);
}

void Take_Picture(Copter& copter)
{
	Mat temp;
    if(copter.find_count >= CAPTURE_FRAME )
    {
        resize(copter.Picture3,copter.Picture3,Size(copter.width,copter.height),0,0,INTER_NEAREST);
		temp = Cam_Image(Rect(copter.x,copter.y,copter.width,copter.height));
			

		copter.Picture3 = temp.clone();
        copter.find_count = 0;
    }
}


void Return_True_Position(Copter& cop)
{
		cop.real_cx = Calib_X[cop.center_y][cop.center_x];
		cop.real_cy = Calib_Y[cop.center_y][cop.center_x];
}

void Zposition(Copter& cop)
{
	int ratio_position = Return_Corner(cop.real_cx, cop.real_cy);
	double X_threshold = Return_Threshold_between_altitude(cop.radX_avr,ratio_position,X_VALUE);
	double Y_threshold = Return_Threshold_between_altitude(cop.radY_avr,ratio_position,Y_VALUE);
	//threshold off
	double ratio_X=0;
	double ratio_Y=0;	
	Return_Ratio(ratio_X, ratio_Y, cop.real_cx, cop.real_cy, X_threshold, Y_threshold);
		
	float _radX= (float)cop.radX_avr/ratio_X;
	float _radY= (float)(cop.radY_avr)/ratio_Y;
	float fx = 884.939;
	float fy = 883.896;
	float Widt, Heig;
	
	//cout << "RATIO X : " << ratio_X << endl;
	//cout << "RATIO Y : " << ratio_Y << endl;
	if(cop.number == 1){
			Widt = 2.5;
			Heig = 2.5;
	}
	else if(cop.number == 2){
			Widt = 2.5;
			Heig = 2.5;
	}

	float Zx = fx*Widt/(_radX/2);
	float Zy = fy*Heig/(_radY/2);

	cop.space_X = ((Zx*(cop.real_cx-(CAMSIZE_X>>1)))/fx);
	cop.space_Y = ((Zy*(cop.real_cy-(CAMSIZE_Y>>1)))/fy);


	/*cout << "                          cop.real_cX : " << cop.real_cx  << endl;
	cout << "  			   cop.real_cY : " << cop.real_cy << endl;
	cout << "ratio_X : " << ratio_X << "  /Y : " << ratio_Y<< endl;
	cout << "                          radX : " << cop.radX_avr << endl;
	cout << "_radX :    " <<_radX << endl;
	cout << "                          radY : " << cop.radY_avr << endl;
	cout << "_radY :    " <<_radY << endl;
	cout << "Zx : " << Zx << " / Zy : " << Zy << endl;
	*/
	float Zreal = ((Zx + Zy)/2);
#if DEBUG
	if(cop.number == 1){
		cout << "Copter 1 " <<  endl;
		cout << "                          radX : " << cop.radX_avr << endl;
		cout << "_radX :    " <<_radX << endl;
		cout << "                          radY : " << cop.radY_avr << endl;
		cout << "_radY :    " <<_radY << endl;
		cout << "Zx : " << Zx << " / Zy : " << Zy << endl;
	
	}
	else if(cop.number == 2){
		cout << "Copter 2 " << endl;
		cout << "                          radX : " << cop.radX_avr << endl;
		cout << "_radX :    " <<_radX << endl;
		cout << "                          radY : " << cop.radY_avr << endl;
		cout << "_radY :    " <<_radY << endl;
		cout << "Zx : " << Zx << " / Zy : " << Zy << endl;
	}
#endif
	cop.Z = Zreal;
}

void Return_Ratio(double& ratio_X, double& ratio_Y, int cx, int cy, double X_th, double Y_th)
{
	int CenterX = (CAMSIZE_X>>1);
	int CenterY = (CAMSIZE_Y>>1);
	double diff_X, diff_Y, diff_X2; //How far from Cx,Cy
	if(cx<CenterX-240) diff_X = (abs((float)cx-(CenterX-240))/(CenterX-240));
	else if(cx>CenterX+100) diff_X = (abs((float)cx-(CenterX+100))/(CenterX-100));
	else
		diff_X = 0;
	if(cx>CenterX+732) diff_X2 = (abs((float)cx-(CenterX+700))/(CenterX-700));
	else
		diff_X2 = 0;

	diff_Y = (abs((float)cy-CenterY)/(CenterY));
	
	if(cx<CenterX){
		ratio_X = 1-((0.111*diff_X)-(X_th*diff_X));
	//	if(cy<CenterY)
	//		ratio_X+=0.02*diff_Y;
	}
	else if(cx>=CenterX)
	{
		ratio_X = 1-((0.11*diff_X));//-(X_th*diff_X));
	}

	if(cy<CenterY)
	{
		if(cx>=CenterX){
			ratio_Y = 1-((0.12*diff_X)-(Y_th*diff_Y));
			ratio_Y -= ((0.1*diff_X2));
		}
		else
			ratio_Y = 1-((0.087*diff_X));
	}
	else if(cy>=CenterY)
	{
		ratio_Y = 1-((0.07*diff_Y)-(Y_th*diff_Y));
		if(ratio_Y>1) ratio_Y=1;
		if(cx>=CenterX){
			ratio_Y-=((0.14*diff_X2));
			ratio_Y-=((0.02*diff_X));
		}
		else
			ratio_Y-=(/*0.085*/0.08*diff_X);
	}

	if(ratio_X>1) ratio_X=1;
	if(ratio_Y>1) ratio_Y=1;

}

int Return_Corner(int cx, int cy)
{
	if(cx > COL_CUT2 && cy < ROW_CUT1) return 1;
	else if(cx < COL_CUT1 && cy < ROW_CUT1) return 2;
	else if(cx < COL_CUT1 && cy > ROW_CUT2) return 3;
	else if(cx > COL_CUT2 && cy > ROW_CUT2) return 4;
	else if(cx > COL_CUT1 && cx < COL_CUT2 && cy > ROW_CUT1 && cy <ROW_CUT2) return 5;
	else if(cx > COL_CUT2 && cy > ROW_CUT1 && cy <ROW_CUT2) return 6;
	else if(cx > COL_CUT1 && cx < COL_CUT2 && cy > ROW_CUT2) return 7;
	else if(cx < COL_CUT1 && cy > ROW_CUT1 && cy <ROW_CUT2) return 8;
	else 
		return 5;
}

double Return_Threshold_between_altitude(float rad_avr, int Corner, bool X)
{
	if(X)
	{
		switch(Corner)
		{
			case 1: return ((44-rad_avr)*0.002); break;
			case 2: return ((46.7-rad_avr)*0.00251); break;
			case 3: return ((45-rad_avr)*0.001); break;
			case 4: return ((43.5-rad_avr)*0.0074); break;
			case 5: return 0; break;
			case 6: return ((41-rad_avr)*0.008); break;
			case 7: return ((49-rad_avr)*0.0016); break;
			case 8: return ((44-rad_avr)*0.0028); break;
			default: cout<<"Error in Return_Threshold"<<endl;
					 return 0;
					 break;
		}
	}
	else
	{
		switch(Corner)
		{
			case 1: return ((44-rad_avr)*0.0015); break;
			case 2: return 0; break;
			case 3: return ((43.8-rad_avr)*0.0001); break;
			case 4: return ((42.8-rad_avr)*0.0014); break;
			case 5: return 0; break;
			case 6: return ((42-rad_avr)*0.0075); break;
			case 7: return ((45-rad_avr)*0.0016); break;
			case 8: return ((44-rad_avr)*0.0019); break;
			default: cout<<"Error in Return_Threshold Y"<<endl;
					 return 0;
					 break;
		}
	}
}
