#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "DB/ColDef.h"
#include "Map/GPSTrack.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math.h"
#include "Math/Point3D.h"
#include "Math/Polyline3D.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"

Map::GPSTrack::GPSTrack(const UTF8Char *sourceName, Bool hasAltitude, UInt32 codePage, const UTF8Char *layerName) : Map::IMapDrawLayer(sourceName, 0, layerName)
{
	this->codePage = codePage;
	this->currTrackName = 0;
	this->extraParser = 0;
	NEW_CLASS(this->recMut, Sync::Mutex());
	NEW_CLASS(this->currTimes, Data::ArrayListInt64());
	NEW_CLASS(this->currRecs, Data::ArrayList<Map::GPSTrack::GPSRecord*>());
	NEW_CLASS(this->currTracks, Data::ArrayList<TrackRecord*>());
	NEW_CLASS(this->currExtraData, Data::ArrayList<const UInt8*>());
	NEW_CLASS(this->currExtraSize, Data::ArrayList<UOSInt>());
	this->tmpRecord = 0;
	NEW_CLASS(this->updMut, Sync::Mutex());
	NEW_CLASS(this->updHdlrs, Data::ArrayList<UpdatedHandler>());
	NEW_CLASS(this->updObjs, Data::ArrayList<void *>());
	this->maxLat = this->minLat = this->maxLon = this->minLon = 0;
	this->hasAltitude = hasAltitude;
	this->csys = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::GeographicCoordinateSystem::GCST_WGS84);
}

Map::GPSTrack::~GPSTrack()
{
	UOSInt i;
	const UInt8 *data;
	DEL_CLASS(currTimes);
	i = currRecs->GetCount();
	while (i-- > 0)
	{
		MemFree(this->currRecs->GetItem(i));
		data = this->currExtraData->GetItem(i);
		if (data)
		{
			MemFree((void*)data);
		}
	}
	DEL_CLASS(this->currRecs);
	DEL_CLASS(this->currExtraData);
	DEL_CLASS(this->currExtraSize);
	SDEL_CLASS(this->extraParser);
	i = currTracks->GetCount();
	while (i-- > 0)
	{
		Map::GPSTrack::TrackRecord *rec;
		rec = (Map::GPSTrack::TrackRecord*)currTracks->GetItem(i);
		if (rec->name)
		{
			Text::StrDelNew(rec->name);
		}
		MemFree(rec->extraData);
		MemFree(rec->extraDataSize);
		MemFree(rec->records);
		MemFree(rec);
	}
	DEL_CLASS(currTracks);
	if (tmpRecord)
	{
		MemFree(tmpRecord);
		tmpRecord = 0;
	}
	if (this->currTrackName)
	{
		Text::StrDelNew(this->currTrackName);
		this->currTrackName = 0;
	}
	DEL_CLASS(this->recMut);
	DEL_CLASS(this->updMut);
	DEL_CLASS(this->updHdlrs);
	DEL_CLASS(this->updObjs);
}

Map::DrawLayerType Map::GPSTrack::GetLayerType()
{
	if (this->hasAltitude)
	{
		return Map::DRAW_LAYER_POLYLINE3D;
	}
	else
	{
		return Map::DRAW_LAYER_POLYLINE;
	}
}

UOSInt Map::GPSTrack::GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr)
{
	Sync::MutexUsage mutUsage(this->recMut);
	UOSInt i = 0;
	UOSInt j = this->currTracks->GetCount();
	while (i < j)
	{
		outArr->Add((Int64)i);
		i++;
	}
	if (this->currTimes->GetCount() > 0)
	{
		outArr->Add((Int64)j);
		mutUsage.EndUse();
		return j + 1;
	}
	else
	{
		mutUsage.EndUse();
		return j;
	}
}

UOSInt Map::GPSTrack::GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Int32 x1, Int32 y1, Int32 x2, Int32 y2, Bool keepEmpty)
{
	return GetObjectIdsMapXY(outArr, nameArr, x1 / mapRate, y1 / mapRate, x2 / mapRate, y2 / mapRate, keepEmpty);
}

UOSInt Map::GPSTrack::GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Double x1, Double y1, Double x2, Double y2, Bool keepEmpty)
{
	Sync::MutexUsage mutUsage(this->recMut);
	Double tmpVal;
	if (x1 > x2)
	{
		tmpVal = x1;
		x1 = x2;
		x2 = tmpVal;
	}
	if (y1 > y2)
	{
		tmpVal = y1;
		y1 = y2;
		y2 = tmpVal;
	}
	Int32 cnt = 0;
	UOSInt i = 0;
	UOSInt j = this->currTracks->GetCount();
	while (i < j)
	{
		Map::GPSTrack::TrackRecord *track;
		track = (Map::GPSTrack::TrackRecord*)this->currTracks->GetItem(i);
		if (track->minLon <= x2 && track->maxLon >= x1 && track->minLat <= y2 && track->maxLat >= y1)
		{
			outArr->Add((Int64)i);
			cnt++;
		}
		i++;
	}
	if (this->currTimes->GetCount() > 0)
	{
		if (this->currMinLon <= x2 && this->currMaxLon >= x1 && this->currMinLat <= y2 && this->currMaxLat >= y1)
		{
			outArr->Add((Int64)j);
			cnt++;
		}
	}
	mutUsage.EndUse();
	return j;
}

Int64 Map::GPSTrack::GetObjectIdMax()
{
	if (this->currTimes->GetCount() > 0)
	{
		return (Int64)this->currTracks->GetCount();
	}
	else
	{
		return (Int64)this->currTracks->GetCount() - 1;
	}
}

void Map::GPSTrack::ReleaseNameArr(void *nameArr)
{
}

