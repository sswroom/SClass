#include "Stdafx.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math.h"
#include "Math/Geometry/Point.h"
#include "SSWR/OrganMgr/OrganTimeAdjLayer.h"

SSWR::OrganMgr::OrganTimeAdjLayer::OrganTimeAdjLayer(Map::GPSTrack *gpsTrk, Data::ArrayList<UserFileInfo *> *userFileList) : Map::MapDrawLayer(CSTR("ImageLayer"), 0, CSTR_NULL, Math::CoordinateSystemManager::CreateDefaultCsys())
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

UOSInt SSWR::OrganMgr::OrganTimeAdjLayer::GetAllObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, Map::NameArray **nameArr)
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

UOSInt SSWR::OrganMgr::OrganTimeAdjLayer::GetObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, Map::NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	return GetObjectIdsMapXY(outArr, nameArr, rect.ToDouble() / mapRate, keepEmpty);
}

UOSInt SSWR::OrganMgr::OrganTimeAdjLayer::GetObjectIdsMapXY(NotNullPtr<Data::ArrayListInt64> outArr, Map::NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	UOSInt cnt = 0;
	UOSInt i;
	UOSInt j;
	UserFileInfo *ufile;
	i = 0;
	j = this->userFileList->GetCount();
	while (i < j)
	{
		ufile = this->userFileList->GetItem(i);
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

UTF8Char *SSWR::OrganMgr::OrganTimeAdjLayer::GetString(UTF8Char *buff, UOSInt buffSize, Map::NameArray *nameArr, Int64 id, UOSInt strIndex)
{
	return 0;
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

Bool SSWR::OrganMgr::OrganTimeAdjLayer::GetColumnDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef)
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
	UserFileInfo *ufile = this->userFileList->GetItem((UOSInt)id);
	if (ufile == 0)
		return 0;
	Math::Geometry::Point *pt;
	Math::Coord2DDbl pos;
	Data::Timestamp ts = ufile->fileTime;
	if (ufile->camera)
	{
		ts = ts.AddSecond(this->cameraMap.Get(ufile->camera));
	}
	UInt32 srid = this->csys->GetSRID();
	pos = this->gpsTrk->GetPosByTime(ts);
	NEW_CLASS(pt, Math::Geometry::Point(srid, pos));
	return pt;
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
