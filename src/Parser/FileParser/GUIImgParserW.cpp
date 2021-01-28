#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/StreamDataStream.h"
#include "IO/StreamReader.h"
#include "Map/VectorLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/VectorImage.h"
#include "Media/ImageCopyC.h"
#include "Media/ImageList.h"
#include "Media/JPEGFile.h"
#include "Media/SharedImage.h"
#include "Media/StaticImage.h"
#include "Parser/FileParser/GUIImgParser.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

#if (defined(__MINGW32__) || defined(_MSC_VER)) && !defined(_WIN32_WCE)
#if defined(WIN32_LEAN_AND_MEAN)
#undef WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include "Win32/COMStream.h"
#include <gdiplus.h>

typedef struct
{
	UInt32 gdip;
	Gdiplus::GdiplusStartupInput *gdiplusStartupInput;
	Sync::Mutex *mut;
} ClassData;

Parser::FileParser::GUIImgParser::GUIImgParser()
{
	ClassData *data = MemAlloc(ClassData, 1);
	NEW_CLASS(data->gdiplusStartupInput, Gdiplus::GdiplusStartupInput());
	Gdiplus::GdiplusStartup((ULONG_PTR*)&data->gdip, data->gdiplusStartupInput, NULL);
	NEW_CLASS(data->mut, Sync::Mutex());
	this->clsData = data;
}

Parser::FileParser::GUIImgParser::~GUIImgParser()
{
	ClassData *data = (ClassData*)this->clsData;
	Gdiplus::GdiplusShutdown(data->gdip);
	DEL_CLASS(data->gdiplusStartupInput);
	DEL_CLASS(data->mut);
	MemFree(data);
}

Int32 Parser::FileParser::GUIImgParser::GetName()
{
	return *(Int32*)"GUII";
}

void Parser::FileParser::GUIImgParser::SetParserList(Parser::ParserList *parsers)
{
	this->parsers = parsers;
}

