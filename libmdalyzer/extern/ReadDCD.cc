/***************************************************************************
 *cr                                                                       
 *cr            (C) Copyright 1995-2001 The Board of Trustees of the           
 *cr                        University of Illinois                       
 *cr                         All Rights Reserved                        
 *cr                                                                   
 ***************************************************************************/

/***************************************************************************
 * RCS INFORMATION:
 *
 *      $RCSfile: ReadDCD.C,v $
 *      $Author: saam $        $Locker:  $                $State: Exp $
 *      $Revision: 1.5 $      $Date: 2004/05/16 08:04:45 $
 *
 ***************************************************************************
 * DESCRIPTION:
 *
 * C routines to read and write binary DCD files (which use the goofy
 * FORTRAN UNFORMATTED format).  These routines are courtesy of
 * Mark Nelson (autographs available upon request and $1 tip).
 *
 ***************************************************************************/

#include <stdlib.h> // for malloc
//#include <time.h>
#include <iostream>
#include <string.h>
#include "ReadDCD.h"
#ifdef DMALLOC
#include <dmalloc.h>
#endif

/* READ Macro to make porting easier */
#define READ(fd, buf, size) \
        fread((buf), (size), 1, (fd))


/* WRITE Macro to make porting easier */
#define WRITE(fd, buf, size) \
        fwrite((buf), (size), 1, (fd))


/* routines for dealing with reverse endian DCD files */
/* currently used only for reading  */
/* reverse between little and big endian and vice versa */

/************************************************************************/
/*									*/
/*			FUNCTION reverseFourByteWord		      	*/
/*									*/
/*   INPUTS:								*/
/*	N        - four byte word cast to int			       	*/
/*									*/
/*   OUTPUTS:								*/
/*	return the reverse endian byte order for N       		*/
/*									*/
/*	reverseFourByteWord switches between little and big endian      */
/*      format for four byte words.                      	       	*/
/*									*/
/************************************************************************/
int* reverseFourByteWord(int* N)
{
  char byteArray[4] = {'0','0','0','0'};

  char *bytePointer;


  bytePointer = (char*)N;

  byteArray[0]  =  *bytePointer;
  byteArray[1]  =  *(bytePointer+1);
  byteArray[2]  =  *(bytePointer+2);
  byteArray[3]  =  *(bytePointer+3);

  *bytePointer     = byteArray[3];
  *(bytePointer+1) = byteArray[2];
  *(bytePointer+2) = byteArray[1];
  *(bytePointer+3) = byteArray[0];

  N=(int*)bytePointer;

  return N;
}

/************************************************************************/
/*									*/
/*			FUNCTION reverseEightByteWord		      	*/
/*									*/
/*   INPUTS:								*/
/*	N        - eight byte word cast to long			      	*/
/*									*/
/*   OUTPUTS:								*/
/*	return the reverse endian byte order for N       		*/
/*									*/
/*	reverseEightByteWord switches between little and big endian     */
/*      format for eight byte words.                      	      	*/
/*      The lower and higher 4 byte words are extracted and reversed.	*/
/************************************************************************/
double* reverseEightByteWord(double* N)
{
  char byteArray[8] = {'0','0','0','0','0','0','0','0'};
  char *bytePointer;

  bytePointer = (char*)N;

  byteArray[0]  =  *bytePointer;
  byteArray[1]  =  *(bytePointer+1);
  byteArray[2]  =  *(bytePointer+2);
  byteArray[3]  =  *(bytePointer+3);
  byteArray[4]  =  *(bytePointer+4);
  byteArray[5]  =  *(bytePointer+5);
  byteArray[6]  =  *(bytePointer+6);
  byteArray[7]  =  *(bytePointer+7);

  *bytePointer     = byteArray[7];
  *(bytePointer+1) = byteArray[6];
  *(bytePointer+2) = byteArray[5];
  *(bytePointer+3) = byteArray[4];
  *(bytePointer+4) = byteArray[3];
  *(bytePointer+5) = byteArray[2];
  *(bytePointer+6) = byteArray[1];
  *(bytePointer+7) = byteArray[0];

  N=(double*)bytePointer;

  return N;
}

/************************************************************************/
/*									*/
/*				MACRO CHECK_FREAD			*/
/*									*/
/*	CHECK_FREAD is used to check the status of a file read.  It     */
/*   is passed the return code from read and a string to print out if   */
/*   an error is detected.  If an error is found, an error message is   */
/*   printed out and the program terminates.  This was made into a      */
/*   macro because it had to be done over and over and over . . .	*/
/*									*/
/************************************************************************/

#define CHECK_FREAD(X, msg)  if (X==-1) \
			     { \
				return(DCD_BADREAD); \
			     }

/************************************************************************/
/*									*/
/*			MACRO CHECK_FEOF				*/
/*									*/
/*	CHECK_FEOF checks for an abnormal EOF on a file read.  It is    */
/*   passed the return status from read and a message to print on error.*/
/*   if an EOF is detected, the error message is printed and the program*/
/*   terminates.  This is used for reads that should find something in  */
/*   the file.								*/
/*									*/
/************************************************************************/

#define CHECK_FEOF(X, msg)  if (X==0) \
			     { \
				return(DCD_BADEOF); \
			     }


/*			GLOBAL VARIABLES				*/

void pad(char *s, int len)
{
	int curlen;
	int i;

	curlen=strlen(s);

	if (curlen>len)
	{
		s[len]=0;
		return;
	}

	for (i=curlen; i<len; i++)
	{
		s[i]=' ';
	}

	s[i]=0;
}

