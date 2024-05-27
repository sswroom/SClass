#include "Stdafx.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math.h"
#include "Math/Geometry/Point.h"
#include "SSWR/OrganMgr/OrganTimeAdjLayer.h"

SSWR::OrganMgr::OrganTimeAdjLayer::OrganTimeAdjLayer(NN<Map::GPSTrack> gpsTrk, NN<Data::ArrayListNN<UserFileInfo>> userFileList) : Map::MapDrawLayer(CSTR("ImageLayer"), 0, CSTR_NULL, Math::CoordinateSystemManager::CreateWGS84Csys())
{
	this->gpsTrk = gpsTrk;
	this->userFileList = userFileList;
}

SSWR::OrganMgr::OrganTimeAdjLayer::~OrganTimeAdjLayer()
{
}

Map::DrawLayerType SSWR::OrganMgr::OrganTimeAdjLayer::GetLayerType() const
{
	return Map::DRAW_LAYER_POINT;
}

UOSInt SSWR::OrganMgr::OrganTimeAdjLayer::GetAllObjectIds(NN<Data::ArrayListInt64> outArr, Map::NameArray **nameArr)
{
	UOSInt i = 0;
	UOSInt j = this->userFileList->GetCount();
	while (i < j)
	{
		outArr->Add((Int64)i);
		i++;
	}
	return j;
}

UOSInt SSWR::OrganMgr::OrganTimeAdjLayer::GetObjectIds(NN<Data::ArrayListInt64> outArr, Map::NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	return GetObjectIdsMapXY(outArr, nameArr, rect.ToDouble() / mapRate, keepEmpty);
}

UOSInt SSWR::OrganMgr::OrganTimeAdjLayer::GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, Map::NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	UOSInt cnt = 0;
	UOSInt i;
	UOSInt j;
	NN<UserFileInfo> ufile;
	i = 0;
	j = this->userFileList->GetCount();
	while (i < j)
	{
		ufile = this->userFileList->GetItemNoCheck(i);
		if (rect.ContainPt(ufile->lon, ufile->lat))
		{
			outArr->Add((Int64)i);
			cnt++;
		}
		i++;
	}
	return cnt;
}

Int64 SSWR::OrganMgr::OrganTimeAdjLayer::GetObjectIdMax() const
{
	return (Int64)userFileList->GetCount() - 1;
}

void SSWR::OrganMgr::OrganTimeAdjLayer::ReleaseNameArr(Map::NameArray *nameArr)
{
}

Bool SSWR::OrganMgr::OrganTimeAdjLayer::GetString(NN<Text::StringBuilderUTF8> sb, Map::NameArray *nameArr, Int64 id, UOSInt strIndex)
{
	return false;
}

UOSInt SSWR::OrganMgr::OrganTimeAdjLayer::GetColumnCnt() const
{
	return 0;
}

UTF8Char *SSWR::OrganMgr::OrganTimeAdjLayer::GetColumnName(UTF8Char *buff, UOSInt colIndex)
{
	return 0;
}

DB::DBUtil::ColType SSWR::OrganMgr::OrganTimeAdjLayer::GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize)
{
	return DB::DBUtil::CT_Unknown;
}

Bool SSWR::OrganMgr::OrganTimeAdjLayer::GetColumnDef(UOSInt colIndex, NN<DB::ColDef> colDef)
{
	return false;
}

UInt32 SSWR::OrganMgr::OrganTimeAdjLayer::GetCodePage() const
{
	return 0;
}

Bool SSWR::OrganMgr::OrganTimeAdjLayer::GetBounds(OutParam<Math::RectAreaDbl> bounds) const
{
	return this->gpsTrk->GetBounds(bounds);
}

Map::GetObjectSess *SSWR::OrganMgr::OrganTimeAdjLayer::BeginGetObject()
{
	return (Map::GetObjectSess*)1;
}

void SSWR::OrganMgr::OrganTimeAdjLayer::EndGetObject(Map::GetObjectSess *session)
{

}

Math::Geometry::Vector2D *SSWR::OrganMgr::OrganTimeAdjLayer::GetNewVectorById(Map::GetObjectSess *session, Int64 id)
{
	NN<UserFileInfo> ufile;
	if (!this->userFileList->GetItem((UOSInt)id).SetTo(ufile))
		return 0;
	Math::Geometry::Point *pt;
	NN<Text::String> s;
	if (ufile->locType != LocType::PhotoExif && ufile->locType != LocType::UserInput)
	{
		Math::Coord2DDbl pos;
		Data::Timestamp ts = ufile->fileTime;
		if (ufile->camera.SetTo(s))
		{
			ts = ts.AddSecond(this->cameraMap.GetNN(s));
		}
		UInt32 srid = this->csys->GetSRID();
		pos = this->gpsTrk->GetPosByTime(ts);
		NEW_CLASS(pt, Math::Geometry::Point(srid, pos));
		return pt;
	}
	else
	{
		NEW_CLASS(pt, Math::Geometry::Point(this->csys->GetSRID(), Math::Coord2DDbl(ufile->lon, ufile->lat)));
		return pt;
	}
}

Map::MapDrawLayer::ObjectClass SSWR::OrganMgr::OrganTimeAdjLayer::GetObjectClass() const
{
	return Map::MapDrawLayer::OC_OTHER;
}

void SSWR::OrganMgr::OrganTimeAdjLayer::SetTimeAdj(Text::String *camera, Int32 timeAdj)
{
	this->cameraMap.Put(camera, timeAdj);
}

void SSWR::OrganMgr::OrganTimeAdjLayer::SetTimeAdj(Text::CString camera, Int32 timeAdj)
{
	this->cameraMap.Put(camera, timeAdj);
}