UTF8Char *Map::GPSTrack::GetString(UTF8Char *buff, UOSInt buffSize, void *nameArr, Int64 id, UOSInt strIndex)
{
	if (strIndex >= 3)
		return 0;

	if ((UInt64)id > this->currTracks->GetCount())
		return 0;
	if (id < 0)
		return 0;
	Data::DateTime dt;
	UTF8Char *sptr;
	if ((UInt64)id == this->currTracks->GetCount())
	{
		if (strIndex == 0)
		{
			dt.SetTicks(this->currTimes->GetItem(0));
			sptr = dt.ToString(buff, "yyyy-MM-dd HH:mm:ss.fff");
			sptr = Text::StrConcat(sptr, (const UTF8Char*)" - ");
			dt.SetTicks(this->currTimes->GetItem(this->currTimes->GetCount() - 1));
			sptr = dt.ToString(sptr, "yyyy-MM-dd HH:mm:ss.fff");
			return sptr;
		}
		else if (strIndex == 1)
		{
			dt.SetTicks(this->currTimes->GetItem(0));
			return dt.ToString(buff, "yyyy-MM-dd HH:mm:ss.fff");
		}
		else if (strIndex == 2)
		{
			dt.SetTicks(this->currTimes->GetItem(this->currTimes->GetCount() - 1));
			return dt.ToString(buff, "yyyy-MM-dd HH:mm:ss.fff");
		}
		else
		{
			return 0;
		}
	}
	else
	{
		Map::GPSTrack::TrackRecord *track = (Map::GPSTrack::TrackRecord *)this->currTracks->GetItem((UOSInt)id);
		if (strIndex == 0)
		{
			dt.SetTicks(track->records[0].utcTimeTicks);
			sptr = dt.ToString(buff, "yyyy-MM-dd HH:mm:ss.fff");
			sptr = Text::StrConcat(sptr, (const UTF8Char*)" - ");
			dt.SetTicks(track->records[track->nRecords - 1].utcTimeTicks);
			sptr = dt.ToString(sptr, "yyyy-MM-dd HH:mm:ss.fff");
			return sptr;
		}
		else if (strIndex == 1)
		{
			dt.SetTicks(track->records[0].utcTimeTicks);
			return dt.ToString(buff, "yyyy-MM-dd HH:mm:ss.fff");
		}
		else if (strIndex == 2)
		{
			dt.SetTicks(track->records[track->nRecords - 1].utcTimeTicks);
			return dt.ToString(buff, "yyyy-MM-dd HH:mm:ss.fff");
		}
		else
		{
			return 0;
		}
	}
}

UOSInt Map::GPSTrack::GetColumnCnt()
{
	return 3;
}

UTF8Char *Map::GPSTrack::GetColumnName(UTF8Char *buff, UOSInt colIndex)
{
	if (colIndex == 0)
	{
		return Text::StrConcat(buff, (const UTF8Char*)"Name");
	}
	else if (colIndex == 1)
	{
		return Text::StrConcat(buff, (const UTF8Char*)"Time Range");
	}
	else if (colIndex == 1)
	{
		return Text::StrConcat(buff, (const UTF8Char*)"Start Time");
	}
	else if (colIndex == 2)
	{
		return Text::StrConcat(buff, (const UTF8Char*)"End Time");
	}
	else
	{
		return 0;
	}
}

DB::DBUtil::ColType Map::GPSTrack::GetColumnType(UOSInt colIndex, UOSInt *colSize)
{
	if (colIndex == 0)
	{
		if (colSize)
		{
			*colSize = 256;
		}
		return DB::DBUtil::CT_VarChar;
	}
	else if (colIndex == 1)
	{
		if (colSize)
		{
			*colSize = 41;
		}
		return DB::DBUtil::CT_VarChar;
	}
	else if (colIndex == 2 || colIndex == 3)
	{
		if (colSize)
		{
			*colSize = 19;
		}
		return DB::DBUtil::CT_DateTime;
	}
	else
	{
		if (colSize)
		{
			*colSize = 0;
		}
		return DB::DBUtil::CT_Unknown;
	}
}

Bool Map::GPSTrack::GetColumnDef(UOSInt colIndex, DB::ColDef *colDef)
{
	if (colIndex == 0)
	{
		colDef->SetColName((const UTF8Char*)"Name");
		colDef->SetColSize(256);
		colDef->SetColDP(0);
		colDef->SetColType(DB::DBUtil::CT_VarChar);
		colDef->SetDefVal(0);
		colDef->SetNotNull(false);
		colDef->SetPK(false);
		colDef->SetAutoInc(false);
		colDef->SetAttr(0);
		return true;
	}
	else if (colIndex == 1)
	{
		colDef->SetColName((const UTF8Char*)"Time Range");
		colDef->SetColSize(41);
		colDef->SetColDP(0);
		colDef->SetColType(DB::DBUtil::CT_VarChar);
		colDef->SetDefVal(0);
		colDef->SetNotNull(false);
		colDef->SetPK(false);
		colDef->SetAutoInc(false);
		colDef->SetAttr(0);
		return true;
	}
	else if (colIndex == 2)
	{
		colDef->SetColName((const UTF8Char*)"Start Time");
		colDef->SetColSize(19);
		colDef->SetColDP(0);
		colDef->SetColType(DB::DBUtil::CT_DateTime);
		colDef->SetDefVal(0);
		colDef->SetNotNull(false);
		colDef->SetPK(false);
		colDef->SetAutoInc(false);
		colDef->SetAttr(0);
		return true;
	}
	else if (colIndex == 3)
	{
		colDef->SetColName((const UTF8Char*)"End Time");
		colDef->SetColSize(19);
		colDef->SetColDP(0);
		colDef->SetColType(DB::DBUtil::CT_DateTime);
		colDef->SetDefVal(0);
		colDef->SetNotNull(false);
		colDef->SetPK(false);
		colDef->SetAutoInc(false);
		colDef->SetAttr(0);
		return true;
	}
	else
	{
		return false;
	}
}

UInt32 Map::GPSTrack::GetCodePage()
{
	return this->codePage;
}

Bool Map::GPSTrack::GetBoundsDbl(Double *minX, Double *minY, Double *maxX, Double *maxY)
{
	*minX = minLon;
	*minY = minLat;
	*maxX = maxLon;
	*maxY = maxLat;
	return this->minLon != 0 || this->minLat != 0 || this->maxLon != 0 || this->maxLat != 0;
}

void *Map::GPSTrack::BeginGetObject()
{
	return (void*)-1;
}

void Map::GPSTrack::EndGetObject(void *session)
{
}