/************************************************************************/
/*									*/
/*			FUNCTION open_dcd_read				*/
/*									*/
/*   INPUTS:								*/
/*	filename - filename to read in					*/
/*									*/
/*   OUTPUTS:								*/
/*	an open filedesriptor (integer) is returned if the call is      */
/*   successful, otherwise a negative number is returned		*/
/*									*/
/*	open_dcd_read opens a dcd file for input.  It first does a check*/
/*   to see if the file really exists.  If the file does not exist,     */
/*   a DCD_DNE is returned, if the file exists but can' be opened for   */
/*   some reason, a DCD_OPENFAILED is returned.  If the open is         */
/*   successful, the file descriptor is returned.			*/
/*									*/
/************************************************************************/

FILE * open_dcd_read(const char *filename) {
  return fopen(filename, "rb");
}





/****************************************************************/
/*								*/
/*			FUNCTION read_dcdheader			*/
/*								*/
/*   INPUTS:							*/
/*	fd - file descriptor for the dcd file			*/
/*	N - Number of atoms					*/
/*	NSET - Number of sets of coordinates			*/
/*	ISTART - Starting timestep of DCD file			*/
/*	NSAVC - Timesteps between DCD saves			*/
/*	DELTA - length of a timestep				*/
/*      reverseEndian - do we need to revert the byte order?    */
/*      charmm - CHARMm flags (OR'd values):                    */
/*          0x01 (DCD_IS_CHARMM):				*/
/*                File was generated from CHARMm		*/
/*          0x02 (DCD_HAS_4DIMS):				*/
/*                File contains 4th dimension which must be	*/
/*                ignored (x, y, z, and `?' blocks)		*/
/*          0x04 (DCD_HAS_EXTRA_BLOCK):				*/
/*                File contains block before x,y,z blocks	*/
/*                which must be ignored				*/
/*								*/
/*          These flags are used to properly read in CHARMm     */
/*          files while keeping compatibility with X-PLOR.      */
/*								*/
/*								*/
/*   OUTPUTS:							*/
/*	N, NSET, ISTART, NSAVC, NAMNF and DELTA are returned populated*/
/*   a 0 is returned if the call is successful, or a negative   */
/*   number if errors are detected				*/
/*								*/
/*	read_header reads in the header from a DCD file and     */
/*   returns the timestep size and the number of atoms in the   */
/*   system.  A 0 is returned if the header is successfully     */
/*   read, or a negative error code is returned otherwise.      */
/*   The function can detect if a DCD file is in reverse endian */
/*   byte order. The appropriate byte reverse function will be  */
/*   called for all values read.                                */
/*								*/
/****************************************************************/

