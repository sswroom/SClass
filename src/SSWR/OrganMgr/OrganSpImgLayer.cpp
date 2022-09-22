#include "Stdafx.h"
#include "Math/Math.h"
#include "Math/Geometry/Point.h"
#include "SSWR/OrganMgr/OrganSpImgLayer.h"

SSWR::OrganMgr::OrganSpImgLayer::OrganSpImgLayer() : Map::IMapDrawLayer(CSTR("ImageLayer"), 0, CSTR_NULL)
{
	this->ClearItems();
}

SSWR::OrganMgr::OrganSpImgLayer::~OrganSpImgLayer()
{
}

Map::DrawLayerType SSWR::OrganMgr::OrganSpImgLayer::GetLayerType()
{
	return Map::DRAW_LAYER_POINT;
}

UOSInt SSWR::OrganMgr::OrganSpImgLayer::GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr)
{
	UOSInt i = 0;
	UOSInt j = this->objList.GetCount();
	while (i < j)
	{
		outArr->Add((Int64)i);
		i++;
	}
	return j;
}

UOSInt SSWR::OrganMgr::OrganSpImgLayer::GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	return GetObjectIdsMapXY(outArr, nameArr, rect.ToDouble() / mapRate, keepEmpty);
}

UOSInt SSWR::OrganMgr::OrganSpImgLayer::GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	UOSInt cnt = 0;
	UOSInt i;
	UOSInt j;
	UserFileInfo *ufile;
	i = 0;
	j = this->objList.GetCount();
	while (i < j)
	{
		ufile = this->objList.GetItem(i);
		if (rect.ContainPt(ufile->lon, ufile->lat))
		{
			outArr->Add((Int64)i);
			cnt++;
		}
		i++;
	}
	return cnt;
}

Int64 SSWR::OrganMgr::OrganSpImgLayer::GetObjectIdMax()
{
	return (Int64)this->objList.GetCount() - 1;
}

void SSWR::OrganMgr::OrganSpImgLayer::ReleaseNameArr(void *nameArr)
{
}

UTF8Char *SSWR::OrganMgr::OrganSpImgLayer::GetString(UTF8Char *buff, UOSInt buffSize, void *nameArr, Int64 id, UOSInt strIndex)
{
	UserFileInfo *ufile;
	ufile = this->objList.GetItem((UOSInt)id);
	if (ufile == 0)
		return 0;
	if (strIndex == 0)
	{
		if (ufile->descript)
		{
			return ufile->descript->ConcatToS(buff, buffSize);
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
		return Text::StrConcatC(buff, UTF8STRC("Descript"));
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
		return DB::DBUtil::CT_VarUTF8Char;
	}
	return DB::DBUtil::CT_Unknown;
}

Bool SSWR::OrganMgr::OrganSpImgLayer::GetColumnDef(UOSInt colIndex, DB::ColDef *colDef)
{
	if (colIndex == 0)
	{
		colDef->SetColName(CSTR("Descript"));
		colDef->SetColDP(0);
		colDef->SetColSize(256);
		colDef->SetColType(DB::DBUtil::CT_VarUTF8Char);
		return true;
	}
	return false;
}

UInt32 SSWR::OrganMgr::OrganSpImgLayer::GetCodePage()
{
	return 65001;
}

Bool SSWR::OrganMgr::OrganSpImgLayer::GetBounds(Math::RectAreaDbl *bounds)
{
	*bounds = Math::RectAreaDbl(this->min, this->max);
	return this->min.x != 0 || this->min.y != 0 || this->max.x != 0 || this->max.y != 0;
}

void *SSWR::OrganMgr::OrganSpImgLayer::BeginGetObject()
{
	return (void*)1;
}

void SSWR::OrganMgr::OrganSpImgLayer::EndGetObject(void *session)
{

}

Math::Geometry::Vector2D *SSWR::OrganMgr::OrganSpImgLayer::GetNewVectorById(void *session, Int64 id)
{
	UserFileInfo *ufile = this->objList.GetItem((UOSInt)id);
	if (ufile == 0)
		return 0;
	UInt32 srid = 0;
	if (this->csys) srid = this->csys->GetSRID();
	Math::Geometry::Point *pt;
	NEW_CLASS(pt, Math::Geometry::Point(0, ufile->lon, ufile->lat));
	return pt;
}

Map::IMapDrawLayer::ObjectClass SSWR::OrganMgr::OrganSpImgLayer::GetObjectClass()
{
	return Map::IMapDrawLayer::OC_OTHER;
}

void SSWR::OrganMgr::OrganSpImgLayer::ClearItems()
{
	this->min = Math::Coord2DDbl(0, 0);
	this->max = Math::Coord2DDbl(0, 0);
	this->objList.Clear();
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
					this->objList.Add(ufile);
					Math::Coord2DDbl pt = Math::Coord2DDbl(ufile->lon, ufile->lat);
					if (!found)
					{
						this->min = this->max = pt;
						found = true;
					}
					else
					{
						this->min = this->min.Min(pt);
						this->max = this->max.Max(pt);
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
			this->objList.Add(ufile);
			Math::Coord2DDbl pt = Math::Coord2DDbl(ufile->lon, ufile->lat);
			if (!found)
			{
				this->min = this->max = pt;
				found = true;
			}
			else
			{
				this->min = this->min.Min(pt);
				this->max = this->max.Max(pt);
			}
		}
		i++;
	}
}

void SSWR::OrganMgr::OrganSpImgLayer::AddItem(UserFileInfo *obj)
{
	if (obj->lat != 0 || obj->lon != 0)
	{
		Bool found = (this->objList.GetCount() > 0);
		this->objList.Add(obj);
		Math::Coord2DDbl pt = Math::Coord2DDbl(obj->lon, obj->lat);
		if (!found)
		{
			this->min = pt;
			this->max = pt;
			found = true;
		}
		else
		{
			this->min = this->min.Min(pt);
			this->max = this->max.Max(pt);
		}
	}
}
