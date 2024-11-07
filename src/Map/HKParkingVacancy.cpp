#include "Stdafx.h"
#include "DB/ColDef.h"
#include "DB/CSVFile.h"
#include "IO/MemoryStream.h"
#include "Map/HKParkingVacancy.h"
#include "Map/MapPOI.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/WKTWriter.h"
#include "Math/Geometry/Point.h"
#include "Net/HTTPClient.h"
#include "Sync/MutexUsage.h"

#include <stdio.h>

void Map::HKParkingVacancy::LoadParkingInfo()
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	Sync::MutexUsage mutUsage(this->parkingMut);
	IO::MemoryStream mstm;
	if (!Net::HTTPClient::LoadContent(this->clif, this->ssl, CSTR("https://ogciopsi.blob.core.windows.net/dataset/parking-vacancy/parking-vacancy-info.csv"), mstm, 10485760))
	{
		return;
	}
	mstm.SeekFromBeginning(0);
	DB::CSVFile csv(mstm, 65001);
	NN<DB::DBReader> r;
	if (csv.QueryTableData(CSTR_NULL, CSTR(""), 0, 0, 0, CSTR_NULL, 0).SetTo(r))
	{
		UOSInt colInd[15];
		UOSInt i = 15;
		while (i-- > 0)
		{
			colInd[i] = INVALID_INDEX;
		}
		i = r->ColCount();
		while (i-- > 0)
		{
			if (r->GetName(i, sbuff).SetTo(sptr))
			{
				if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("park_id")))
				{
					colInd[0] = i;
				}
				else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("parking_name_en")))
				{
					colInd[1] = i;
				}
				else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("parking_address_en")))
				{
					colInd[2] = i;
				}
				else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("parking_district_en")))
				{
					colInd[3] = i;
				}
				else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("parking_name_sc")))
				{
					colInd[4] = i;
				}
				else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("parking_address_sc")))
				{
					colInd[5] = i;
				}
				else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("parking_district_sc")))
				{
					colInd[6] = i;
				}
				else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("parking_name_tc")))
				{
					colInd[7] = i;
				}
				else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("parking_address_tc")))
				{
					colInd[8] = i;
				}
				else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("parking_district_tc")))
				{
					colInd[9] = i;
				}
				else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("parking_starttime")))
				{
					colInd[10] = i;
				}
				else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("parking_endtime")))
				{
					colInd[11] = i;
				}
				else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("parking_contactNo")))
				{
					colInd[12] = i;
				}
				else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("parking_latitude")))
				{
					colInd[13] = i;
				}
				else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("parking_longitude")))
				{
					colInd[14] = i;
				}
			}
		}
		Bool found = false;
		i = 15;
		while (i-- > 0)
		{
			if (colInd[i] == INVALID_INDEX)
			{
				found = true;
				printf("HKParkingVacancy: ParkingInfo column %d not found\r\n", (UInt32)i);
			}
		}
		if (!found)
		{
			Math::RectAreaDbl bounds = this->bounds;
			NN<ParkingInfo> parking;
			while (r->ReadNext())
			{
				parking = MemAllocNN(ParkingInfo);
				parking->parkId = r->GetNewStrNN(colInd[0]);
				parking->parkingNameEn = r->GetNewStr(colInd[1]);
				parking->parkingAddressEn = r->GetNewStr(colInd[2]);
				parking->parkingDistictEn = r->GetNewStr(colInd[3]);
				parking->parkingNameSc = r->GetNewStr(colInd[4]);
				parking->parkingAddressSc = r->GetNewStr(colInd[5]);
				parking->parkingDistictSc = r->GetNewStr(colInd[6]);
				parking->parkingNameTc = r->GetNewStr(colInd[7]);
				parking->parkingAddressTc = r->GetNewStr(colInd[8]);
				parking->parkingDistictTc = r->GetNewStr(colInd[9]);
				parking->parkingStarttime = r->GetNewStr(colInd[10]);
				parking->parkingEndtime = r->GetNewStr(colInd[11]);
				parking->parkingContactNo = r->GetNewStr(colInd[12]);
				parking->parkingLatitude = r->GetDblOrNAN(colInd[13]);
				parking->parkingLongitude = r->GetDblOrNAN(colInd[14]);
				parking->vacancy = -1;
				parking->lastupdate = 0;
				Math::Coord2DDbl coord = Math::Coord2DDbl(parking->parkingLongitude, parking->parkingLatitude);
				if (this->parkingMap.GetCount() == 0)
				{
					bounds = Math::RectAreaDbl(coord, coord);
				}
				else
				{
					bounds = bounds.MergePoint(coord);
				}
				if (this->parkingMap.PutNN(parking->parkId, parking).SetTo(parking))
				{
					ParkingInfoFree(parking);
				}
			}
			this->bounds = bounds;
		}
		csv.CloseReader(r);
	}
}

