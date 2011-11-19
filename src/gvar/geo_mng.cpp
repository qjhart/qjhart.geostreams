#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "gvarHeader/gvarHeader.h"
#include "gvarBlock/gvarBlock.h"
#include "lineDoc/lineDoc.h"
#include "block1to10/block1to10.h"
#include "block0/block0.h"
#include "block0/block0Doc.h"
// #include "utils/options/options.h"
#include "analysis/pointset.h"

#define MNG_SUPPORT_WRITE
#define MNG_INCLUDE_WRITE_PROCS
#define MNG_ACCESS_CHUNKS
//#define MNG_INCLUDE_ZLIB
#define MNG_SUPPORT_DISPLAY

#include <libmng.h>


#include <iostream>
using namespace std;
//#include "../../libmng.h"


typedef struct user_struct {
	FILE *hFile;                 /* file handle */
	int  iIndent;                /* for nice indented formatting */
} userdata;


typedef userdata * userdatap;



/* ************************************************************************** */
mng_ptr myalloc (mng_size_t iSize){
	return (mng_ptr)calloc (1, iSize);   /* duh! */
}

/* ************************************************************************** */
void myfree (mng_ptr pPtr, mng_size_t iSize){
	free (pPtr);                         /* duh! */
	return;

}

/* ************************************************************************** */
mng_bool myopenstream (mng_handle hMNG){
	return MNG_TRUE;                     /* already opened in main function */
}

/* ************************************************************************** */
mng_bool myclosestream (mng_handle hMNG){
	return MNG_TRUE;                     /* gets closed in main function */
}


mng_bool mywritedata (mng_handle hMNG,
                      mng_ptr    pBuf,
                      mng_uint32 iSize,
                      mng_uint32 *iWritten)
{                                      /* get to my file handle */
	userdatap pMydata = (userdatap)mng_get_userdata (hMNG);
	/* write it */
	*iWritten = fwrite (pBuf, 1, iSize, pMydata->hFile);
	/* iWritten will indicate errors */
	return MNG_TRUE;
}

userdatap pMydata;
mng_handle initializemng(char * zFilename)
{


	mng_handle hMNG;
	mng_retcode iRC;

	/* get a data buffer */
	pMydata = (userdatap)calloc (1, sizeof (userdata));

	if (pMydata == NULL)                 /* oke ? */
	{
		fprintf (stderr, "Cannot allocate a data buffer.\n");
		exit(1);
	}
	/* can we open the file ? */
	if ((pMydata->hFile = fopen (zFilename, "wb")) == NULL)
	{                                    /* error out if we can't */
		fprintf (stderr, "Cannot open output file %s.\n", zFilename);
		exit(1);
	}
	/* let's initialize the library */
	hMNG = mng_initialize ((mng_ptr)pMydata, myalloc, myfree, MNG_NULL);

	if (!hMNG)                           /* did that work out ? */
	{
		fprintf (stderr, "Cannot initialize libmng.\n");
		exit(1);
	}

	if ( ((iRC = mng_setcb_openstream  (hMNG, myopenstream )) != 0) ||
		        ((iRC = mng_setcb_closestream (hMNG, myclosestream)) != 0) ||
		        ((iRC = mng_setcb_writedata   (hMNG, mywritedata  )) != 0)    ){
			fprintf (stderr, "Cannot set callbacks for libmng.\n");
			exit(2);
                }

//	mng_set_zlib_level(hMNG, 0);

    return hMNG;


}






int makeimage (mng_handle hMNG, PointSet* pointset_array[], int length)
{

	mng_retcode iRC;
	int size = pointset_array[0]->getSize();
	cout << "Width of the image is : " << size << endl;
	if(size > 500) size =500;

	if ( ((iRC = mng_create        (hMNG)                                                    ) != 0) ||
		((iRC = mng_putchunk_mhdr (hMNG, size, 100, 0, 0, 0, 0, 65)                   ) != 0) )
		{
		cout << "Error creating MNG. " << iRC << endl;
		exit(3);
		}

//***************************   Start of PNG Chunks  **************************************


	if ( (iRC = mng_putchunk_ihdr(hMNG, size, 100, 16, 0, 0, 0, 0  )) !=0){
               cout << "Error Putting IHDR Chunk." << iRC << endl;
	       exit(5);

	 }

	for(int i = 0; i <= 99; i++){

           uint16 * data = pointset_array[i]->getData();

	   if ( (iRC = mng_putchunk_idat(hMNG, size, (mng_ptr)data  ) )!=0){
	       cout << "Error Putting IDAT Chunk." <<  iRC << endl;
	       exit(6);
	    }

	}

	if ( (iRC = mng_putchunk_iend (hMNG) )!=0){
	       cout << "Error Putting IEND Chunk." << iRC << endl;
	       exit(7);
	    }



//***************************   End of PNG Chunks  **************************************
	if  ( (iRC = mng_putchunk_mend (hMNG) ) != 0 )
	{
		fprintf (stderr, "Cannot create the chunks for the image.\n");
	exit(4);
	}
	else
	{
		if ((iRC = mng_write (hMNG)) != 0)
			fprintf (stderr, "Cannot write the image.\n");

	}


		mng_cleanup (&hMNG);               /* cleanup the library */


	fclose (pMydata->hFile);             /* cleanup */
	free (pMydata);

	return iRC;
}


void geo_mng (PointSet* pointset_array[], int length)
{
     //PointSet* pointset_array[num_images]
     mng_handle hMNG;
     hMNG =  initializemng("geomng.mng");
     makeimage (hMNG, pointset_array, length);
}

