#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "Map/HKRoadNetwork2.h"
#include "Map/HKTrafficLayer2.h"
#include "Map/VectorLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Geometry/Point.h"
#include "Math/Geometry/Polyline.h"
#include "Net/HTTPClient.h"
#include "Net/HTTPOSClient.h"
#include "Sync/MutexUsage.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/URLString.h"
#include "Text/XMLReader.h"
//#include <stdio.h>

#define SPEEDHIGH 40.0
#define SPEEDLOW 20.0

void Map::HKTrafficLayer2::SetSpeedMap(Int32 segmentId, Double speed, Bool valid)
{
	RoadInfo *road;
	Math::Geometry::Vector2D *vec;
	Sync::MutexUsage mutUsage(this->roadMut);
	road = this->roadMap.Get(segmentId);
	if (road == 0)
	{
		vec = this->vecMap.Get(segmentId);
		if (vec)
		{
			road = MemAllocA(RoadInfo, 1);
			road->segmentId = segmentId;
			road->vec = vec;
			road->speed = speed;
			road->valid = valid;
			road->bounds = vec->GetBounds();
			this->roadMap.Put(segmentId, road);
		}
	}
	else
	{
		road->speed = speed;
		road->valid = valid;
	}
}

IO::Stream *Map::HKTrafficLayer2::OpenURLStream()
{
	if (this->url->StartsWithICase(UTF8STRC("FILE:///")))
	{
		IO::FileStream *fs;
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		sptr = Text::URLString::GetURLFilePath(sbuff, this->url->v, this->url->leng);
		NEW_CLASS(fs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyAll, IO::FileStream::BufferType::Normal));
		if (!fs->IsError())
		{
			return fs;
		}
		DEL_CLASS(fs);
		return 0;
	}
	else
	{
		Int32 status;
		NN<Net::HTTPClient> cli;
		cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, this->url->ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, true);
		while (true)
		{
			status = cli->GetRespStatus();
			if (status == 301 || status == 302)
			{
				Text::StringBuilderUTF8 sb;
				cli->GetRespHeader(CSTR("Location"), sb);
				cli.Delete();
				if (!this->url->Equals(sb.ToString(), sb.GetLength()))
				{
					this->url->Release();
					this->url = Text::String::New(sb.ToCString());
					return this->OpenURLStream();
				}
				else
				{
					return 0;
				}
			}
			else
			{
				break;
			}
		}

		if (status == 200)
		{
			return cli.Ptr();
		}
		cli.Delete();
		return 0;
	}
}

Map::HKTrafficLayer2::HKTrafficLayer2(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Optional<Text::EncodingFactory> encFact, HKRoadNetwork2 *rn2) : Map::MapDrawLayer(CSTR("HKTraffic2"), 0, CSTR("HKTraffic2"), Math::CoordinateSystemManager::CreateProjCoordinateSystemDefNameOrDef(Math::CoordinateSystemManager::PCST_HK80))
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->encFact = encFact;
	this->bounds = Math::RectAreaDbl(0, 0, 0, 0);
	this->url = Text::String::New(UTF8STRC("https://resource.data.one.gov.hk/td/traffic-detectors/irnAvgSpeed-all.xml"));

	UTF8Char sbuff[256];
	UTF8Char *sptr;
	NN<Math::Geometry::Vector2D> vec;
	DB::ReadingDB *db = rn2->GetDB();
	if(db)
	{
		NN<DB::DBReader> r;
		if (db->QueryTableData(CSTR_NULL, CSTR("CENTERLINE"), 0, 0, 0, CSTR_NULL, 0).SetTo(r))
		{
			UOSInt shapeCol = INVALID_INDEX;
			UOSInt idCol = INVALID_INDEX;
			UOSInt i = r->ColCount();
			while (i-- > 0)
			{
				sptr = r->GetName(i, sbuff);
				if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("ROUTE_ID")))
				{
					idCol = i;
				}
				else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("SHAPE")))
				{
					shapeCol = i;
				}
			}
			if (shapeCol != INVALID_INDEX && idCol != INVALID_INDEX)
			{
				while (r->ReadNext())
				{
					if (r->GetVector(shapeCol).SetTo(vec))
					{
						if (vec.Set(this->vecMap.Put(r->GetInt32(idCol), vec.Ptr())))
						{
							vec.Delete();
						}
					}
				}
			}
			db->CloseReader(r);
		}
	}
