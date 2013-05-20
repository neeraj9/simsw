/**
 * SimSW: Simulate Sliding Window Protocol
 *
 * See LICENSE for license information.
 *
 * copyright (c) 2001 Neeraj Sharma <neeraj.sharma@alumni.iitg.ernet.in>
 */

#include <iostream.h>
#include <fstream.h>
#include <string.h>

#define debug 1      /* comment it for the graphics */

#include "sliding.h"
#ifndef debug
 #include "window.h"
 #include "packet.h"
#endif


void printHelp(void)
{
	 cout << endl;
	 cout << " Usage: " <<endl;
	 cout << " sim  bits  pe  maxNum " << endl;
	 cout << endl << endl;
	 cout << "      bits    : packet size in bits" << endl;
	 cout << "      pe      : bit error rate" << endl;
	 cout << "      maxNum  : numbering scheme (0 .. maxNum)" << endl;
	 cout << endl;
}




void sSimPacket(sSlidingWindow swin,ofstream &fout)
{
 char swindow[50];
// char s1[40], s3[46];

	swin.getPktNumbers(swindow);
	cout << " Sender's window : " << swindow << endl;
//	cout << " test :" << s1 << ":" << swindow << ":" << s3 << endl;
	fout << " Sender's window : " << swindow << endl;
//	cout << endl;
}

void rSimPacket(rSlidingWindow rwin, ofstream &fout)
{
 char rwindow[50];
// char s1[40], s3[46];

	rwin.getPktNumbers(rwindow);
	cout << " Receiver's window : " << rwindow << endl;
//	cout << " test :" << s1 << ":" << rwindow << ":" << s3 << endl;
	cout << endl;
	fout << " Receiver's window : " << rwindow << endl;
	fout << endl;

}



// function for the sender's window
void sFunction()
{


}

// function for the receiver's window
void rFunction()
{

}