int read_dcdheader(FILE * fd, int *N, int *NSET, int *ISTART, 
		   int *NSAVC, double *DELTA, int *NAMNF, 
		   int **FREEINDEXES, int *reverseEndian, int *charmm)
{
  int input_integer;  /* Integer buffer space             */
  int ret_val;	      /* Return value from read           */
  int i;	      /* Loop counter                     */
  char hdrbuf[84];    /* Char buffer used to store header */
  int NTITLE;         /* DCD title string                 */

  /*  First thing in the file should be an 84		*/
  ret_val = READ(fd, &input_integer, sizeof(int));
  CHECK_FREAD(ret_val, "reading first int from dcd file");
  CHECK_FEOF(ret_val, "reading first int from dcd file");

  /* Check magic number in file header and determine byte order*/
  if (input_integer != 84) {
    /* check to see if its merely reversed endianism     */
    /* rather than a totally incorrect file magic number */
    input_integer= *reverseFourByteWord(&input_integer);

    if (input_integer == 84) {
      std::cerr << "Enabling byte swapping to correct DCD file endianism." 
              << std::endl;
      *reverseEndian=1;
    }
    else {
      std::cerr << "Bad magic number in DCD file." << std::endl;  
      std::cerr << "This DCD file appears to be corrupt!" << std::endl;  
      return(DCD_BADFORMAT);
    }
  }
  else {
    *reverseEndian=0;    
  }

  /*  Buffer the entire header for random access	*/
  ret_val = READ(fd, hdrbuf, 84);
  CHECK_FREAD(ret_val, "buffering header");
  CHECK_FEOF(ret_val, "buffering header");

  /*  Check for the ID string "COORD"		*/
  if (hdrbuf[0] != 'C' || hdrbuf[1] != 'O' ||
      hdrbuf[2] != 'R' || hdrbuf[3] != 'D') {
      std::cerr << "Missing CORD tag in DCD file!" << std::endl;  
      std::cerr << "This DCD file appears to be nonstandard or corrupt!" 
             << std::endl;
    return DCD_BADFORMAT;
  }

  /* CHARMm-genereate DCD files set the last integer in the     */
  /* header, which is unused by X-PLOR, to its version number.  */
  /* Checking if this is nonzero tells us this is a CHARMm file */
  /* and to look for other CHARMm flags.                        */
  if (*((int *) (hdrbuf + 80)) != 0) {
	(*charmm) = DCD_IS_CHARMM;
	if (*((int *) (hdrbuf + 44)) == 1)
		(*charmm) |= DCD_HAS_EXTRA_BLOCK;
	if (*((int *) (hdrbuf + 48)) == 1)
		(*charmm) |= DCD_HAS_4DIMS;
  }
  else (*charmm) = 0;

  /*  if ((*charmm) & DCD_IS_CHARMM) {
    std::cerr << "DCD file type: CHARMm";
    if ((*charmm) & DCD_HAS_EXTRA_BLOCK) {
      std::cerr << ", has extra data block";
    }
    if ((*charmm) & DCD_HAS_4DIMS) {
      std::cerr << ", has 4 dimensions";
    }
    std::cerr << std::endl;
  }
  else {
    std::cerr << "DCD file type: X-PLOR" << std::endl;
    } */

  /*  Store the number of sets of coordinates (NSET) */
  (*NSET) = *((int *) (hdrbuf + 4));
  if (*reverseEndian) NSET=reverseFourByteWord(NSET);

  /*  Store ISTART, the starting timestep	     */
  (*ISTART) = *((int *) (hdrbuf + 8));
  if (*reverseEndian) ISTART=reverseFourByteWord(ISTART);

  /*  Store NSAVC, the number of timesteps between   */
  /*  dcd saves				               */
  (*NSAVC) = *((int *) (hdrbuf + 12));
  if (*reverseEndian) NSAVC=reverseFourByteWord(NSAVC);

  /*  Store NAMNF, the number of NON-free atoms	     */
  (*NAMNF) = *((int *) (hdrbuf + 36));
  if (*reverseEndian) NAMNF=reverseFourByteWord(NAMNF);

  /*  Read in the timestep, DELTA			*/
  /*  Note: DELTA is stored as a double with X-PLOR but */
  /*  as a float with CHARMm				*/
  if ((*charmm) & DCD_IS_CHARMM) {
    double * tmp;
    float * tmp2;
    tmp = (double *) (hdrbuf + 40);
    if (*reverseEndian) tmp=reverseEightByteWord(tmp);
    tmp2 = (float *) tmp;
    (*DELTA) = (double) *(tmp2);
  }
  else {
    (*DELTA) = *((double *) (hdrbuf + 40));
    if (*reverseEndian) DELTA=reverseEightByteWord(DELTA);
  }

  /*  Get the end size of the first block			*/
  ret_val = READ(fd, &input_integer, sizeof(int));
  CHECK_FREAD(ret_val, "reading second 84 from dcd file");
  CHECK_FEOF(ret_val, "reading second 84 from dcd file");
  if (*reverseEndian) input_integer= *reverseFourByteWord(&input_integer);

  if (input_integer != 84) {
    return(DCD_BADFORMAT);
  }

  /*  Read in the size of the next block			*/
  ret_val = READ(fd, &input_integer, sizeof(int));
  CHECK_FREAD(ret_val, "reading size of title block");
  CHECK_FEOF(ret_val, "reading size of title block");
  if (*reverseEndian) input_integer= *reverseFourByteWord(&input_integer);

  if ( ((input_integer-4)%80) == 0) {
    /*  Read NTITLE, the number of 80 characeter    */
    /*  title strings there are                     */
    ret_val = READ(fd, &NTITLE, sizeof(int));
    CHECK_FREAD(ret_val, "reading NTITLE");
    CHECK_FEOF(ret_val, "reading NTITLE");
    if (*reverseEndian) NTITLE= *reverseFourByteWord(&NTITLE);

    for (i=0; i<NTITLE; i++) {
      fseek(fd, 80, SEEK_CUR);
      CHECK_FEOF(ret_val, "reading TITLE");
    }

    /*  Get the ending size for this block		*/
    ret_val = READ(fd, &input_integer, sizeof(int));

    CHECK_FREAD(ret_val, "reading size of title block");
    CHECK_FEOF(ret_val, "reading size of title block");
  }
  else {
    return(DCD_BADFORMAT);
  }

  /*  Read in an 4				*/
  ret_val = READ(fd, &input_integer, sizeof(int));
  CHECK_FREAD(ret_val, "reading an 4");
  CHECK_FEOF(ret_val, "reading an 4");
  if (*reverseEndian) input_integer= *reverseFourByteWord(&input_integer);

  if (input_integer != 4) {
    return(DCD_BADFORMAT);
  }

  /*  Read in the number of atoms			*/
  ret_val = READ(fd, N, sizeof(int));
  CHECK_FREAD(ret_val, "reading number of atoms");
  CHECK_FEOF(ret_val, "reading number of atoms");
  if (*reverseEndian) N=reverseFourByteWord(N);

  /*  Read in an 4				*/
  ret_val = READ(fd, &input_integer, sizeof(int));
  CHECK_FREAD(ret_val, "reading an 4");
  CHECK_FEOF(ret_val, "reading an 4");
  if (*reverseEndian) input_integer= *reverseFourByteWord(&input_integer);

  if (input_integer != 4) {
    return(DCD_BADFORMAT);
  }
 
  // std::cerr << "freeatoms " << (*N) << " " << (*NAMNF) << " " << ((*N)-(*NAMNF)) << std::endl;

  if (*NAMNF != 0) {
    (*FREEINDEXES) = (int *) calloc(((*N)-(*NAMNF)), sizeof(int));

    if (*FREEINDEXES == NULL)
      return(DCD_BADMALLOC);
	
    /*  Read in an size				*/
    ret_val = READ(fd, &input_integer, sizeof(int));
    CHECK_FREAD(ret_val, "reading size of index array");
    CHECK_FEOF(ret_val, "reading size of index array");
    if (*reverseEndian) input_integer= *reverseFourByteWord(&input_integer);

    if (input_integer != ((*N)-(*NAMNF))*4) {
      return(DCD_BADFORMAT);
    }
	
    // Read index array of free atoms	
    ret_val = READ(fd, (*FREEINDEXES), ((*N)-(*NAMNF))*sizeof(int));
    CHECK_FREAD(ret_val, "reading index array");
    CHECK_FEOF(ret_val, "reading index array");
    if (*reverseEndian)
    {
      for (input_integer =0; input_integer < ((*N)-(*NAMNF)); input_integer++)
      {
        FREEINDEXES[input_integer]= reverseFourByteWord(FREEINDEXES[input_integer]);
      }
    }

    // Don't know why the size is read again!
    ret_val = READ(fd, &input_integer, sizeof(int));
    CHECK_FREAD(ret_val, "reading size of index array");
    CHECK_FEOF(ret_val, "reading size of index array");
    if (*reverseEndian) input_integer= *reverseFourByteWord(&input_integer);

    if (input_integer != ((*N)-(*NAMNF))*4) {
      return(DCD_BADFORMAT);
    }
  }

  return(0);
}

