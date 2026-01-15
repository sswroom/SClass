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

UnsafeArrayOpt<UTF8Char> Media::ZXingReader::ReadY8(UnsafeArray<UTF8Char> buff, UnsafeArray<const UInt8> imgData, UIntOS width, UIntOS height, UIntOS lineSize) const
{
	ZXing::ImageView imgView(imgData.Ptr(), (int)width, (int)height, ZXing::ImageFormat::Lum, (int)lineSize, 1);
	ZXing::Result result = ZXing::ReadBarcode(imgView);
	if (result.error().type() == ZXing::Error::Type::None)
	{
		ZXing::ByteArray s = result.bytes();
		return Text::StrConcatC(buff, s.data(), (UIntOS)s.size());
	}
	return 0;
}

UnsafeArrayOpt<UTF8Char> Media::ZXingReader::ReadImg(UnsafeArray<UTF8Char> buff, NN<const Media::RasterImage> simg) const
{
	NN<Media::StaticImage> tmpImg = simg->CreateStaticImage();
	tmpImg->ToB8G8R8A8();
	ZXing::Result result;
	{
		ZXing::ImageView imgView(tmpImg->data.Ptr(), (int)tmpImg->info.dispSize.x, (int)tmpImg->info.dispSize.y, ZXing::ImageFormat::BGRX, (int)tmpImg->GetDataBpl(), 4);
		result = ZXing::ReadBarcode(imgView);
	}
	tmpImg.Delete();
	if (result.error().type() == ZXing::Error::Type::None)
	{
		ZXing::ByteArray s = result.bytes();
		return Text::StrConcatC(buff, s.data(), (UIntOS)s.size());
	}
	return 0;
}
