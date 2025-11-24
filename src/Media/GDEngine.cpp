#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "Math/Unit/Angle.h"
#include "Media/DrawEngine.h"
#include "Media/GDEngine.h"
#include "Sync/Event.h"
#include "Text/MyStringW.h"
#include <gd.h>

#define PI 3.1415926535898

int UCS2UTF8(WChar *buff, int buffSize, char *outBuff)
{
	int retVal = 0;
	WChar val;
	while (buffSize > 0)
	{
		buffSize -= 2;
		val = *buff++;
		if (val < 0x80)
		{
			*outBuff++ = (char)val;
			retVal++;
		}
		else if (val < 0x800)
		{
			outBuff[1] = (val & 0x3f) | 0x80;
			outBuff[0] = (val >> 6) | 0xc0;
			outBuff += 2;
			retVal += 2;
		}
		else
		{
			outBuff[2] = (val & 0x3f) | 0x80;
			val >>= 6;
			outBuff[1] = (val & 0x3f) | 0x80;
			outBuff[0] = (val >> 6) | 0xe0;
			outBuff += 3;
			retVal += 3;
		}
	}
	return retVal;
}

gdImagePtr GDELoadBitmap(int dataSize, void *dataPtr)
{
	UInt8 *imgData;
	UInt8 *hdr;
	UInt8 *pal;
	UInt8 *currPtr;
	Int32 imgWidth;
	Int32 imgHeight;
	Int32 bpp;
	imgData = (UInt8*)dataPtr;
	hdr = &imgData[0];

	if (*(Int16*)hdr != *(Int16*)"BM")
	{
		return 0;
	}
	if (*(Int32*)&hdr[14] == 40)
	{
		imgWidth = *(Int32*)&hdr[18];
		imgHeight = *(Int32*)&hdr[22];
		bpp = *(Int16*)&hdr[28];
		currPtr = &imgData[54];
	}
	else if (*(Int32*)&hdr[14] == 12)
	{
		imgWidth = *(Int16*)&hdr[18];
		imgHeight = *(Int16*)&hdr[20];
		bpp = *(Int16*)&hdr[24];
		currPtr = &imgData[26];
	}
	else
	{
		return 0;
	}
	gdImagePtr img = gdImageCreateTrueColor(imgWidth, imgHeight);
	if (img)
	{
		int **pixelData = img->tpixels;
		Int32 lineW;
		switch (bpp)
		{
		case 8:
			pal = currPtr;
			currPtr = &imgData[*(Int32*)&hdr[10]];
			lineW = imgWidth;
			if (lineW & 3)
			{
				lineW = lineW + 4 - (lineW & 3);
			}
			
			break;
		case 16:
			currPtr = &imgData[*(Int32*)&hdr[10]];
			//////////////////////////////////////////////////////////////////////
			break;
		case 24:
			currPtr = &imgData[*(Int32*)&hdr[10]];
			lineW = imgWidth * 3;
			if (lineW & 3)
			{
				lineW = lineW + 4 - (lineW & 3);
			}
			
			break;
		case 32:
			currPtr = &imgData[*(Int32*)&hdr[10]];
			break;
		default:
			gdImageDestroy(img);
			img = 0;
			break;
		}
	}
	else
	{
		img = 0;
	}
	return img;
}

Media::GDEngine::GDEngine()
{
}

Media::GDEngine::~GDEngine()
{
}

Optional<Media::DrawImage> Media::GDEngine::CreateImage32(Math::Size2D<UOSInt> size, Media::AlphaType atype)
{
	GDImage *img;
	gdImagePtr imgPtr = gdImageCreateTrueColor((int)size.GetWidth(), (int)size.GetHeight());
	if (imgPtr == 0)
		return 0;
	NEW_CLASS(img, GDImage(*this, size, 32, imgPtr));
	return img;
}

