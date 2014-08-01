#include "Copters.h"

Copter_Packet cp1;
Copter_Packet cp2;

Mat Cam_Image;
Mat Cam_YUV;
Mat Cam_YUV2;
Mat Cam_Binary1_1(CAMSIZE_Y,CAMSIZE_X,CV_8UC1);
Mat Cam_Binary1_2(CAMSIZE_Y,CAMSIZE_X,CV_8UC1);
Mat Cam_Binary2_1(CAMSIZE_Y,CAMSIZE_X,CV_8UC1);
Mat Cam_Binary2_2(CAMSIZE_Y,CAMSIZE_X,CV_8UC1);


int filedes;


//Mat temp=(Mat_<char>(3,3) << 0, -1 , 0, -1 , 5, -1 , 0 ,-1, 0);
extern Copter copter1;
extern Copter copter2;


VideoCapture camera(0);
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // thread initializing
void* Send(void *data)
{
    while(1)
    {
		Send_Packet(cp1,cp2,filedes);
        usleep(100000);
//        cout <<" 4"<<endl;
    }

}
void* Receive_Cam(void *data)
{
	
	while(1)
	{
		pthread_mutex_lock(&mutex);
		//cout << "Thread : before_Cam_Receive"<<endl;
		camera >> Cam_Image;
		//Cam_Image2 = Cam_Image.clone();
		pthread_mutex_unlock(&mutex);
		//Send_Packet(cp1,cp2,filedes);
#if DEBUG 
		cout << "Receive_CAM" << endl;
#endif
//cout << " 3"<<endl;
	}

}

void* cop2_main(void *data)
{
	int buffer_index2=0;
	int Z_buffer_index2=0;

	double start,start2,end,end2;
	while(1)
    {
#if DEUBUG
		start = (double)cvGetTickCount();	
#endif
		switch(copter2.Flag)
        {
        case NOTFOUND:
            {
#if DEBUG
		start2 = (double)cvGetTickCount();	
#endif
		pthread_mutex_lock(&mutex);
                cvtColor(Cam_Image,Cam_YUV2,CV_RGB2YCrCb);
		pthread_mutex_unlock(&mutex);
                Copter_Color(Cam_YUV2,&Cam_Binary2_1,&Cam_Binary2_2,copter2);
                Take_Copter(Cam_Binary2_1,Cam_Binary2_2,copter2);
				//***********Need an algorithm to distinguish the ROI_Image, whether It is a real Copter or not. 
                Check_Copter(copter2);
#if DEBUG
				end2 = (double)cvGetTickCount();
				printf("Not_FOUND_Tik : %g ms\n",(end2-start2)/((double)cvGetTickFrequency()*1000.));
#endif    
				break;
            }


        case TRACKING:
            {
                Check_Copter(copter2);
				buffer_setting(copter2,buffer_index2);
				Return_True_Position(copter2);
                Zposition(copter2);
				Z_buffer_setting(copter2,Z_buffer_index2);
//                print_info(copter2);
				break;
            }
        }
        //cout << "2"<<endl;
		Socket_Data(copter2,cp2);
#if IMSHOW  
		pthread_mutex_lock(&mutex);
		//circle(Cam_Image2,Point(copter2.center_x,copter2.center_y),2,Scalar(0,255,0),2);		
			imshow("3",copter2.Copter_Layer);
				
		pthread_mutex_unlock(&mutex);
#endif
#if DEBUG        
				end = (double)cvGetTickCount();
				printf("check_Copter_Tik : %g ms\n",(end-start)/((double)cvGetTickFrequency()*1000.));
#endif
    }

}

