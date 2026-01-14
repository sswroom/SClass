#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
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

Optional<Media::DrawImage> Media::CodeImageGen::QRCodeImageGen::GenCode(Text::CStringNN code, UOSInt codeWidth, NN<Media::DrawEngine> eng)
{
	if (code.leng == 0)
		return nullptr;

	NN<Media::StaticImage> simg;
	if (!Media::ZXingWriter::GenQRCode(code, Math::Size2D<UOSInt>(256, 256)).SetTo(simg))
		return nullptr;
	Optional<Media::DrawImage> dimg = eng->ConvImage(simg, nullptr);
	simg.Delete();
	return dimg;
}
