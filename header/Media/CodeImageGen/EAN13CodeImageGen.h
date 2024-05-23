#ifndef _SM_MEDIA_CODEIMAGEGEN_EAN13CODEIMAGEGEN
#define _SM_MEDIA_CODEIMAGEGEN_EAN13CODEIMAGEGEN
#include "Media/CodeImageGen/CodeImageGen.h"

namespace Media
{
	namespace CodeImageGen
	{
		class EAN13CodeImageGen : public Media::CodeImageGen::CodeImageGen
		{
		public:
			EAN13CodeImageGen();
			virtual ~EAN13CodeImageGen();

			virtual CodeType GetCodeType();
			virtual UOSInt GetMinLength();
			virtual UOSInt GetMaxLength();
			virtual Optional<Media::DrawImage> GenCode(Text::CString code, UOSInt codeWidth, NN<Media::DrawEngine> eng);
		};
	}
}
#endif
