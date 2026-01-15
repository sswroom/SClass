#include "Stdafx.h"
#include "IO/StreamReader.h"
#include "Map/CSVMapParser.h"
#include "Map/VectorLayer.h"
#include "Math/Geometry/Point.h"
#include "Text/UTF8Reader.h"

Map::MapDrawLayer *Map::CSVMapParser::ParseAsPoint(NN<IO::Stream> stm, UInt32 codePage, Text::CStringNN layerName, Text::CStringNN nameCol, Text::CStringNN latCol, Text::CStringNN lonCol, NN<Math::CoordinateSystem> csys)
{
	Text::PString tmpArr[2];
	UnsafeArray<UnsafeArrayOpt<const UTF8Char>> tmpcArr2;
	UIntOS colCnt;
	UIntOS totalCnt;
	UIntOS latIndex = INVALID_INDEX;
	UIntOS lonIndex = INVALID_INDEX;
	UIntOS nameIndex = INVALID_INDEX;
	UTF8Char sbuff[2048];
	Data::ArrayListObj<UnsafeArrayOpt<const UTF8Char>> colNames;
	NN<IO::Reader> reader;
	if (codePage == 65001)
	{
		NEW_CLASSNN(reader, Text::UTF8Reader(stm));
	}
	else
	{
		NEW_CLASSNN(reader, IO::StreamReader(stm, codePage));
	}
	reader->ReadLine(sbuff, 2048);
	colCnt = Text::StrCSVSplitP(tmpArr, 2, sbuff);
	totalCnt = 0;
	while (true)
	{
		colNames.Add(UnsafeArrayOpt<const UTF8Char>(tmpArr[0].v));
		
		if (tmpArr[0].Equals(latCol))
		{
			latIndex = totalCnt;
		}
		else if (tmpArr[0].Equals(lonCol))
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
		NN<Math::Geometry::Point> pt;
		UIntOS i;

		tmpcArr2 = MemAllocArr(UnsafeArrayOpt<const UTF8Char>, totalCnt + 1);
		i = totalCnt;
		while (i-- > 0)
		{
			tmpcArr2[i] = colNames.GetItem(i);
		}
		NEW_CLASS(lyr, Map::VectorLayer(Map::DRAW_LAYER_POINT, layerName, totalCnt, tmpcArr2, csys, nameIndex, layerName));
		
		UnsafeArray<UnsafeArray<UTF8Char>> tmpUArr2 = UnsafeArray<UnsafeArray<UTF8Char>>::ConvertFrom(tmpcArr2);
		while (reader->ReadLine(sbuff, 2048).NotNull())
		{
			if (totalCnt == Text::StrCSVSplit(tmpUArr2, totalCnt + 1, sbuff))
			{
				NEW_CLASSNN(pt, Math::Geometry::Point(csys->GetSRID(), Text::StrToDoubleOrNAN(tmpUArr2[lonIndex]), Text::StrToDoubleOrNAN(tmpUArr2[latIndex])));
				lyr->AddVector2(pt, UnsafeArray<UnsafeArrayOpt<const UTF8Char>>::ConvertFrom(tmpUArr2));
			}
		}		

		MemFreeArr(tmpcArr2);
		reader.Delete();
		return lyr;
	}
	csys.Delete();
	reader.Delete();
	return 0;
}
