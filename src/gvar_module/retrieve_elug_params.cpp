/*
 * retrieve_elug_params creates an file containing current
 * GOES parameters.
 *
 * (the code is not very clean but works ;-))
 * 
 *
 * @version $Id: retrieve_elug_params.cpp,v 1.8 2007/09/07 23:58:44 crueda Exp $
	Generated file:
        FORMAT:
                float[336]            -- as before
                "imc=%d\n"
                "flipflag=%d\n"
                "iofnc=nscyc1=%d"
                "iofni=nsinc1=%d\n"
                "iofec=ewcyc1=%d\n"
                "iofei=ewinc1=%d\n"
                "%d,%d,%d,%d,%d\n"    -- epoch time: year, day, hrs, min, sec
                "%d,%d,%d,%d,%d\n"    -- curr  time: year, day, hrs, min, sec
 
  I'm building this program as follow (on geostreams):
	GVAR_INCLUDEDIR=/home/carueda/include/libgvar/ GVAR_LIBDIR=/home/carueda/lib  make  retrieve_elug_params
  and running:
	LD_LIBRARY_PATH=/home/carueda/lib ./retrieve_elug_params
 */
#include "Block.h"
#include "GvarStream.h"
#include <iostream>
#include <fstream>
#include <cassert>

using namespace std;
using namespace Geostream ;

// yes, read from socket ;-)
#define READ_FROM_SOCKET 1

// name of generated file:
static const char* out_filename = "rec.dat";

// if reading from socket 
#ifdef READ_FROM_SOCKET
// old address char* ipaddr = "169.237.153.55" ;
char* ipaddr = "169.237.230.75" ;
int port = 21009 ;
#endif

static void reverse_bytes(void* datum, unsigned len) {       
	char* bin = (char*) datum;
	char bout[1024];
	assert( len < sizeof(bout) );
	for ( unsigned i = 0; i < len; i++ ) {
		bout[i] = bin[len - i - 1];
	}
	memcpy(datum, bout, len);
}

static void printCdaTime(FILE* file, CdaTime* cdat) {
	if(cdat == NULL) {
		cout << ":  time is NULL" << endl;
		exit(1);
	}
	fprintf(file, "%d,%d,%d,%d,%d\n", 
		cdat->year(),
		cdat->day(),
		cdat->hrs(),
		cdat->min(),
		cdat->sec()
	);
}


