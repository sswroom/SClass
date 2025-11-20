#ifndef _SM_TEXT_TEXTBINENC_TEXTBINENCLIST
#define _SM_TEXT_TEXTBINENC_TEXTBINENCLIST
#include "Data/ArrayListNN.hpp"
#include "Text/TextBinEnc/TextBinEnc.h"

namespace Text
{
	namespace TextBinEnc
	{
		class TextBinEncList
		{
		private:
			Data::ArrayListNN<Text::TextBinEnc::TextBinEnc> encList;

		public:
			TextBinEncList();
			~TextBinEncList();

			NN<Data::ArrayListNN<Text::TextBinEnc::TextBinEnc>> GetEncList();
		};
	}
}
#endif
