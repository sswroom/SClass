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
			virtual OSInt GetMinLength();
			virtual OSInt GetMaxLength();
			virtual Media::DrawImage *GenCode(const UTF8Char *code, OSInt codeWidth, Media::DrawEngine *eng);
		};
	};
};
#endif
