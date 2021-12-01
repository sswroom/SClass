#ifndef _SM_MATH_MATH
#define _SM_MATH_MATH
#if defined(_MSC_VER) && _MSC_VER == 1500
#pragma warning (disable: 4985)
#endif

#include <math.h>

namespace Math
{
	extern const Double PI;
	Double Math_Log10(Double val);
	Double Math_Ln(Double val);
	Double Math_Exp(Double val);
	Double Math_Pow(Double x, Double y);

#if defined(FPUBUG)
	FORCEINLINE Double Sin(Double val)
	{
		return sin(val);
	}

	FORCEINLINE Double Cos(Double val)
	{
		return cos(val);
	}

	FORCEINLINE Double Tan(Double val)
	{
		return tan(val);
	}

	FORCEINLINE Double ArcSin(Double val)
	{
		return asin(val);
	}

	FORCEINLINE Double ArcCos(Double val)
	{
		return acos(val);
	}

	FORCEINLINE Double ArcTan(Double val)
	{
		return atan(val);
	}

	FORCEINLINE Double ArcTan2(Double valY, Double valX)
	{
		return atan2(valY, valX);
	}

	FORCEINLINE Double Sinh(Double val)
	{
		return 0.5 * (Math_Exp(val) - Math_Exp(-val));
	}

	FORCEINLINE Double Cosh(Double val)
	{
		return 0.5 * (Math_Exp(val) + Math_Exp(-val));
	}

	FORCEINLINE Double Tanh(Double val)
	{
		val = val * 2;
		return (Math_Exp(val) - 1) / (Math_Exp(val) + 1);
	}

	FORCEINLINE Double ArcSinh(Double val)
	{
		return Math_Ln(val + sqrt(val * val + 1));
	}

	FORCEINLINE Double ArcCosh(Double val)
	{
		return Math_Ln(val + sqrt(val - 1) * sqrt(val + 1));
	}

	FORCEINLINE Double ArcTanh(Double val)
	{
		return 0.5 * Math_Ln((1 + val) / (1 - val));
	}

	FORCEINLINE Double Log10(Double val)
	{
		return Math_Log10(val);
	}

	FORCEINLINE Double Ln(Double val)
	{
		return Math_Ln(val);
	}

	FORCEINLINE Double Exp(Double val)
	{
		return Math_Exp(val);
	}

	FORCEINLINE Double Abs(Double val)
	{
		if (val < 0)
			return -val;
		else
			return val;
	}

	FORCEINLINE Bool IsPrime(Int32 val)
	{
		if (val <= 0)
			return false;
		if (val <= 3)
			return true;
		if ((val & 1) == 0)
			return false;
		Int32 rt = (Int32)sqrt((double)val);
		Int32 i = 3;
		while (i <= rt)
		{
			if ((val % i) == 0)
				return false;
			i += 2;
		}
		return true;
	}

	FORCEINLINE Double Pow(Double x, Double y)
	{
		return Math_Pow(x, y);
	}

	FORCEINLINE Double Sqrt(Double val)
	{
		return sqrt(val);
	}
#else
	FORCEINLINE Double Sin(Double val)
	{
		return sin(val);
	}

	FORCEINLINE Double Cos(Double val)
	{
		return cos(val);
	}

	FORCEINLINE Double Tan(Double val)
	{
		return tan(val);
	}

	FORCEINLINE Double ArcSin(Double val)
	{
		return asin(val);
	}

	FORCEINLINE Double ArcCos(Double val)
	{
		return acos(val);
	}

	FORCEINLINE Double ArcTan(Double val)
	{
		return atan(val);
	}

	FORCEINLINE Double ArcTan2(Double valY, Double valX)
	{
		return atan2(valY, valX);
	}

	FORCEINLINE Double Sinh(Double val)
	{
		return 0.5 * (exp(val) - exp(-val));
	}

	FORCEINLINE Double Cosh(Double val)
	{
		return 0.5 * (exp(val) + exp(-val));
	}

	FORCEINLINE Double Tanh(Double val)
	{
		val = val * 2;
		return (exp(val) - 1) / (exp(val) + 1);
	}

	FORCEINLINE Double ArcSinh(Double val)
	{
		return log(val + sqrt(val * val + 1));
	}

	FORCEINLINE Double ArcCosh(Double val)
	{
		return log(val + sqrt(val - 1) * sqrt(val + 1));
	}

	FORCEINLINE Double ArcTanh(Double val)
	{
		return 0.5 * log((1 + val) / (1 - val));
	}

	FORCEINLINE Double Log10(Double val)
	{
		return log10(val);
	}

	FORCEINLINE Double Ln(Double val)
	{
		return log(val);
	}

	FORCEINLINE Double Exp(Double val)
	{
		return exp(val);
	}