/************************************************************************/
/*									*/
/*			FUNCTION read_dcdstep				*/
/*									*/
/*   INPUTS:								*/
/*	fd - file descriptor to use					*/
/*	N - Number of atoms						*/
/*	X - array of X coordinates					*/
/*	Y - array of Y coordinates					*/
/*	Z - array of Z coordinates					*/
/*	num_fixed - Number of fixed atoms				*/
/*	first - flag 1->first time called				*/
/*	indexes - indexes for free atoms				*/
/*      reverseEndian - is the byte order reversed?                     */
/*      charmm - Special CHARMm flags (see read_dcdheader())            */
/*									*/
/*   OUTPUTS:								*/
/*	read step populates the arrays X, Y, Z and returns a 0 if the   */
/*   read is successful.  If the read fails, a negative error code is   */
/*   returned.								*/
/*									*/
/*	read_step reads in the coordinates from one step.  It places    */
/*   these coordinates into the arrays X, Y, and Z.			*/
/*   The function can detect if a DCD file is in reverse endian         */
/*   byte order. The appropriate byte reverse function will be          */
/*   called for all values read.                                        */
/*									*/
/************************************************************************/

int read_dcdstep(FILE * fd, int N, float *X, float *Y, float *Z, int num_fixed,
		 int first, int *indexes, int reverseEndian, int charmm) {
  int ret_val;          /*  Return value from read		*/
  int input_integer;    /*  Integer buffer space		*/
  int i;	        /*  Loop counter			*/
  float *freeatoms;     /*  Local array to read free atoms      */

  // First frame contains the coordinates for all atoms,
  // while the other frames contain only coords for the free atoms.
  if ( (num_fixed==0) || first) {

    /* If this is a CHARMm file and contains an extra data block,
       we must skip it to avoid problems */
    if ((charmm & DCD_IS_CHARMM) &&
        (charmm & DCD_HAS_EXTRA_BLOCK)) {
        ret_val = READ(fd, &input_integer, sizeof(int));
        CHECK_FREAD(ret_val, "reading extra charmm block");
        if (reverseEndian) input_integer = *reverseFourByteWord(&input_integer);
        fseek(fd, input_integer, SEEK_CUR); //skip extra charmm block
        ret_val = READ(fd, &input_integer, sizeof(int));
        CHECK_FREAD(ret_val, "reading extra charmm block");
    }

    /*  Get the first size from the file                          */
    ret_val = READ(fd, &input_integer, sizeof(int));
    CHECK_FREAD(ret_val, "reading number of atoms at begining of step");
    if (reverseEndian) input_integer= *reverseFourByteWord(&input_integer);

    /*  See if we've reached the end of the file                  */
    if (ret_val == 0) {
      return(-1);   // EOF
    }

    if (input_integer != 4*N)
      return(DCD_BADFORMAT);

    ret_val = READ(fd, X, 4*N); 
    CHECK_FREAD(ret_val, "reading X array");
    CHECK_FEOF(ret_val, "reading X array");
    if (reverseEndian) {
      for (i=0; i<N; i++) {
        X[i]=*((float*)(reverseFourByteWord((int*)&X[i])));
      }
    }

    ret_val = READ(fd, &input_integer, sizeof(int));
    CHECK_FREAD(ret_val, "reading number of atoms after X array");
    CHECK_FEOF(ret_val, "reading number of atoms after X array");
    if (reverseEndian) input_integer= *reverseFourByteWord(&input_integer);

    if (input_integer != 4*N) 
      return(DCD_BADFORMAT);

    ret_val = READ(fd, &input_integer, sizeof(int));
    CHECK_FREAD(ret_val, "reading number of atoms after X array");
    CHECK_FEOF(ret_val, "reading number of atoms after X array");
    if (reverseEndian) input_integer= *reverseFourByteWord(&input_integer);

    if (input_integer != 4*N)
      return(DCD_BADFORMAT);

    ret_val = READ(fd, Y, 4*N);
    CHECK_FREAD(ret_val, "reading Y array");
    CHECK_FEOF(ret_val, "reading Y array");
    if (reverseEndian) {
      for (i=0; i<N; i++) {
        Y[i]=*((float*)(reverseFourByteWord((int*)&Y[i])));
      }
    }

    ret_val = READ(fd, &input_integer, sizeof(int));
    CHECK_FREAD(ret_val, "reading number of atoms after Y array");
    CHECK_FEOF(ret_val, "reading number of atoms after Y array");
    if (reverseEndian) input_integer= *reverseFourByteWord(&input_integer);

    if (input_integer != 4*N)
      return(DCD_BADFORMAT);

    ret_val = READ(fd, &input_integer, sizeof(int));
    CHECK_FREAD(ret_val, "reading number of atoms after Y array");
    CHECK_FEOF(ret_val, "reading number of atoms after Y array");
    if (reverseEndian) input_integer= *reverseFourByteWord(&input_integer);

    if (input_integer != 4*N)
      return(DCD_BADFORMAT);

    ret_val = READ(fd, Z, 4*N);
    CHECK_FREAD(ret_val, "reading Z array");
    CHECK_FEOF(ret_val, "reading Z array");
    if (reverseEndian) {
      for (i=0; i<N; i++) {
        Z[i]=*((float*)(reverseFourByteWord((int*)&Z[i])));
      }
    }

    ret_val = READ(fd, &input_integer, sizeof(int));
    CHECK_FREAD(ret_val, "reading number of atoms after Z array");
    CHECK_FEOF(ret_val, "reading number of atoms after Z array");
    if (reverseEndian) input_integer= *reverseFourByteWord(&input_integer);

    if (input_integer != 4*N)
      return(DCD_BADFORMAT);

    /* If this is a CHARMm file and contains a 4th dimension block,
       we must skip it to avoid problems */
    if ((charmm & DCD_IS_CHARMM) &&
        (charmm & DCD_HAS_4DIMS)) {
        ret_val = READ(fd, &input_integer, sizeof(int));
        CHECK_FREAD(ret_val, "reading extra charmm block");
        if (reverseEndian) input_integer = *reverseFourByteWord(&input_integer);
        fseek(fd, input_integer, SEEK_CUR); // skip extra charmm block
        ret_val = READ(fd, &input_integer, sizeof(int));
        CHECK_FREAD(ret_val, "reading extra charmm block");
    }

  }
  else {
    /* there are fixed atoms and not the first time through... */

    /* If this is a CHARMm file and contains an extra data block,
       we must skip it to avoid problems */
    if ((charmm & DCD_IS_CHARMM) &&
        (charmm & DCD_HAS_EXTRA_BLOCK)) {
        ret_val = READ(fd, &input_integer, sizeof(int));
        CHECK_FREAD(ret_val, "reading extra charmm block");
        if (reverseEndian) input_integer = *reverseFourByteWord(&input_integer);
        fseek(fd, input_integer, SEEK_CUR); // skip extra charmm block
        ret_val = READ(fd, &input_integer, sizeof(int));
        CHECK_FREAD(ret_val, "reading extra charmm block");
    }

    /*  Get the first size from the file                          */
    ret_val = READ(fd, &input_integer, sizeof(int));
    CHECK_FREAD(ret_val, "reading number of atoms at begining of step");
    if (reverseEndian) input_integer= *reverseFourByteWord(&input_integer);

    /*  See if we've reached the end of the file                  */
    if (ret_val == 0) {
      return(-1);
    }

    if (input_integer != 4*(N-num_fixed))  /* check num of free atoms */
      return(DCD_BADFORMAT);

    /* load up free atoms X coords */
    freeatoms = (float *) malloc(sizeof(float)*(N-num_fixed)) ;

    ret_val = READ(fd, freeatoms, 4*(N-num_fixed)); 
    CHECK_FREAD(ret_val, "reading free atoms X array");
    CHECK_FEOF(ret_val, "reading free atoms X array");
    if (reverseEndian) {
      for (i=0; i<N-num_fixed; i++) {
        X[indexes[i]-1]=*((float*)(reverseFourByteWord((int*)&freeatoms[i])));
      }
    } else {
      for (i=0; i<N-num_fixed; i++) {
        X[indexes[i]-1] = freeatoms[i]; // update only the free atoms in X
      }
    }

    free(freeatoms);

    ret_val = READ(fd, &input_integer, sizeof(int));
    CHECK_FREAD(ret_val, "reading number of atoms after X array");
    CHECK_FEOF(ret_val, "reading number of atoms after X array");
    if (reverseEndian) input_integer= *reverseFourByteWord(&input_integer);

    if (input_integer != 4*(N-num_fixed)) /* check num of free atoms */
      return(DCD_BADFORMAT);

    ret_val = READ(fd, &input_integer, sizeof(int));
    CHECK_FREAD(ret_val, "reading number of atoms after X array");
    CHECK_FEOF(ret_val, "reading number of atoms after X array");
    if (reverseEndian) input_integer= *reverseFourByteWord(&input_integer);

    if (input_integer != 4*(N-num_fixed)) /* check num of free atoms */
      return(DCD_BADFORMAT);

    /* load up free atoms Y coords */
    freeatoms = (float *) malloc(sizeof(float)*(N-num_fixed));

    ret_val = READ(fd, freeatoms, 4*(N-num_fixed)); 
    CHECK_FREAD(ret_val, "reading free atoms Y array");
    CHECK_FEOF(ret_val, "reading free atoms Y array");
    if (reverseEndian) {
      for (i=0; i<N-num_fixed; i++) {
        Y[indexes[i]-1]=*((float*)(reverseFourByteWord((int*)&freeatoms[i])));
      }
    } else {
      for (i=0; i<N-num_fixed; i++) {
        Y[indexes[i]-1]=freeatoms[i]; // update only the free atoms in Y
      }
    }

    free(freeatoms);

    ret_val = READ(fd, &input_integer, sizeof(int));
    CHECK_FREAD(ret_val, "reading number of atoms after Y array");
    CHECK_FEOF(ret_val, "reading number of atoms after Y array");
    if (reverseEndian) input_integer= *reverseFourByteWord(&input_integer);

    if (input_integer != 4*(N-num_fixed))
      return(DCD_BADFORMAT);

    ret_val = READ(fd, &input_integer, sizeof(int));
    CHECK_FREAD(ret_val, "reading number of atoms after Y array");
    CHECK_FEOF(ret_val, "reading number of atoms after Y array");
    if (reverseEndian) input_integer= *reverseFourByteWord(&input_integer);

    if (input_integer != 4*(N-num_fixed))
      return(DCD_BADFORMAT);

    /* load up free atoms Z coords */
    freeatoms = (float *) malloc(sizeof(float)*(N-num_fixed));

    ret_val = READ(fd, freeatoms, 4*(N-num_fixed)); 
    CHECK_FREAD(ret_val, "reading free atoms Z array");
    CHECK_FEOF(ret_val, "reading free atoms Z array");
    if (reverseEndian) {
      for (i=0; i<N-num_fixed; i++) {
        Z[indexes[i]-1]=*((float*)(reverseFourByteWord((int*)&freeatoms[i])));
      }
    } else {
      for (i=0; i<N-num_fixed; i++) {
        Z[indexes[i]-1]=freeatoms[i]; // update only the free atoms in Z
      }
    }

    free(freeatoms);

    ret_val = READ(fd, &input_integer, sizeof(int));
    CHECK_FREAD(ret_val, "reading number of atoms after Z array");
    CHECK_FEOF(ret_val, "reading number of atoms after Z array");
    if (reverseEndian) input_integer= *reverseFourByteWord(&input_integer);

    if (input_integer != 4*(N-num_fixed))
      return(DCD_BADFORMAT);

    /* If this is a CHARMm file and contains a 4th dimension block,
       we must skip it to avoid problems */
    if ((charmm & DCD_IS_CHARMM) &&
        (charmm & DCD_HAS_4DIMS)) {
        ret_val = READ(fd, &input_integer, sizeof(int));
        CHECK_FREAD(ret_val, "reading extra charmm block");
        if (reverseEndian) input_integer = *reverseFourByteWord(&input_integer);
        fseek(fd, input_integer, SEEK_CUR); // skip extra charmm block
        ret_val = READ(fd, &input_integer, sizeof(int));
        CHECK_FREAD(ret_val, "reading extra charmm block");
    }
  }

  return(0);
}



