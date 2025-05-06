#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListInt32.h"
#include "Data/ArrayListICaseString.h"
#include "Data/Sort/ArtificialQuickSortFunc.h"
#include "DB/ColDef.h"
#include "DB/TableDef.h"
#include "Math/Math.h"
#include "Math/Geometry/Point.h"
#include "Map/CIPLayer2.h"
#include "Map/MapDrawLayer.h"
#include "Map/ScaledMapView.h"
#include "Map/SPDLayer.h"
#include "Map/VectorLayer.h"
#include "Math/WKTWriter.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"

OSInt __stdcall Map::MapDrawLayer::ObjectCompare(NN<ObjectInfo> obj1, NN<ObjectInfo> obj2)
{
	if (obj1->objDist > obj2->objDist)
	{
		return 1;
	}
	else if (obj1->objDist < obj2->objDist)
	{
		return -1;
	}
	else if (obj1->objId > obj2->objId)
	{
		return 1;
	}
	else if (obj1->objId < obj2->objId)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

Map::MapDrawLayer::MapDrawLayer(NN<Text::String> sourceName, UOSInt nameCol, Optional<Text::String> layerName, NN<Math::CoordinateSystem> csys) : DB::ReadingDB(sourceName)//IO::ParsedObject(sourceName)
{
	this->nameCol = nameCol;
	this->layerName = Text::String::CopyOrNull(layerName);
	this->csys = csys;

	this->pgColor = 0;
	this->lineColor = 0;
	this->lineWidth = 0;
	this->iconImg = 0;
	this->iconSpotX = 0;
	this->iconSpotY = 0;
	this->flags = 0;
}

Map::MapDrawLayer::MapDrawLayer(Text::CStringNN sourceName, UOSInt nameCol, Text::CString layerName, NN<Math::CoordinateSystem> csys) : DB::ReadingDB(sourceName)//IO::ParsedObject(sourceName)
{
	this->nameCol = nameCol;
	this->layerName = Text::String::NewOrNull(layerName);
	this->csys = csys;

	this->pgColor = 0;
	this->lineColor = 0;
	this->lineWidth = 0;
	this->iconImg = 0;
	this->iconSpotX = 0;
	this->iconSpotY = 0;
	this->flags = 0;
}

Map::MapDrawLayer::~MapDrawLayer()
{
	this->csys.Delete();
	this->iconImg.Delete();
	OPTSTR_DEL(this->layerName);
}

void Map::MapDrawLayer::SetCurrScale(Double scale)
{
}

void Map::MapDrawLayer::SetCurrTimeTS(Int64 timeStamp)
{
}

Int64 Map::MapDrawLayer::GetTimeStartTS() const
{
	return 0;
}

Int64 Map::MapDrawLayer::GetTimeEndTS() const
{
	return 0;
}

NN<Map::MapView> Map::MapDrawLayer::CreateMapView(Math::Size2DDbl scnSize)
{
	NN<Map::MapView> view;
	Math::RectAreaDbl minMax;
	this->GetBounds(minMax);
	NEW_CLASSNN(view, Map::ScaledMapView(scnSize, minMax.GetCenter(), 10000, this->csys->IsProjected()));
	return view;
}

void Map::MapDrawLayer::SetMixedData(MixedData mixedData)
{
}

void Map::MapDrawLayer::SetDispSize(Math::Size2DDbl size, Double dpi)
{

}

void Map::MapDrawLayer::AddUpdatedHandler(UpdatedHandler hdlr, AnyType obj)
{
}

void Map::MapDrawLayer::RemoveUpdatedHandler(UpdatedHandler hdlr, AnyType obj)
{
}

UOSInt Map::MapDrawLayer::QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names)
{
	if (schemaName.leng != 0)
		return 0;
	names->Add(this->sourceName->Clone());
	return 1;
}

Optional<DB::DBReader> Map::MapDrawLayer::QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition)
{
	NN<DB::DBReader> r;
	NEW_CLASSNN(r, Map::MapLayerReader(*this));
	return r;
}

Optional<DB::TableDef> Map::MapDrawLayer::GetTableDef(Text::CString schemaName, Text::CStringNN tableName)
{
	DB::TableDef *tab;
	NEW_CLASS(tab, DB::TableDef(schemaName, tableName));
	NN<DB::ColDef> col;
	UOSInt i = 0;
	UOSInt j = this->GetColumnCnt();
	while (i < j)
	{
		NEW_CLASSNN(col, DB::ColDef(CSTR("")));
		this->GetColumnDef(i, col);
		tab->AddCol(col);
		i++;
	}
	return tab;
}

void Map::MapDrawLayer::CloseReader(NN<DB::DBReader> r)
{
	Map::MapLayerReader *rdr = (Map::MapLayerReader*)r.Ptr();
	DEL_CLASS(rdr);
}