	FORCEINLINE Double Abs(Double val)
	{
		if (val < 0)
			return -val;
		else
			return val;
	}

	FORCEINLINE Bool IsPrime(Int32 val)
	{
		if (val <= 0)
			return false;
		if (val <= 3)
			return true;
		if ((val & 1) == 0)
			return false;
		Int32 rt = (Int32)sqrt((double)val);
		Int32 i = 3;
		while (i <= rt)
		{
			if ((val % i) == 0)
				return false;
			i += 2;
		}
		return true;
	}

	FORCEINLINE Double Pow(Double x, Double y)
	{
		return pow(x, y);
	}

	FORCEINLINE Double Sqrt(Double val)
	{
		return sqrt(val);
	}
#endif

	FORCEINLINE Int32 Double2Int32(Double val)
	{
	#ifdef HAS_ASM32
		Int32 iv;
		_asm
		{
			fld val
			fistp iv
			mov eax,iv
		}
	#else
		if (val < 0)
		{
			return (Int32)(val - 0.5);
		}
		else
		{
			return (Int32)(val + 0.5);
		}
	#endif
	}

	FORCEINLINE Int64 Double2Int64(Double val)
	{
		if (val < 0)
		{
			return (Int64)(val - 0.5);
		}
		else
		{
			return (Int64)(val + 0.5);
		}
	}

	FORCEINLINE OSInt Double2OSInt(Double val)
	{
#if _OSINT_SIZE == 64
		return Double2Int64(val);
#else
		return Double2Int32(val);
#endif
	}

	FORCEINLINE Double OSInt2Double(OSInt val)
	{
		return (Double)val;
	}

	FORCEINLINE Double UOSInt2Double(UOSInt val)
	{
		return (Double)val;
	}

	FORCEINLINE Double Int64_Double(Int64 val)
	{
		return (Double)val;
	}

	FORCEINLINE Double UInt64_Double(UInt64 val)
	{
		return (Double)val;
	}

	FORCEINLINE Int32 SDouble2Int32(Double val)
	{
		if (val < -2147483648.0)
			return -1 - 2147483647;
		else if (val > 2147483647.0)
			return 2147483647;
		else
			return Math::Double2Int32(val);
	}

	FORCEINLINE Int32 SDouble2Int24(Double val)
	{
		if (val < -8388608.0)
			return -8388608;
		else if (val > 8388607.0)
			return 8388607;
		else
			return Math::Double2Int32(val);
	}
	
	FORCEINLINE Int16 SDouble2Int16(Double val)
	{
		if (val < -32768.0)
			return -32768;
		else if (val > 32767.0)
			return 32767;
		else
			return (Int16)Math::Double2Int32(val);
	}

	FORCEINLINE UInt16 SDouble2UInt16(Double val)
	{
		if (val < 0.0)
			return 0;
		else if (val > 65535.0)
			return 65535;
		else
			return (UInt16)Math::Double2Int32(val);
	}

	FORCEINLINE UInt8 SDouble2UInt8(Double val)
	{
		if (val > 255.0)
			return 255;
		else if (val < 0.0)
			return 0;
		else
			return (UInt8)Math::Double2Int32(val);
	}

	FORCEINLINE UInt8 SInt32_UInt8(Int32 val)
	{
		if (val > 255)
			return 255;
		else if (val < 0)
			return 0;
		else
			return (UInt8)val;
	}

	FORCEINLINE Double Fix(Double val)
	{
		return floor(val);
	}

	FORCEINLINE Bool IsNAN(Double val)
	{
		return (0x7fffffffffffffffLL == *(Int64*)&val) || (0xffffffffffffffffLL == *(UInt64*)&val);
	}

	FORCEINLINE Bool IsInfinity(Double val)
	{
		return 0x7ff0000000000000LL == *(Int64*)&val;
	}

	FORCEINLINE Bool IsNeg(Double val)
	{
		return ((Int64)0x8000000000000000LL & *(Int64*)&val) != 0;
	}

	FORCEINLINE Double GetNAN()
	{
		UInt64 v = 0xffffffffffffffffLL;
		return *(Double*)&v;
	}

	FORCEINLINE Bool IsRealNum(Double val)
	{
		if ((0x7fffffffffffffffLL == *(Int64*)&val) || (0xffffffffffffffffLL == *(UInt64*)&val) || (0x7ff0000000000000LL == *(Int64*)&val) || (0xfff8000000000000LL == *(UInt64*)&val) || (0x7ff8000000000000LL == *(Int64*)&val))
		{
			return false;
		}
		return true;
	}
}
extern "C"
{
	void Math_Int32Arr2DblArr(Double *dblArr, Int32 *intArr, UOSInt arrCnt);
}
#endif
