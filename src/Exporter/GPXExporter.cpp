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

Bool Exporter::GPXExporter::GetOutputName(UOSInt index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff)
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
	UnsafeArray<UTF8Char> sptr;

	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	UnsafeArray<Map::GPSTrack::GPSRecord3> recs;
	Data::DateTime dt;

	Text::Encoding enc(this->codePage);
	IO::BufferedOutputStream cstm(stm, 65536);
	IO::StreamWriter writer(cstm, &enc);

	writer.Write(CSTR("<?xml version=\"1.0\" encoding=\""));
	sptr = Text::EncodingFactory::GetInternetName(sbuff, this->codePage);
	writer.Write(CSTRP(sbuff, sptr));
	writer.WriteLine(CSTR("\"?>"));
	writer.WriteLine(CSTR("<gpx version=\"v1.0\" creator=\"iTravel Tech Inc. - http://www.itravel-tech.com\">"));
	writer.WriteLine(CSTR("<trk>"));
	writer.WriteLine(CSTR("<name>Track</name>"));
	i = 0;
	j = track->GetTrackCnt();
	while (i < j)
	{
		if (track->GetTrack(i, l).SetTo(recs))
		{
			writer.WriteLine(CSTR("<trkseg>"));
			k = 0;
			while (k < l)
			{
				writer.Write(CSTR("<trkpt lat=\""));
				sptr = Text::StrDouble(sbuff, recs[k].pos.GetLat());
				writer.Write(CSTRP(sbuff, sptr));
				writer.Write(CSTR("\" lon=\""));
				sptr = Text::StrDouble(sbuff, recs[k].pos.GetLon());
				writer.Write(CSTRP(sbuff, sptr));
				writer.WriteLine(CSTR("\">"));
				
				writer.Write(CSTR("<ele>"));
				sptr = Text::StrDouble(sbuff, recs[k].altitude);
				writer.Write(CSTRP(sbuff, sptr));
				writer.WriteLine(CSTR("</ele>"));

				writer.Write(CSTR("<time>"));
				dt.SetInstant(recs[k].recTime);
				sptr = dt.ToString(sbuff, "yyyy-MM-ddTHH:mm:ssZ");
				writer.Write(CSTRP(sbuff, sptr));
				writer.WriteLine(CSTR("</time>"));

				writer.Write(CSTR("<desc>lat.="));
				sptr = Text::StrDoubleFmt(sbuff, recs[k].pos.GetLat(), "0.000000");
				writer.Write(CSTRP(sbuff, sptr));
				writer.Write(CSTR(", lon.="));
				sptr = Text::StrDoubleFmt(sbuff, recs[k].pos.GetLon(), "0.000000");
				writer.Write(CSTRP(sbuff, sptr));
				writer.Write(CSTR(", Alt.="));
				sptr = Text::StrDoubleFmt(sbuff, recs[k].altitude, "0.000000");
				writer.Write(CSTRP(sbuff, sptr));
				writer.Write(CSTR("m, Speed="));
				sptr = Text::StrDoubleFmt(sbuff, recs[k].speed * 1.852, "0.000000");
				writer.Write(CSTRP(sbuff, sptr));
				writer.WriteLine(CSTR("m/h.</desc>"));

				writer.Write(CSTR("<speed>"));
				sptr = Text::StrDoubleFmt(sbuff, recs[k].speed * 1.852 / 3.6, "0.000000");
				writer.Write(CSTRP(sbuff, sptr));
				writer.WriteLine(CSTR("</speed>"));

				writer.WriteLine(CSTR("</trkpt>"));

				k++;
			}
			writer.WriteLine(CSTR("</trkseg>"));
		}
		i++;
	}

	writer.WriteLine(CSTR("</trk>"));
	writer.WriteLine(CSTR("</gpx>"));
	return true;
}
