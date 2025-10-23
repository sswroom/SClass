#ifndef _SM_MATH_BIGINTUTIL
#define _SM_MATH_BIGINTUTIL
namespace Math
{
	class BigIntUtil
	{
	public:
		static void LSBNeg2(UnsafeArray<UOSInt> valBuff, UOSInt valCnt);
		static void LSBAdd2(UnsafeArray<UOSInt> destBuff, UnsafeArray<const UOSInt> srcBuff, UOSInt destCnt, UOSInt srcCnt);
		static void LSBSub2(UnsafeArray<UOSInt> destBuff, UnsafeArray<const UOSInt> srcBuff, UOSInt destCnt, UOSInt srcCnt);
		static void LSBMul2(UnsafeArray<UOSInt> destBuff, UnsafeArray<const UOSInt> srcBuff, UOSInt destCnt, UOSInt srcCnt);
		static void LSBAnd2(UnsafeArray<UOSInt> destBuff, UnsafeArray<const UOSInt> srcBuff, UOSInt destCnt, UOSInt srcCnt);
		static void LSBOr2(UnsafeArray<UOSInt> destBuff, UnsafeArray<const UOSInt> srcBuff, UOSInt destCnt, UOSInt srcCnt);
		static void LSBXor2(UnsafeArray<UOSInt> destBuff, UnsafeArray<const UOSInt> srcBuff, UOSInt destCnt, UOSInt srcCnt);
		static UOSInt LSBAddUOS2(UnsafeArray<UOSInt> valBuff, UOSInt valCnt, UOSInt val); //return overflow value
		static UOSInt LSBMulUOS2(UnsafeArray<UOSInt> valBuff, UOSInt valCnt, UOSInt val); //return overflow value
		static UOSInt LSBDivUOS2(UnsafeArray<UOSInt> valBuff, UOSInt valCnt, UOSInt val); //return remainder
		static void LSBAssignI2(UnsafeArray<UOSInt> valBuff, UOSInt valCnt, OSInt val);
		static void LSBAssignU2(UnsafeArray<UOSInt> valBuff, UOSInt valCnt, UOSInt val);
		static void LSBAssignStr2(UnsafeArray<UOSInt> valBuff, UOSInt valCnt, UnsafeArray<const UTF8Char> val);
		static UnsafeArray<UTF8Char> LSBToString2(UnsafeArray<UTF8Char> buff, UnsafeArray<const UOSInt> valArr, UnsafeArray<UOSInt> tmpArr, UOSInt valCnt);
	};
}
#endif
