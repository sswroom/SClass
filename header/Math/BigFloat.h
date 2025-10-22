#ifndef _SM_MATH_BIGFLOAT
#define _SM_MATH_BIGFLOAT
#include "Text/CString.h"

namespace Math
{
	class BigFloat
	{
	private:
		UnsafeArray<UInt8> valArr;
		UnsafeArray<UInt8> tmpArr;
		Int32 valIndex;
		Int32 tmpIndex;
		UOSInt valSize;
		Bool isNeg;

		void RemZero();
		void PrepareTmpBuff(Int32 tmpSize);
		void PrepareTmpBuff(NN<const BigFloat> val);
		void PrepareSum();
		void DoSum();
		Bool DoSubtract();
	public:
		BigFloat(NN<const BigFloat> val);
		BigFloat(UOSInt valSize, Text::CStringNN val);
		BigFloat(UOSInt valSize, Double val);
		BigFloat(UOSInt valSize);
		~BigFloat();

		Int32 operator =(Int32 val);
		Double operator =(Double val);
		NN<BigFloat> operator =(Text::CStringNN val);
		NN<BigFloat> operator =(NN<const BigFloat> val);
		NN<BigFloat> operator +=(NN<const BigFloat> val);
		NN<BigFloat> operator -=(NN<const BigFloat> val);
		NN<BigFloat> operator *=(NN<const BigFloat> val);
		NN<BigFloat> operator *=(Int32 val);
		NN<BigFloat> operator /=(NN<const BigFloat> val);
		NN<BigFloat> operator /=(Int32 val);
		NN<BigFloat> operator /=(UInt32 val);
		NN<BigFloat> ToNeg();

		Bool IsZero();
		Bool operator >(NN<const BigFloat> val);
		Bool operator >=(NN<const BigFloat> val);
		Bool operator <(NN<const BigFloat> val);
		Bool operator <=(NN<const BigFloat> val);
		Bool operator ==(NN<const BigFloat> val);
		Bool operator !=(NN<const BigFloat> val);

		NN<BigFloat> Factorial(Int32 val);
		NN<BigFloat> SetPI();
		NN<BigFloat> SetE(NN<BigFloat> val);
		NN<BigFloat> SetLn(NN<BigFloat> val);/////////////////////////////////////////////
		NN<BigFloat> SetSin(NN<const BigFloat> val);
		NN<BigFloat> SetCos(NN<const BigFloat> val);
		NN<BigFloat> SetTan(NN<const BigFloat> val);

		NN<BigFloat> CalcPow(NN<const BigFloat> val); ///////////////////////////////////////////
		NN<BigFloat> ToSqrt(); //////////////////////////////////

		UOSInt GetSize();
		UnsafeArray<UTF8Char> ToString(UnsafeArray<UTF8Char> buff);
	};
}
#endif
