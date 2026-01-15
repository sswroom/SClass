#ifndef _SM_MATH_BIGFLOAT
#define _SM_MATH_BIGFLOAT
#include "Text/StringBuilderUTF8.h"

namespace Math
{
	class BigFloat
	{
	private:
		UnsafeArray<UIntOS> valArr;
		UnsafeArray<UIntOS> tmpArr;
		Int32 valIndex;
		Int32 tmpIndex;
		UIntOS valCnt;
		Bool neg;

		void RemZero();
		void PrepareTmpBuff(UIntOS tmpCnt);
		void PrepareTmpBuff(NN<const BigFloat> val);
		void PrepareSum();
		void DoSum();
		Bool DoSubtract();
	public:
		BigFloat(NN<const BigFloat> val);
		BigFloat(UIntOS valSize, Text::CStringNN val);
		BigFloat(UIntOS valSize, IntOS val);
		BigFloat(UIntOS valSize);
		~BigFloat();

		IntOS operator =(IntOS val);
		//Double operator =(Double val);
		NN<BigFloat> operator =(Text::CStringNN val);
		NN<BigFloat> operator =(NN<const BigFloat> val);
		NN<BigFloat> operator +=(NN<const BigFloat> val);
		NN<BigFloat> operator -=(NN<const BigFloat> val);
		NN<BigFloat> operator *=(NN<const BigFloat> val);
		NN<BigFloat> operator *=(IntOS val);
		NN<BigFloat> operator /=(NN<const BigFloat> val); ////////////////////////////////////
		NN<BigFloat> operator /=(UIntOS val); ////////////////////////////////////
		NN<BigFloat> ToNeg();

		Bool IsZero();
		Bool operator >(NN<const BigFloat> val);
		Bool operator >=(NN<const BigFloat> val);
		Bool operator <(NN<const BigFloat> val);
		Bool operator <=(NN<const BigFloat> val);
		Bool operator ==(NN<const BigFloat> val);
		Bool operator !=(NN<const BigFloat> val);

		NN<BigFloat> Factorial(UIntOS val);
		NN<BigFloat> SetPI();
		NN<BigFloat> SetE(NN<BigFloat> val);
		NN<BigFloat> SetLn(NN<BigFloat> val);/////////////////////////////////////////////
		NN<BigFloat> SetSin(NN<const BigFloat> val);
		NN<BigFloat> SetCos(NN<const BigFloat> val);
		NN<BigFloat> SetTan(NN<const BigFloat> val);

		NN<BigFloat> CalcPow(NN<const BigFloat> val); ///////////////////////////////////////////
		NN<BigFloat> ToSqrt(); //////////////////////////////////

		UIntOS GetSize();
		UnsafeArray<UTF8Char> ToString(UnsafeArray<UTF8Char> buff);
		void ToString(NN<Text::StringBuilderUTF8> sb);
	};
}
#endif
