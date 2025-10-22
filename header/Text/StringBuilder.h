#ifndef _SM_TEXT_STRINGBUILDER
#define _SM_TEXT_STRINGBUILDER
#include "MemTool.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "Text/LineBreakType.h"
#include "Text/MyString.h"

namespace Text
{
	template <class T> class StringBuilder
	{
	protected:
		UnsafeArray<T> buff;
		UnsafeArray<T> buffEnd;
		UOSInt buffSize;
	public:
		StringBuilder();
		virtual ~StringBuilder();

		NN<StringBuilder<T>> AppendI16(Int16 iVal);
		NN<StringBuilder<T>> AppendU16(UInt16 iVal);
		NN<StringBuilder<T>> AppendI32(Int32 iVal);
		NN<StringBuilder<T>> AppendU32(UInt32 iVal);
		NN<StringBuilder<T>> AppendI64(Int64 iVal);
		NN<StringBuilder<T>> AppendU64(UInt64 iVal);
		NN<StringBuilder<T>> AppendDate(NN<Data::DateTime> dt);
		NN<StringBuilder<T>> AppendOSInt(OSInt iVal);
		NN<StringBuilder<T>> AppendUOSInt(UOSInt iVal);
		NN<StringBuilder<T>> AppendSB(NN<Text::StringBuilder<T>> sb);

		NN<StringBuilder<T>> AppendHex8(UInt8 iVal);
		NN<StringBuilder<T>> AppendHex16(UInt16 iVal);
		NN<StringBuilder<T>> AppendHex24(UInt32 iVal);
		NN<StringBuilder<T>> AppendHex32(UInt32 iVal);
		NN<StringBuilder<T>> AppendHex32V(UInt32 iVal);
		NN<StringBuilder<T>> AppendHex64(UInt64 iVal);
		NN<StringBuilder<T>> AppendHex64V(UInt64 iVal);
		NN<StringBuilder<T>> AppendHexOS(UOSInt iVal);
		NN<StringBuilder<T>> AppendHex(UnsafeArray<const UInt8> buff, UOSInt buffSize, T seperator, LineBreakType lineBreak);

		void ClearStr();
		void AllocLeng(UOSInt leng);
		UOSInt GetLength();
		void RemoveChars(UOSInt cnt);
		void RemoveChars(UOSInt index, UOSInt cnt);
		void Trim();
		void TrimWSCRLF();
		void TrimRight();
		void TrimToLength(UOSInt leng);
		NN<StringBuilder<T>> SetSubstr(UOSInt index);
		void ToUpper();
		void ToLower();
		void ToCapital();

		Bool ToUInt16(OutParam<UInt16> outVal);
		Bool ToUInt32(OutParam<UInt32> outVal);
		Bool ToUInt32S(OutParam<UInt32> outVal, UInt32 failVal);
		Bool ToUOSInt(OutParam<UOSInt> outVal);
		Bool ToInt32(OutParam<Int32> outVal);
		Int32 ToInt32();
		Bool ToInt64(OutParam<Int64> outVal);
		Int64 ToInt64();
		Bool ToUInt64(OutParam<UInt64> outVal);
		UInt64 ToUInt64();
		Bool ToOSInt(OutParam<OSInt> outVal);
		UOSInt Hex2Bytes(OutParam<UInt8> buff);

		UnsafeArray<T> ToString();
		T *ToPtr();
		UnsafeArray<T> GetEndPtr();
		void SetEndPtr(UnsafeArray<T> ptr);
		UOSInt IndexOf(UnsafeArray<const T> s);
		UOSInt IndexOf(UnsafeArray<const T> s, UOSInt index);
		UOSInt IndexOf(T c);
		UOSInt LastIndexOf(T c);
		Bool Equals(UnsafeArray<const T> s);
		Bool EqualsICase(UnsafeArray<const T> s);
		Bool StartsWith(UnsafeArray<const T> s);
		Bool EndsWith(T c);
		UnsafeArray<T> SubString(UnsafeArray<T> buff, UOSInt start, UOSInt length);
		UOSInt Replace(T fromChar, T toChar);
		UOSInt Replace(UnsafeArray<const T> fromStr, UnsafeArray<const T> toStr);
		UOSInt ReplaceICase(UnsafeArray<const T> fromStr, UnsafeArray<const T> toStr);
	};
	#include "Text/StringBuilder_T.h"
}
#endif
