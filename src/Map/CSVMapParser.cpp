#include "Stdafx.h"
#include "IO/StreamReader.h"
#include "Map/CSVMapParser.h"
#include "Map/VectorLayer.h"
#include "Math/Point.h"
#include "Text/UTF8Reader.h"

Map::IMapDrawLayer *Map::CSVMapParser::ParseAsPoint(IO::Stream *stm, UInt32 codePage, Text::CString layerName, Text::CString nameCol, Text::CString latCol, Text::CString lonCol, Math::CoordinateSystem *csys)
{
	Text::PString tmpArr[2];
	const UTF8Char **tmpcArr2;
	UOSInt colCnt;
	UOSInt totalCnt;
	UOSInt latIndex = INVALID_INDEX;
	UOSInt lonIndex = INVALID_INDEX;
	UOSInt nameIndex = INVALID_INDEX;
	UTF8Char sbuff[2048];
	Data::ArrayList<const UTF8Char *> colNames;
	IO::Reader *reader;
	if (codePage == 65001)
	{
		NEW_CLASS(reader, Text::UTF8Reader(stm));
	}
	else
	{
		NEW_CLASS(reader, IO::StreamReader(stm, codePage));
	}
	reader->ReadLine(sbuff, 2048);
	colCnt = Text::StrCSVSplitP(tmpArr, 2, sbuff);
	totalCnt = 0;
	while (true)
	{
		colNames.Add(tmpArr[0].v);
		
		if (tmpArr[0].Equals(latCol.v, latCol.leng))
		{
			latIndex = totalCnt;
		}
		else if (tmpArr[0].Equals(lonCol.v, lonCol.leng))
		{
			lonIndex = totalCnt;
		}
		else if (tmpArr[0].Equals(nameCol.v, nameCol.leng))
		{
			nameIndex = totalCnt;
		}
		totalCnt += 1;
		if (colCnt < 2)
			break;
		colCnt = Text::StrCSVSplitP(tmpArr, 2, tmpArr[1].v);
	}

	if (latIndex != INVALID_INDEX && lonIndex != INVALID_INDEX)
	{
		Map::VectorLayer *lyr;
		Math::Point *pt;
		UOSInt i;

		tmpcArr2 = MemAlloc(const UTF8Char*, totalCnt + 1);
		i = totalCnt;
		while (i-- > 0)
		{
			tmpcArr2[i] = colNames.GetItem(i);
		}
		NEW_CLASS(lyr, Map::VectorLayer(Map::DRAW_LAYER_POINT, layerName, totalCnt, tmpcArr2, csys, nameIndex, layerName));
		
		UTF8Char **tmpUArr2 = (UTF8Char**)tmpcArr2;
		while (reader->ReadLine(sbuff, 2048))
		{
			if (totalCnt == Text::StrCSVSplit(tmpUArr2, totalCnt + 1, sbuff))
			{
				NEW_CLASS(pt, Math::Point(csys->GetSRID(), Text::StrToDouble(tmpUArr2[lonIndex]), Text::StrToDouble(tmpUArr2[latIndex])));
				lyr->AddVector(pt, (const UTF8Char**)tmpUArr2);
			}
		}		

		MemFree(tmpcArr2);
		DEL_CLASS(reader);
		return lyr;
	}
	SDEL_CLASS(csys);
	DEL_CLASS(reader);
	return 0;
}
