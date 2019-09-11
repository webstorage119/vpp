/** @file

MODULE						: VectorQuantiserDim16Impl3

TAG								: VQD16I3

FILE NAME					: VectorQuantiserDim16Impl3.cpp

DESCRIPTION				: A class to extend the VectorQuantiser class for video 4x4
										dimension 6 bit colour component values. NOTE: After 
										instantiation of an object Create() must be called. The 
										search algorithm includes early exit and the distortion 
										inequality equation.

REVISION HISTORY	:
									: 

COPYRIGHT					: (c)VICS 2000-2006  all rights resevered - info@videocoding.com

RESTRICTIONS			: The information/data/code contained within this file is 
										the property of VICS limited and has been classified as 
										CONFIDENTIAL.
===========================================================================
*/
#ifdef _WINDOWS
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#else
#include <stdio.h>
#endif

#include <memory.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "VectorQuantiserDim16Impl3.h"

/*
---------------------------------------------------------------------------
	Class constants.
---------------------------------------------------------------------------
*/
#define VQD16I3_TABLE_SIZE	256
#define VQD16I3_VECTOR_DIM	16
#define VQD16I3_DIM_SHIFT		4

/*
---------------------------------------------------------------------------
	Public Interface.
---------------------------------------------------------------------------
*/
VectorQuantiserDim16Impl3::VectorQuantiserDim16Impl3()
{
	VQ_CODEBOOK = NULL;
	VQ_SUM			= NULL;
}//end constructor.

VectorQuantiserDim16Impl3::~VectorQuantiserDim16Impl3()
{
	Destroy();
}//end destructor.

	/** Create and destroy private mem objects.
	Methods to alloc and delete memory that is best not
	done in the constructor as failure to alloc cannot 
	be reported. (Try-catch trap is possible).
	@return	: 1 = success, 0 = failure.
	*/
int VectorQuantiserDim16Impl3::Create(void)
{
	if(VQ_CODEBOOK == NULL)
	{
		VQ_CODEBOOK = new const short*[VQD16I3_TABLE_SIZE];
		if(VQ_CODEBOOK == NULL)
			return(0);	// Alloc failed.

		for(int i = 0; i < VQD16I3_TABLE_SIZE; i++)
			VQ_CODEBOOK[i] = &(VQ_TABLE[VQD16I3_VECTOR_DIM * i]);

	}//end if !VQ_CODEBOOK...

	if(VQ_SUM == NULL)
	{
		VQ_SUM = new short[VQD16I3_TABLE_SIZE];
		if(VQ_SUM == NULL)
		{
			delete[] VQ_CODEBOOK;
			VQ_CODEBOOK = NULL;
			return(0);
		}//end if !VQ_SUM...

		// Fill it with the sum of each vq vector.
		for(int j = 0; j < VQD16I3_TABLE_SIZE; j++)
		{
			short sum = 0;
			for(int k = 0; k < VQD16I3_VECTOR_DIM; k++)
				sum += VQ_CODEBOOK[j][k];
			VQ_SUM[j] = sum;
		}//end for j...

	}//end if !VQ_SUM...

///////////////////////////////////////////////////////////
	// Test code.
//	int i,j,test;
//	srand(31);
//	short vec[16];
//	int count = 0;
//	int tests = 10;
//	
//	for(test = 0; test < tests; test++)
//	{
//		int tester = 0;
//		int sumVec = 0;
//		// Make a vector.
//		for(i = 0; i < 16; i++)
//		{
//			vec[i] = (rand() * 63)/RAND_MAX - 32;
//			sumVec += vec[i];
//		}//end for i...
//
//		// Test against.
//		int tDist;
//		int tWin = Quantise((const void *)vec, &tDist);
//
//		// Scan the table.
//		int win = 0;
//		int winDist = 10000000;
//		int winIneq = 10000000;
//		for(i = 0; i < VQD16I3_TABLE_SIZE; i++)
//		{
//			int sumRef = 0;
//			for(j = 0; j < 16; j++)
//				sumRef += VQ_CODEBOOK[i][j];
//
//			int inequality = ((sumVec - sumRef)*(sumVec - sumRef))/16;
//
//			if(inequality <= winDist)
//			{
//				count++;
//				int dist = 0;
//				for(j = 0; j < 16; j++)
//				{
//					int diff = (vec[j] - VQ_CODEBOOK[i][j]);
//					dist += (diff * diff);
//				}//end for j...
//
//				if(dist < winDist)
//				{
//					win = i;
//					winDist = dist;
//					winIneq = inequality;
//				}//end if dist...
//			}//end if inequality...
//
//		}//end for i...
//
//		// test inequality.
//		if( tWin != win)
//			tester = 1;
//
//	}//end for test...
//
//	int savings = (VQD16I3_TABLE_SIZE * tests) - count;
//	double fs = (double)savings / (double)(VQD16I3_TABLE_SIZE * tests) ;
//
///////////////////////////////////////////////////////////

	return(1);
}//end Create.

void VectorQuantiserDim16Impl3::Destroy(void)
{
	if(VQ_CODEBOOK != NULL)
		delete[] VQ_CODEBOOK;
	VQ_CODEBOOK = NULL;

	if(VQ_SUM != NULL)
		delete[] VQ_SUM;
	VQ_SUM = NULL;
}//end Destroy.

/** Get the implementation vector dimension.
This is defined as a constant of the code book.
@return	: Vector dimension.
*/
int	VectorQuantiserDim16Impl3::GetDimension(void)
{
	return(VQD16I3_VECTOR_DIM);
}//end GetDimension.

/** Get the number of vectors in the code book.
This is defined as a constant of the code book.
@return	: Code book length.
*/
int VectorQuantiserDim16Impl3::GetCodeBookLength(void)
{
	return(VQD16I3_TABLE_SIZE);
}//end GetCodeBookLength.

