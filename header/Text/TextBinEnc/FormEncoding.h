
#ifndef _SM_TEXT_TEXTENC_FORMENCODING
#define _SM_TEXT_TEXTENC_FORMENCODING
#include "Text/StringBuilderUTF.h"
#include "Text/TextBinEnc/ITextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class FormEncoding : public Text::TextBinEnc::ITextBinEnc
		{
		public:
			static void FormEncode(Text::StringBuilderUTF *sb, const UTF8Char *uri, UOSInt uriLen);
			static UTF8Char *FormEncode(UTF8Char *buff, const UTF8Char *uri);
			static UTF8Char *FormDecode(UTF8Char *buff, const UTF8Char *uri);
			
			FormEncoding();
			virtual ~FormEncoding();
			virtual UOSInt EncodeBin(Text::StringBuilderUTF *sb, const UInt8 *dataBuff, UOSInt buffSize);
			virtual UOSInt CalcBinSize(const UTF8Char *str, UOSInt strLen);
			virtual UOSInt DecodeBin(const UTF8Char *str, UOSInt strLen, UInt8 *dataBuff);
			virtual Text::CString GetName();
		};

	}
}
#endif
