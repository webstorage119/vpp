/** @file

MODULE				: IForwardTransform

TAG						: IFT

FILE NAME			: IForwardTransform.h

DESCRIPTION		: An interface to forward transform implementations. The 
								interface has 2 options as either in-place or from an 
								input to an output parameter argument. Further, they 
								may include or exclude the quantisation and scaling. 
								The implementations must define the mem type (e.g. short, 
								int) and whether it is a 2-D on a block or 1-D on an 
								array.

COPYRIGHT			:	(c)CSIR 2007-2014 all rights resevered

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
#ifndef _IFORWARDTRANSFORM_H
#define _IFORWARDTRANSFORM_H

/*
---------------------------------------------------------------------------
	Interface definition.
---------------------------------------------------------------------------
*/
class IForwardTransform
{
public:
	virtual ~IForwardTransform() {}
	
	/** In-place forward transform.
	The transform is performed on the input and replaces it with the coeffs.
	@param ptr	: Data to transform.
	@return			:	none.
	*/
	virtual void Transform(void* ptr) = 0;

	/** Transfer forward transform.
	The transform is performed on the input and the coeffs are written to 
	the output.
	@param pIn		: Input data.
	@param pCoeff	: Output coeffs.
	@return				:	none.
	*/
	virtual void Transform(void* pIn, void* pCoeff) = 0;

	/** Set scaling array.
	Each coefficient may be scaled after transforming and therefore 
	requires setting up.
	@param	pScale:	Scale factor array.
	@return				:	none.
	*/
	virtual void	SetScale(void* pScale) = 0;
	virtual void* GetScale(void) = 0;

	/** Set the mode of operation.
	Allow an implementation to define modes of operation or
	interpretation of the quantisation process.
	@param mode	: Mode to set.
	@return			: none.
	*/
	virtual void SetMode(int mode) = 0;
	virtual int  GetMode(void) = 0;

	/** Set and get parameters for the implementation.
	Implementations require some flexibility in the interface	to provide extra
	parameters that are internal. Most implementations will have empty methods.
	@param paramID	: Parameter to set/get.
	@param paramVal	: Parameter value.
	@return					: None (Set) or the param value (Get).
	*/
	virtual void	SetParameter(int paramID, int paramVal) = 0;
	virtual int		GetParameter(int paramID) = 0;

  /** Quantise a single value in a block.
  Implement quantising a value in a specified position of the coeffs at a
  specified QP value.
  @param val  : Value to quantise.
  @param pos  : Postion of value in 1-D array of 2-D data.
  @param qp   : Quant param to use.
  @return     : Result of quantisation.
  */
  virtual int QuantiseValue(short val, int pos, int qp) = 0;

	/// Global constants for all implementations.
public:

	/// Parameter IDs.
	static const int QUANT_ID						= 0;	///< Sets/Gets quantisation parameter.
	static const int INTRA_FLAG_ID			= 1;	///< Sets/Gets intra flag.

	/// Modes.
	static const int TransformAndQuant	= 0;
	static const int TransformOnly			= 1;
	static const int QuantOnly					= 2;

};//end IForwardTransform.


#endif	// _IFORWARDTRANSFORM_H