Map::DrawObjectL *Map::GPSTrack::GetObjectByIdD(void *session, Int64 id)
{
	Map::DrawObjectL *outObj;
	UOSInt i;
	UOSInt j;
	Double lastLat;
	Double lastLon;
	Double *ptPtr;
	if (id < 0)
		return 0;
	Sync::MutexUsage mutUsage(this->recMut);
	if ((UInt64)id > this->currTracks->GetCount())
	{
		mutUsage.EndUse();
		return 0;
	}
	else if ((UInt64)id == this->currTracks->GetCount())
	{
		if (this->currTimes->GetCount() > 0)
		{
			Map::GPSTrack::GPSRecord *rec;
			lastLat = 0;
			lastLon = 0;
			outObj = MemAlloc(Map::DrawObjectL, 1);
			outObj->objId = id;
			outObj->nPtOfst = 1;
			outObj->nPoint = (UInt32)(j = this->currRecs->GetCount());
			outObj->ptOfstArr = MemAlloc(UInt32, 1);
			outObj->pointArr = MemAlloc(Double, outObj->nPoint * 2);
			outObj->ptOfstArr[0] = 0;
			ptPtr = outObj->pointArr;
			i = 0;
			while (i < j)
			{
				rec = (Map::GPSTrack::GPSRecord*)this->currRecs->GetItem(i);
				if (rec->lat == 0 && rec->lon == 0)
				{
					ptPtr[0] = lastLon;
					ptPtr[1] = lastLat;
				}
				else
				{
					lastLon = ptPtr[0] = rec->lon;
					lastLat = ptPtr[1] = rec->lat;
				}
				ptPtr += 2;
				i++;
			}
			outObj->flags = 0;
			outObj->lineColor = 0;
			mutUsage.EndUse();
			return outObj;
		}
		else
		{
			mutUsage.EndUse();
			return 0;
		}
	}
	else
	{
		Map::GPSTrack::TrackRecord *track;
		lastLat = 0;
		lastLon = 0;
		track = (Map::GPSTrack::TrackRecord *)this->currTracks->GetItem((UOSInt)id);
		outObj = MemAlloc(Map::DrawObjectL, 1);
		outObj->objId = id;
		outObj->nPtOfst = 1;
		outObj->nPoint = (UInt32)(j = track->nRecords);
		outObj->ptOfstArr = MemAlloc(UInt32, 1);
		outObj->pointArr = MemAlloc(Double, outObj->nPoint * 2);
		outObj->ptOfstArr[0] = 0;
		ptPtr = outObj->pointArr;
		i = 0;
		while (i < j)
		{
			if (track->records[i].lat == 0 && track->records[i].lon == 0)
			{
				ptPtr[0] = lastLon;
				ptPtr[1] = lastLat;
			}
			else
			{
				lastLon = ptPtr[0] = track->records[i].lon;
				lastLat = ptPtr[1] = track->records[i].lat;
			}
			ptPtr += 2;
			i++;
		}
		outObj->flags = 0;
		outObj->lineColor = 0;
		mutUsage.EndUse();
		return outObj;
	}
}

Math::Vector2D *Map::GPSTrack::GetVectorById(void *session, Int64 id)
{
	UOSInt i;
	UOSInt j;
	Double lastLat;
	Double lastLon;
	Double lastAlt;
	Double *ptPtr;
	Double *altList;
	if (id < 0)
		return 0;
	Sync::MutexUsage mutUsage(this->recMut);
	if ((UInt64)id > this->currTracks->GetCount())
	{
		mutUsage.EndUse();
		return 0;
	}
	else if ((UInt64)id == this->currTracks->GetCount())
	{
		if (this->currTimes->GetCount() > 0)
		{
			Map::GPSTrack::GPSRecord *rec;
			lastLat = 0;
			lastLon = 0;
			lastAlt = 0;
			if (this->hasAltitude)
			{
				Math::Polyline3D *pl;

				NEW_CLASS(pl, Math::Polyline3D(4326, 1, j = this->currRecs->GetCount()));
				pl->GetPtOfstList(&i)[0] = 0;
				ptPtr = pl->GetPointList(&j);
				altList = pl->GetAltitudeList(&j);
				i = 0;
				while (i < j)
				{
					rec = (Map::GPSTrack::GPSRecord*)this->currRecs->GetItem(i);
					if (rec->lat == 0 && rec->lon == 0)
					{
						ptPtr[0] = lastLon;
						ptPtr[1] = lastLat;
						altList[i] = lastAlt;
					}
					else
					{
						lastLon = ptPtr[0] = rec->lon;
						lastLat = ptPtr[1] = rec->lat;
						lastAlt = altList[i] = rec->altitude;
					}
					ptPtr += 2;
					i++;
				}
				mutUsage.EndUse();
				return pl;
			}
			else
			{
				Math::Polyline *pl;
				NEW_CLASS(pl, Math::Polyline(4326, 1, j = this->currRecs->GetCount()));
				pl->GetPtOfstList(&i)[0] = 0;
				ptPtr = pl->GetPointList(&j);
				i = 0;
				while (i < j)
				{
					rec = (Map::GPSTrack::GPSRecord*)this->currRecs->GetItem(i);
					if (rec->lat == 0 && rec->lon == 0)
					{
						ptPtr[0] = lastLon;
						ptPtr[1] = lastLat;
					}
					else
					{
						lastLon = ptPtr[0] = rec->lon;
						lastLat = ptPtr[1] = rec->lat;
					}

					ptPtr += 2;
					i++;
				}
				mutUsage.EndUse();
				return pl;
			}
		}
		else
		{
			mutUsage.EndUse();
			return 0;
		}
	}
	else
	{
		Map::GPSTrack::TrackRecord *track;
		lastLat = 0;
		lastLon = 0;
		lastAlt = 0;
		track = (Map::GPSTrack::TrackRecord *)this->currTracks->GetItem((UOSInt)id);
		if (this->hasAltitude)
		{
			Math::Polyline3D *pl;

			NEW_CLASS(pl, Math::Polyline3D(4326, 1, track->nRecords));
			pl->GetPtOfstList(&i)[0] = 0;
			ptPtr = pl->GetPointList(&j);
			altList = pl->GetAltitudeList(&j);
			i = 0;
			while (i < j)
			{
				if (track->records[i].lat == 0 && track->records[i].lon == 0)
				{
					ptPtr[0] = lastLon;
					ptPtr[1] = lastLat;
					altList[i] = lastAlt;
				}
				else
				{
					lastLon = ptPtr[0] = track->records[i].lon;
					lastLat = ptPtr[1] = track->records[i].lat;
					lastAlt = altList[i] = track->records[i].altitude;
				}
				ptPtr += 2;
				i++;
			}
			mutUsage.EndUse();
			return pl;
		}
		else
		{
			Math::Polyline *pl;

			NEW_CLASS(pl, Math::Polyline(4326, 1, track->nRecords));
			pl->GetPtOfstList(&i)[0] = 0;
			ptPtr = pl->GetPointList(&j);
			i = 0;
			while (i < j)
			{
				if (track->records[i].lat == 0 && track->records[i].lon == 0)
				{
					ptPtr[0] = lastLon;
					ptPtr[1] = lastLat;
				}
				else
				{
					lastLon = ptPtr[0] = track->records[i].lon;
					lastLat = ptPtr[1] = track->records[i].lat;
				}
				ptPtr += 2;
				i++;
			}
			mutUsage.EndUse();
			return pl;
		}
	}
}

