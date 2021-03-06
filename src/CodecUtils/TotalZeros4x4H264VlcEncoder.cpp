/** @file

MODULE				: TotalZeros4x4H264VlcEncoder

TAG						: TZ4H264VE

FILE NAME			: TotalZeros4x4H264VlcEncoder.cpp

DESCRIPTION		: A total zeros Vlc encoder implementation as defined in
								H.264 Recommendation (03/2005) Table 9.7 page 204. The 
								total number of coeffs defines which vlc array to use. 
								This implementation is implemented with an IVlcEncoder 
								Interface.

COPYRIGHT			: (c)CSIR 2007-2010 all rights resevered

LICENSE				: Software License Agreement (BSD License)

RESTRICTIONS	: Redistribution and use in source and binary forms, with or without 
								modification, are permitted provided that the following conditions 
								are met:

								* Redistributions of source code must retain the above copyright notice, 
								this list of conditions and the following disclaimer.
								* Redistributions in binary form must reproduce the above copyright notice, 
								this list of conditions and the following disclaimer in the documentation 
								and/or other materials provided with the distribution.
								* Neither the name of the CSIR nor the names of its contributors may be used 
								to endorse or promote products derived from this software without specific 
								prior written permission.

								THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
								"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
								LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
								A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
								CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
								EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
								PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
								PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
								LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
								NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
								SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=====================================================================================
*/
#ifdef _WINDOWS
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#else
#include <stdio.h>
#endif

#include "TotalZeros4x4H264VlcEncoder.h"

/*
---------------------------------------------------------------------------
	Construction and destruction.
---------------------------------------------------------------------------
*/
TotalZeros4x4H264VlcEncoder::TotalZeros4x4H264VlcEncoder(void)
{
	_numCodeBits	= 0;
	_bitCode			= 0;
}//end constructor.

TotalZeros4x4H264VlcEncoder::~TotalZeros4x4H264VlcEncoder(void)
{
}//end destructor.

/*
---------------------------------------------------------------------------
	Interface Methods.
---------------------------------------------------------------------------
*/
/** Encode total zeros - total coeffs symbols.
The 2 param symbols represent total zeros and total coeffs, respectively, and 
are encoded with the number of bits returned. The _numCodeBits and _bitCode 
members are set accordingly.
@param symbol1	:	Total zeros in this block 
@param symbol2	: Total coeffs for this block
@return					: Num of bits in the codeword.
*/
int	TotalZeros4x4H264VlcEncoder::Encode2(int symbol1, int symbol2)
{
	int totalZeros	= symbol1;
	int totalCoeffs	= symbol2;

	_bitCode			= table[totalZeros][totalCoeffs].codeWord;
	_numCodeBits	= table[totalZeros][totalCoeffs].numBits;

	return(_numCodeBits);
}// end Encode2.


/*
---------------------------------------------------------------------------
	Table Constants.
---------------------------------------------------------------------------
*/
const TZ4H264VE_VlcCodeType TotalZeros4x4H264VlcEncoder::table[16][16] = 
{///<	0				 1				2				 3				4				 5				6				 7				8				 9			 10				11			 12				13			 14				15	= totalCoeffs.
	{ { 0, 0}, { 1, 1}, { 3, 7}, { 4, 5}, { 5, 3}, { 4, 5}, { 6, 1}, { 6, 1}, { 6, 1}, { 6, 1}, { 5, 1}, { 4, 0}, { 4, 0}, { 3, 0}, { 2, 0}, { 1, 0} },	///< 0 = totalZeros.
	{ { 0, 0}, { 3, 3}, { 3, 6}, { 3, 7}, { 3, 7}, { 4, 4}, { 5, 1}, { 5, 1}, { 4, 1}, { 6, 0}, { 5, 0}, { 4, 1}, { 4, 1}, { 3, 1}, { 2, 1}, { 1, 1} },	///< 1
	{ { 0, 0}, { 3, 2}, { 3, 5}, { 3, 6}, { 4, 5}, { 4, 3}, { 3, 7}, { 3, 5}, { 5, 1}, { 4, 1}, { 3, 1}, { 3, 1}, { 2, 1}, { 1, 1}, { 1, 1}, { 0, 0} },	///< 2
	{ { 0, 0}, { 4, 3}, { 3, 4}, { 3, 5}, { 4, 4}, { 3, 7}, { 3, 6}, { 3, 4}, { 3, 3}, { 2, 3}, { 2, 3}, { 3, 2}, { 1, 1}, { 2, 1}, { 0, 0}, { 0, 0} },	///< 3
	{ { 0, 0}, { 4, 2}, { 3, 3}, { 4, 4}, { 3, 6}, { 3, 6}, { 3, 5}, { 3, 3}, { 2, 3}, { 2, 2}, { 2, 2}, { 1, 1}, { 3, 1}, { 0, 0}, { 0, 0}, { 0, 0} },	///< 4
	{ { 0, 0}, { 5, 3}, { 4, 5}, { 4, 3}, { 3, 5}, { 3, 5}, { 3, 4}, { 2, 3}, { 2, 2}, { 3, 1}, { 2, 1}, { 3, 3}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0} },	///< 5
	{ { 0, 0}, { 5, 2}, { 4, 4}, { 3, 4}, { 3, 4}, { 3, 4}, { 3, 3}, { 3, 2}, { 3, 2}, { 2, 1}, { 4, 1}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0} },	///< 6
	{ { 0, 0}, { 6, 3}, { 4, 3}, { 3, 3}, { 4, 3}, { 3, 3}, { 3, 2}, { 4, 1}, { 3, 1}, { 5, 1}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0} },	///< 7
	{ { 0, 0}, { 6, 2}, { 4, 2}, { 4, 2}, { 3, 3}, { 4, 2}, { 4, 1}, { 3, 1}, { 6, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0} },	///< 8
	{ { 0, 0}, { 7, 3}, { 5, 3}, { 5, 3}, { 4, 2}, { 5, 1}, { 3, 1}, { 6, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0} },	///< 9
	{ { 0, 0}, { 7, 2}, { 5, 2}, { 5, 2}, { 5, 2}, { 4, 1}, { 6, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0} },	///< 10
	{ { 0, 0}, { 8, 3}, { 6, 3}, { 6, 1}, { 5, 1}, { 5, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0} },	///< 11
	{ { 0, 0}, { 8, 2}, { 6, 2}, { 5, 1}, { 5, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0} },	///< 12
	{ { 0, 0}, { 9, 3}, { 6, 1}, { 6, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0} },	///< 13
	{ { 0, 0}, { 9, 2}, { 6, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0} },	///< 14
	{ { 0, 0}, { 9, 1}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0} }	///< 15
};


