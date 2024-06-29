#ifndef _SM_TEXT_ENCODING
#define _SM_TEXT_ENCODING

namespace Text
{
	class Encoding
	{
	private:
		UInt32 codePage;
		Bool lastHigh;
	public:
		Encoding(UInt32 codePage);
		Encoding();
		~Encoding();
		
		void SetCodePage(UInt32 codePage);
		UInt32 GetEncCodePage() const;
		UnsafeArray<UTF8Char> ToString(UnsafeArray<UTF8Char> buff);
		Bool IsUTF8() const { return this->codePage == 65001; }

		// bug if _WCHAR_SIZE != 2
		UOSInt CountWChars(UnsafeArray<const UInt8> bytes, UOSInt byteSize);
		UnsafeArray<WChar> WFromBytes(UnsafeArray<WChar> buff, UnsafeArray<const UInt8> bytes, UOSInt byteSize, OptOut<UOSInt> byteConv);

		UOSInt CountUTF8Chars(UnsafeArray<const UInt8> bytes, UOSInt byteSize);
		UnsafeArray<UTF8Char> UTF8FromBytes(UnsafeArray<UTF8Char> buff, UnsafeArray<const UInt8> bytes, UOSInt byteSize, OptOut<UOSInt> byteConv);

		// bug if _WCHAR_SIZE != 2
		UOSInt WCountBytes(UnsafeArray<const WChar> str); // Optimized, retSize include null byte
		UOSInt WCountBytesC(UnsafeArray<const WChar> str, UOSInt strLen); // Optimized, strLen = other to force size and retSize exclude null
		UOSInt WToBytes(UnsafeArray<UInt8> bytes, UnsafeArray<const WChar> str); // Optimized, retSize include null byte
		UOSInt WToBytesC(UnsafeArray<UInt8> bytes, UnsafeArray<const WChar> str, UOSInt strLen); // Optimized, strLen = other to force size and retSize exclude null

		UOSInt UTF8CountBytes(UnsafeArray<const UTF8Char> str); // Optimized, retSize include null byte
		UOSInt UTF8CountBytesC(UnsafeArray<const UTF8Char> str, UOSInt strLen); // Optimized, strLen = other to force size and retSize exclude null
		UOSInt UTF8ToBytes(UnsafeArray<UInt8> bytes, UnsafeArray<const UTF8Char> str); // Optimized, retSize include null byte
		UOSInt UTF8ToBytesC(UnsafeArray<UInt8> bytes, UnsafeArray<const UTF8Char> str, UOSInt strLen); // Optimized, strLen = other to force size and retSize exclude null

		UnsafeArray<const UInt8> NextWChar(UnsafeArray<const UInt8> buff, OutParam<WChar> outputChar);
	};
}
#endif
