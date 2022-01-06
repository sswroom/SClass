#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "Exporter/PLTExporter.h"
#include "Map/GPSTrack.h"
#include "Math/Math.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/UTF8Writer.h"

Exporter::PLTExporter::PLTExporter()
{
}

Exporter::PLTExporter::~PLTExporter()
{
}

Int32 Exporter::PLTExporter::GetName()
{
	//OziExplorer Track file
	return *(Int32*)"PLTE";
}

IO::FileExporter::SupportType Exporter::PLTExporter::IsObjectSupported(IO::ParsedObject *pobj)
{
	if (pobj->GetParserType() != IO::ParserType::MapLayer)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	Map::IMapDrawLayer *layer = (Map::IMapDrawLayer *)pobj;
	if (layer->GetObjectClass() != Map::IMapDrawLayer::OC_GPS_TRACK)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	return IO::FileExporter::SupportType::NormalStream;
}

Bool Exporter::PLTExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcat(nameBuff, (const UTF8Char*)"OziExplorer Track file");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.plt");
		return true;
	}
	return false;
}

Bool Exporter::PLTExporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
{
	if (pobj->GetParserType() != IO::ParserType::MapLayer)
	{
		return false;
	}
	Map::IMapDrawLayer *layer = (Map::IMapDrawLayer *)pobj;
	if (layer->GetObjectClass() != Map::IMapDrawLayer::OC_GPS_TRACK)
		return false;
	Map::GPSTrack *track = (Map::GPSTrack*)layer;
	UTF8Char sbuff[256];
	UTF8Char *sptr;

	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	Text::UTF8Writer *writer;
	Map::GPSTrack::GPSRecord *recs;
	Data::DateTime dt;
	Data::DateTime refTime;
	
	NEW_CLASS(writer, Text::UTF8Writer(stm));
	refTime.SetValue(1899, 12, 30, 0, 0, 0, 0);

	writer->WriteLineC(UTF8STRC("OziExplorer Track Point File Version 2.0"));
	writer->WriteLineC(UTF8STRC("WGS 84"));
	writer->WriteLineC(UTF8STRC("Altitude is in Feet"));
	writer->WriteLineC(UTF8STRC("Reserved 3"));
	writer->WriteLineC(UTF8STRC("0,2,255,Ozi Track Log File,1")); //0,<line width>,<line color>,<description>,<track skip value>,<type: 0=normal, 10=closed polygon, 20=alarm zone>,<fill style>,<fill color>
	writer->WriteLineC(UTF8STRC("0")); //number of points
	
	i = 0;
	j = track->GetTrackCnt();
	while (i < j)
	{
		k = 0;
		recs = track->GetTrack(i, &l);
		while (k < l)
		{
			sptr = FixDouble(sbuff, recs[k].lat, "0.000000", 11);
			sptr = Text::StrConcat(sptr, (const UTF8Char*)",");
			sptr = FixDouble(sptr, recs[k].lon, "0.000000", 11);
			sptr = Text::StrConcat(sptr, (const UTF8Char*)",");
			if (k == 0)
			{
				sptr = Text::StrConcat(sptr, (const UTF8Char*)"1,");
			}
			else
			{
				sptr = Text::StrConcat(sptr, (const UTF8Char*)"0,");
			}
			sptr = FixDouble(sptr, recs[k].altitude * 3.2808333333333333333333333333333, "0", 7);
			sptr = Text::StrConcat(sptr, (const UTF8Char*)",");
			dt.SetTicks(recs[k].utcTimeTicks);
			sptr = Text::StrDoubleFmt(sptr, (Double)dt.DiffMS(&refTime) / 86400000.0, "0.0000000");
			sptr = Text::StrConcat(sptr, (const UTF8Char*)", ");
			sptr = dt.ToString(sptr, "dd-MMM-yy");
			sptr = Text::StrConcat(sptr, (const UTF8Char*)", ");
			sptr = dt.ToString(sptr, "hh:mm:ss tt");

			writer->WriteLine(sbuff);

			k++;
		}

		i++;
	}

	DEL_CLASS(writer);
	return true;
}

UTF8Char *Exporter::PLTExporter::FixDouble(UTF8Char *sbuff, Double val, const Char *format, Int32 colSize)
{
	UTF8Char sbuff2[16];
	UTF8Char *sptr;
	OSInt i;
	sptr = Text::StrDoubleFmt(sbuff2, val, format);
	i = colSize - (sptr - sbuff2);
	while (i-- > 0)
	{
		*sbuff++ = ' ';
	}
	return Text::StrConcat(sbuff, sbuff2);
}
