#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "Exporter/GPXExporter.h"
#include "IO/BufferedOutputStream.h"
#include "IO/StreamWriter.h"
#include "Map/GPSTrack.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

Exporter::GPXExporter::GPXExporter()
{
	this->codePage = 65001;
}

Exporter::GPXExporter::~GPXExporter()
{
}

Int32 Exporter::GPXExporter::GetName()
{
	return *(Int32*)"GPXE";
}

IO::FileExporter::SupportType Exporter::GPXExporter::IsObjectSupported(NN<IO::ParsedObject> pobj)
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

Bool Exporter::GPXExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("GPX file"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.gpx"));
		return true;
	}
	return false;
}

void Exporter::GPXExporter::SetCodePage(UInt32 codePage)
{
	this->codePage = codePage;
}

Bool Exporter::GPXExporter::ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
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

	Text::Encoding enc(this->codePage);
	IO::BufferedOutputStream cstm(stm, 65536);
	IO::StreamWriter writer(cstm, &enc);

	writer.WriteStrC(UTF8STRC("<?xml version=\"1.0\" encoding=\""));
	sptr = Text::EncodingFactory::GetInternetName(sbuff, this->codePage);
	writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
	writer.WriteLineC(UTF8STRC("\"?>"));
	writer.WriteLineC(UTF8STRC("<gpx version=\"v1.0\" creator=\"iTravel Tech Inc. - http://www.itravel-tech.com\">"));
	writer.WriteLineC(UTF8STRC("<trk>"));
	writer.WriteLineC(UTF8STRC("<name>Track</name>"));
	i = 0;
	j = track->GetTrackCnt();
	while (i < j)
	{
		writer.WriteLineC(UTF8STRC("<trkseg>"));
		k = 0;
		recs = track->GetTrack(i, l);
		while (k < l)
		{
			writer.WriteStrC(UTF8STRC("<trkpt lat=\""));
			sptr = Text::StrDouble(sbuff, recs[k].pos.GetLat());
			writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			writer.WriteStrC(UTF8STRC("\" lon=\""));
			sptr = Text::StrDouble(sbuff, recs[k].pos.GetLon());
			writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			writer.WriteLineC(UTF8STRC("\">"));
			
			writer.WriteStrC(UTF8STRC("<ele>"));
			sptr = Text::StrDouble(sbuff, recs[k].altitude);
			writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			writer.WriteLineC(UTF8STRC("</ele>"));

			writer.WriteStrC(UTF8STRC("<time>"));
			dt.SetInstant(recs[k].recTime);
			sptr = dt.ToString(sbuff, "yyyy-MM-ddTHH:mm:ssZ");
			writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			writer.WriteLineC(UTF8STRC("</time>"));

			writer.WriteStrC(UTF8STRC("<desc>lat.="));
			sptr = Text::StrDoubleFmt(sbuff, recs[k].pos.GetLat(), "0.000000");
			writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			writer.WriteStrC(UTF8STRC(", lon.="));
			sptr = Text::StrDoubleFmt(sbuff, recs[k].pos.GetLon(), "0.000000");
			writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			writer.WriteStrC(UTF8STRC(", Alt.="));
			sptr = Text::StrDoubleFmt(sbuff, recs[k].altitude, "0.000000");
			writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			writer.WriteStrC(UTF8STRC("m, Speed="));
			sptr = Text::StrDoubleFmt(sbuff, recs[k].speed * 1.852, "0.000000");
			writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			writer.WriteLineC(UTF8STRC("m/h.</desc>"));

			writer.WriteStrC(UTF8STRC("<speed>"));
			sptr = Text::StrDoubleFmt(sbuff, recs[k].speed * 1.852 / 3.6, "0.000000");
			writer.WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			writer.WriteLineC(UTF8STRC("</speed>"));

			writer.WriteLineC(UTF8STRC("</trkpt>"));

			k++;
		}

		writer.WriteLineC(UTF8STRC("</trkseg>"));
		i++;
	}

	writer.WriteLineC(UTF8STRC("</trk>"));
	writer.WriteLineC(UTF8STRC("</gpx>"));
	return true;
}
