#ifndef _SM_MATH_BIGINTUTIL
#define _SM_MATH_BIGINTUTIL
namespace Math
{
	class BigIntUtil
	{
	public:
		static void LSBNeg(UnsafeArray<UOSInt> valBuff, UOSInt valCnt);
		static void LSBAdd(UnsafeArray<UOSInt> destBuff, UnsafeArray<const UOSInt> srcBuff, UOSInt destCnt, UOSInt srcCnt);
		static void LSBSub(UnsafeArray<UOSInt> destBuff, UnsafeArray<const UOSInt> srcBuff, UOSInt destCnt, UOSInt srcCnt);
		static void LSBMul(UnsafeArray<UOSInt> destBuff, UnsafeArray<const UOSInt> srcBuff, UOSInt destCnt, UOSInt srcCnt);
		static void LSBAnd(UnsafeArray<UOSInt> destBuff, UnsafeArray<const UOSInt> srcBuff, UOSInt destCnt, UOSInt srcCnt);
		static void LSBOr(UnsafeArray<UOSInt> destBuff, UnsafeArray<const UOSInt> srcBuff, UOSInt destCnt, UOSInt srcCnt);
		static void LSBXor(UnsafeArray<UOSInt> destBuff, UnsafeArray<const UOSInt> srcBuff, UOSInt destCnt, UOSInt srcCnt);
		static UOSInt LSBAddUOS(UnsafeArray<UOSInt> valBuff, UOSInt valCnt, UOSInt val); //return overflow value
		static UOSInt LSBMulUOS(UnsafeArray<UOSInt> valBuff, UOSInt valCnt, UOSInt val); //return overflow value
		static UOSInt LSBDivUOS(UnsafeArray<UOSInt> valBuff, UOSInt valCnt, UOSInt val); //return remainder
		static void LSBAssignI(UnsafeArray<UOSInt> valBuff, UOSInt valCnt, OSInt val);
		static void LSBAssignU(UnsafeArray<UOSInt> valBuff, UOSInt valCnt, UOSInt val);
		static void LSBAssignStr(UnsafeArray<UOSInt> valBuff, UOSInt valCnt, UnsafeArray<const UTF8Char> val);
		static UnsafeArray<UTF8Char> LSBToString(UnsafeArray<UTF8Char> buff, UnsafeArray<const UOSInt> valArr, UnsafeArray<UOSInt> tmpArr, UOSInt valCnt);
	};
}
#endif