void Map::GPSTrack::ReleaseObject(void *session, DrawObjectL *obj)
{
	MemFree(obj->ptOfstArr);
	MemFree(obj->pointArr);
	MemFree(obj);
}

/*void Map::GPSTrack::ReleaseObject(void *session, DrawObjectDbl *obj)
{
	MemFree(obj->parts);
	MemFree(obj->points);
	MemFree(obj);
}*/

void Map::GPSTrack::AddUpdatedHandler(UpdatedHandler hdlr, void *obj)
{
	Sync::MutexUsage mutUsage(this->updMut);
	this->updHdlrs->Add(hdlr);
	this->updObjs->Add(obj);
	mutUsage.EndUse();
}

void Map::GPSTrack::RemoveUpdatedHandler(UpdatedHandler hdlr, void *obj)
{
	UOSInt i;
	Sync::MutexUsage mutUsage(this->updMut);
	i = this->updHdlrs->GetCount();
	while (i-- > 0)
	{
		if (this->updHdlrs->GetItem(i) == hdlr && this->updObjs->GetItem(i) == obj)
		{
			this->updHdlrs->RemoveAt(i);
			this->updObjs->RemoveAt(i);
		}
	}
	mutUsage.EndUse();
}

UOSInt Map::GPSTrack::GetTableNames(Data::ArrayList<const UTF8Char*> *names)
{
	names->Add(this->sourceName);
	names->Add((const UTF8Char*)"GPSData");
	return 2;
}

DB::DBReader *Map::GPSTrack::GetTableData(const UTF8Char *name, UOSInt maxCnt, void *ordering, void *condition)
{
	DB::DBReader *r;
	if (name != 0 && Text::StrEquals(name, (const UTF8Char*)"GPSData"))
	{
		NEW_CLASS(r, Map::GPSDataReader(this));
	}
	else
	{
		NEW_CLASS(r, Map::GPSTrackReader(this));
	}
	return r;
}

Map::IMapDrawLayer::ObjectClass Map::GPSTrack::GetObjectClass()
{
	return Map::IMapDrawLayer::OC_GPS_TRACK;
}

void Map::GPSTrack::NewTrack()
{
	if (this->currTimes->GetCount() > 0)
	{
		Sync::MutexUsage mutUsage(this->recMut);
		Map::GPSTrack::TrackRecord *rec;
		Map::GPSTrack::GPSRecord *recPtr;
		Map::GPSTrack::GPSRecord *gpsrec;
		UOSInt i;
		rec = MemAlloc(Map::GPSTrack::TrackRecord, 1);
		rec->nRecords = this->currTimes->GetCount();
		rec->maxLat = this->currMaxLat;
		rec->maxLon = this->currMaxLon;
		rec->minLat = this->currMinLat;
		rec->minLon = this->currMinLon;
		rec->name = this->currTrackName;
		recPtr = rec->records = MemAlloc(Map::GPSTrack::GPSRecord, rec->nRecords);
		i = 0;
		while (i < rec->nRecords)
		{
			gpsrec = (Map::GPSTrack::GPSRecord*)this->currRecs->GetItem(i);
			MemCopyNO(recPtr, gpsrec, sizeof(Map::GPSTrack::GPSRecord));
			MemFree(gpsrec);
			recPtr++;
			i++;
		}
		rec->extraData = MemAlloc(const UInt8 *, rec->nRecords);
		MemCopyNO(rec->extraData, this->currExtraData->GetArray(&i), sizeof(const UInt8*) * rec->nRecords);
		rec->extraDataSize = MemAlloc(UOSInt, rec->nRecords);
		MemCopyNO(rec->extraDataSize, this->currExtraSize->GetArray(&i), sizeof(UOSInt) * rec->nRecords);
		this->currRecs->Clear();
		this->currTimes->Clear();
		this->currExtraData->Clear();
		this->currExtraSize->Clear();
		this->currTracks->Add(rec);
		this->currTrackName = 0;
		mutUsage.EndUse();
		if (this->tmpRecord)
		{
			MemFree(this->tmpRecord);
			this->tmpRecord = 0;
		}
	}
}