Optional<Media::DrawImage> Media::GDEngine::LoadImage(Text::CStringNN fileName)
{
	FILE *f;
	DrawImage *img;
	gdImagePtr imgPtr = 0;

	{
		IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (fs.IsError())
		{
			return 0;
		}
		UOSInt fileSize = (UOSInt)fs.GetLength();
		UnsafeArray<UInt8> dataPtr;
		dataPtr = MemAllocArr(UInt8, fileSize);
		if (fs.Read(Data::ByteArray(dataPtr, fileSize)) != fileSize)
		{
			MemFreeArr(dataPtr);
			return 0;
		}

		if (imgPtr == 0)
		{
			imgPtr = GDELoadBitmap((int)fileSize, dataPtr.Ptr());
		}
		if (imgPtr == 0)
		{
			imgPtr = gdImageCreateFromJpegPtr((int)fileSize, dataPtr.Ptr());
		}
		if (imgPtr == 0)
		{
			imgPtr = gdImageCreateFromPngPtr((int)fileSize, dataPtr.Ptr());
		}
		if (imgPtr == 0)
		{
			imgPtr = gdImageCreateFromGifPtr((int)fileSize, dataPtr.Ptr());
		}
		if (imgPtr == 0)
		{
			imgPtr = gdImageCreateFromGdPtr((int)fileSize, dataPtr.Ptr());
		}
		if (imgPtr == 0)
		{
			imgPtr = gdImageCreateFromGd2Ptr((int)fileSize, dataPtr.Ptr());
		}
		if (imgPtr == 0)
		{
			imgPtr = gdImageCreateFromWBMPPtr((int)fileSize, dataPtr.Ptr());
		}
		MemFreeArr(dataPtr);
	}

	if (imgPtr)
	{
		NEW_CLASS(img, GDImage(*this, Math::Size2D<UOSInt>(gdImageSX(imgPtr), gdImageSY(imgPtr)), 32, imgPtr));
		return img;
	}
	return 0;
}

Bool Media::GDEngine::DeleteImage(NN<DrawImage> img)
{
	NN<Media::GDImage> gimg = NN<GDImage>::ConvertFrom(img);
	gdImageDestroy(gimg->imgPtr.GetOpt<gdImage>().OrNull());
	gimg.Delete();
	return true;
}

int GDE_GetC(struct gdIOCtx *ctx)
{
	NN<IO::Stream> stm = *(NN<IO::Stream>*)&((char*)ctx)[-(int)sizeof(NN<IO::Stream>)];
	unsigned char buff = 0;
	stm->Read(Data::ByteArray(&buff, 1));
	return buff;
}

int GDE_GetBuf(struct gdIOCtx *ctx, void *buf, int size)
{
	NN<IO::Stream> stm = *(NN<IO::Stream>*)&((char*)ctx)[-(int)sizeof(NN<IO::Stream>)];
	return stm->Read(Data::ByteArray((UInt8*)buf, (UOSInt)size));
}

void GDE_PutC(struct gdIOCtx *ctx, int ch)
{
	NN<IO::Stream> stm = *(NN<IO::Stream>*)&((char*)ctx)[-(int)sizeof(NN<IO::Stream>)];
	stm->Write(Data::ByteArrayR((UInt8*)&ch, 1));
}

int GDE_PutBuf(struct gdIOCtx *ctx, const void *buf, int size)
{
	NN<IO::Stream> stm = *(NN<IO::Stream>*)&((char*)ctx)[-(int)sizeof(NN<IO::Stream>)];
	return stm->Write(Data::ByteArrayR((UInt8*)buf, size));
}

int GDE_Seek(struct gdIOCtx *ctx, const int pos)
{
	NN<IO::SeekableStream> stm = *(NN<IO::SeekableStream>*)&((char*)ctx)[-(int)sizeof(NN<IO::Stream>)];
	if (stm->SeekFromBeginning(pos) == pos)
		return 1;
	else
		return 0;
}

