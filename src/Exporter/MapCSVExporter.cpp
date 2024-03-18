#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "Exporter/MapCSVExporter.h"
#include "IO/BufferedOutputStream.h"
#include "IO/StreamWriter.h"
#include "Map/GPSTrack.h"
#include "Math/Geometry/Polyline.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

Exporter::MapCSVExporter::MapCSVExporter()
{
	this->codePage = 65001;
}

Exporter::MapCSVExporter::~MapCSVExporter()
{
}

Int32 Exporter::MapCSVExporter::GetName()
{
	return *(Int32*)"MCSV";
}

IO::FileExporter::SupportType Exporter::MapCSVExporter::IsObjectSupported(NotNullPtr<IO::ParsedObject> pobj)
{
	if (pobj->GetParserType() != IO::ParserType::MapLayer)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	NotNullPtr<Map::MapDrawLayer> layer = NotNullPtr<Map::MapDrawLayer>::ConvertFrom(pobj);
	if (layer->GetLayerType() != Map::DRAW_LAYER_POLYLINE && layer->GetLayerType() != Map::DRAW_LAYER_POLYLINE3D)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	return IO::FileExporter::SupportType::NormalStream;
}

Bool Exporter::MapCSVExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("Map CSV File"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.csv"));
		return true;
	}
	return false;
}

void Exporter::MapCSVExporter::SetCodePage(UInt32 codePage)
{
	this->codePage = codePage;
}

Bool Exporter::MapCSVExporter::ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CStringNN fileName, NotNullPtr<IO::ParsedObject> pobj, Optional<ParamData> param)
{
	if (pobj->GetParserType() != IO::ParserType::MapLayer)
	{
		return false;
	}
	NotNullPtr<Map::MapDrawLayer> layer = NotNullPtr<Map::MapDrawLayer>::ConvertFrom(pobj);
	if (layer->GetLayerType() != Map::DRAW_LAYER_POLYLINE && layer->GetLayerType() != Map::DRAW_LAYER_POLYLINE3D)
	{
		return false;
	}

	UTF8Char sbuff[1024];
	UTF8Char *sptr;
	Text::Encoding enc(this->codePage);
	IO::BufferedOutputStream cstm(stm, 65536);
	IO::StreamWriter writer(cstm, &enc);
	
	if (layer->GetObjectClass() == Map::MapDrawLayer::OC_GPS_TRACK)
	{
		NotNullPtr<Map::GPSTrack> track = NotNullPtr<Map::GPSTrack>::ConvertFrom(layer);
		Map::GPSTrack::GPSRecord3 *rec;
		UOSInt recCnt;
		Double v;
		Int32 currInd = 1;
		UOSInt k;
		UOSInt i = 0;
		UOSInt j = track->GetTrackCnt();
		Data::DateTime d;

		writer.WriteLineC(UTF8STRC("INDEX, UTC DATE, UTC TIME, VALID, LATITUDE, N/S, LONGITUDE, E/W, HEIGHT, SPEED, HEADING, NSAT(USED/VIEW)"));

		while (i < j)
		{
			rec = track->GetTrack(i, recCnt);
			k = 0;
			while (k < recCnt)
			{
				sptr = Text::StrInt32(sbuff, currInd);
				sptr = Text::StrConcatC(sptr, UTF8STRC(", "));
				d.SetInstant(rec[k].recTime);
				sptr = d.ToString(sptr, "yyyy/MM/dd, HH:mm:ss.fff");
				if (rec[k].valid == 1)
				{
					sptr = Text::StrConcatC(sptr, UTF8STRC(", DGPS, "));
				}
				else if (rec[k].valid)
				{
					sptr = Text::StrConcatC(sptr, UTF8STRC(", SPS, "));
				}
				else
				{
					sptr = Text::StrConcatC(sptr, UTF8STRC(", Estimated (dead reckoning), "));
				}
				v = rec[k].pos.GetLat();
				if (v < 0)
				{
					sptr = Text::StrDouble(sptr, -v);
					sptr = Text::StrConcatC(sptr, UTF8STRC(", S, "));
				}
				else
				{
					sptr = Text::StrDouble(sptr, v);
					sptr = Text::StrConcatC(sptr, UTF8STRC(", N, "));
				}
				v = rec[k].pos.GetLon();
				if (v < 0)
				{
					sptr = Text::StrDouble(sptr, -v);
					sptr = Text::StrConcatC(sptr, UTF8STRC(", W, "));
				}
				else
				{
					sptr = Text::StrDouble(sptr, v);
					sptr = Text::StrConcatC(sptr, UTF8STRC(", E, "));
				}
				sptr = Text::StrDoubleFmt(sptr, rec[k].altitude, "0.000");
				sptr = Text::StrConcatC(sptr, UTF8STRC(" M, "));
				sptr = Text::StrDoubleFmt(sptr, rec[k].speed * 1.852, "0.000");
				sptr = Text::StrConcatC(sptr, UTF8STRC(" km/h, "));
				sptr = Text::StrDoubleFmt(sptr, rec[k].heading, "0.000000");
				sptr = Text::StrConcatC(sptr, UTF8STRC(", "));
				sptr = Text::StrInt32(sptr, rec[k].nSateUsedGPS);
				sptr = Text::StrConcatC(sptr, UTF8STRC("/"));
				sptr = Text::StrInt32(sptr, rec[k].nSateViewGPS);
				writer.WriteLineC(sbuff, (UOSInt)(sptr - sbuff));

				currInd++;
				k++;
			}
			i++;
		}
	}
	else
	{
		Math::RectAreaDbl minMax;
		Double v;
		Math::Coord2DDbl *points;
		Int32 currInd = 1;
		Map::NameArray *nameArr;
		UOSInt i;
		UOSInt j;
		UOSInt k;
		UOSInt l;

		writer.WriteLineC(UTF8STRC("INDEX, LATITUDE, N/S, LONGITUDE, E/W"));

		Data::ArrayListInt64 objIds;
		Map::GetObjectSess *sess = layer->BeginGetObject();
		layer->GetBounds(minMax);
		layer->GetObjectIdsMapXY(objIds, &nameArr, minMax, true);
		i = 0;
		j = objIds.GetCount();
		while (i < j)
		{
			Math::Geometry::LineString *pl = (Math::Geometry::LineString*)layer->GetNewVectorById(sess, objIds.GetItem(i));
			points = pl->GetPointList(l);
			k = 0;
			while (k < l)
			{
				sptr = Text::StrInt32(sbuff, currInd);
				sptr = Text::StrConcatC(sptr, UTF8STRC(","));
				v = points[k].y;
				if (v < 0)
				{
					sptr = Text::StrDouble(sptr, -v);
					sptr = Text::StrConcatC(sptr, UTF8STRC(",S,"));
				}
				else
				{
					sptr = Text::StrDouble(sptr, v);
					sptr = Text::StrConcatC(sptr, UTF8STRC(",N,"));
				}
				v = points[k].x;
				if (v < 0)
				{
					sptr = Text::StrDouble(sptr, -v);
					sptr = Text::StrConcatC(sptr, UTF8STRC(",W"));
				}
				else
				{
					sptr = Text::StrDouble(sptr, v);
					sptr = Text::StrConcatC(sptr, UTF8STRC(",E"));
				}
				writer.WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
				currInd++;
				k++;
			}

			DEL_CLASS(pl);
			i++;
		}
		layer->EndGetObject(sess);
	}
	return true;
}
