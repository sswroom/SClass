#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/PackageFile.h"
#include "IO/Path.h"
#include "IO/StreamDataStream.h"
#include "IO/StreamReader.h"
#include "Map/VectorLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math_C.h"
#include "Math/Geometry/VectorImage.h"
#include "Media/ImageCopy_C.h"
#include "Media/ImageList.h"
#include "Media/ImagePreviewTool.h"
#include "Media/JPEGFile.h"
#include "Media/SharedImage.h"
#include "Media/StaticImage.h"
#include "Parser/FileParser/GUIImgParser.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

#if (defined(__MINGW32__) || defined(_MSC_VER)) && !defined(_WIN32_WCE)
#if defined(WIN32_LEAN_AND_MEAN)
#undef WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include "Win32/COMStream.h"
#include <gdiplus.h>

struct Parser::FileParser::GUIImgParser::ClassData
{
	UInt32 gdip;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Sync::Mutex mut;
};

Parser::FileParser::GUIImgParser::GUIImgParser()
{
	NEW_CLASS(this->clsData, ClassData());
	Gdiplus::GdiplusStartup((ULONG_PTR*)&this->clsData->gdip, &this->clsData->gdiplusStartupInput, NULL);
}

Parser::FileParser::GUIImgParser::~GUIImgParser()
{
	Gdiplus::GdiplusShutdown(this->clsData->gdip);
	DEL_CLASS(this->clsData);
}

Int32 Parser::FileParser::GUIImgParser::GetName()
{
	return *(Int32*)"GUII";
}

void Parser::FileParser::GUIImgParser::SetParserList(Optional<Parser::ParserList> parsers)
{
	this->parsers = parsers;
}

void Parser::FileParser::GUIImgParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::ImageList)
	{
		selector->AddFilter(CSTR("*.tif"), CSTR("TIFF(GDI+) File"));
		selector->AddFilter(CSTR("*.tiff"), CSTR("TIFF(GDI+) File"));
		selector->AddFilter(CSTR("*.gif"), CSTR("GIF(GDI+) File"));
		selector->AddFilter(CSTR("*.png"), CSTR("PNG(GDI+) File"));
		selector->AddFilter(CSTR("*.jpg"), CSTR("JPG(GDI+) File"));
		selector->AddFilter(CSTR("*.jpeg"), CSTR("JPG(GDI+) File"));
	}
}

IO::ParserType Parser::FileParser::GUIImgParser::GetParserType()
{
	return IO::ParserType::ImageList;
}