long GDE_Tell(struct gdIOCtx *ctx)
{
	NN<IO::SeekableStream> stm = *(NN<IO::SeekableStream>*)&((char*)ctx)[-(int)sizeof(NN<IO::Stream>)];
	return (long)stm->GetPosition();
}

void GDE_GD_Free(struct gdIOCtx *ctx)
{
}


void *Media::GDEngine::CreateIOCtx(NN<IO::SeekableStream> stm)
{
	UInt8 *obj = MemAlloc(UInt8, sizeof(NN<IO::SeekableStream>) + sizeof(gdIOCtx));
	gdIOCtx *io = (gdIOCtx*)&obj[sizeof(NN<IO::SeekableStream>)];
	*((NN<IO::SeekableStream>*)obj) = stm;

	io->getC = GDE_GetC;
	io->getBuf = GDE_GetBuf;
	io->putC = GDE_PutC;
	io->putBuf = GDE_PutBuf;
	io->seek = GDE_Seek;
	io->tell = GDE_Tell;
	io->gd_free = GDE_GD_Free;
	return io;
}

void Media::GDEngine::DeleteIOCtx(void *obj)
{
	void *o = &((char*)obj)[-(int)sizeof(NN<IO::SeekableStream>)];
	MemFree(o);
}

Media::GDBrush::GDBrush(UInt32 color, NN<Media::DrawImage> img)
{
	gdImagePtr im = NN<Media::GDImage>::ConvertFrom(img)->imgPtr.GetOpt<gdImage>().OrNull();
	this->color = gdImageColorAllocateAlpha(im, (color & 0xff0000) >> 16, (color & 0xff00) >> 8, (color & 0xff), ((color >> 25) & 0x7f) ^ 0x7f);
}

Media::GDBrush::~GDBrush()
{
}

Int32 Media::GDBrush::InitImage(NN<Media::DrawImage> img)
{
	return (Int32)this->color;
}

Media::GDPen::GDPen(UInt32 color, Double thick, UnsafeArrayOpt<UInt8> pattern, UOSInt nPattern, NN<DrawImage> img)
{
	gdImagePtr im = NN<Media::GDImage>::ConvertFrom(img)->imgPtr.GetOpt<gdImage>().OrNull();
	this->color = gdImageColorAllocateAlpha(im, (color & 0xff0000) >> 16, (color & 0xff00) >> 8, (color & 0xff), ((color >> 25) & 0x7f) ^ 0x7f);
	this->thick = thick;
	if (this->thick <= 1)
		this->thick = 1;
	UnsafeArray<UInt8> tmp;
	UnsafeArray<UInt8> tmp3;
	if (nPattern > 0 && pattern.SetTo(tmp))
	{
		tmp3 = tmp;
		Int32 i = nPattern;
		Int32 totalPixel = 0;
		while (i--)
			totalPixel += *tmp++;

		this->pattern = MemAlloc(int, totalPixel * this->thick);
		this->nPattern = totalPixel * this->thick;
		int *tmp2 = this->pattern;
		tmp = tmp3;

		while (nPattern-- > 0)
		{
			i = this->thick * *tmp++;
			while (i--)
			{
				*tmp2++ = this->color;
			}

			if (nPattern-- > 0)
			{
				i = this->thick * *tmp++;
				while (i--)
				{
					*tmp2++ = gdTransparent;
				}
			}
		}
	}
	else
	{
		this->pattern = 0;
		this->nPattern = 0;
	}
}

Media::GDPen::~GDPen()
{
	if (this->pattern)
	{
		MemFree(this->pattern);
		this->pattern = 0;
		this->nPattern = 0;
	}
}

Int32 Media::GDPen::InitImage(NN<DrawImage> img)
{
	gdImagePtr im = NN<Media::GDImage>::ConvertFrom(img)->imgPtr.GetOpt<gdImage>().OrNull();
	gdImageSetThickness(im, this->thick);
	if (this->nPattern > 0)
	{
		gdImageSetStyle(im, this->pattern, this->nPattern);
		return gdStyled;
	}
	else
	{
		return this->color;
	}
}