//--------------------------------------------------------------------------------


int skip_one_frame(FILE * fd, int N, int num_fixed, int first, int reverseEndian, int charmm) 
{
  int ret_val;          /*  Return value from read		*/
  int input_integer;    /*  Integer buffer space		*/
  int skip=0;             // number of bytes per frame

  //  int st = ftell( fd); // get the start position

  //debugging stuff
  //charmm|= DCD_IS_CHARMM;
  //charmm|= DCD_HAS_EXTRA_BLOCK;

  // Read the blocksize = number of atoms
  //ret_val = READ(fd, &input_integer, sizeof(int));
  //std::cerr << "natoms " << input_integer<<" start "<<ftell(fd)<<std::endl;
  //std::cerr << "charmm " << charmm;

  //fseek(fd, st, SEEK_SET); // reset indicator

  // First frame contains the coordinates for all atoms,
  // while the other frames contain only coords for the free atoms.

  if ( (num_fixed==0) || first) {
    /* If this is a CHARMm file and contains an extra data block,
       we must skip it to avoid problems */
    if ((charmm & DCD_IS_CHARMM) &&
        (charmm & DCD_HAS_EXTRA_BLOCK)) {
        ret_val = READ(fd, &input_integer, sizeof(int));
        CHECK_FREAD(ret_val, "reading extra charmm block");
        if (reverseEndian) input_integer = *reverseFourByteWord(&input_integer);
        fseek(fd, input_integer, SEEK_CUR); //skip extra charmm block
        ret_val = READ(fd, &input_integer, sizeof(int));
        CHECK_FREAD(ret_val, "reading extra charmm block");
    }

    /*  Get the first size from the file                          */
    ret_val = READ(fd, &input_integer, sizeof(int));
    CHECK_FREAD(ret_val, "reading number of atoms at begining of step");
    if (reverseEndian) input_integer= *reverseFourByteWord(&input_integer);

    /*  See if we've reached the end of the file                  */
    if (ret_val == 0) {
      return(-1);   // EOF
    }

    if (input_integer != 4*N)
      return(DCD_BADFORMAT);

    /*  Three data blocks and the 5 remaining surrounding size blocks */
    skip+=3*4*N;
    skip+=5*sizeof(int);

    /* If this is a CHARMm file and contains a 4th dimension block,
       we must skip it to avoid problems */
    if ((charmm & DCD_IS_CHARMM) &&
        (charmm & DCD_HAS_4DIMS)) {
      skip+=2*sizeof(int);
      skip+=input_integer; // input_integer=nAtoms
    }
  }

  else {
    /* there are fixed atoms and not the first time through... */
    /* If this is a CHARMm file and contains an extra data block,
       we must skip it to avoid problems */
  
    /* If this is a CHARMm file and contains an extra data block,
       we must skip it to avoid problems */
    if ((charmm & DCD_IS_CHARMM) &&
        (charmm & DCD_HAS_EXTRA_BLOCK)) {
        ret_val = READ(fd, &input_integer, sizeof(int));
        CHECK_FREAD(ret_val, "reading extra charmm block");
        if (reverseEndian) input_integer = *reverseFourByteWord(&input_integer);
        fseek(fd, input_integer, SEEK_CUR); //skip extra charmm block
        ret_val = READ(fd, &input_integer, sizeof(int));
        CHECK_FREAD(ret_val, "reading extra charmm block");
    }

    /*  Get the first size from the file                          */
    ret_val = READ(fd, &input_integer, sizeof(int));
    CHECK_FREAD(ret_val, "reading number of atoms at begining of step");
    if (reverseEndian) input_integer= *reverseFourByteWord(&input_integer);

    /*  See if we've reached the end of the file                  */
    if (ret_val == 0) {
      return(-1);
    }
  
    if (input_integer != 4*(N-num_fixed))
      return(DCD_BADFORMAT);


    /*  Three data blocks and the 5 remaining surrounding size blocks */
    skip+=3*4*(N-num_fixed);
    skip+=5*sizeof(int);

    /* If this is a CHARMm file and contains a 4th dimension block,
       we must skip it to avoid problems */
    if ((charmm & DCD_IS_CHARMM) &&
        (charmm & DCD_HAS_4DIMS)) {
      skip+=2*sizeof(int);
      skip+=input_integer; // input_integer=nAtoms
    }
  }

  // Now skip over the entire block:
  fseek(fd, skip, SEEK_CUR);
  
  //int en = ftell( fd);
  //std::cerr << "read_dcdstep: start = " << st << std::endl;
  //std::cerr << "read_dcdstep: end   = " << en << std::endl;
  //std::cerr << "en-st= " << en-st << "skip = " << skip <<std::endl;
  return(0);
}





