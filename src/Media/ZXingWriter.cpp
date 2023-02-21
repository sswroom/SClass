#include "Stdafx.h"
#include "Media/ZXingWriter.h"

#include <ZXing/BitMatrix.h>
#include <ZXing/MultiFormatWriter.h>

Media::StaticImage *Media::ZXingWriter::GenQRCode(Text::CString content, UOSInt width, UOSInt height)
{
	ZXing::MultiFormatWriter writer(ZXing::BarcodeFormat::QRCode);
	std::string s((const char*)content.v, (size_t)content.leng);
	ZXing::BitMatrix bitMatrix = writer.encode(s, (int)width, (int) height);
	Media::StaticImage *simg;
	NEW_CLASS(simg, Media::StaticImage(width, height, 0, 1, Media::PixelFormat::PF_PAL_W1, 0, 0, Media::ColorProfile::YUVT_BT601, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
	WriteUInt32(&simg->pal[0], 0xFF000000);
	WriteUInt32(&simg->pal[4], 0xFFFFFFFF);
	UInt8 *imgPtr = simg->data;
	UInt8 b;
	UOSInt i;
	UOSInt w = ((UOSInt)bitMatrix.width()) >> 3;
	UOSInt bpl = simg->GetDataBpl();
	UOSInt j = 0;
	UOSInt h = (UOSInt)bitMatrix.height();
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
