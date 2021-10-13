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
	if (pobj->GetParserType() != IO::ParsedObject::PT_MAP_LAYER_PARSER)
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
	if (pobj->GetParserType() != IO::ParsedObject::PT_MAP_LAYER_PARSER)
	{
		return false;
	}
	Map::IMapDrawLayer *layer = (Map::IMapDrawLayer *)pobj;
	if (layer->GetObjectClass() != Map::IMapDrawLayer::OC_GPS_TRACK)
		return false;
	Map::GPSTrack *track = (Map::GPSTrack*)layer;
	UTF8Char sbuff[256];

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

	writer->Write((const UTF8Char*)"<?xml version=\"1.0\" encoding=\"");
	Text::EncodingFactory::GetInternetName(sbuff, this->codePage);
	writer->Write(sbuff);
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
			writer->Write((const UTF8Char*)"<trkpt lat=\"");
			Text::StrDouble(sbuff, recs[k].lat);
			writer->Write(sbuff);
			writer->Write((const UTF8Char*)"\" lon=\"");
			Text::StrDouble(sbuff, recs[k].lon);
			writer->Write(sbuff);
			writer->WriteLine((const UTF8Char*)"\">");
			
			writer->Write((const UTF8Char*)"<ele>");
			Text::StrDouble(sbuff, recs[k].altitude);
			writer->Write(sbuff);
			writer->WriteLine((const UTF8Char*)"</ele>");

			writer->Write((const UTF8Char*)"<time>");
			dt.SetTicks(recs[k].utcTimeTicks);
			dt.ToString(sbuff, "yyyy-MM-ddTHH:mm:ssZ");
			writer->Write(sbuff);
			writer->WriteLine((const UTF8Char*)"</time>");

			writer->Write((const UTF8Char*)"<desc>lat.=");
			Text::StrDoubleFmt(sbuff, recs[k].lat, "0.000000");
			writer->Write(sbuff);
			writer->Write((const UTF8Char*)", lon.=");
			Text::StrDoubleFmt(sbuff, recs[k].lon, "0.000000");
			writer->Write(sbuff);
			writer->Write((const UTF8Char*)", Alt.=");
			Text::StrDoubleFmt(sbuff, recs[k].altitude, "0.000000");
			writer->Write(sbuff);
			writer->Write((const UTF8Char*)"m, Speed=");
			Text::StrDoubleFmt(sbuff, recs[k].speed * 1.852, "0.000000");
			writer->Write(sbuff);
			writer->WriteLine((const UTF8Char*)"m/h.</desc>");

			writer->Write((const UTF8Char*)"<speed>");
			Text::StrDoubleFmt(sbuff, recs[k].speed * 1.852 / 3.6, "0.000000");
			writer->Write(sbuff);
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
