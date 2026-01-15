#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "IO/Path.h"
#include "IO/StreamDataStream.h"
#include "IO/StmData/FileData.h"
#include "Map/VectorLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math_C.h"
#include "Math/PointMappingCoordinateSystem.h"
#include "Math/Geometry/VectorImage.h"
#include "Media/ImageList.h"
#include "Media/ImagePreviewTool.h"
#include "Parser/ParserList.h"
#include "Parser/FileParser/OziMapParser.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/UTF8Reader.h"

Parser::FileParser::OziMapParser::OziMapParser()
{
	this->parsers = nullptr;
}

Parser::FileParser::OziMapParser::~OziMapParser()
{
}

Int32 Parser::FileParser::OziMapParser::GetName()
{
	return *(Int32*)"OMAP";
}

void Parser::FileParser::OziMapParser::SetParserList(Optional<Parser::ParserList> parsers)
{
	this->parsers = parsers;
}

void Parser::FileParser::OziMapParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MapLayer)
	{
		selector->AddFilter(CSTR("*.map"), CSTR("OziExplorer Map File"));
	}
}

IO::ParserType Parser::FileParser::OziMapParser::GetParserType()
{
	return IO::ParserType::MapLayer;
}

Optional<IO::ParsedObject> Parser::FileParser::OziMapParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	UTF8Char sbuff[1024];
	UnsafeArray<UTF8Char> sptr;
	Text::String *fileName = 0;
	UnsafeArray<UTF8Char> tmpArr[6];
	Map::VectorLayer *lyr = 0;
	Bool valid;
	NN<Parser::ParserList> parsers;

	if (!this->parsers.SetTo(parsers))
		return nullptr;

	if (!Text::StrEqualsC(&hdr[0], 34, UTF8STRC("OziExplorer Map Data File Version ")))
		return nullptr;

	IO::StreamDataStream stm(fd);
	Text::UTF8Reader reader(stm);

	valid = true;
	if (!reader.ReadLine(sbuff, 1024).SetTo(sptr) || !Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("OziExplorer Map Data File Version ")))
	{
		valid = false;
	}
	if (reader.ReadLine(sbuff, 1024).SetTo(sptr))
	{
		fileName = Text::String::New(sbuff, (UOSInt)(sptr - sbuff)).Ptr();
	}
	else
	{
		valid = false;
	}
	reader.ReadLine(sbuff, 1024); //Full Path
	reader.ReadLine(sbuff, 1024); //?
	if (!reader.ReadLine(sbuff, 1024).SetTo(sptr) || !Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("WGS 84")))
	{
		valid = false;
	}
	if (!reader.ReadLine(sbuff, 1024).SetTo(sptr) || !Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Reserved ")))
	{
		valid = false;
	}
	if (valid)
	{
		UInt32 currPt;
		UInt32 nPts = 0;
		Double *ptXY = 0;
		Int32 *ptStatus = 0;
		Double imgW = 0;
		Double imgH = 0;
		while (reader.ReadLine(sbuff, 1024).SetTo(sptr))
		{
			if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("MMPNUM,")))
			{
				if (ptXY == 0)
				{
					nPts = Text::StrToUInt32(&sbuff[7]);
					if (nPts > 0 && nPts <= 10)
					{
						ptXY = MemAlloc(Double, nPts << 2);
						ptStatus = MemAlloc(Int32, nPts);
						currPt = nPts;
						while (currPt-- > 0)
						{
							ptStatus[currPt] = 0;
						}
					}
				}
			}
			else if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("MMPXY,")))
			{
				if (ptXY && Text::StrSplitTrim(tmpArr, 6, sbuff, ',') == 4)
				{
					currPt = Text::StrToUInt32(tmpArr[1]) - 1;
					if ((Int32)currPt >= 0 && currPt < nPts)
					{
						ptXY[(currPt << 2)] = Text::StrToDoubleOrNAN(tmpArr[2]);
						ptXY[(currPt << 2) + 1] = Text::StrToDoubleOrNAN(tmpArr[3]);
						ptStatus[currPt] |= 1;
					}
				}
			}
			else if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("MMPLL,")))
			{
				if (ptXY && Text::StrSplitTrim(tmpArr, 6, sbuff, ',') == 4)
				{
					currPt = Text::StrToUInt32(tmpArr[1]) - 1;
					if ((Int32)currPt >= 0 && currPt <nPts)
					{
						ptXY[(currPt << 2) + 2] = Text::StrToDoubleOrNAN(tmpArr[2]);
						ptXY[(currPt << 2) + 3] = Text::StrToDoubleOrNAN(tmpArr[3]);
						ptStatus[currPt] |= 2;
					}
				}
			}
			else if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("IWH,")))
			{
				if (Text::StrSplitTrim(tmpArr, 6, sbuff, ',') == 4)
				{
					imgW = Text::StrToDoubleOrNAN(tmpArr[2]);
					imgH = Text::StrToDoubleOrNAN(tmpArr[3]);
				}
			}
		}
		if (ptXY == 0)
		{
			valid = false;
		}
		else
		{
			currPt = nPts;
			while (currPt-- > 0)
			{
				if (ptStatus[currPt] != 3)
					valid = false;
				break;
			}
		}
		if (imgW == 0 || imgH == 0)
		{
			valid = false;
		}

		if (valid)
		{
			Optional<Media::ImageList> imgList = nullptr;
			NN<Media::ImageList> nnimgList;
			sptr = fd->GetFullFileName()->ConcatTo(sbuff);
			sptr = IO::Path::AppendPath(sbuff, sptr, fileName->ToCString());
			{
				IO::StmData::FileData imgFd(CSTRP(sbuff, sptr), false);
				imgList = Optional<Media::ImageList>::ConvertFrom(parsers->ParseFileType(imgFd, IO::ParserType::ImageList));
			}

			if (imgList.SetTo(nnimgList))
			{
				NN<Math::PointMappingCoordinateSystem> csys;
				NN<Math::Geometry::VectorImage> vimg;
				NN<Media::SharedImage> shimg;
				NN<Math::GeographicCoordinateSystem> gcs;
				if (Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84).SetTo(gcs))
				{
					NEW_CLASSNN(csys, Math::PointMappingCoordinateSystem(fd->GetFullName(), 4326, CSTR("PointMapping"), gcs));
					currPt = 0;
					while (currPt < nPts)
					{
						csys->AddMappingPoint(ptXY[(currPt << 2) + 0], ptXY[(currPt << 2) + 1], ptXY[(currPt << 2) + 2], ptXY[(currPt << 2) + 3]);
						currPt++;
					}
					Data::ArrayListNN<Media::StaticImage> prevList;
					Media::ImagePreviewTool::CreatePreviews(nnimgList, prevList, 640);
					NEW_CLASSNN(shimg, Media::SharedImage(nnimgList, prevList));
					NEW_CLASSNN(vimg, Math::Geometry::VectorImage(csys->GetSRID(), shimg, Math::Coord2DDbl(0, 0), Math::Coord2DDbl(imgW, imgH), false, CSTRP(sbuff, sptr), 0, 0));
					UOSInt i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
					NN<Text::String> s = Text::String::New(&sbuff[i + 1], (UOSInt)(sptr - &sbuff[i + 1]));
					NEW_CLASS(lyr, Map::VectorLayer(Map::DRAW_LAYER_IMAGE, fd->GetFullName(), csys, s.Ptr()));
					s->Release();
					lyr->AddVector2(vimg, (Text::PString*)0);
				}
			}
		}
		if (ptXY)
		{
			MemFree(ptXY);
			MemFree(ptStatus);
		}
	}
	SDEL_STRING(fileName);
	return lyr;
}