/*********************************************************************/
/*								     */
/*			FUNCTION open_dcd_write			     */
/*								     */
/*   INPUTS:							     */
/*	dcdfile - Name of the dcd file				     */
/*								     */
/*   OUTPUTS:							     */
/*	returns an open file descriptor for writing		     */
/*								     */
/*	This function will open a dcd file for writing.  It takes    */
/*   the filename to open as its only argument.	 It will return a    */
/*   valid file descriptor if successful or DCD_OPENFAILED if the    */
/*   open fails for some reason.  If the file specifed already       */
/*   exists, DCD_FILEEXISTS is returned.			     */
/*								     */
/*********************************************************************/

FILE * open_dcd_write(char *dcdname) {
  return fopen(dcdname, "w+b");
}

/************************************************************************/
/*									*/
/*				FUNCTION write_dcdstep			*/
/*									*/
/*   INPUTS:								*/
/*	fd - file descriptor for the DCD file to write to		*/
/*	N - Number of atoms						*/
/*	X - X coordinates						*/
/*	Y - Y coordinates						*/
/*	Z - Z coordinates						*/
/*									*/
/*   OUTPUTS:								*/
/*	none								*/
/*									*/
/*	write_dcdstep writes the coordinates out for a given timestep   */
/*   to the specified DCD file.						*/
/*									*/
/************************************************************************/