void Map::MapDrawLayer::GetLastErrorMsg(NN<Text::StringBuilderUTF8> str)
{
}

void Map::MapDrawLayer::Reconnect()
{
}

UOSInt Map::MapDrawLayer::GetNameCol() const
{
	return this->nameCol;
}

void Map::MapDrawLayer::SetNameCol(UOSInt nameCol)
{
	this->nameCol = nameCol;
}

NN<Text::String> Map::MapDrawLayer::GetName() const
{
	NN<Text::String> layerName;
	if (this->layerName.SetTo(layerName) && layerName->leng > 0)
	{
		return layerName;
	}
	else
	{
		return this->sourceName;
	}
}

IO::ParserType Map::MapDrawLayer::GetParserType() const
{
	return IO::ParserType::MapLayer;
}

NN<Math::CoordinateSystem> Map::MapDrawLayer::GetCoordinateSystem()
{
	return this->csys;
}

void Map::MapDrawLayer::SetCoordinateSystem(NN<Math::CoordinateSystem> csys)
{
	this->csys.Delete();
	this->csys = csys;
}

Int32 Map::MapDrawLayer::CalBlockSize()
{
	if (this->GetLayerType() == Map::DRAW_LAYER_POINT || this->GetLayerType() == Map::DRAW_LAYER_POINT3D)
	{
		return 5000;
	}
	else if (this->GetObjectClass() == Map::MapDrawLayer::OC_CIP_LAYER)
	{
		return ((Map::CIPLayer2*)this)->GetBlockSize();
	}
	else if (this->GetObjectClass() == Map::MapDrawLayer::OC_SPD_LAYER)
	{
		return ((Map::SPDLayer*)this)->GetBlockSize();
	}
	else if (this->GetObjectClass() == Map::MapDrawLayer::OC_TILE_MAP_LAYER)
	{
		return 5000;
	}
	else
	{
		Int32 blkSize;
		Data::ArrayListInt64 idList;
		Math::RectAreaDbl minMax;
		this->GetBounds(minMax);
				this->GetAllObjectIds(idList, 0);
		
		Double tVal = minMax.GetArea() / UOSInt2Double(idList.GetCount());
		if (minMax.max.x > 180)
		{
			blkSize = Double2Int32(Math_Sqrt(tVal) * 3);
			if (blkSize < 5000)
			{
				blkSize = 5000;
			}
			else if (blkSize > 5000000)
			{
				blkSize = 5000000;
			}
		}
		else
		{
			blkSize = Double2Int32(Math_Sqrt(tVal) * 500000);
			if (blkSize < 5000)
			{
				blkSize = 5000;
			}
			else if (blkSize > 5000000)
			{
				blkSize = 5000000;
			}
		}
		return blkSize;
	}
}

void Map::MapDrawLayer::SetLayerName(Text::CString name)
{
	OPTSTR_DEL(this->layerName);
	this->layerName = Text::String::NewOrNull(name);
}

Bool Map::MapDrawLayer::IsError() const
{
	return false;
}

Bool Map::MapDrawLayer::GetPGLabel(NN<Text::StringBuilderUTF8> sb, Math::Coord2DDbl coord, OptOut<Math::Coord2DDbl> outCoord, UOSInt strIndex)
{
	Bool retVal = false;

	Map::DrawLayerType layerType = this->GetLayerType();
	if (layerType != DRAW_LAYER_POLYGON && layerType != DRAW_LAYER_MIXED)
		return retVal;

	NN<Map::GetObjectSess> sess = BeginGetObject();
	Optional<Map::NameArray> names;
	Data::ArrayListInt64 arr;
	Int64 lastId;
	UOSInt i;
	Int64 thisId;
	GetObjectIdsMapXY(arr, names, Math::RectAreaDbl(coord, coord), false);
	lastId = -1;
	i = arr.GetCount();
	while (i-- > 0)
	{
		thisId = arr.GetItem(i);
		if (thisId != lastId)
		{
			lastId = thisId;
			NN<Math::Geometry::Vector2D> vec;
			if (this->GetNewVectorById(sess, thisId).SetTo(vec))
			{
				if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::Polygon)
				{
					NN<Math::Geometry::Polygon> pg = NN<Math::Geometry::Polygon>::ConvertFrom(vec);
					if (pg->InsideOrTouch(coord))
					{
						retVal = this->GetString(sb, names, lastId, strIndex);
						if (retVal)
						{
							outCoord.Set(coord);
							vec.Delete();
							break;
						}
					}
				}
				vec.Delete();
			}
		}
	}
	ReleaseNameArr(names);
	EndGetObject(sess);
	return retVal;
}

