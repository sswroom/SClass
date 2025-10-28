#ifndef _SM_MATH_BIGFLOAT
#define _SM_MATH_BIGFLOAT
#include "Text/StringBuilderUTF8.h"

namespace Math
{
	class BigFloat
	{
	private:
		UnsafeArray<UOSInt> valArr;
		UnsafeArray<UOSInt> tmpArr;
		Int32 valIndex;
		Int32 tmpIndex;
		UOSInt valCnt;
		Bool neg;

		void RemZero();
		void PrepareTmpBuff(UOSInt tmpCnt);
		void PrepareTmpBuff(NN<const BigFloat> val);
		void PrepareSum();
		void DoSum();
		Bool DoSubtract();
	public:
		BigFloat(NN<const BigFloat> val);
		BigFloat(UOSInt valSize, Text::CStringNN val);
		BigFloat(UOSInt valSize, OSInt val);
		BigFloat(UOSInt valSize);
		~BigFloat();

		OSInt operator =(OSInt val);
		//Double operator =(Double val);
		NN<BigFloat> operator =(Text::CStringNN val);
		NN<BigFloat> operator =(NN<const BigFloat> val);
		NN<BigFloat> operator +=(NN<const BigFloat> val);
		NN<BigFloat> operator -=(NN<const BigFloat> val);
		NN<BigFloat> operator *=(NN<const BigFloat> val);
		NN<BigFloat> operator *=(OSInt val);
		NN<BigFloat> operator /=(NN<const BigFloat> val); ////////////////////////////////////
		NN<BigFloat> operator /=(UOSInt val); ////////////////////////////////////
		NN<BigFloat> ToNeg();

		Bool IsZero();
		Bool operator >(NN<const BigFloat> val);
		Bool operator >=(NN<const BigFloat> val);
		Bool operator <(NN<const BigFloat> val);
		Bool operator <=(NN<const BigFloat> val);
		Bool operator ==(NN<const BigFloat> val);
		Bool operator !=(NN<const BigFloat> val);

		NN<BigFloat> Factorial(UOSInt val);
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
		void ToString(NN<Text::StringBuilderUTF8> sb);
	};
}
#endif