Double Media::GDPen::GetThick()
{
	return this->thick;
}

Media::GDFont::GDFont(Text::CStringNN name, Double pxSize, Media::DrawEngine::DrawFontStyle style)
{
	this->name = Text::String::New(name);
	this->pxSize = pxSize;
	this->style = style;
	this->ttcName = 0;
}

Media::GDFont::~GDFont()
{
	this->name->Release();
	OPTSTR_DEL(this->ttcName);
}

NN<Text::String> Media::GDFont::GetName() const
{
	return this->name;
}

NN<Text::String> Media::GDFont::GetTTCName()
{
	NN<Text::String> ttcName;
	if (this->ttcName.SetTo(ttcName))
		return ttcName;
	this->ttcName = ttcName = Text::String::New(this->name->leng + 4);
	Text::StrConcatC(Text::StrConcatC(ttcName->v, this->name->v, this->name->leng), UTF8STRC(".ttc"));
	return ttcName;
}

Double Media::GDFont::GetPXSize() const
{
	return this->pxSize;
}

Double Media::GDFont::GetPointSize() const
{
	return this->pxSize * 72.0 / 96.0;
}

Media::DrawEngine::DrawFontStyle Media::GDFont::GetFontStyle() const
{
	return this->style;
}

Media::GDImage::GDImage(NN<GDEngine> eng, Math::Size2D<UOSInt> size, UInt32 bitCount, AnyType imgPtr)
{
	this->eng = eng;
	this->width = size.GetWidth();
	this->height = size.GetHeight();
	this->bitCount = bitCount;
	this->imgPtr = imgPtr;
}

Media::GDImage::~GDImage()
{
}

UOSInt Media::GDImage::GetWidth() const
{
	return this->width;
}

UOSInt Media::GDImage::GetHeight() const
{
	return this->height;
}

Math::Size2D<UOSInt> Media::GDImage::GetSize() const
{
	return Math::Size2D<UOSInt>(this->width, this->height);
}

UInt32 Media::GDImage::GetBitCount() const
{
	return bitCount;
}

Bool Media::GDImage::DrawLine(Double x1, Double y1, Double x2, Double y2, NN<DrawPen> p)
{
	gdImagePtr im = this->imgPtr.GetOpt<gdImage>().OrNull();
	NN<GDPen> pen = NN<GDPen>::ConvertFrom(p);
	Int32 c = pen->InitImage(*this);
	gdImageLine(im, Double2Int32(x1), Double2Int32(y1), Double2Int32(x2), Double2Int32(y2), c);
	return true;
}

Bool Media::GDImage::DrawPolylineI(UnsafeArray<const Int32> points, UOSInt nPoints, NN<DrawPen> p)
{
	gdImagePtr im = this->imgPtr.GetOpt<gdImage>().OrNull();
	NN<GDPen> pen = NN<GDPen>::ConvertFrom(p);
	Int32 c = pen->InitImage(*this);
	gdImageOpenPolygon(im, (gdPointPtr)points.Ptr(), nPoints, c);
	return true;
}

Bool Media::GDImage::DrawPolygonI(UnsafeArray<const Int32> points, UOSInt nPoints, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	gdImagePtr im = this->imgPtr.GetOpt<gdImage>().OrNull();
	NN<GDPen> pen;
	NN<GDBrush> brush;
	Int32 c;
	if (Optional<GDBrush>::ConvertFrom(b).SetTo(brush))
	{
		c = brush->InitImage(*this);
		gdImageFilledPolygon(im, (gdPointPtr)points.Ptr(), nPoints, c);
	}
	if (Optional<GDPen>::ConvertFrom(p).SetTo(pen))
	{
		c = pen->InitImage(*this);
		gdImagePolygon(im, (gdPointPtr)points.Ptr(), nPoints, c);
	}
	return true;
}

