#ifndef _SM_MATH_MATH_C
#define _SM_MATH_MATH_C
#if defined(_MSC_VER) && _MSC_VER == 1500
#pragma warning (disable: 4985)
#endif

#include <math.h>

Double Math_MyLog10(Double val);
Double Math_MyLn(Double val);
Double Math_MyExp(Double val);
Double Math_MyPow(Double x, Double y);

#if defined(FPUBUG)
#define Math_Log10(val) Math_MyLog10(val)
#define Math_Ln(val) Math_MyLn(val)
#define Math_Exp(val) Math_MyExp(val)
#define Math_Pow(x, y) Math_MyPow(x, y)
#else
#define Math_Log10(val) log10((Double)(val))
#define Math_Ln(val) log(val)
#define Math_Exp(val) exp(val)
#define Math_Pow(x, y) pow(x, y)
#endif

#define Math_Sin(val) sin(val)
#define Math_Cos(val) cos(val)
#define Math_Tan(val) tan(val)
#define Math_ArcSin(val) asin(val)
#define Math_ArcCos(val) acos(val)
#define Math_ArcTan(val) atan(val)
#define Math_ArcTan2(valY, valX) atan2(valY, valX)
#define Math_Abs(val) (((val) < 0)?(-val):(val))
#define Math_Sqrt(val) sqrt((Double)(val))

#define Math_Sinh(val) (0.5 * (Math_Exp(val) - Math_Exp(-val)))
#define Math_Cosh(val) (0.5 * (Math_Exp(val) + Math_Exp(-val)))
#define Math_Tanh(val) (0.5 * ((Math_Exp(val * 2) - 1) / (Math_Exp(val * 2) + 1))
#define Math_ArcSinh(val) (Math_Ln(val + Math_Sqrt(val * val + 1)))
#define Math_ArcCosh(val) (Math_Ln(val + Math_Sqrt(val - 1) * Math_Sqrt(val + 1)))
#define Math_ArcTanh(val) (0.5 * Math_Ln((1 + val) / (1 - val)))
#define Math_Fix(val) floor(val)
#define Math_Round(val) ((val) >= 0 ? ((Int32)((val) + 0.5)) : ((Int32)((val) - 0.5)))
#define Math_Min(val1, val2) (((val1) < (val2))?(val1):(val2))
#define Math_Max(val1, val2) (((val1) > (val2))?(val1):(val2))

FORCEINLINE Double Math_Sqr(Double v)
{
	return v * v;
}
namespace Math
{
	extern const Double PI;

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

	FORCEINLINE Int32 SDouble2Int32(Double val)
	{
		if (val < -2147483648.0)
			return -1 - 2147483647;
		else if (val > 2147483647.0)
			return 2147483647;
		else
			return Double2Int32(val);
	}

	FORCEINLINE Int32 SDouble2Int24(Double val)
	{
		if (val < -8388608.0)
			return -8388608;
		else if (val > 8388607.0)
			return 8388607;
		else
			return Double2Int32(val);
	}
	
	FORCEINLINE Int16 SDouble2Int16(Double val)
	{
		if (val < -32768.0)
			return -32768;
		else if (val > 32767.0)
			return 32767;
		else
			return (Int16)Double2Int32(val);
	}

	FORCEINLINE UInt16 SDouble2UInt16(Double val)
	{
		if (val < 0.0)
			return 0;
		else if (val > 65535.0)
			return 65535;
		else
			return (UInt16)Double2Int32(val);
	}

	FORCEINLINE UInt8 SDouble2UInt8(Double val)
	{
		if (val > 255.0)
			return 255;
		else if (val < 0.0)
			return 0;
		else
			return (UInt8)Double2Int32(val);
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

	FORCEINLINE Bool IsNAN(Double val)
	{
		UInt64 iVal = *(UInt64*)&val;
		return (iVal & 0x7ff0000000000000LL) == 0x7ff0000000000000LL && (iVal & 0xFFFFFFFFFFFFFLL) != 0;
		// 0x7ff8000000000001LL = qNaN
		// 0x7ff0000000000001LL = sNaN
		// 0x7fffffffffffffffLL = NaN
	}

	FORCEINLINE Bool IsInfinity(Double val)
	{
		return (0x7ff0000000000000LL == *(UInt64*)&val) || (0xfff0000000000000LL == *(UInt64*)&val);
	}

	FORCEINLINE Bool IsNeg(Double val)
	{
		return ((Int64)0x8000000000000000LL & *(Int64*)&val) != 0;
	}

	FORCEINLINE Double GetInfinity()
	{
		UInt64 v = 0x7ff0000000000000LL;
		return *(Double*)&v;
	}

	FORCEINLINE Double GetNAN()
	{
		UInt64 v = 0xffffffffffffffffLL;
		return *(Double*)&v;
	}

	FORCEINLINE Double GetSNAN()
	{
		UInt64 v = 0x7ff0000000000001LL;
		return *(Double*)&v;
	}

	FORCEINLINE Double GetQNAN()
	{
		UInt64 v = 0x7ff8000000000001LL;
		return *(Double*)&v;
	}

	FORCEINLINE Double NANTo(Double srcV, Double toV)
	{
		return IsNAN(srcV)?toV:srcV;
	}
	
	FORCEINLINE Bool NearlyEquals(Double val1, Double val2, Double diffRatio)
	{
		if (IsInfinity(val1))
		{
			return IsInfinity(val2);
		}
		if (IsNAN(val1))
		{
			return IsNAN(val2);
		}
		Double aval1 = Math_Abs(val1);
		Double aval2 = Math_Abs(val2);
		Double diffV;
		if (aval1 > aval2)
			diffV = aval1 * diffRatio;
		else
			diffV = aval2 * diffRatio;
		Double diff = val1 - val2;
		return diff >= -diffV && diff <= diffV;
	}

	FORCEINLINE Bool NearlyEqualsDbl(Double val1, Double val2)
	{
		return NearlyEquals(val1, val2, 0.00000000001);
	}

	FORCEINLINE Bool IsRealNum(Double val)
	{
		if ((0x7fffffffffffffffLL == *(Int64*)&val) || (0xffffffffffffffffLL == *(UInt64*)&val) || (0x7ff0000000000000LL == *(Int64*)&val) || (0xfff8000000000000LL == *(UInt64*)&val) || (0x7ff8000000000000LL == *(Int64*)&val))
		{
			return false;
		}
		return true;
	}

	FORCEINLINE Double Max(Double v1, Double v2, Double v3, Double v4)
	{
		if (v2 > v1) v1 = v2;
		if (v3 > v1) v1 = v3;
		if (v4 > v1) v1 = v4;
		return v1;
	}

	FORCEINLINE Double Min(Double v1, Double v2, Double v3, Double v4)
	{
		if (v2 < v1) v1 = v2;
		if (v3 < v1) v1 = v3;
		if (v4 < v1) v1 = v4;
		return v1;
	}
}
extern "C"
{
	void Math_Int32Arr2DblArr(Double *dblArr, const Int32 *intArr, UIntOS arrCnt);
}
#endif
