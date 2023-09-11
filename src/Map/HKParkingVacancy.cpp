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
	UTF8Char *sptr;
	Sync::MutexUsage mutUsage(this->parkingMut);
	IO::MemoryStream mstm;
	if (!Net::HTTPClient::LoadContent(this->sockf, this->ssl, CSTR("https://ogciopsi.blob.core.windows.net/dataset/parking-vacancy/parking-vacancy-info.csv"), &mstm, 10485760))
	{
		return;
	}
	mstm.SeekFromBeginning(0);
	DB::CSVFile csv(mstm, 65001);
	DB::DBReader *r = csv.QueryTableData(CSTR_NULL, CSTR_NULL, 0, 0, 0, CSTR_NULL, 0);
	if (r)
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
			sptr = r->GetName(i, sbuff);
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
			ParkingInfo *parking;
			while (r->ReadNext())
			{
				parking = MemAlloc(ParkingInfo, 1);
				parking->parkId = r->GetNewStr(colInd[0]);
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
				parking->parkingLatitude = r->GetDbl(colInd[13]);
				parking->parkingLongitude = r->GetDbl(colInd[14]);
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
				parking = this->parkingMap.Put(parking->parkId, parking);
				if (parking)
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
	UTF8Char *sptr;
	if (this->parkingMap.GetCount() > 0)
	{
		Sync::MutexUsage mutUsage(this->parkingMut);
		IO::MemoryStream mstm;
		if (!Net::HTTPClient::LoadContent(this->sockf, this->ssl, CSTR("https://dashboard.data.gov.hk/api/parking-vacancy?format=csv"), &mstm, 10485760))
		{
			return;
		}
		mstm.SeekFromBeginning(0);
		DB::CSVFile csv(mstm, 65001);
		DB::DBReader *r = csv.QueryTableData(CSTR_NULL, CSTR_NULL, 0, 0, 0, CSTR_NULL, 0);
		if (r)
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
				sptr = r->GetName(i, sbuff);
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
				ParkingInfo *parking;
				while (r->ReadNext())
				{
					sptr = r->GetStr(colInd[0], sbuff, 512);
					if (sptr)
					{
						parking = this->parkingMap.GetC(CSTRP(sbuff, sptr));
						if (parking)
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

void Map::HKParkingVacancy::ParkingInfoFree(ParkingInfo *parking)
{
	SDEL_STRING(parking->parkId);
	SDEL_STRING(parking->parkingNameEn);
	SDEL_STRING(parking->parkingAddressEn);
	SDEL_STRING(parking->parkingDistictEn);
	SDEL_STRING(parking->parkingNameSc);
	SDEL_STRING(parking->parkingAddressSc);
	SDEL_STRING(parking->parkingDistictSc);
	SDEL_STRING(parking->parkingNameTc);
	SDEL_STRING(parking->parkingAddressTc);
	SDEL_STRING(parking->parkingDistictTc);
	SDEL_STRING(parking->parkingStarttime);
	SDEL_STRING(parking->parkingEndtime);
	SDEL_STRING(parking->parkingContactNo);
	MemFree(parking);
}

Map::HKParkingVacancy::HKParkingVacancy(NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl) : Map::MapDrawLayer(CSTR("HKParkingVacancy"), 16, CSTR("HKParkingVacancy"))
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->bounds = Math::RectAreaDbl(Math::Coord2DDbl(0, 0), Math::Coord2DDbl(0, 0));
	this->LoadParkingInfo();
	this->LoadVacancy();

	this->SetCoordinateSystem(Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84));
	Media::StaticImage *simg = Map::MapPOI::CreateParkingPOI();
	Media::ImageList *imgList;
	NEW_CLASS(imgList, Media::ImageList(CSTR("ParkingPOI")));
	imgList->AddImage(simg, 0);
	Media::SharedImage *shimg;
	NEW_CLASS(shimg, Media::SharedImage(imgList, false));
	this->SetIconStyle(shimg, 8, 8);
	DEL_CLASS(shimg);
}

Map::HKParkingVacancy::~HKParkingVacancy()
{
	UOSInt i = this->parkingMap.GetCount();
	while (i-- > 0)
	{
		ParkingInfoFree(this->parkingMap.GetItem(i));
	}
}

Map::DrawLayerType Map::HKParkingVacancy::GetLayerType() const
{
	return Map::DRAW_LAYER_POINT;
}

UOSInt Map::HKParkingVacancy::GetAllObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr)
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

UOSInt Map::HKParkingVacancy::GetObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	return GetObjectIdsMapXY(outArr, nameArr, rect.ToDouble() * mapRate, keepEmpty);
}