UOSInt Map::GPSTrack::AddRecord(Map::GPSTrack::GPSRecord *rec)
{
	Sync::MutexUsage mutUsage(this->recMut);
	if (this->currTimes->GetCount() == 0)
	{
		this->currMaxLat = this->currMinLat = rec->lat;
		this->currMaxLon = this->currMinLon = rec->lon;
		if (this->currTracks->GetCount() == 0)
		{
			this->maxLat = this->minLat = rec->lat;
			this->maxLon = this->minLon = rec->lon;
		}
	}
	else
	{
		if (this->currMaxLat < rec->lat)
		{
			this->currMaxLat = rec->lat;
		}
		if (this->currMinLat > rec->lat)
		{
			this->currMinLat = rec->lat;
		}
		if (this->currMaxLon < rec->lon)
		{
			this->currMaxLon = rec->lon;
		}
		if (this->currMinLon > rec->lon)
		{
			this->currMinLon = rec->lon;
		}
	}

	if (this->maxLat < rec->lat)
	{
		this->maxLat = rec->lat;
	}
	if (this->minLat > rec->lat)
	{
		this->minLat = rec->lat;
	}
	if (this->maxLon < rec->lon)
	{
		this->maxLon = rec->lon;
	}
	if (this->minLon > rec->lon)
	{
		this->minLon = rec->lon;
	}

	Map::GPSTrack::GPSRecord *newRec;
	newRec = MemAlloc(Map::GPSTrack::GPSRecord, 1);
	MemCopyNO(newRec, rec, sizeof(Map::GPSTrack::GPSRecord));
	UOSInt i = this->currTimes->SortedInsert(rec->utcTimeTicks);
	this->currRecs->Insert(i, newRec);
	this->currExtraData->Insert(i, 0);
	this->currExtraSize->Insert(i, 0);
	if (this->tmpRecord)
	{
		MemFree(this->tmpRecord);
		this->tmpRecord = 0;
	}
	mutUsage.EndUse();
	
	UOSInt j;
	Sync::MutexUsage updMutUsage(this->updMut);
	j = this->updHdlrs->GetCount();
	while (j-- > 0)
	{
		this->updHdlrs->GetItem(j)(this->updObjs->GetItem(j));
	}
	updMutUsage.EndUse();
	return i;
}

Bool Map::GPSTrack::RemoveRecordRange(UOSInt index, UOSInt recStart, UOSInt recEnd)
{
	if (recStart > recEnd)
	{
		return false;
	}
	if (index > this->currTracks->GetCount())
	{
		return false;
	}
	else if (index == this->currTracks->GetCount())
	{
		const UInt8 *data;
		Sync::MutexUsage mutUsage(this->recMut);
		this->currTimes->RemoveRange(recStart, recEnd - recStart + 1);
		recEnd++;
		while (recEnd-- > recStart)
		{
			MemFree(this->currRecs->RemoveAt(recEnd));
			data = this->currExtraData->RemoveAt(recEnd);
			if (data)
			{
				MemFree((void*)data);
			}
		}
		if (this->tmpRecord)
		{
			MemFree(this->tmpRecord);
			this->tmpRecord = 0;
		}
		mutUsage.EndUse();
		return true;
	}
	else
	{
		UOSInt currCnt;
		Sync::MutexUsage mutUsage(this->recMut);
		Map::GPSTrack::TrackRecord *recs = this->currTracks->GetItem(index);
		currCnt = recs->nRecords;
		recEnd++;
		recs->nRecords -= recEnd - recStart;
		while (recEnd < currCnt)
		{
			MemCopyO(&recs->records[recStart], &recs->records[recEnd], sizeof(Map::GPSTrack::GPSRecord));
			if (recs->extraData[recStart])
			{
				MemFree((void*)recs->extraData[recStart]);
			}
			recs->extraData[recStart] = recs->extraData[recEnd];
			recs->extraDataSize[recStart] = recs->extraDataSize[recEnd];
			recStart++;
			recEnd++;
		}
		mutUsage.EndUse();
		return true;
	}
}


Bool Map::GPSTrack::GetHasAltitude()
{
	return this->hasAltitude;
}

void Map::GPSTrack::SetTrackName(const UTF8Char *name)
{
	if (this->currTrackName)
	{
		Text::StrDelNew(this->currTrackName);
	}
	if (name)
	{
		this->currTrackName = Text::StrCopyNew(name);
	}
	else
	{
		this->currTrackName = 0;
	}
}

void Map::GPSTrack::GetTrackNames(Data::ArrayListStrUTF8 *nameArr)
{
	UOSInt i = 0;
	UOSInt j = this->currTracks->GetCount();
	while (i < j)
	{
		nameArr->Add(this->currTracks->GetItem(i)->name);
		i++;
	}
	if (this->currRecs->GetCount() > 0)
	{
		nameArr->Add(this->currTrackName);
	}
}

const UTF8Char *Map::GPSTrack::GetTrackName(UOSInt index)
{
	if (index < this->currTracks->GetCount())
	{
		return this->currTracks->GetItem(index)->name;
	}
	else if (index == this->currTracks->GetCount() && this->currTimes->GetCount() > 0)
	{
		return this->currTrackName;
	}
	else
	{
		return 0;
	}

}

Bool Map::GPSTrack::GetTrackStartTime(UOSInt index, Data::DateTime *dt)
{
	if (index < this->currTracks->GetCount())
	{
		Map::GPSTrack::TrackRecord *track = this->currTracks->GetItem(index);
		dt->SetTicks(track->records[0].utcTimeTicks);
		return true;
	}
	else if (index == this->currTracks->GetCount() && this->currTimes->GetCount() > 0)
	{
		dt->SetTicks(this->currTimes->GetItem(0));
		return true;
	}
	else
	{
		return false;
	}
}

Bool Map::GPSTrack::GetTrackEndTime(UOSInt index, Data::DateTime *dt)
{
	if (index < this->currTracks->GetCount())
	{
		Map::GPSTrack::TrackRecord *track = this->currTracks->GetItem(index);
		dt->SetTicks(track->records[track->nRecords - 1].utcTimeTicks);
		return true;
	}
	else if (index == this->currTracks->GetCount() && this->currTimes->GetCount() > 0)
	{
		dt->SetTicks(this->currTimes->GetItem(this->currTimes->GetCount() - 1));
		return true;
	}
	else
	{
		return false;
	}
}

UOSInt Map::GPSTrack::GetTrackCnt()
{
	UOSInt cnt = this->currTracks->GetCount();
	if (this->currTimes->GetCount() > 0)
		cnt++;
	return cnt;
}

Map::GPSTrack::GPSRecord *Map::GPSTrack::GetTrack(UOSInt index, UOSInt *recordCnt)
{
	if (this->currTracks->GetCount() < index)
		return 0;
	if (this->currTracks->GetCount() == index)
	{
		if (this->currTimes->GetCount() == 0)
			return 0;
		UOSInt i = this->currTimes->GetCount();
		if (recordCnt)
			*recordCnt = i;
		if (this->tmpRecord)
		{
			return this->tmpRecord;
		}
		this->tmpRecord = MemAlloc(Map::GPSTrack::GPSRecord, this->currTimes->GetCount());
		while (i-- > 0)
		{
			MemCopyNO(&this->tmpRecord[i], this->currRecs->GetItem(i), sizeof(Map::GPSTrack::GPSRecord));
		}
		return this->tmpRecord;
	}
	else
	{
		Map::GPSTrack::TrackRecord *rec = (Map::GPSTrack::TrackRecord*)this->currTracks->GetItem(index);
		if (recordCnt)
			*recordCnt = rec->nRecords;
		return rec->records;
	}
}

