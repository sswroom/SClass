#include "Stdafx.h"
#include "Media/ZXingWriter.h"
#include "Text/MyStringW.h"

#include <ZXing/BitMatrix.h>
#include <ZXing/MultiFormatWriter.h>
#include <ZXing/ZXVersion.h>

Optional<Media::StaticImage> Media::ZXingWriter::GenQRCode(Text::CStringNN content, Math::Size2D<UIntOS> outputSize)
{
#if (ZXING_VERSION_MAJOR * 10000 + ZXING_VERSION_MINOR * 100 + ZXING_VERSION_PATCH) >= 10200
	ZXing::MultiFormatWriter writer(ZXing::BarcodeFormat::QRCode);
#else
	ZXing::MultiFormatWriter writer(ZXing::BarcodeFormat::QR_CODE);
#endif
#if (ZXING_VERSION_MAJOR * 10000 + ZXING_VERSION_MINOR * 100 + ZXING_VERSION_PATCH) >= 10400
	std::string s((const char*)content.v.Ptr(), (size_t)content.leng);
#else
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(content.v);
	std::wstring s(wptr.Ptr());
	Text::StrDelNew(wptr);
#endif
	ZXing::BitMatrix bitMatrix = writer.encode(s, (int)outputSize.x, (int) outputSize.y);
	Media::StaticImage *simg;
	UnsafeArray<UInt8> pal;
	NEW_CLASS(simg, Media::StaticImage(outputSize, 0, 1, Media::PixelFormat::PF_PAL_W1, 0, Media::ColorProfile(), Media::ColorProfile::YUVT_BT601, Media::AT_ALPHA_ALL_FF, Media::YCOFST_C_CENTER_LEFT));
	if (simg->pal.SetTo(pal))
	{
		WriteUInt32(&pal[0], 0xFF000000);
		WriteUInt32(&pal[4], 0xFFFFFFFF);
	}
	UnsafeArray<UInt8> imgPtr = simg->data;
	UInt8 b;
	UIntOS i;
	UIntOS w = ((UIntOS)bitMatrix.width()) >> 3;
	UIntOS bpl = simg->GetDataBpl();
	UIntOS j = 0;
	UIntOS h = (UIntOS)bitMatrix.height();
	while (j < h)
	{
		i = 0;
		while (i < w)
		{
			b = bitMatrix.get((int)i * 8, (int)j)?0x0:0x80;
			if (!bitMatrix.get((int)i * 8 + 1, (int)j)) b = (UInt8)(b | 0x40);
			if (!bitMatrix.get((int)i * 8 + 2, (int)j)) b = (UInt8)(b | 0x20);
			if (!bitMatrix.get((int)i * 8 + 3, (int)j)) b = (UInt8)(b | 0x10);
			if (!bitMatrix.get((int)i * 8 + 4, (int)j)) b = (UInt8)(b | 0x8);
			if (!bitMatrix.get((int)i * 8 + 5, (int)j)) b = (UInt8)(b | 0x4);
			if (!bitMatrix.get((int)i * 8 + 6, (int)j)) b = (UInt8)(b | 0x2);
			if (!bitMatrix.get((int)i * 8 + 7, (int)j)) b = (UInt8)(b | 0x1);
			imgPtr[i] = b;
			i++;
		}
		imgPtr += bpl;
		j++;
	}
	return simg;
}
