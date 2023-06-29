#include "Stdafx.h"
#include "Media/ZXingReader.h"
#include "Text/MyString.h"

#include <ZXing/ReadBarcode.h>

Media::ZXingReader::ZXingReader()
{
}

Media::ZXingReader::~ZXingReader()
{
}

UTF8Char *Media::ZXingReader::ReadY8(UTF8Char *buff, const UInt8 *imgData, UOSInt width, UOSInt height, UOSInt lineSize)
{
	ZXing::ImageView imgView(imgData, (int)width, (int)height, ZXing::ImageFormat::Lum, (int)lineSize, 1);
	ZXing::Result result = ZXing::ReadBarcode(imgView);
	if (result.error().type() == ZXing::Error::Type::None)
	{
		ZXing::ByteArray s = result.bytes();
		return Text::StrConcatC(buff, s.data(), (UOSInt)s.size());
	}
	return 0;
}

UTF8Char *Media::ZXingReader::ReadImg(UTF8Char *buff, const Media::Image *simg)
{
	Media::StaticImage *tmpImg = simg->CreateStaticImage();
	tmpImg->To32bpp();
	ZXing::Result result;
	{
		ZXing::ImageView imgView(tmpImg->data, (int)tmpImg->info.dispSize.x, (int)tmpImg->info.dispSize.y, ZXing::ImageFormat::BGRX, (int)tmpImg->GetDataBpl(), 4);
		result = ZXing::ReadBarcode(imgView);
	}
	DEL_CLASS(tmpImg;)
	if (result.error().type() == ZXing::Error::Type::None)
	{
		ZXing::ByteArray s = result.bytes();
		return Text::StrConcatC(buff, s.data(), (UOSInt)s.size());
	}
	return 0;
	
}
