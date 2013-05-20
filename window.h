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
#include<string.h>


#define WIN_WIDTH 25    //is the window width
#define WIN_LENGTH 20   //is the length of a basic unit of rectangle in window
#define WIN_Y_POSN 250   //Y co-ordinate of the top left of the window
#define LEFT 20		//X co-ordinate of the top left of tx window
#define RIGHT 480	//X co-ordinate of the top left of rx window

#define MAXSIZE 20		//max size of the window
			//increase this max size if segmentation fault comes up
#define DELAY 1


struct contents{		//contents of the window data structure

	char left[MAXSIZE];
	char middle[MAXSIZE];
	char right[MAXSIZE];
};

struct information{  //information that will be extracted from contents
	int left[MAXSIZE];	//actual data in integer form
	int middle[MAXSIZE];
	int right[MAXSIZE];
	int len[3];   //respective lengths

};

enum posn{ left = LEFT, right = RIGHT};

class Window{


 private: //will be changed to private later
	posn pos_win ;
	struct contents ip_data;
	const struct information *extract(const struct contents &ip_data);
	void draw_win();

 public:

	void take_input(posn winType,char *Lstr, char *Mstr, char *Rstr){
		pos_win = winType;
		strcpy(ip_data.left,Lstr);
		strcpy(ip_data.middle,Mstr);
		strcpy(ip_data.right,Rstr);
		draw_win();
	}

};


void Window::draw_win(){
	struct information *op_data;
	char pck_str_no[10];
	int len_win = 0;
	int x_cord = pos_win;


	op_data = (struct information *)extract(ip_data);
	len_win = op_data->len[0] + op_data->len[1] + op_data->len[2];
	for(int cnt = 0;cnt < op_data->len[0]; cnt++){
		setcolor(GREEN);
		rectangle(x_cord,WIN_Y_POSN,x_cord+WIN_LENGTH,WIN_Y_POSN+WIN_WIDTH);


		itoa(op_data->left[cnt],pck_str_no,10);
		settextstyle(SMALL_FONT, HORIZ_DIR, 4);
		outtextxy(x_cord+WIN_LENGTH/2,WIN_Y_POSN+WIN_WIDTH/2-5,pck_str_no);
		x_cord = x_cord + WIN_LENGTH;
	}
	int slwin_x_top = x_cord;
	for(cnt = 0;cnt < op_data->len[1]; cnt++){
		setcolor(CYAN);
		rectangle(x_cord,WIN_Y_POSN,x_cord+WIN_LENGTH,WIN_Y_POSN+WIN_WIDTH);
		itoa(op_data->middle[cnt],pck_str_no,10);
		settextstyle(SMALL_FONT, HORIZ_DIR, 4);
		outtextxy(x_cord+WIN_LENGTH/2,WIN_Y_POSN+WIN_WIDTH/2-5,pck_str_no);
		x_cord = x_cord + WIN_LENGTH;
	}
	int slwin_x_bot = x_cord;
	setcolor(CYAN);
	rectangle(slwin_x_top,WIN_Y_POSN-10,slwin_x_bot,WIN_Y_POSN+WIN_WIDTH+10);

	for(cnt = 0;cnt < op_data->len[2]; cnt++){
		setcolor(RED);
		rectangle(x_cord,WIN_Y_POSN,x_cord+WIN_LENGTH,WIN_Y_POSN+WIN_WIDTH);


		itoa(op_data->right[cnt],pck_str_no,10);

		settextstyle(SMALL_FONT, HORIZ_DIR, 4);
		outtextxy(x_cord+WIN_LENGTH/2,WIN_Y_POSN+WIN_WIDTH/2-5,pck_str_no);
		x_cord = x_cord + WIN_LENGTH;
	}
	// following statements are needed !!
	setcolor(CYAN);
	rectangle(slwin_x_top,WIN_Y_POSN-10,slwin_x_bot,WIN_Y_POSN+WIN_WIDTH+10);

}




const struct information* Window::extract(const struct contents &ip_data){

		char dummy[5];	//this will contain a string which i will
				//subsequently convert into a integer

		struct information *op_data;
		int c = 0;	//cntr for character i/p(left/mid/right)
		int i = 0;	//cntr for integers
		int tmp = 0;    //temporary counter
		char *ip_ptr;
		int *op_ptr;


		op_data = (struct information*) new (struct information);
		for(int cnt = 1;cnt <=3 ;cnt++){
			c = 0;
			i = 0;
			tmp = 0;
			if(cnt == 1){
				ip_ptr = (char *)ip_data.left;
				op_ptr = op_data->left;
			}
			if(cnt == 2){
				ip_ptr = (char *)ip_data.middle;
				op_ptr = op_data->middle;
			}
			if(cnt == 3){
				ip_ptr = (char *)ip_data.right;
				op_ptr = op_data->right;
			}
			while(ip_ptr[c]!='\0'){
				if(ip_ptr[c] == '-'){
					 //char b = dummy[tmp-1];
					dummy[tmp] = '\0';
					op_ptr[i] = atoi(dummy);
				  //	printf("%d data entered \n",op_ptr[i]);
					tmp = 0;
					i++;	//increment it to point to next location in the integer array
					c++;
					continue;
				}
				dummy[tmp] = ip_ptr[c];
				tmp++;
				c++;

			}
			op_data->len[cnt-1] = i++;



		}
		return(op_data);
	}


void ini(){
		  int gdriver = DETECT, gmode, errorcode;

	   /* initialize graphics and local variables */
	  initgraph(&gdriver, &gmode, "");

	   /* read result of initialization */
	   errorcode = graphresult();
	   if (errorcode != grOk)  /* an error occurred */
	   {
	      printf("Graphics error: %s\n", grapherrormsg(errorcode));
	      printf("Press any key to halt:");
	      getch();
	      exit(1); /* terminate with an error code */
	   }
}


/*
main(){
	clrscr();
	Window obj;
	ini();


	obj.take_input(right,"1-2-3-","4-5-","6-7-");
	getch();
*/
      /*	op_data = (struct information *)obj.extract(dummy);
	for(int i = 0;i< op_data->len[2];i++)
		printf("%d,",op_data->right[i]);*/
/*
}
*/


