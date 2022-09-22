#include "Stdafx.h"
#include "Math/Math.h"
#include "Math/Geometry/Point.h"
#include "SSWR/OrganMgr/OrganTimeAdjLayer.h"

SSWR::OrganMgr::OrganTimeAdjLayer::OrganTimeAdjLayer(Map::GPSTrack *gpsTrk, Data::ArrayList<UserFileInfo *> *userFileList) : Map::IMapDrawLayer(CSTR("ImageLayer"), 0, CSTR_NULL)
{
	this->gpsTrk = gpsTrk;
	this->userFileList = userFileList;
	NEW_CLASS(this->cameraMap, Data::StringMap<Int32>());
}

SSWR::OrganMgr::OrganTimeAdjLayer::~OrganTimeAdjLayer()
{
	DEL_CLASS(this->cameraMap);
}

Map::DrawLayerType SSWR::OrganMgr::OrganTimeAdjLayer::GetLayerType()
{
	return Map::DRAW_LAYER_POINT;
}

UOSInt SSWR::OrganMgr::OrganTimeAdjLayer::GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr)
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

UOSInt SSWR::OrganMgr::OrganTimeAdjLayer::GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	return GetObjectIdsMapXY(outArr, nameArr, rect.ToDouble() / mapRate, keepEmpty);
}

UOSInt SSWR::OrganMgr::OrganTimeAdjLayer::GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
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

Int64 SSWR::OrganMgr::OrganTimeAdjLayer::GetObjectIdMax()
{
	return (Int64)userFileList->GetCount() - 1;
}

void SSWR::OrganMgr::OrganTimeAdjLayer::ReleaseNameArr(void *nameArr)
{
}

UTF8Char *SSWR::OrganMgr::OrganTimeAdjLayer::GetString(UTF8Char *buff, UOSInt buffSize, void *nameArr, Int64 id, UOSInt strIndex)
{
	return 0;
}

UOSInt SSWR::OrganMgr::OrganTimeAdjLayer::GetColumnCnt()
{
	return 0;
}

UTF8Char *SSWR::OrganMgr::OrganTimeAdjLayer::GetColumnName(UTF8Char *buff, UOSInt colIndex)
{
	return 0;
}

DB::DBUtil::ColType SSWR::OrganMgr::OrganTimeAdjLayer::GetColumnType(UOSInt colIndex, UOSInt *colSize)
{
	return DB::DBUtil::CT_Unknown;
}

Bool SSWR::OrganMgr::OrganTimeAdjLayer::GetColumnDef(UOSInt colIndex, DB::ColDef *colDef)
{
	return false;
}

UInt32 SSWR::OrganMgr::OrganTimeAdjLayer::GetCodePage()
{
	return 0;
}

Bool SSWR::OrganMgr::OrganTimeAdjLayer::GetBounds(Math::RectAreaDbl *bounds)
{
	return this->gpsTrk->GetBounds(bounds);
}

void *SSWR::OrganMgr::OrganTimeAdjLayer::BeginGetObject()
{
	return (void*)1;
}

void SSWR::OrganMgr::OrganTimeAdjLayer::EndGetObject(void *session)
{

}

Math::Geometry::Vector2D *SSWR::OrganMgr::OrganTimeAdjLayer::GetNewVectorById(void *session, Int64 id)
{
	UserFileInfo *ufile = this->userFileList->GetItem((UOSInt)id);
	if (ufile == 0)
		return 0;
	Math::Geometry::Point *pt;
	Math::Coord2DDbl pos;
	Data::DateTime dt;
	dt.SetTicks(ufile->fileTimeTicks);
	if (ufile->camera)
	{
		dt.AddSecond(this->cameraMap->Get(ufile->camera));
	}
	UInt32 srid = 0;
	if (this->csys) srid = this->csys->GetSRID();
	this->gpsTrk->GetPosByTime(&dt, &pos);
	NEW_CLASS(pt, Math::Geometry::Point(srid, pos));
	return pt;
}

Map::IMapDrawLayer::ObjectClass SSWR::OrganMgr::OrganTimeAdjLayer::GetObjectClass()
{
	return Map::IMapDrawLayer::OC_OTHER;
}

void SSWR::OrganMgr::OrganTimeAdjLayer::SetTimeAdj(Text::String *camera, Int32 timeAdj)
{
	this->cameraMap->Put(camera, timeAdj);
}

void SSWR::OrganMgr::OrganTimeAdjLayer::SetTimeAdj(Text::CString camera, Int32 timeAdj)
{
	this->cameraMap->Put(camera, timeAdj);
}
