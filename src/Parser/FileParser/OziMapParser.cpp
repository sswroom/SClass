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
#include "Math/VectorImage.h"
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

void Parser::FileParser::OziMapParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MapLayer)
	{
		selector->AddFilter((const UTF8Char*)"*.map", (const UTF8Char*)"OziExplorer Map File");
	}
}

IO::ParserType Parser::FileParser::OziMapParser::GetParserType()
{
	return IO::ParserType::MapLayer;
}

IO::ParsedObject *Parser::FileParser::OziMapParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UInt8 buff[40];
	UTF8Char sbuff[1024];
	UTF8Char *sptr;
	const UTF8Char *fileName = 0;
	UTF8Char *tmpArr[6];
	Map::VectorLayer *lyr = 0;
	Bool valid;

	if (this->parsers == 0)
		return 0;

	fd->GetRealData(0, 34, buff);
	buff[34] = 0;
	if (!Text::StrEqualsC(buff, 34, UTF8STRC("OziExplorer Map Data File Version ")))
		return 0;

	IO::StreamDataStream *stm;
	Text::UTF8Reader *reader;
	NEW_CLASS(stm, IO::StreamDataStream(fd));
	NEW_CLASS(reader, Text::UTF8Reader(stm));

	valid = true;
	sptr = reader->ReadLine(sbuff, 1024);
	if (!Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("OziExplorer Map Data File Version ")))
	{
		valid = false;
	}
	sptr = reader->ReadLine(sbuff, 1024); //File Name
	if (sptr)
	{
		fileName = Text::StrCopyNewC(sbuff, (UOSInt)(sptr - sbuff));
	}
	else
	{
		valid = false;
	}
	reader->ReadLine(sbuff, 1024); //Full Path
	reader->ReadLine(sbuff, 1024); //?
	sptr = reader->ReadLine(sbuff, 1024);
	if (sptr == 0 || !Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("WGS 84")))
	{
		valid = false;
	}
	sptr = reader->ReadLine(sbuff, 1024);
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
		while ((sptr = reader->ReadLine(sbuff, 1024)) != 0)
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
			fd->GetFullFileName()->ConcatTo(sbuff);
			sptr = IO::Path::AppendPath(sbuff, fileName);
			NEW_CLASS(imgFd, IO::StmData::FileData({sbuff, (UOSInt)(sptr - sbuff)}, false));
			imgList = (Media::ImageList*)this->parsers->ParseFileType(imgFd, IO::ParserType::ImageList);
			DEL_CLASS(imgFd);

			if (imgList)
			{
				Math::PointMappingCoordinateSystem *csys = 0;
				Math::VectorImage *vimg;
				Media::SharedImage *shimg;
				NEW_CLASS(csys, Math::PointMappingCoordinateSystem(fd->GetFullName(), 4326, (const UTF8Char*)"PointMapping", Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84)));
				currPt = 0;
				while (currPt < nPts)
				{
					csys->AddMappingPoint(ptXY[(currPt << 2) + 0], ptXY[(currPt << 2) + 1], ptXY[(currPt << 2) + 2], ptXY[(currPt << 2) + 3]);
					currPt++;
				}
				NEW_CLASS(shimg, Media::SharedImage(imgList, true));
				NEW_CLASS(vimg, Math::VectorImage(csys->GetSRID(), shimg, 0, 0, imgW, imgH, false, sbuff, 0, 0));
				UOSInt i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
				Text::String *s = Text::String::NewNotNull(&sbuff[i + 1]);
				NEW_CLASS(lyr, Map::VectorLayer(Map::DRAW_LAYER_IMAGE, fd->GetFullName(), 0, (const UTF8Char**)0, csys, 0, s));
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
	SDEL_TEXT(fileName);
	DEL_CLASS(reader);
	DEL_CLASS(stm);
	return lyr;
}