//	printf("VecMap Count = %d\r\n", (Int32)this->vecMap.GetCount());
	this->ReloadData();
}

Map::HKTrafficLayer2::~HKTrafficLayer2()
{
	UOSInt i;
	RoadInfo *road;
	Math::Geometry::Vector2D *vec;
	i = this->roadMap.GetCount();
	while (i-- > 0)
	{
		road = this->roadMap.GetItem(i);
		MemFreeA(road);
	}

	i = this->vecMap.GetCount();
	while (i-- > 0)
	{
		vec = this->vecMap.GetItem(i);
		DEL_CLASS(vec);
	}
	this->url->Release();
}

void Map::HKTrafficLayer2::ReloadData()
{
	if (this->vecMap.GetCount() == 0)
	{
		return;
	}
	UInt8 buff[2048];
	UOSInt readSize;
	IO::Stream *stm;
//	printf("Reloading traffic data...");
	stm = this->OpenURLStream();
	if (stm)
	{
		IO::MemoryStream mstm;
		while (true)
		{
			readSize = stm->Read(BYTEARR(buff));
			if (readSize <= 0)
				break;
			mstm.Write(buff, readSize);
		}
		mstm.SeekFromBeginning(0);
		NN<Text::String> nodeName;
		Text::StringBuilderUTF8 sb;
		Text::XMLReader reader(this->encFact, mstm, Text::XMLReader::PM_XML);
		while (reader.NextElementName().SetTo(nodeName))
		{
			if (nodeName->Equals(UTF8STRC("segment_speed_list")))
			{
				while (reader.NextElementName().SetTo(nodeName))
				{
					if (nodeName->Equals(UTF8STRC("segments")))
					{
						while (reader.NextElementName().SetTo(nodeName))
						{
							if (nodeName->Equals(UTF8STRC("segment")))
							{
								Int32 segmentId = 0;
								Double speed = -1;
								Bool valid = false;

								while (reader.NextElementName().SetTo(nodeName))
								{
									if (nodeName->Equals(UTF8STRC("segment_id")))
									{
										sb.ClearStr();
										reader.ReadNodeText(sb);
										sb.ToInt32(segmentId);
									}
									else if (nodeName->Equals(UTF8STRC("speed")))
									{
										sb.ClearStr();
										reader.ReadNodeText(sb);
										sb.ToDouble(speed);
									}
									else if (nodeName->Equals(UTF8STRC("valid")))
									{
										sb.ClearStr();
										reader.ReadNodeText(sb);
										valid = sb.Equals(UTF8STRC("Y"));
									}
									else
									{
										reader.SkipElement();
									}
								}
								if (segmentId != 0 && speed >= 0)
								{
									SetSpeedMap(segmentId, speed, valid);
								}
							}
							else
							{
								reader.SkipElement();
							}
						}
					}
					else
					{
						reader.SkipElement();
					}
				}
			}
			else
			{
				reader.SkipElement();
			}
		}
		DEL_CLASS(stm);
//		printf("Loaded %d routes\r\n", (UInt32)this->roadMap.GetCount());
	}
	else
	{
//		printf("failed\r\n");
//		printf("URL=%s\r\n", this->url);
	}
}

Map::DrawLayerType Map::HKTrafficLayer2::GetLayerType() const
{
	return Map::DRAW_LAYER_POLYLINE;
}

