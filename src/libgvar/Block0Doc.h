#ifndef BLOCK0_DOC_H
#define BLOCK0_DOC_H

#include "cdaTime.h"
#include "types.h"
#include "gvar.h"
#include "Block.h"

#include "selFloat.h"

/* pg 68 , goes i-m, oge specs */

#define BLOCK0_BSZ 8042

#include <iostream>
using std::iostream ;
using std::ostream ;

namespace Gvar {
  class Block ;

  class Block0Doc { 
  private:
    Block *block ;

    // uchar8 junk[2]; // to align the block
    uchar8 Spcid;/* 1 */
    uchar8 Spsid;/* 2 */
   
    //ISCAN Iscan;/* 3 6 */

    /** to get IMC and flipflag: comment by Haiyan
     *  since I can't find definition of ISCAN,
     *  I declare an unit32 here.
     */
    uint32 Iscan;
    int imc;
    int flipflag; 

    uchar8 Idsub; /* 7 */
    uchar8 IdsubIR[7]; /* 8 14 */

    // uchar8 IdsubVis[8];/* 15 22 */

    CdaTime Tcurr ; /* 23 30 */
    CdaTime Tched ; /* 31 38 */
    CdaTime Tctrl ; /* 39 46 */
    CdaTime Tlhed ; /* 47 54 */
    CdaTime Tltrl ; /* 55 62 */
    CdaTime Tipfs ; /* 63 70 */
    CdaTime Tinfs ; /* 71 78 */
    CdaTime Tispc ; /* 79 86 */
    CdaTime Tiecl ; /* 87 94 */
    CdaTime Tibbc ; /* 95 102*/
    CdaTime Tistr ; /*103 110*/
    CdaTime Tlran ; /*111 118*/
    CdaTime Tiirt ; /*119 126*/
    CdaTime Tivit ; /*127 134*/
    CdaTime Tclmt ; /*135 142*/
    CdaTime Tiona ; /*143 150*/

    uint16 Risct; /*151 152*/
    uint16 Aisct; /*153 154*/
    uint16 Insln; /*155 156*/
    uint16 Iwfpx; /*157 158*/
    uint16 Iefpx; /*159 160*/
    uint16 Infln; /*161 162*/
    uint16 Isfln; /*163 164*/

    uint16 Impdx; /*165 166*/
    // uint16 Imdln; /*167 168*/
    // uint16 Imdct; /*169 170*/
 
    // uint16 Igvln; /*171 172*/
    // uint16 Igvpx; /*173 174*/
    // SelFloat  Subla; /*175 178*/
     float  Sublo; /*178 182*/
    // SelFloat  Sublo; /*178 182*/
    // uchar8 Czone; /*183*/
    // uchar8 V1phy; /*184*/
    // uint16 G1cnt; /*185 186*/
    //  uint16 G2cnt; /*187 188*/
    // uint16 Gbias; /*189 190*/
    // uint16 Lbias; /*191 192*/

    // uchar8 w193_4[2]; /*193 194*/

    // SelFloat Idber; /*195 198*/
    // SelFloat Range; /*199 202*/
    // SelFloat Gpath; /*203 206*/
    // SelFloat Xmsne; /*207 210*/

    // CdaTime Tgpat; /*211 218*/
    // CdaTime Txmsn; /*219 226*/
    // uint16 Istim; /*227 228*/

    uchar8 Ifram; /*229*/
    // uchar8 Imode; /*230*/


     /** where is SelFloat defined ? I'll use float 
      *  temporarily
      */
     //SelFloat Ifnw1; /*231 234*/
     //SelFloat Ifnw2; /*235 238*/
     //SelFloat Ifse1; /*239 242*/
     //SelFloat Ifse2; /*243 246*/

     float Ifnw1; /*231 234*/
     float Ifnw2; /*235 238*/
     float Ifse1; /*239 242*/
     float Ifse2; /*243 246*/
    // uchar8 w247_277[31]; /*247 277*/
    // uchar8 Parity_1_277; /*278*/

    /* see goes-i 
     * Imager Documentation, Block 0 Format Defination 
     * Orbit and Attitude Parameters 
     */

     /** parsed by Haiyan Yang
      *   word 279 to word 1626 provides the Imager 
      *   O & A parameters 
      */ 
     float rec[336]; /* 279 1626*/

     uchar8 Imc_identifier[4]; // 279 282 

#if 0
       uchar8 w283_94[12]; // 283 294

