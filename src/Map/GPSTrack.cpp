#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "DB/ColDef.h"
#include "DB/TableDef.h"
#include "Map/GPSTrack.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math.h"
#include "Math/Geometry/PointZ.h"
#include "Math/Geometry/LineString.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"

Map::GPSTrack::GPSTrack(NotNullPtr<Text::String> sourceName, Bool hasAltitude, UInt32 codePage, Text::String *layerName) : Map::MapDrawLayer(sourceName, 0, layerName)
{
	this->codePage = codePage;
	this->currTrackName = 0;
	this->extraParser = 0;
	this->tmpRecord = 0;
	this->maxLat = this->minLat = this->maxLon = this->minLon = 0;
	this->hasAltitude = hasAltitude;
	this->csys = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84);
}

Map::GPSTrack::GPSTrack(Text::CString sourceName, Bool hasAltitude, UInt32 codePage, Text::CString layerName) : Map::MapDrawLayer(sourceName, 0, layerName)
{
	this->codePage = codePage;
	this->currTrackName = 0;
	this->extraParser = 0;
	this->tmpRecord = 0;
	this->maxLat = this->minLat = this->maxLon = this->minLon = 0;
	this->hasAltitude = hasAltitude;
	this->csys = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84);
}

Map::GPSTrack::~GPSTrack()
{
	UOSInt i;
	const UInt8 *data;
	i = this->currRecs.GetCount();
	while (i-- > 0)
	{
		MemFreeA(this->currRecs.GetItem(i));
		data = this->currExtraData.GetItem(i);
		if (data)
		{
			MemFree((void*)data);
		}
	}
	SDEL_CLASS(this->extraParser);
	i = this->currTracks.GetCount();
	while (i-- > 0)
	{
		Map::GPSTrack::TrackRecord *rec;
		rec = this->currTracks.GetItem(i);
		if (rec->name)
		{
			rec->name->Release();
		}
		MemFree(rec->extraData);
		MemFree(rec->extraDataSize);
		MemFreeA(rec->records);
		MemFree(rec);
	}
	if (tmpRecord)
	{
		MemFreeA(tmpRecord);
		tmpRecord = 0;
	}
	if (this->currTrackName)
	{
		this->currTrackName->Release();
		this->currTrackName = 0;
	}
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

UOSInt Map::GPSTrack::GetAllObjectIds(Data::ArrayListInt64 *outArr, NameArray **nameArr)
{
	Sync::MutexUsage mutUsage(this->recMut);
	UOSInt i = 0;
	UOSInt j = this->currTracks.GetCount();
	while (i < j)
	{
		outArr->Add((Int64)i);
		i++;
	}
	if (this->currTimes.GetCount() > 0)
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

UOSInt Map::GPSTrack::GetObjectIds(Data::ArrayListInt64 *outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	return GetObjectIdsMapXY(outArr, nameArr, rect.ToDouble() / mapRate, keepEmpty);
}

UOSInt Map::GPSTrack::GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	Sync::MutexUsage mutUsage(this->recMut);
	rect = rect.Reorder();
	UOSInt cnt = 0;
	UOSInt i = 0;
	UOSInt j = this->currTracks.GetCount();
	while (i < j)
	{
		Map::GPSTrack::TrackRecord *track;
		track = this->currTracks.GetItem(i);
		if (track->minLon <= rect.br.x && track->maxLon >= rect.tl.x && track->minLat <= rect.br.y && track->maxLat >= rect.tl.y)
		{
			outArr->Add((Int64)i);
			cnt++;
		}
		i++;
	}
	if (this->currTimes.GetCount() > 0)
	{
		if (this->currMinLon <= rect.br.x && this->currMaxLon >= rect.tl.x && this->currMinLat <= rect.br.y && this->currMaxLat >= rect.tl.y)
		{
			outArr->Add((Int64)j);
			cnt++;
		}
	}
	return cnt;
}

Int64 Map::GPSTrack::GetObjectIdMax()
{
	if (this->currTimes.GetCount() > 0)
	{
		return (Int64)this->currTracks.GetCount();
	}
	else
	{
		return (Int64)this->currTracks.GetCount() - 1;
	}
}

void Map::GPSTrack::ReleaseNameArr(NameArray *nameArr)
{
}

UTF8Char *Map::GPSTrack::GetString(UTF8Char *buff, UOSInt buffSize, NameArray *nameArr, Int64 id, UOSInt strIndex)
{
	if (strIndex >= 3)
		return 0;

	if ((UInt64)id > this->currTracks.GetCount())
		return 0;
	if (id < 0)
		return 0;
	Data::DateTime dt;
	UTF8Char *sptr;
	if ((UInt64)id == this->currTracks.GetCount())
	{
		if (strIndex == 0)
		{
			dt.SetTicks(this->currTimes.GetItem(0));
			sptr = dt.ToString(buff, "yyyy-MM-dd HH:mm:ss.fff");
			sptr = Text::StrConcatC(sptr, UTF8STRC(" - "));
			dt.SetTicks(this->currTimes.GetItem(this->currTimes.GetCount() - 1));
			sptr = dt.ToString(sptr, "yyyy-MM-dd HH:mm:ss.fff");
			return sptr;
		}
		else if (strIndex == 1)
		{
			dt.SetTicks(this->currTimes.GetItem(0));
			return dt.ToString(buff, "yyyy-MM-dd HH:mm:ss.fff");
		}
		else if (strIndex == 2)
		{
			dt.SetTicks(this->currTimes.GetItem(this->currTimes.GetCount() - 1));
			return dt.ToString(buff, "yyyy-MM-dd HH:mm:ss.fff");
		}
		else
		{
			return 0;
		}
	}
	else
	{
		Map::GPSTrack::TrackRecord *track = this->currTracks.GetItem((UOSInt)id);
		if (strIndex == 0)
		{
			dt.SetInstant(track->records[0].recTime);
			sptr = dt.ToStringNoZone(buff);
			sptr = Text::StrConcatC(sptr, UTF8STRC(" - "));
			dt.SetInstant(track->records[track->nRecords - 1].recTime);
			sptr = dt.ToStringNoZone(sptr);
			return sptr;
		}
		else if (strIndex == 1)
		{
			dt.SetInstant(track->records[0].recTime);
			return dt.ToStringNoZone(buff);
		}
		else if (strIndex == 2)
		{
			dt.SetInstant(track->records[track->nRecords - 1].recTime);
			return dt.ToStringNoZone(buff);
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
		return Text::StrConcatC(buff, UTF8STRC("Name"));
	}
	else if (colIndex == 1)
	{
		return Text::StrConcatC(buff, UTF8STRC("Start Time"));
	}
	else if (colIndex == 2)
	{
		return Text::StrConcatC(buff, UTF8STRC("End Time"));
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
		return DB::DBUtil::CT_VarUTF8Char;
	}
	else if (colIndex == 1 || colIndex == 2)
	{
		if (colSize)
		{
			*colSize = 3;
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

Bool Map::GPSTrack::GetColumnDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef)
{
	switch(colIndex)
	{
	case 0:
		colDef->SetColName(CSTR("Name"));
		colDef->SetColSize(256);
		colDef->SetColDP(0);
		colDef->SetColType(DB::DBUtil::CT_VarUTF8Char);
		colDef->SetDefVal(CSTR_NULL);
		colDef->SetNotNull(false);
		colDef->SetPK(false);
		colDef->SetAutoIncNone();
		colDef->SetAttr(CSTR_NULL);
		return true;
	case 1:
		colDef->SetColName(CSTR("Start Time"));
		colDef->SetColSize(3);
		colDef->SetColDP(0);
		colDef->SetColType(DB::DBUtil::CT_DateTime);
		colDef->SetDefVal(CSTR_NULL);
		colDef->SetNotNull(false);
		colDef->SetPK(false);
		colDef->SetAutoIncNone();
		colDef->SetAttr(CSTR_NULL);
		return true;
	case 2:
		colDef->SetColName(CSTR("End Time"));
		colDef->SetColSize(3);
		colDef->SetColDP(0);
		colDef->SetColType(DB::DBUtil::CT_DateTime);
		colDef->SetDefVal(CSTR_NULL);
		colDef->SetNotNull(false);
		colDef->SetPK(false);
		colDef->SetAutoIncNone();
		colDef->SetAttr(CSTR_NULL);
		return true;
	default:
		return false;
	}
}

UInt32 Map::GPSTrack::GetCodePage()
{
	return this->codePage;
}

Bool Map::GPSTrack::GetBounds(Math::RectAreaDbl *bounds)
{
	*bounds = Math::RectAreaDbl(Math::Coord2DDbl(minLon, minLat), Math::Coord2DDbl(maxLon, maxLat));
	return this->minLon != 0 || this->minLat != 0 || this->maxLon != 0 || this->maxLat != 0;
}

Map::GetObjectSess *Map::GPSTrack::BeginGetObject()
{
	return (Map::GetObjectSess*)-1;
}

void Map::GPSTrack::EndGetObject(Map::GetObjectSess *session)
{
}

Math::Geometry::Vector2D *Map::GPSTrack::GetNewVectorById(Map::GetObjectSess *session, Int64 id)
{
	UOSInt i;
	UOSInt j;
	Math::Coord2DDbl lastPos;
	Double lastAlt;
	Math::Coord2DDbl *ptPtr;
	Double *altList;
	if (id < 0)
		return 0;
	Sync::MutexUsage mutUsage(this->recMut);
	if ((UInt64)id > this->currTracks.GetCount())
	{
		mutUsage.EndUse();
		return 0;
	}
	else if ((UInt64)id == this->currTracks.GetCount())
	{
		if (this->currTimes.GetCount() > 0)
		{
			Map::GPSTrack::GPSRecord3 *rec;
			lastPos = Math::Coord2DDbl(0, 0);
			lastAlt = 0;
			if (this->hasAltitude)
			{
				Math::Geometry::LineString *pl;

				NEW_CLASS(pl, Math::Geometry::LineString(4326, j = this->currRecs.GetCount(), true, false));
				ptPtr = pl->GetPointList(&j);
				altList = pl->GetZList(&j);
				i = 0;
				while (i < j)
				{
					rec = this->currRecs.GetItem(i);
					if (rec->pos.IsZero())
					{
						*ptPtr = lastPos;
						altList[i] = lastAlt;
					}
					else
					{
						lastPos = *ptPtr = rec->pos;
						lastAlt = altList[i] = rec->altitude;
					}
					ptPtr += 1;
					i++;
				}
				mutUsage.EndUse();
				return pl;
			}
			else
			{
				Math::Geometry::LineString *pl;
				NEW_CLASS(pl, Math::Geometry::LineString(4326, j = this->currRecs.GetCount(), false, false));
				ptPtr = pl->GetPointList(&j);
				i = 0;
				while (i < j)
				{
					rec = this->currRecs.GetItem(i);
					if (rec->pos.IsZero())
					{
						*ptPtr = lastPos;
					}
					else
					{
						lastPos = *ptPtr = rec->pos;
					}

					ptPtr += 1;
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
		lastPos = Math::Coord2DDbl(0, 0);
		lastAlt = 0;
		track = this->currTracks.GetItem((UOSInt)id);
		if (this->hasAltitude)
		{
			Math::Geometry::LineString *pl;

			NEW_CLASS(pl, Math::Geometry::LineString(4326, track->nRecords, true, false));
			ptPtr = pl->GetPointList(&j);
			altList = pl->GetZList(&j);
			i = 0;
			while (i < j)
			{
				if (track->records[i].pos.IsZero())
				{
					*ptPtr = lastPos;
					altList[i] = lastAlt;
				}
				else
				{
					lastPos = *ptPtr = track->records[i].pos;
					lastAlt = altList[i] = track->records[i].altitude;
				}
				ptPtr += 1;
				i++;
			}
			mutUsage.EndUse();
			return pl;
		}
		else
		{
			Math::Geometry::LineString *pl;

			NEW_CLASS(pl, Math::Geometry::LineString(4326, track->nRecords, false, false));
			ptPtr = pl->GetPointList(&j);
			i = 0;
			while (i < j)
			{
				if (track->records[i].pos.IsZero())
				{
					*ptPtr = lastPos;
				}
				else
				{
					lastPos = *ptPtr = track->records[i].pos;
				}
				ptPtr += 1;
				i++;
			}
			mutUsage.EndUse();
			return pl;
		}
	}
}

void Map::GPSTrack::AddUpdatedHandler(UpdatedHandler hdlr, void *obj)
{
	Sync::MutexUsage mutUsage(this->updMut);
	this->updHdlrs.Add(hdlr);
	this->updObjs.Add(obj);
	mutUsage.EndUse();
}

void Map::GPSTrack::RemoveUpdatedHandler(UpdatedHandler hdlr, void *obj)
{
	UOSInt i;
	Sync::MutexUsage mutUsage(this->updMut);
	i = this->updHdlrs.GetCount();
	while (i-- > 0)
	{
		if (this->updHdlrs.GetItem(i) == hdlr && this->updObjs.GetItem(i) == obj)
		{
			this->updHdlrs.RemoveAt(i);
			this->updObjs.RemoveAt(i);
		}
	}
	mutUsage.EndUse();
}

UOSInt Map::GPSTrack::QueryTableNames(Text::CString schemaName, Data::ArrayListNN<Text::String> *names)
{
	if (schemaName.leng != 0)
		return 0;
	names->Add(this->sourceName->Clone());
	names->Add(Text::String::New(UTF8STRC("GPSData")));
	return 2;
}

DB::DBReader *Map::GPSTrack::QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListNN<Text::String> *columnName, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	DB::DBReader *r;
	if (tableName.v != 0 && tableName.Equals(UTF8STRC("GPSData")))
	{
		NEW_CLASS(r, Map::GPSDataReader(*this));
	}
	else
	{
		NEW_CLASS(r, Map::GPSTrackReader(*this));
	}
	return r;
}

DB::TableDef *Map::GPSTrack::GetTableDef(Text::CString schemaName, Text::CString tableName)
{
	UOSInt i = 0;
	UOSInt j;
	NotNullPtr<DB::ColDef> col;
	DB::TableDef *tab = 0;
	if (tableName.v != 0 && tableName.Equals(UTF8STRC("GPSData")))
	{
		if (this->hasAltitude)
		{
			j = 17;
		}
		else
		{
			j = 16;
		}
		NEW_CLASS(tab, DB::TableDef(schemaName, tableName));
		while (i < j)
		{
			NEW_CLASSNN(col, DB::ColDef(Text::String::NewEmpty()));
			GPSDataReader::GetColDefV(i, col, this->hasAltitude);
			tab->AddCol(col);
			i++;
		}
	}
	else
	{
		j = 4;
		NEW_CLASS(tab, DB::TableDef(schemaName, tableName));
		while (i < j)
		{
			NEW_CLASSNN(col, DB::ColDef(Text::String::NewEmpty()));
			Map::MapLayerReader::GetColDefV(i, col, *this);
			tab->AddCol(col);
			i++;
		}
	}
	return tab;
}

Map::MapDrawLayer::ObjectClass Map::GPSTrack::GetObjectClass()
{
	return Map::MapDrawLayer::OC_GPS_TRACK;
}

void Map::GPSTrack::NewTrack()
{
	if (this->currTimes.GetCount() > 0)
	{
		Sync::MutexUsage mutUsage(this->recMut);
		Map::GPSTrack::TrackRecord *rec;
		Map::GPSTrack::GPSRecord3 *recPtr;
		Map::GPSTrack::GPSRecord3 *gpsrec;
		UOSInt i;
		rec = MemAlloc(Map::GPSTrack::TrackRecord, 1);
		rec->nRecords = this->currTimes.GetCount();
		rec->maxLat = this->currMaxLat;
		rec->maxLon = this->currMaxLon;
		rec->minLat = this->currMinLat;
		rec->minLon = this->currMinLon;
		rec->name = this->currTrackName;
		recPtr = rec->records = MemAllocA(Map::GPSTrack::GPSRecord3, rec->nRecords);
		i = 0;
		while (i < rec->nRecords)
		{
			gpsrec = this->currRecs.GetItem(i);
			MemCopyNO(recPtr, gpsrec, sizeof(Map::GPSTrack::GPSRecord3));
			MemFreeA(gpsrec);
			recPtr++;
			i++;
		}
		rec->extraData = MemAlloc(const UInt8 *, rec->nRecords);
		MemCopyNO(rec->extraData, this->currExtraData.GetArray(&i), sizeof(const UInt8*) * rec->nRecords);
		rec->extraDataSize = MemAlloc(UOSInt, rec->nRecords);
		MemCopyNO(rec->extraDataSize, this->currExtraSize.GetArray(&i), sizeof(UOSInt) * rec->nRecords);
		this->currRecs.Clear();
		this->currTimes.Clear();
		this->currExtraData.Clear();
		this->currExtraSize.Clear();
		this->currTracks.Add(rec);
		this->currTrackName = 0;
		mutUsage.EndUse();
		if (this->tmpRecord)
		{
			MemFreeA(this->tmpRecord);
			this->tmpRecord = 0;
		}
	}
}

UOSInt Map::GPSTrack::AddRecord(NotNullPtr<Map::GPSTrack::GPSRecord3> rec)
{
	Sync::MutexUsage mutUsage(this->recMut);
	if (this->currTimes.GetCount() == 0)
	{
		this->currMaxLat = this->currMinLat = rec->pos.GetLat();
		this->currMaxLon = this->currMinLon = rec->pos.GetLon();
		if (this->currTracks.GetCount() == 0)
		{
			this->maxLat = this->minLat = rec->pos.GetLat();
			this->maxLon = this->minLon = rec->pos.GetLon();
		}
	}
	else
	{
		if (this->currMaxLat < rec->pos.GetLat())
		{
			this->currMaxLat = rec->pos.GetLat();
		}
		if (this->currMinLat > rec->pos.GetLat())
		{
			this->currMinLat = rec->pos.GetLat();
		}
		if (this->currMaxLon < rec->pos.GetLon())
		{
			this->currMaxLon = rec->pos.GetLon();
		}
		if (this->currMinLon > rec->pos.GetLon())
		{
			this->currMinLon = rec->pos.GetLon();
		}
	}

	if (this->maxLat < rec->pos.GetLat())
	{
		this->maxLat = rec->pos.GetLat();
	}
	if (this->minLat > rec->pos.GetLat())
	{
		this->minLat = rec->pos.GetLat();
	}
	if (this->maxLon < rec->pos.GetLon())
	{
		this->maxLon = rec->pos.GetLon();
	}
	if (this->minLon > rec->pos.GetLon())
	{
		this->minLon = rec->pos.GetLon();
	}

	Map::GPSTrack::GPSRecord3 *newRec;
	newRec = MemAllocA(Map::GPSTrack::GPSRecord3, 1);
	MemCopyNO(newRec, rec.Ptr(), sizeof(Map::GPSTrack::GPSRecord3));
	UOSInt i = this->currTimes.SortedInsert(rec->recTime.ToTicks());
	this->currRecs.Insert(i, newRec);
	this->currExtraData.Insert(i, 0);
	this->currExtraSize.Insert(i, 0);
	if (this->tmpRecord)
	{
		MemFreeA(this->tmpRecord);
		this->tmpRecord = 0;
	}
	mutUsage.EndUse();
	
	UOSInt j;
	Sync::MutexUsage updMutUsage(this->updMut);
	j = this->updHdlrs.GetCount();
	while (j-- > 0)
	{
		this->updHdlrs.GetItem(j)(this->updObjs.GetItem(j));
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
	if (index > this->currTracks.GetCount())
	{
		return false;
	}
	else if (index == this->currTracks.GetCount())
	{
		const UInt8 *data;
		Sync::MutexUsage mutUsage(this->recMut);
		this->currTimes.RemoveRange(recStart, recEnd - recStart + 1);
		recEnd++;
		while (recEnd-- > recStart)
		{
			MemFreeA(this->currRecs.RemoveAt(recEnd));
			data = this->currExtraData.RemoveAt(recEnd);
			if (data)
			{
				MemFree((void*)data);
			}
		}
		if (this->tmpRecord)
		{
			MemFreeA(this->tmpRecord);
			this->tmpRecord = 0;
		}
		mutUsage.EndUse();
		return true;
	}
	else
	{
		UOSInt currCnt;
		Sync::MutexUsage mutUsage(this->recMut);
		Map::GPSTrack::TrackRecord *recs = this->currTracks.GetItem(index);
		currCnt = recs->nRecords;
		recEnd++;
		recs->nRecords -= recEnd - recStart;
		while (recEnd < currCnt)
		{
			MemCopyO(&recs->records[recStart], &recs->records[recEnd], sizeof(Map::GPSTrack::GPSRecord3));
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

void Map::GPSTrack::SetTrackName(Text::CString name)
{
	SDEL_STRING(this->currTrackName);
	this->currTrackName = Text::String::NewOrNull(name);
}

void Map::GPSTrack::GetTrackNames(Data::ArrayListString *nameArr)
{
	UOSInt i = 0;
	UOSInt j = this->currTracks.GetCount();
	while (i < j)
	{
		nameArr->Add(this->currTracks.GetItem(i)->name);
		i++;
	}
	if (this->currRecs.GetCount() > 0)
	{
		nameArr->Add(this->currTrackName);
	}
}

Text::String *Map::GPSTrack::GetTrackName(UOSInt index)
{
	if (index < this->currTracks.GetCount())
	{
		return this->currTracks.GetItem(index)->name;
	}
	else if (index == this->currTracks.GetCount() && this->currTimes.GetCount() > 0)
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
	if (index < this->currTracks.GetCount())
	{
		Map::GPSTrack::TrackRecord *track = this->currTracks.GetItem(index);
		dt->SetInstant(track->records[0].recTime);
		return true;
	}
	else if (index == this->currTracks.GetCount() && this->currTimes.GetCount() > 0)
	{
		dt->SetTicks(this->currTimes.GetItem(0));
		return true;
	}
	else
	{
		return false;
	}
}

Bool Map::GPSTrack::GetTrackEndTime(UOSInt index, Data::DateTime *dt)
{
	if (index < this->currTracks.GetCount())
	{
		Map::GPSTrack::TrackRecord *track = this->currTracks.GetItem(index);
		dt->SetInstant(track->records[track->nRecords - 1].recTime);
		return true;
	}
	else if (index == this->currTracks.GetCount() && this->currTimes.GetCount() > 0)
	{
		dt->SetTicks(this->currTimes.GetItem(this->currTimes.GetCount() - 1));
		return true;
	}
	else
	{
		return false;
	}
}

UOSInt Map::GPSTrack::GetTrackCnt()
{
	UOSInt cnt = this->currTracks.GetCount();
	if (this->currTimes.GetCount() > 0)
		cnt++;
	return cnt;
}

Map::GPSTrack::GPSRecord3 *Map::GPSTrack::GetTrack(UOSInt index, UOSInt *recordCnt)
{
	if (this->currTracks.GetCount() < index)
		return 0;
	if (this->currTracks.GetCount() == index)
	{
		if (this->currTimes.GetCount() == 0)
			return 0;
		UOSInt i = this->currTimes.GetCount();
		if (recordCnt)
			*recordCnt = i;
		if (this->tmpRecord)
		{
			return this->tmpRecord;
		}
		this->tmpRecord = MemAllocA(Map::GPSTrack::GPSRecord3, this->currTimes.GetCount());
		while (i-- > 0)
		{
			MemCopyNO(&this->tmpRecord[i], this->currRecs.GetItem(i), sizeof(Map::GPSTrack::GPSRecord3));
		}
		return this->tmpRecord;
	}
	else
	{
		Map::GPSTrack::TrackRecord *rec = this->currTracks.GetItem(index);
		if (recordCnt)
			*recordCnt = rec->nRecords;
		return rec->records;
	}
}

Math::Coord2DDbl Map::GPSTrack::GetPosByTime(const Data::Timestamp &ts)
{
	return GetPosByTicks(ts.ToTicks());
}

Math::Coord2DDbl Map::GPSTrack::GetPosByTime(Data::DateTime *dt)
{
	return GetPosByTicks(dt->ToTicks());
}

Math::Coord2DDbl Map::GPSTrack::GetPosByTicks(Int64 ticks)
{
	OSInt si;
	if (this->currTimes.GetCount() > 0)
	{
		if (ticks >= this->currTimes.GetItem(0) && ticks <= this->currTimes.GetItem(this->currTimes.GetCount() - 1))
		{
			si = this->currTimes.SortedIndexOf(ticks);
			if (si >= 0)
			{
				Map::GPSTrack::GPSRecord3 *rec = this->currRecs.GetItem((UOSInt)si);
				return rec->pos;
			}
			else
			{
				Int64 tDiff;
				Map::GPSTrack::GPSRecord3 *rec1 = this->currRecs.GetItem((UOSInt)~si - 1);
				Map::GPSTrack::GPSRecord3 *rec2 = this->currRecs.GetItem((UOSInt)~si);
				tDiff = rec2->recTime.DiffMS(rec1->recTime);
				return (rec1->pos * (Double)(rec2->recTime.ToTicks() - ticks) + rec2->pos * (Double)(ticks - rec1->recTime.ToTicks())) / (Double)tDiff;
			}
		}
	}
	if (this->currTracks.GetCount() > 0)
	{
		Map::GPSTrack::TrackRecord *rec;
		UOSInt i;
		UOSInt j;
		i = this->currTracks.GetCount();
		while (i-- > 0)
		{
			rec = this->currTracks.GetItem(i);
			if (ticks >= rec->records[0].recTime.ToTicks() && ticks <= rec->records[rec->nRecords - 1].recTime.ToTicks())
			{
				j = 0;
				while (j < rec->nRecords)
				{
					if (ticks <= rec->records[j].recTime.ToTicks())
						break;
					j++;
				}
				if (ticks == rec->records[j].recTime.ToTicks())
				{
					return rec->records[j].pos;
				}
				else
				{
					Int64 tDiff;
					tDiff = rec->records[j].recTime.DiffMS(rec->records[j - 1].recTime);
					return (rec->records[j - 1].pos * (Double)(rec->records[j].recTime.ToTicks() - ticks) + rec->records[j].pos * (Double)(ticks - rec->records[j - 1].recTime.ToTicks())) / (Double)tDiff;
				}
			}
		}
	}
	return Math::Coord2DDbl(0, 0);
}

void Map::GPSTrack::SetExtraParser(GPSExtraParser *parser)
{
	SDEL_CLASS(this->extraParser);
	this->extraParser = parser;
}

void Map::GPSTrack::SetExtraDataIndex(UOSInt recIndex, const UInt8 *data, UOSInt dataSize)
{
	Sync::MutexUsage mutUsage(this->recMut);
	if (recIndex < this->currExtraData.GetCount())
	{
		UInt8 *newData = MemAlloc(UInt8, dataSize);
		MemCopyNO(newData, data, dataSize);
		data = this->currExtraData.GetItem(recIndex);
		this->currExtraData.SetItem(recIndex, newData);
		this->currExtraSize.SetItem(recIndex, dataSize);
		if (data)
		{
			MemFree((void*)data);
		}
	}
	mutUsage.EndUse();
}

const UInt8 *Map::GPSTrack::GetExtraData(UOSInt trackIndex, UOSInt recIndex, UOSInt *dataSize)
{
	if (this->currTracks.GetCount() < trackIndex)
		return 0;
	if (this->currTracks.GetCount() == trackIndex)
	{
		if (this->currTimes.GetCount() <= recIndex)
			return 0;
		const UInt8 *data;
		Sync::MutexUsage mutUsage(this->recMut);
		*dataSize = this->currExtraSize.GetItem(recIndex);
		data = this->currExtraData.GetItem(recIndex);
		mutUsage.EndUse();
		return data;
	}
	else
	{
		Map::GPSTrack::TrackRecord *rec = this->currTracks.GetItem(trackIndex);
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

Bool Map::GPSTrack::GetExtraName(UOSInt trackIndex, UOSInt recIndex, UOSInt extIndex, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	if (this->extraParser == 0)
		return false;
	UOSInt dataSize;
	const UInt8 *data = this->GetExtraData(trackIndex, recIndex, &dataSize);
	if (data == 0)
		return false;
	return this->extraParser->GetExtraName(data, dataSize, extIndex, sb);
}

Bool Map::GPSTrack::GetExtraValueStr(UOSInt trackIndex, UOSInt recIndex, UOSInt extIndex, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	if (this->extraParser == 0)
		return false;
	UOSInt dataSize;
	const UInt8 *data = this->GetExtraData(trackIndex, recIndex, &dataSize);
	if (data == 0)
		return false;
	return this->extraParser->GetExtraValueStr(data, dataSize, extIndex, sb);
}

Map::GPSTrackReader::GPSTrackReader(NotNullPtr<Map::GPSTrack> gps) : Map::MapLayerReader(gps)
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
		return DB::DBReader::DateErrType::Error;
	if (colIndex == 2)
	{
		if (this->gps->GetTrackStartTime(id, outVal))
			return DB::DBReader::DateErrType::Ok;
	}
	else if (colIndex == 3)
	{
		if (this->gps->GetTrackEndTime(id, outVal))
			return DB::DBReader::DateErrType::Ok;
	}
	return DB::DBReader::DateErrType::Error;
}

Map::GPSDataReader::GPSDataReader(NotNullPtr<Map::GPSTrack> gps)
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
	Map::GPSTrack::GPSRecord3 *rec;
	this->currRow++;
	rowLeft = (UOSInt)this->currRow;
	i = 0;
	j = this->gps->GetTrackCnt();
	while (i < j)
	{
		rec = this->gps->GetTrack(i, &k);
		if (rec == 0)
		{
			return false;
		}
		else if (rowLeft < k)
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
		return 17;
	else
		return 16;
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
		return Double2Int32(this->currRec->pos.GetLat());
	}
	else if (colIndex == 3)
	{
		return Double2Int32(this->currRec->pos.GetLon());
	}
	else if (colIndex == 4)
	{
		return Double2Int32(this->currRec->speed);
	}
	else if (colIndex == 5)
	{
		return Double2Int32(this->currRec->heading);
	}
	else if (colIndex == 6)
	{
		return this->currRec->valid;
	}
	else if (colIndex == 7)
	{
		return this->currRec->nSateUsedGPS;
	}
	else if (colIndex == 8)
	{
		return this->currRec->nSateViewGPS;
	}
	else if (colIndex == 9)
	{
		return this->currRec->nSateUsed;
	}
	else if (colIndex == 10)
	{
		return this->currRec->nSateUsedSBAS;
	}
	else if (colIndex == 11)
	{
		return this->currRec->nSateUsedGLO;
	}
	else if (colIndex == 12)
	{
		return this->currRec->nSateViewGLO;
	}
	else if (colIndex == 13)
	{
		return this->currRec->nSateViewGA;
	}
	else if (colIndex == 14)
	{
		return this->currRec->nSateViewQZSS;
	}
	else if (colIndex == 15)
	{
		return this->currRec->nSateViewBD;
	}
	else if (colIndex == 16)
	{
		return Double2Int32(this->currRec->altitude);
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
		UTF8Char sbuff[32];
		GetTimestamp(0).ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
		return Text::StrUTF8_WChar(buff, sbuff, 0);
	}
	else if (colIndex == 1)
	{
		return buff;
	}
	else if (colIndex >= 2 && colIndex <= 16)
	{
		return Text::StrDouble(buff, this->GetDbl(colIndex));
	}
	return 0;
}

Bool Map::GPSDataReader::GetStr(UOSInt colIndex, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	if ((sptr = this->GetStr(colIndex, sbuff, sizeof(sbuff))) != 0)
	{
		sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
		return true;
	}
	return false;
}

Text::String *Map::GPSDataReader::GetNewStr(UOSInt colIndex)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	if ((sptr = this->GetStr(colIndex, sbuff, sizeof(sbuff))) != 0)
	{
		return Text::String::New(sbuff, (UOSInt)(sptr - sbuff)).Ptr();
	}
	return 0;
}

UTF8Char *Map::GPSDataReader::GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize)
{
	if (this->currRec == 0)
		return 0;
	if (colIndex == 0)
	{
		return GetTimestamp(0).ToString(buff, "yyyy-MM-dd HH:mm:ss.fff");
	}
	else if (colIndex == 1)
	{
		return buff;
	}
	else if (colIndex >= 2 && colIndex <= 16)
	{
		return Text::StrDouble(buff, this->GetDbl(colIndex));
	}
	return 0;
}

Data::Timestamp Map::GPSDataReader::GetTimestamp(UOSInt colIndex)
{
	if (this->currRec == 0)
		return Data::Timestamp(0);
	if (colIndex == 0)
	{
		return Data::Timestamp(this->currRec->recTime, Data::DateTimeUtil::GetLocalTzQhr());
	}
	return Data::Timestamp(0);
}

Double Map::GPSDataReader::GetDbl(UOSInt colIndex)
{
	if (this->currRec == 0)
		return 0;
	if (colIndex == 2)
	{
		return this->currRec->pos.GetLat();
	}
	else if (colIndex == 3)
	{
		return this->currRec->pos.GetLon();
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
		return this->currRec->nSateUsedGPS;
	}
	else if (colIndex == 8)
	{
		return this->currRec->nSateViewGPS;
	}
	else if (colIndex == 9)
	{
		return this->currRec->nSateUsed;
	}
	else if (colIndex == 10)
	{
		return this->currRec->nSateUsedSBAS;
	}
	else if (colIndex == 11)
	{
		return this->currRec->nSateUsedGLO;
	}
	else if (colIndex == 12)
	{
		return this->currRec->nSateViewGLO;
	}
	else if (colIndex == 13)
	{
		return this->currRec->nSateViewGA;
	}
	else if (colIndex == 14)
	{
		return this->currRec->nSateViewQZSS;
	}
	else if (colIndex == 15)
	{
		return this->currRec->nSateViewBD;
	}
	else if (colIndex == 16)
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

Math::Geometry::Vector2D *Map::GPSDataReader::GetVector(UOSInt colIndex)
{
	if (this->currRec == 0)
		return 0;
	if (colIndex != 1)
		return 0;
	Math::Geometry::Point *pt;
	if (this->gps->GetHasAltitude())
	{
		NEW_CLASS(pt, Math::Geometry::PointZ(4326, this->currRec->pos.x, this->currRec->pos.y, this->currRec->altitude));
	}
	else
	{
		NEW_CLASS(pt, Math::Geometry::Point(4326, this->currRec->pos.x, this->currRec->pos.y));
	}
	return pt;
}

Bool Map::GPSDataReader::GetUUID(UOSInt colIndex, Data::UUID *uuid)
{
	return false;
}

UTF8Char *Map::GPSDataReader::GetName(UOSInt colIndex, UTF8Char *buff)
{
	Text::CString cstr = GetName(colIndex, this->gps->GetHasAltitude());
	if (cstr.v)
		return cstr.ConcatTo(buff);
	return 0;
}

Bool Map::GPSDataReader::IsNull(UOSInt colIndex)
{
	if (this->currRec == 0)
		return true;
	if (colIndex > 16)
		return true;
	if (colIndex == 16 && !this->gps->GetHasAltitude())
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
		return DB::DBUtil::CT_Int32;
	case 10:
		return DB::DBUtil::CT_Int32;
	case 11:
		return DB::DBUtil::CT_Int32;
	case 12:
		return DB::DBUtil::CT_Int32;
	case 13:
		return DB::DBUtil::CT_Int32;
	case 14:
		return DB::DBUtil::CT_Int32;
	case 15:
		return DB::DBUtil::CT_Int32;
	case 16:
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

Bool Map::GPSDataReader::GetColDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef)
{
	return GetColDefV(colIndex, colDef, this->gps->GetHasAltitude());
}

Text::CString Map::GPSDataReader::GetName(UOSInt colIndex, Bool hasAltitude)
{
	switch (colIndex)
	{
	case 0:
		return CSTR("Time");
	case 1:
		return CSTR("Shape");
	case 2:
		return CSTR("Latitude");
	case 3:
		return CSTR("Longitude");
	case 4:
		return CSTR("Speed");
	case 5:
		return CSTR("Heading");
	case 6:
		return CSTR("GPSFix");
	case 7:
		return CSTR("nSateUsedGPS");
	case 8:
		return CSTR("nSateViewGPS");
	case 9:
		return CSTR("nSateUsed");
	case 10:
		return CSTR("nSateUsedSBAS");
	case 11:
		return CSTR("nSateUsedGLO");
	case 12:
		return CSTR("nSateViewGLO");
	case 13:
		return CSTR("nSateViewGA");
	case 14:
		return CSTR("nSateViewQZSS");
	case 15:
		return CSTR("nSateViewBD");
	case 16:
		if (hasAltitude)
		{
			return CSTR("Altitude");
		}
		else
		{
			return CSTR_NULL;
		}
	default:
		return CSTR_NULL;
	}
}

Bool Map::GPSDataReader::GetColDefV(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef, Bool hasAltitude)
{
	colDef->SetNotNull(true);
	colDef->SetPK(false);
	colDef->SetAutoIncNone();
	colDef->SetDefVal(CSTR_NULL);
	colDef->SetAttr(CSTR_NULL);
	switch (colIndex)
	{
	case 0:
		colDef->SetColName(GetName(colIndex, hasAltitude));
		colDef->SetColType(DB::DBUtil::CT_DateTime);
		colDef->SetColSize(20);
		colDef->SetColDP(0);
		colDef->SetPK(true);
		return true;
	case 1:
		colDef->SetColName(GetName(colIndex, hasAltitude));
		colDef->SetColType(DB::DBUtil::CT_Vector);
		colDef->SetColSize(0x7fffffff);
		colDef->SetColDP(0);
		colDef->SetPK(false);
		return true;
	case 2:
		colDef->SetColName(GetName(colIndex, hasAltitude));
		colDef->SetColType(DB::DBUtil::CT_Double);
		colDef->SetColSize(30);
		colDef->SetColDP(15);
		return true;
	case 3:
		colDef->SetColName(GetName(colIndex, hasAltitude));
		colDef->SetColType(DB::DBUtil::CT_Double);
		colDef->SetColSize(30);
		colDef->SetColDP(15);
		return true;
	case 4:
		colDef->SetColName(GetName(colIndex, hasAltitude));
		colDef->SetColType(DB::DBUtil::CT_Double);
		colDef->SetColSize(30);
		colDef->SetColDP(15);
		return true;
	case 5:
		colDef->SetColName(GetName(colIndex, hasAltitude));
		colDef->SetColType(DB::DBUtil::CT_Double);
		colDef->SetColSize(30);
		colDef->SetColDP(15);
		return true;
	case 6:
		colDef->SetColName(GetName(colIndex, hasAltitude));
		colDef->SetColType(DB::DBUtil::CT_Int32);
		colDef->SetColSize(11);
		colDef->SetColDP(0);
		return true;
	case 7:
		colDef->SetColName(GetName(colIndex, hasAltitude));
		colDef->SetColType(DB::DBUtil::CT_Int32);
		colDef->SetColSize(11);
		colDef->SetColDP(0);
		return true;
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
		colDef->SetColName(GetName(colIndex, hasAltitude));
		colDef->SetColType(DB::DBUtil::CT_Int32);
		colDef->SetColSize(11);
		colDef->SetColDP(0);
		return true;
	case 16:
		if (hasAltitude)
		{
			colDef->SetColName(GetName(colIndex, hasAltitude));
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
