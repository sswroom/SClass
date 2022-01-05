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

IO::FileExporter::SupportType Exporter::GPXExporter::IsObjectSupported(IO::ParsedObject *pobj)
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

Bool Exporter::GPXExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcat(nameBuff, (const UTF8Char*)"GPX file");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.gpx");
		return true;
	}
	return false;
}

void Exporter::GPXExporter::SetCodePage(UInt32 codePage)
{
	this->codePage = codePage;
}

Bool Exporter::GPXExporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
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
	IO::BufferedOutputStream *cstm;
	IO::StreamWriter *writer;
	Map::GPSTrack::GPSRecord *recs;
	Data::DateTime dt;

	Text::Encoding enc(this->codePage);

	NEW_CLASS(cstm, IO::BufferedOutputStream(stm, 65536));
	NEW_CLASS(writer, IO::StreamWriter(cstm, &enc));

	writer->WriteStrC(UTF8STRC("<?xml version=\"1.0\" encoding=\""));
	sptr = Text::EncodingFactory::GetInternetName(sbuff, this->codePage);
	writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
	writer->WriteLine((const UTF8Char*)"\"?>");
	writer->WriteLine((const UTF8Char*)"<gpx version=\"v1.0\" creator=\"iTravel Tech Inc. - http://www.itravel-tech.com\">");
	writer->WriteLine((const UTF8Char*)"<trk>");
	writer->WriteLine((const UTF8Char*)"<name>Track</name>");
	i = 0;
	j = track->GetTrackCnt();
	while (i < j)
	{
		writer->WriteLine((const UTF8Char*)"<trkseg>");
		k = 0;
		recs = track->GetTrack(i, &l);
		while (k < l)
		{
			writer->WriteStrC(UTF8STRC("<trkpt lat=\""));
			sptr = Text::StrDouble(sbuff, recs[k].lat);
			writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			writer->WriteStrC(UTF8STRC("\" lon=\""));
			sptr = Text::StrDouble(sbuff, recs[k].lon);
			writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			writer->WriteLine((const UTF8Char*)"\">");
			
			writer->WriteStrC(UTF8STRC("<ele>"));
			sptr = Text::StrDouble(sbuff, recs[k].altitude);
			writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			writer->WriteLine((const UTF8Char*)"</ele>");

			writer->WriteStrC(UTF8STRC("<time>"));
			dt.SetTicks(recs[k].utcTimeTicks);
			sptr = dt.ToString(sbuff, "yyyy-MM-ddTHH:mm:ssZ");
			writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			writer->WriteLine((const UTF8Char*)"</time>");

			writer->WriteStrC(UTF8STRC("<desc>lat.="));
			sptr = Text::StrDoubleFmt(sbuff, recs[k].lat, "0.000000");
			writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			writer->WriteStrC(UTF8STRC(", lon.="));
			sptr = Text::StrDoubleFmt(sbuff, recs[k].lon, "0.000000");
			writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			writer->WriteStrC(UTF8STRC(", Alt.="));
			sptr = Text::StrDoubleFmt(sbuff, recs[k].altitude, "0.000000");
			writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			writer->WriteStrC(UTF8STRC("m, Speed="));
			sptr = Text::StrDoubleFmt(sbuff, recs[k].speed * 1.852, "0.000000");
			writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			writer->WriteLine((const UTF8Char*)"m/h.</desc>");

			writer->WriteStrC(UTF8STRC("<speed>"));
			sptr = Text::StrDoubleFmt(sbuff, recs[k].speed * 1.852 / 3.6, "0.000000");
			writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
			writer->WriteLine((const UTF8Char*)"</speed>");

			writer->WriteLine((const UTF8Char*)"</trkpt>");

			k++;
		}

		writer->WriteLine((const UTF8Char*)"</trkseg>");
		i++;
	}

	writer->WriteLine((const UTF8Char*)"</trk>");
	writer->WriteLine((const UTF8Char*)"</gpx>");

	DEL_CLASS(writer);
	DEL_CLASS(cstm);
	return true;
}
