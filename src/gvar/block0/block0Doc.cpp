#include "block0Doc.h"

// extern Options Opt; 

// int Block0Doc::imc()
// { return Iscan.imcStatus();}

Block0Doc::Block0Doc(GvarBlock *block){
  this->block = block ;

  uchar8* data = block->getData () ;

  Spcid = data[0] ;
  Spsid = data[1] ;

  // iscan
  for (int i=2; i<6; i++) {
  }

  Idsub = data[6] ;

  for (int i=7; i<14; i++) {
    IdsubIR[i-7] = data[i] ;
  }

  // ignore time for now, jiezhang ???
  Tcurr.setTime (&data[22]) ;
  Tched.setTime (&data[30]) ;
  Tctrl.setTime (&data[38]) ;
  Tlhed.setTime (&data[46]) ;
  Tltrl.setTime (&data[54]) ;
  Tipfs.setTime (&data[62]) ;
  Tinfs.setTime (&data[70]) ;
  Tispc.setTime (&data[78]) ;
  Tiecl.setTime (&data[86]) ;
  Tibbc.setTime (&data[94]) ;
  Tistr.setTime (&data[102]) ;
  Tlran.setTime (&data[110]) ;
  Tiirt.setTime (&data[118]) ;
  Tivit.setTime (&data[126]) ;
  Tclmt.setTime (&data[134]) ;
  Tiona.setTime (&data[142]) ;

  Risct = ((uint16)(data[150] << 8)) + ((uint16)data[151]) ;
  Aisct = ((uint16)(data[152] << 8)) + ((uint16)data[153]) ;
  Insln = ((uint16)(data[154] << 8)) + ((uint16)data[155]) ;

  // get Iwfpx, Iefpx, Infln, Isfln
  Iwfpx = ((uint16)(data[156] << 8)) + ((uint16)data[157]) ;
  Iefpx = ((uint16)(data[158] << 8)) + ((uint16)data[159]) ;
  Infln = ((uint16)(data[160] << 8)) + ((uint16)data[161]) ;
  Isfln = ((uint16)(data[162] << 8)) + ((uint16)data[163]) ;
  // get Ifram
  Ifram = data[228] ;

}

/*
void Block0Doc::NavigationParameters(ostream & out){
  
  out << 1 ;
  out << "\t" << instrument();
  out << "\n";

  Tcurr.print(out);
  out << "\tCurrent Time\n";

  out << Insln; 
  out << "\tNorthern-most visible detector scan line in current scan\n";
  
  out << Iwfpx;
  out << "\tWestern-most visible pixel in current frame\n";
  
  out << Iefpx;
  out << "\tEastern-most visible pixel in current frame\n";
  
  out << Infln;
  out <<"\tNorthern-most visible pixel in current frame\n";
  
  out << Isfln; 
  out <<"\tSouthern-most visible pixel in current frame\n";  
    
  for ( int l = 0; l < 4; l++)
    out << Imc_identifier[l];
  out << "\tIMC identifier \n";

  out << (double)ReferenceLongitude; 
  out << "\tReference Longitude\n";
  out << (double)ReferenceRadialDistance;
  out << "\tReference Radial Distance\n";
  out << (double)ReferenceLatitude;
  out << "\tReference Radial Latitude\n";
  out << (double)ReferenceOrbitYaw;
  out << "\tReference Orbit Yaw\n";
  out << (double)ReferenceAttitudeRoll;
  out << "\tReferenceAttitudeRoll\n";
  out << (double)ReferenceAttitudePitch;
  out << "\tReference Attitude Pitch\n";
  out << (double)ReferenceAttitudeYaw;
  out << "\tReferenceAttitudeYaw\n";
  out << EpochDate;
  out << "\tEpoch Date\n";
  out << (double) IMCenableFromEpoch;
  out << "\tIMC enable from epoch\n";
  out << (double)CompensationRoll;
  out << "\tCompensationRoll\n";
  out << (double)CompensationPitch;
  out << "\tCompensation Pitch\n";
  out << (double)CompensationYaw;
  out << "\tCompensation Yaw\n";
  for(int i = 0; i < 13; i++){
    out << (double)ChangeLongitude[i];
    out << "\tChange Longitude "<< i << "\n";
  }
  for (int j=0; j < 11 ; j++){
    out << (double)ChangeRadialDistance[j]; 
    out << "\tChange Radial Distance " << j << "\n";
  }
  for(int k=0; k < 9; k++){
    out << (double)SineGeocentricLatitude[k];
    out << "\tChange Geocoentric Latitude " << k << "\n";
    out << (double)SineOrbitYaw[k];
    out << "\tSine Orbit Yaw " << k << "\n";
  }
  out << (double)DailySolarRate;
  out << "\tDaily Solar Rate\n";
  out << (double)ExponentialStartFromEpoch;
  out << "\tExponential Start From Epoch\n";
  

  out << "\nRoll Angle\n";
  out << RollAngle;

  out <<"\nPitch Angle\n";
  out << PitchAngle;
  
  out << "\nYaw Angle\n";
  out << YawAngle;
  
  out << "\nRoll Misalignment\n";
  out << RollMisalignment;
  
  out << "\nPitch Misalignment\n";
  out << PitchMisalignment;

  out << "\n\n";
}
*/

