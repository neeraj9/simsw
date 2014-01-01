/**
 * SimSW: Simulate Sliding Window Protocol
 *
 * See LICENSE for license information.
 *
 * copyright (c) 2001 Neeraj Sharma <neeraj.sharma@alumni.iitg.ernet.in>
 */

#include<graphics.h>
#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
#include<dos.h>


#define COOL_COLOR 2  //it is green actually
#define TROUBLE_COLOR 14 //it is yellow
#define HOPELESS_COLOR 4 //it is red


const int SIZE_INFO = 30 ;
const int PCKT_WIDTH = 15 ;
const int PCKT_LENGTH = 35 ;

struct data     	//contains the data for the structure
{
	char status_destroyed_pck;        // by default wont be destroyed
	char status_destroyed_ack;        // by default wont be destroyed
	char status_corrupted_pck;        //by default wont be corrupted
	char status_corrupted_ack;        //by default wont be corrupted
	int pck_num;
	int ack_num;
	int X_start1,Y_start1;		//the path for object 1
	int X_dest1,Y_dest1;		//the path for object 1
	int X_start2,Y_start2;		//the path for object 2
	int X_dest2,Y_dest2;		//the path for object 2
	int cnt_obj;			//the count for the objects to move
};



class Animation{
 private:
	char input_path[SIZE_INFO]; 	//will store the path of the object
	int delay_amt;
	char status_destroyed_pck;        // by default wont be destroyed
	char status_destroyed_ack;        // by default wont be destroyed
	char status_corrupted_pck;        //by default wont be corrupted
	char status_corrupted_ack;        //by default wont be corrupted

	int pck_num;
	int ack_num;

	int X_start1,Y_start1;		//the path for object 1
	int X_dest1,Y_dest1;		//the path for object 1
	int X_start2,Y_start2;		//the path for object 2
	int X_dest2,Y_dest2;		//the path for object 2
	int cnt_obj;			//the count for the objects to move
	void *pckt;		        //image pointer for the packet
	void *ack;			//image pointer for the ack
	int img_size;

	int maxx;

	void start_animation_1();
	void drawimg_and_setcolor(){

	      char pck_str_no[5];
	      char ack_str_no[5];
	      int color_pck = status_destroyed_pck * HOPELESS_COLOR +
			status_corrupted_pck * TROUBLE_COLOR;
	      color_pck = color_pck==0 ? GREEN : color_pck;

	      int color_ack = status_destroyed_ack * HOPELESS_COLOR +
			status_corrupted_ack * TROUBLE_COLOR;
	      color_ack = color_ack==0 ? GREEN : color_ack;

	      if(cnt_obj==1 || cnt_obj==2){
	      itoa(pck_num,pck_str_no,10);
	      /* draw the 3-d bar */
	      setcolor(color_pck);
//	      setfillstyle(6,4); //	6 is good 9 is best 10 is also good
	      rectangle(X_start1,Y_start1, X_start1+PCKT_LENGTH,
		    Y_start1+PCKT_WIDTH);
	      settextstyle(SMALL_FONT, HORIZ_DIR, 4);
	      outtextxy(X_start1+PCKT_LENGTH/2,Y_start1+PCKT_WIDTH/2-5,pck_str_no);
//	      img_size = imagesize(X_start1,Y_start1, X_start1+PCKT_LENGTH,
//		    Y_start1+PCKT_WIDTH);

//	      pckt = malloc(img_size);
	      }

	      if(cnt_obj==2 || cnt_obj==3){
		      itoa(ack_num,ack_str_no,10);
		      setcolor(color_ack);
		      rectangle(X_start2,Y_start2, X_start2+PCKT_LENGTH,
			   Y_start2+PCKT_WIDTH/*,2.5,1*/);
		      settextstyle(SMALL_FONT, HORIZ_DIR, 4);
		      outtextxy(X_start2+PCKT_LENGTH/2,Y_start2+PCKT_WIDTH/2-5,ack_str_no);
		      getimage(X_start2,Y_start2, X_start2+PCKT_LENGTH,
			   Y_start2+PCKT_WIDTH,ack);
//		      ack = malloc(img_size);
	      }
	    //  getch();
	      /* output a messsage */
	  }

	 void start_animation_2();	//declaration




 public:

	Animation(int d_mnt){
		delay_amt = d_mnt;
		status_destroyed_pck=1;        // by default wont be destroyed
		status_corrupted_pck=0;        //by default wont be corrupted
		status_destroyed_ack=0;        // by default wont be destroyed
		status_corrupted_ack=1;        //by default wont be corrupted

		cnt_obj = 2;
		X_start1 = 150  ;
		Y_start1 = 30 ;
		X_dest1 = 70;
		Y_dest1 = 30;

		X_start2 = 2  ;
		Y_start2 = 70 ;
		X_dest2 = 80;
		Y_dest2 = 70;

//	      img_size = imagesize(X_start1,Y_start1, X_start1+PCKT_LENGTH,
//		    Y_start1+PCKT_WIDTH);
		img_size = PCKT_WIDTH * PCKT_WIDTH;
	      pckt = malloc(img_size);
	      ack = malloc(img_size);


	}
	~Animation() { free(pckt); free(ack); }
	void take_input(struct data &ip_data)
	{
		status_destroyed_pck = ip_data.status_destroyed_pck;        // by default wont be destroy
		status_corrupted_pck = ip_data.status_corrupted_pck;	//by default wont be corrupted
		status_destroyed_ack = ip_data.status_destroyed_ack;        // by default wont be destroyed
		status_corrupted_ack = ip_data.status_corrupted_ack;        //by default wont be corrupted
		pck_num = ip_data.pck_num;
		ack_num = ip_data.ack_num;

		cnt_obj = ip_data.cnt_obj;
		X_start1 = ip_data.X_start1;
		Y_start1 = ip_data.Y_start1;
		X_dest1 = ip_data.X_dest1;
		Y_dest1 = ip_data.Y_dest1;

		X_start2 = ip_data.X_start2;
		Y_start2 = ip_data.Y_start2;
		X_dest2 = ip_data.X_dest2;
		Y_dest2 = ip_data.Y_dest2;

	}

	void start_draw();


};





 void Animation::start_animation_1(){
	      int move_by = 3;
	      char reached_end = 0;

	      if(X_dest1<=X_start1)
		   move_by = (-1)*move_by;

	      while (!reached_end)
		{
		   getimage( X_start1,Y_start1, X_start1+PCKT_LENGTH,
		    Y_start1+PCKT_WIDTH,pckt);

		   /* erase old image */
		   putimage(X_start1, Y_start1, pckt, XOR_PUT);

		   X_start1 += move_by;
		   if (abs(X_start1 - X_dest1)<=abs(move_by))
		       reached_end = 1;

		   /* plot new image */
		   putimage(X_start1,Y_start1, pckt, XOR_PUT);
		   delay(delay_amt);
		   delay(delay_amt);
		}

	 }

 void Animation::start_animation_2(){
	      char reached_end1= 0;
	      char reached_end2= 0;
	      int move_by1 = 1;
	      int move_by2 = 1;
	      if(X_dest1<=X_start1)
		   move_by1 = (-1)*move_by1;
	      if(X_dest2<=X_start2)
		   move_by2 = (-1)*move_by2;


	      while ((!reached_end1 && cnt_obj <=2) || !reached_end2)
	      {
		 if (cnt_obj == 1 || cnt_obj == 2)
		    if(!reached_end1)
		    {
			    getimage(X_start1,Y_start1, X_start1+PCKT_LENGTH,
			    Y_start1+PCKT_WIDTH,pckt);

			   /* erase old image */
			   putimage(X_start1, Y_start1, pckt, XOR_PUT);

			   X_start1 += move_by1;
			   if (abs(X_start1 - X_dest1)<=abs(move_by1))
			       reached_end1 = 1;

			   /* plot new image */
			   putimage(X_start1,Y_start1, pckt, XOR_PUT);
			   delay(delay_amt);
		     }

		    if(!reached_end2)
		    {
			    getimage(X_start2,Y_start2, X_start2+PCKT_LENGTH,
			    Y_start2+PCKT_WIDTH,ack);

			   /* erase old image */
			   putimage(X_start2, Y_start2, ack, XOR_PUT);

			   X_start2 += move_by2;
			   if (abs(X_start2 - X_dest2)<=abs(move_by2))
			       reached_end2 = 1;

			   /* plot new image */
			   putimage(X_start2,Y_start2, ack, XOR_PUT);
			   delay(delay_amt);
		     }

		      // delay(delay_amt);
	     }

	 }




	void Animation::start_draw(){
		drawimg_and_setcolor();
		switch(cnt_obj){
			case 1 :
				  start_animation_1();
				  break;
			case 2 :
			case 3 :
				  start_animation_2();
				  break;
		}


	}

/*
main(){
	Animation obj;
	struct data ip_data;

	ip_data.status_destroyed_pck=1;        // by default wont be destroy
	ip_data.status_corrupted_pck= 0;	//by default wont be corrupted
	ip_data.status_destroyed_ack=0;        // by default wont be destroyed
	ip_data.status_corrupted_ack=0;        //by default wont be corrupted
	ip_data.pck_num = 3;
	ip_data.ack_num = 4;

	ip_data.cnt_obj=2;
	ip_data.X_start1=150;
	ip_data.Y_start1=30;
	ip_data.X_dest1=70;
	ip_data.Y_dest1=30;

	ip_data.X_start2=2;
	ip_data.Y_start2=70;
	ip_data.X_dest2=80;
	ip_data.Y_dest2=70;

	obj.take_input(ip_data);
	obj.start_draw();

}
*/