void Map::GPSTrack::GetLatLonByTime(Data::DateTime *dt, Double *lat, Double *lon)
{
	GetLatLonByTicks(dt->ToTicks(), lat, lon);
}

void Map::GPSTrack::GetLatLonByTicks(Int64 ticks, Double *lat, Double *lon)
{
	OSInt si;
	if (this->currTimes->GetCount() > 0)
	{
		if (ticks >= this->currTimes->GetItem(0) && ticks <= this->currTimes->GetItem(this->currTimes->GetCount() - 1))
		{
			si = this->currTimes->SortedIndexOf(ticks);
			if (si >= 0)
			{
				Map::GPSTrack::GPSRecord *rec = this->currRecs->GetItem((UOSInt)si);
				*lat = rec->lat;
				*lon = rec->lon;
				return;
			}
			else
			{
				Int64 tDiff;
				Map::GPSTrack::GPSRecord *rec1 = this->currRecs->GetItem((UOSInt)~si - 1);
				Map::GPSTrack::GPSRecord *rec2 = this->currRecs->GetItem((UOSInt)~si);
				tDiff = rec2->utcTimeTicks - rec1->utcTimeTicks;
				*lat = (rec1->lat * (Double)(rec2->utcTimeTicks - ticks) + rec2->lat * (Double)(ticks - rec1->utcTimeTicks)) / (Double)tDiff;
				*lon = (rec1->lon * (Double)(rec2->utcTimeTicks - ticks) + rec2->lon * (Double)(ticks - rec1->utcTimeTicks)) / (Double)tDiff;
				return;
			}
		}
	}
	if (this->currTracks->GetCount() > 0)
	{
		Map::GPSTrack::TrackRecord *rec;
		UOSInt i;
		UOSInt j;
		i = this->currTracks->GetCount();
		while (i-- > 0)
		{
			rec = this->currTracks->GetItem(i);
			if (ticks >= rec->records[0].utcTimeTicks && ticks <= rec->records[rec->nRecords - 1].utcTimeTicks)
			{
				j = 0;
				while (j < rec->nRecords)
				{
					if (ticks <= rec->records[j].utcTimeTicks)
						break;
					j++;
				}
				if (ticks == rec->records[j].utcTimeTicks)
				{
					*lat = rec->records[j].lat;
					*lon = rec->records[j].lon;
					return;
				}
				else
				{
					Int64 tDiff;
					tDiff = rec->records[j].utcTimeTicks - rec->records[j - 1].utcTimeTicks;
					*lat = (rec->records[j - 1].lat * (Double)(rec->records[j].utcTimeTicks - ticks) + rec->records[j].lat * (Double)(ticks - rec->records[j - 1].utcTimeTicks)) / (Double)tDiff;
					*lon = (rec->records[j - 1].lon * (Double)(rec->records[j].utcTimeTicks - ticks) + rec->records[j].lon * (Double)(ticks - rec->records[j - 1].utcTimeTicks)) / (Double)tDiff;
					return;
				}
			}
		}
	}
	*lat = 0;
	*lon = 0;
}

void Map::GPSTrack::SetExtraParser(GPSExtraParser *parser)
{
	SDEL_CLASS(this->extraParser);
	this->extraParser = parser;
}

void Map::GPSTrack::SetExtraDataIndex(UOSInt recIndex, const UInt8 *data, UOSInt dataSize)
{
	Sync::MutexUsage mutUsage(this->recMut);
	if (recIndex < this->currExtraData->GetCount())
	{
		UInt8 *newData = MemAlloc(UInt8, dataSize);
		MemCopyNO(newData, data, dataSize);
		data = this->currExtraData->GetItem(recIndex);
		this->currExtraData->SetItem(recIndex, newData);
		this->currExtraSize->SetItem(recIndex, dataSize);
		if (data)
		{
			MemFree((void*)data);
		}
	}
	mutUsage.EndUse();
}

const UInt8 *Map::GPSTrack::GetExtraData(UOSInt trackIndex, UOSInt recIndex, UOSInt *dataSize)
{
	if (this->currTracks->GetCount() < trackIndex)
		return 0;
	if (this->currTracks->GetCount() == trackIndex)
	{
		if (this->currTimes->GetCount() <= recIndex)
			return 0;
		const UInt8 *data;
		Sync::MutexUsage mutUsage(this->recMut);
		*dataSize = this->currExtraSize->GetItem(recIndex);
		data = this->currExtraData->GetItem(recIndex);
		mutUsage.EndUse();
		return data;
	}
	else
	{
		Map::GPSTrack::TrackRecord *rec = (Map::GPSTrack::TrackRecord*)this->currTracks->GetItem(trackIndex);
		if (rec->nRecords <= recIndex)
			return 0;
		*dataSize = rec->extraDataSize[recIndex];
		return rec->extraData[recIndex];
	}
}

UOSInt Map::GPSTrack::GetExtraCount(UOSInt trackIndex, UOSInt recIndex)
{
	if (this->extraParser == 0)
		return 0;
	UOSInt dataSize;
	const UInt8 *data = this->GetExtraData(trackIndex, recIndex, &dataSize);
	if (data == 0)
		return 0;
	return this->extraParser->GetExtraCount(data, dataSize);
}

Bool Map::GPSTrack::GetExtraName(UOSInt trackIndex, UOSInt recIndex, UOSInt extIndex, Text::StringBuilderUTF *sb)
{
	if (this->extraParser == 0)
		return false;
	UOSInt dataSize;
	const UInt8 *data = this->GetExtraData(trackIndex, recIndex, &dataSize);
	if (data == 0)
		return false;
	return this->extraParser->GetExtraName(data, dataSize, extIndex, sb);
}