Bool Map::MapDrawLayer::GetPLLabel(NN<Text::StringBuilderUTF8> sb, Math::Coord2DDbl coord, OutParam<Math::Coord2DDbl> outCoord, UOSInt strIndex)
{
	Bool retVal = 0;
	Map::DrawLayerType layerType = this->GetLayerType();
	if (layerType != DRAW_LAYER_POLYLINE && layerType != DRAW_LAYER_POINT && layerType != DRAW_LAYER_MIXED && layerType != DRAW_LAYER_POINT3D && layerType != DRAW_LAYER_POLYLINE3D)
		return retVal;

	NN<Map::GetObjectSess> sess = BeginGetObject();
	Text::StringBuilderUTF8 tmpSb;
	Optional<Map::NameArray> names;
	Data::ArrayListInt64 arr;
	Int64 lastId;
	Int64 thisId;
	UOSInt i;
	NN<Math::Geometry::Vector2D> vec;
	Math::Coord2DDbl nearPt;
	Double thisDist;
	Double dist = 1000.0;
	Int32 blkSize = this->CalBlockSize();

	Int32 xBlk = Double2Int32(coord.x * 200000.0 / blkSize);
	Int32 yBlk = Double2Int32(coord.y * 200000.0 / blkSize);
	GetObjectIds(arr, names, 200000.0, Math::RectArea<Int32>((xBlk - 1) * blkSize, (yBlk - 1) * blkSize, 3 * blkSize - 1, 3 * blkSize - 1), false);
	lastId = -1;
	i = arr.GetCount();
	while (i-- > 0)
	{
		thisId = arr.GetItem(i);
		if (thisId != lastId)
		{
			lastId = thisId;
			if (this->GetNewVectorById(sess, thisId).SetTo(vec))
			{
				Bool succ = this->GetString(tmpSb, names, thisId, strIndex);
				if (succ && tmpSb.GetLength() > 0)
				{
					thisDist = vec->CalSqrDistance(coord, nearPt);
					if (thisDist < dist)
					{
						dist = thisDist;
						sb->ClearStr();
						sb->Append(tmpSb);
						retVal = true;
						outCoord.Set(nearPt);
					}
				}
				vec.Delete();
			}
		}
	}

	ReleaseNameArr(names);
	EndGetObject(sess);
	return retVal;
}

Bool Map::MapDrawLayer::CanQuery()
{
	return false;
}

Bool Map::MapDrawLayer::QueryInfos(Math::Coord2DDbl coord, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayList<UOSInt>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList)
{
	return false;
}

Int64 Map::MapDrawLayer::GetNearestObjectId(NN<GetObjectSess> session, Math::Coord2DDbl pt, OptOut<Math::Coord2DDbl> nearPt)
{
	Data::ArrayListInt64 objIds;
	Int32 blkSize = this->CalBlockSize();
	if (pt.x > 180 || pt.x < -180)
	{
		this->GetObjectIdsMapXY(objIds, 0, Math::RectAreaDbl(pt - blkSize, pt + blkSize), true);
	}
	else
	{
		this->GetObjectIdsMapXY(objIds, 0, Math::RectAreaDbl(pt.x - (blkSize / 200000.0), pt.y - (blkSize / 200000.0), (blkSize / 200000.0 * 2), (blkSize / 200000.0 * 2)), true);
	}

	UOSInt i = objIds.GetCount();
	Int64 nearObjId = -1;
	Double minDist = 0x7fffffff;
	Double dist;
	Math::Coord2DDbl currPt;
	Math::Coord2DDbl near = Math::Coord2DDbl(0, 0);

	while (i-- > 0)
	{
		NN<Math::Geometry::Vector2D> vec;
		if (this->GetNewVectorById(session, objIds.GetItem(i)).SetTo(vec))
		{
			dist = vec->CalSqrDistance(pt, currPt);
			if (dist < minDist)
			{
				nearObjId = objIds.GetItem(i);
				near = currPt;
				minDist = dist;
			}
			vec.Delete();
		}
	}
	nearPt.Set(near);
	return nearObjId;
}

