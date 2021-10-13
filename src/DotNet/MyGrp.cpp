#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/Resizer/LanczosResizer8_C8.h"
#include "Media/Resizer/LanczosResizerH13_8.h"
#include "Media/ImageList.h"
#include "Media/StaticImage.h"
#include "IO/FileStream.h"
#include "IO/StmData/FileData.h"
#include "IO/ParsedObject.h"
#include "Parser/FullParserList.h"

#ifdef WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <gdiplus.h>

#include "Win32/COMStream.h"

#undef LoadImage

#include "DotNet/MyGrp.h"

void DotNet::MyGrp::GrpAdjustColor64(void *pSrc, void *pDest, long width, long height, long lineW, long bright, long cont)
{
	register char *src;
	register char *dest;
	register long i;
	register long j;
	long lineAdd = lineW - (width << 3);
	double r;
	double g;
	double b;
	i = height;
	while (i--)
	{
		j = width;
		while (j--)
		{

			r = *(unsigned short*)&src[0];
			g = *(unsigned short*)&src[2];
			b = *(unsigned short*)&src[4];

			r = r * cont + (bright * 655.35 - 65535);
			g = g * cont + (bright * 655.35 - 65535);
			b = b * cont + (bright * 655.35 - 65535);

			if (r < 0)
				r = 0;
			if (g < 0)
				g = 0;
			if (b < 0)
				b = 0;

			if ((r >= g) && (r >= b))
			{
				if (r > 65535.0)
				{
					g = g * 65535.0 / r;
					b = b * 65535.0 / r;
					r = 65535.0;
				}

			}
			else if ((g >= r) && (g >= b))
			{
				if (g > 65535.0)
				{
					r = r * 65535.0 / g;
					b = b * 65535.0 / g;
					g = 65535.0;
				}
			}
			else
			{
				if (b > 65535.0)
				{
					r = r * 65535.0 / b;
					g = g * 65535.0 / b;
					b = 65535.0;
				}
			}
			*(unsigned short*)&dest[0] = (unsigned short)r;
			*(unsigned short*)&dest[2] = (unsigned short)g;
			*(unsigned short*)&dest[4] = (unsigned short)b;
			*(unsigned short*)&dest[6] = *(unsigned short*)&src[6];

			src += 8;
			dest += 8;
		}
		src += lineAdd;
		dest += lineAdd;
	}
}

void DotNet::MyGrp::GrpCopyImageData(void *img, void *imgData)
{
	Media::ImageList *imgList = (Media::ImageList*)img;
	Media::StaticImage *i = (Media::StaticImage*)imgList->GetImage(0, 0);
	memcpy(imgData, i->data, i->info->width * i->info->height * 4);
}

void DotNet::GrpCropPtr32(void *pSrc, long srcX, long srcY, long srcLineW, void *pDest, long destW, long destH, long destLineW)
{
	_asm
	{
		mov esi,pSrc
		mov edi,pDest
		mov eax,srcLineW
		imul srcY
		add esi,eax
		mov eax,srcX
		shl eax,2
		add esi,eax
		mov ebx,destH
		cld
gcp32lop:
		push edi
		push esi
		mov ecx,destW
		rep movsd
		pop esi
		pop edi
		add esi,srcLineW
		add edi,destLineW
		dec ebx
		jnz gcp32lop
	}
}

void DotNet::MyGrp::GrpDelImage(void *img)
{
	DEL_CLASS((Media::ImageList*)img);
}

void *DotNet::MyGrp::GrpLoadImage32(WChar *fileName, Int32 *width, Int32 *height)
{
	::IO::StmData::FileData *fd;
	::IO::ParsedObject::ParserType pt;
	NEW_CLASS(fd, IO::StmData::FileData(fileName, false));
	if (fd->GetDataSize() <= 0)
	{
		DEL_CLASS(fd);
		return false;
	}
	IO::ParsedObject *obj = parsers->ParseFile(fd, &pt);
	DEL_CLASS(fd);
	if (obj == 0)
		return false;
	if (obj->GetParserType() != IO::ParsedObject::PT_IMAGE_LIST_PARSER)
	{
		DEL_CLASS(obj);
		return false;
	}
	Media::ImageList *imgList = (Media::ImageList*)obj;
	imgList->ToStaticImage(0);
	Media::StaticImage *img = (Media::StaticImage*)imgList->GetImage(0, 0);
	if (!img->To32bpp())
	{
		DEL_CLASS(imgList);
		return 0;
	}
	*width = img->info->width;
	*height = img->info->height;
	return imgList;
}

