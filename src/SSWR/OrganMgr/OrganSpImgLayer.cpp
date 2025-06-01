#include "Stdafx.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math.h"
#include "Math/Geometry/Point.h"
#include "SSWR/OrganMgr/OrganSpImgLayer.h"

SSWR::OrganMgr::OrganSpImgLayer::OrganSpImgLayer() : Map::MapDrawLayer(CSTR("ImageLayer"), 0, CSTR_NULL, Math::CoordinateSystemManager::CreateWGS84Csys())
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

UOSInt SSWR::OrganMgr::OrganSpImgLayer::GetAllObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<Map::NameArray>> nameArr)
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

UOSInt SSWR::OrganMgr::OrganSpImgLayer::GetObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<Map::NameArray>> nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	return GetObjectIdsMapXY(outArr, nameArr, rect.ToDouble() / mapRate, keepEmpty);
}

UOSInt SSWR::OrganMgr::OrganSpImgLayer::GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, OptOut<Optional<Map::NameArray>> nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	UOSInt cnt = 0;
	UOSInt i;
	UOSInt j;
	NN<UserFileInfo> ufile;
	i = 0;
	j = this->objList.GetCount();
	while (i < j)
	{
		ufile = this->objList.GetItemNoCheck(i);
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

void SSWR::OrganMgr::OrganSpImgLayer::ReleaseNameArr(Optional<Map::NameArray> nameArr)
{
}

Bool SSWR::OrganMgr::OrganSpImgLayer::GetString(NN<Text::StringBuilderUTF8> sb, Optional<Map::NameArray> nameArr, Int64 id, UOSInt strIndex)
{
	NN<UserFileInfo> ufile;
	NN<Text::String> s;
	if (!this->objList.GetItem((UOSInt)id).SetTo(ufile))
		return false;
	if (strIndex == 0)
	{
		if (ufile->descript.SetTo(s))
		{
			sb->Append(s);
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

UOSInt SSWR::OrganMgr::OrganSpImgLayer::GetColumnCnt() const
{
	return 1;
}

UnsafeArrayOpt<UTF8Char> SSWR::OrganMgr::OrganSpImgLayer::GetColumnName(UnsafeArray<UTF8Char> buff, UOSInt colIndex)
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

Bool SSWR::OrganMgr::OrganSpImgLayer::GetColumnDef(UOSInt colIndex, NN<DB::ColDef> colDef)
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

NN<Map::GetObjectSess> SSWR::OrganMgr::OrganSpImgLayer::BeginGetObject()
{
	return NN<Map::GetObjectSess>::ConvertFrom(NN<OrganSpImgLayer>(*this));
}

void SSWR::OrganMgr::OrganSpImgLayer::EndGetObject(NN<Map::GetObjectSess> session)
{

}

Optional<Math::Geometry::Vector2D> SSWR::OrganMgr::OrganSpImgLayer::GetNewVectorById(NN<Map::GetObjectSess> session, Int64 id)
{
	NN<UserFileInfo> ufile;
	if (!this->objList.GetItem((UOSInt)id).SetTo(ufile))
		return 0;
	UInt32 srid = this->csys->GetSRID();
	Math::Geometry::Point *pt;
	NEW_CLASS(pt, Math::Geometry::Point(srid, ufile->lon, ufile->lat));
	return pt;
}

UOSInt SSWR::OrganMgr::OrganSpImgLayer::GetGeomCol() const
{
	return INVALID_INDEX;
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

void SSWR::OrganMgr::OrganSpImgLayer::AddItems(NN<Data::ArrayListNN<OrganImageItem>> objList)
{
	UOSInt i;
	UOSInt j;
	NN<OrganImageItem> imgItem;
	NN<UserFileInfo> ufile;
	Bool found = false;
	i = 0;
	j = objList->GetCount();
	found = (j > 0);
	while (i < j)
	{
		imgItem = objList->GetItemNoCheck(i);
		if (imgItem->GetFileType() == OrganImageItem::FileType::UserFile)
		{
			if (imgItem->GetUserFile().SetTo(ufile))
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

void SSWR::OrganMgr::OrganSpImgLayer::AddItems(NN<Data::ArrayListNN<UserFileInfo>> objList)
{
	UOSInt i;
	UOSInt j;
	NN<UserFileInfo> ufile;
	Bool found = false;
	i = 0;
	j = objList->GetCount();
	found = (j > 0);
	while (i < j)
	{
		ufile = objList->GetItemNoCheck(i);
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

void SSWR::OrganMgr::OrganSpImgLayer::AddItem(NN<UserFileInfo> obj)
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
