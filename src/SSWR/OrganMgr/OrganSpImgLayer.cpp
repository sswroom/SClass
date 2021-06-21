#include "Stdafx.h"
#include "Math/Math.h"
#include "Math/Point.h"
#include "SSWR/OrganMgr/OrganSpImgLayer.h"

SSWR::OrganMgr::OrganSpImgLayer::OrganSpImgLayer() : Map::IMapDrawLayer((const UTF8Char*)"ImageLayer", 0, 0)
{
	NEW_CLASS(this->objList, Data::ArrayList<UserFileInfo*>());
	this->ClearItems();
}

SSWR::OrganMgr::OrganSpImgLayer::~OrganSpImgLayer()
{
	DEL_CLASS(this->objList);
}

Map::DrawLayerType SSWR::OrganMgr::OrganSpImgLayer::GetLayerType()
{
	return Map::DRAW_LAYER_POINT;
}

UOSInt SSWR::OrganMgr::OrganSpImgLayer::GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr)
{
	UOSInt i = 0;
	UOSInt j = this->objList->GetCount();
	while (i < j)
	{
		outArr->Add((Int64)i);
		i++;
	}
	return j;
}

UOSInt SSWR::OrganMgr::OrganSpImgLayer::GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Int32 x1, Int32 y1, Int32 x2, Int32 y2, Bool keepEmpty)
{
	return GetObjectIdsMapXY(outArr, nameArr, x1 / mapRate, y1 / mapRate, x2 / mapRate, y2 / mapRate, keepEmpty);
}

UOSInt SSWR::OrganMgr::OrganSpImgLayer::GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Double x1, Double y1, Double x2, Double y2, Bool keepEmpty)
{
	UOSInt cnt = 0;
	UOSInt i;
	UOSInt j;
	UserFileInfo *ufile;
	i = 0;
	j = this->objList->GetCount();
	while (i < j)
	{
		ufile = this->objList->GetItem(i);
		if (x1 <= ufile->lon && x2 >= ufile->lon && y1 <= ufile->lat && y2 >= ufile->lat)
		{
			outArr->Add(i);
			cnt++;
		}
		i++;
	}
	return cnt;
}

Int64 SSWR::OrganMgr::OrganSpImgLayer::GetObjectIdMax()
{
	return (Int64)objList->GetCount() - 1;
}

void SSWR::OrganMgr::OrganSpImgLayer::ReleaseNameArr(void *nameArr)
{
}

UTF8Char *SSWR::OrganMgr::OrganSpImgLayer::GetString(UTF8Char *buff, UOSInt buffSize, void *nameArr, Int64 id, UOSInt strIndex)
{
	UserFileInfo *ufile;
	ufile = this->objList->GetItem((UOSInt)id);
	if (ufile == 0)
		return 0;
	if (strIndex == 0)
	{
		if (ufile->descript)
		{
			return Text::StrConcatS(buff, ufile->descript, buffSize);
		}
		else
		{
			buff[0] = 0;
			return 0;
		}
	}
	buff[0] = 0;
	return 0;
}

UOSInt SSWR::OrganMgr::OrganSpImgLayer::GetColumnCnt()
{
	return 1;
}

UTF8Char *SSWR::OrganMgr::OrganSpImgLayer::GetColumnName(UTF8Char *buff, UOSInt colIndex)
{
	if (colIndex == 0)
	{
		return Text::StrConcat(buff, (const UTF8Char*)"Descript");
	}
	return 0;
}

DB::DBUtil::ColType SSWR::OrganMgr::OrganSpImgLayer::GetColumnType(UOSInt colIndex, UOSInt *colSize)
{
	if (colIndex == 0)
	{
		if (colSize)
		{
			*colSize = 256;
		}
		return DB::DBUtil::CT_VarChar;
	}
	return DB::DBUtil::CT_Unknown;
}

Bool SSWR::OrganMgr::OrganSpImgLayer::GetColumnDef(UOSInt colIndex, DB::ColDef *colDef)
{
	if (colIndex == 0)
	{
		colDef->SetColName((const UTF8Char*)"Descript");
		colDef->SetColDP(0);
		colDef->SetColSize(256);
		colDef->SetColType(DB::DBUtil::CT_VarChar);
		return true;
	}
	return false;
}

UInt32 SSWR::OrganMgr::OrganSpImgLayer::GetCodePage()
{
	return 65001;
}

Bool SSWR::OrganMgr::OrganSpImgLayer::GetBoundsDbl(Double *minX, Double *minY, Double *maxX, Double *maxY)
{
	*minX = this->minX;
	*minY = this->minY;
	*maxX = this->maxX;
	*maxY = this->maxY;
	return this->minX != 0 || this->minY != 0 || this->maxX != 0 || this->maxY != 0;
}

void *SSWR::OrganMgr::OrganSpImgLayer::BeginGetObject()
{
	return (void*)1;
}

void SSWR::OrganMgr::OrganSpImgLayer::EndGetObject(void *session)
{

}