       SelFloat ReferenceLongitude; // 295 298

       SelFloat ReferenceRadialDistance; /* 299 302*/
       SelFloat ReferenceLatitude; /* 303 306*/
       SelFloat ReferenceOrbitYaw; /* 307 310*/
       SelFloat ReferenceAttitudeRoll; /* 311 314*/
       SelFloat ReferenceAttitudePitch; /* 315 318*/
       SelFloat ReferenceAttitudeYaw; /* 319 322*/
#endif
       CdaTime EpochDate; /* 323 330*/

#if 0
       SelFloat IMCenableFromEpoch; /* 331 334*/
       SelFloat CompensationRoll; /* 335 338*/
       SelFloat CompensationPitch; /* 339 342*/
       SelFloat CompensationYaw; /* 343 346*/
       SelFloat ChangeLongitude[13]; /* 347 398*/
       SelFloat ChangeRadialDistance[11]; /* 399 442*/
       SelFloat SineGeocentricLatitude[9]; /* 443 478*/
       SelFloat SineOrbitYaw[9]; /* 479 514*/
       SelFloat DailySolarRate; /* 515 518*/
       SelFloat ExponentialStartFromEpoch; /* 519 522*/

       AttitudeAngle RollAngle; /* 523 742*/
       AttitudeAngle PitchAngle; /* 743 962*/
       AttitudeAngle YawAngle; /* 963 1182*/
       AttitudeAngle RollMisalignment; /*1183 1402*/
       AttitudeAngle PitchMisalignment; /*1403 1662*/

       uchar8 w1623_625[3]; /*1623 1625*/
       uchar8 Parity_279_1625; /*1626 */
#endif


    //   uchar8 Coregisteration_table_id[4]; /*1627 1630*/
    //   uchar8 East_west_half_hourly_correction_terms[48]; /*1631 1678*/
    //   uchar8 Index_of_correction_term_currently_active; /*1679 */
    //   uchar8 w1680_1690[11]; /*1680 1690*/

    // uint16 w1691_782[46]; 
    // uint16 w1783_874[46];
    // uint16 w1875_966[46];
    // uint16 w1967_2058[46];

    // uchar8 w2059_80[22];
    // uchar8 w2081_102[22];
    // uchar8 w2103_24[22];
    // uchar8 w2125_46[22];
    // uint16 Block_telemetry_word[2][39] ; /* 2147 2302 */
    // uchar8 w2303_5[3]; /* 2305 2305 */
    // uchar8 Parity_1691_2305; /* 2306 */

    // uchar8 Grid1Detector[512]; /*2307 2818*/
    // uchar8 Grid2Detector[512]; /*2819 3330*/

    // uint16 Grid1Pixel[512]; /*3331 4353*/
    // uint16 Grid2Pixel[512]; /*4355 5378*/
  
    // uint16 Gridset1RevLevel; /*5379 5380*/
    // uint16 Gridset2RevLevel; /*5381 5382*/

    // uchar8 w5383_85[3]; /*5383 5385*/
    // uchar8 Parity_2307_5385; /*5386 */
  
    // uint16 w5387_478[46];
    // uint16 w5479_570[46];
    // CdaTime Tophed; /* 5571 5578 */
    // CdaTime Totrl ; /* 2279 5586 */

    /* ir calibratrion */
    // SelFloat  Iwbias[7]; /* 5587 5614*/
    // SelFloat  Igain1[7]; /* 5615 5642*/
    // SelFloat  Igain2[7]; /* 5643 5670*/
    // SelFloat  Ibrate[7]; /* 5671 5698*/
  
    // CdaTime Tod_of_westernmost_ir_pixel; /* 5699 5706 */
  
    // BiasStatistics ImagerIRclamped ; /* 5707 5970 */ 
    // BiasStatistics ImagerIRdrift ; /* 5971 6234 */

    // uchar8 NS_half_hourly_correction_terms[48];  /* 6235 6282 */
    // uchar8 Scan_clamp_e_w_clipping_edge_limb_offset[4]; /* 6283 6286 */
    // uchar8 w6287_303[17] ; /* 6287 6203 */
    // uchar8 Parity_5387_6303 ; /* 6304 */
  
    uchar8 Iofnc ; /* 6305 */
    uchar8 Iofec ; /* 6306 */
    uint16 Iofni ; /* 6307 6308 */
    uint16 Iofei ; /* 6309 6310 */
 
