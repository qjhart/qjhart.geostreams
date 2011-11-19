#ifndef FRAME_CHECK
#define FRAME_CHECK

#include "include/gvar.h"
#include "block0/imagerDoc.h"
#include "block11/doc/sounderDoc.h"

int skipIt(int frame=ALL, int instrument=ALL, int channel=ALL);

int skipIt(int frame, int instrument, int channel){

  extern Options Opt;
  extern ImagerDoc imagerDoc;
  extern SounderDoc sounderDoc;
  
  int skip = TRUE;

  int i0,i1, f0, f1, c0, c1;

  if(instrument== ALL) { i0 = 0; i1 = sounder ; }
  else { i0 = instrument; i1 = instrument ; }
  
  int xstart, ystart, xend, yend, hrs, visCounts;

  for (int i=i0; i<=i1 && skip; i++){
    if (frame==ALL) { f0 = 0; f1 = Opt.frames(i); }
    else { f0 = frame; f1 = frame+1; }
    if(channel==ALL) { c0 = 0; c1 = Channel[i];}
    else { c0 = channel; c1 = channel+1;}    
    
    for(int f=f0; f<f1 && skip ; f++)
      for(int c=c0; c<c1 && skip; c++){

	if (i==imager) {
	  xstart=imagerDoc.wpx();
	  ystart=imagerDoc.nln();
	  xend  =imagerDoc.epx();
	  yend  =imagerDoc.sln();
	  hrs = imagerDoc.Tcurr.hrs();
	  visCounts = Detector[i][0] * imagerDoc.aScanCount();
	}
	else if (i==sounder){
	  xstart=sounderDoc.wpx();
	  ystart=sounderDoc.nln();
	  xend  =sounderDoc.epx();
	  yend  =sounderDoc.sln();
	  hrs = sounderDoc.Tcurr.hrs();    
	  visCounts = Detector[i][0] * sounderDoc.aScanCount();
	}
	
	if(
	   (VALID(Opt.xstart(f,i),xstart) <= xend      ) && 
	   (VALID(Opt.ystart(f,i),ystart) <= yend      ) &&
	   (VALID(Opt.xend(f,i), xend)    >= xstart    ) &&
	   (VALID(Opt.yend(f,i), yend)    >= ystart    ) &&
	   (VALID(Opt.yend(f,i), yend)    >= visCounts ) &&
	   (Opt.dataType(f,i,c) != Undef )
	   ) skip = FALSE;

	if(!skip){
	  int h0 = Opt.gmtHoursStart(f,i,c);
	  int h1 = Opt.gmtHoursStop(f,i,c);
	  if(h1 > h0)
	    {if (hrs > h0 && hrs <  h1 ) skip = TRUE; } 
	  else 
	    {if (hrs < h0 || hrs > h1 ) skip = TRUE; }
	} 
      }
  }
  return(skip);
}

#endif
