#ifndef _SM_MATH_BIGINTLSB
#define _SM_MATH_BIGINTLSB
#include "Data/Random.h"
#include "Text/StringBuilderUTF8.h"

namespace Math
{
	class BigIntLSB
	{
	private:
		UnsafeArray<UOSInt> valArr;
		UnsafeArray<UOSInt> tmpArr;
		UOSInt valSize;

	public:
		BigIntLSB(UOSInt valSize);
		BigIntLSB(UOSInt valSize, Text::CStringNN val);
		BigIntLSB(UOSInt valSize, UnsafeArray<const UInt8> val);
		~BigIntLSB();

		Bool IsNeg() const;
		Bool IsOdd() const;
		Bool IsEven() const;

		void ByteSwap();
		void SetRandom(NN<Data::Random> rnd);
		void FromBytesMSB(UnsafeArray<const UInt8> valBuff, UOSInt buffLen);
		UOSInt GetOccupiedSize() const;
		UOSInt GetStoreSize() const;
		UOSInt GetBytesMSB(UnsafeArray<UInt8> byteBuff, Bool occupiedOnly) const;

		Bool EqualsToUI32(UInt32 val);
		Bool EqualsToI32(Int32 val);
		void AssignI32(Int32 val);
		void AssignStr(UnsafeArray<const UTF8Char> val);
		void AssignBI(NN<const BigIntLSB> val);
		void Neg();
		void AndBI(NN<const BigIntLSB> val);
		void OrBI(NN<const BigIntLSB> val);
		void XorBI(NN<const BigIntLSB> val);
		Bool SetFactorial(UInt32 val); //return true if overflow
		UInt32 MultiplyBy(UInt32 val); //return overflow value
		UInt32 DivideBy(UInt32 val); //return remainder

		Int32 operator =(Int32 val);
		NN<BigIntLSB> operator =(Text::CStringNN val);
		NN<BigIntLSB> operator =(NN<const BigIntLSB> val);
		NN<BigIntLSB> operator +=(NN<BigIntLSB> val);
		NN<BigIntLSB> operator ^=(NN<const BigIntLSB> val);
		NN<BigIntLSB> operator &=(NN<const BigIntLSB> val);
		NN<BigIntLSB> operator |=(NN<const BigIntLSB> val);
		NN<BigIntLSB> operator *=(UInt32 val);
		NN<BigIntLSB> operator /=(UInt32 val);

		UnsafeArray<UTF8Char> ToString(UnsafeArray<UTF8Char> buff) const;
		UnsafeArray<UTF8Char> ToHex(UnsafeArray<UTF8Char> buff);
		UnsafeArray<UTF8Char> ToByteStr(UnsafeArray<UTF8Char> buff);
		void ToString(NN<Text::StringBuilderUTF8> sb) const;
	};
}
#endif