/** Inverse quantise the index.
Return a pointer to the code book vector referenced by the input
index.
@param index	: Code book vector index.
@return				: Reference to the code book vector.
*/
const void* VectorQuantiserDim16Impl3::InverseQuantise(int index)
{
	if(index < VQD16I3_TABLE_SIZE)
		return((void *)VQ_CODEBOOK[index]);
	return(NULL);
}//end InverseQuantise.

/** Quantise the input vector.
Match the input vector with the implementation code book element
that produces the smallest distortion.
@param vector			:	Input vector to match in the code book.
@param distortion	:	Distortion of the winning vector.
@return						: Winning vector index.
*/
int VectorQuantiserDim16Impl3::Quantise(const void* vector, int* distortion)
{
  int	 book;
  int	 best_dist	= 0x0FFFFFFF;
  int	 best_index	= 0;

	// Set the inequality comparison value for this vector.
	const short* p = (const short *)vector;
	short vecSum = 0;
	for(int i = 0; i < VQD16I3_VECTOR_DIM; i++)
		vecSum += *(p++);

  for(book = 0; book < VQD16I3_TABLE_SIZE; book++)
  {
		// Test inequality equation first.
		int inequality = ( 8 + ((vecSum - VQ_SUM[book])*(vecSum - VQ_SUM[book]))) >> 4;
		int	x,y;
    int						dist	= 0;
    const short*	pC		= VQ_CODEBOOK[book];
		const short*	pV		= (const short *)vector;

		if(inequality > best_dist)
			goto VQD16I3_early_terminate;

		// [0][0].
		x	= *(pV++) - *(pC++);
		dist += (x*x);
		if(dist >= best_dist)
			goto VQD16I3_early_terminate;

		// [0][1].
		y	= *(pV++) - *(pC++);
		dist += (y*y);
		if(dist >= best_dist)
			goto VQD16I3_early_terminate;

		// [0][2].
		x	= *(pV++) - *(pC++);
		dist += (x*x);
		if(dist >= best_dist)
			goto VQD16I3_early_terminate;

		// [0][3].
		y	= *(pV++) - *(pC++);
		dist += (y*y);
		if(dist >= best_dist)
			goto VQD16I3_early_terminate;

		// [1][0].
		x	= *(pV++) - *(pC++);
		dist += (x*x);
		if(dist >= best_dist)
			goto VQD16I3_early_terminate;

		// [1][1].
		y	= *(pV++) - *(pC++);
		dist += (y*y);
		if(dist >= best_dist)
			goto VQD16I3_early_terminate;

		// [1][2].
		x	= *(pV++) - *(pC++);
		dist += (x*x);
		if(dist >= best_dist)
			goto VQD16I3_early_terminate;

		// [1][3].
		y	= *(pV++) - *(pC++);
		dist += (y*y);
		if(dist >= best_dist)
			goto VQD16I3_early_terminate;

		// [2][0].
		x	= *(pV++) - *(pC++);
		dist += (x*x);
		if(dist >= best_dist)
			goto VQD16I3_early_terminate;

		// [2][1].
		y	= *(pV++) - *(pC++);
		dist += (y*y);
		if(dist >= best_dist)
			goto VQD16I3_early_terminate;

		// [2][2].
		x	= *(pV++) - *(pC++);
		dist += (x*x);
		if(dist >= best_dist)
			goto VQD16I3_early_terminate;

		// [2][3].
		y	= *(pV++) - *(pC++);
		dist += (y*y);
		if(dist >= best_dist)
			goto VQD16I3_early_terminate;

		// [3][0].
		x	= *(pV++) - *(pC++);
		dist += (x*x);
		if(dist >= best_dist)
			goto VQD16I3_early_terminate;

		// [3][1].
		y	= *(pV++) - *(pC++);
		dist += (y*y);
		if(dist >= best_dist)
			goto VQD16I3_early_terminate;

		// [3][2].
		x	= *(pV++) - *(pC++);
		dist += (x*x);
		if(dist >= best_dist)
			goto VQD16I3_early_terminate;

		// [3][3].
		y	= *(pV) - *(pC);
		dist += (y*y);

    if(dist < best_dist)
    {
      best_dist		= dist;
      best_index	= book;
    }//end if dist...

		VQD16I3_early_terminate: ;//null

  }//end for book...

  *distortion = best_dist;
  return(best_index);
}//end Quantise.

