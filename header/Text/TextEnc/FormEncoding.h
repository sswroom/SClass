
#ifndef _SM_TEXT_TEXTENC_FORMENCODING
#define _SM_TEXT_TEXTENC_FORMENCODING
#include "Text/StringBuilderUTF.h"
#include "Text/TextEncoding.h"

namespace Text
{
	namespace TextEnc
	{
		class FormEncoding : public Text::TextEncoding
		{
		public:
			static void FormEncode(Text::StringBuilderUTF *sb, const UTF8Char *uri);
			static UTF8Char *FormEncode(UTF8Char *buff, const UTF8Char *uri);
			static UTF8Char *FormDecode(UTF8Char *buff, const UTF8Char *uri);
			
			FormEncoding();
			virtual ~FormEncoding();
			virtual UTF8Char *EncodeString(UTF8Char *buff, const UTF8Char *strToEnc);
			virtual UTF8Char *DecodeString(UTF8Char *buff, const UTF8Char *strToDec);
		};

	}
}
#endif
