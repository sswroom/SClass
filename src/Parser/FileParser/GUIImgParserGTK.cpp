#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteOrderLSB.h"
#include "Data/ByteOrderMSB.h"
#include "Core/ByteTool_C.h"
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
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include <gdk/gdk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <stdio.h>

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
	this->parsers = parsers;
}

void Parser::FileParser::GUIImgParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::ImageList)
	{
		selector->AddFilter(CSTR("*.tif"), CSTR("TIFF(GDK) File"));
		selector->AddFilter(CSTR("*.tiff"), CSTR("TIFF(GDK) File"));
		selector->AddFilter(CSTR("*.gif"), CSTR("GIF(GDK) File"));
		selector->AddFilter(CSTR("*.png"), CSTR("PNG(GDK) File"));
		selector->AddFilter(CSTR("*.jpg"), CSTR("JPG(GDK) File"));
		selector->AddFilter(CSTR("*.jpeg"), CSTR("JPG(GDK) File"));
	}
}

IO::ParserType Parser::FileParser::GUIImgParser::GetParserType()
{
	return IO::ParserType::ImageList;
}

Optional<IO::ParsedObject> Parser::FileParser::GUIImgParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
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
		return nullptr;

	Optional<Media::ImageList> imgList = nullptr;
	NN<Media::ImageList> nnimgList;
	UInt64 dataSize = fd->GetDataSize();
	{
		Data::ByteBuffer data((UIntOS)dataSize);
		fd->GetRealData(0, (UIntOS)dataSize, data);
		GInputStream *inpStream = g_memory_input_stream_new_from_data(data.Arr().Ptr(), (gssize)dataSize, 0);
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
			UInt32 bpl = (UInt32)gdk_pixbuf_get_rowstride(pixBuf);
			Double xdpi = 72.0;
			Double ydpi = 72.0;
			const gchar *cptr;
			cptr = gdk_pixbuf_get_option(pixBuf, "x-dpi");
			if (cptr) Text::StrToDoubleCh(cptr, xdpi);
			cptr = gdk_pixbuf_get_option(pixBuf, "y-dpi");
			if (cptr) Text::StrToDoubleCh(cptr, ydpi);

			Optional<Media::StaticImage> optimg = nullptr;
			NN<Media::StaticImage> img;
			Media::AlphaType aType = (isImage == 2||!hasAlpha)?Media::AT_IGNORE_ALPHA:Media::AT_ALPHA;

			if (nChannels == 3 && bps == 8 && !hasAlpha)
			{
				NEW_CLASSNN(img, Media::StaticImage(Math::Size2D<UIntOS>((UIntOS)width, (UIntOS)height), 0, 24, Media::PF_R8G8B8, 0, Media::ColorProfile(), Media::ColorProfile::YUVT_UNKNOWN, aType, Media::YCOFST_C_CENTER_LEFT));
				UnsafeArray<UInt8> imgDest = img->data;
				ImageCopy_ImgCopyR(imgPtr, imgDest.Ptr(), (UIntOS)width * 3, (UIntOS)height, bpl, img->GetDataBpl(), img->IsUpsideDown());
				NEW_CLASSNN(nnimgList, Media::ImageList(fd->GetFullName()));
				nnimgList->AddImage(img, 0);
				imgList = nnimgList;
				optimg = img;
			}
			else if (nChannels == 3 && bps == 8 && hasAlpha)
			{
				NEW_CLASSNN(img, Media::StaticImage(Math::Size2D<UIntOS>((UIntOS)width, (UIntOS)height), 0, 32, Media::PF_R8G8B8A8, 0, Media::ColorProfile(), Media::ColorProfile::YUVT_UNKNOWN, aType, Media::YCOFST_C_CENTER_LEFT));
				UnsafeArray<UInt8> imgDest = img->data;
				ImageCopy_ImgCopyR(imgPtr, imgDest.Ptr(), (UIntOS)width * 4, (UIntOS)height, bpl, img->GetDataBpl(), img->IsUpsideDown());
				NEW_CLASSNN(nnimgList, Media::ImageList(fd->GetFullName()));
				nnimgList->AddImage(img, 0);
				imgList = nnimgList;
				optimg = img;
			}
			else if (nChannels == 4 && bps == 8 && hasAlpha)
			{
				NEW_CLASSNN(img, Media::StaticImage(Math::Size2D<UIntOS>((UIntOS)width, (UIntOS)height), 0, 32, Media::PF_R8G8B8A8, 0, Media::ColorProfile(), Media::ColorProfile::YUVT_UNKNOWN, aType, Media::YCOFST_C_CENTER_LEFT));
				UnsafeArray<UInt8> imgDest = img->data;
				ImageCopy_ImgCopyR(imgPtr, imgDest.Ptr(), (UIntOS)width * 4, (UIntOS)height, bpl, img->GetDataBpl(), img->IsUpsideDown());
				NEW_CLASSNN(nnimgList, Media::ImageList(fd->GetFullName()));
				nnimgList->AddImage(img, 0);
				imgList = nnimgList;
				optimg = img;
			}
			else
			{
				printf("GUIImgParser: unsupport: width = %d, height = %d, nChannels = %d, bps = %d, bpl = %d, xdpi = %lf, ydpi = %lf\r\n", width, height, nChannels, bps, bpl, xdpi, ydpi);
			}

			if (optimg.SetTo(img) && imgList.SetTo(nnimgList))
			{
				img->info.hdpi = xdpi;
				img->info.vdpi = ydpi;
				if (isImage == 2)
				{
					Media::JPEGFile::ParseJPEGHeader(fd, img, nnimgList, this->parsers);
				}
				else if (isImage == 4 && img->exif.IsNull())
				{
					NN<Data::ByteOrder> bo;
					UInt64 nextOfst;
				
					if (*(Int16*)&data[0] == *(Int16*)"MM")
					{
						NEW_CLASSNN(bo, Data::ByteOrderMSB());
					}
					else
					{
						NEW_CLASSNN(bo, Data::ByteOrderLSB());
					}
					UInt16 fmt = bo->GetUInt16(data.Arr() + 2);
					if (fmt == 42)
					{
						nextOfst = bo->GetUInt32(data.Arr() + 4);
						img->exif = Media::EXIFData::ParseIFD(&data[nextOfst], data.GetSize() - nextOfst, bo, nextOfst, Media::EXIFData::EM_STANDARD, data.Ptr());
					}
					else if (fmt == 43 && bo->GetUInt16(data.Arr() + 4) == 8 && bo->GetUInt16(data.Arr() + 6) == 0) //BigTIFF
					{
						nextOfst = bo->GetUInt64(data.Arr() + 8);
						//img->exif = Media::EXIFData::ParseIFD64(&data[nextOfst], data.GetSize() - nextOfst, bo, nextOfst, Media::EXIFData::EM_STANDARD, 0);
					}
					bo.Delete();
				}
				printf("Image parsed by GUIImgParserGTK: %s\r\n", fd->GetFullName()->v.Ptr());
			}
			g_object_unref(pixBuf);
		}
		g_input_stream_close(inpStream, 0, 0);
	}

	NN<Media::StaticImage> img;
	if (targetType != IO::ParserType::ImageList && imgList.SetTo(nnimgList) && nnimgList->GetCount() >= 1 && Optional<Media::StaticImage>::ConvertFrom(nnimgList->GetImage(0, 0)).SetTo(img))
	{
		Double minX;
		Double minY;
		Double maxX;
		Double maxY;
		UInt32 srid;
		NN<Media::EXIFData> exif;
		if (img->exif.SetTo(exif) && exif->GetGeoBounds(img->info.dispSize, srid, minX, minY, maxX, maxY))
		{
			Map::VectorLayer *lyr;
			NN<Math::Geometry::VectorImage> vimg;
			NN<Media::SharedImage> simg;
			
			NN<Math::CoordinateSystem> csys;
			if (srid == 0)
			{
				csys = Math::CoordinateSystemManager::CreateCsysByCoord(Math::Coord2DDbl((minX + maxX) * 0.5, (minY + maxY) * 0.5));
				srid = csys->GetSRID();
			}
			else
			{
				csys = Math::CoordinateSystemManager::SRCreateCSysOrDef(srid);
			}
			NEW_CLASS(lyr, Map::VectorLayer(Map::DRAW_LAYER_IMAGE, fd->GetFullName(), csys, nullptr));
			Data::ArrayListNN<Media::StaticImage> prevList;
			Media::ImagePreviewTool::CreatePreviews(nnimgList, prevList, 640);
			NEW_CLASSNN(simg, Media::SharedImage(nnimgList, prevList));
			NEW_CLASSNN(vimg, Math::Geometry::VectorImage(srid, simg, Math::Coord2DDbl(minX, minY), Math::Coord2DDbl(maxX, maxY), Math::Coord2DDbl(maxX - minX, maxY - minY), false, fd->GetFullName().Ptr(), 0, 0));
			lyr->AddVector2(vimg, (Text::String**)0);
			simg.Delete();
			
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
			UIntOS i = sb.LastIndexOf(IO::Path::PATH_SEPERATOR);
			UIntOS j = sb.LastIndexOf('.');
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
			NN<Math::CoordinateSystem> csys = Math::CoordinateSystemManager::CreateCsysByCoord(Math::Coord2DDbl(xCoord + xPxSize * UIntOS2Double(img->info.dispSize.x) * 0.5, yCoord + yPxSize * UIntOS2Double(img->info.dispSize.y) * 0.5));
			
			NEW_CLASS(lyr, Map::VectorLayer(Map::DRAW_LAYER_IMAGE, fd->GetFullName(), csys, nullptr));
			Data::ArrayListNN<Media::StaticImage> prevList;
			Media::ImagePreviewTool::CreatePreviews(nnimgList, prevList, 640);
			NEW_CLASSNN(simg, Media::SharedImage(nnimgList, prevList));
			NEW_CLASSNN(vimg, Math::Geometry::VectorImage(csys->GetSRID(), simg, Math::Coord2DDbl(xCoord - xPxSize * 0.5, yCoord + yPxSize * (UIntOS2Double(img->info.dispSize.y) - 0.5)), Math::Coord2DDbl(xCoord + xPxSize * (UIntOS2Double(img->info.dispSize.x) - 0.5), yCoord - yPxSize * 0.5), Math::Coord2DDbl(xPxSize * UIntOS2Double(img->info.dispSize.x), yPxSize * UIntOS2Double(img->info.dispSize.y)), false, fd->GetFullName().Ptr(), 0, 0));
			lyr->AddVector2(vimg, (Text::String**)0);
			simg.Delete();
			
			return lyr;
		}
	}
	return imgList;
}