int write_dcdstep(FILE * fd, int N, float *X, float *Y, float *Z)
{
	int out_integer;

	out_integer = N*4;
	WRITE(fd, (char *) &out_integer, sizeof(int));
	WRITE(fd, (char *) X, out_integer);
	WRITE(fd, (char *) &out_integer, sizeof(int));
	WRITE(fd, (char *) &out_integer, sizeof(int));
	WRITE(fd, (char *) Y, out_integer);
	WRITE(fd, (char *) &out_integer, sizeof(int));
	WRITE(fd, (char *) &out_integer, sizeof(int));
	WRITE(fd, (char *) Z, out_integer);
	WRITE(fd, (char *) &out_integer, sizeof(int));
	return (ferror(fd));
}


/************************************************************************/
/*									*/
/*				FUNCTION write_dcdstep			*/
/*									*/
/*   INPUTS:								*/
/*	fd - file descriptor for the DCD file to write to		*/
/*	N -         Number of atoms					*/
/*	num_fixed - Number of fixed atoms  			        */
/*	indexes -   array of fixed atom indexes				*/
/*	X -         X coordinates					*/
/*	Y -         Y coordinates					*/
/*	Z -         Z coordinates					*/
/*									*/
/*   OUTPUTS:								*/
/*	none								*/
/*									*/
/*	write_dcdstep writes the coordinates out for a given timestep   */
/*   to the specified DCD file.						*/
/*									*/
/************************************************************************/

int write_dcdstep(FILE * fd, int N, int num_fixed, int *indexes,
                  const float *X, const float *Y, const float *Z)
{
    int out_integer;
    int i;	        /*  Loop counter			*/
    float *freeatoms;   /*  Local array to read free atoms      */
    static int first=1; /*  first=1 only for the first call     */
    
    /* First frame contains the coordinates for all atoms, */
    /* while the other frames contain only coords for the free atoms. */
    if ( (num_fixed==0) || first) {
	first = 0;
	out_integer = 4*N; /* Size of array */
	WRITE(fd, (char *) &out_integer, sizeof(int));
	WRITE(fd, (const char *) X, out_integer);
	WRITE(fd, (char *) &out_integer, sizeof(int));
	WRITE(fd, (char *) &out_integer, sizeof(int));
	WRITE(fd, (const char *) Y, out_integer);
	WRITE(fd, (char *) &out_integer, sizeof(int));
	WRITE(fd, (char *) &out_integer, sizeof(int));
	WRITE(fd, (const char *) Z, out_integer);
	WRITE(fd, (char *) &out_integer, sizeof(int));
    } else {
	/* there are fixed atoms and not the first time through... */

	freeatoms = (float *) malloc(sizeof(float)*(N-num_fixed)) ;
	out_integer = 4*(N-num_fixed); /* Size of array */

	WRITE(fd, (char *) &out_integer, sizeof(int));
	for (i=0; i<N-num_fixed; i++) {
	    freeatoms[i] = X[indexes[i]-1]; /* write only the free atoms in X */
	}
	WRITE(fd, (const char *) freeatoms, out_integer);
	WRITE(fd, (char *) &out_integer, sizeof(int));
    
	WRITE(fd, (char *) &out_integer, sizeof(int));
	for (i=0; i<N-num_fixed; i++) {
	    freeatoms[i] = Y[indexes[i]-1]; /* write only the free atoms in Y */
	}
	WRITE(fd, (const char *) freeatoms, out_integer);
	WRITE(fd, (char *) &out_integer, sizeof(int));

	WRITE(fd, (char *) &out_integer, sizeof(int));
	for (i=0; i<N-num_fixed; i++) {
	    freeatoms[i] = Z[indexes[i]-1]; /* write only the free atoms in Z */
	}
	WRITE(fd, (const char *) freeatoms, out_integer);
	WRITE(fd, (char *) &out_integer, sizeof(int));
    }
    return 1;
}