int main()
{
	int buffer_index1=0;
	int Z_buffer_index1=0;
	pthread_t p_thread;
	pthread_t p_thread2;
	pthread_t p_thread3;
cout << "1 " << endl;
	
	if((filedes = open("../monzoPipe",O_WRONLY))<0)
	{
		cout << "fail to call open()" << endl;
		exit(1);
	}

cout << " 2" <<endl;

	int thr_id, thr_id2, thr_id3;
	int a=1;
	int b=1;
	int a2=1;
	int b2=1;
	int a3=1;
	int b3=1;
	
#if IMSHOW
	namedWindow("1",CV_WINDOW_AUTOSIZE);
    namedWindow("2",CV_WINDOW_AUTOSIZE);
    namedWindow("3",CV_WINDOW_AUTOSIZE);
#endif
    camera.set(CV_CAP_PROP_FRAME_HEIGHT,CAMSIZE_Y);
    camera.set(CV_CAP_PROP_FRAME_WIDTH,CAMSIZE_X);
    camera.set(CV_CAP_PROP_FPS,FRAME_SPEED);
    
cout << " 3" <<endl;
	Copter1_init();
    
	while(!copter1.Flag){
        camera >> Cam_Image;
		cvtColor(Cam_Image,Cam_YUV,CV_RGB2YCrCb);
        Copter_Color(Cam_YUV,&Cam_Binary1_1,&Cam_Binary1_2,copter1);
		Take_Copter_Init(Cam_Binary1_1,Cam_Binary1_2,copter1.Pic_Binary1,copter1.Pic_Binary2,copter1);
		Check_Copter(copter1);
		rad_buffer_init(copter1);
		Return_True_Position(copter1);
        Zposition(copter1);
		Z_buffer_init(copter1);
    }
#if COPTER2
	
	Copter2_init();
	
	while(!copter2.Flag){
        camera >> Cam_Image;
		cvtColor(Cam_Image,Cam_YUV2,CV_RGB2YCrCb);
        Copter_Color(Cam_YUV2,&Cam_Binary2_1,&Cam_Binary2_2,copter2);
		Take_Copter_Init(Cam_Binary2_1,Cam_Binary2_2,copter2.Pic_Binary1,copter2.Pic_Binary2,copter2);
		Check_Copter(copter2);
		rad_buffer_init(copter2);
		Return_True_Position(copter2);
        Zposition(copter2);
		Z_buffer_init(copter2);
	}
	cop_packet_init(cp1,copter1);
	cop_packet_init(cp2,copter2);
#endif
	thr_id = pthread_create(&p_thread,NULL,Receive_Cam,(void*)&a);
		sleep(1);
#if COPTER2
	thr_id2 = pthread_create(&p_thread2,NULL,cop2_main,(void*)&a2);
    	sleep(1);
#endif
	thr_id3 = pthread_create(&p_thread2,NULL,Send,(void*)&a3);
    	sleep(1);

#if DEBUG
	double start, end, start2, end2;
#endif
	while(1)
    {
#if DEBUG
		start = (double)cvGetTickCount();	
#endif
		//waitKey(1); 
        switch(copter1.Flag)
        {

        case NOTFOUND:
            {
#if DEBUG
		start2 = (double)cvGetTickCount();	
#endif
                cvtColor(Cam_Image,Cam_YUV,CV_RGB2YCrCb);
                Copter_Color(Cam_YUV,&Cam_Binary1_1,&Cam_Binary1_2,copter1);
                Take_Copter(Cam_Binary1_1,Cam_Binary1_2,copter1);
				//***********Need an algorithm to distinguish the ROI_Image, whether It is a real Copter or not. 
                Check_Copter(copter1);
#if DEBUG
				end2 = (double)cvGetTickCount();
				printf("Not_FOUND_Tik : %g ms\n",(end2-start2)/((double)cvGetTickFrequency()*1000.));
#endif    
				break;
            }


        case TRACKING:
            {
//		pthread_mutex_lock(&mutex);
                Check_Copter(copter1);
//		pthread_mutex_unlock(&mutex);
				buffer_setting(copter1,buffer_index1);
				Return_True_Position(copter1);
                Zposition(copter1);
				Z_buffer_setting(copter1,Z_buffer_index1);
//				cout << "copter1_X : " << copter1.real_cx<<endl;
//				cout << "copter1_Y : " << copter1.real_cy<<endl;
                if(abs(copter1.X_v)>1 || abs(copter1.Y_v)>1 ){
                cout << "               VELOCITY_X : "<< copter1.X_v << endl;
                cout << "               VELOCITY_Y : "<< copter1.Y_v << endl;
                } 
				//imshow("1",copter1.Copter_Layer);
#if IMSHOW 
		pthread_mutex_lock(&mutex);
				imshow("1",copter1.Copter_Layer);
//				imshow("2",copter2.Copter_Layer);
		pthread_mutex_unlock(&mutex);
#endif	
				break;
            }
        }
        //cout << "1"<<endl;
		Socket_Data(copter1,cp1);
#if DEBUG
				end = (double)cvGetTickCount();
				printf("Copter1_while_Tik : %g ms\n",(end-start)/((double)cvGetTickFrequency()*1000.));
#endif
    }
		pthread_join(p_thread,(void **)&b);
#if COPTER2
		pthread_join(p_thread2,(void **)&b2);
#endif
		pthread_join(p_thread3,(void **)&b3);
	return 0;
}


