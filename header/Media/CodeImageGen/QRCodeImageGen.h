#ifndef _SM_MEDIA_CODEIMAGEGEN_QRCODEIMAGEGEN
#define _SM_MEDIA_CODEIMAGEGEN_QRCODEIMAGEGEN
#include "Media/CodeImageGen/CodeImageGen.h"

namespace Media
{
	namespace CodeImageGen
	{
		class QRCodeImageGen : public Media::CodeImageGen::CodeImageGen
		{
		public:
			QRCodeImageGen();
			virtual ~QRCodeImageGen();

			virtual CodeType GetCodeType();
			virtual UOSInt GetMinLength();
			virtual UOSInt GetMaxLength();
			virtual Media::DrawImage *GenCode(Text::CString code, UOSInt codeWidth, Media::DrawEngine *eng);
		};
	}
}
#endif