void DotNet::MyGrp::GrpResizePtr32_32(void *pSrc, long srcW, long srcH, void *pDest, long destW, long destH)
{
	resizer8->Resize((UInt8*)pSrc, srcW << 2, srcW, srcH, 0, 0, (UInt8*)pDest, destW << 2, destW, destH);

	return;
}

int DotNet::MyGrp::GetEncoderClsid(const WChar* format, void* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure

	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(MAlloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*(CLSID*)pClsid = pImageCodecInfo[j].Clsid;
			MemFree(pImageCodecInfo);
			return j;  // Success
		}    
	}

	MemFree(pImageCodecInfo);
	return -1;  // Failure
}

void *DotNet::MyGrp::GDIPImageFromNet(System::Drawing::Bitmap *bmp)
{
	Gdiplus::Rect rect;
	Gdiplus::BitmapData bdd;
	rect.X = 0;
	rect.Y = 0;
	rect.Width = bmp->Width;
	rect.Height = bmp->Height;
	Gdiplus::Bitmap *nbmp = new Gdiplus::Bitmap(bmp->Width, bmp->Height);
	System::Drawing::Imaging::BitmapData *bds;
	if (nbmp->LockBits(&rect, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bdd) != Gdiplus::Ok)
	{
		delete nbmp;
		return 0;
	}
	
	bds = bmp->LockBits(System::Drawing::Rectangle(0, 0, bmp->Width, bmp->Height), System::Drawing::Imaging::ImageLockMode::ReadOnly, System::Drawing::Imaging::PixelFormat::Format32bppArgb);

	UInt8 *srcPtr;
	UInt8 *destPtr;
	Int32 i = bmp->Height;
	srcPtr = (UInt8*)bds->Scan0.ToPointer();
	destPtr = (UInt8*)bdd.Scan0;
	while (i-- > 0)
	{
		memcpy(destPtr, srcPtr, bdd.Width << 2);
		srcPtr += bds->Stride;
		destPtr += bdd.Stride;
	}

	bmp->UnlockBits(bds);
	nbmp->UnlockBits(&bdd);
	return nbmp;
}

void DotNet::MyGrp::Init(Int32 nTap)
{
	NEW_CLASS(colorMgr, Media::ColorManager());
	NEW_CLASS(resizer8, Media::Resizer::LanczosResizer8_C8(14, Media::CS::TRANT_sRGB, Media::CS::TRANT_sRGB, 2.2, colorMgr));
	//NEW_CLASS(resizer16, Media::Resizer::LanczosResizerH13_8(4));
	NEW_CLASS(parsers, Parser::FullParserList());
}

void DotNet::MyGrp::Deinit()
{
	DEL_CLASS(resizer8);
//	DEL_CLASS(resizer16);
	DEL_CLASS(parsers);
	DEL_CLASS(colorMgr);
}

