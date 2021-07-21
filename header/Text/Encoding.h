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
		UInt32 GetEncCodePage();
		UTF8Char *ToString(UTF8Char *buff);

		// bug if _WCHAR_SIZE != 2
		UOSInt CountWChars(const UInt8 *bytes, UOSInt byteSize);
		WChar *WFromBytes(WChar *buff, const UInt8 *bytes, UOSInt byteSize, UOSInt *byteConv);

		UOSInt CountUTF8Chars(const UInt8 *bytes, UOSInt byteSize);
		UTF8Char *UTF8FromBytes(UTF8Char *buff, const UInt8 *bytes, UOSInt byteSize, UOSInt *byteConv);

		// bug if _WCHAR_SIZE != 2
		UOSInt WCountBytes(const WChar *str); // Optimized, retSize include null byte
		UOSInt WCountBytesC(const WChar *str, UOSInt strLen); // Optimized, strLen = other to force size and retSize exclude null
		UOSInt WToBytes(UInt8 *bytes, const WChar *str); // Optimized, retSize include null byte
		UOSInt WToBytesC(UInt8 *bytes, const WChar *str, UOSInt strLen); // Optimized, strLen = other to force size and retSize exclude null

		UOSInt UTF8CountBytes(const UTF8Char *str); // Optimized, retSize include null byte
		UOSInt UTF8CountBytesC(const UTF8Char *str, UOSInt strLen); // Optimized, strLen = other to force size and retSize exclude null
		UOSInt UTF8ToBytes(UInt8 *bytes, const UTF8Char *str); // Optimized, retSize include null byte
		UOSInt UTF8ToBytesC(UInt8 *bytes, const UTF8Char *str, UOSInt strLen); // Optimized, strLen = other to force size and retSize exclude null

		const UInt8 *NextWChar(const UInt8 *buff, WChar *outputChar);
	};
}
#endif