void Map::HKParkingVacancy::LoadVacancy()
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	if (this->parkingMap.GetCount() > 0)
	{
		Sync::MutexUsage mutUsage(this->parkingMut);
		IO::MemoryStream mstm;
		if (!Net::HTTPClient::LoadContent(this->clif, this->ssl, CSTR("https://dashboard.data.gov.hk/api/parking-vacancy?format=csv"), mstm, 10485760))
		{
			return;
		}
		mstm.SeekFromBeginning(0);
		DB::CSVFile csv(mstm, 65001);
		NN<DB::DBReader> r;
		if (csv.QueryTableData(CSTR_NULL, CSTR(""), 0, 0, 0, CSTR_NULL, 0).SetTo(r))
		{
			UOSInt colInd[3];
			UOSInt i = 3;
			while (i-- > 0)
			{
				colInd[i] = INVALID_INDEX;
			}
			i = r->ColCount();
			while (i-- > 0)
			{
				if (r->GetName(i, sbuff).SetTo(sptr))
				{
					if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("park_id")))
					{
						colInd[0] = i;
					}
					else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("vacancy")))
					{
						colInd[1] = i;
					}
					else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("lastupdate")))
					{
						colInd[2] = i;
					}
				}
			}
			Bool found = false;
			i = 3;
			while (i-- > 0)
			{
				if (colInd[i] == INVALID_INDEX)
				{
					found = true;
					printf("HKParkingVacancy: Vacancy column %d not found\r\n", (UInt32)i);
				}
			}
			if (!found)
			{
				NN<ParkingInfo> parking;
				while (r->ReadNext())
				{
					if (r->GetStr(colInd[0], sbuff, 512).SetTo(sptr))
					{
						if (this->parkingMap.GetC(CSTRP(sbuff, sptr)).SetTo(parking))
						{
							parking->vacancy = r->GetInt32(colInd[1]);
							parking->lastupdate = r->GetTimestamp(colInd[2]);
						}
					}
				}
			}
			csv.CloseReader(r);
		}
	}
}

void Map::HKParkingVacancy::ParkingInfoFree(NN<ParkingInfo> parking)
{
	parking->parkId->Release();
	OPTSTR_DEL(parking->parkingNameEn);
	OPTSTR_DEL(parking->parkingAddressEn);
	OPTSTR_DEL(parking->parkingDistictEn);
	OPTSTR_DEL(parking->parkingNameSc);
	OPTSTR_DEL(parking->parkingAddressSc);
	OPTSTR_DEL(parking->parkingDistictSc);
	OPTSTR_DEL(parking->parkingNameTc);
	OPTSTR_DEL(parking->parkingAddressTc);
	OPTSTR_DEL(parking->parkingDistictTc);
	OPTSTR_DEL(parking->parkingStarttime);
	OPTSTR_DEL(parking->parkingEndtime);
	OPTSTR_DEL(parking->parkingContactNo);
	MemFreeNN(parking);
}

Map::HKParkingVacancy::HKParkingVacancy(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl) : Map::MapDrawLayer(CSTR("HKParkingVacancy"), 16, CSTR("HKParkingVacancy"), Math::CoordinateSystemManager::CreateWGS84Csys())
{
	this->clif = clif;
	this->ssl = ssl;
	this->bounds = Math::RectAreaDbl(Math::Coord2DDbl(0, 0), Math::Coord2DDbl(0, 0));
	this->LoadParkingInfo();
	this->LoadVacancy();
	NN<Media::StaticImage> simg = Map::MapPOI::CreateParkingPOI();
	NN<Media::ImageList> imgList;
	NEW_CLASSNN(imgList, Media::ImageList(CSTR("ParkingPOI")));
	imgList->AddImage(simg, 0);
	NN<Media::SharedImage> shimg;
	NEW_CLASSNN(shimg, Media::SharedImage(imgList, false));
	this->SetIconStyle(shimg, 8, 8);
	shimg.Delete();
}

Map::HKParkingVacancy::~HKParkingVacancy()
{
	this->parkingMap.FreeAll(ParkingInfoFree);
}

Map::DrawLayerType Map::HKParkingVacancy::GetLayerType() const
{
	return Map::DRAW_LAYER_POINT;
}

UOSInt Map::HKParkingVacancy::GetAllObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr)
{
	UOSInt i = 0;
	UOSInt j = this->parkingMap.GetCount();
	while (i < j)
	{
		outArr->Add((Int64)i);
		i++;
	}
	return j;
}