System::Drawing::Bitmap *DotNet::MyGrp::ResizeBitmap(System::Drawing::Bitmap *bmp, System::Int32 maxWidth, System::Int32 maxHeight, System::Boolean copyExif)
{
	System::Int32 newW;
	System::Int32 newH;
	if ((bmp->Width * maxHeight) > (maxWidth * bmp->Height))
	{
		newW = maxWidth;
		newH = MulDiv(maxWidth, bmp->Height, bmp->Width);
	}
	else
	{
		newH = maxHeight;
		newW = MulDiv(maxHeight, bmp->Width, bmp->Height);
	}
    
	System::Drawing::Bitmap *nbmp = new System::Drawing::Bitmap(newW, newH, System::Drawing::Imaging::PixelFormat::Format32bppArgb);
	System::Int32 tmp[] = bmp->PropertyIdList;
	System::Int32 tmp2 = bmp->GetFrameCount(System::Drawing::Imaging::FrameDimension::Page);
	System::Int32 i;
	System::Int32 j;
	if (copyExif)
	{
		i = 0;
		System::Drawing::Imaging::PropertyItem *pis[] = bmp->PropertyItems;
		j = pis->Length;
		while (i < j)
		{
			nbmp->SetPropertyItem(pis[i++]);
		}
	}
	System::Drawing::Imaging::BitmapData *bds;
	System::Drawing::Imaging::BitmapData *bdd;
	System::Drawing::Rectangle rects;
	System::Drawing::Rectangle rectd;
	rects = System::Drawing::Rectangle(0, 0, bmp->Width, bmp->Height);
	rectd = System::Drawing::Rectangle(0, 0, nbmp->Width, nbmp->Height);
	System::Drawing::Imaging::PixelFormat spf = bmp->PixelFormat;
	bds = bmp->LockBits(rects, System::Drawing::Imaging::ImageLockMode::ReadOnly, System::Drawing::Imaging::PixelFormat::Format32bppArgb);
	bdd = nbmp->LockBits(rectd, System::Drawing::Imaging::ImageLockMode::WriteOnly, System::Drawing::Imaging::PixelFormat::Format32bppArgb);
	GrpResizePtr32_32(bds->Scan0.ToPointer(), bds->Width, bds->Height, bdd->Scan0.ToPointer(), bdd->Width, bdd->Height);
	bmp->UnlockBits(bds);
	nbmp->UnlockBits(bdd);
	return nbmp;
}

System::Drawing::Bitmap *DotNet::MyGrp::CropBitmap(System::Drawing::Bitmap *bmp, System::Int32 x, System::Int32 y, System::Int32 width, System::Int32 height)
{
	System::Int32 left;
	System::Int32 right;
	System::Int32 top;
	System::Int32 bottom;
	left = x;
	right = x + width;
	top = y;
	bottom = y + height;
	if (left < 0)
		left = 0;
	if (top < 0)
		top = 0;
	if (right > bmp->Width)
		right = bmp->Width;
	if (bottom > bmp->Height)
		bottom = bmp->Height;
	System::Drawing::Bitmap *nbmp = new System::Drawing::Bitmap(right - left, bottom - top);
	System::Drawing::Imaging::BitmapData *bds;
	System::Drawing::Imaging::BitmapData *bdd;
	System::Drawing::Rectangle rects;
	System::Drawing::Rectangle rectd;
	rects = System::Drawing::Rectangle(0, 0, bmp->Width, bmp->Height);
	rectd = System::Drawing::Rectangle(0, 0, nbmp->Width, nbmp->Height);
	bds = bmp->LockBits(rects, System::Drawing::Imaging::ImageLockMode::ReadWrite, System::Drawing::Imaging::PixelFormat::Format32bppArgb);
	bdd = nbmp->LockBits(rectd, System::Drawing::Imaging::ImageLockMode::ReadWrite, System::Drawing::Imaging::PixelFormat::Format32bppArgb);
	GrpCropPtr32(bds->Scan0.ToPointer(), left, top, bds->Stride, bdd->Scan0.ToPointer(), bdd->Width, bdd->Height, bdd->Stride);
	bmp->UnlockBits(bds);
	nbmp->UnlockBits(bdd);
	return nbmp;
}

System::Drawing::Bitmap *DotNet::MyGrp::AdjustBmp(System::Drawing::Bitmap *bmp, System::Int32 bright, System::Int32 cont)
{
	System::Drawing::Bitmap *nbmp = new System::Drawing::Bitmap(bmp->Width, bmp->Height, System::Drawing::Imaging::PixelFormat::Format64bppArgb);
	System::Drawing::Imaging::BitmapData *bds;
	System::Drawing::Imaging::BitmapData *bdd;
	System::Drawing::Rectangle rects;
	System::Drawing::Rectangle rectd;
	rects = System::Drawing::Rectangle(0, 0, bmp->Width, bmp->Height);
	rectd = System::Drawing::Rectangle(0, 0, nbmp->Width, nbmp->Height);
	bds = bmp->LockBits(rects, System::Drawing::Imaging::ImageLockMode::ReadWrite, System::Drawing::Imaging::PixelFormat::Format64bppArgb);
	bdd = nbmp->LockBits(rectd, System::Drawing::Imaging::ImageLockMode::ReadWrite, System::Drawing::Imaging::PixelFormat::Format64bppArgb);
	GrpAdjustColor64(bds->Scan0.ToPointer(), bdd->Scan0.ToPointer(), bds->Width, bds->Height, bds->Stride, bright, cont);
	bmp->UnlockBits(bds);
	nbmp->UnlockBits(bdd);
	return nbmp;
}

