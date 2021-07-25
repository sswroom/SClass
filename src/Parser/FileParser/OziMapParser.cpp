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

void Parser::FileParser::OziMapParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_MAP_LAYER_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.map", (const UTF8Char*)"OziExplorer Map File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::OziMapParser::GetParserType()
{
	return IO::ParsedObject::PT_MAP_LAYER_PARSER;
}

IO::ParsedObject *Parser::FileParser::OziMapParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt8 buff[40];
	UTF8Char sbuff[1024];
	const UTF8Char *fileName;
	UTF8Char *tmpArr[6];
	Map::VectorLayer *lyr = 0;
	Bool valid;

	if (this->parsers == 0)
		return 0;

	fd->GetRealData(0, 34, buff);
	buff[34] = 0;
	if (Text::StrCompareICase((Char*)buff, "OziExplorer Map Data File Version ") != 0)
		return 0;

	IO::StreamDataStream *stm;
	Text::UTF8Reader *reader;
	NEW_CLASS(stm, IO::StreamDataStream(fd));
	NEW_CLASS(reader, Text::UTF8Reader(stm));

	valid = true;
	reader->ReadLine(sbuff, 1024);
	if (!Text::StrStartsWith(sbuff, (const UTF8Char*)"OziExplorer Map Data File Version "))
	{
		valid = false;
	}
	reader->ReadLine(sbuff, 1024); //File Name
	fileName = Text::StrCopyNew(sbuff);
	reader->ReadLine(sbuff, 1024); //Full Path
	reader->ReadLine(sbuff, 1024); //?
	reader->ReadLine(sbuff, 1024);
	if (!Text::StrStartsWith(sbuff, (const UTF8Char*)"WGS 84"))
	{
		valid = false;
	}
	reader->ReadLine(sbuff, 1024);
	if (!Text::StrStartsWith(sbuff, (const UTF8Char*)"Reserved "))
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
		while (reader->ReadLine(sbuff, 1024))
		{
			if (Text::StrStartsWith(sbuff, (const UTF8Char*)"MMPNUM,"))
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
			else if (Text::StrStartsWith(sbuff, (const UTF8Char*)"MMPXY,"))
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
			else if (Text::StrStartsWith(sbuff, (const UTF8Char*)"MMPLL,"))
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
			else if (Text::StrStartsWith(sbuff, (const UTF8Char*)"IWH,"))
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
			Text::StrConcat(sbuff, fd->GetFullFileName());
			IO::Path::AppendPath(sbuff, fileName);
			NEW_CLASS(imgFd, IO::StmData::FileData(sbuff, false));
			imgList = (Media::ImageList*)this->parsers->ParseFileType(imgFd, IO::ParsedObject::PT_IMAGE_LIST_PARSER);
			DEL_CLASS(imgFd);

			if (imgList)
			{
				Math::PointMappingCoordinateSystem *csys = 0;
				Math::VectorImage *vimg;
				Media::SharedImage *shimg;
				NEW_CLASS(csys, Math::PointMappingCoordinateSystem(fd->GetFullName(), 4326, (const UTF8Char*)"PointMapping", Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::GeographicCoordinateSystem::GCST_WGS84)));
				currPt = 0;
				while (currPt < nPts)
				{
					csys->AddMappingPoint(ptXY[(currPt << 2) + 0], ptXY[(currPt << 2) + 1], ptXY[(currPt << 2) + 2], ptXY[(currPt << 2) + 3]);
					currPt++;
				}
				NEW_CLASS(shimg, Media::SharedImage(imgList, true));
				NEW_CLASS(vimg, Math::VectorImage(csys->GetSRID(), shimg, 0, 0, imgW, imgH, false, sbuff, 0, 0));
				UOSInt i = Text::StrLastIndexOf(sbuff, IO::Path::PATH_SEPERATOR);
				NEW_CLASS(lyr, Map::VectorLayer(Map::DRAW_LAYER_IMAGE, fd->GetFullName(), 0, (const UTF8Char**)0, csys, 0, &sbuff[i + 1]));
				lyr->AddVector(vimg, 0);
			}
		}
		if (ptXY)
		{
			MemFree(ptXY);
			MemFree(ptStatus);
		}
	}
	Text::StrDelNew(fileName);
	DEL_CLASS(reader);
	DEL_CLASS(stm);
	return lyr;
}