UOSInt Map::HKParkingVacancy::GetObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	return GetObjectIdsMapXY(outArr, nameArr, rect.ToDouble() * mapRate, keepEmpty);
}

UOSInt Map::HKParkingVacancy::GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	NN<ParkingInfo> parking;
	UOSInt i = 0;
	UOSInt j = this->parkingMap.GetCount();
	while (i < j)
	{
		parking = this->parkingMap.GetItemNoCheck(i);
		if (rect.ContainPt(parking->parkingLongitude, parking->parkingLatitude))
		{
			outArr->Add((Int64)i);
		}
		i++;
	}
	return j;
}

Int64 Map::HKParkingVacancy::GetObjectIdMax() const
{
	return (Int64)this->parkingMap.GetCount() - 1;
}

void Map::HKParkingVacancy::ReleaseNameArr(Optional<NameArray> nameArr)
{
}

Bool Map::HKParkingVacancy::GetString(NN<Text::StringBuilderUTF8> sb, Optional<NameArray> nameArr, Int64 id, UOSInt strIndex)
{
	Sync::MutexUsage mutUsage(this->parkingMut);
	NN<ParkingInfo> parking;
	if (!this->parkingMap.GetItem((UOSInt)id).SetTo(parking))
	{
		return false;
	}
	NN<Text::String> s;
	switch (strIndex)
	{
	case 0:
		sb->Append(parking->parkId);
		return true;
	case 1:
		if (!parking->parkingNameEn.SetTo(s))
			return false;
		sb->Append(s);
		return true;
	case 2:
		if (!parking->parkingAddressEn.SetTo(s))
			return false;
		sb->Append(s);
		return true;
	case 3:
		if (!parking->parkingDistictEn.SetTo(s))
			return false;
		sb->Append(s);
		return true;
	case 4:
		if (!parking->parkingNameSc.SetTo(s))
			return false;
		sb->Append(s);
		return true;
	case 5:
		if (!parking->parkingAddressSc.SetTo(s))
			return false;
		sb->Append(s);
		return true;
	case 6:
		if (!parking->parkingDistictSc.SetTo(s))
			return false;
		sb->Append(s);
		return true;
	case 7:
		if (!parking->parkingNameTc.SetTo(s))
			return false;
		sb->Append(s);
		return true;
	case 8:
		if (!parking->parkingAddressTc.SetTo(s))
			return false;
		sb->Append(s);
		return true;
	case 9:
		if (!parking->parkingDistictTc.SetTo(s))
			return false;
		sb->Append(s);
		return true;
	case 10:
		if (!parking->parkingStarttime.SetTo(s))
			return false;
		sb->Append(s);
		return true;
	case 11:
		if (!parking->parkingEndtime.SetTo(s))
			return false;
		sb->Append(s);
		return true;
	case 12:
		if (!parking->parkingContactNo.SetTo(s))
			return false;
		sb->Append(s);
		return true;
	case 13:
		sb->AppendDouble(parking->parkingLatitude);
		return true;
	case 14:
		sb->AppendDouble(parking->parkingLongitude);
		return true;
	case 15:
		{
			Math::Geometry::Point pt(4326, parking->parkingLongitude, parking->parkingLatitude);
			Math::WKTWriter wkt;
			return wkt.ToText(sb, pt);
		}
	case 16:
		sb->AppendOSInt(parking->vacancy);
		return true;
	case 17:
		sb->AppendTSNoZone(parking->lastupdate);
		return true;
	}
	return false;
}

UOSInt Map::HKParkingVacancy::GetColumnCnt() const
{
	return 18;
}

