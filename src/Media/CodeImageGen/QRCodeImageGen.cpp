#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/ZXingWriter.h"
#include "Media/CodeImageGen/QRCodeImageGen.h"
#include "Text/MyString.h"

Media::CodeImageGen::QRCodeImageGen::QRCodeImageGen()
{
}

Media::CodeImageGen::QRCodeImageGen::~QRCodeImageGen()
{
}

Media::CodeImageGen::CodeImageGen::CodeType Media::CodeImageGen::QRCodeImageGen::GetCodeType()
{
	return Media::CodeImageGen::CodeImageGen::CT_QRCODE;
}

UOSInt Media::CodeImageGen::QRCodeImageGen::GetMinLength()
{
	return 1;
}

UOSInt Media::CodeImageGen::QRCodeImageGen::GetMaxLength()
{
	return 7089;
}

Media::DrawImage *Media::CodeImageGen::QRCodeImageGen::GenCode(Text::CString code, UOSInt codeWidth, NotNullPtr<Media::DrawEngine> eng)
{
	if (code.v == 0 || code.leng == 0)
		return 0;

	Media::StaticImage *simg = Media::ZXingWriter::GenQRCode(code, Math::Size2D<UOSInt>(256, 256));
	if (simg == 0)
		return 0;
	Media::DrawImage *dimg = eng->ConvImage(simg);
	DEL_CLASS(simg);
	return dimg;
}