void Parser::FileParser::GUIImgParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_IMAGE_LIST_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.tif", (const UTF8Char*)"TIFF(GDI+) File");
		selector->AddFilter((const UTF8Char*)"*.tiff", (const UTF8Char*)"TIFF(GDI+) File");
		selector->AddFilter((const UTF8Char*)"*.gif", (const UTF8Char*)"GIF(GDI+) File");
		selector->AddFilter((const UTF8Char*)"*.png", (const UTF8Char*)"PNG(GDI+) File");
		selector->AddFilter((const UTF8Char*)"*.jpg", (const UTF8Char*)"JPG(GDI+) File");
		selector->AddFilter((const UTF8Char*)"*.jpeg", (const UTF8Char*)"JPG(GDI+) File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::GUIImgParser::GetParserType()
{
	return IO::ParsedObject::PT_IMAGE_LIST_PARSER;
}

IO::ParsedObject *Parser::FileParser::GUIImgParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	ClassData *data = (ClassData*)this->clsData;
	IO::StreamDataStream *stm;
	Win32::COMStream *cstm;
	UInt8 buff[256];
	Int32 isImage = 0;
	fd->GetRealData(0, 256, buff);
	if (*(Int32*)&buff[0] == 0x474e5089 && *(Int32*)&buff[4] == 0x0a1a0a0d)
	{
		isImage = 1;
	}
	else if (buff[0] == 0xff && buff[1] == 0xd8)
	{
		isImage = 2;
	}
	else if (*(Int32*)&buff[0] == *(Int32*)"GIF8" && (*(Int16*)&buff[4] == *(Int16*)"7a" || *(Int16*)&buff[4] == *(Int16*)"9a"))
	{
		isImage = 3;
	}
	else if (*(Int16*)&buff[0] == *(Int16*)"MM" || *(Int16*)&buff[0] == *(Int16*)"II")
	{
		isImage = 4;
	}
	if (isImage == 0)
		return 0;

	Media::ImageList *imgList = 0;

	data->mut->Lock();
	NEW_CLASS(stm, IO::StreamDataStream(fd));
	NEW_CLASS(cstm, Win32::COMStream(stm));
	Gdiplus::Bitmap *bmp = Gdiplus::Bitmap::FromStream(cstm, 0);
	if (bmp)
	{
		Gdiplus::BitmapData bmpd;
		Gdiplus::Status stat;
		Gdiplus::Rect rect(0, 0, bmp->GetWidth(), bmp->GetHeight());
		Gdiplus::PixelFormat pxFmt = bmp->GetPixelFormat();
		if (pxFmt == PixelFormat48bppRGB)
		{
			Media::AlphaType aType = Media::AT_NO_ALPHA;
			if ((stat = bmp->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat48bppRGB, &bmpd)) == Gdiplus::Ok)
			{
				Media::StaticImage *img;

				NEW_CLASS(img, Media::StaticImage(bmp->GetWidth(), bmp->GetHeight(), 0, 48, Media::PF_LE_B16G16R16, 0, 0, Media::ColorProfile::YUVT_UNKNOWN, aType, Media::YCOFST_C_CENTER_LEFT));
				UInt8 *imgSrc = (UInt8*)bmpd.Scan0;
				UInt8 *imgDest = (UInt8*)img->data;
				ImageCopy_ImgCopy(imgSrc, imgDest, bmpd.Width * 6, bmpd.Height, bmpd.Stride, img->GetDataBpl());
				bmp->UnlockBits(&bmpd);
				NEW_CLASS(imgList, Media::ImageList(fd->GetFullName()));
				imgList->AddImage(img, 0);
				img->info->hdpi = bmp->GetHorizontalResolution();
				img->info->vdpi = bmp->GetVerticalResolution();

				if (isImage == 2)
				{
					Media::JPEGFile::ParseJPEGHeader(fd, img, imgList, this->parsers);
				}
			}
		}
		else if (pxFmt == PixelFormat64bppARGB || pxFmt == PixelFormat64bppPARGB)
		{
			Media::AlphaType aType = (isImage == 2)?Media::AT_NO_ALPHA:Media::AT_ALPHA;
			if (pxFmt == PixelFormat64bppARGB)
			{
				aType = Media::AT_ALPHA;
			}
			else if (pxFmt == PixelFormat64bppPARGB)
			{
				aType = Media::AT_PREMUL_ALPHA;
			}
			if ((stat = bmp->LockBits(&rect, Gdiplus::ImageLockModeRead, pxFmt, &bmpd)) == Gdiplus::Ok)
			{
				Media::StaticImage *img;

				NEW_CLASS(img, Media::StaticImage(bmp->GetWidth(), bmp->GetHeight(), 0, 64, Media::PF_LE_B16G16R16A16, 0, 0, Media::ColorProfile::YUVT_UNKNOWN, aType, Media::YCOFST_C_CENTER_LEFT));
				UInt8 *imgSrc = (UInt8*)bmpd.Scan0;
				UInt8 *imgDest = (UInt8*)img->data;
				ImageCopy_ImgCopy(imgSrc, imgDest, bmpd.Width << 3, bmpd.Height, bmpd.Stride, bmpd.Width << 3);
				bmp->UnlockBits(&bmpd);
				NEW_CLASS(imgList, Media::ImageList(fd->GetFullName()));
				imgList->AddImage(img, 0);
				img->info->hdpi = bmp->GetHorizontalResolution();
				img->info->vdpi = bmp->GetVerticalResolution();

				if (isImage == 2)
				{
					Media::JPEGFile::ParseJPEGHeader(fd, img, imgList, this->parsers);
				}
			}
		}
		else if (pxFmt == PixelFormat24bppRGB)
		{
			if ((stat = bmp->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat24bppRGB, &bmpd)) == Gdiplus::Ok)
			{
				Media::StaticImage *img;
				NEW_CLASS(img, Media::StaticImage(bmp->GetWidth(), bmp->GetHeight(), 0, 24, Media::PF_B8G8R8, 0, 0, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_ALPHA, Media::YCOFST_C_CENTER_LEFT));
				UInt8 *imgSrc = (UInt8*)bmpd.Scan0;
				UInt8 *imgDest = (UInt8*)img->data;
				ImageCopy_ImgCopy(imgSrc, imgDest, bmpd.Width * 3, bmpd.Height, bmpd.Stride, img->GetDataBpl());
				bmp->UnlockBits(&bmpd);
				NEW_CLASS(imgList, Media::ImageList(fd->GetFullName()));
				imgList->AddImage(img, 0);
				img->info->hdpi = bmp->GetHorizontalResolution();
				img->info->vdpi = bmp->GetVerticalResolution();

				if (isImage == 2)
				{
					Media::JPEGFile::ParseJPEGHeader(fd, img, imgList, this->parsers);
				}
			}
		}
		else if (pxFmt == PixelFormat8bppIndexed)
		{
			if ((stat = bmp->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat8bppIndexed, &bmpd)) == Gdiplus::Ok)
			{
				Media::StaticImage *img;
				Gdiplus::ColorPalette *pal;
				NEW_CLASS(img, Media::StaticImage(bmp->GetWidth(), bmp->GetHeight(), 0, 8, Media::PF_PAL_8, 0, 0, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_ALPHA, Media::YCOFST_C_CENTER_LEFT));
				UInt8 *imgSrc = (UInt8*)bmpd.Scan0;
				UInt8 *imgDest = (UInt8*)img->data;
				ImageCopy_ImgCopy(imgSrc, imgDest, bmpd.Width, bmpd.Height, bmpd.Stride, img->GetDataBpl());
				bmp->UnlockBits(&bmpd);
				Int32 size = bmp->GetPaletteSize();
				pal = (Gdiplus::ColorPalette*)MemAlloc(UInt8, size);
				bmp->GetPalette(pal, size);
				MemCopyNO(img->pal, &pal->Entries, pal->Count * 4);
				MemFree(pal);
				NEW_CLASS(imgList, Media::ImageList(fd->GetFullName()));
				imgList->AddImage(img, 0);
				img->info->hdpi = bmp->GetHorizontalResolution();
				img->info->vdpi = bmp->GetVerticalResolution();
			}
		}
		else
		{
			if ((stat = bmp->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bmpd)) == Gdiplus::Ok)
			{
				Media::StaticImage *img;
				NEW_CLASS(img, Media::StaticImage(bmp->GetWidth(), bmp->GetHeight(), 0, 32, Media::PF_B8G8R8A8, 0, 0, Media::ColorProfile::YUVT_UNKNOWN, (isImage == 2)?Media::AT_NO_ALPHA:Media::AT_ALPHA, Media::YCOFST_C_CENTER_LEFT));
				UInt8 *imgSrc = (UInt8*)bmpd.Scan0;
				UInt8 *imgDest = (UInt8*)img->data;
				ImageCopy_ImgCopy(imgSrc, imgDest, bmpd.Width << 2, bmpd.Height, bmpd.Stride, bmpd.Width << 2);
				bmp->UnlockBits(&bmpd);
				NEW_CLASS(imgList, Media::ImageList(fd->GetFullName()));
				imgList->AddImage(img, 0);
				img->info->hdpi = bmp->GetHorizontalResolution();
				img->info->vdpi = bmp->GetVerticalResolution();

				if (isImage == 2)
				{
					Media::JPEGFile::ParseJPEGHeader(fd, img, imgList, this->parsers);
				}
			}
		}
		if (isImage == 3)
		{
			GUID guids[10];
			UInt32 cnt = bmp->GetFrameDimensionsCount();
			if (cnt > 10)
			{
				cnt = 10;
			}
			bmp->GetFrameDimensionsList(guids, cnt);
			UInt32 frameCnt = bmp->GetFrameCount(&guids[0]);
			UInt32 i = 1;
			while (i < frameCnt)
			{
				bmp->SelectActiveFrame(&guids[0], i);

				if ((stat = bmp->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bmpd)) == Gdiplus::Ok)
				{
					Media::StaticImage *img;
					NEW_CLASS(img, Media::StaticImage(bmp->GetWidth(), bmp->GetHeight(), 0, 32, Media::PF_B8G8R8A8, 0, 0, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_ALPHA, Media::YCOFST_C_CENTER_LEFT));
					UInt8 *imgSrc = (UInt8*)bmpd.Scan0;
					UInt8 *imgDest = (UInt8*)img->data;
					ImageCopy_ImgCopy(imgSrc, imgDest, bmpd.Width << 2, bmpd.Height, bmpd.Stride, bmpd.Width << 2);
					bmp->UnlockBits(&bmpd);
					imgList->AddImage(img, 500);
					img->info->hdpi = bmp->GetHorizontalResolution();
					img->info->vdpi = bmp->GetVerticalResolution();
				}
				i++;
			}
		}
		delete bmp;
	}
	DEL_CLASS(cstm);
	DEL_CLASS(stm);
	data->mut->Unlock();

	if (targetType != IO::ParsedObject::PT_IMAGE_LIST_PARSER && imgList && imgList->GetCount() == 1)
	{
		Media::StaticImage *img = (Media::StaticImage*)imgList->GetImage(0, 0);
		Double minX;
		Double minY;
		Double maxX;
		Double maxY;
		Int32 srid;
		if (img->exif && img->exif->GetGeoBounds(img->info->dispWidth, img->info->dispHeight, &srid, &minX, &minY, &maxX, &maxY))
		{
			Map::VectorLayer *lyr;
			Math::VectorImage *vimg;
			Media::SharedImage *simg;
			if (srid == 0)
			{
				srid = 4326;
			}
			
			NEW_CLASS(lyr, Map::VectorLayer(Map::DRAW_LAYER_IMAGE, fd->GetFullName(), 0, 0, Math::CoordinateSystemManager::SRCreateCSys(srid), 0, 0, 0, 0, 0));
			img->To32bpp();
			
			NEW_CLASS(simg, Media::SharedImage(imgList, true));
			NEW_CLASS(vimg, Math::VectorImage(srid, simg, minX, minY, maxX, maxY, false, fd->GetFullName(), 0, 0));
			lyr->AddVector(vimg, 0);
			DEL_CLASS(simg);
			
			return lyr;
		}

		if (fd->IsFullFile())
		{
			Text::StringBuilderUTF8 sb;
			sb.Append(fd->GetFullFileName());
			sb.AllocLeng(5);
			sb.SetEndPtr(IO::Path::ReplaceExt(sb.ToString(), (const UTF8Char*)"tfw"));
			if (IO::Path::GetPathType(sb.ToString()) == IO::Path::PT_FILE)
			{
				IO::FileStream *fs;
				IO::StreamReader *reader;
				Bool valid = true;
				Double xPxSize;
				Double rotY;
				Double rotX;
				Double yPxSize;
				Double xCoord;
				Double yCoord;
				NEW_CLASS(fs, IO::FileStream(sb.ToString(), IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_SEQUENTIAL));
				NEW_CLASS(reader, IO::StreamReader(fs, 0));
				sb.ClearStr();
				if (!reader->ReadLine(&sb, 1024) || !Text::StrToDouble(sb.ToString(), &xPxSize))
				{
					valid = false;
				}
				sb.ClearStr();
				if (!reader->ReadLine(&sb, 1024) || !Text::StrToDouble(sb.ToString(), &rotY))
				{
					valid = false;
				}
				sb.ClearStr();
				if (!reader->ReadLine(&sb, 1024) || !Text::StrToDouble(sb.ToString(), &rotX))
				{
					valid = false;
				}
				sb.ClearStr();
				if (!reader->ReadLine(&sb, 1024) || !Text::StrToDouble(sb.ToString(), &yPxSize))
				{
					valid = false;
				}
				sb.ClearStr();
				if (!reader->ReadLine(&sb, 1024) || !Text::StrToDouble(sb.ToString(), &xCoord))
				{
					valid = false;
				}
				sb.ClearStr();
				if (!reader->ReadLine(&sb, 1024) || !Text::StrToDouble(sb.ToString(), &yCoord))
				{
					valid = false;
				}
				DEL_CLASS(reader);
				DEL_CLASS(fs);

				if (valid && rotX == 0 && rotY == 0)
				{
					Map::VectorLayer *lyr;
					Math::VectorImage *vimg;
					Media::SharedImage *simg;
					
					Math::CoordinateSystem *csys = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::GeographicCoordinateSystem::GCST_WGS84);
					NEW_CLASS(lyr, Map::VectorLayer(Map::DRAW_LAYER_IMAGE, fd->GetFullName(), 0, 0, csys, 0, 0, 0, 0, 0));
					img->To32bpp();
					NEW_CLASS(simg, Media::SharedImage(imgList, true));
					NEW_CLASS(vimg, Math::VectorImage(lyr->GetCoordinateSystem()->GetSRID(), simg, xCoord - xPxSize * 0.5, yCoord + yPxSize * (img->info->dispHeight - 0.5), xCoord + xPxSize * (img->info->dispWidth - 0.5), yCoord - yPxSize * 0.5, false, fd->GetFullName(), 0, 0));
					lyr->AddVector(vimg, 0);
					DEL_CLASS(simg);
					
					return lyr;
				}
			}
		}
	}
	return imgList;
}
#else

Parser::FileParser::GUIImgParser::GUIImgParser()
{
}

Parser::FileParser::GUIImgParser::~GUIImgParser()
{
}

Int32 Parser::FileParser::GUIImgParser::GetName()
{
	return *(Int32*)"GUII";
}

void Parser::FileParser::GUIImgParser::SetParserList(Parser::ParserList *parsers)
{
}

void Parser::FileParser::GUIImgParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
}

IO::ParsedObject::ParserType Parser::FileParser::GUIImgParser::GetParserType()
{
	return IO::ParsedObject::PT_IMAGE_LIST_PARSER;
}

IO::ParsedObject *Parser::FileParser::GUIImgParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	return 0;
}
#endif