void Block0Doc::block0time(ostream & out ){
  out <<Risct<<"\t"<<Tcurr<<"\t"<<Tched<<"\t"<<Tctrl<<"\t"<<Tlhed<<"\t"
    <<Tltrl<<"\t"<<Tipfs<<"\t"<<Tinfs<<"\t"<<Tispc<<"\t"
      <<Tiecl<<"\t"<<Tibbc<<"\t"<<Tistr<<"\t"<<Tlran<<"\t"
	<<Tiirt<<"\t"<<Tivit<<"\t"<<Tclmt<<"\t"<<Tiona;
}

void Block0Doc::block0timeText(ostream & out){
  /*		out << "Gvar Imager Documentation Block0 Format Definition";
		out << "OGE Interface Spec DRL 504-02-1 Rev C; table 6.3 pp 67 \n";
		*/
  out<<"Risct\t";
  out<<"Tcurr-y\tTcurr-d\tTcurr-h\tTcurr-m\tTcurr-s\tTcurr-ms\tTcurr-f\t";
  out<<"Tched-y\tTched-d\tTched-h\tTched-m\tTched-s\tTched-ms\tTched-f\t";
  out<<"Tctrl-y\tTctrl-d\tTctrl-h\tTctrl-m\tTctrl-s\tTctrl-ms\tTctrl-f\t";
  out<<"Tlhed-y\tTlhed-d\tTlhed-h\tTlhed-m\tTlhed-s\tTlhed-ms\tTlhed-f\t";

  out<<"Tltrl-y\tTltrl-d\tTltrl-h\tTltrl-m\tTltrl-s\tTltrl-ms\tTltrl-f\t";
  out<<"Tipfs-y\tTipfs-d\tTipfs-h\tTipfs-m\tTipfs-s\tTipfs-ms\tTipfs-f\t";
  out<<"Tinfs-y\tTinfs-d\tTinfs-h\tTinfs-m\tTinfs-s\tTinfs-ms\tTinfs-f\t";
  out<<"Tispc-y\tTispc-d\tTispc-h\tTispc-m\tTispc-s\tTispc-ms\tTispc-f\t";

  out<<"Tiecl-y\tTiecl-d\tTiecl-h\tTiecl-m\tTiecl-s\tTiecl-ms\tTiecl-f\t";
  out<<"Tibbc-y\tTibbc-d\tTibbc-h\tTibbc-m\tTibbc-s\tTibbc-ms\tTibbc-f\t";
  out<<"Tistr-y\tTistr-d\tTistr-h\tTistr-m\tTistr-s\tTistr-ms\tTistr-f\t";
  out<<"Tlran-y\tTlran-d\tTlran-h\tTlran-m\tTlran-s\tTlran-ms\tTlran-f\t";

  out<<"Tiirt-y\tTiirt-d\tTiirt-h\tTiirt-m\tTiirt-s\tTiirt-ms\tTiirt-f\t";
  out<<"Tivit-y\tTivit-d\tTivit-h\tTivit-m\tTivit-s\tTivit-ms\tTivit-f\t";
  out<<"Tclmt-y\tTclmt-d\tTclmt-h\tTclmt-m\tTclmt-s\tTclmt-ms\tTclmt-f\t";
  out<<"Tiona-y\tTiona-d\tTiona-h\tTiona-m\tTiona-s\tTiona-ms\tTiona-f";

}
