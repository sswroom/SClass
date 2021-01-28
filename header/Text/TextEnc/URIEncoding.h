#ifndef _SM_TEXT_TEXTENC_URIENCODING
#define _SM_TEXT_TEXTENC_URIENCODING
#include "Text/TextEncoding.h"

namespace Text
{
	namespace TextEnc
	{
		class URIEncoding : public Text::TextEncoding
		{
		public:
			static UTF8Char *URIEncode(UTF8Char *buff, const UTF8Char *uri);
			static UTF8Char *URIDecode(UTF8Char *buff, const UTF8Char *uri);
			
			URIEncoding();
			virtual ~URIEncoding();
			virtual UTF8Char *EncodeString(UTF8Char *buff, const UTF8Char *strToEnc);
			virtual UTF8Char *DecodeString(UTF8Char *buff, const UTF8Char *strToDec);
		};
	}
}
#endif