void Map::MapDrawLayer::GetNearestObjectIds(NN<GetObjectSess> session, Math::Coord2DDbl pt, NN<Data::ArrayList<Int64>> ids, OptOut<Math::Coord2DDbl> nearPt)
{
	Data::ArrayListInt64 objIds;
	Int32 blkSize = this->CalBlockSize();
	if (pt.x > 180 || pt.x < -180)
	{
		this->GetObjectIdsMapXY(objIds, 0, Math::RectAreaDbl(pt - blkSize, pt + blkSize), true);
	}
	else
	{
		this->GetObjectIdsMapXY(objIds, 0, Math::RectAreaDbl(pt.x - (blkSize / 200000.0), pt.y - (blkSize / 200000.0), (blkSize / 200000.0 * 2), (blkSize / 200000.0 * 2)), true);
	}

	UOSInt i = objIds.GetCount();
	Double minDist = 0x7fffffff;
	Double dist;
	Int64 id;
	Math::Coord2DDbl currPt;
	Math::Coord2DDbl near = pt;

	while (i-- > 0)
	{
		NN<Math::Geometry::Vector2D> vec;
		id = objIds.GetItem(i);
		if (this->GetNewVectorById(session, id).SetTo(vec))
		{
			dist = vec->CalSqrDistance(pt, currPt);
			if (dist < minDist)
			{
				ids->Clear();
				ids->Add(id);
				near = currPt;
				minDist = dist;
			}
			else if (dist == minDist)
			{
				ids->Add(objIds.GetItem(i));
			}
			vec.Delete();
		}
	}
	nearPt.Set(near);
}

OSInt Map::MapDrawLayer::GetNearObjects(NN<GetObjectSess> session, NN<Data::ArrayListNN<ObjectInfo>> objList, Math::Coord2DDbl pt, Double maxDist)
{
	Data::ArrayListInt64 objIds;
	Int32 blkSize = this->CalBlockSize();
	if (pt.x > 180 || pt.x < -180)
	{
		this->GetObjectIdsMapXY(objIds, 0, Math::RectAreaDbl(pt - blkSize, pt + blkSize), true);
	}
	else
	{
		this->GetObjectIdsMapXY(objIds, 0, Math::RectAreaDbl(pt.x - (blkSize / 200000.0), pt.y - (blkSize / 200000.0), (blkSize / 200000.0 * 2), (blkSize / 200000.0 * 2)), true);
	}

	UOSInt i = objIds.GetCount();
	Int64 nearObjId = -1;
	Double minDist = 0x7fffffff;
	Double dist;
	Math::Coord2DDbl currPt;
	Math::Coord2DDbl nearPt = Math::Coord2DDbl(0, 0);
	Double sqrMaxDist = maxDist * maxDist;
	NN<ObjectInfo> objInfo;
	OSInt ret = 0;

	while (i-- > 0)
	{
		NN<Math::Geometry::Vector2D> vec;
		if (this->GetNewVectorById(session, objIds.GetItem(i)).SetTo(vec))
		{
			dist = vec->CalSqrDistance(pt, currPt);
			if (dist <= sqrMaxDist)
			{
				objInfo = MemAllocANN(ObjectInfo);
				objInfo->objId = objIds.GetItem(i);
				objInfo->objPos = currPt;
				objInfo->objDist = Math_Sqrt(dist);
				objList->Add(objInfo);
				ret++;
			}
			if (dist < minDist)
			{
				nearObjId = objIds.GetItem(i);
				nearPt = currPt;
				minDist = dist;
			}
			vec.Delete();
		}
	}

	if (ret > 0)
	{
		Data::Sort::ArtificialQuickSortFunc<NN<ObjectInfo>>::Sort(objList, ObjectCompare);
	}
	else if (nearObjId != -1)
	{
		objInfo = MemAllocANN(ObjectInfo);
		objInfo->objId = nearObjId;
		objInfo->objPos = nearPt;
		objInfo->objDist = Math_Sqrt(minDist);
		objList->Add(objInfo);
		ret++;
	}
	return ret;
}

void Map::MapDrawLayer::FreeObjects(NN<Data::ArrayListNN<ObjectInfo>> objList)
{
	NN<ObjectInfo> objInfo;
	UOSInt i;
	i = objList->GetCount();
	while (i-- > 0)
	{
		objInfo = objList->GetItemNoCheck(i);
		MemFreeANN(objInfo);
	}
	objList->Clear();
}