/*****************************************************************************/
/*									     */
/*				FUNCTION write_dcdheader		     */
/*									     */
/*   INPUTS:								     */
/*	fd - file descriptor for the dcd file				     */
/*	N - Number of atoms						     */
/*	NSET - Number of sets of coordinates				     */
/*	ISTART - Starting timestep of DCD file				     */
/*	NSAVC - Timesteps between DCD saves				     */
/*	DELTA - length of a timestep					     */
/*									     */
/*   OUTPUTS:								     */
/*	none								     */
/*									     */
/*	This function prints the "header" information to the DCD file.  Since*/
/*   this is duplicating an unformatted binary output from FORTRAN, its ugly.*/
/*   So if you're squimish, don't look.					     */
/*									     */
/*****************************************************************************/

int write_dcdheader(FILE * fd, char *filename, int N, int NSET, int ISTART, 
		   int NSAVC, double DELTA)
{
  int	out_integer;
  char	title_string[200];
  //time_t  cur_time;
  //struct  tm *tmbuf;
  //char    time_str[11];

  out_integer = 84;
  WRITE(fd, (char *) & out_integer, sizeof(int));
  strcpy(title_string, "CORD");
  WRITE(fd, title_string, 4);
  WRITE(fd, (char *) &NSET, sizeof(int));
  WRITE(fd, (char *) &ISTART, sizeof(int));
  WRITE(fd, (char *) &NSAVC, sizeof(int));
  out_integer=0;
  WRITE(fd, (char *) &out_integer, sizeof(int));
  WRITE(fd, (char *) &out_integer, sizeof(int));
  WRITE(fd, (char *) &out_integer, sizeof(int));
  WRITE(fd, (char *) &out_integer, sizeof(int));
  WRITE(fd, (char *) &out_integer, sizeof(int));
  WRITE(fd, (char *) &out_integer, sizeof(int));
  WRITE(fd, (char *) &DELTA, sizeof(double));
  WRITE(fd, (char *) &out_integer, sizeof(int));
  WRITE(fd, (char *) &out_integer, sizeof(int));
  WRITE(fd, (char *) &out_integer, sizeof(int));
  WRITE(fd, (char *) &out_integer, sizeof(int));
  WRITE(fd, (char *) &out_integer, sizeof(int));
  WRITE(fd, (char *) &out_integer, sizeof(int));
  WRITE(fd, (char *) &out_integer, sizeof(int));
  WRITE(fd, (char *) &out_integer, sizeof(int));
  WRITE(fd, (char *) &out_integer, sizeof(int));
  out_integer = 84;
  WRITE(fd, (char *) & out_integer, sizeof(int));

  out_integer = 164;
  WRITE(fd, (char *) & out_integer, sizeof(int));
  out_integer = 2;
  WRITE(fd, (char *) & out_integer, sizeof(int));

  sprintf(title_string, "REMARKS FILENAME=%s CREATED BY READDCD", filename);
  pad(title_string, 80);
  WRITE(fd, title_string, 80);

  //cur_time=time(NULL);
  //tmbuf=localtime(&cur_time);
  //strftime(time_str, 10, "%m/%d/%y", tmbuf);

  //sprintf(title_string, "REMARKS DATE: %s CREATED BY USER: %s",
  //   time_str, vmd_username());
  //sprintf(title_string, "REMARKS DATE: %s",
  //   time_str);
  pad(title_string, 80);
  WRITE(fd, title_string, 80);
  out_integer = 164;
  WRITE(fd, (char *) & out_integer, sizeof(int));
  out_integer = 4;
  WRITE(fd, (char *) & out_integer, sizeof(int));
  out_integer = N;
  WRITE(fd, (char *) & out_integer, sizeof(int));
  out_integer = 4;
  WRITE(fd, (char *) & out_integer, sizeof(int));
  return (ferror(fd));
}

/****************************************************************/
/*								*/
/*			FUNCTION close_dcd_read			*/
/*								*/
/*   INPUTS:							*/
/*	fd - file descriptor to close				*/
/*	num_fixed - the number of fixed atoms			*/
/*	indexes - Array of indexes to be deallocated		*/
/*								*/
/*   OUTPUTS:							*/
/*	the file pointed to by fd is closed and the memory      */
/*   pointed to by indexes is freed if any was allocated	*/
/*								*/
/*	close_dcd_read closes a dcd file that was opened for    */
/*   reading.  It also deallocated memory used for the indexes  */
/*   used for the free atom list, if there were fixed atoms.    */
/*								*/
/****************************************************************/

void close_dcd_read(FILE* fd, int num_fixed, int *indexes) {
  if (fd) fclose(fd);

  if (num_fixed) {
    free(indexes);
  }
}

/****************************************************************/
/*								*/
/*			FUNCTION close_dcd_write		*/
/*								*/
/*   INPUTS:							*/
/*	fd - file descriptor to close				*/
/*								*/
/*   OUTPUTS:							*/
/*	the file pointed to by fd				*/
/*								*/
/*	close_dcd_write close a dcd file that was opened for    */
/*   writing							*/
/*								*/
/****************************************************************/

void close_dcd_write(FILE * fd) {
  fclose(fd);
}