System::Boolean DotNet::MyGrp::SaveJPGQuality(System::Drawing::Bitmap *bmp, System::String *fileName, System::Int32 quality)
{
	CLSID   encoderClsid;
	Gdiplus::Status  stat;
	Gdiplus::Image*   image;

	image = (Gdiplus::Image*)GDIPImageFromNet(bmp);
	if (image == 0)
	{
		return true;
	}

	// Get the CLSID of the PNG encoder.
	if (GetEncoderClsid(L"image/jpeg", &encoderClsid) < 0)
	{
		delete image;
		return true;
	}

	::IO::FileStream *fs;
	Win32::COMStream *cstm;
	System::IntPtr strPtr = System::Runtime::InteropServices::Marshal::StringToHGlobalUni(fileName);

	NEW_CLASS(fs, ::IO::FileStream((WChar*)strPtr.ToPointer(), ::IO::FileStream::FileMode::Create, ::IO::FileStream::FileShare::DenyNone));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		delete image;

		return true;
	}
	else
	{
		NEW_CLASS(cstm, Win32::COMStream(fs));

		Gdiplus::EncoderParameters params;
		params.Count = 1;
		params.Parameter[0].Guid = Gdiplus::EncoderQuality;
		params.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
		params.Parameter[0].NumberOfValues = 1;
		params.Parameter[0].Value = &quality;
		stat = image->Save(cstm, &encoderClsid, &params);

		System::Runtime::InteropServices::Marshal::FreeHGlobal(strPtr);
		DEL_CLASS(fs);
		DEL_CLASS(cstm);
	}
	delete image;

	return false;

/*
	//System::Drawing::Imaging::ImageCodecInfo *icis[] = System::Drawing::Imaging::ImageCodecInfo::GetImageEncoders();
	System::Drawing::Imaging::ImageCodecInfo::GetImageEncoders();
	System::Int32 i = 0;//encoders->Length;
	System::Drawing::Imaging::ImageCodecInfo *ici;
	System::Drawing::Imaging::EncoderParameters *ep;
    
	ep = new System::Drawing::Imaging::EncoderParameters(1);

	System::IO::FileInfo *fi;

	ep->Param[0] = new System::Drawing::Imaging::EncoderParameter(System::Drawing::Imaging::Encoder::Quality, (System::Int64)quality);

	while (i-- > 0)
	{
		ici = System::Drawing::Imaging::ImageCodecInfo::GetImageEncoders()[i];
		if (ici->FormatDescription->Equals(S"JPEG"))
		{
			break;
		}
	}

	bmp->Save(fileName, ici, ep);
	ep->Dispose();*/
}

