#ifndef _SM_MEDIA_CODEIMAGEGEN_CODEIMAGEGEN
#define _SM_MEDIA_CODEIMAGEGEN_CODEIMAGEGEN
#include "Media/DrawEngine.h"
#include "Text/CString.h"

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
				CT_QRCODE,

				CT_FIRST = CT_EAN13,
				CT_LAST = CT_QRCODE
			} CodeType;
		public:
			virtual ~CodeImageGen();

			virtual CodeType GetCodeType() = 0;
			virtual UIntOS GetMinLength() = 0;
			virtual UIntOS GetMaxLength() = 0;
			virtual Optional<Media::DrawImage> GenCode(Text::CStringNN code, UIntOS codeWidth, NN<Media::DrawEngine> eng) = 0;
		public:
			static Optional<CodeImageGen> CreateGenerator(CodeType codeType);
			static Text::CStringNN GetCodeName(CodeType codeType);
		};
	}
}
#endif