Map::DrawObjectL *SSWR::OrganMgr::OrganSpImgLayer::GetObjectByIdD(void *session, Int64 id)
{
	UserFileInfo *ufile = this->objList->GetItem((UOSInt)id);
	if (ufile == 0)
		return 0;
	Map::DrawObjectL *dobj;

	dobj = MemAlloc(Map::DrawObjectL, 1);
	dobj->nPtOfst = 0;
	dobj->nPoint = 1;
	dobj->objId = id;
	dobj->ptOfstArr = 0;
	dobj->pointArr = MemAlloc(Double, 2);
	dobj->pointArr[0] = ufile->lon;
	dobj->pointArr[1] = ufile->lat;
	dobj->flags = 0;
	dobj->lineColor = 0;
	return dobj;
}

Math::Vector2D *SSWR::OrganMgr::OrganSpImgLayer::GetVectorById(void *session, Int64 id)
{
	UserFileInfo *ufile = this->objList->GetItem((UOSInt)id);
	if (ufile == 0)
		return 0;
	Math::Point *pt;
	NEW_CLASS(pt, Math::Point(this->csys->GetSRID(), ufile->lon, ufile->lat));
	return pt;
}

void SSWR::OrganMgr::OrganSpImgLayer::ReleaseObject(void *session, Map::DrawObjectL *obj)
{
	MemFree(obj->pointArr);
	MemFree(obj);
}

Map::IMapDrawLayer::ObjectClass SSWR::OrganMgr::OrganSpImgLayer::GetObjectClass()
{
	return Map::IMapDrawLayer::OC_OTHER;
}

void SSWR::OrganMgr::OrganSpImgLayer::ClearItems()
{
	this->minX = 0;
	this->minY = 0;
	this->maxX = 0;
	this->maxY = 0;
	this->objList->Clear();
}

void SSWR::OrganMgr::OrganSpImgLayer::AddItems(Data::ArrayList<OrganImageItem*> *objList)
{
	UOSInt i;
	UOSInt j;
	OrganImageItem *imgItem;
	UserFileInfo *ufile;
	Bool found = false;
	i = 0;
	j = objList->GetCount();
	found = (j > 0);
	while (i < j)
	{
		imgItem = objList->GetItem(i);
		if (imgItem->GetFileType() == OrganImageItem::FT_USERFILE)
		{
			ufile = imgItem->GetUserFile();
			if (ufile)
			{
				if (ufile->lat != 0 || ufile->lon != 0)
				{
					this->objList->Add(ufile);
					if (!found)
					{
						this->minX = ufile->lon;
						this->maxX = ufile->lon;
						this->minY = ufile->lat;
						this->maxY = ufile->lat;
						found = true;
					}
					else
					{
						if (this->minX > ufile->lon)
							this->minX = ufile->lon;
						if (this->maxX < ufile->lon)
							this->maxX = ufile->lon;
						if (this->minY > ufile->lat)
							this->minY = ufile->lat;
						if (this->maxY < ufile->lat)
							this->maxY = ufile->lat;
					}
				}
			}
		}
		i++;
	}
}

void SSWR::OrganMgr::OrganSpImgLayer::AddItems(Data::ArrayList<UserFileInfo*> *objList)
{
	UOSInt i;
	UOSInt j;
	UserFileInfo *ufile;
	Bool found = false;
	i = 0;
	j = objList->GetCount();
	found = (j > 0);
	while (i < j)
	{
		ufile = objList->GetItem(i);
		if (ufile->lat != 0 || ufile->lon != 0)
		{
			this->objList->Add(ufile);
			if (!found)
			{
				this->minX = ufile->lon;
				this->maxX = ufile->lon;
				this->minY = ufile->lat;
				this->maxY = ufile->lat;
				found = true;
			}
			else
			{
				if (this->minX > ufile->lon)
					this->minX = ufile->lon;
				if (this->maxX < ufile->lon)
					this->maxX = ufile->lon;
				if (this->minY > ufile->lat)
					this->minY = ufile->lat;
				if (this->maxY < ufile->lat)
					this->maxY = ufile->lat;
			}
		}
		i++;
	}
}

void SSWR::OrganMgr::OrganSpImgLayer::AddItem(UserFileInfo *obj)
{
	if (obj->lat != 0 || obj->lon != 0)
	{
		Bool found = (this->objList->GetCount() > 0);
		this->objList->Add(obj);
		if (!found)
		{
			this->minX = obj->lon;
			this->maxX = obj->lon;
			this->minY = obj->lat;
			this->maxY = obj->lat;
			found = true;
		}
		else
		{
			if (this->minX > obj->lon)
				this->minX = obj->lon;
			if (this->maxX < obj->lon)
				this->maxX = obj->lon;
			if (this->minY > obj->lat)
				this->minY = obj->lat;
			if (this->maxY < obj->lat)
				this->maxY = obj->lat;
		}
	}
}