int main(int argc, char *argv[])
{
 float pe;
 int maxNum, bits;
 rSlidingWindow rwin;
 sSlidingWindow swin;
 pktStatus pkt;
 int pnum, anum;
 int timeOut;
 int result,i;
 int ack_flag;
 char ch;
 rNumber rnum;
 ofstream fout;
 pktStatus ackStat;
 char s1[50],s2[50],s3[50];
#ifndef debug
 Window senderWin, receiverWin;
 Animation obj(10);
 struct data ip_data;

#endif

//	clrscr();
	fout.open("sliding.txt"); // open file for writing
	if (argc < 3)
	{
	 printHelp();
   exit(1);
  }
  else
  {
    bits = atoi(argv[1]);
    pe = atoi(argv[2]);
    maxNum = atoi(argv[3]);
    timeOut = 10;  // min value is 2
	  fout << " Time Out " << timeOut << endl << endl;
	}
/* initialize sender */
	swin.setPara(bits,maxNum,maxNum-1,timeOut);
	swin.setErrorDistribution(pe);
	sSimPacket(swin,fout);
/* initialize receiver */
	rwin.setPara(bits,maxNum,maxNum-4);
	rwin.setErrorDistribution(pe);
	rSimPacket(rwin,fout);

#ifndef debug
	ini();
	swin.getStrings(s1,s2,s3);
	rwin.getStrings(s1,s2,s3);
	senderWin.take_input(left,s1,s2,s3);
	receiverWin.take_input(right,s1,s2,s3);

	ip_data.cnt_obj=2; // activate both initially
// packet
	ip_data.X_start1=2;
	ip_data.Y_start1=200;
	ip_data.X_dest1=480;
	ip_data.Y_dest1=200;
// ack
	ip_data.X_start2=480;
	ip_data.Y_start2=300;
	ip_data.X_dest2=2;
	ip_data.Y_dest2=300;



	obj.take_input(ip_data);
//	obj.start_draw();

#endif


	ch = 'g';
	ack_flag = RESET;

	while (ch != 'x' && ch != 'X')
//	while (!kbhit())
	{

//	 sFunction();
//	 sSimPacket(swin);
	 if (ack_flag == SET)
	 {
#ifdef debug
	  cout << " received ack #" << anum << " status = ";
	  cout << stat_chr[ackStat] << endl;
#endif
	  fout << " received ack #" << anum << " status = ";
	  fout << stat_chr[ackStat]<<endl;
	  if (ackStat == OK)
	   result = swin.receive(anum,ACK);
/*	  cout <<"  result =" << result<< endl;
	  fout <<"  result =" << result<< endl;
*/
	  ack_flag = RESET;
	 }
	 pnum = swin.dataSend(pkt); // pkt is Status of data packet sent
	 if (pnum != ERROR && pnum != CONNECTION_LOST)
	 {
#ifdef debug
	  cout << " packet #" << pnum <<" sent; status = " << stat_chr[pkt];
	  cout << endl;
#endif
	  fout << " packet #" << pnum <<" sent; status = " << stat_chr[pkt];
	  fout << endl;
	 }
	 else
	  if (pnum == CONNECTION_LOST)
	  {
#ifdef debug
	   cout << " Destination unreachable !! " << endl;
#endif
	   fout << " Destination unreachable !! " << endl;
#ifndef debug
	   closegraph();
#endif
	   return 0;
	  }
#ifdef debug
	  sSimPacket(swin,fout);
#else
	if (pnum != ERROR && pnum != CONNECTION_LOST )
	 cleardevice();
	swin.getStrings(s1,s2,s3);
	senderWin.take_input(left,s1,s2,s3);
	rwin.getStrings(s1,s2,s3);
	receiverWin.take_input(right,s1,s2,s3);


	ip_data.status_destroyed_pck= (pkt==DESTROYED);
	ip_data.status_corrupted_pck= (pkt==CORRUPT);
	ip_data.pck_num = pnum;
	if (pnum >= 0)
	{
	 ip_data.cnt_obj = 1;
	 obj.take_input(ip_data);
	 obj.start_draw();
	}

#endif


//	 getch();
//	 sleep(1);
//	 if (rnum.get_rand() >= 0.5)
	 if (pnum != ERROR && pnum != CONNECTION_LOST)
	 {
	  if (pkt == OK)
	   result = rwin.receive(pnum,fout);
#ifdef debug
	  cout << " received data pkt# "<< pnum << " status = ";
	  cout << stat_chr[pkt] << endl;
#endif
	  fout << " received data pkt# "<< pnum << " status = ";
	  fout << stat_chr[pkt] << endl;
	 }
	 if ((pkt == CANNOT_SEND) || (pkt == DESTROYED))
	 {
	  anum = rwin.pktSend(ACK,ackStat);   //ackStat is status of ack
	  if (anum != ERROR)
	  {
	   ack_flag = SET;
#ifdef debug
	   cout << " sending ACK #" << anum;
	   cout << " . status = " << stat_chr[ackStat] << endl;
#endif
	   fout << " sending ACK #" << anum;
	   fout << " . status = " << stat_chr[ackStat] << endl;
	  }
	 }

//	 cout << " operation @ receiver ="<< result << ".  ";
#ifdef debug
	 rSimPacket(rwin,fout);
#else
	cleardevice();
	swin.getStrings(s1,s2,s3);
	senderWin.take_input(left,s1,s2,s3);
	rwin.getStrings(s1,s2,s3);
	receiverWin.take_input(right,s1,s2,s3);


	ip_data.status_destroyed_ack= (ackStat == DESTROYED);
	ip_data.status_corrupted_ack= (ackStat == CORRUPT);
	ip_data.ack_num = anum;

	if (ack_flag == SET)
	{
	 ip_data.cnt_obj = 3;
	 obj.take_input(ip_data);
	 obj.start_draw();
	}

#endif

//	 sleep(1);
//	 ch = getch();
//	 rFunction();
//	 simPacket();
	 if (1)
	 {
//	  ch = getch();
//	  if (ch == 'p' || ch =='P')
//	   ch = getch();
	 }
	}
	fout.close();
#ifndef debug
	closegraph();
#endif
	return 0;
}