UOSInt Map::HKParkingVacancy::GetObjectIdsMapXY(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	ParkingInfo *parking;
	UOSInt i = 0;
	UOSInt j = this->parkingMap.GetCount();
	while (i < j)
	{
		parking = this->parkingMap.GetItem(i);
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

void Map::HKParkingVacancy::ReleaseNameArr(NameArray *nameArr)
{
}

UTF8Char *Map::HKParkingVacancy::GetString(UTF8Char *buff, UOSInt buffSize, NameArray *nameArr, Int64 id, UOSInt strIndex)
{
	Sync::MutexUsage mutUsage(this->parkingMut);
	ParkingInfo *parking = this->parkingMap.GetItem((UOSInt)id);
	if (parking == 0)
	{
		return 0;
	}
	switch (strIndex)
	{
	case 0:
		if (parking->parkId)
			return parking->parkId->ConcatToS(buff, buffSize);
		return 0;
	case 1:
		if (parking->parkingNameEn)
			return parking->parkingNameEn->ConcatToS(buff, buffSize);
		return 0;
	case 2:
		if (parking->parkingAddressEn)
			return parking->parkingAddressEn->ConcatToS(buff, buffSize);
		return 0;
	case 3:
		if (parking->parkingDistictEn)
			return parking->parkingDistictEn->ConcatToS(buff, buffSize);
		return 0;
	case 4:
		if (parking->parkingNameSc)
			return parking->parkingNameSc->ConcatToS(buff, buffSize);
		return 0;
	case 5:
		if (parking->parkingAddressSc)
			return parking->parkingAddressSc->ConcatToS(buff, buffSize);
		return 0;
	case 6:
		if (parking->parkingDistictSc)
			return parking->parkingDistictSc->ConcatToS(buff, buffSize);
		return 0;
	case 7:
		if (parking->parkingNameTc)
			return parking->parkingNameTc->ConcatToS(buff, buffSize);
		return 0;
	case 8:
		if (parking->parkingAddressTc)
			return parking->parkingAddressTc->ConcatToS(buff, buffSize);
		return 0;
	case 9:
		if (parking->parkingDistictTc)
			return parking->parkingDistictTc->ConcatToS(buff, buffSize);
		return 0;
	case 10:
		if (parking->parkingStarttime)
			return parking->parkingStarttime->ConcatToS(buff, buffSize);
		return 0;
	case 11:
		if (parking->parkingEndtime)
			return parking->parkingEndtime->ConcatToS(buff, buffSize);
		return 0;
	case 12:
		if (parking->parkingContactNo)
			return parking->parkingContactNo->ConcatToS(buff, buffSize);
		return 0;
	case 13:
		return Text::StrDouble(buff, parking->parkingLatitude);
	case 14:
		return Text::StrDouble(buff, parking->parkingLongitude);
	case 15:
		{
			Math::Geometry::Point pt(4326, parking->parkingLongitude, parking->parkingLatitude);
			Math::WKTWriter wkt;
			Text::StringBuilderUTF8 sb;
			wkt.ToText(sb, pt);
			return sb.ConcatToS(buff, buffSize);
		}
	case 16:
		return Text::StrOSInt(buff, parking->vacancy);
	case 17:
		return parking->lastupdate.ToStringNoZone(buff);
	}
	return 0;
}

UOSInt Map::HKParkingVacancy::GetColumnCnt() const
{
	return 18;
}

UTF8Char *Map::HKParkingVacancy::GetColumnName(UTF8Char *buff, UOSInt colIndex)
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

DB::DBUtil::ColType Map::HKParkingVacancy::GetColumnType(UOSInt colIndex, UOSInt *colSize)
{
	if (colIndex < 13)
	{
		*colSize = 256;
		return DB::DBUtil::CT_VarUTF32Char;
	}
	switch (colIndex)
	{
	case 13:
	case 14:
		*colSize = 0;
		return DB::DBUtil::CT_Double;
	case 15:
		*colSize = 0;
		return DB::DBUtil::CT_Vector;
	case 16:
		*colSize = 11;
		return DB::DBUtil::CT_Int32;
	case 17:
		*colSize = 0;
		return DB::DBUtil::CT_DateTime;
	}
	return DB::DBUtil::CT_Unknown;
}
Bool Map::HKParkingVacancy::GetColumnDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
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
	sptr = this->GetColumnName(sbuff, colIndex);
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

Map::GetObjectSess *Map::HKParkingVacancy::BeginGetObject()
{
	return (GetObjectSess*)-1;
}

void Map::HKParkingVacancy::EndGetObject(GetObjectSess *session)
{
}

Math::Geometry::Vector2D *Map::HKParkingVacancy::GetNewVectorById(GetObjectSess *session, Int64 id)
{
	Sync::MutexUsage mutUsage(this->parkingMut);
	ParkingInfo *parking = this->parkingMap.GetItem((UOSInt)id);
	if (parking)
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
