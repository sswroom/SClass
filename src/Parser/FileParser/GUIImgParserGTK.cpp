#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/StreamReader.h"
#include "Map/VectorLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math.h"
#include "Math/VectorImage.h"
#include "Media/ImageCopyC.h"
#include "Media/ImageList.h"
#include "Media/JPEGFile.h"
#include "Media/SharedImage.h"
#include "Media/StaticImage.h"
#include "Parser/FileParser/GUIImgParser.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include <gdk/gdk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <wchar.h>

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
	this->parsers = parsers;
}

void Parser::FileParser::GUIImgParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_IMAGE_LIST_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.tif", (const UTF8Char*)"TIFF(GDK) File");
		selector->AddFilter((const UTF8Char*)"*.tiff", (const UTF8Char*)"TIFF(GDK) File");
		selector->AddFilter((const UTF8Char*)"*.gif", (const UTF8Char*)"GIF(GDK) File");
		selector->AddFilter((const UTF8Char*)"*.png", (const UTF8Char*)"PNG(GDK) File");
		selector->AddFilter((const UTF8Char*)"*.jpg", (const UTF8Char*)"JPG(GDK) File");
		selector->AddFilter((const UTF8Char*)"*.jpeg", (const UTF8Char*)"JPG(GDK) File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::GUIImgParser::GetParserType()
{
	return IO::ParsedObject::PT_IMAGE_LIST_PARSER;
}

IO::ParsedObject *Parser::FileParser::GUIImgParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
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
	UInt64 dataSize = fd->GetDataSize();
	UInt8 *data = MemAlloc(UInt8, dataSize);
	fd->GetRealData(0, (UOSInt)dataSize, data);
	GInputStream *inpStream = g_memory_input_stream_new_from_data(data, (gssize)dataSize, 0);
	GdkPixbuf *pixBuf = gdk_pixbuf_new_from_stream(inpStream, 0, 0);
	if (pixBuf)
	{
		guint leng;
		const guint8 *imgPtr = gdk_pixbuf_get_pixels_with_length(pixBuf, &leng);
		gboolean hasAlpha = gdk_pixbuf_get_has_alpha(pixBuf);
		int nChannels = gdk_pixbuf_get_n_channels(pixBuf);
		int bps = gdk_pixbuf_get_bits_per_sample(pixBuf);
		int width = gdk_pixbuf_get_width(pixBuf);
		int height = gdk_pixbuf_get_height(pixBuf);
		int bpl = gdk_pixbuf_get_rowstride(pixBuf);
		Double xdpi = 72.0;
		Double ydpi = 72.0;
		const gchar *cptr;
		cptr = gdk_pixbuf_get_option(pixBuf, "x-dpi");
		if (cptr) Text::StrToDouble(cptr, &xdpi);
		cptr = gdk_pixbuf_get_option(pixBuf, "y-dpi");
		if (cptr) Text::StrToDouble(cptr, &ydpi);

		Media::StaticImage *img = 0;
		Media::AlphaType aType = (isImage == 2||!hasAlpha)?Media::AT_NO_ALPHA:Media::AT_ALPHA;

		if (nChannels == 3 && bps == 8 && !hasAlpha)
		{
			NEW_CLASS(img, Media::StaticImage((UOSInt)width, (UOSInt)height, 0, 24, Media::PF_R8G8B8, 0, 0, Media::ColorProfile::YUVT_UNKNOWN, aType, Media::YCOFST_C_CENTER_LEFT));
			UInt8 *imgDest = (UInt8*)img->data;
			if (imgDest)
			{
				ImageCopy_ImgCopy(imgPtr, imgDest, (UOSInt)width * 3, (UOSInt)height, bpl, img->GetDataBpl());
				NEW_CLASS(imgList, Media::ImageList(fd->GetFullName()));
				imgList->AddImage(img, 0);
			}
			else
			{
				DEL_CLASS(img);
				img = 0;
			}
		}
		else if (nChannels == 3 && bps == 8 && hasAlpha)
		{
			NEW_CLASS(img, Media::StaticImage((UOSInt)width, (UOSInt)height, 0, 32, Media::PF_R8G8B8A8, 0, 0, Media::ColorProfile::YUVT_UNKNOWN, aType, Media::YCOFST_C_CENTER_LEFT));
			UInt8 *imgDest = (UInt8*)img->data;
			if (imgDest)
			{
				ImageCopy_ImgCopy(imgPtr, imgDest, (UOSInt)width * 4, (UOSInt)height, bpl, img->GetDataBpl());
				NEW_CLASS(imgList, Media::ImageList(fd->GetFullName()));
				imgList->AddImage(img, 0);
			}
			else
			{
				DEL_CLASS(img);
				img = 0;
			}
		}
		else if (nChannels == 4 && bps == 8 && hasAlpha)
		{
			NEW_CLASS(img, Media::StaticImage((UOSInt)width, (UOSInt)height, 0, 32, Media::PF_R8G8B8A8, 0, 0, Media::ColorProfile::YUVT_UNKNOWN, aType, Media::YCOFST_C_CENTER_LEFT));
			UInt8 *imgDest = (UInt8*)img->data;
			if (imgDest)
			{
				ImageCopy_ImgCopy(imgPtr, imgDest, (UOSInt)width * 4, (UOSInt)height, bpl, img->GetDataBpl());
				NEW_CLASS(imgList, Media::ImageList(fd->GetFullName()));
				imgList->AddImage(img, 0);
			}
			else
			{
				DEL_CLASS(img);
				img = 0;
			}
		}
		else
		{
			wprintf(L"GUIImgParser: unsupport: width = %d, height = %d, nChannels = %d, bps = %d, bpl = %d, xdpi = %lf, ydpi = %lf\r\n", width, height, nChannels, bps, bpl, xdpi, ydpi);
		}

		if (img)
		{
			img->info->hdpi = xdpi;
			img->info->vdpi = ydpi;
			if (isImage == 2)
			{
				Media::JPEGFile::ParseJPEGHeader(fd, img, imgList, this->parsers);
			}
		}
		g_object_unref(pixBuf);
	}
	g_input_stream_close(inpStream, 0, 0);
	MemFree(data);

	if (targetType != IO::ParsedObject::PT_IMAGE_LIST_PARSER && imgList && imgList->GetCount() >= 1)
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
			
			NEW_CLASS(lyr, Map::VectorLayer(Map::DRAW_LAYER_IMAGE, fd->GetFullName(), 0, 0, Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::GeographicCoordinateSystem::GCST_WGS84), 0, 0, 0, 0, 0));
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
			OSInt i = sb.LastIndexOf(IO::Path::PATH_SEPERATOR);
			OSInt j = sb.LastIndexOf('.');
			if (j > i)
			{
				sb.RemoveChars(sb.GetCharCnt() - (UOSInt)j);
			}
			sb.Append((const UTF8Char*)".tfw");
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
					NEW_CLASS(simg, Media::SharedImage(imgList, true));
					NEW_CLASS(vimg, Math::VectorImage(csys->GetSRID(), simg, xCoord - xPxSize * 0.5, yCoord + yPxSize * (Math::UOSInt2Double(img->info->dispHeight) - 0.5), xCoord + xPxSize * (Math::UOSInt2Double(img->info->dispWidth) - 0.5), yCoord - yPxSize * 0.5, false, fd->GetFullName(), 0, 0));
					lyr->AddVector(vimg, 0);
					DEL_CLASS(simg);
					
					return lyr;
				}
			}
		}
	}
	return imgList;
}

