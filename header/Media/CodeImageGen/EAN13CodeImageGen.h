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
			virtual OSInt GetMinLength();
			virtual OSInt GetMaxLength();
			virtual Media::DrawImage *GenCode(const UTF8Char *code, OSInt codeWidth, Media::DrawEngine *eng);
		};
	};
};
#endif
