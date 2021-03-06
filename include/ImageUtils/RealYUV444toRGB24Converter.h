/** @file

MODULE				: RealYUV420toRGB24Converter

FILE NAME			: RealYUV420toRGB24Converter.h

DESCRIPTION			: Floating point implementation of YUV420 (8 bpp) to RGB 24 bit 
colour convertion derived from the YUV420toRGBConverter base 
class.
					  
					  
LICENSE: Software License Agreement (BSD License)

Copyright (c) 2008 - 2012, CSIR
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* Neither the name of the CSIR nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

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
#ifndef _REALYUV444TORGB24CONVERTER_H
#define _REALYUV444TORGB24CONVERTER_H
#include "YUV444toRGBConverter.h"

/**
* \ingroup ImageLib
* Floating point implementation of YUV420 (8 bpp) to RGB 24 bit 
* colour convertion derived from the YUV420toRGBConverter base 
* class.
*/

class RealYUV444toRGB24Converter: public YUV444toRGBConverter
{
public:
  // Construction and destruction.
  RealYUV444toRGB24Converter(void) { }
  RealYUV444toRGB24Converter(int width, int height): YUV444toRGBConverter(width,height) { }
  virtual ~RealYUV444toRGB24Converter(void) {}

  // Interface.
  virtual void Convert(void* pInputBuffer, void* pRgb);



};//end RealYUV444toRGB24Converter.


#endif	// _REALYUV444TORGB24CONVERTER_H