UOSInt Map::HKTrafficLayer2::GetAllObjectIds(NN<Data::ArrayListInt64> outArr, NameArray **nameArr)
{
	UOSInt ret = 0;
	UOSInt i;
	UOSInt j;
	RoadInfo *road;
	Sync::MutexUsage mutUsage(this->roadMut);
	i = 0;
	j = this->roadMap.GetCount();
	while (i < j)
	{
		road = this->roadMap.GetItem(i);
		if (road->vec)
		{
			outArr->Add(road->segmentId);
			ret++;
		}
		i++;
	}
	return ret;
}

UOSInt Map::HKTrafficLayer2::GetObjectIds(NN<Data::ArrayListInt64> outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	return GetObjectIdsMapXY(outArr, nameArr, rect.ToDouble() / mapRate, keepEmpty);
}

UOSInt Map::HKTrafficLayer2::GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	UOSInt retCnt = 0;
	RoadInfo *road;
	UOSInt i;
	UOSInt j;
	rect = rect.Reorder();
	Sync::MutexUsage mutUsage(this->roadMut);
	i = 0;
	j = this->roadMap.GetCount();
	while (i < j)
	{
		road = this->roadMap.GetItem(i);
		if (road->vec && rect.OverlapOrTouch(road->bounds))
		{
			outArr->Add(road->segmentId);
			retCnt++;
		}
		i++;
	}
	return retCnt;
}

Int64 Map::HKTrafficLayer2::GetObjectIdMax() const
{
	Sync::MutexUsage mutUsage(this->roadMut);
	return this->roadMap.GetKey(this->roadMap.GetCount() - 1);
}

void Map::HKTrafficLayer2::ReleaseNameArr(NameArray *nameArr)
{
}

Bool Map::HKTrafficLayer2::GetString(NN<Text::StringBuilderUTF8> sb, NameArray *nameArr, Int64 id, UOSInt strIndex)
{
	return false;
}

UOSInt Map::HKTrafficLayer2::GetColumnCnt() const
{
	////////////////////////////
	return 0;
}

UTF8Char *Map::HKTrafficLayer2::GetColumnName(UTF8Char *buff, UOSInt colIndex)
{
	////////////////////////////
	return 0;
}

DB::DBUtil::ColType Map::HKTrafficLayer2::GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize)
{
	////////////////////////////
	return DB::DBUtil::CT_Unknown;
}

Bool Map::HKTrafficLayer2::GetColumnDef(UOSInt colIndex, NN<DB::ColDef> colDef)
{
	////////////////////////////
	return false;
}

UInt32 Map::HKTrafficLayer2::GetCodePage() const
{
	return 65001;
}

Bool Map::HKTrafficLayer2::GetBounds(OutParam<Math::RectAreaDbl> bounds) const
{
	bounds.Set(this->bounds);
	return true;
}

Map::GetObjectSess *Map::HKTrafficLayer2::BeginGetObject()
{
	return (GetObjectSess*)-1;
}

void Map::HKTrafficLayer2::EndGetObject(GetObjectSess *session)
{
}

Math::Geometry::Vector2D *Map::HKTrafficLayer2::GetNewVectorById(GetObjectSess *session, Int64 id)
{
	RoadInfo *road;
	Math::Geometry::Vector2D *vec = 0;
	Sync::MutexUsage mutUsage(this->roadMut);
	road = this->roadMap.Get(id);
	if (road && road->vec)
	{
		vec = road->vec->Clone().Ptr();
		if (road->speed >= SPEEDHIGH)
		{
			((Math::Geometry::Polyline*)vec)->SetColor(0xff00ff00);
		}
		else if (road->speed >= SPEEDLOW)
		{
			((Math::Geometry::Polyline*)vec)->SetColor(0xffffff00);
		}
		else
		{
			((Math::Geometry::Polyline*)vec)->SetColor(0xffff0000);
		}
	}
	return vec;
}

Map::MapDrawLayer::ObjectClass Map::HKTrafficLayer2::GetObjectClass() const
{
	return Map::MapDrawLayer::OC_HKTRAFFIC_LAYER2;
}