/*
---------------------------------------------------------------------------
	Constants.
---------------------------------------------------------------------------
*/
const short VectorQuantiserDim16Impl3::VQ_TABLE[VQD16I3_TABLE_SIZE * VQD16I3_VECTOR_DIM] =
{
    -1,   0,  -1,  -1,   0,   0,   0,   0,   0,  -1,   0,   0,   0,   0,   0,   0,  //0
     0,   0,   0,   0,   0,  -1,  -1,   0,   0,   0,   0,   0,   0,   0,   0,   0,  //1
     0,   1,   0,   0,  -1,   0,   0,   0,   0,   1,   0,   0,  -1,   0,   0,   0,  //2
     0,   0,   1,   0,   0,   0,   0,   1,   1,   0,   0,   1,   1,   1,   0,   0,  //3
     0,   0,   0,   0,   0,   0,   1,   0,  -1,  -1,   0,   0,   0,   0,   1,   0,  //4
     1,   0,  -1,  -1,   1,   1,   0,  -1,   0,   1,   0,  -1,   0,   0,   0,  -1,  //5
    -1,  -2,  -1,  -1,  -2,  -1,  -1,  -1,  -1,  -2,  -1,  -1,  -1,  -2,  -1,  -1,  //6
     2,   2,   2,   1,   2,   2,   2,   2,   2,   2,   2,   1,   1,   2,   2,   2,  //7
    -3,  -4,  -4,  -4,  -2,  -4,  -4,  -4,  -2,  -3,  -3,  -3,  -2,  -2,  -3,  -4,  //8
    -1,  -1,  -1,  -1,  -1,   0,   0,   1,   0,   1,   2,   2,   1,   3,   5,   4,  //9
     4,   4,   4,   3,   5,   4,   5,   3,   4,   4,   4,   3,   5,   5,   4,   4,  //10
    -1,  -1,   1,   4,  -1,  -1,   1,   5,  -2,  -1,   0,   3,  -2,  -2,   0,   2,  //11
    -6,  -6,  -5,  -6,  -7,  -6,  -6,  -6,  -6,  -6,  -6,  -6,  -6,  -6,  -6,  -5,  //12
     1,   0,   0,  -1,   2,   0,   0,   0,  -2,  -1,   0,   0,  -6,  -3,  -1,   1,  //13
     3,   4,   5,   4,   1,   2,   2,   1,  -1,   0,  -1,  -1,  -2,  -2,  -3,  -3,  //14
    -6,  -5,  -3,  -1,  -6,  -4,  -2,   0,  -5,  -3,  -1,   0,  -4,  -2,   0,   2,  //15
     7,   7,   7,   7,   7,   8,   8,   7,   7,   8,   8,   8,   7,   8,   8,   8,  //16
     2,   1,  -1,  -2,   2,   0,  -3,  -4,   2,  -2,  -4,  -6,   1,  -1,  -5,  -6,  //17
     1,   1,  -3,  -7,   1,   1,  -1,  -6,   1,   1,   1,  -2,   0,   1,   1,   0,  //18
     1,   0,  -1,   0,   2,   1,   0,   0,   6,   2,   0,  -1,   8,   5,   1,   0,  //19
     6,   5,   2,   1,   7,   4,   1,   0,   4,   2,   0,   0,   2,  -1,  -1,  -1,  //20
    -1,   0,   1,   1,  -4,  -2,  -1,  -1,  -5,  -5,  -4,  -3,  -6,  -8,  -6,  -4,  //21
     0,   1,   4,   6,   0,   2,   5,   7,   1,   3,   6,   7,   2,   3,   6,   8,  //22
   -11, -10,  -9,  -9, -11, -10, -10, -10, -10, -10, -10,  -9, -10,  -9,  -9,  -9,  //23
     1,   1,   2,   1,  -3,  -4,  -4,  -3,  -2,  -2,  -3,  -4,   0,   2,   3,   2,  //24
    -2,  -2,   1,   2,  -2,  -1,   1,   2,   0,   0,   0,  -3,   1,   1,  -1,  -9,  //25
    14,  14,  13,  12,  14,  14,  13,  13,  14,  13,  13,  12,  13,  12,  12,  12,  //26
     2,  -3,  -6,   3,   2,  -2,  -6,   2,   2,  -3,  -4,   2,   1,  -3,  -2,   1,  //27
    -4,   3,   3,   0,  -7,   3,   4,   2,  -7,   2,   3,   2,  -7,   1,   2,   1,  //28
     5,   9,   9,   9,   2,   6,   8,   8,  -1,   1,   3,   5,  -1,  -1,   0,   2,  //29
    -3,  -6,  -7,  -5,  -2,  -4,  -7,  -6,   2,   0,   0,  -1,   4,   5,   4,   2,  //30
   -15,  -2,   1,   0,  -7,   1,   1,   0,  -1,   1,  -1,  -2,   1,  -1,  -1,  -1,  //31
    19,  19,  19,  18,  19,  19,  19,  19,  18,  18,  18,  18,  17,  17,  17,  17,  //32
     0,  -1,  -7, -12,   0,  -1,  -5, -13,   0,   0,  -4, -12,   1,   0,  -4, -10,  //33
   -12,  -9,  -5,  -2, -13, -10,  -5,  -1, -13,  -9,  -5,  -2, -11,  -8,  -4,  -1,  //34
   -13, -14, -14, -14, -14, -15, -16, -14, -14, -15, -16, -13, -14, -15, -15, -13,  //35
   -11, -12, -11, -11,  -8,  -9,  -9, -10,  -3,  -3,  -4,  -5,   1,   0,   0,  -1,  //36
    -4,  -5,  -4,  -3,   2,   2,   1,   1,   8,   7,   8,   7,  10,  10,  10,   9,  //37
    10,  11,  11,  10,  11,  10,  10,   7,   8,   6,   6,   3,   5,   3,   2,   0,  //38
     1,   0,   0,  -1,   1,   0,  -1,   1,   0,  -2,  -1,   6,   0,  -2,   1,  16,  //39
   -10, -13, -11,  -7,   0,   1,   0,  -1,   1,   1,   2,   2,   1,   0,   0,   0,  //40
    11,   8,   0,   0,  11,   8,   0,   0,  13,  10,   0,  -1,  11,   9,  -1,  -1,  //41
     1,  -2,  -5,  -6,   0,  -3,  -7,  -9,  -3,  -5, -10, -12,  -4,  -8, -12, -12,  //42
   -30, -29, -30, -30, -30, -30, -31, -30, -30, -30, -30, -31, -29, -30, -30, -30,  //43
     0,   0,   4,  16,   0,  -1,   1,  13,   0,  -1,  -1,   3,   1,   0,  -1,   1,  //44
   -22, -24, -25, -25, -23, -25, -25, -26, -23, -25, -25, -25, -22, -24, -24, -24,  //45
   -16, -17, -17, -16, -19, -19, -19, -18, -21, -21, -20, -20, -20, -21, -21, -20,  //46
     0,   1,  -1,  -2,   0,  -1,  -1,   0,   2,   5,  10,  12,   0,  -1,  -2,  -4,  //47
     2,   5,  11,  16,   1,   3,  11,  15,   0,   3,  10,  14,   0,   3,   9,  11,  //48
    13,  14,  10,   4,  14,  12,   8,   2,  13,  12,   7,   2,  13,  11,   7,   2,  //49
    30,  30,  30,  29,  30,  30,  30,  29,  30,  31,  30,  30,  31,  31,  31,  30,  //50
     7,   7,   2,  -7,   8,   5,   0, -10,   7,   4,   0,  -9,   7,   4,  -1, -10,  //51
    21,  23,  23,  22,  23,  24,  24,  24,  23,  25,  25,  25,  24,  25,  25,  25,  //52
     9,   0,   1,   2,  12,  -1,   0,   2,  12,  -3,   0,   2,  10,  -3,  -1,   2,  //53
    -8,  -4,   4,   7,  -9,  -6,   3,   8,  -8,  -6,   2,   6,  -8,  -6,   2,   5,  //54
    16,  -1,  -3,  -1,   1,  -3,  -1,   0,  -2,  -1,   0,   2,  -1,  -1,   0,   1,  //55
    -3,  -2,   7,  -5,  -3,  -3,   8,  -6,  -1,  -4,   9,  -5,  -1,  -4,   9,  -2,  //56
     8,   5,   2,   0,  13,  10,   7,   4,  15,  14,  11,   9,  16,  16,  15,  13,  //57
    -4,  -2,  -1,   0,  -8,  -6,  -5,  -2, -13, -12, -10,  -6, -15, -14, -14, -10,  //58
    -3,  11,  -4,  -1,  -3,   8,  -4,  -1,  -3,   7,  -4,  -1,   0,   4,  -3,   1,  //59
   -20, -15, -10,  -6, -20, -15, -11,  -6, -19, -15, -11,  -7, -18, -16, -11,  -6,  //60
    -1,  -3,  -2,  -2,   6,   7,  10,   9,   7,   8,   9,   7,  -2,  -2,  -3,  -3,  //61
     1,   4,   8,   3,   1,   6,   9,   1,   1,   8,  10,  -2,   3,   9,   9,  -3,  //62
     2,   4,   7,  11,   5,   7,  13,  15,   9,  13,  15,  16,  11,  15,  16,  15,  //63
     1,   2,   0,   0,  -5,  -1,  -1,  -1, -15,  -4,  -1,   1, -20,  -8,   0,   1,  //64
     4,   0,  -7,  -9,   1,  -4,  -8,  -8,  -3,  -8,  -7,  -1,  -7,  -7,  -1,   3,  //65
    -1,  -9, -12,  -7,   0,  -8, -13,  -9,   1,  -7, -12,  -8,   1,  -6, -10,  -8,  //66
    -8, -13, -16, -18,  -6, -12, -14, -16,  -4, -10, -14, -16,  -2,  -8, -13, -16,  //67
    -1,  -2,  -3,  -4,   2,   7,  11,  10,  -3,  -3,  -3,  -4,  -2,  -2,  -2,  -2,  //68
     7,   3,  -5, -11,   8,   7,   2,  -4,   7,   9,   7,   1,   4,   7,   8,   4,  //69
    10,  15,  16,  18,   8,  14,  16,  19,   5,  11,  15,  18,   4,   9,  14,  15,  //70
    -1,   0,   0,   1,   0,   0,   0,   0,  -2,  -2,  -1,  -1,  14,  16,  15,  13,  //71
    -5,  -6,  -7,  -8, -10, -12, -12, -13, -14, -15, -15, -17, -18, -19, -18, -19,  //72
    34,  36,  36,  36,  35,  37,  37,  37,  36,  37,  37,  37,  35,  38,  37,  36,  //73
   -11,  -8,  -3,   1, -10,  -8,  -1,   5,  -5,  -1,   6,   8,   1,   7,   6,   7,  //74
     0,   0,   2,   2,   0,   1,   2,   1,   1,   1,  -2,  -4,   0,  -6, -15, -17,  //75
   -20, -20, -20, -14, -17, -17, -16, -15, -10, -10, -10, -10,  -3,  -4,  -4,  -3,  //76
     0,   0,  -1,  13,   0,  -1,   1,  15,   0,  -1,  -2,  19,   0,  -1,  -1,  18,  //77
    -4,  -1,   2,  20,   0,  -2,  -1,  -5,   0,   0,  -1,  -2,   0,   0,   1,   0,  //78
     8,   8,   7,   6,   6,   6,   3,   2,  -6,  -8,  -8,  -3,  -8, -12, -10,  -5,  //79
    -1,  -1,   0,   0,  -3,  -2,  -1,   0,  14,  11,   2,  -2,  -4,  -4,  -1,   3,  //80
   -25, -25, -21, -13, -27, -24, -20, -11, -26, -23, -18,  -9, -25, -21, -15,  -8,  //81
     8,  14,  20,  17,   1,  -1,  -2,  -2,  -1,   0,  -1,   0,  -1,   0,   0,  -2,  //82
    -2,   0,   0,  -1,  -4,  -2,  -2,  -1,  -1,  -3,   0,  -2,  24,   4,  -1,   0,  //83
   -19,  -2,   3,  -2, -20,   1,   3,  -1, -21,   0,   3,  -2, -20,   0,   3,  -3,  //84
    -1,   1,  -8, -25,   0,   1,   0,  -2,   0,  -2,   1,   3,  -2,  -2,   0,   1,  //85
    -1,   0,  17,  -4,   1,  -1,   6,   1,   1,   2,  -3,   1,   1,   1,  -5,   1,  //86
    -1,   0,  -1,   0,   1,   1,   0,   0,   1,   3,   2,   1, -15, -18, -13,  -5,  //87
   -38, -37, -35, -36, -39, -38, -36, -37, -38, -38, -37, -36, -37, -37, -36, -33,  //88
    -9,  -9,   0,   5,  -6, -12,  -6,   5,   0, -11,  -9,   3,   2,  -5, -12,  -2,  //89
    21,   2,   0,   0,  24,   4,   0,   1,  26,   6,   0,   1,  25,   7,   0,   1,  //90
     1,  -2,  -1,   2,   2,  -1,   1,   1,   0,   5,  -2,   1,  -3,  16,  -7,   1,  //91
    28,   9,   0,  -1,  16,   3,   0,   1,   3,   0,   1,   2,   0,   0,   0,   0,  //92
    -4,  -2,  -2,   0,  18,  13,   1,  -2,   0,  -1,   0,   1,   0,   2,   0,   0,  //93
    18,  15,  11,   4,  20,  18,  14,   8,  22,  21,  17,  11,  23,  22,  19,  13,  //94
     3, -14,   2,   2,   4, -17,   2,   4,   3, -15,   2,   3,   1, -14,   2,   3,  //95
     1,   0,   0,  -1,   2,   1,   0,  -1, -14, -15, -13,  -7,   2,   2,   2,   2,  //96
    -3,  -4,  -2,   0,   1,  -4,  -4,  -3,  14,   0, -10,  -9,  12,   2,  -9,  -6,  //97
    28,  24,  19,  12,  29,  21,  15,   9,  25,  17,  11,   6,  22,  15,   9,   5,  //98
     7,   9,  10,  10,  13,  16,  16,  16,  18,  22,  22,  22,  22,  26,  26,  25,  //99
     0,  -1, -10, -18,  -1,  -3, -11, -21,  -2,  -4, -13, -24,  -3,  -7, -15, -24,  //100
   -22, -24, -25, -22, -20, -22, -23, -22, -13, -18, -21, -21,  -4, -11, -16, -17,  //101
     7,   3, -13,  -8,   7,   5, -13,  -8,   6,   5, -13,  -9,   6,   4, -11, -10,  //102
    21,  22,  20,  18,  14,  15,  16,  14,   2,   3,   5,   6,   1,   0,   0,   2,  //103
    -3, -12, -21, -24,  -5, -15, -23, -24,  -7, -17, -23, -23,  -9, -19, -23, -21,  //104
    30,  28,  24,  21,  32,  28,  23,  19,  32,  26,  19,  16,  31,  27,  20,  16,  //105
    15,  19,   7,  -3,  -6,  -7,  -1,   3,  -3,  -1,  -1,  -1,   0,   0,   1,   0,  //106
     1,  -2, -13,   5,   2,  -1, -18,   6,   0,  -1, -19,   5,   0,  -3, -18,   4,  //107
     2,   1,   1,   5,   0,   1,   4,  14,   1,   2,  13,  24,   1,   4,  18,  26,  //108
    -3,   9,   5, -12,  -5,   8,   4, -14,  -4,   6,   4, -11,  -5,   4,   3,  -8,  //109
     6,  16,  23,  27,   6,  18,  24,  28,   6,  20,  26,  28,   6,  19,  25,  27,  //110
     1,   2,   3,   2, -15, -18, -18, -16,   0,   1,   1,   0,   1,   1,   0,   0,  //111
     2,   0,  -3, -25,   3,   1,  -1, -25,   3,   1,  -2, -23,   2,   1,  -1, -24,  //112
   -18, -15,   1,   4, -19, -15,   1,   5, -20, -18,  -1,   5, -18, -17,  -1,   2,  //113
    -2,  -1,  -1,   3,  -2,  -1,  -1,  -3,  -1,   0,   0, -20,  -1,   1,   0, -26,  //114
   -32, -30, -27, -17, -31, -30, -29, -16, -31, -30, -27, -15, -30, -31, -28, -16,  //115
   -43, -46, -47, -44, -45, -47, -47, -45, -46, -47, -46, -45, -45, -46, -45, -44,  //116
     7,   2,  -3,  -1,   5,   9,   4,  -2,  -7,   1,  11,   6, -12,  -8,   6,  14,  //117
   -29, -21,  -5,   1, -23,  -9,  -2,   2,  -6,   0,   0,   2,   0,   0,   0,   0,  //118
     1,   1,   7,  33,   0,   1,   8,  33,   0,   0,   7,  33,   1,   1,   8,  32,  //119
     4,  -3,  -6,   0,   7,   4,  -6,  -2,  -3,   6,   4,  -5,  -8,   7,  15,  -5,  //120
    28,  20,   3,   1,  29,  22,   2,   0,  29,  25,   2,   1,  30,  25,   4,   1,  //121
   -17,  -1,  10,  11,  -7,   4,  10,   5,   4,  11,   7,   2,   9,   9,   3,   1,  //122
    30,  31,  30,  28,  27,  27,  27,  28,  20,  23,  25,  25,  11,  15,  17,  18,  //123
     0,   4,  20,  31,  -1,  -1,   9,  24,   0,  -1,   2,   9,   1,  -1,   0,   2,  //124
   -10, -12, -12,  -7, -10, -11, -11,  -1,  -9,  -9,  -1,  12,  -9,  -4,   8,  17,  //125
    13,  19,  23,  21,  15,  23,  21,  15,  20,  22,  18,   5,  16,  12,   5,  -4,  //126
     6,   7,   3,   4,   1,   1,  -2,  -7, -12, -17, -21, -21, -13, -22, -22, -19,  //127
    13,  15,  14,  11,  15,  16,  13,  11,   8,   6,   2,   0,  -9, -12, -13, -14,  //128
     2,   2,   0,  -5,   0,  -2,  -2,  -1,  -4,  -2,  -3,   6,  -1,  10,  24,  29,  //129
     1,  -7, -11, -13,  12,  -1, -10, -11,  17,   6,  -4,  -9,  16,  11,   1,  -5,  //130
    -6, -21, -14,  -2,  -5, -23, -15,  -3,  -6, -24, -17,  -2,  -5, -22, -19,  -5,  //131
   -28,  -7,  -4,  -3, -31,  -9,  -4,  -2, -32, -12,  -3,  -3, -33, -16,  -4,  -4,  //132
    18,  -6, -11,  -3,  20,  -7, -11,  -1,  20,  -5, -10,   0,  17,  -5, -11,  -2,  //133
    15,  15,   9, -12,  15,  15,   9, -13,  16,  16,   9, -13,  16,  14,   8, -13,  //134
    -6,  -4,  -4,   0,  -1,   1,   4,   9,   9,  16,  19,  18,  18,  22,  20,  17,  //135
     3,  22,   2,   0,   4,  24,   4,   2,   2,  25,   3,   1,   1,  20,   5,   1,  //136
    -8,  12,  15,   7, -14,  12,  12,   5, -14,   8,  12,   6, -13,   6,  13,   7,  //137
     7,   9,  13,   9, -10, -14, -15,  -7,  -8, -13, -14, -13,  -7,  -7,  -6,  -4,  //138
   -16, -19, -19, -17,  -6, -11, -11, -13,   8,   8,   5,   2,  12,  14,  11,  10,  //139
   -24, -31, -28, -24,  -3,  -3,  -6,  -2,  -2,  -2,  -1,  -1,  -2,  -2,  -3,  -3,  //140
   -36, -28,  -6,  -1, -37, -32,  -6,   0, -36, -30,  -7,   0, -35, -28,  -9,   0,  //141
   -22, -16, -11,  -6, -32, -28, -24, -20, -34, -33, -30, -28, -32, -34, -33, -32,  //142
     1,   9,  14,  17,  -7,   3,  12,  16, -17,  -8,   2,  14, -17, -15,  -5,   6,  //143
    -1,   2,   0,   0,   0,   0,   0,  -1,  22,  13,   4,  -1,  31,  25,  14,  -1,  //144
     5,   6,   6,   8,  21,  26,  26,  24,   0,  -1,   0,  -1,   0,  -2,  -2,   0,  //145
   -13,  -8,  -4,  21, -15,  -9,  -4,  22, -14,  -9,  -3,  23, -16, -10,  -5,  20,  //146
     0,   6,  31,  36,  -1,   6,  32,  38,   0,   4,  32,  39,   0,   5,  28,  38,  //147
    -9, -19, -23, -20,   1,  -8, -19, -24,   7,   3, -12, -25,   6,   5,  -7, -21,  //148
    -6, -27, -30, -31,  -6, -29, -31, -32,  -8, -30, -32, -32,  -9, -28, -31, -29,  //149
   -27, -30, -30, -27, -26, -30, -31, -28,  -7,  -6,  -6,  -7,  -2,  -1,  -3,  -2,  //150
    -8,  -3,   3,   6, -24, -19, -10,   2, -26, -25, -22, -12, -23, -29, -25, -21,  //151
     8,  18,  17,   6,   6,  20,  23,   5,   4,  19,  23,   1,   5,  20,  22,   3,  //152
    -2,   0,   0,   0,  -1,   1,   0,  -1,  -6,  -5,  -2,  -2, -16, -24, -28, -26,  //153
    42,  45,  44,  41,  45,  47,  47,  43,  44,  47,  47,  44,  43,  46,  45,  42,  //154
   -11, -11,  12,  17, -12,  -8,  16,  18, -12,   1,  17,  16,  -6,   6,  16,  17,  //155
     3,  -1, -26, -20,   2,   0, -28, -23,   3,  -1, -28, -24,   3,  -2, -26, -22,  //156
     0,   5,  26,  25,  -1,   4,  27,  24,  -1,   4,  28,  24,   0,   8,  28,  21,  //157
    18,  21,  20,  14,  13,  21,  23,  18,   4,  12,  23,  26,  -3,  -3,   8,  18,  //158
   -26, -40, -42, -39, -24, -39, -42, -41, -22, -39, -43, -42, -20, -37, -42, -40,  //159
     2,  -8, -25, -31,   1,   1, -13, -24,   1,   2,   1,   0,   0,  -2,  -2,  -1,  //160
    39,  41,  36,  20,  42,  42,  38,  21,  42,  41,  38,  22,  41,  41,  36,  24,  //161
    33,  26,   8,   0,  33,  16,   2,  -2,  26,   7,  -1,  -2,  14,   1,  -2,  -2,  //162
   -29, -33, -31, -18, -29, -34, -25,  -8, -28, -25,  -5,  -1, -21,  -7,   1,  -3,  //163
    32,  32,  29,  25,   3,   1,   1,   0,   1,   1,  -2,   0,   0,   0,  -1,   2,  //164
    30,  33,  25,   3,  30,  34,  30,   1,  30,  34,  30,   1,  27,  33,  29,   4,  //165
   -20, -11,  -2,   5, -19, -18, -10,  -6,  -7, -12, -20, -18,  -1,  -3, -11, -14,  //166
     1,  -2, -29, -37,   1,  -1, -23, -38,   0,  -2, -14, -32,   1,   0,  -4, -17,  //167
    19,  14,   0, -10,  15,   7,  -9, -18,  10,  -3, -15, -22,   2, -10, -18, -21,  //168
    -4, -14, -31, -36, -17, -31, -40, -39, -31, -39, -41, -38, -30, -37, -38, -35,  //169
    -3,   1,  28,   1,  -2,   1,  28,   0,  -4,   0,  23,   1,  -1,   1,  15,  -3,  //170
   -31, -36, -34, -30, -19, -34, -35, -35,  -2, -20, -34, -37,   2,  -5, -26, -35,  //171
    32,  32,  31,  32,  28,  32,  31,  31,   8,   7,   7,   9,   0,   1,   2,   2,  //172
    -2,  -1,  -4, -11,  -8, -16, -23, -28, -17, -26, -32, -35, -23, -33, -33, -34,  //173
    -7, -14, -37, -43,  -6, -13, -38, -42,  -6, -10, -36, -42,  -6, -14, -34, -40,  //174
    -3,  -2,  -8, -33,  -3,  -3, -15, -39,  -1,  -4, -24, -42,  -2,  -8, -29, -42,  //175
     2,  -3,  -7,  -8,  -8, -12, -11,  -7, -24, -25, -11,  -1, -27, -27, -11,   1,  //176
   -32, -33, -27,   3, -34, -33, -28,   3, -33, -32, -26,   3, -35, -33, -23,   4,  //177
    20,  22,  19,  22,  11,  15,  18,  22, -17, -18, -19, -12, -15, -19, -21, -17,  //178
   -35, -28,  -9,  -2, -36, -19,  -1,  -1, -34, -10,   2,  -1, -26,  -3,   4,   2,  //179
     2,  22,  21,   3,  13,  20,   6,  -2,  17,   6,  -2,  -1,  10,   2,  -3,  -3,  //180
    37,  39,  39,  36,  37,  39,  40,  29,  40,  38,  32,  16,  34,  21,   7,   1,  //181
   -16, -32, -38, -36,  -2, -17, -32, -33,  -2,  -5, -12, -15,  -2,  -2,  -1,  -5,  //182
    20,   6,  -2,  -3,  32,  20,   2,   0,  35,  32,  16,   1,  32,  35,  28,  10,  //183
     6,  30,  38,  37,   5,  34,  39,  36,   5,  32,  40,  35,   3,  29,  38,  32,  //184
    29,   9, -13, -21,  28,  13, -12, -22,  27,  14,  -9, -17,  26,  16,  -6, -14,  //185
   -34, -35, -35, -32, -34, -37, -37, -34, -28, -31, -28, -25,  -5,  -5,  -5,  -7,  //186
    10,  21, -12, -16,   5,  24, -10, -16,   6,  23,  -9, -16,   9,  23, -10, -18,  //187
     0,  -1,   0,   1,   1,   0,   1,   1,  17,  11,   5,   5,  38,  39,  38,  33,  //188
     3, -15, -23, -10, -15, -21,  -2,   6,  -4,   1,   5,   2,   4,   1,   0,  -2,  //189
    -9, -12, -11, -10,  -3,  -6,  -7,  -5,  11,  14,  19,  17,  -8, -14, -15, -17,  //190
     5,  22,  33,  31,  -1,   8,  28,  33,  -2,   0,  10,  26,  -1,   0,   1,   7,  //191
    36,  41,  39,  38,  31,  42,  43,  41,  22,  33,  38,  39,   7,  19,  31,  37,  //192
    39,  35,  10,  -3,  39,  37,  10,  -3,  40,  35,  10,  -4,  39,  33,  10,  -3,  //193
    21,  35,  36,  33,  10,  27,  35,  34,  -1,  11,  32,  36,   0,   2,  18,  34,  //194
    19,  13,   6,   3,  30,  29,  25,  16,  36,  35,  34,  24,  36,  37,  36,  33,  //195
     1,   7,  22,  30,  11,  25,  36,  38,  25,  36,  38,  38,  33,  40,  39,  38,  //196
   -35, -42, -42, -35, -39, -42, -41, -32, -42, -44, -35, -12, -42, -38, -18,  -1,  //197
   -19,  -3,   6,   5, -31,  -8,   3,   4, -38, -27,  -2,   4, -36, -37, -21,  -1,  //198
   -22, -20,   9,  22, -27, -21,   7,  23, -24, -21,   7,  23, -20, -19,  10,  22,  //199
   -17, -13, -12,  -9, -19, -17, -16, -15, -11, -13, -12, -10,  15,  21,  20,  19,  //200
     0,   1,  -1,   1,  23,  26,  27,  26,  23,  28,  29,  27,   0,  -1,  -2,  -4,  //201
    -7,  -8,  -8,  -8,  -7,  11,  15,   7,  -7,  19,  23,  18,  -6,  14,  22,  18,  //202
    14,  21, -20,  10,  13,  21, -22,  12,  16,  18, -22,  13,  15,  17, -21,  12,  //203
    -6,  -3,   3,  17,  -6,   2,  22,  32,   7,  27,  31,  32,  29,  37,  31,  26,  //204
    13, -17, -23, -24,  12, -16, -21, -23,  13, -16, -25, -21,   9, -15, -24, -22,  //205
   -45, -40, -26,  -3, -47, -46, -34,  -5, -46, -46, -40, -14, -47, -45, -39, -21,  //206
    -6,  -8,  -4,  -2,  -1,  -1,  -2,  -4,  25,  31,  36,  35,  21,  29,  32,  30,  //207
   -17, -20, -23, -24,  17,  17,  16,  12,  23,  22,  20,  17,  10,  11,   9,  10,  //208
     9, -10, -17, -21,  14,   2, -11, -18,  17,  16,  12,   4,  16,  18,  16,  17,  //209
   -16, -15, -19, -13, -23, -24, -25, -26, -25, -30, -35, -37,   5,   9,   4,  -3,  //210
     8,   7,   9,   8,  -8, -15, -10, -10,  18,  18,  18,  18,  20,  21,  20,  20,  //211
    14,   9, -12, -27,  -2, -11, -27, -29,  -8, -23, -24, -11,   0,   0,   5,  10,  //212
    14,  -1,  10,  19,  10, -13,   2,  26,   5, -21,   0,  24,   7, -16,  -3,  19,  //213
   -30,   5,  15,  30, -29,   5,  12,  31, -28,   0,  10,  28, -29,  -5,  14,  24,  //214
   -11,  30,  -5,   4, -13,  32, -14,  -5, -11,  34, -13,  -2, -13,  29,  -9,  -3,  //215
     0,  13, -34,  34,  -2,  14, -33,  33,  -2,  19, -35,  32,   1,  20, -35,  30,  //216
    43,  42,  36,  15,  40,  40,  23,   5,  23,  20,  10,   0,  -3,  -8,  -4,  -1,  //217
    35,  38,  37,  32,  -2,   9,  21,  22,  13,   5,  -3,  -8,   9,   1,  -5,  -8,  //218
     5,   6,   1,   0,  12,  20,  25,  26, -12, -14, -20, -17,  15,  18,  21,  17,  //219
    -3,  -1,   0,   2,  -5,   1,   3,   3, -43, -40, -29, -34, -41, -38, -39, -38,  //220
    -4, -37, -18,   8,  -2, -37, -38,   5,  -3, -25, -44, -13,  -4, -22, -42, -29,  //221
   -19, -22,  -5,   2, -28, -24,   5,  14, -25,  -4,  23,  39,  -7,  24,  40,  43,  //222
   -26, -30, -33, -31,  22,  26,  27,  25, -10, -10,  -2,   0,  -1,  -3,  -4,   4,  //223
    28,  36,  22,  11,   5,   6,  -4, -13, -29, -28, -28, -32,   1,   3,   9,  -1,  //224
     4,   5,   3,   0,  -7,  -3,   9,  18, -36, -16,  26,  36, -39,  -8,  28,  32,  //225
   -28, -29, -11,  18, -35, -29, -24, -21,   3,   0,  16,  28,   5,  -4,  13,  29,  //226
   -11,   9,  -2,  -3,  34, -30,   6, -11, -26,  15, -24, -11,  -4, -12,  -8,  -6,  //227
     6,   0,  -1,  -3,  20,   4,  -2,  -4,  42,  45,  52,  46,   6,   1,  -2,   5,  //228
   -26, -25, -36, -24,  -8,  37,  37,  31,  16,  14,  38,  33,  39,  27,  24,  33,  //229
    12,  28,  18,  17,   6,  28, -14,  -4,  -4,   5, -22, -33, -11,  29,  -7, -17,  //230
    -3, -27, -22, -10,  15,   0,   8,  16, -26, -33, -35, -24,  24,  24,  30,  21,  //231
     8,   8,   9,   5, -45, -57, -58, -55,  42,   6,   0,  -1, -44, -30,  -3,  18,  //232
   -12,  52,  50,  19, -10,  40,  51,  57, -13,   2,  52, -10,  -4,  -9, -24, -25,  //233
    24,  19,  -7,   2,  43, -15,  -4,  -8,  -4, -38,   3,  -7, -15, -39,  14,   6,  //234
   -28, -43, -13,   0, -12,  -4,  15, -10, -23, -40, -27, -24, -30, -34,  -8,  -6,  //235
    -4,   5,  11,  -9,  21,   4, -20, -21, -37, -47, -24,   8,  38,  40,  31, -28,  //236
    32,  50,   4,  49,  -6,   6, -30,  10,   7, -42, -11,   5,   6, -34,  -4,   4,  //237
   -19, -39, -18,  10, -19, -34,  28,  48,   5, -30,   9,  50,  15,  -3,  22,   1,  //238
    -3,  45,  55,  52,  -1,  32,  55,  58, -48, -44,  -9,   0, -53, -48, -23,  14,  //239
   -29,  12,  -7,   7,  23, -26,  36, -23,   2,   3,   8,  35,  -5,   2,   5,   3,  //240
    -2,   5,  -2,  -1,   2,  23, -18, -34,  22, -18, -37, -17, -27, -36, -17,  11,  //241
    14,  -2, -23,   5, -34,   7, -36, -12,  14,  16, -30,  43,   9,   7,  -8, -20,  //242
    -5, -10,   4, -28,  -7,  15, -36, -16,  32, -21, -13, -10,   9, -36, -27, -11,  //243
     2,  15,  17,   0,  17,  27,  23,  28,   3,   1, -40, -57, -15, -52, -51, -52,  //244
     7,   1,  34,  39, -14,  40,  48,  33, -15,  30,  31, -13,   0, -12,   9,  24,  //245
    -1,  11,  -4,  32,  28, -25,  35,  46, -18,  44,  18,   7,  44,   7,   4,  28,  //246
    40,   5,  50, -28,  -6,   9,  24,  26,  25, -22,   5,  -4, -15, -13,  28,  11,  //247
    -2,   6,  -5,  45,   2,   3,  42,  22, -20, -19,  20,  14,  35,  42,  42,  10,  //248
   -19,  29,  26, -31,  30,  -9,  -6,  -1,  30, -10, -43,   6,  16, -12, -37,  -7,  //249
   -46, -47, -45, -43, -44, -46, -45, -46, -17,  15,  33,  29,  40,  32,  30,  32,  //250
    29,  34,  30,  26,  26,  20,  13, -12, -36, -29, -16,   7,  39,  41,  32,  22,  //251
    30,  18, -14,  11, -11, -11,  12, -14,   7,  23, -36,  10,   0, -46, -16,   5,  //252
     0, -21,   0,  18,  39,  18, -17,  11, -26,  -6, -44,   5, -17,  -2, -28, -51,  //253
   -18,  -9,  -3,  16,  -7,  18,  18,  18,  44,  45,  17,   5,  46,  23, -22, -40,  //254
    -7,  29,  20, -34,  -6,  35,  20, -49,  14,  41,  20, -40, -18,  -1,  -9, -32  //255
};
												

												