NN<Map::VectorLayer> Map::MapDrawLayer::CreateEditableLayer()
{
	Text::StringBuilderUTF8 sb;
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	UOSInt *ofsts;
	UnsafeArray<UnsafeArrayOpt<const UTF8Char>> sptrs;
	NN<Map::VectorLayer> lyr;
	Data::ArrayListInt64 objIds;
	Optional<Math::Geometry::Vector2D> vec;
	NN<Math::Geometry::Vector2D> nnvec;
	Optional<NameArray> nameArr;
	NN<GetObjectSess> sess;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;

	k = this->GetColumnCnt();
	i = k;
	ofsts = MemAlloc(UOSInt, k);
	sptrs = MemAllocArr(UnsafeArrayOpt<const UTF8Char>, k);
	sb.AllocLeng(65536);
	sptr = sb.v;
	while (i-- > 0)
	{
		if (this->GetColumnName(sptr, i).SetTo(sptr2))
		{
			sptrs[i] = UnsafeArray<const UTF8Char>(sptr);
			sptr = sptr2 + 1;
		}
		else
		{
			sptrs[i] = 0;
		}
	}
	NN<Math::CoordinateSystem> csys = this->csys->Clone();
	NEW_CLASSNN(lyr, Map::VectorLayer(this->GetLayerType(), this->sourceName, k, sptrs, csys, this->GetNameCol(), this->layerName));

	sess = this->BeginGetObject();
	this->GetAllObjectIds(objIds, nameArr);
	i = 0;
	j = objIds.GetCount();
	while (i < j)
	{
		vec = this->GetNewVectorById(sess, objIds.GetItem(i));
		if (vec.IsNull())
		{
			vec = this->GetNewVectorById(sess, objIds.GetItem(i));
		}
		if (vec.SetTo(nnvec))
		{
			sb.ClearStr();
			l = k;
			while (l-- > 0)
			{
				ofsts[l] = sb.GetLength();
				if (this->GetString(sb, nameArr, objIds.GetItem(i), l))
				{
					sb.AppendUTF8Char('\0');
				}
				else
				{
					ofsts[l] = INVALID_INDEX;
				}
			}
			l = k;
			while (l-- > 0)
			{
				if (ofsts[l] == INVALID_INDEX)
				{
					sptrs[l] = 0;
				}
				else
				{
					sptrs[l] = &sb.v[ofsts[l]];
				}
			}
			if (lyr->AddVector2(nnvec, sptrs) == -1)
			{
				nnvec.Delete();
			}
		}

		i++;
	}

	MemFreeArr(sptrs);
	MemFree(ofsts);
	return lyr;
}

Optional<Text::SearchIndexer> Map::MapDrawLayer::CreateSearchIndexer(NN<Text::TextAnalyzer> ta, UOSInt strIndex)
{
	if (strIndex >= this->GetColumnCnt())
		return 0;

	Text::SearchIndexer *searching;
	Data::ArrayListInt64 objIds;
	Optional<NameArray> nameArr;
	UOSInt i;

	NEW_CLASS(searching, Text::SearchIndexer(ta));
	this->GetAllObjectIds(objIds, nameArr);
	Text::StringBuilderUTF8 sb;
	i = objIds.GetCount();
	while (i-- > 0)
	{
		sb.ClearStr();
		if (this->GetString(sb, nameArr, objIds.GetItem(i), strIndex))
		{
			searching->IndexString(sb.v, objIds.GetItem(i));
		}
	}
	this->ReleaseNameArr(nameArr);
	return searching;
}

UOSInt Map::MapDrawLayer::SearchString(NN<Data::ArrayListString> outArr, NN<Text::SearchIndexer> srchInd, Optional<NameArray> nameArr, UnsafeArray<const UTF8Char> srchStr, UOSInt maxResult, UOSInt strIndex)
{
	Text::PString s;

	if (maxResult <= 0)
		return 0;

	Data::ArrayListInt64 objIds;
	Data::ArrayListICaseString strList;
	srchInd->SearchString(objIds, srchStr, maxResult * 10);
	
	Text::StringBuilderUTF8 sb;
	UOSInt i = 0;
	UOSInt j = objIds.GetCount();
	OSInt k;
	UOSInt resCnt = 0;
	while (i < j)
	{
		sb.ClearStr();
		if (this->GetString(sb, nameArr, objIds.GetItem(i), strIndex))
		{
			sb.Trim();
			s = sb;
			k = strList.SortedIndexOfPtr(s.v, s.leng);
			if (k < 0)
			{
				strList.Insert((UOSInt)~k, Text::String::New(s.v, s.leng).Ptr());
				if (++resCnt >= maxResult)
					break;
			}
		}
		
		i++;
	}

	outArr->AddAll(strList);
	return resCnt;
}

void Map::MapDrawLayer::ReleaseSearchStr(NN<Data::ArrayListString> strArr)
{
	LIST_FREE_STRING(strArr);
}

Optional<Math::Geometry::Vector2D> Map::MapDrawLayer::GetVectorByStr(NN<Text::SearchIndexer> srchInd, Optional<Map::NameArray> nameArr, NN<Map::GetObjectSess> session, Text::CStringNN srchStr, UOSInt strIndex)
{
	Optional<Math::Geometry::Vector2D> vec = 0;
	NN<Math::Geometry::Vector2D> nnvec;
	Data::ArrayListInt64 objIds;
	srchInd->SearchString(objIds, srchStr.v, 10000);

	Text::StringBuilderUTF8 sb;
	UOSInt i = 0;
	UOSInt j = objIds.GetCount();
	while (i < j)
	{
		sb.ClearStr();
		this->GetString(sb, nameArr, objIds.GetItem(i), strIndex);
		sb.Trim();
		if (srchStr.EqualsICase(sb))
		{
			if (!vec.SetTo(nnvec))
			{
				vec = this->GetNewVectorById(session, objIds.GetItem(i));
			}
			else
			{
				NN<Math::Geometry::Vector2D> tmpVec;
				if (this->GetNewVectorById(session, objIds.GetItem(i)).SetTo(tmpVec))
				{
					nnvec->JoinVector(tmpVec);
					tmpVec.Delete();
				}
			}
		}
		i++;
	}
	return vec;
}