System::Boolean DotNet::MyGrp::SaveJPGSize(System::Drawing::Bitmap *bmp, System::String *fileName, System::Int32 targetSize)
{
	CLSID   encoderClsid;
	Gdiplus::Status  stat;
	Gdiplus::Image*   image;

	System::Int32 j;
	System::Int32 k;
	System::Int32 l;
	System::Int64 jVal;
	System::Int64 kVal;
	System::Int64 lVal;
	System::IO::FileInfo *fi;

	image = (Gdiplus::Image*)GDIPImageFromNet(bmp);
	if (image == 0)
	{
		return true;
	}

	// Get the CLSID of the PNG encoder.
	if (GetEncoderClsid(L"image/jpeg", &encoderClsid) < 0)
	{
		delete image;
		return true;
	}

	::IO::FileStream *fs;
	Win32::COMStream *cstm;
	System::IntPtr strPtr = System::Runtime::InteropServices::Marshal::StringToHGlobalUni(fileName);

	Int32 quality;
	Gdiplus::EncoderParameters params;
	params.Count = 1;
	params.Parameter[0].Guid = Gdiplus::EncoderQuality;
	params.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
	params.Parameter[0].NumberOfValues = 1;
	params.Parameter[0].Value = &quality;

	quality = 0;
	NEW_CLASS(fs, ::IO::FileStream((WChar*)strPtr.ToPointer(), ::IO::FileStream::FileMode::Create, ::IO::FileStream::FileShare::DenyNone));
	NEW_CLASS(cstm, Win32::COMStream(fs));
	stat = image->Save(cstm, &encoderClsid, &params);
	DEL_CLASS(fs);
	DEL_CLASS(cstm);

	fi = new System::IO::FileInfo(fileName);
	jVal = fi->Length;
	if (targetSize <= jVal)
	{
		System::Runtime::InteropServices::Marshal::FreeHGlobal(strPtr);
		delete image;
		return false;
	}

	quality = 100;
	NEW_CLASS(fs, ::IO::FileStream((WChar*)strPtr.ToPointer(), ::IO::FileStream::FileMode::Create, ::IO::FileStream::FileShare::DenyNone));
	NEW_CLASS(cstm, Win32::COMStream(fs));
	stat = image->Save(cstm, &encoderClsid, &params);
	DEL_CLASS(fs);
	DEL_CLASS(cstm);

	fi = new System::IO::FileInfo(fileName);
	kVal = fi->Length;
	if (targetSize >= kVal)
	{
		System::Runtime::InteropServices::Marshal::FreeHGlobal(strPtr);
		delete image;
		return false;
	}

	j = 0;
	k = 100;
	while (j <= k)
	{
		l = (j + k) >> 1;
		quality = l;
		NEW_CLASS(fs, ::IO::FileStream((WChar*)strPtr.ToPointer(), ::IO::FileStream::FileMode::Create, ::IO::FileStream::FileShare::DenyNone));
		NEW_CLASS(cstm, Win32::COMStream(fs));
		stat = image->Save(cstm, &encoderClsid, &params);
		DEL_CLASS(fs);
		DEL_CLASS(cstm);

		fi = new System::IO::FileInfo(fileName);
		lVal = fi->Length;
		if (lVal > targetSize)
		{
			k = l - 1;
		}
		else if (lVal < targetSize)
		{
			j = l + 1;
		}
		else
		{
			System::Runtime::InteropServices::Marshal::FreeHGlobal(strPtr);
			delete image;
			return false;
		}
	}

	System::Runtime::InteropServices::Marshal::FreeHGlobal(strPtr);
	delete image;

	return false;

	/*
	//System::Drawing::Imaging::ImageCodecInfo *icis[] = System::Drawing::Imaging::ImageCodecInfo::GetImageEncoders();
	System::Drawing::Imaging::ImageCodecInfo::GetImageEncoders();
	System::Int32 i = 0;//System::Drawing::Imaging::ImageCodecInfo::GetImageEncoders()->Length;
	System::Drawing::Imaging::ImageCodecInfo *ici;
	System::Drawing::Imaging::EncoderParameters *ep;
	
	ep = new System::Drawing::Imaging::EncoderParameters(1);
    
	System::Int32 q;
	System::Double fs;
	System::Int32 j;
	System::Int32 k;
	System::Int32 l;
	System::Int64 jVal;
	System::Int64 kVal;
	System::Int64 lVal;
	System::IO::FileInfo *fi;

	while (i-- > 0)
	{
		ici = System::Drawing::Imaging::ImageCodecInfo::GetImageEncoders()[i];
		if (ici->FormatDescription->Equals(S"JPEG"))
			break;
	}

	ep->Param[0] = new System::Drawing::Imaging::EncoderParameter(System::Drawing::Imaging::Encoder::Quality, 0LL);
	bmp->Save(fileName, ici, ep);
	fi = new System::IO::FileInfo(fileName);
	jVal = fi->Length;
	if (targetSize <= jVal)
		return;

	ep->Param[0] = new System::Drawing::Imaging::EncoderParameter(System::Drawing::Imaging::Encoder::Quality, 100LL);
	bmp->Save(fileName, ici, ep);
	fi = new System::IO::FileInfo(fileName);
	kVal = fi->Length;
	if (targetSize >= kVal)
		return;

	j = 0;
	k = 100;
	while (j <= k)
	{
		l = (j + k) >> 1;
		ep->Param[0] = new System::Drawing::Imaging::EncoderParameter(System::Drawing::Imaging::Encoder::Quality, (System::Int64)l);
		bmp->Save(fileName, ici, ep);
		fi = new System::IO::FileInfo(fileName);
		lVal = fi->Length;
		if (lVal > targetSize)
		{
			k = l - 1;
		}
		else if (lVal < targetSize)
		{
			j = l + 1;
		}
		else
		{
			return;
		}
	}
	*/
}

