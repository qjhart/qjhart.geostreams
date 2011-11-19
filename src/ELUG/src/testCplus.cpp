/************************************************/
/*** This file is to test conversion between 
   * Pixel/Line and Lat/Lon for GOES data
***/

#include <stdlib.h>
#include <iostream>

#include "ELUG.h"

using namespace std;

int main(int argc, char** argv) {

   float pixel; 
   float line;
   float lon; 
   float lat;
   float rlon;
   float rlat;


   float Pixel[2][2][2];
   float Line[2][2][2];
   int flipflagIndex;
   int imcIndex;
   int instrIndex;

   Pixel[0][0][0] =  10405.39;
   Line[0][0][0] = 3487.36;
   Pixel[0][0][1] =  1162.99;
   Line[0][0][1] = 1219.35;
   Pixel[0][1][0] =  10282.76;
   Line[0][1][0] = 3626.88;
   Pixel[0][1][1] =  1152.22;
   Line[0][1][1] = 1238.93;

   Pixel[1][0][0] =  10405.39;
   Line[1][0][0] = 3487.36;
   Pixel[1][0][1] =  1162.87;
   Line[1][0][1] = 1219.41;
   Pixel[1][1][0] =  10267.155273;
   Line[1][1][0] = 3617.920410;
   Pixel[1][1][1] =  1151.16;
   Line[1][1][1] = 1238.05;

   Elug testElug;

   double epoch;
   double time; 
   double subsat_lat;
   double subsat_lon;
   float rrlat;
   float rrlon;
   float elev;
   float scan;

   int flipflag;   // 1=normal    -1=inverted
   int imc;          // 1=disaled 0=enabled
   int instr;       // 1=imager   2=sounder

   int ierr;


/*** Users will do this with actual Rec data 
   extern float one_rec[];
   testElug.setRec(one_rec);
***/

   /*** set constants ***/
   testElug.setcons(4,3068,2,3068,4,1402,2,1402);

   /*** compute epoch time ***/
   epoch= testElug.timex(1994,299,00,00,0.0);
   time=  testElug.timex(1994,299,16,05,0.0);

   cout<<"epoch = "<<epoch<<"  time = "<<time << endl;

   epoch = (float)20557829.57612;
   time = epoch + (float)20.;

   cout<<"epoch = "<<epoch<<"  time = "<<time << endl;
 
   for (flipflag = 1; flipflag >= -1; flipflag -=2) {
       testElug.setFlipflag(flipflag);

       if (flipflag ==1) 
	 flipflagIndex = 1;
       else 
	 flipflagIndex = 0;

       for (imc = 0; imc <=1; imc++) {
	 testElug.setImc(imc);
	 imcIndex = imc;

	 testElug.lmodel(time, epoch, imc);
	 subsat_lon = testElug.getSubsatLon();
	 subsat_lat = testElug.getSubsatLat();

	 cout <<"subsat_lat = " <<subsat_lat << " subsat_lon = "
	      << subsat_lon << endl;

	 for (instr = 1; instr <=2; instr++) {
	   testElug.setInstr(instr);

	   instrIndex = instr - 1; 
	   flipflag = testElug.getFlipflag();
	   imc = testElug.getImc();
	   instr = testElug.getInstr();
	   cout<<"instr = "<< instr <<" flipflag =" << flipflag << " imc = " 
	       << imc << endl << endl;

	   line = Line[flipflagIndex][imcIndex][instrIndex];
	   pixel = Pixel[flipflagIndex][imcIndex][instrIndex];

	   /*** CONVERT Line/pixel to lat/lon ***/
	   cout<<"LINE/PIXEL TO LAT/LON TRANSFORMATION: "<<endl;
	   cout<<"     Line="<< line << " Pixel= " << pixel <<endl;
	   ierr = testElug.pl2ll(pixel, line, &rrlon, &rrlat);

	   elev = testElug.getEvln(instr, line);
	   scan = testElug.getScpx(instr, pixel);
	   cout<<"     elev= " <<elev <<"  scan= "<< scan <<endl;
	   cout<<"    dE= "<< rad2deg(elev) <<" dS="<< rad2deg(scan) << endl;
                            
	   cout<<"err = "<< ierr << endl;
	   if (ierr== 0) {
	     cout<<"     RRLAT= "<<rrlat <<" RRLON= "<< rrlon <<endl;
	     cout<<"     DLAT= " << rad2deg(rrlat) << " DLON= "
		 << rad2deg(rrlon) << endl << endl;
	   }

	   /*** CONVERT lat/lon back to Line/pixel ***/
	   cout<<endl<<endl<<"Now convert lat/lon back to line/pixel"<<endl; 
	   rlat = rrlat;
	   rlon = rrlon;
	   lat = rad2deg(rrlat);
	   lon = rad2deg(rrlon);
	   cout<<"LAT/LON TO LINE/PIXEL TRANSFORMATION: "<<endl;
	   cout<<"     Lat="<< lat << " Lon= " << lon <<endl;

	   ierr = testElug.ll2pl(rlon, rlat, &pixel, &line);
	   /***
	       elev = testElug.getEvln(instr, line);
	       scan = testElug.getScpx(instr, pixel);
	       cout<<"     elev= " <<elev <<"  scan= "<< scan <<endl;
	       cout<<"    dE= "<< rad2deg(elev) <<" dS="<< rad2deg(scan) << endl;
	   ***/                        
	   cout<<"err = "<< ierr << endl;
	   if (ierr== 0) {
	     cout<<"     Line= " << line << " Pixel= "
		 << pixel << endl;

	   }
	
	 }
       }
       testElug.setcons(4,3068,2,3068,4,1403,2,1403);
   }



	

   return 0;
}	


