/** @file

MODULE				: RunBeforeH264VlcDecoder

TAG						: RBH264VD

FILE NAME			: RunBeforeH264VlcDecoder.h

DESCRIPTION		: A run before Vlc decoder implementation as defined in
								H.264 Recommendation (03/2005) Table 9.10 page 206. The 
								zero coeffs left defines which vlc array to use. This 
								implementation is implemented with an IVlcEncoder 
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

===========================================================================
*/
#ifndef _RUNBEFOREH264VLCDECODER_H
#define _RUNBEFOREH264VLCDECODER_H

#pragma once

#include "IVlcDecoder.h"

/*
---------------------------------------------------------------------------
	Class definition.
---------------------------------------------------------------------------
*/
class RunBeforeH264VlcDecoder : public IVlcDecoder
{
public:
	RunBeforeH264VlcDecoder();
	virtual ~RunBeforeH264VlcDecoder();

public:
	/// Interface implementation.
	int GetNumDecodedBits(void)	{ return(_numCodeBits); }
	int Marker(void)						{ return(0); }	///< No markers for this decoder.
	/// A single symbol has no meaning for combined multi symbol encoding.
	virtual int Decode(IBitStreamReader* bsr) { _numCodeBits = 0; return(0); } 

	/// Optional interface implementation.
	/// The 2 symbols represent run before (output) and zeros left (input), respectively.
	virtual int	Decode2(IBitStreamReader* bsr, int* symbol1, int* symbol2);

protected:
	int _numCodeBits;	///< Number of coded bits for this symbol.

};// end class RunBeforeH264VlcDecoder.

#endif	// _RUNBEFOREH264VLCDECODER_H