Optional<IO::ParsedObject> Parser::FileParser::GUIImgParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	NN<IO::StreamDataStream> stm;
	Win32::COMStream *cstm;
	Int32 isImage = 0;
	if (ReadUInt32(&hdr[0]) == 0x474e5089 && ReadUInt32(&hdr[4]) == 0x0a1a0a0d)
	{
		isImage = 1;
	}
	else if (hdr[0] == 0xff && hdr[1] == 0xd8)
	{
		isImage = 2;
	}
	else if (*(Int32*)&hdr[0] == *(Int32*)"GIF8" && (*(Int16*)&hdr[4] == *(Int16*)"7a" || *(Int16*)&hdr[4] == *(Int16*)"9a"))
	{
		isImage = 3;
	}
	else if (*(Int16*)&hdr[0] == *(Int16*)"MM" || *(Int16*)&hdr[0] == *(Int16*)"II")
	{
		isImage = 4;
	}
	if (isImage == 0)
		return 0;

	Media::ImageList *imgList = 0;
	NN<Media::ImageList> nnimgList;

	Sync::MutexUsage mutUsage(this->clsData->mut);
	NEW_CLASSNN(stm, IO::StreamDataStream(fd));
	NEW_CLASS(cstm, Win32::COMStream(stm));
	Gdiplus::Bitmap *bmp = Gdiplus::Bitmap::FromStream(cstm, 0);
	if (bmp)
	{
		Gdiplus::BitmapData bmpd;
		Gdiplus::Status stat;
		Gdiplus::Rect rect(0, 0, (INT)bmp->GetWidth(), (INT)bmp->GetHeight());
		Gdiplus::PixelFormat pxFmt = bmp->GetPixelFormat();
		if (pxFmt == PixelFormat48bppRGB)
		{
			Media::AlphaType aType = Media::AT_ALPHA_ALL_FF;
			if ((stat = bmp->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat48bppRGB, &bmpd)) == Gdiplus::Ok)
			{
				NN<Media::StaticImage> img;

				NEW_CLASSNN(img, Media::StaticImage(Math::Size2D<UOSInt>(bmp->GetWidth(), bmp->GetHeight()), 0, 48, Media::PF_LE_B16G16R16, 0, Media::ColorProfile(), Media::ColorProfile::YUVT_UNKNOWN, aType, Media::YCOFST_C_CENTER_LEFT));
				UInt8 *imgSrc = (UInt8*)bmpd.Scan0;
				UnsafeArray<UInt8> imgDest = img->data;
				ImageCopy_ImgCopyR(imgSrc, imgDest.Ptr(), bmpd.Width * 6, bmpd.Height, (UOSInt)bmpd.Stride, img->GetDataBpl(), false);
				bmp->UnlockBits(&bmpd);
				NEW_CLASSNN(nnimgList, Media::ImageList(fd->GetFullName()));
				nnimgList->AddImage(img, 0);
				img->info.hdpi = bmp->GetHorizontalResolution();
				img->info.vdpi = bmp->GetVerticalResolution();

				if (isImage == 2)
				{
					Media::JPEGFile::ParseJPEGHeader(fd, img, nnimgList, this->parsers);
				}
				imgList = nnimgList.Ptr();
			}
		}
		else if (pxFmt == PixelFormat64bppARGB || pxFmt == PixelFormat64bppPARGB)
		{
			Media::AlphaType aType = (isImage == 2)?Media::AT_IGNORE_ALPHA:Media::AT_ALPHA;
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
				NN<Media::StaticImage> img;

				NEW_CLASSNN(img, Media::StaticImage(Math::Size2D<UOSInt>(bmp->GetWidth(), bmp->GetHeight()), 0, 64, Media::PF_LE_B16G16R16A16, 0, Media::ColorProfile(), Media::ColorProfile::YUVT_UNKNOWN, aType, Media::YCOFST_C_CENTER_LEFT));
				UInt8 *imgSrc = (UInt8*)bmpd.Scan0;
				UnsafeArray<UInt8> imgDest = img->data;
				ImageCopy_ImgCopy(imgSrc, imgDest.Ptr(), bmpd.Width << 3, bmpd.Height, bmpd.Stride, bmpd.Width << 3);
				bmp->UnlockBits(&bmpd);
				NEW_CLASSNN(nnimgList, Media::ImageList(fd->GetFullName()));
				nnimgList->AddImage(img, 0);
				img->info.hdpi = bmp->GetHorizontalResolution();
				img->info.vdpi = bmp->GetVerticalResolution();

				if (isImage == 2)
				{
					Media::JPEGFile::ParseJPEGHeader(fd, img, nnimgList, this->parsers);
				}
				imgList = nnimgList.Ptr();
			}
		}
		else if (pxFmt == PixelFormat24bppRGB)
		{
			if ((stat = bmp->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat24bppRGB, &bmpd)) == Gdiplus::Ok)
			{
				NN<Media::StaticImage> img;
				NEW_CLASSNN(img, Media::StaticImage(Math::Size2D<UOSInt>(bmp->GetWidth(), bmp->GetHeight()), 0, 24, Media::PF_B8G8R8, 0, Media::ColorProfile(), Media::ColorProfile::YUVT_UNKNOWN, Media::AT_ALPHA, Media::YCOFST_C_CENTER_LEFT));
				UInt8 *imgSrc = (UInt8*)bmpd.Scan0;
				UnsafeArray<UInt8> imgDest = img->data;
				ImageCopy_ImgCopyR(imgSrc, imgDest.Ptr(), bmpd.Width * 3, bmpd.Height, (UOSInt)bmpd.Stride, img->GetDataBpl(), false);
				bmp->UnlockBits(&bmpd);
				NEW_CLASSNN(nnimgList, Media::ImageList(fd->GetFullName()));
				nnimgList->AddImage(img, 0);
				img->info.hdpi = bmp->GetHorizontalResolution();
				img->info.vdpi = bmp->GetVerticalResolution();

				if (isImage == 2)
				{
					Media::JPEGFile::ParseJPEGHeader(fd, img, nnimgList, this->parsers);
				}
				imgList = nnimgList.Ptr();
			}
		}
		else if (pxFmt == PixelFormat8bppIndexed)
		{
			if ((stat = bmp->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat8bppIndexed, &bmpd)) == Gdiplus::Ok)
			{
				NN<Media::StaticImage> img;
				Gdiplus::ColorPalette *pal;
				NEW_CLASSNN(img, Media::StaticImage(Math::Size2D<UOSInt>(bmp->GetWidth(), bmp->GetHeight()), 0, 8, Media::PF_PAL_8, 0, Media::ColorProfile(), Media::ColorProfile::YUVT_UNKNOWN, Media::AT_ALPHA, Media::YCOFST_C_CENTER_LEFT));
				UInt8 *imgSrc = (UInt8*)bmpd.Scan0;
				UnsafeArray<UInt8> imgDest = img->data;
				ImageCopy_ImgCopyR(imgSrc, imgDest.Ptr(), bmpd.Width, bmpd.Height, (UOSInt)bmpd.Stride, img->GetDataBpl(), false);
				bmp->UnlockBits(&bmpd);
				Int32 size = bmp->GetPaletteSize();
				pal = (Gdiplus::ColorPalette*)MemAlloc(UInt8, (UInt32)size);
				bmp->GetPalette(pal, size);
				UnsafeArray<UInt8> nnpal;
				if (img->pal.SetTo(nnpal))
				{
					MemCopyNO(nnpal.Ptr(), &pal->Entries, pal->Count * 4);
				}
				MemFree(pal);
				NEW_CLASS(imgList, Media::ImageList(fd->GetFullName()));
				imgList->AddImage(img, 0);
				img->info.hdpi = bmp->GetHorizontalResolution();
				img->info.vdpi = bmp->GetVerticalResolution();
			}
		}
		else
		{
			if ((stat = bmp->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bmpd)) == Gdiplus::Ok)
			{
				NN<Media::StaticImage> img;
				NEW_CLASSNN(img, Media::StaticImage(Math::Size2D<UOSInt>(bmp->GetWidth(), bmp->GetHeight()), 0, 32, Media::PF_B8G8R8A8, 0, Media::ColorProfile(), Media::ColorProfile::YUVT_UNKNOWN, (isImage == 2)?Media::AT_IGNORE_ALPHA:Media::AT_ALPHA, Media::YCOFST_C_CENTER_LEFT));
				UInt8 *imgSrc = (UInt8*)bmpd.Scan0;
				UnsafeArray<UInt8> imgDest = img->data;
				ImageCopy_ImgCopy(imgSrc, imgDest.Ptr(), bmpd.Width << 2, bmpd.Height, bmpd.Stride, bmpd.Width << 2);
				bmp->UnlockBits(&bmpd);
				NEW_CLASSNN(nnimgList, Media::ImageList(fd->GetFullName()));
				nnimgList->AddImage(img, 0);
				img->info.hdpi = bmp->GetHorizontalResolution();
				img->info.vdpi = bmp->GetVerticalResolution();

				if (isImage == 2)
				{
					Media::JPEGFile::ParseJPEGHeader(fd, img, nnimgList, this->parsers);
				}
				imgList = nnimgList.Ptr();
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
					NN<Media::StaticImage> img;
					NEW_CLASSNN(img, Media::StaticImage(Math::Size2D<UOSInt>(bmp->GetWidth(), bmp->GetHeight()), 0, 32, Media::PF_B8G8R8A8, 0, Media::ColorProfile(), Media::ColorProfile::YUVT_UNKNOWN, Media::AT_ALPHA, Media::YCOFST_C_CENTER_LEFT));
					UInt8 *imgSrc = (UInt8*)bmpd.Scan0;
					UnsafeArray<UInt8> imgDest = img->data;
					ImageCopy_ImgCopy(imgSrc, imgDest.Ptr(), bmpd.Width << 2, bmpd.Height, bmpd.Stride, bmpd.Width << 2);
					bmp->UnlockBits(&bmpd);
					imgList->AddImage(img, 500);
					img->info.hdpi = bmp->GetHorizontalResolution();
					img->info.vdpi = bmp->GetVerticalResolution();
				}
				i++;
			}
		}
		delete bmp;
	}
	DEL_CLASS(cstm);
	stm.Delete();
	mutUsage.EndUse();

	NN<Media::StaticImage> img;
	if (targetType != IO::ParserType::ImageList && nnimgList.Set(imgList) && imgList->GetCount() == 1 && Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(0, 0)).SetTo(img))
	{
		Math::Coord2DDbl min;
		Math::Coord2DDbl max;
		UInt32 srid;
		NN<Media::EXIFData> exif;
		if (img->exif.SetTo(exif) && exif->GetGeoBounds(img->info.dispSize, srid, min.x, min.y, max.x, max.y))
		{
			Map::VectorLayer *lyr;
			NN<Math::Geometry::VectorImage> vimg;
			NN<Math::CoordinateSystem> csys;
			if (srid == 0)
			{
				csys = Math::CoordinateSystemManager::CreateCsysByCoord((min + max) * 0.5);
				srid = csys->GetSRID();
			}
			else
			{
				csys = Math::CoordinateSystemManager::SRCreateCSysOrDef(srid);
			}
			NEW_CLASS(lyr, Map::VectorLayer(Map::DRAW_LAYER_IMAGE, fd->GetFullName(), csys, 0));
			Data::ArrayListNN<Media::StaticImage> prevList;
			Media::ImagePreviewTool::CreatePreviews(nnimgList, prevList, 640);
			Media::SharedImage simg(nnimgList, prevList);
			NEW_CLASSNN(vimg, Math::Geometry::VectorImage(srid, simg, min, max, false, fd->GetFullName().Ptr(), 0, 0));
			lyr->AddVector2(vimg, (Text::String**)0);
			return lyr;
		}

		UTF8Char fileNameBuff[1024];
		UnsafeArray<UTF8Char> sptr;
		NN<IO::PackageFile> nnpkgFile;
		NN<IO::StreamData> pkgFd;
		Bool valid = false;
		Double xPxSize = 0;
		Double rotY = 0;
		Double rotX = 0;
		Double yPxSize = 0;
		Double xCoord = 0;
		Double yCoord = 0;
		if (pkgFile.SetTo(nnpkgFile))
		{
			fd->GetShortName().OrEmpty().ConcatTo(fileNameBuff);
			sptr = IO::Path::ReplaceExt(UARR(fileNameBuff), UTF8STRC("tfw"));
			if (nnpkgFile->GetItemStmDataNew(CSTRP(fileNameBuff, sptr)).SetTo(pkgFd))
			{
				Text::StringBuilderUTF8 sb;
				{
					valid = true;
					IO::StreamDataStream fs(pkgFd);
					IO::StreamReader reader(fs, 0);
					sb.ClearStr();
					if (!reader.ReadLine(sb, 1024) || !Text::StrToDouble(sb.ToString(), xPxSize))
					{
						valid = false;
					}
					sb.ClearStr();
					if (!reader.ReadLine(sb, 1024) || !Text::StrToDouble(sb.ToString(), rotY))
					{
						valid = false;
					}
					sb.ClearStr();
					if (!reader.ReadLine(sb, 1024) || !Text::StrToDouble(sb.ToString(), rotX))
					{
						valid = false;
					}
					sb.ClearStr();
					if (!reader.ReadLine(sb, 1024) || !Text::StrToDouble(sb.ToString(), yPxSize))
					{
						valid = false;
					}
					sb.ClearStr();
					if (!reader.ReadLine(sb, 1024) || !Text::StrToDouble(sb.ToString(), xCoord))
					{
						valid = false;
					}
					sb.ClearStr();
					if (!reader.ReadLine(sb, 1024) || !Text::StrToDouble(sb.ToString(), yCoord))
					{
						valid = false;
					}
				}
				pkgFd.Delete();
			}
		}
		if (!valid && fd->IsFullFile())
		{
			Text::StringBuilderUTF8 sb;
			sb.Append(fd->GetFullFileName());
			UOSInt i = sb.LastIndexOf(IO::Path::PATH_SEPERATOR);
			UOSInt j = sb.LastIndexOf('.');
			if (j != INVALID_INDEX && (i == INVALID_INDEX || j > i))
			{
				sb.RemoveChars(sb.GetCharCnt() - j);
			}
			sb.AppendC(UTF8STRC(".tfw"));
			if (IO::Path::GetPathType(sb.ToCString()) == IO::Path::PathType::File)
			{
				valid = true;
				{
					IO::FileStream fs(sb.ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential);
					IO::StreamReader reader(fs, 0);
					sb.ClearStr();
					if (!reader.ReadLine(sb, 1024) || !Text::StrToDouble(sb.ToString(), xPxSize))
					{
						valid = false;
					}
					sb.ClearStr();
					if (!reader.ReadLine(sb, 1024) || !Text::StrToDouble(sb.ToString(), rotY))
					{
						valid = false;
					}
					sb.ClearStr();
					if (!reader.ReadLine(sb, 1024) || !Text::StrToDouble(sb.ToString(), rotX))
					{
						valid = false;
					}
					sb.ClearStr();
					if (!reader.ReadLine(sb, 1024) || !Text::StrToDouble(sb.ToString(), yPxSize))
					{
						valid = false;
					}
					sb.ClearStr();
					if (!reader.ReadLine(sb, 1024) || !Text::StrToDouble(sb.ToString(), xCoord))
					{
						valid = false;
					}
					sb.ClearStr();
					if (!reader.ReadLine(sb, 1024) || !Text::StrToDouble(sb.ToString(), yCoord))
					{
						valid = false;
					}
				}
			}
		}
		if (valid && rotX == 0 && rotY == 0)
		{
			Map::VectorLayer *lyr;
			NN<Math::Geometry::VectorImage> vimg;
			NN<Media::SharedImage> simg;
			NN<Math::CoordinateSystem> csys = Math::CoordinateSystemManager::CreateCsysByCoord(Math::Coord2DDbl(xCoord + xPxSize * UOSInt2Double(img->info.dispSize.x) * 0.5, yCoord + yPxSize * UOSInt2Double(img->info.dispSize.y) * 0.5));
			
			NEW_CLASS(lyr, Map::VectorLayer(Map::DRAW_LAYER_IMAGE, fd->GetFullName(), csys, 0));
			Data::ArrayListNN<Media::StaticImage> prevList;
			Media::ImagePreviewTool::CreatePreviews(nnimgList, prevList, 640);
			NEW_CLASSNN(simg, Media::SharedImage(nnimgList, prevList));
			NEW_CLASSNN(vimg, Math::Geometry::VectorImage(csys->GetSRID(), simg, Math::Coord2DDbl(xCoord - xPxSize * 0.5, yCoord + yPxSize * (UOSInt2Double(img->info.dispSize.y) - 0.5)), Math::Coord2DDbl(xCoord + xPxSize * (UOSInt2Double(img->info.dispSize.x) - 0.5), yCoord - yPxSize * 0.5), false, fd->GetFullName().Ptr(), 0, 0));
			lyr->AddVector2(vimg, (Text::String**)0);
			simg.Delete();
			
			return lyr;
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

void Parser::FileParser::GUIImgParser::SetParserList(Optional<Parser::ParserList> parsers)
{
}

void Parser::FileParser::GUIImgParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
}

IO::ParserType Parser::FileParser::GUIImgParser::GetParserType()
{
	return IO::ParserType::ImageList;
}

Optional<IO::ParsedObject> Parser::FileParser::GUIImgParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	return 0;
}
#endif