Bool Media::GDImage::DrawPolyPolygonI(UnsafeArray<const Int32> points, UnsafeArray<const UInt32> pointCnt, UOSInt nPointCnt, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	gdImagePtr im = this->imgPtr.GetOpt<gdImage>().OrNull();
	NN<GDPen> pen;
	NN<GDBrush> brush;
	Int32 c;
	UOSInt i;
	UOSInt j;
	if (Optional<GDBrush>::ConvertFrom(b).SetTo(brush))
	{
		c = brush->InitImage(*this);
		i = 0;
		j = 0;
		while (i < nPointCnt)
		{
			gdImageFilledPolygon(im, (gdPointPtr)&points[j<<1], (Int32)pointCnt[i], c);
			j += pointCnt[i++];
		}
	}
	if (Optional<GDPen>::ConvertFrom(p).SetTo(pen))
	{
		c = pen->InitImage(*this);
		i = 0;
		j = 0;
		while (i < nPointCnt)
		{
			gdImagePolygon(im, (gdPointPtr)&points[j<<1], (Int32)pointCnt[i], c);
			j += pointCnt[i++];
		}
	}
	return true;
}

Bool Media::GDImage::DrawRect(Math::Coord2DDbl tl, Math::Size2DDbl size, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	gdImagePtr im = this->imgPtr.GetOpt<gdImage>().OrNull();
	NN<GDPen> pen;
	NN<GDBrush> brush;
	Int32 c;
	if (Optional<GDBrush>::ConvertFrom(b).SetTo(brush))
	{
		c = brush->InitImage(*this);
		gdImageFilledRectangle(im, Double2Int32(tl.x), Double2Int32(tl.y), Double2Int32(tl.x + size.GetWidth()), Double2Int32(tl.y + size.GetHeight()), c);
	}
	if (Optional<GDPen>::ConvertFrom(p).SetTo(pen))
	{
		c = pen->InitImage(*this);
		gdImageRectangle(im, Double2Int32(tl.x), Double2Int32(tl.y), Double2Int32(tl.x + size.GetWidth()), Double2Int32(tl.y + size.GetHeight()), c);
	}
	return true;
}

Bool Media::GDImage::DrawString(Math::Coord2DDbl tl, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b)
{
	gdImagePtr im = this->imgPtr.GetOpt<gdImage>().OrNull();
	NN<GDBrush> brush = NN<GDBrush>::ConvertFrom(b);
	NN<GDFont> fnt = NN<GDFont>::ConvertFrom(f);
	int brect[8];
	int c = brush->InitImage(*this);
	if (gdImageStringFT(im, brect, c, (const Char*)fnt->GetName().Ptr(), fnt->GetPointSize(), 0, Double2Int32(tl.x), Double2Int32(tl.y), (const Char*)str.v.Ptr()) != 0)
	{
		gdImageStringFT(im, brect, c, (const Char*)fnt->GetTTCName().Ptr(), fnt->GetPointSize(), 0, Double2Int32(tl.x), Double2Int32(tl.y), (const Char*)str.v.Ptr());
	}
	return true;
}

Bool Media::GDImage::DrawStringRot(Math::Coord2DDbl center, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegreeACW)
{
	gdImagePtr im = this->imgPtr.GetOpt<gdImage>().OrNull();
	NN<GDBrush> brush = NN<GDBrush>::ConvertFrom(b);
	NN<GDFont> fnt = NN<GDFont>::ConvertFrom(f);
	int brect[8];
	int c = brush->InitImage(*this);
	double rad = Math::Unit::Angle::Convert(Math::Unit::Angle::AU_DEGREE, Math::Unit::Angle::AU_RADIAN, angleDegreeACW);
	int tlx;
	int tly;
	Char *retV = gdImageStringFT(0, brect, 0, (const Char*)fnt->GetName().Ptr(), fnt->GetPointSize(), rad, 0, 0, (const Char*)str->v.Ptr());
	if (retV != 0)
	{
		gdImageStringFT(0, brect, 0, (const Char*)fnt->GetTTCName().Ptr(), fnt->GetPointSize(), rad, 0, 0, (const Char*)str->v.Ptr());
	}
	tlx = ((brect[4] + brect[0]) >> 1);
	tly = ((brect[5] + brect[1]) >> 1);

	retV = gdImageStringFT(im, brect, c, (const Char*)fnt->GetName().Ptr(), fnt->GetPointSize(), rad, Double2Int32(center.x) - tlx, Double2Int32(center.y) - tly, (const Char*)str->v.Ptr());
	return retV == 0;
}