    // uint16 Vis_detector_x_offset[8]; /* 6311 6325 */
    // uint16 Ir_detector_x_offset[14]; /* 6326 6354 */
    // uint16 Vis_detector_y_offset[8]; /* 6355 6369 */
    // uint16 Ir_detector_y_offset[14]; /* 6370 6398 */

    // SelFloat Ivcrb[8]; /* 6399 6430 */
    // SelFloat Ivcr1[8]; /* 6431 6462 */
    // SelFloat Ivcr2[8]; /* 6463 6494 */
    // SelFloat Ivral;    /* 6495 6498 */
  
    // SelFloat Iicrb[2][7] ; /* 6499 6554 */
    // SelFloat Iicr1[2][7] ; /* 6555 6610 */
    // SelFloat Iicr2[2][7] ; /* 6611 6666 */
    // SelFloat Iisfb[2][7] ; /* 6667 6722 */
    // SelFloat Iisf1[2][7] ; /* 6723 6778 */
    // SelFloat Ig2it[2][7][4] ; /* 6779 7002 */
    // SelFloat Ig2bp[4] ; /* 7003 7018 */
    // SelFloat Ibbtr[2][7][4] ; /* 7019 7242 */
  
    // uchar8 w7234_366[124]; /* 7243 7336 */

    // Calibration Imager; /* 7367 8014 */
    // int imager ; // added by jiezhang ???
  
    // SelFloat Patch_control_voltage_gain; /* 8015 8018 */ 
    // SelFloat Patch_control_voltage_bias; /* 8019 8022 */
    // SelFloat Instrument_current_gain ;   /* 8023 8026 */
    // SelFloat Instrument_current_bias;    /* 8027 8030 */
  
    // uchar8 w8031_39[9] ;
    // uchar8 Parity_6305_8039; 
    // uint16 CRC;

  public: 
    Block0Doc(Block *block);

    int ifram();
    void block0timeText(ostream & out); 
    void block0time(ostream & out );
    void NavigationParameters(ostream & out);

    // uint16 crc();  // CRC word

    int spcid(); // SpaceCraft ID
    int spsid(); // SPSID

    

    int frame() ; // frame number 
    int wpx(); // Western edge pixel
    int epx(); // Eastern edge pixel

    int nsln () ;

    int nln(); // Northern edge pixel
    int sln(); // Sothern edge pixel
    int aScanCount(); // Absoulute scan count
    CdaTime* getCurrentTime(); //returns current frame time.b
 
    // return imc identifier, and put it into imcIdentifier parameter.
    void getImcIdentifier (char* imcIdentifier) ;

	//added by Haiyan
 	/** returns a pointer to the internal REC location */
	int getImc();
	int getFlipflag();
	const float* getRec(); 
    
        uchar8 getIofnc(); /* 6305 */
        uchar8 getIofec() ; /* 6306 */
        uint16 getIofni() ; /* 6307 6308 */
        uint16 getIofei() ; /* 6309 6310 */

        CdaTime* getEpochDate(); /* 323 330*/
    // int instrument(); // Instrument

     float lon0(); // Frame top latitude
     float lon1(); // Frame bot latitude
     float lat0(); // Frame left longitude
     float lat1(); // Frame right longitude
  
     float getSublo(); 
    /// int imc(); 

    // void reset();// Reset used variables

  };

  // inline uint16 Block0Doc::crc() { return CRC; }
  inline int Block0Doc::spcid() {return (int)Spcid;} 
  inline int Block0Doc::spsid() {return (int)Spsid;} 
  inline int Block0Doc::wpx() { return Iwfpx; } 
  inline int Block0Doc::epx() { return Iefpx; } 
  inline int Block0Doc::nsln () { return Insln; }
  inline int Block0Doc::nln() { return Infln; } 
  inline int Block0Doc::sln() { return Isfln; } 
  inline int Block0Doc::aScanCount() { return Aisct; } 
  // inline int Block0Doc::instrument() { return imager; }
  inline int Block0Doc::frame() { return (int)Ifram ; }
   inline float Block0Doc::lat0() { return Ifnw1; }
   inline float Block0Doc::lat1() { return Ifse1; }
   inline float Block0Doc::lon0() { return Ifnw2; }
   inline float Block0Doc::lon1() { return Ifse2; }
   inline float Block0Doc::getSublo() { return Sublo; }

  // inline void Block0Doc::reset() {G1cnt = 0; G2cnt = 0; }
}//namespace Gvar
#endif 