Bool Map::MapDrawLayer::HasLineStyle()
{
	return this->lineWidth > 0;
}

Bool Map::MapDrawLayer::HasPGStyle()
{
	return this->pgColor != 0;
}

Bool Map::MapDrawLayer::HasIconStyle()
{
	return this->iconImg != 0;
}

void Map::MapDrawLayer::SetLineStyle(UInt32 lineColor, Double lineWidth)
{
	this->lineColor = lineColor;
	this->lineWidth = lineWidth;
}

void Map::MapDrawLayer::SetPGStyle(UInt32 pgColor)
{
	this->pgColor = pgColor;
}

void Map::MapDrawLayer::SetIconStyle(NN<Media::SharedImage> iconImg, OSInt iconSpotX, OSInt iconSpotY)
{
	this->iconImg.Delete();
	this->iconImg = iconImg->Clone();
	this->iconSpotX = iconSpotX;
	this->iconSpotY = iconSpotY;
}

UInt32 Map::MapDrawLayer::GetLineStyleColor()
{
	return this->lineColor;
}

Double Map::MapDrawLayer::GetLineStyleWidth()
{
	return this->lineWidth;
}

UInt32 Map::MapDrawLayer::GetPGStyleColor()
{
	return this->pgColor;
}

Optional<Media::SharedImage> Map::MapDrawLayer::GetIconStyleImg()
{
	return this->iconImg;
}

OSInt Map::MapDrawLayer::GetIconStyleSpotX()
{
	return this->iconSpotX;
}

OSInt Map::MapDrawLayer::GetIconStyleSpotY()
{
	return this->iconSpotY;
}

Bool Map::MapDrawLayer::IsLabelVisible()
{
	return this->flags & 1;
}

void Map::MapDrawLayer::SetLabelVisible(Bool labelVisible)
{
	if (labelVisible)
	{
		this->flags |= 1;
	}
	else
	{
		this->flags &= ~1;
	}
}

Map::DrawLayerType Map::MapDrawLayer::VectorType2LayerType(Math::Geometry::Vector2D::VectorType vtype)
{
	switch (vtype)
	{
	case Math::Geometry::Vector2D::VectorType::Point:
	case Math::Geometry::Vector2D::VectorType::MultiPoint:
		return Map::DRAW_LAYER_POINT;
	case Math::Geometry::Vector2D::VectorType::Polygon:
	case Math::Geometry::Vector2D::VectorType::MultiPolygon:
	case Math::Geometry::Vector2D::VectorType::CurvePolygon:
	case Math::Geometry::Vector2D::VectorType::MultiSurface:
	case Math::Geometry::Vector2D::VectorType::LinearRing:
		return Map::DRAW_LAYER_POLYGON;
	case Math::Geometry::Vector2D::VectorType::Polyline:
	case Math::Geometry::Vector2D::VectorType::CompoundCurve:
	case Math::Geometry::Vector2D::VectorType::CircularString:
	case Math::Geometry::Vector2D::VectorType::LineString:
		return Map::DRAW_LAYER_POLYLINE;
	case Math::Geometry::Vector2D::VectorType::Image:
		return Map::DRAW_LAYER_IMAGE;
	case Math::Geometry::Vector2D::VectorType::GeometryCollection:
	case Math::Geometry::Vector2D::VectorType::MultiCurve:
	case Math::Geometry::Vector2D::VectorType::Curve:
	case Math::Geometry::Vector2D::VectorType::Surface:
	case Math::Geometry::Vector2D::VectorType::PolyhedralSurface:
		return Map::DRAW_LAYER_MIXED;
	case Math::Geometry::Vector2D::VectorType::Tin:
	case Math::Geometry::Vector2D::VectorType::Triangle:
	case Math::Geometry::Vector2D::VectorType::String:
	case Math::Geometry::Vector2D::VectorType::Ellipse:
	case Math::Geometry::Vector2D::VectorType::PieArea:
	case Math::Geometry::Vector2D::VectorType::Unknown:
	default:
		return Map::DRAW_LAYER_UNKNOWN;
	}
}

Int64 Map::MapLayerReader::GetCurrObjId()
{
	return this->objIds.GetItem((UOSInt)this->currIndex);
}

