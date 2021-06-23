#ifndef _SM_MEDIA_CODEIMAGEGEN_CODEIMAGEGEN
#define _SM_MEDIA_CODEIMAGEGEN_CODEIMAGEGEN
#include "Media/DrawEngine.h"

namespace Media
{
	namespace CodeImageGen
	{
		class CodeImageGen
		{
		public:
			typedef enum
			{
				CT_EAN13,
				CT_EAN8,
				CT_EAN5,
				CT_EAN2,

				CT_FIRST = CT_EAN13,
				CT_LAST = CT_EAN2
			} CodeType;
		public:
			virtual ~CodeImageGen();

			virtual CodeType GetCodeType() = 0;
			virtual UOSInt GetMinLength() = 0;
			virtual UOSInt GetMaxLength() = 0;
			virtual Media::DrawImage *GenCode(const UTF8Char *code, UOSInt codeWidth, Media::DrawEngine *eng) = 0;
		public:
			static CodeImageGen *CreateGenerator(CodeType codeType);
			static const UTF8Char *GetCodeName(CodeType codeType);
		};
	}
}
#endif
