#ifndef _SM_TEXT_TEXTBINENC_TEXTBINENCLIST
#define _SM_TEXT_TEXTBINENC_TEXTBINENCLIST
#include "Data/ArrayList.h"
#include "Text/TextBinEnc/ITextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class TextBinEncList
		{
		private:
			Data::ArrayList<Text::TextBinEnc::ITextBinEnc*> *encList;

		public:
			TextBinEncList();
			~TextBinEncList();

			Data::ArrayList<Text::TextBinEnc::ITextBinEnc*> *GetEncList();
		};
	}
}
#endif