Map::MapLayerReader::MapLayerReader(NN<Map::MapDrawLayer> layer) : DB::DBReader()
{
	this->layer = layer;

	this->layer->GetAllObjectIds(this->objIds, this->nameArr);
	this->currIndex = -1;
}

Map::MapLayerReader::~MapLayerReader()
{
	this->layer->ReleaseNameArr(this->nameArr);
}

Bool Map::MapLayerReader::ReadNext()
{
	this->currIndex++;
	if ((UOSInt)this->currIndex >= this->objIds.GetCount())
		return false;
	else
		return true;
}

UOSInt Map::MapLayerReader::ColCount()
{
	return this->layer->GetColumnCnt() + 1;
}

OSInt Map::MapLayerReader::GetRowChanged()
{
	return -1;
}

Int32 Map::MapLayerReader::GetInt32(UOSInt colIndex)
{
	if (colIndex <= 0)
		return 0;
	Text::StringBuilderUTF8 sb;
	this->layer->GetString(sb, this->nameArr, this->GetCurrObjId(), colIndex - 1);
	return sb.ToInt32();
}

Int64 Map::MapLayerReader::GetInt64(UOSInt colIndex)
{
	if (colIndex == 0)
		return 0;
	Text::StringBuilderUTF8 sb;
	this->layer->GetString(sb, this->nameArr, this->GetCurrObjId(), colIndex - 1);
	return sb.ToInt64();
}

UnsafeArrayOpt<WChar> Map::MapLayerReader::GetStr(UOSInt colIndex, UnsafeArray<WChar> buff)
{
	if (colIndex <= 0)
	{
		NN<Math::Geometry::Vector2D> vec;
		if (!this->GetVector(0).SetTo(vec))
			return 0;
		Math::WKTWriter writer;
		Text::StringBuilderUTF8 sb;
		Bool succ = writer.ToText(sb, vec);
		vec.Delete();
		if (!succ)
		{
			return 0;
		}
		return Text::StrUTF8_WCharC(buff, sb.v, sb.leng, 0);
	}
	Text::StringBuilderUTF8 sb;
	if (this->layer->GetString(sb, this->nameArr, this->GetCurrObjId(), colIndex - 1))
	{
		return Text::StrUTF8_WCharC(buff, sb.v, sb.leng, 0);
	}
	return 0;
}

Bool Map::MapLayerReader::GetStr(UOSInt colIndex, NN<Text::StringBuilderUTF8> sb)
{
	if (colIndex <= 0)
	{
		NN<Math::Geometry::Vector2D> vec;
		if (!this->GetVector(0).SetTo(vec))
			return 0;
		Math::WKTWriter writer;
		Bool succ = writer.ToText(sb, vec);
		vec.Delete();
		return succ;
	}
	return this->layer->GetString(sb, this->nameArr, this->GetCurrObjId(), colIndex - 1);
}

Optional<Text::String> Map::MapLayerReader::GetNewStr(UOSInt colIndex)
{
	if (colIndex <= 0)
	{
		NN<Math::Geometry::Vector2D> vec;
		if (!this->GetVector(0).SetTo(vec))
			return 0;
		Math::WKTWriter writer;
		Text::StringBuilderUTF8 sb;
		Bool succ = writer.ToText(sb, vec);
		vec.Delete();
		if (!succ)
		{
			return 0;
		}
		return Text::String::New(sb.ToCString());
	}
	Text::StringBuilderUTF8 sb;
	if (this->layer->GetString(sb, this->nameArr, this->GetCurrObjId(), colIndex - 1))
		return Text::String::New(sb.ToCString());
	return 0;
}

UnsafeArrayOpt<UTF8Char> Map::MapLayerReader::GetStr(UOSInt colIndex, UnsafeArray<UTF8Char> buff, UOSInt buffSize)
{
	if (colIndex <= 0)
	{
		NN<Math::Geometry::Vector2D> vec;
		if (!this->GetVector(0).SetTo(vec))
			return 0;
		Math::WKTWriter writer;
		Text::StringBuilderUTF8 sb;
		Bool succ = writer.ToText(sb, vec);
		vec.Delete();
		if (!succ)
		{
			return 0;
		}
		return sb.ConcatToS(buff, buffSize);
	}
	Text::StringBuilderUTF8 sb;
	if (this->layer->GetString(sb, this->nameArr, this->GetCurrObjId(), colIndex - 1))
		return sb.ConcatToS(buff, buffSize);
	return 0;
}

