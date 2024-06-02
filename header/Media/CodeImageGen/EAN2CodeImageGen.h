#ifndef _SM_MEDIA_CODEIMAGEGEN_EAN2CODEIMAGEGEN
#define _SM_MEDIA_CODEIMAGEGEN_EAN2CODEIMAGEGEN
#include "Media/CodeImageGen/CodeImageGen.h"

namespace Media
{
	namespace CodeImageGen
	{
		class EAN2CodeImageGen : public Media::CodeImageGen::CodeImageGen
		{
		public:
			EAN2CodeImageGen();
			virtual ~EAN2CodeImageGen();

			virtual CodeType GetCodeType();
			virtual UOSInt GetMinLength();
			virtual UOSInt GetMaxLength();
			virtual Optional<Media::DrawImage> GenCode(Text::CStringNN code, UOSInt codeWidth, NN<Media::DrawEngine> eng);
		};
	}
}
#endif