int main() {

  int imc;
  int flipflag;

  bool succeed = true;

// if read from socket 
#ifdef READ_FROM_SOCKET
  cout<< "ipaddr=" <<ipaddr<< " port=" <<port<< endl;
  GvarStream* gvar = new GvarStream(ipaddr, port);
  succeed = gvar->listen();
#else
  char* block0_filename = "block0file";
  GvarStream* gvar = new GvarStream(block0_filename);
#endif

  Gvar::Block0Doc* block0doc = NULL;

  cout<< "waiting for block 0 ..." <<endl;
  while ( succeed ) {
  	cout<< "."; cout.flush();

  	Gvar::Block* block = gvar->readBlock ();
  	int blockId = block->getHeader()->blockId();

  	if (blockId == 0) {
		cout<< endl;

    		Gvar::Block0* block0 = new Gvar::Block0 (block) ;
    		block0doc = block0->getBlock0Doc();
		break;
	}
  }
  assert( block0doc != NULL ) ;



    // output here
	const float* Rec = (const float*) block0doc->getRec();

	int SPCID = block0doc->spcid();
	int SPSID = block0doc->spsid();
	cout << "SPCID= "<< SPCID << "  SPSID= "<< SPSID << endl;

	char imcId[5] ;
	block0doc->getImcIdentifier(imcId) ;
	imcId[4] = '\0' ;

	cout << "imcIdentifier=" << imcId << "\n" ;
	cout << "imcIdentifier[3]=" << imcId[3] << "\n" ;


	char* imcIdenti = (char*) Rec;
	cout<<"IMC Identifier = ";
	for (int i = 0; i < 4; i++) {
	   cout<<imcIdenti[i];
	}
	cout<<endl;

     
	float lat0 = block0doc->lat0();
	float lat1 = block0doc->lat1();
	float lon0 = block0doc->lon0();
	float lon1 = block0doc->lon1();


	cout<<"lat0= "<<lat0 << "  lon0=" << lon0 << " lat1=" <<lat1 << "  lon1="<<lon1;
	cout<<endl;

	float sublo = block0doc->getSublo();
	cout<<"Sublo = "<<sublo<<endl;

	float rec5 = Rec[4];
	cout<<"Rec5 = "<<rec5<< "  ref long , positive east " <<endl;

 

	//for (int i=0; i<336; i++) {
	  //cout<< i << "  -->  " <<Rec[i] << endl;
	  ////out << Rec[i] << endl; 
	//}

	
	//out.close();

	const int* epoch1 = (const int*) & Rec[11];
	const int* epoch2 = (const int*) & Rec[12];
	const unsigned char* uepoch1 = (const unsigned char*) & Rec[11];
	printf("uepoch1 = ");
	for(int i = 0; i < 4; i++ ) {
		printf("%02d ", (int) uepoch1[i]);
	}

	printf("\n");

	printf("epoch1 = %08x\n", *epoch1);
	printf("epoch2 = %08x\n", *epoch2);

	const int* epochIEEE = (const int*) & Rec[11];
        printf("epoch after IEEE conversion = %d %d \n", *epochIEEE, *(epochIEEE+1));

	imc = block0doc->getImc();
	flipflag = block0doc->getFlipflag();

	cout<<"imc = "<<imc <<"   flipflag="<<flipflag<<endl;

        uchar8 iofnc = block0doc->getIofnc(); /* 6305 */
        uchar8 iofec = block0doc->getIofec() ; /* 6306 */
        uint16 iofni = block0doc->getIofni() ; /* 6307 6308 */
        uint16 iofei = block0doc->getIofei() ; /* 6309 6310 */

	unsigned char* ptr = (unsigned char* ) &iofni;
	unsigned new_iofni = (unsigned) ptr[0] << 8 | ptr[1];

	printf("new_iofni = %04x   iofni = %04x\n", new_iofni, iofni);
	cout << "new_iofni = " << new_iofni << endl;
	cout<<"  iofnc=nscyc="<<(int)iofnc<<"  iofec=ewcyc="<<(int)iofec
	    <<"  iofni=nsinc="<<iofni<<     "  iofei=ewinc="<<iofei<<endl;

	
	//ofstream out(out_filename, ios::out);
	FILE* out_file = fopen(out_filename, "wb");

	cout<< "Writing rec ..." <<endl;
	int numelements = fwrite(Rec, 1, 336*4, out_file);
	cout<< "  done.  numelements = " << numelements<<endl;

	// imc & filpflag
	fprintf(out_file, "imc=%d\n", imc);
	fprintf(out_file, "flipflag=%d\n", flipflag);

	// only for imager (sounder ignored)
	fprintf(out_file, "iofnc=nscyc1=%d\n", (int)iofnc);
	fprintf(out_file, "iofni=nsinc1=%d\n", (int)iofni);
	fprintf(out_file, "iofec=ewcyc1=%d\n", (int)iofec);
	fprintf(out_file, "iofei=ewinc1=%d\n", (int)iofei);


	CdaTime* epochTime = block0doc->getEpochDate();
	CdaTime* curTime = block0doc->getCurrentTime();

	fprintf(stdout, "Epoch time\n");
	printCdaTime(out_file, epochTime);
	printCdaTime(stdout, epochTime);
	fprintf(stdout, "Current time\n");
	printCdaTime(out_file, curTime);
	printCdaTime(stdout, curTime);

	//curTime->print(cout);
	//epochTime->print(cout); 


	fclose(out_file);

	gvar->close();
	delete gvar ;
  
	return 0;
}