Data::Timestamp Map::MapLayerReader::GetTimestamp(UOSInt colIndex)
{
	if (colIndex <= 0)
		return Data::Timestamp(0);
	Text::StringBuilderUTF8 sb;
	if (this->layer->GetString(sb, this->nameArr, this->GetCurrObjId(), colIndex - 1))
		return Data::Timestamp(sb.ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
	return 0;
}

Double Map::MapLayerReader::GetDblOrNAN(UOSInt colIndex)
{
	if (colIndex <= 0)
		return NAN;
	Text::StringBuilderUTF8 sb;
	this->layer->GetString(sb, this->nameArr, this->GetCurrObjId(), colIndex - 1);
	return sb.ToDoubleOrNAN();
}

Bool Map::MapLayerReader::GetBool(UOSInt colIndex)
{
	if (colIndex <= 0)
		return 0;
	Text::StringBuilderUTF8 sb;
	this->layer->GetString(sb, this->nameArr, this->GetCurrObjId(), colIndex - 1);
	return sb.ToBool() != 0;
}

UOSInt Map::MapLayerReader::GetBinarySize(UOSInt colIndex)
{
	return 0;
}

UOSInt Map::MapLayerReader::GetBinary(UOSInt colIndex, UnsafeArray<UInt8> buff)
{
	return 0;
}

Optional<Math::Geometry::Vector2D> Map::MapLayerReader::GetVector(UOSInt colIndex)
{
	if (colIndex != 0)
		return 0;
	if ((UOSInt)this->currIndex >= this->objIds.GetCount() || this->currIndex < 0)
		return 0;
	NN<GetObjectSess> sess = this->layer->BeginGetObject();
	Optional<Math::Geometry::Vector2D> vec = this->layer->GetNewVectorById(sess, this->GetCurrObjId());
	this->layer->EndGetObject(sess);
	return vec;

}

Bool Map::MapLayerReader::GetUUID(UOSInt colIndex, NN<Data::UUID> uuid)
{
	return false;
}
 
Bool Map::MapLayerReader::IsNull(UOSInt colIndex)
{
	if (colIndex == 0)
		return false;
	Text::StringBuilderUTF8 sb;
	return !this->layer->GetString(sb, this->nameArr, this->GetCurrObjId(), colIndex - 1);
}

UnsafeArrayOpt<UTF8Char> Map::MapLayerReader::GetName(UOSInt colIndex, UnsafeArray<UTF8Char> buff)
{
	if (colIndex == 0)
		return Text::StrConcatC(buff, UTF8STRC("Shape"));
	return this->layer->GetColumnName(buff, colIndex - 1);
}

DB::DBUtil::ColType Map::MapLayerReader::GetColType(UOSInt colIndex, OptOut<UOSInt> colSize)
{
	if (colIndex == 0)
		return DB::DBUtil::CT_Vector;
	return this->layer->GetColumnType(colIndex - 1, colSize);
}

Bool Map::MapLayerReader::GetColDef(UOSInt colIndex, NN<DB::ColDef> colDef)
{
	return GetColDefV(colIndex, colDef, this->layer);
}

Bool Map::MapLayerReader::GetColDefV(UOSInt colIndex, NN<DB::ColDef> colDef, NN<Map::MapDrawLayer> layer)
{
	if (colIndex == 0)
	{
		NN<Math::CoordinateSystem> csys = layer->GetCoordinateSystem();
		colDef->SetColType(DB::DBUtil::CT_Vector);
		colDef->SetColName(CSTR("Shape"));
		switch (layer->GetLayerType())
		{
		case Map::DRAW_LAYER_UNKNOWN:
		case Map::DRAW_LAYER_MIXED:
		default:
			colDef->SetColSize((UOSInt)DB::ColDef::GeometryType::Any);
			break;
		case Map::DRAW_LAYER_POINT:
			colDef->SetColSize((UOSInt)DB::ColDef::GeometryType::Point);
			break;
		case Map::DRAW_LAYER_POLYLINE:
			colDef->SetColSize((UOSInt)DB::ColDef::GeometryType::Polyline);
			break;
		case Map::DRAW_LAYER_POLYGON:
			colDef->SetColSize((UOSInt)DB::ColDef::GeometryType::Polygon);
			break;
		case Map::DRAW_LAYER_POINT3D:
			colDef->SetColSize((UOSInt)DB::ColDef::GeometryType::PointZ);
			break;
		case Map::DRAW_LAYER_POLYLINE3D:
			colDef->SetColSize((UOSInt)DB::ColDef::GeometryType::PolylineZ);
			break;
		case Map::DRAW_LAYER_IMAGE:
			colDef->SetColSize((UOSInt)DB::ColDef::GeometryType::Any);
			break;
		}
		colDef->SetColDP(csys->GetSRID());
		colDef->SetAttr(CSTR(""));
		colDef->SetDefVal(CSTR_NULL);
		colDef->SetAutoIncNone();
		colDef->SetNotNull(true);
		colDef->SetPK(false);
		return true;
	}
	return layer->GetColumnDef(colIndex - 1, colDef);
}
