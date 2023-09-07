#ifndef _SM_TEXT_TEXTBINENC_UTF8TEXTBINENC
#define _SM_TEXT_TEXTBINENC_UTF8TEXTBINENC
#include "Text/TextBinEnc/CodePageTextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class UTF8TextBinEnc : public Text::TextBinEnc::CodePageTextBinEnc
		{
		public:
			UTF8TextBinEnc();
			virtual ~UTF8TextBinEnc();
			virtual Text::CStringNN GetName() const;
		};
	}
}
#endif