Bool Media::GDImage::DrawImagePt(NN<DrawImage> img, Math::Coord2DDbl tl)
{
	gdImageCopy(this->imgPtr.GetOpt<gdImage>().OrNull(), NN<GDImage>::ConvertFrom(img)->imgPtr.GetOpt<gdImage>().OrNull(), Double2Int32(tl.x), Double2Int32(tl.y), 0, 0, Double2Int32(NN<GDImage>::ConvertFrom(img)->GetWidth()), Double2Int32(NN<GDImage>::ConvertFrom(img)->GetHeight()));
	return true;
}

NN<Media::DrawPen> Media::GDImage::NewPenARGB(UInt32 color, Double thick, UnsafeArrayOpt<UInt8> pattern, UOSInt nPattern)
{
	NN<GDPen> p;
	NEW_CLASSNN(p, GDPen(color, thick, pattern, nPattern, *this));
	return p;
}

NN<Media::DrawBrush> Media::GDImage::NewBrushARGB(UInt32 color)
{
	NN<GDBrush> b;
	NEW_CLASSNN(b, GDBrush(color, *this));
	return b;
}

NN<Media::DrawFont> Media::GDImage::NewFontPt(Text::CStringNN name, Double ptSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage)
{
	NN<GDFont> f;
	NEW_CLASSNN(f, GDFont(name, ptSize * 96 / 72, fontStyle));
	return f;
}

NN<Media::DrawFont> Media::GDImage::NewFontPx(Text::CStringNN name, Double pxSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage)
{
	NN<GDFont> f;
	NEW_CLASSNN(f, GDFont(name, pxSize, fontStyle));
	return f;
}

void Media::GDImage::DelPen(NN<DrawPen> p)
{
	NN<GDPen>::ConvertFrom(p).Delete();
}

void Media::GDImage::DelBrush(NN<DrawBrush> b)
{
	NN<GDBrush>::ConvertFrom(b).Delete();
}

void Media::GDImage::DelFont(NN<DrawFont> f)
{
	NN<GDFont>::ConvertFrom(f).Delete();
}

Math::Size2DDbl Media::GDImage::GetTextSize(NN<DrawFont> fnt, Text::CStringNN txt)
{
	NN<GDFont> f = NN<GDFont>::ConvertFrom(fnt);
	int brect[8];
	Char *ret = gdImageStringFT(0, brect, 0, (const Char*)f->GetName()->v.Ptr(), f->GetPointSize(), 0, 0, 0, (const Char*)txt.v.Ptr());
	if (ret != 0)
	{
		ret = gdImageStringFT(0, brect, 0, (const Char*)f->GetTTCName()->v.Ptr(), f->GetPointSize(), 0, 0, 0, (const Char*)txt.v.Ptr());
		if (ret != 0)
		{
			return Math::Size2DDbl(0, 0);
		}
	}
	return Math::Size2DDbl(brect[2] - brect[6], brect[3] - brect[7]);
}

UOSInt Media::GDImage::SavePng(NN<IO::SeekableStream> stm)
{
	gdIOCtxPtr ptr = (gdIOCtxPtr)eng->CreateIOCtx(stm);
	gdImagePngCtx(this->imgPtr.GetOpt<gdImage>().OrNull(), ptr);
	eng->DeleteIOCtx(ptr);
	return false;
}

