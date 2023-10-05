#include "Stdafx.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math.h"
#include "Math/Geometry/Point.h"
#include "SSWR/OrganMgr/OrganSpImgLayer.h"

SSWR::OrganMgr::OrganSpImgLayer::OrganSpImgLayer() : Map::MapDrawLayer(CSTR("ImageLayer"), 0, CSTR_NULL, Math::CoordinateSystemManager::CreateDefaultCsys())
{
	this->ClearItems();
}

SSWR::OrganMgr::OrganSpImgLayer::~OrganSpImgLayer()
{
}

Map::DrawLayerType SSWR::OrganMgr::OrganSpImgLayer::GetLayerType() const
{
	return Map::DRAW_LAYER_POINT;
}

UOSInt SSWR::OrganMgr::OrganSpImgLayer::GetAllObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, Map::NameArray **nameArr)
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

UOSInt SSWR::OrganMgr::OrganSpImgLayer::GetObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, Map::NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	return GetObjectIdsMapXY(outArr, nameArr, rect.ToDouble() / mapRate, keepEmpty);
}

UOSInt SSWR::OrganMgr::OrganSpImgLayer::GetObjectIdsMapXY(NotNullPtr<Data::ArrayListInt64> outArr, Map::NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
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

Int64 SSWR::OrganMgr::OrganSpImgLayer::GetObjectIdMax() const
{
	return (Int64)this->objList.GetCount() - 1;
}

void SSWR::OrganMgr::OrganSpImgLayer::ReleaseNameArr(Map::NameArray *nameArr)
{
}

UTF8Char *SSWR::OrganMgr::OrganSpImgLayer::GetString(UTF8Char *buff, UOSInt buffSize, Map::NameArray *nameArr, Int64 id, UOSInt strIndex)
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

UOSInt SSWR::OrganMgr::OrganSpImgLayer::GetColumnCnt() const
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

DB::DBUtil::ColType SSWR::OrganMgr::OrganSpImgLayer::GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize)
{
	if (colIndex == 0)
	{
		colSize.Set(256);
		return DB::DBUtil::CT_VarUTF8Char;
	}
	return DB::DBUtil::CT_Unknown;
}

Bool SSWR::OrganMgr::OrganSpImgLayer::GetColumnDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef)
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

UInt32 SSWR::OrganMgr::OrganSpImgLayer::GetCodePage() const
{
	return 65001;
}

Bool SSWR::OrganMgr::OrganSpImgLayer::GetBounds(OutParam<Math::RectAreaDbl> bounds) const
{
	bounds.Set(Math::RectAreaDbl(this->min, this->max));
	return this->min.x != 0 || this->min.y != 0 || this->max.x != 0 || this->max.y != 0;
}

Map::GetObjectSess *SSWR::OrganMgr::OrganSpImgLayer::BeginGetObject()
{
	return (Map::GetObjectSess*)1;
}

void SSWR::OrganMgr::OrganSpImgLayer::EndGetObject(Map::GetObjectSess *session)
{

}

Math::Geometry::Vector2D *SSWR::OrganMgr::OrganSpImgLayer::GetNewVectorById(Map::GetObjectSess *session, Int64 id)
{
	UserFileInfo *ufile = this->objList.GetItem((UOSInt)id);
	if (ufile == 0)
		return 0;
	UInt32 srid = this->csys->GetSRID();
	Math::Geometry::Point *pt;
	NEW_CLASS(pt, Math::Geometry::Point(srid, ufile->lon, ufile->lat));
	return pt;
}

Map::MapDrawLayer::ObjectClass SSWR::OrganMgr::OrganSpImgLayer::GetObjectClass() const
{
	return Map::MapDrawLayer::OC_OTHER;
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
		if (imgItem->GetFileType() == OrganImageItem::FileType::UserFile)
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
