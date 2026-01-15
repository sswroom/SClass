#ifndef _SM_MATH_BIGINTUTIL
#define _SM_MATH_BIGINTUTIL
namespace Math
{
	class BigIntUtil
	{
	public:
		static void LSBNeg(UnsafeArray<UIntOS> valBuff, UIntOS valCnt);
		static void LSBAdd(UnsafeArray<UIntOS> destBuff, UnsafeArray<const UIntOS> srcBuff, UIntOS destCnt, UIntOS srcCnt);
		static void LSBSub(UnsafeArray<UIntOS> destBuff, UnsafeArray<const UIntOS> srcBuff, UIntOS destCnt, UIntOS srcCnt);
		static void LSBMul(UnsafeArray<UIntOS> destBuff, UnsafeArray<const UIntOS> srcBuff, UIntOS destCnt, UIntOS srcCnt);
		static void LSBAnd(UnsafeArray<UIntOS> destBuff, UnsafeArray<const UIntOS> srcBuff, UIntOS destCnt, UIntOS srcCnt);
		static void LSBOr(UnsafeArray<UIntOS> destBuff, UnsafeArray<const UIntOS> srcBuff, UIntOS destCnt, UIntOS srcCnt);
		static void LSBXor(UnsafeArray<UIntOS> destBuff, UnsafeArray<const UIntOS> srcBuff, UIntOS destCnt, UIntOS srcCnt);
		static UIntOS LSBAddUOS(UnsafeArray<UIntOS> valBuff, UIntOS valCnt, UIntOS val); //return overflow value
		static UIntOS LSBMulUOS(UnsafeArray<UIntOS> valBuff, UIntOS valCnt, UIntOS val); //return overflow value
		static UIntOS LSBDivUOS(UnsafeArray<UIntOS> valBuff, UIntOS valCnt, UIntOS val); //return remainder
		static void LSBAssignI(UnsafeArray<UIntOS> valBuff, UIntOS valCnt, IntOS val);
		static void LSBAssignU(UnsafeArray<UIntOS> valBuff, UIntOS valCnt, UIntOS val);
		static void LSBAssignStr(UnsafeArray<UIntOS> valBuff, UIntOS valCnt, UnsafeArray<const UTF8Char> val);
		static UnsafeArray<UTF8Char> LSBToString(UnsafeArray<UTF8Char> buff, UnsafeArray<const UIntOS> valArr, UnsafeArray<UIntOS> tmpArr, UIntOS valCnt);
	};
}
#endif