UnsafeArrayOpt<UTF8Char> Map::HKParkingVacancy::GetColumnName(UnsafeArray<UTF8Char> buff, UOSInt colIndex)
{
	switch (colIndex)
	{
	case 0:
		return Text::StrConcatC(buff, UTF8STRC("park_id"));
	case 1:
		return Text::StrConcatC(buff, UTF8STRC("parking_name_en"));
	case 2:
		return Text::StrConcatC(buff, UTF8STRC("parking_address_en"));
	case 3:
		return Text::StrConcatC(buff, UTF8STRC("parking_district_en"));
	case 4:
		return Text::StrConcatC(buff, UTF8STRC("parking_name_sc"));
	case 5:
		return Text::StrConcatC(buff, UTF8STRC("parking_address_sc"));
	case 6:
		return Text::StrConcatC(buff, UTF8STRC("parking_district_sc"));
	case 7:
		return Text::StrConcatC(buff, UTF8STRC("parking_name_tc"));
	case 8:
		return Text::StrConcatC(buff, UTF8STRC("parking_address_tc"));
	case 9:
		return Text::StrConcatC(buff, UTF8STRC("parking_district_tc"));
	case 10:
		return Text::StrConcatC(buff, UTF8STRC("parking_starttime"));
	case 11:
		return Text::StrConcatC(buff, UTF8STRC("parking_endtime"));
	case 12:
		return Text::StrConcatC(buff, UTF8STRC("parking_contactNo"));
	case 13:
		return Text::StrConcatC(buff, UTF8STRC("parking_latitude"));
	case 14:
		return Text::StrConcatC(buff, UTF8STRC("parking_longitude"));
	case 15:
		return Text::StrConcatC(buff, UTF8STRC("Shape"));
	case 16:
		return Text::StrConcatC(buff, UTF8STRC("vacancy"));
	case 17:
		return Text::StrConcatC(buff, UTF8STRC("lastupdate"));
	default:
		return 0;
	}
}

DB::DBUtil::ColType Map::HKParkingVacancy::GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize)
{
	if (colIndex < 13)
	{
		colSize.Set(256);
		return DB::DBUtil::CT_VarUTF32Char;
	}
	switch (colIndex)
	{
	case 13:
	case 14:
		colSize.Set(32);
		return DB::DBUtil::CT_Double;
	case 15:
		colSize.Set(0x7fffffff);
		return DB::DBUtil::CT_Vector;
	case 16:
		colSize.Set(11);
		return DB::DBUtil::CT_Int32;
	case 17:
		colSize.Set(33);
		return DB::DBUtil::CT_DateTime;
	}
	return DB::DBUtil::CT_Unknown;
}
Bool Map::HKParkingVacancy::GetColumnDef(UOSInt colIndex, NN<DB::ColDef> colDef)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	if (colIndex >= 18)
	{
		return false;
	}
	colDef->SetPK(colIndex == 0);
	colDef->SetNotNull(colIndex == 0 || colIndex >= 13);
	if (colIndex < 13)
	{
		colDef->SetNativeType(CSTR("String"));
		colDef->SetColType(DB::DBUtil::CT_VarUTF32Char);
		colDef->SetColSize(256);
	}
	else
	{
		switch (colIndex)
		{
		case 13:
		case 14:
			colDef->SetNativeType(CSTR("Double"));
			colDef->SetColType(DB::DBUtil::CT_Double);
			colDef->SetColSize(0);
			colDef->SetColDP(12);
			break;
		case 15:
			colDef->SetNativeType(CSTR("Vector2D"));
			colDef->SetColType(DB::DBUtil::CT_Vector);
			colDef->SetColSize(0);
			break;
		case 16:
			colDef->SetNativeType(CSTR("OSInt"));
			colDef->SetColType(DB::DBUtil::CT_Int32);
			colDef->SetColSize(11);
			break;
		case 17:
			colDef->SetNativeType(CSTR("DateTime"));
			colDef->SetColType(DB::DBUtil::CT_DateTime);
			colDef->SetColSize(0);
			break;
		}
	}
	sbuff[0] = 0;
	sptr = this->GetColumnName(sbuff, colIndex).Or(sbuff);
	colDef->SetColName(CSTRP(sbuff, sptr));
	return true;
}

UInt32 Map::HKParkingVacancy::GetCodePage() const
{
	return 65001;
}

Bool Map::HKParkingVacancy::GetBounds(OutParam<Math::RectAreaDbl> rect) const
{
	rect.Set(bounds);
	return true;
}

NN<Map::GetObjectSess> Map::HKParkingVacancy::BeginGetObject()
{
	return NN<GetObjectSess>::ConvertFrom(NN<HKParkingVacancy>(*this));
}

void Map::HKParkingVacancy::EndGetObject(NN<GetObjectSess> session)
{
}

Optional<Math::Geometry::Vector2D> Map::HKParkingVacancy::GetNewVectorById(NN<GetObjectSess> session, Int64 id)
{
	Sync::MutexUsage mutUsage(this->parkingMut);
	NN<ParkingInfo> parking;
	if (this->parkingMap.GetItem((UOSInt)id).SetTo(parking))
	{
		Math::Geometry::Point *pt;
		NEW_CLASS(pt, Math::Geometry::Point(4326, parking->parkingLongitude, parking->parkingLatitude));
		return pt;
	}
	return 0;
}

Map::MapDrawLayer::ObjectClass Map::HKParkingVacancy::GetObjectClass() const
{
	return ObjectClass::OC_OTHER;
}
