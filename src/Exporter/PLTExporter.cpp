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

IO::FileExporter::SupportType Exporter::PLTExporter::IsObjectSupported(NN<IO::ParsedObject> pobj)
{
	if (pobj->GetParserType() != IO::ParserType::MapLayer)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	NN<Map::MapDrawLayer> layer = NN<Map::MapDrawLayer>::ConvertFrom(pobj);
	if (layer->GetObjectClass() != Map::MapDrawLayer::OC_GPS_TRACK)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	return IO::FileExporter::SupportType::NormalStream;
}

Bool Exporter::PLTExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("OziExplorer Track file"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.plt"));
		return true;
	}
	return false;
}

Bool Exporter::PLTExporter::ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
{
	if (pobj->GetParserType() != IO::ParserType::MapLayer)
	{
		return false;
	}
	NN<Map::MapDrawLayer> layer = NN<Map::MapDrawLayer>::ConvertFrom(pobj);
	if (layer->GetObjectClass() != Map::MapDrawLayer::OC_GPS_TRACK)
		return false;
	NN<Map::GPSTrack> track = NN<Map::GPSTrack>::ConvertFrom(layer);
	UTF8Char sbuff[256];
	UTF8Char *sptr;

	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	Map::GPSTrack::GPSRecord3 *recs;
	Data::DateTime dt;
	Data::DateTime refTime;
	
	Text::UTF8Writer writer(stm);
	refTime.SetValue(1899, 12, 30, 0, 0, 0, 0);

	writer.WriteLine(CSTR("OziExplorer Track Point File Version 2.0"));
	writer.WriteLine(CSTR("WGS 84"));
	writer.WriteLine(CSTR("Altitude is in Feet"));
	writer.WriteLine(CSTR("Reserved 3"));
	writer.WriteLine(CSTR("0,2,255,Ozi Track Log File,1")); //0,<line width>,<line color>,<description>,<track skip value>,<type: 0=normal, 10=closed polygon, 20=alarm zone>,<fill style>,<fill color>
	writer.WriteLine(CSTR("0")); //number of points
	
	i = 0;
	j = track->GetTrackCnt();
	while (i < j)
	{
		k = 0;
		recs = track->GetTrack(i, l);
		while (k < l)
		{
			sptr = FixDouble(sbuff, recs[k].pos.GetLat(), "0.000000", 11);
			sptr = Text::StrConcatC(sptr, UTF8STRC(","));
			sptr = FixDouble(sptr, recs[k].pos.GetLon(), "0.000000", 11);
			sptr = Text::StrConcatC(sptr, UTF8STRC(","));
			if (k == 0)
			{
				sptr = Text::StrConcatC(sptr, UTF8STRC("1,"));
			}
			else
			{
				sptr = Text::StrConcatC(sptr, UTF8STRC("0,"));
			}
			sptr = FixDouble(sptr, recs[k].altitude * 3.2808333333333333333333333333333, "0", 7);
			sptr = Text::StrConcatC(sptr, UTF8STRC(","));
			dt.SetInstant(recs[k].recTime);
			sptr = Text::StrDoubleFmt(sptr, (Double)dt.DiffMS(refTime) / 86400000.0, "0.0000000");
			sptr = Text::StrConcatC(sptr, UTF8STRC(", "));
			sptr = dt.ToString(sptr, "dd-MMM-yy");
			sptr = Text::StrConcatC(sptr, UTF8STRC(", "));
			sptr = dt.ToString(sptr, "hh:mm:ss tt");

			writer.WriteLine(CSTRP(sbuff, sptr));

			k++;
		}

		i++;
	}
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
