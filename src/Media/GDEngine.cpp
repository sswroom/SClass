#include "stdafx.h"
#include "MyMemory.h"
#include "Sync/Event.h"
#include "IO/Stream.h"
#include "Media/DrawEngine.h"
#include "Media/GDEngine.h"
#include "Text/Encoding.h"
#include "gd.h"

#ifdef WIN32
#include <share.h>
#endif

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
			
			_asm
			{
				mov edi,pixelData
				mov eax,imgHeight
				shl eax,2
				add edi,eax
				mov esi,currPtr
				mov ebx,pal
				mov ecx,imgHeight
				cld
glb8lop2:
				sub edi,4
				mov eax,dword ptr [edi]
				push edi
				mov edi,eax
				mov eax,0

				push ecx
				push esi
				mov ecx,imgWidth
glb8lop:
				lodsb
				mov edx,dword ptr [ebx+eax*4]
				or edx,0x7f000000
				mov dword ptr [edi],edx
				add edi,4
				dec ecx
				jnz glb8lop

				pop esi
				pop ecx
				add esi,lineW
				pop edi
				dec ecx
				jnz glb8lop2

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
			
			_asm
			{
				mov esi,currPtr
				mov edx,pixelData
				mov eax,imgHeight
				shl eax,2
				add edx,eax

				mov al,0x7f
				mov ebx,imgHeight
				cld
glb24lop2:
				sub edx,4
				mov edi,dword ptr [edx]

				push esi
				mov ecx,imgWidth

glb24lop:
				movsw
				movsb
				stosb
				dec ecx
				jnz glb24lop

				pop esi
				add esi,lineW
				dec ebx
				jnz glb24lop2
			}
			break;
		case 32:
			currPtr = &imgData[*(Int32*)&hdr[10]];
			_asm
			{
				mov edx,pixelData
				mov eax,imgHeight
				shl eax,2
				add edx,eax
				mov esi,currPtr
				mov ecx,imgHeight
				cld
glb32lop2:
				sub edx,4
				mov edi,dword ptr [edx]
				mov ebx,imgWidth
glb32lop:
				movsw
				movsb
				lodsb
				shr al,1
				xor al,0x7f
				stosb
				dec ebx
				jnz glb32lop

				dec ecx
				jnz glb32lop2
			}
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

Media::DrawImage *Media::GDEngine::CreateImage32(Int32 width, Int32 height)
{
	GDImage *img;
	gdImagePtr imgPtr = gdImageCreateTrueColor(width, height);
	if (imgPtr == 0)
		return 0;
	NEW_CLASS(img, GDImage(this, width, height, 32, imgPtr));
	return img;
}

Media::DrawImage *Media::GDEngine::LoadImageA(Char *fileName)
{
	FILE *f;
	DrawImage *img;
	gdImagePtr imgPtr = 0;
#if 0//WIN32
	f = _fsopen(fileName, "rb", _SH_DENYWR);
#else
	f = fopen(fileName, "rb");
#endif

	if (f)
	{
		void *dataPtr;
		UInt32 dataSize;
		dataSize = fseek(f, 0, SEEK_END);
		dataSize = ftell(f);
		fseek(f, 0, SEEK_SET);
		dataPtr = MemAlloc(dataSize);
		fread(dataPtr, 1, dataSize, f);
		fclose(f);

		if (imgPtr == 0)
		{
			imgPtr = GDELoadBitmap(dataSize, dataPtr);
		}
		if (imgPtr == 0)
		{
			imgPtr = gdImageCreateFromJpegPtr(dataSize, dataPtr);
		}
		if (imgPtr == 0)
		{
			imgPtr = gdImageCreateFromPngPtr(dataSize, dataPtr);
		}
		if (imgPtr == 0)
		{
			imgPtr = gdImageCreateFromGifPtr(dataSize, dataPtr);
		}
		if (imgPtr == 0)
		{
			imgPtr = gdImageCreateFromGdPtr(dataSize, dataPtr);
		}
		if (imgPtr == 0)
		{
			imgPtr = gdImageCreateFromGd2Ptr(dataSize, dataPtr);
		}
		if (imgPtr == 0)
		{
			imgPtr = gdImageCreateFromWBMPPtr(dataSize, dataPtr);
		}
		MemFree(dataPtr);
	}

	if (imgPtr)
	{
		NEW_CLASS(img, GDImage(this, gdImageSX(imgPtr), gdImageSY(imgPtr), 32, imgPtr));
		return img;
	}
	return 0;
}

Media::DrawImage *Media::GDEngine::LoadImageW(WChar *fileName)
{
	FILE *f;
	DrawImage *img;
	gdImagePtr imgPtr = 0;
#if 0//WIN32
	f = _wfsopen(fileName, L"rb", _SH_DENYWR);
#else
	f = _wfopen(fileName, L"rb");
#endif

	if (f)
	{
		void *dataPtr;
		UInt32 dataSize;
		dataSize = fseek(f, 0, SEEK_END);
		dataSize = ftell(f);
		fseek(f, 0, SEEK_SET);
		dataPtr = MemAlloc(dataSize);
		fread(dataPtr, 1, dataSize, f);
		fclose(f);

		if (imgPtr == 0)
		{
			imgPtr = GDELoadBitmap(dataSize, dataPtr);
		}
		if (imgPtr == 0)
		{
			imgPtr = gdImageCreateFromJpegPtr(dataSize, dataPtr);
		}
		if (imgPtr == 0)
		{
			imgPtr = gdImageCreateFromPngPtr(dataSize, dataPtr);
		}
		if (imgPtr == 0)
		{
			imgPtr = gdImageCreateFromGifPtr(dataSize, dataPtr);
		}
		if (imgPtr == 0)
		{
			imgPtr = gdImageCreateFromGdPtr(dataSize, dataPtr);
		}
		if (imgPtr == 0)
		{
			imgPtr = gdImageCreateFromGd2Ptr(dataSize, dataPtr);
		}
		if (imgPtr == 0)
		{
			imgPtr = gdImageCreateFromWBMPPtr(dataSize, dataPtr);
		}
		MemFree(dataPtr);
	}

	if (imgPtr)
	{
		NEW_CLASS(img, GDImage(this, gdImageSX(imgPtr), gdImageSY(imgPtr), 32, imgPtr));
		return img;
	}
	return 0;
}

Bool Media::GDEngine::DeleteImage(DrawImage *img)
{
	Media::GDImage *gimg = (GDImage*)img;
	gdImageDestroy((gdImagePtr)gimg->imgPtr);
	DEL_CLASS((GDImage *)img);
	return true;
}

int GDE_GetC(struct gdIOCtx *ctx)
{
	IO::Stream *stm = *(IO::Stream**)&((char*)ctx)[-(int)sizeof(IO::Stream*)];
	unsigned char buff = 0;
	stm->Read(&buff, 1);
	return buff;
}

int GDE_GetBuf(struct gdIOCtx *ctx, void *buf, int size)
{
	IO::Stream *stm = *(IO::Stream**)&((char*)ctx)[-(int)sizeof(IO::Stream*)];
	return stm->Read((UInt8*)buf, size);
}

void GDE_PutC(struct gdIOCtx *ctx, int ch)
{
	IO::Stream *stm = *(IO::Stream**)&((char*)ctx)[-(int)sizeof(IO::Stream*)];
	stm->Write((UInt8*)&ch, 1);
}

int GDE_PutBuf(struct gdIOCtx *ctx, const void *buf, int size)
{
	IO::Stream *stm = *(IO::Stream**)&((char*)ctx)[-(int)sizeof(IO::Stream*)];
	return stm->Write((UInt8*)buf, size);
}

int GDE_Seek(struct gdIOCtx *ctx, const int pos)
{
	IO::Stream *stm = *(IO::Stream**)&((char*)ctx)[-(int)sizeof(IO::Stream*)];
	if (stm->Seek(IO::Stream::Begin, pos) == pos)
		return 1;
	else
		return 0;
}

long GDE_Tell(struct gdIOCtx *ctx)
{
	IO::Stream *stm = *(IO::Stream**)&((char*)ctx)[-(int)sizeof(IO::Stream*)];
	return (long)stm->Seek(IO::Stream::Current, 0);
}

void GDE_GD_Free(struct gdIOCtx *ctx)
{
}


void *Media::GDEngine::CreateIOCtx(IO::Stream *stm)
{
	void *obj = MemAlloc(sizeof(IO::Stream*) + sizeof(gdIOCtx));
	gdIOCtx *io = (gdIOCtx*)&((char*)obj)[sizeof(IO::Stream*)];
	*((IO::Stream**)obj) = stm;

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
	void *o = &((char*)obj)[-(int)sizeof(IO::Stream*)];
	MemFree(o);
}

Media::GDBrush::GDBrush(Int32 color, Media::DrawImage *img)
{
	gdImagePtr im = (gdImagePtr) ((Media::GDImage*)img)->imgPtr;
	this->color = gdImageColorAllocateAlpha(im, (color & 0xff0000) >> 16, (color & 0xff00) >> 8, (color & 0xff), ((color >> 25) & 0x7f) ^ 0x7f);
}

Media::GDBrush::~GDBrush()
{
}

Int32 Media::GDBrush::InitImage(Media::DrawImage *img)
{
	return this->color;
}

Media::GDPen::GDPen(Int32 color, Int32 thick, UInt8 *pattern, Int32 nPattern, DrawImage *img)
{
	gdImagePtr im = (gdImagePtr) ((GDImage*)img)->imgPtr;
	this->color = gdImageColorAllocateAlpha(im, (color & 0xff0000) >> 16, (color & 0xff00) >> 8, (color & 0xff), ((color >> 25) & 0x7f) ^ 0x7f);
	this->thick = thick;
	if (this->thick <= 1)
		this->thick = 1;
	if (nPattern > 0)
	{
		UInt8 *tmp = pattern;
		Int32 i = nPattern;
		Int32 totalPixel = 0;
		while (i--)
			totalPixel += *tmp++;

		this->pattern = (int*)MemAlloc(totalPixel * this->thick * sizeof(int));
		this->nPattern = totalPixel * this->thick;
		int *tmp2 = this->pattern;
		tmp = pattern;

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

Int32 Media::GDPen::InitImage(DrawImage *img)
{
	gdImagePtr im = (gdImagePtr)((GDImage*)img)->imgPtr;
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

Media::GDFont::GDFont(Char *name, Int16 pxSize, Int16 style)
{
	Char *tmp = name;
	while (*tmp++);
	tmp = this->name = (Char*)MemAlloc((Int32)(tmp - name + 4));
	while (*tmp++ = *name++);
	this->pxSize = pxSize;
	this->style = style;
	this->isTTCName = false;
}

Media::GDFont::GDFont(WChar *name, Int16 pxSize, Int16 style)
{
	Text::Encoding enc(65001);
	WChar *tmp = name;
	while (*tmp++);
	this->name = (Char*)MemAlloc(enc.CountBytes(name, tmp - name - 1) + 1);
	this->name[enc.ToBytes((UInt8*)this->name, name, tmp - name - 1)] = 0;
	this->pxSize = pxSize;
	this->style = style;
	this->isTTCName = false;
}

Media::GDFont::~GDFont()
{
	MemFree(this->name);
}

Char *Media::GDFont::GetName()
{
	return this->name; 
}

Char *Media::GDFont::GetTTCName()
{
	if (this->isTTCName)
		return this->name;
	this->isTTCName = true;
	Char *dest = this->name;
	Char *src;
	while (*dest++);
	dest--;
	src = ".ttc";
	while (*dest++ = *src++);
	return this->name; 
}

Int16 Media::GDFont::GetPointSize()
{
	return this->pxSize;
}

Int16 Media::GDFont::GetFontStyle()
{
	return this->style;
}

Media::GDImage::GDImage(GDEngine *eng, Int32 width, Int32 height, Int32 bitCount, void *imgPtr)
{
	this->eng = eng;
	this->width = width;
	this->height = height;
	this->bitCount = bitCount;
	this->imgPtr = imgPtr;
}

Media::GDImage::~GDImage()
{
}

Int32 Media::GDImage::GetWidth()
{
	return width;
}

Int32 Media::GDImage::GetHeight()
{
	return height;
}

Int32 Media::GDImage::GetBitCount()
{
	return bitCount;
}

Bool Media::GDImage::DrawLine(Int32 x1, Int32 y1, Int32 x2, Int32 y2, DrawPen *p)
{
	gdImagePtr im = (gdImagePtr)this->imgPtr;
	GDPen *pen = (GDPen*)p;
	Int32 c = pen->InitImage(this);
	gdImageLine(im, x1, y1, x2, y2, c);
	return true;
}

Bool Media::GDImage::DrawPolyline(Int32 *points, Int32 nPoints, DrawPen *p)
{
	gdImagePtr im = (gdImagePtr)this->imgPtr;
	GDPen *pen = (GDPen*)p;
	Int32 c = pen->InitImage(this);
	gdImageOpenPolygon(im, (gdPointPtr)points, nPoints, c);
	return true;
}

Bool Media::GDImage::DrawPolygon(Int32 *points, Int32 nPoints, DrawPen *p, DrawBrush *b)
{
	gdImagePtr im = (gdImagePtr)this->imgPtr;
	GDPen *pen = (GDPen*)p;
	GDBrush *brush = (GDBrush*)b;
	Int32 c;
	if (b)
	{
		c = brush->InitImage(this);
		gdImageFilledPolygon(im, (gdPointPtr)points, nPoints, c);
	}
	if (p)
	{
		c = pen->InitImage(this);
		gdImagePolygon(im, (gdPointPtr)points, nPoints, c);
	}
	return true;
}

Bool Media::GDImage::DrawPolyPolygon(Int32 *points, Int32 *pointCnt, Int32 nPointCnt, DrawPen *p, DrawBrush *b)
{
	gdImagePtr im = (gdImagePtr)this->imgPtr;
	GDPen *pen = (GDPen*)p;
	GDBrush *brush = (GDBrush*)b;
	Int32 c;
	Int32 i;
	Int32 j;
	if (b)
	{
		c = brush->InitImage(this);
		i = 0;
		j = 0;
		while (i < nPointCnt)
		{
			gdImageFilledPolygon(im, (gdPointPtr)&points[j<<1], pointCnt[i], c);
			j += pointCnt[i++];
		}
	}
	if (p)
	{
		c = pen->InitImage(this);
		i = 0;
		j = 0;
		while (i < nPointCnt)
		{
			gdImagePolygon(im, (gdPointPtr)&points[j<<1], pointCnt[i], c);
			j += pointCnt[i++];
		}
	}
	return true;
}

Bool Media::GDImage::DrawRect(Int32 x, Int32 y, Int32 w, Int32 h, DrawPen *p, DrawBrush *b)
{
	gdImagePtr im = (gdImagePtr)this->imgPtr;
	GDPen *pen = (GDPen*)p;
	GDBrush *brush = (GDBrush*)b;
	Int32 c;
	if (b)
	{
		c = brush->InitImage(this);
		gdImageFilledRectangle(im, x, y, x + w, y + h, c);
	}
	if (p)
	{
		c = pen->InitImage(this);
		gdImageRectangle(im, x, y, x + w, y + h, c);
	}
	return true;
}

Bool Media::GDImage::DrawStringW(Int32 tlx, Int32 tly, WChar *str, DrawFont *f, DrawBrush *p)
{
	Char utf8Str[256];
	WChar *src = str;
	while (*src++);
	utf8Str[UCS2UTF8(str, (Int32)((src - str - 1) << 1), utf8Str)] = 0;
	gdImagePtr im = (gdImagePtr)this->imgPtr;
	GDBrush *b = (GDBrush*)p;
	GDFont *fnt = (GDFont*)f;
	int brect[8];
	int c = b->InitImage(this);

	Int32 x;
	Int32 y;
	Char *retV = gdImageStringFT(0, brect, 0, fnt->GetName(), fnt->GetPointSize(), 0, 0, 0, utf8Str);
	if (retV != 0)
	{
		retV = gdImageStringFT(0, brect, 0, fnt->GetTTCName(), fnt->GetPointSize(), 0, 0, 0, utf8Str);
	}
	x = tlx - brect[6];
	y = tly - brect[7];

	retV = gdImageStringFT(im, brect, c, fnt->GetName(), fnt->GetPointSize(), 0, x, y, utf8Str);
	return true;
}

Bool Media::GDImage::DrawStringRotW(Int32 centX, Int32 centY, WChar *str, DrawFont *f, DrawBrush *p, Int32 angleDegree)
{
	Char utf8Str[256];
	WChar *src = str;
	while (*src++);
	utf8Str[UCS2UTF8(str, (Int32)((src - str - 1) << 1), utf8Str)] = 0;
	gdImagePtr im = (gdImagePtr)this->imgPtr;
	GDBrush *b = (GDBrush*)p;
	GDFont *fnt = (GDFont*)f;
	int brect[8];
	int c = b->InitImage(this);
	double rad = angleDegree * PI / 180.0;
	int tlx;
	int tly;
	Char *retV = gdImageStringFT(0, brect, 0, fnt->GetName(), fnt->GetPointSize(), rad, 0, 0, utf8Str);
	if (retV != 0)
	{
		retV = gdImageStringFT(0, brect, 0, fnt->GetTTCName(), fnt->GetPointSize(), rad, 0, 0, utf8Str);
	}
	tlx = ((brect[4] + brect[0]) >> 1);
	tly = ((brect[5] + brect[1]) >> 1);

	retV = gdImageStringFT(im, brect, c, fnt->GetName(), fnt->GetPointSize(), rad, centX - tlx, centY - tly, utf8Str);
	return retV == 0;
}

Bool Media::GDImage::DrawImagePt(DrawImage *img, Int32 tlx, Int32 tly)
{
	gdImageCopy((gdImagePtr)this->imgPtr, (gdImagePtr)((GDImage*)img)->imgPtr, tlx, tly, 0, 0, ((GDImage*)img)->GetWidth(), ((GDImage*)img)->GetHeight());
	return true;
}

Bool Media::GDImage::DrawString(Int32 tlx, Int32 tly, Char *utf8Str, DrawFont *f, DrawBrush *p)
{
	gdImagePtr im = (gdImagePtr)this->imgPtr;
	GDBrush *b = (GDBrush*)p;
	GDFont *fnt = (GDFont*)f;
	int brect[8];
	int c = b->InitImage(this);
	if (gdImageStringFT(im, brect, c, fnt->GetName(), fnt->GetPointSize(), 0, tlx, tly, utf8Str) != 0)
	{
		gdImageStringFT(im, brect, c, fnt->GetTTCName(), fnt->GetPointSize(), 0, tlx, tly, utf8Str);
	}
	return true;
}

Bool Media::GDImage::DrawStringRot(Int32 centX, Int32 centY, Char *utf8Str, DrawFont *f, DrawBrush *p, Int32 angleDegree)
{
	gdImagePtr im = (gdImagePtr)this->imgPtr;
	GDBrush *b = (GDBrush*)p;
	GDFont *fnt = (GDFont*)f;
	int brect[8];
	int c = b->InitImage(this);
	double rad = angleDegree * PI / 180.0;
	int tlx;
	int tly;
	Char *retV = gdImageStringFT(0, brect, 0, fnt->GetName(), fnt->GetPointSize(), rad, 0, 0, utf8Str);
	if (retV != 0)
	{
		gdImageStringFT(0, brect, 0, fnt->GetTTCName(), fnt->GetPointSize(), rad, 0, 0, utf8Str);
	}
	tlx = ((brect[4] + brect[0]) >> 1);
	tly = ((brect[5] + brect[1]) >> 1);

	retV = gdImageStringFT(im, brect, c, fnt->GetName(), fnt->GetPointSize(), rad, centX - tlx, centY - tly, utf8Str);
	return retV == 0;
}

Media::DrawPen *Media::GDImage::NewPenARGB(Int32 color, Int32 thick, UInt8 *pattern, Int32 nPattern)
{
	GDPen *p;
	NEW_CLASS(p, GDPen(color, thick, pattern, nPattern, this));
	return p;
}

Media::DrawBrush *Media::GDImage::NewBrushARGB(Int32 color)
{
	GDBrush *b;
	NEW_CLASS(b, GDBrush(color, this));
	return b;
}

Media::DrawFont *Media::GDImage::NewFontA(Char *name, Int16 pxSize, Int16 fontStyle)
{
	GDFont *f;
	NEW_CLASS(f, GDFont(name, pxSize, fontStyle));
	return f;
}

Media::DrawFont *Media::GDImage::NewFontW(WChar *name, Int16 pxSize, Int16 fontStyle)
{
	GDFont *f;
	NEW_CLASS(f, GDFont(name, pxSize, fontStyle));
	return f;
}

void Media::GDImage::DelPen(DrawPen *p)
{
	DEL_CLASS((GDPen*)p);
}

void Media::GDImage::DelBrush(DrawBrush *b)
{
	DEL_CLASS((GDBrush*)b);
}

void Media::GDImage::DelFont(DrawFont *f)
{
	DEL_CLASS((GDFont*)f);
}

Bool Media::GDImage::GetTextSize(DrawFont *fnt, WChar *txt, Int32 txtLen, Int32 *sz)
{
	/////////////////////////////////
	GDFont *f = (GDFont*)fnt;
	int brect[8];
	Text::Encoding enc(65001);
	UInt8 bytes[512];
	Int32 retSize;
	bytes[retSize = enc.ToBytes(bytes, txt, txtLen)] = 0;
	Char *name = f->GetName();
	if (retSize > 511)
	{
		_asm int 3;
	}
	Char *ret = gdImageStringFT(0, brect, 0, name, f->GetPointSize(), 0, 0, 0, (Char*)bytes);
	if (ret != 0)
	{
			sz[0] = 0;
			sz[1] = 0;
			return 0;
		ret = gdImageStringFT(0, brect, 0, f->GetTTCName(), f->GetPointSize(), 0, 0, 0, (Char*)bytes);
		if (ret != 0)
		{
			sz[0] = 0;
			sz[1] = 0;
			return 0;
		}
	}
	sz[0] = brect[2] - brect[6];
	sz[1] = brect[3] - brect[7];
	return 1;
}

Int32 Media::GDImage::SavePng(IO::SeekableStream *stm)
{
	gdIOCtxPtr ptr = (gdIOCtxPtr)eng->CreateIOCtx(stm);
	gdImagePngCtx((gdImagePtr)this->imgPtr, ptr);
	eng->DeleteIOCtx(ptr);
	return false;
}