Bool Map::GPSTrack::GetExtraValueStr(UOSInt trackIndex, UOSInt recIndex, UOSInt extIndex, Text::StringBuilderUTF *sb)
{
	if (this->extraParser == 0)
		return false;
	UOSInt dataSize;
	const UInt8 *data = this->GetExtraData(trackIndex, recIndex, &dataSize);
	if (data == 0)
		return false;
	return this->extraParser->GetExtraValueStr(data, dataSize, extIndex, sb);
}

Map::GPSTrackReader::GPSTrackReader(Map::GPSTrack *gps) : Map::MapLayerReader(gps)
{
	this->gps = gps;
}

Map::GPSTrackReader::~GPSTrackReader()
{
}

DB::DBReader::DateErrType Map::GPSTrackReader::GetDate(UOSInt colIndex, Data::DateTime *outVal)
{
	UOSInt id = (UOSInt)this->currIndex;
	if (colIndex != 1 && colIndex != 2)
		return DB::DBReader::DET_ERROR;
	if (colIndex == 2)
	{
		if (this->gps->GetTrackStartTime(id, outVal))
			return DB::DBReader::DET_OK;
	}
	else if (colIndex == 3)
	{
		if (this->gps->GetTrackEndTime(id, outVal))
			return DB::DBReader::DET_OK;
	}
	return DB::DBReader::DET_ERROR;
}

Map::GPSDataReader::GPSDataReader(Map::GPSTrack *gps)
{
	this->gps = gps;
	this->currRow = -1;
	this->currRec = 0;
}

Map::GPSDataReader::~GPSDataReader()
{
}

Bool Map::GPSDataReader::ReadNext()
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt rowLeft;
	Map::GPSTrack::GPSRecord *rec;
	this->currRow++;
	rowLeft = (UOSInt)this->currRow;
	i = 0;
	j = this->gps->GetTrackCnt();
	while (i < j)
	{
		rec = this->gps->GetTrack(i, &k);
		if (rowLeft < k)
		{
			currRec = &rec[rowLeft];
			return true;
		}
		else
		{
			rowLeft -= k;
		}
		i++;
	}
	return false;
}

UOSInt Map::GPSDataReader::ColCount()
{
	if (this->gps->GetHasAltitude())
		return 10;
	else
		return 9;
}

OSInt Map::GPSDataReader::GetRowChanged()
{
	return -1;
}

Int32 Map::GPSDataReader::GetInt32(UOSInt colIndex)
{
	if (this->currRec == 0)
		return 0;
	if (colIndex == 2)
	{
		return Math::Double2Int32(this->currRec->lat);
	}
	else if (colIndex == 3)
	{
		return Math::Double2Int32(this->currRec->lon);
	}
	else if (colIndex == 4)
	{
		return Math::Double2Int32(this->currRec->speed);
	}
	else if (colIndex == 5)
	{
		return Math::Double2Int32(this->currRec->heading);
	}
	else if (colIndex == 6)
	{
		return this->currRec->valid;
	}
	else if (colIndex == 7)
	{
		return this->currRec->nSateUsed;
	}
	else if (colIndex == 8)
	{
		return this->currRec->nSateView;
	}
	else if (colIndex == 9)
	{
		return Math::Double2Int32(this->currRec->altitude);
	}
	return 0;
}

Int64 Map::GPSDataReader::GetInt64(UOSInt colIndex)
{
	return this->GetInt32(colIndex);
}

WChar *Map::GPSDataReader::GetStr(UOSInt colIndex, WChar *buff)
{
	if (this->currRec == 0)
		return 0;
	if (colIndex == 0)
	{
		Data::DateTime dt;
		UTF8Char sbuff[32];
		GetDate(0, &dt);
		dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
		return Text::StrUTF8_WChar(buff, sbuff, 0);
	}
	else if (colIndex == 1)
	{
		return buff;
	}
	else if (colIndex >= 2 && colIndex <= 9)
	{
		return Text::StrDouble(buff, this->GetDbl(colIndex));
	}
	return 0;
}

Bool Map::GPSDataReader::GetStr(UOSInt colIndex, Text::StringBuilderUTF *sb)
{
	UTF8Char sbuff[32];
	if (this->GetStr(colIndex, sbuff, sizeof(sbuff)))
	{
		sb->Append(sbuff);
		return true;
	}
	return false;
}

const UTF8Char *Map::GPSDataReader::GetNewStr(UOSInt colIndex)
{
	UTF8Char sbuff[64];
	if (this->GetStr(colIndex, sbuff, sizeof(sbuff)))
	{
		return Text::StrCopyNew(sbuff);
	}
	return 0;
}

UTF8Char *Map::GPSDataReader::GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize)
{
	if (this->currRec == 0)
		return 0;
	if (colIndex == 0)
	{
		Data::DateTime dt;
		GetDate(0, &dt);
		return dt.ToString(buff, "yyyy-MM-dd HH:mm:ss");
	}
	else if (colIndex == 1)
	{
		return buff;
	}
	else if (colIndex >= 2 && colIndex <= 9)
	{
		return Text::StrDouble(buff, this->GetDbl(colIndex));
	}
	return 0;
}

DB::DBReader::DateErrType Map::GPSDataReader::GetDate(UOSInt colIndex, Data::DateTime *outVal)
{
	if (this->currRec == 0)
		return DB::DBReader::DET_ERROR;
	if (colIndex == 0)
	{
		outVal->SetTicks(this->currRec->utcTimeTicks);
		outVal->ToLocalTime();
		return DB::DBReader::DET_OK;
	}
	return DB::DBReader::DET_ERROR;
}

Double Map::GPSDataReader::GetDbl(UOSInt colIndex)
{
	if (this->currRec == 0)
		return 0;
	if (colIndex == 2)
	{
		return this->currRec->lat;
	}
	else if (colIndex == 3)
	{
		return this->currRec->lon;
	}
	else if (colIndex == 4)
	{
		return this->currRec->speed;
	}
	else if (colIndex == 5)
	{
		return this->currRec->heading;
	}
	else if (colIndex == 6)
	{
		return this->currRec->valid;
	}
	else if (colIndex == 7)
	{
		return this->currRec->nSateUsed;
	}
	else if (colIndex == 8)
	{
		return this->currRec->nSateView;
	}
	else if (colIndex == 9)
	{
		return this->currRec->altitude;
	}
	return 0;
}

