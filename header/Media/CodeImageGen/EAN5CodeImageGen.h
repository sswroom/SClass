#ifndef _SM_MEDIA_CODEIMAGEGEN_EAN5CODEIMAGEGEN
#define _SM_MEDIA_CODEIMAGEGEN_EAN5CODEIMAGEGEN
#include "Media/CodeImageGen/CodeImageGen.h"

namespace Media
{
	namespace CodeImageGen
	{
		class EAN5CodeImageGen : public Media::CodeImageGen::CodeImageGen
		{
		public:
			EAN5CodeImageGen();
			virtual ~EAN5CodeImageGen();

			virtual CodeType GetCodeType();
			virtual UIntOS GetMinLength();
			virtual UIntOS GetMaxLength();
			virtual Optional<Media::DrawImage> GenCode(Text::CStringNN code, UIntOS codeWidth, NN<Media::DrawEngine> eng);
		};
	}
}
#endif
