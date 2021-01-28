#ifndef _SM_MATH_BIGINT
#define _SM_MATH_BIGINT
#include "Data/Random.h"

namespace Math
{
	class BigInt
	{
	private:
		UInt8 *valArr;
		UInt8 *tmpArr;
		Int32 valSize;

	public:
		BigInt(Int32 valSize);
		BigInt(Int32 valSize, const WChar *val);
		BigInt(Int32 valSize, UInt8 *val);
		~BigInt();

		Bool IsNeg() const;
		Bool IsOdd() const;
		Bool IsEven() const;

		BigInt *ByteSwap();
		void SetRandom(Data::Random *rnd);

		Bool EqualsToUI32(UInt32 val);
		Bool EqualsToI32(Int32 val);
		void AssignI32(Int32 val);
		void AssignStr(const WChar *val);
		void AssignBI(const BigInt *val);
		void Neg();
		void AndBI(const BigInt *val);
		void OrBI(const BigInt *val);
		void XorBI(const BigInt *val);
		Bool SetFactorial(UInt32 val); //return true if overflow
		UInt32 MultiplyBy(UInt32 val); //return overflow value
		UInt32 DivideBy(UInt32 val); //return remainder

		Int32 operator =(Int32 val);
		BigInt *operator =(const WChar *val);
		BigInt *operator =(const BigInt *val);
		BigInt *operator +=(BigInt *val);
		BigInt *operator ^=(const BigInt *val);
		BigInt *operator &=(const BigInt *val);
		BigInt *operator |=(const BigInt *val);
		BigInt *operator *=(UInt32 val);
		BigInt *operator /=(UInt32 val);


		WChar *ToString(WChar *buff);
		WChar *ToHex(WChar *buff);
		WChar *ToByteStr(WChar *buff);
	};
};
#endif