Bool Map::GPSDataReader::GetBool(UOSInt colIndex)
{
	return GetInt32(colIndex) != 0;
}

UOSInt Map::GPSDataReader::GetBinarySize(UOSInt colIndex)
{
	return 0;
}

UOSInt Map::GPSDataReader::GetBinary(UOSInt colIndex, UInt8 *buff)
{
	return 0;
}

Math::Vector2D *Map::GPSDataReader::GetVector(UOSInt colIndex)
{
	if (this->currRec == 0)
		return 0;
	if (colIndex != 1)
		return 0;
	Math::Point *pt;
	if (this->gps->GetHasAltitude())
	{
		NEW_CLASS(pt, Math::Point3D(4326, this->currRec->lon, this->currRec->lat, this->currRec->altitude));
	}
	else
	{
		NEW_CLASS(pt, Math::Point(4326, this->currRec->lon, this->currRec->lat));
	}
	return pt;
}

UTF8Char *Map::GPSDataReader::GetName(UOSInt colIndex, UTF8Char *buff)
{
	const UTF8Char *sptr = GetName(colIndex);
	if (sptr)
		return Text::StrConcat(buff, sptr);
	return 0;
}

Bool Map::GPSDataReader::IsNull(UOSInt colIndex)
{
	if (this->currRec == 0)
		return true;
	if (colIndex > 9)
		return true;
	if (colIndex == 9 && !this->gps->GetHasAltitude())
		return true;
	return false;
}

DB::DBUtil::ColType Map::GPSDataReader::GetColType(UOSInt colIndex, UOSInt *colSize)
{
	switch (colIndex)
	{
	case 0:
		return DB::DBUtil::CT_DateTime;
	case 1:
		return DB::DBUtil::CT_Vector;
	case 2:
		return DB::DBUtil::CT_Double;
	case 3:
		return DB::DBUtil::CT_Double;
	case 4:
		return DB::DBUtil::CT_Double;
	case 5:
		return DB::DBUtil::CT_Double;
	case 6:
		return DB::DBUtil::CT_Int32;
	case 7:
		return DB::DBUtil::CT_Int32;
	case 8:
		return DB::DBUtil::CT_Int32;
	case 9:
		if (this->gps->GetHasAltitude())
		{
			return DB::DBUtil::CT_Double;
		}
		else
		{
			return DB::DBUtil::CT_Unknown;
		}
	default:
		return DB::DBUtil::CT_Unknown;
	}
}

Bool Map::GPSDataReader::GetColDef(UOSInt colIndex, DB::ColDef *colDef)
{
	colDef->SetNotNull(true);
	colDef->SetPK(false);
	colDef->SetAutoInc(false);
	colDef->SetDefVal(0);
	colDef->SetAttr(0);
	switch (colIndex)
	{
	case 0:
		colDef->SetColName(this->GetName(colIndex));
		colDef->SetColType(DB::DBUtil::CT_DateTime);
		colDef->SetColSize(20);
		colDef->SetColDP(0);
		colDef->SetPK(true);
		return true;
	case 1:
		colDef->SetColName(this->GetName(colIndex));
		colDef->SetColType(DB::DBUtil::CT_Vector);
		colDef->SetColSize(0x7fffffff);
		colDef->SetColDP(0);
		colDef->SetPK(false);
		return true;
	case 2:
		colDef->SetColName(this->GetName(colIndex));
		colDef->SetColType(DB::DBUtil::CT_Double);
		colDef->SetColSize(30);
		colDef->SetColDP(15);
		return true;
	case 3:
		colDef->SetColName(this->GetName(colIndex));
		colDef->SetColType(DB::DBUtil::CT_Double);
		colDef->SetColSize(30);
		colDef->SetColDP(15);
		return true;
	case 4:
		colDef->SetColName(this->GetName(colIndex));
		colDef->SetColType(DB::DBUtil::CT_Double);
		colDef->SetColSize(30);
		colDef->SetColDP(15);
		return true;
	case 5:
		colDef->SetColName(this->GetName(colIndex));
		colDef->SetColType(DB::DBUtil::CT_Double);
		colDef->SetColSize(30);
		colDef->SetColDP(15);
		return true;
	case 6:
		colDef->SetColName(this->GetName(colIndex));
		colDef->SetColType(DB::DBUtil::CT_Int32);
		colDef->SetColSize(11);
		colDef->SetColDP(0);
		return true;
	case 7:
		colDef->SetColName(this->GetName(colIndex));
		colDef->SetColType(DB::DBUtil::CT_Int32);
		colDef->SetColSize(11);
		colDef->SetColDP(0);
		return true;
	case 8:
		colDef->SetColName(this->GetName(colIndex));
		colDef->SetColType(DB::DBUtil::CT_Int32);
		colDef->SetColSize(11);
		colDef->SetColDP(0);
		return true;
	case 9:
		if (this->gps->GetHasAltitude())
		{
			colDef->SetColName(this->GetName(colIndex));
			colDef->SetColType(DB::DBUtil::CT_Double);
			colDef->SetColSize(30);
			colDef->SetColDP(15);
			return true;
		}
		else
		{
			return false;
		}
	default:
		return false;
	}
}

void Map::GPSDataReader::DelNewStr(const UTF8Char *s)
{
	Text::StrDelNew(s);
}

const UTF8Char *Map::GPSDataReader::GetName(UOSInt colIndex)
{
	switch (colIndex)
	{
	case 0:
		return (const UTF8Char*)"Time";
	case 1:
		return (const UTF8Char*)"Shape";
	case 2:
		return (const UTF8Char*)"Latitude";
	case 3:
		return (const UTF8Char*)"Longitude";
	case 4:
		return (const UTF8Char*)"Speed";
	case 5:
		return (const UTF8Char*)"Heading";
	case 6:
		return (const UTF8Char*)"GPSFix";
	case 7:
		return (const UTF8Char*)"nSateUsed";
	case 8:
		return (const UTF8Char*)"nSateView";
	case 9:
		if (this->gps->GetHasAltitude())
		{
			return (const UTF8Char*)"Altitude";
		}
		else
		{
			return 0;
		}
	default:
		return 0;
	}
}
