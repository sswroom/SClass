#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "IO/Path.h"
#include "IO/StreamDataStream.h"
#include "IO/StmData/FileData.h"
#include "Map/VectorLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math.h"
#include "Math/PointMappingCoordinateSystem.h"
#include "Math/Geometry/VectorImage.h"
#include "Media/ImageList.h"
#include "Parser/ParserList.h"
#include "Parser/FileParser/OziMapParser.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/UTF8Reader.h"

Parser::FileParser::OziMapParser::OziMapParser()
{
	this->parsers = 0;
}

Parser::FileParser::OziMapParser::~OziMapParser()
{
}

Int32 Parser::FileParser::OziMapParser::GetName()
{
	return *(Int32*)"OMAP";
}

void Parser::FileParser::OziMapParser::SetParserList(Parser::ParserList *parsers)
{
	this->parsers = parsers;
}

void Parser::FileParser::OziMapParser::PrepareSelector(IO::FileSelector *selector, IO::ParserType t)
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

IO::ParsedObject *Parser::FileParser::OziMapParser::ParseFileHdr(IO::StreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	UTF8Char sbuff[1024];
	UTF8Char *sptr;
	Text::String *fileName = 0;
	UTF8Char *tmpArr[6];
	Map::VectorLayer *lyr = 0;
	Bool valid;

	if (this->parsers == 0)
		return 0;

	if (!Text::StrEqualsC(hdr, 34, UTF8STRC("OziExplorer Map Data File Version ")))
		return 0;

	IO::StreamDataStream stm(fd);
	Text::UTF8Reader reader(stm);

	valid = true;
	sptr = reader.ReadLine(sbuff, 1024);
	if (!Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("OziExplorer Map Data File Version ")))
	{
		valid = false;
	}
	sptr = reader.ReadLine(sbuff, 1024); //File Name
	if (sptr)
	{
		fileName = Text::String::New(sbuff, (UOSInt)(sptr - sbuff)).Ptr();
	}
	else
	{
		valid = false;
	}
	reader.ReadLine(sbuff, 1024); //Full Path
	reader.ReadLine(sbuff, 1024); //?
	sptr = reader.ReadLine(sbuff, 1024);
	if (sptr == 0 || !Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("WGS 84")))
	{
		valid = false;
	}
	sptr = reader.ReadLine(sbuff, 1024);
	if (sptr == 0 || !Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Reserved ")))
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
		while ((sptr = reader.ReadLine(sbuff, 1024)) != 0)
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
						ptXY[(currPt << 2)] = Text::StrToDouble(tmpArr[2]);
						ptXY[(currPt << 2) + 1] = Text::StrToDouble(tmpArr[3]);
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
						ptXY[(currPt << 2) + 2] = Text::StrToDouble(tmpArr[2]);
						ptXY[(currPt << 2) + 3] = Text::StrToDouble(tmpArr[3]);
						ptStatus[currPt] |= 2;
					}
				}
			}
			else if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("IWH,")))
			{
				if (Text::StrSplitTrim(tmpArr, 6, sbuff, ',') == 4)
				{
					imgW = Text::StrToDouble(tmpArr[2]);
					imgH = Text::StrToDouble(tmpArr[3]);
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
			IO::StmData::FileData *imgFd;
			Media::ImageList *imgList = 0;
			sptr = fd->GetFullFileName()->ConcatTo(sbuff);
			sptr = IO::Path::AppendPath(sbuff, sptr, fileName->ToCString());
			NEW_CLASS(imgFd, IO::StmData::FileData(CSTRP(sbuff, sptr), false));
			imgList = (Media::ImageList*)this->parsers->ParseFileType(imgFd, IO::ParserType::ImageList);
			DEL_CLASS(imgFd);

			if (imgList)
			{
				Math::PointMappingCoordinateSystem *csys = 0;
				Math::Geometry::VectorImage *vimg;
				Media::SharedImage *shimg;
				NEW_CLASS(csys, Math::PointMappingCoordinateSystem(fd->GetFullName(), 4326, CSTR("PointMapping"), Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84)));
				currPt = 0;
				while (currPt < nPts)
				{
					csys->AddMappingPoint(ptXY[(currPt << 2) + 0], ptXY[(currPt << 2) + 1], ptXY[(currPt << 2) + 2], ptXY[(currPt << 2) + 3]);
					currPt++;
				}
				NEW_CLASS(shimg, Media::SharedImage(imgList, true));
				NEW_CLASS(vimg, Math::Geometry::VectorImage(csys->GetSRID(), shimg, Math::Coord2DDbl(0, 0), Math::Coord2DDbl(imgW, imgH), false, CSTRP(sbuff, sptr), 0, 0));
				UOSInt i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
				NotNullPtr<Text::String> s = Text::String::New(&sbuff[i + 1], (UOSInt)(sptr - &sbuff[i + 1]));
				NEW_CLASS(lyr, Map::VectorLayer(Map::DRAW_LAYER_IMAGE, fd->GetFullName(), 0, (const UTF8Char**)0, csys, 0, s.Ptr()));
				s->Release();
				lyr->AddVector(vimg, (const UTF8Char**)0);
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