System::Boolean DotNet::MyGrp::SaveTIFF(System::Drawing::Bitmap *bmp, System::String *fileName, System::Boolean compressed)
{
	CLSID   encoderClsid;
	Gdiplus::Status  stat;
	Gdiplus::Image*   image;

	image = (Gdiplus::Image*)GDIPImageFromNet(bmp);
	if (image == 0)
	{
		return true;
	}

	// Get the CLSID of the PNG encoder.
	if (GetEncoderClsid(L"image/tiff", &encoderClsid) < 0)
	{
		delete image;
		return true;
	}

	::IO::FileStream *fs;
	Win32::COMStream *cstm;
	System::IntPtr strPtr = System::Runtime::InteropServices::Marshal::StringToHGlobalUni(fileName);

	NEW_CLASS(fs, ::IO::FileStream((WChar*)strPtr.ToPointer(), ::IO::FileStream::FileMode::Create, ::IO::FileStream::FileShare::DenyNone));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		delete image;

		return true;
	}
	else
	{
		NEW_CLASS(cstm, Win32::COMStream(fs));

		Gdiplus::EncoderParameters params;
		Gdiplus::EncoderValue val;
		if (compressed)
		{
			val = Gdiplus::EncoderValueCompressionLZW;
		}
		else
		{
			val = Gdiplus::EncoderValueCompressionNone;
		}
		params.Count = 1;
		params.Parameter[0].Guid = Gdiplus::EncoderCompression;
		params.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
		params.Parameter[0].NumberOfValues = 1;
		params.Parameter[0].Value = &val;
		stat = image->Save(cstm, &encoderClsid, &params);

		System::Runtime::InteropServices::Marshal::FreeHGlobal(strPtr);
		DEL_CLASS(fs);
		DEL_CLASS(cstm);
	}
	delete image;

	return false;
}

System::Drawing::Bitmap *DotNet::MyGrp::LoadImage(System::String *fileName)
{
	System::Int32 width;
	System::Int32 height;
	void *img;
	System::IntPtr s = System::Runtime::InteropServices::Marshal::StringToHGlobalUni(fileName);
	img = GrpLoadImage32((WChar*)s.ToPointer(), &width, &height);
	System::Runtime::InteropServices::Marshal::FreeHGlobal(s);
	if (img == 0)
	{
		try
		{
			return new System::Drawing::Bitmap(fileName);
		}
		catch (System::Exception *)
		{
			return 0;
		}
	}
	System::Drawing::Bitmap *bmp = new System::Drawing::Bitmap(width, height, System::Drawing::Imaging::PixelFormat::Format32bppArgb);
	try
	{
		System::Drawing::Imaging::BitmapData *bd = bmp->LockBits(System::Drawing::Rectangle(0, 0, width, height), System::Drawing::Imaging::ImageLockMode::WriteOnly, System::Drawing::Imaging::PixelFormat::Format32bppArgb);
		GrpCopyImageData(img, bd->Scan0.ToPointer());
		bmp->UnlockBits(bd);
	}
	catch (System::Exception *)
	{
	}
	GrpDelImage(img);
	return bmp;
}
