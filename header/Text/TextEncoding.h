#ifndef _SM_TEXT_TEXTENCODING
#define _SM_TEXT_TEXTENCODING

namespace Text
{
	class TextEncoding
	{
	public:
		virtual ~TextEncoding() {};
		
		virtual UTF8Char *EncodeString(UTF8Char *buff, const UTF8Char *strToEnc) = 0;
		virtual UTF8Char *DecodeString(UTF8Char *buff, const UTF8Char *strToDec) = 0;
	};
}
#endif
