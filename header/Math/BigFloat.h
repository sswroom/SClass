#ifndef _SM_MATH_BIGFLOAT
#define _SM_MATH_BIGFLOAT

namespace Math
{
	class BigFloat
	{
	private:
		UInt8 *valArr;
		UInt8 *tmpArr;
		Int32 valIndex;
		Int32 tmpIndex;
		Int32 valSize;
		Bool isNeg;

		void RemZero();
		void PrepareTmpBuff(Int32 tmpSize);
		void PrepareTmpBuff(const BigFloat *val);
		void PrepareSum();
		void DoSum();
		Bool DoSubtract();
	public:
		BigFloat(const BigFloat *val);
		BigFloat(Int32 valSize, const WChar *val);
		BigFloat(Int32 valSize, Double val);
		BigFloat(Int32 valSize);
		~BigFloat();

		Int32 operator =(Int32 val);
		Double operator =(Double val);
		BigFloat *operator =(const WChar *val);
		BigFloat *operator =(const BigFloat *val);
		BigFloat *operator =(const BigFloat &val);
		BigFloat *operator +=(const BigFloat *val);
		BigFloat *operator +=(const BigFloat &val);
		BigFloat *operator -=(const BigFloat *val);
		BigFloat *operator -=(const BigFloat &val);
		BigFloat *operator *=(const BigFloat *val);
		BigFloat *operator *=(const BigFloat &val);
		BigFloat *operator *=(Int32 val);
		BigFloat *operator /=(const BigFloat *val);
		BigFloat *operator /=(const BigFloat &val);
		BigFloat *operator /=(Int32 val);
		BigFloat *operator /=(UInt32 val);
		BigFloat *ToNeg();

		Bool IsZero();
		Bool operator >(const BigFloat *val);
		Bool operator >=(const BigFloat *val);
		Bool operator <(const BigFloat *val);
		Bool operator <=(const BigFloat *val);
		Bool operator ==(const BigFloat *val);
		Bool operator !=(const BigFloat *val);

		BigFloat *Factorial(Int32 val);
		BigFloat *SetPI();
		BigFloat *SetE(BigFloat *val);
		BigFloat *SetLn(BigFloat *val);/////////////////////////////////////////////
		BigFloat *SetSin(const BigFloat *val);
		BigFloat *SetSin(const BigFloat &val);
		BigFloat *SetCos(const BigFloat *val);
		BigFloat *SetCos(const BigFloat &val);
		BigFloat *SetTan(const BigFloat *val);
		BigFloat *SetTan(const BigFloat &val);

		BigFloat *CalcPow(const BigFloat *val); ///////////////////////////////////////////
		BigFloat *ToSqrt(); //////////////////////////////////

		Int32 GetSize();
		WChar *ToString(WChar *buff);
	};
};
#endif
