#ifndef _SM_TEXT_TEXTBINENC_TEXTBINENCLIST
#define _SM_TEXT_TEXTBINENC_TEXTBINENCLIST
#include "Data/ArrayListNN.h"
#include "Text/TextBinEnc/ITextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class TextBinEncList
		{
		private:
			Data::ArrayListNN<Text::TextBinEnc::ITextBinEnc> encList;

		public:
			TextBinEncList();
			~TextBinEncList();

			NN<Data::ArrayListNN<Text::TextBinEnc::ITextBinEnc>> GetEncList();
		};
	}
}
#endif
