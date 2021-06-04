#include "Stdafx.h"
#include "Math/Math.h"
#include "Math/Point.h"
#include "SSWR/OrganMgr/OrganTimeAdjLayer.h"

SSWR::OrganMgr::OrganTimeAdjLayer::OrganTimeAdjLayer(Map::GPSTrack *gpsTrk, Data::ArrayList<UserFileInfo *> *userFileList) : Map::IMapDrawLayer((const UTF8Char*)"ImageLayer", 0, 0)
{
	this->gpsTrk = gpsTrk;
	this->userFileList = userFileList;
	NEW_CLASS(this->cameraMap, Data::StringUTF8Map<Int32>());
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

UOSInt SSWR::OrganMgr::OrganTimeAdjLayer::GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Int32 x1, Int32 y1, Int32 x2, Int32 y2, Bool keepEmpty)
{
	return GetObjectIdsMapXY(outArr, nameArr, x1 / mapRate, y1 / mapRate, x2 / mapRate, y2 / mapRate, keepEmpty);
}

UOSInt SSWR::OrganMgr::OrganTimeAdjLayer::GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Double x1, Double y1, Double x2, Double y2, Bool keepEmpty)
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
		if (x1 <= ufile->lon && x2 >= ufile->lon && y1 <= ufile->lat && y2 >= ufile->lat)
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

Bool SSWR::OrganMgr::OrganTimeAdjLayer::GetBoundsDbl(Double *minX, Double *minY, Double *maxX, Double *maxY)
{
	return this->gpsTrk->GetBoundsDbl(minX, minY, maxX, maxY);
}

void *SSWR::OrganMgr::OrganTimeAdjLayer::BeginGetObject()
{
	return (void*)1;
}

void SSWR::OrganMgr::OrganTimeAdjLayer::EndGetObject(void *session)
{

}

Map::DrawObjectL *SSWR::OrganMgr::OrganTimeAdjLayer::GetObjectByIdD(void *session, Int64 id)
{
	UserFileInfo *ufile = this->userFileList->GetItem((UOSInt)id);
	if (ufile == 0)
		return 0;
	Map::DrawObjectL *dobj;

	dobj = MemAlloc(Map::DrawObjectL, 1);
	dobj->nPtOfst = 0;
	dobj->nPoint = 1;
	dobj->objId = id;
	dobj->ptOfstArr = 0;
	dobj->pointArr = MemAlloc(Double, 2);
	Data::DateTime dt;
	dt.SetTicks(ufile->fileTimeTicks);
	if (ufile->camera)
	{
		dt.AddSecond(this->cameraMap->Get(ufile->camera));
	}
	this->gpsTrk->GetLatLonByTime(&dt, &dobj->pointArr[1], &dobj->pointArr[0]);
	dobj->flags = 0;
	dobj->lineColor = 0;
	return dobj;
}

Math::Vector2D *SSWR::OrganMgr::OrganTimeAdjLayer::GetVectorById(void *session, Int64 id)
{
	UserFileInfo *ufile = this->userFileList->GetItem((UOSInt)id);
	if (ufile == 0)
		return 0;
	Math::Point *pt;
	Double lat;
	Double lon;
	Data::DateTime dt;
	dt.SetTicks(ufile->fileTimeTicks);
	if (ufile->camera)
	{
		dt.AddSecond(this->cameraMap->Get(ufile->camera));
	}
	this->gpsTrk->GetLatLonByTime(&dt, &lat, &lon);
	NEW_CLASS(pt, Math::Point(this->csys->GetSRID(), lon, lat));
	return pt;
}

void SSWR::OrganMgr::OrganTimeAdjLayer::ReleaseObject(void *session, Map::DrawObjectL *obj)
{
	MemFree(obj->pointArr);
	MemFree(obj);
}

Map::IMapDrawLayer::ObjectClass SSWR::OrganMgr::OrganTimeAdjLayer::GetObjectClass()
{
	return Map::IMapDrawLayer::OC_OTHER;
}

void SSWR::OrganMgr::OrganTimeAdjLayer::SetTimeAdj(const UTF8Char *camera, Int32 timeAdj)
{
	this->cameraMap->Put(camera, timeAdj);
}
