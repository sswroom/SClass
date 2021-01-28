#ifndef _SM_TEXT_TEXTBINENC_ANSITEXTBINENC
#define _SM_TEXT_TEXTBINENC_ANSITEXTBINENC
#include "Text/TextBinEnc/CodePageTextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class ANSITextBinEnc : public Text::TextBinEnc::CodePageTextBinEnc
		{
		public:
			ANSITextBinEnc();
			virtual ~ANSITextBinEnc();
			virtual const UTF8Char *GetName();
		};
	}
}
#endif
