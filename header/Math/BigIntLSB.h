#ifndef _SM_MATH_BIGINTLSB
#define _SM_MATH_BIGINTLSB
#include "Data/Random.h"
#include "Text/StringBuilderUTF8.h"

namespace Math
{
	class BigIntLSB
	{
	private:
		UnsafeArray<UIntOS> valArr;
		UnsafeArray<UIntOS> tmpArr;
		UIntOS valCnt;

	public:
		BigIntLSB(UIntOS valSize);
		BigIntLSB(UIntOS valSize, Text::CStringNN val);
		BigIntLSB(UIntOS valSize, UnsafeArray<const UInt8> val);
		~BigIntLSB();

		Bool IsNeg() const;
		Bool IsOdd() const;
		Bool IsEven() const;

		void ByteSwap();
		void SetRandom(NN<Data::Random> rnd);
		void FromBytesMSB(UnsafeArray<const UInt8> valBuff, UIntOS buffLen);
		UIntOS GetOccupiedSize() const;
		UIntOS GetStoreSize() const;
		UIntOS GetBytesMSB(UnsafeArray<UInt8> byteBuff, Bool occupiedOnly) const;

		Bool EqualsToUI32(UInt32 val);
		Bool EqualsToI32(Int32 val);
		void AssignI32(Int32 val);
		void AssignU32(UInt32 val);
		void AssignI64(Int64 val);
		void AssignU64(UInt64 val);
		void AssignStr(UnsafeArray<const UTF8Char> val);
		void AssignBI(NN<const BigIntLSB> val);
		void Neg();
		void AndBI(NN<const BigIntLSB> val);
		void OrBI(NN<const BigIntLSB> val);
		void XorBI(NN<const BigIntLSB> val);
		Bool SetFactorial(UIntOS val); //return true if overflow
		UIntOS MultiplyBy(UIntOS val); //return overflow value
		UIntOS DivideBy(UIntOS val); //return remainder

		Int32 operator =(Int32 val);
		UInt32 operator =(UInt32 val);
		Int64 operator =(Int64 val);
		UInt64 operator =(UInt64 val);
		NN<BigIntLSB> operator =(Text::CStringNN val);
		NN<BigIntLSB> operator =(NN<const BigIntLSB> val);
		NN<BigIntLSB> operator +=(NN<BigIntLSB> val);
		NN<BigIntLSB> operator ^=(NN<const BigIntLSB> val);
		NN<BigIntLSB> operator &=(NN<const BigIntLSB> val);
		NN<BigIntLSB> operator |=(NN<const BigIntLSB> val);
		NN<BigIntLSB> operator *=(UIntOS val);
		NN<BigIntLSB> operator /=(UIntOS val);

		Bool operator ==(NN<const BigIntLSB> val);
		Bool operator !=(NN<const BigIntLSB> val);

		UnsafeArray<UTF8Char> ToString(UnsafeArray<UTF8Char> buff) const;
		UnsafeArray<UTF8Char> ToHex(UnsafeArray<UTF8Char> buff);
		UnsafeArray<UTF8Char> ToByteStr(UnsafeArray<UTF8Char> buff);
		void ToString(NN<Text::StringBuilderUTF8> sb) const;
	};
}
#endif
