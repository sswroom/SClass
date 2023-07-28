#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListInt32.h"
#include "Data/ArrayListICaseString.h"
#include "Data/Sort/ArtificialQuickSortC.h"
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

OSInt __stdcall Map::MapDrawLayer::ObjectCompare(void *obj1, void *obj2)
{
	ObjectInfo *objInfo1 = (ObjectInfo*)obj1;
	ObjectInfo *objInfo2 = (ObjectInfo*)obj2;
	if (objInfo1->objDist > objInfo2->objDist)
	{
		return 1;
	}
	else if (objInfo1->objDist < objInfo2->objDist)
	{
		return -1;
	}
	else if (objInfo1->objId > objInfo2->objId)
	{
		return 1;
	}
	else if (objInfo1->objId < objInfo2->objId)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

Map::MapDrawLayer::MapDrawLayer(NotNullPtr<Text::String> sourceName, UOSInt nameCol, Text::String *layerName) : DB::ReadingDB(sourceName)//IO::ParsedObject(sourceName)
{
	this->nameCol = nameCol;
	this->layerName = SCOPY_STRING(layerName);
	this->csys = 0;

	this->pgColor = 0;
	this->lineColor = 0;
	this->lineWidth = 0;
	this->iconImg = 0;
	this->iconSpotX = 0;
	this->iconSpotY = 0;
	this->flags = 0;
}

Map::MapDrawLayer::MapDrawLayer(Text::CString sourceName, UOSInt nameCol, Text::CString layerName) : DB::ReadingDB(sourceName)//IO::ParsedObject(sourceName)
{
	this->nameCol = nameCol;
	this->layerName = Text::String::New(layerName).Ptr();
	this->csys = 0;

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
	SDEL_CLASS(this->csys);
	SDEL_CLASS(this->iconImg);
	SDEL_STRING(this->layerName);
}

void Map::MapDrawLayer::SetCurrScale(Double scale)
{
}

void Map::MapDrawLayer::SetCurrTimeTS(Int64 timeStamp)
{
}

Int64 Map::MapDrawLayer::GetTimeStartTS()
{
	return 0;
}

Int64 Map::MapDrawLayer::GetTimeEndTS()
{
	return 0;
}

Map::MapView *Map::MapDrawLayer::CreateMapView(Math::Size2DDbl scnSize)
{
	Map::MapView *view;
	Math::RectAreaDbl minMax;
	this->GetBounds(&minMax);
	if (this->csys)
	{
		NEW_CLASS(view, Map::ScaledMapView(scnSize, minMax.GetCenter(), 10000, this->csys->IsProjected()));
	}
	else
	{
		NEW_CLASS(view, Map::ScaledMapView(scnSize, minMax.GetCenter(), 10000, minMax.br.x > 1000));
	}
	return view;
}

void Map::MapDrawLayer::SetMixedData(MixedData mixedData)
{
}

void Map::MapDrawLayer::SetDispSize(Math::Size2DDbl size, Double dpi)
{

}

void Map::MapDrawLayer::AddUpdatedHandler(UpdatedHandler hdlr, void *obj)
{
}

void Map::MapDrawLayer::RemoveUpdatedHandler(UpdatedHandler hdlr, void *obj)
{
}

UOSInt Map::MapDrawLayer::QueryTableNames(Text::CString schemaName, Data::ArrayListNN<Text::String> *names)
{
	if (schemaName.leng != 0)
		return 0;
	names->Add(this->sourceName->Clone());
	return 1;
}

DB::DBReader *Map::MapDrawLayer::QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListNN<Text::String> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	DB::DBReader *r;
	NEW_CLASS(r, Map::MapLayerReader(this));
	return r;
}

DB::TableDef *Map::MapDrawLayer::GetTableDef(Text::CString schemaName, Text::CString tableName)
{
	DB::TableDef *tab;
	NEW_CLASS(tab, DB::TableDef(schemaName, tableName));
	DB::ColDef *col;
	UOSInt i = 0;
	UOSInt j = this->GetColumnCnt();
	while (i < j)
	{
		NEW_CLASS(col, DB::ColDef(CSTR("")));
		this->GetColumnDef(i, col);
		tab->AddCol(col);
	}
	return tab;
}

void Map::MapDrawLayer::CloseReader(DB::DBReader *r)
{
	Map::MapLayerReader *rdr = (Map::MapLayerReader*)r;
	DEL_CLASS(rdr);
}

void Map::MapDrawLayer::GetLastErrorMsg(Text::StringBuilderUTF8 *str)
{
}

void Map::MapDrawLayer::Reconnect()
{
}

UOSInt Map::MapDrawLayer::GetNameCol()
{
	return this->nameCol;
}

void Map::MapDrawLayer::SetNameCol(UOSInt nameCol)
{
	this->nameCol = nameCol;
}

NotNullPtr<Text::String> Map::MapDrawLayer::GetName()
{
	NotNullPtr<Text::String> layerName;
	if (layerName.Set(this->layerName) && this->layerName->leng > 0)
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

Math::CoordinateSystem *Map::MapDrawLayer::GetCoordinateSystem()
{
	return this->csys;
}

void Map::MapDrawLayer::SetCoordinateSystem(Math::CoordinateSystem *csys)
{
	SDEL_CLASS(this->csys);
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
		Data::ArrayListInt64 *idList;
		Math::RectAreaDbl minMax;
		this->GetBounds(&minMax);
		
		NEW_CLASS(idList, Data::ArrayListInt64());
		this->GetAllObjectIds(idList, 0);
		
		Double tVal = minMax.GetArea() / UOSInt2Double(idList->GetCount());
		if (minMax.br.x > 180)
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
		DEL_CLASS(idList);
		return blkSize;
	}
}

void Map::MapDrawLayer::SetLayerName(Text::CString name)
{
	SDEL_STRING(this->layerName);
	this->layerName = Text::String::New(name).Ptr();
}

Bool Map::MapDrawLayer::IsError()
{
	return false;
}

UTF8Char *Map::MapDrawLayer::GetPGLabel(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl coord, Math::Coord2DDbl *outCoord, UOSInt strIndex)
{
	UTF8Char *retVal = 0;

	Map::DrawLayerType layerType = this->GetLayerType();
	if (layerType != DRAW_LAYER_POLYGON && layerType != DRAW_LAYER_MIXED)
		return retVal;

	Map::GetObjectSess *sess = BeginGetObject();
	Map::NameArray *names;
	Data::ArrayListInt64 *arr;
	Int64 lastId;
	UOSInt i;
	Int64 thisId;
	NEW_CLASS(arr, Data::ArrayListInt64());
	GetObjectIdsMapXY(arr, &names, Math::RectAreaDbl(coord, coord), false);
	lastId = -1;
	i = arr->GetCount();
	while (i-- > 0)
	{
		thisId = arr->GetItem(i);
		if (thisId != lastId)
		{
			lastId = thisId;
			Math::Geometry::Vector2D *vec = this->GetNewVectorById(sess, thisId);
			if (vec)
			{
				if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::Polygon)
				{
					Math::Geometry::Polygon *pg = (Math::Geometry::Polygon*)vec;
					if (pg->InsideVector(coord))
					{
						retVal = this->GetString(buff, buffSize, names, lastId, strIndex);
						if (buff != retVal)
						{
							*outCoord = coord;
							DEL_CLASS(vec);
							break;
						}
					}
				}
				DEL_CLASS(vec);
			}
		}
	}

	DEL_CLASS(arr);
	ReleaseNameArr(names);
	EndGetObject(sess);
	return retVal;
}

UTF8Char *Map::MapDrawLayer::GetPLLabel(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl coord, Math::Coord2DDbl *outCoord, UOSInt strIndex)
{
	UTF8Char *retVal = 0;
	UTF8Char *tmpBuff;
	Map::DrawLayerType layerType = this->GetLayerType();
	if (layerType != DRAW_LAYER_POLYLINE && layerType != DRAW_LAYER_POINT && layerType != DRAW_LAYER_MIXED && layerType != DRAW_LAYER_POINT3D && layerType != DRAW_LAYER_POLYLINE3D)
		return retVal;

	Map::GetObjectSess *sess = BeginGetObject();
	tmpBuff = MemAlloc(UTF8Char, buffSize);
	Map::NameArray *names;
	Data::ArrayListInt64 arr;
	Int64 lastId;
	Int64 thisId;
	UOSInt i;
	Math::Geometry::Vector2D *vec;
	Math::Coord2DDbl nearPt;
	Double thisDist;
	Double dist = 1000.0;
	Int32 blkSize = this->CalBlockSize();

	Int32 xBlk = Double2Int32(coord.x * 200000.0 / blkSize);
	Int32 yBlk = Double2Int32(coord.y * 200000.0 / blkSize);
	GetObjectIds(&arr, &names, 200000.0, Math::RectArea<Int32>((xBlk - 1) * blkSize, (yBlk - 1) * blkSize, 3 * blkSize - 1, 3 * blkSize - 1), false);
	lastId = -1;
	i = arr.GetCount();
	while (i-- > 0)
	{
		thisId = arr.GetItem(i);
		if (thisId != lastId)
		{
			lastId = thisId;
			vec = this->GetNewVectorById(sess, thisId);
			UTF8Char *sptr = this->GetString(tmpBuff, buffSize, names, thisId, strIndex);
			if (sptr && sptr != tmpBuff)
			{
				thisDist = vec->CalSqrDistance(coord, &nearPt);
				if (thisDist < dist)
				{
					dist = thisDist;
					retVal = Text::StrConcat(buff, tmpBuff);
					if (outCoord)
					{
						*outCoord = nearPt;
					}
				}
			}
			DEL_CLASS(vec);
		}
	}

	ReleaseNameArr(names);
	EndGetObject(sess);
	MemFree(tmpBuff);
	return retVal;
}

Bool Map::MapDrawLayer::CanQuery()
{
	return false;
}

Bool Map::MapDrawLayer::QueryInfos(Math::Coord2DDbl coord, Data::ArrayList<Math::Geometry::Vector2D*> *vecList, Data::ArrayList<UOSInt> *valueOfstList, Data::ArrayListNN<Text::String> *nameList, Data::ArrayList<Text::String*> *valueList)
{
	return false;
}

Int64 Map::MapDrawLayer::GetNearestObjectId(GetObjectSess *session, Math::Coord2DDbl pt, Math::Coord2DDbl *nearPt)
{
	Data::ArrayListInt64 *objIds;
	NEW_CLASS(objIds, Data::ArrayListInt64());
	Int32 blkSize = this->CalBlockSize();
	if (pt.x > 180 || pt.x < -180)
	{
		this->GetObjectIdsMapXY(objIds, 0, Math::RectAreaDbl(pt - blkSize, pt + blkSize), true);
	}
	else
	{
		this->GetObjectIdsMapXY(objIds, 0, Math::RectAreaDbl(pt.x - (blkSize / 200000.0), pt.y - (blkSize / 200000.0), (blkSize / 200000.0 * 2), (blkSize / 200000.0 * 2)), true);
	}

	UOSInt i = objIds->GetCount();
	Int64 nearObjId = -1;
	Double minDist = 0x7fffffff;
	Double dist;
	Math::Coord2DDbl currPt;
	Math::Coord2DDbl near = Math::Coord2DDbl(0, 0);

	while (i-- > 0)
	{
		Math::Geometry::Vector2D *vec = this->GetNewVectorById(session, objIds->GetItem(i));
		if (vec)
		{
			dist = vec->CalSqrDistance(pt, &currPt);
			if (dist < minDist)
			{
				nearObjId = objIds->GetItem(i);
				near = currPt;
				minDist = dist;
			}
			DEL_CLASS(vec);
		}
	}

	DEL_CLASS(objIds);
	if (nearPt)
	{
		*nearPt = near;
	}
	return nearObjId;
}

OSInt Map::MapDrawLayer::GetNearObjects(GetObjectSess *session, Data::ArrayList<ObjectInfo*> *objList, Math::Coord2DDbl pt, Double maxDist)
{
	Data::ArrayListInt64 *objIds;
	NEW_CLASS(objIds, Data::ArrayListInt64());
	Int32 blkSize = this->CalBlockSize();
	if (pt.x > 180 || pt.x < -180)
	{
		this->GetObjectIdsMapXY(objIds, 0, Math::RectAreaDbl(pt - blkSize, pt + blkSize), true);
	}
	else
	{
		this->GetObjectIdsMapXY(objIds, 0, Math::RectAreaDbl(pt.x - (blkSize / 200000.0), pt.y - (blkSize / 200000.0), (blkSize / 200000.0 * 2), (blkSize / 200000.0 * 2)), true);
	}

	UOSInt i = objIds->GetCount();
	Int64 nearObjId = -1;
	Double minDist = 0x7fffffff;
	Double dist;
	Math::Coord2DDbl currPt;
	Math::Coord2DDbl nearPt = Math::Coord2DDbl(0, 0);
	Double sqrMaxDist = maxDist * maxDist;
	ObjectInfo *objInfo;
	OSInt ret = 0;

	while (i-- > 0)
	{
		Math::Geometry::Vector2D *vec = this->GetNewVectorById(session, objIds->GetItem(i));
		dist = vec->CalSqrDistance(pt, &currPt);
		if (dist <= sqrMaxDist)
		{
			objInfo = MemAllocA(ObjectInfo, 1);
			objInfo->objId = objIds->GetItem(i);
			objInfo->objPos = currPt;
			objInfo->objDist = Math_Sqrt(dist);
			objList->Add(objInfo);
			ret++;
		}
		if (dist < minDist)
		{
			nearObjId = objIds->GetItem(i);
			nearPt = currPt;
			minDist = dist;
		}
		DEL_CLASS(vec);
	}

	DEL_CLASS(objIds);

	if (ret > 0)
	{
		void **arr = (void**)objList->GetArray(&i);
		ArtificialQuickSort_SortCmp(arr, ObjectCompare, 0, (OSInt)i - 1);
	}
	else if (nearObjId != -1)
	{
		objInfo = MemAllocA(ObjectInfo, 1);
		objInfo->objId = nearObjId;
		objInfo->objPos = nearPt;
		objInfo->objDist = Math_Sqrt(minDist);
		objList->Add(objInfo);
		ret++;
	}
	return ret;
}

void Map::MapDrawLayer::FreeObjects(Data::ArrayList<ObjectInfo*> *objList)
{
	ObjectInfo *objInfo;
	UOSInt i;
	i = objList->GetCount();
	while (i-- > 0)
	{
		objInfo = objList->GetItem(i);
		MemFreeA(objInfo);
	}
	objList->Clear();
}

Map::VectorLayer *Map::MapDrawLayer::CreateEditableLayer()
{
	UTF8Char *sbuff = MemAlloc(UTF8Char, 65536);
	UTF8Char *sptr;
	UTF8Char *sptr2;
	const UTF8Char **sptrs;
	Map::VectorLayer *lyr;
	Data::ArrayListInt64 *objIds;
	Math::Geometry::Vector2D *vec;
	NameArray *nameArr;
	GetObjectSess *sess;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;


	NEW_CLASS(objIds, Data::ArrayListInt64());

	k = this->GetColumnCnt();
	i = k;
	sptrs = MemAlloc(const UTF8Char*, k);
	sptr = sbuff;
	while (i-- > 0)
	{
		sptr2 = this->GetColumnName(sptr, i);
		if (sptr2)
		{
			sptrs[i] = sptr;
			sptr = sptr2 + 1;
		}
		else
		{
			sptrs[i] = 0;
		}
	}
	Math::CoordinateSystem *csys = 0;
	if (this->csys)
	{
		csys = this->csys->Clone();
	}
	NEW_CLASS(lyr, Map::VectorLayer(this->GetLayerType(), this->sourceName, k, (const UTF8Char**)sptrs, csys, this->GetNameCol(), this->layerName));

	sess = this->BeginGetObject();
	this->GetAllObjectIds(objIds, &nameArr);
	i = 0;
	j = objIds->GetCount();
	while (i < j)
	{
		vec = this->GetNewVectorById(sess, objIds->GetItem(i));
		if (vec == 0)
		{
			vec = this->GetNewVectorById(sess, objIds->GetItem(i));
		}
		if (vec)
		{
			sptr = sbuff;
			l = k;
			while (l-- > 0)
			{
				sptr2 = this->GetString(sptr, 65536, nameArr, objIds->GetItem(i), l);
				if (sptr2)
				{
					sptrs[l] = sptr;
					sptr = sptr2 + 1;
				}
				else
				{
					sptrs[l] = 0;
				}
			}
			if (!lyr->AddVector(vec, sptrs))
			{
				DEL_CLASS(vec);
			}
		}

		i++;
	}

	MemFree(sptrs);
	MemFree(sbuff);
	DEL_CLASS(objIds);
	
	return lyr;
}

Text::SearchIndexer *Map::MapDrawLayer::CreateSearchIndexer(Text::TextAnalyzer *ta, UOSInt strIndex)
{
	if (strIndex >= this->GetColumnCnt())
		return 0;

	Text::SearchIndexer *searching;
	Data::ArrayListInt64 *objIds;
	NameArray *nameArr;
	UTF8Char sbuff[256];
	UOSInt i;

	NEW_CLASS(searching, Text::SearchIndexer(ta));
	NEW_CLASS(objIds, Data::ArrayListInt64());
	this->GetAllObjectIds(objIds, &nameArr);
	i = objIds->GetCount();
	while (i-- > 0)
	{
		if (this->GetString(sbuff, sizeof(sbuff), nameArr, objIds->GetItem(i), strIndex))
		{
			searching->IndexString(sbuff, objIds->GetItem(i));
		}
	}
	DEL_CLASS(objIds);
	this->ReleaseNameArr(nameArr);
	return searching;
}

UOSInt Map::MapDrawLayer::SearchString(Data::ArrayListString *outArr, Text::SearchIndexer *srchInd, NameArray *nameArr, const UTF8Char *srchStr, UOSInt maxResult, UOSInt strIndex)
{
	Data::ArrayListInt64 *objIds;
	NotNullPtr<Data::ArrayListICaseString> strList;
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Text::PString s;

	if (maxResult <= 0)
		return 0;

	NEW_CLASS(objIds, Data::ArrayListInt64());
	NEW_CLASSNN(strList, Data::ArrayListICaseString());
	srchInd->SearchString(objIds, srchStr, maxResult * 10);
	
	UOSInt i = 0;
	UOSInt j = objIds->GetCount();
	OSInt k;
	UOSInt resCnt = 0;
	while (i < j)
	{
		sptr = this->GetString(sbuff, sizeof(sbuff), nameArr, objIds->GetItem(i), strIndex);
		if (sptr)
		{
			s.v = sbuff;
			s.leng = (UOSInt)(sptr - sbuff);
			s.Trim();
			k = strList->SortedIndexOfPtr(s.v, s.leng);
			if (k < 0)
			{
				strList->Insert((UOSInt)~k, Text::String::New(s.v, s.leng).Ptr());
				if (++resCnt >= maxResult)
					break;
			}
		}
		
		i++;
	}

	outArr->AddAll(strList);
	strList.Delete();
	DEL_CLASS(objIds);
	return resCnt;
}

void Map::MapDrawLayer::ReleaseSearchStr(Data::ArrayListString *strArr)
{
	LIST_FREE_STRING(strArr);
}

Math::Geometry::Vector2D *Map::MapDrawLayer::GetVectorByStr(Text::SearchIndexer *srchInd, Map::NameArray *nameArr, Map::GetObjectSess *session, const UTF8Char *srchStr, UOSInt strIndex)
{
	UTF8Char sbuff[256];
	Data::ArrayListInt64 *objIds;
	Math::Geometry::Vector2D *vec = 0;

	NEW_CLASS(objIds, Data::ArrayListInt64());
	srchInd->SearchString(objIds, srchStr, 10000);

	UOSInt i = 0;
	UOSInt j = objIds->GetCount();
	while (i < j)
	{
		this->GetString(sbuff, sizeof(sbuff), nameArr, objIds->GetItem(i), strIndex);
		Text::StrTrim(sbuff);
		if (Text::StrCompareICase(srchStr, sbuff) == 0)
		{
			if (vec == 0)
			{
				vec = this->GetNewVectorById(session, objIds->GetItem(i));
			}
			else
			{
				Math::Geometry::Vector2D *tmpVec = this->GetNewVectorById(session, objIds->GetItem(i));
				vec->JoinVector(tmpVec);
				DEL_CLASS(tmpVec);
			}
		}
		i++;
	}

	DEL_CLASS(objIds);
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

void Map::MapDrawLayer::SetLineStyle(UInt32 lineColor, UInt32 lineWidth)
{
	this->lineColor = lineColor;
	this->lineWidth = lineWidth;
}

void Map::MapDrawLayer::SetPGStyle(UInt32 pgColor)
{
	this->pgColor = pgColor;
}

void Map::MapDrawLayer::SetIconStyle(Media::SharedImage *iconImg, OSInt iconSpotX, OSInt iconSpotY)
{
	SDEL_CLASS(this->iconImg);
	if (iconImg)
	{
		this->iconImg = iconImg->Clone();
	}
	this->iconSpotX = iconSpotX;
	this->iconSpotY = iconSpotY;
}

UInt32 Map::MapDrawLayer::GetLineStyleColor()
{
	return this->lineColor;
}

UInt32 Map::MapDrawLayer::GetLineStyleWidth()
{
	return this->lineWidth;
}

UInt32 Map::MapDrawLayer::GetPGStyleColor()
{
	return this->pgColor;
}

Media::SharedImage *Map::MapDrawLayer::GetIconStyleImg()
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
		return Map::DRAW_LAYER_POINT;
	case Math::Geometry::Vector2D::VectorType::Polygon:
		return Map::DRAW_LAYER_POLYGON;
	case Math::Geometry::Vector2D::VectorType::Polyline:
		return Map::DRAW_LAYER_POLYLINE;
	case Math::Geometry::Vector2D::VectorType::Image:
		return Map::DRAW_LAYER_IMAGE;
	case Math::Geometry::Vector2D::VectorType::MultiPoint:
	case Math::Geometry::Vector2D::VectorType::MultiPolygon:
	case Math::Geometry::Vector2D::VectorType::CurvePolygon:
	case Math::Geometry::Vector2D::VectorType::CompoundCurve:
	case Math::Geometry::Vector2D::VectorType::CircularString:
	case Math::Geometry::Vector2D::VectorType::LineString:
	case Math::Geometry::Vector2D::VectorType::GeometryCollection:
	case Math::Geometry::Vector2D::VectorType::MultiCurve:
	case Math::Geometry::Vector2D::VectorType::MultiSurface:
	case Math::Geometry::Vector2D::VectorType::Curve:
	case Math::Geometry::Vector2D::VectorType::Surface:
	case Math::Geometry::Vector2D::VectorType::PolyhedralSurface:
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
	return this->objIds->GetItem((UOSInt)this->currIndex);
}

Map::MapLayerReader::MapLayerReader(Map::MapDrawLayer *layer) : DB::DBReader()
{
	this->layer = layer;

	NEW_CLASS(this->objIds, Data::ArrayListInt64());
	this->layer->GetAllObjectIds(this->objIds, &this->nameArr);
	this->currIndex = -1;
}

Map::MapLayerReader::~MapLayerReader()
{
	DEL_CLASS(this->objIds);
	this->layer->ReleaseNameArr(this->nameArr);
}

Bool Map::MapLayerReader::ReadNext()
{
	this->currIndex++;
	if ((UOSInt)this->currIndex >= this->objIds->GetCount())
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
	UTF8Char sbuff[256];
	if (colIndex <= 0)
		return 0;
	this->layer->GetString(sbuff, sizeof(sbuff), this->nameArr, this->GetCurrObjId(), colIndex - 1);
	return Text::StrToInt32(sbuff);
}

Int64 Map::MapLayerReader::GetInt64(UOSInt colIndex)
{
	UTF8Char sbuff[256];
	if (colIndex == 0)
		return 0;
	this->layer->GetString(sbuff, sizeof(sbuff), this->nameArr, this->GetCurrObjId(), colIndex - 1);
	return Text::StrToInt64(sbuff);
}

WChar *Map::MapLayerReader::GetStr(UOSInt colIndex, WChar *buff)
{
	UTF8Char sbuff[256];
	if (colIndex <= 0)
	{
		Math::Geometry::Vector2D *vec = this->GetVector(0);
		if (vec == 0)
			return 0;
		Math::WKTWriter writer;
		Text::StringBuilderUTF8 sb;
		Bool succ = writer.ToText(&sb, vec);
		DEL_CLASS(vec);
		if (!succ)
		{
			return 0;
		}
		return Text::StrUTF8_WCharC(buff, sb.v, sb.leng, 0);
	}
	if (this->layer->GetString(sbuff, sizeof(sbuff), this->nameArr, this->GetCurrObjId(), colIndex - 1))
	{
		return Text::StrUTF8_WChar(buff, sbuff, 0);
	}
	return 0;
}

Bool Map::MapLayerReader::GetStr(UOSInt colIndex, Text::StringBuilderUTF8 *sb)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	if (colIndex <= 0)
	{
		Math::Geometry::Vector2D *vec = this->GetVector(0);
		if (vec == 0)
			return 0;
		Math::WKTWriter writer;
		Bool succ = writer.ToText(sb, vec);
		DEL_CLASS(vec);
		return succ;
	}
	sptr = this->layer->GetString(sbuff, sizeof(sbuff), this->nameArr, this->GetCurrObjId(), colIndex - 1);
	if (sptr == 0)
	{
		return false;
	}
	sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	return true;
}

Text::String *Map::MapLayerReader::GetNewStr(UOSInt colIndex)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	if (colIndex <= 0)
	{
		Math::Geometry::Vector2D *vec = this->GetVector(0);
		if (vec == 0)
			return 0;
		Math::WKTWriter writer;
		Text::StringBuilderUTF8 sb;
		Bool succ = writer.ToText(&sb, vec);
		DEL_CLASS(vec);
		if (!succ)
		{
			return 0;
		}
		return Text::String::New(sb.ToCString()).Ptr();
	}
	sptr = this->layer->GetString(sbuff, sizeof(sbuff), this->nameArr, this->GetCurrObjId(), colIndex - 1);
	return Text::String::NewP(sbuff, sptr).Ptr();
}

UTF8Char *Map::MapLayerReader::GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize)
{
	if (colIndex <= 0)
	{
		Math::Geometry::Vector2D *vec = this->GetVector(0);
		if (vec == 0)
			return 0;
		Math::WKTWriter writer;
		Text::StringBuilderUTF8 sb;
		Bool succ = writer.ToText(&sb, vec);
		DEL_CLASS(vec);
		if (!succ)
		{
			return 0;
		}
		return sb.ConcatToS(buff, buffSize);
	}
	return this->layer->GetString(buff, buffSize, this->nameArr, this->GetCurrObjId(), colIndex - 1);
}

Data::Timestamp Map::MapLayerReader::GetTimestamp(UOSInt colIndex)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	if (colIndex <= 0)
		return Data::Timestamp(0);
	sptr = this->layer->GetString(sbuff, sizeof(sbuff), this->nameArr, this->GetCurrObjId(), colIndex - 1);
	return Data::Timestamp(CSTRP(sbuff, sptr), Data::DateTimeUtil::GetLocalTzQhr());
}

Double Map::MapLayerReader::GetDbl(UOSInt colIndex)
{
	UTF8Char sbuff[256];
	if (colIndex <= 0)
		return 0;
	this->layer->GetString(sbuff, sizeof(sbuff), this->nameArr, this->GetCurrObjId(), colIndex - 1);
	return Text::StrToDouble(sbuff);
}

Bool Map::MapLayerReader::GetBool(UOSInt colIndex)
{
	UTF8Char sbuff[256];
	if (colIndex <= 0)
		return 0;
	this->layer->GetString(sbuff, sizeof(sbuff), this->nameArr, this->GetCurrObjId(), colIndex - 1);
	return Text::StrToBool(sbuff);
}

UOSInt Map::MapLayerReader::GetBinarySize(UOSInt colIndex)
{
	return 0;
}

UOSInt Map::MapLayerReader::GetBinary(UOSInt colIndex, UInt8 *buff)
{
	return 0;
}

Math::Geometry::Vector2D *Map::MapLayerReader::GetVector(UOSInt colIndex)
{
	if (colIndex != 0)
		return 0;
	if ((UOSInt)this->currIndex >= this->objIds->GetCount() || this->currIndex < 0)
		return 0;
	GetObjectSess *sess = this->layer->BeginGetObject();
	Math::Geometry::Vector2D *vec = this->layer->GetNewVectorById(sess, this->GetCurrObjId());
	this->layer->EndGetObject(sess);
	return vec;

}

Bool Map::MapLayerReader::GetUUID(UOSInt colIndex, Data::UUID *uuid)
{
	return false;
}
 
Bool Map::MapLayerReader::IsNull(UOSInt colIndex)
{
	UTF8Char sbuff[256];
	if (colIndex == 0)
		return false;
	return this->layer->GetString(sbuff, sizeof(sbuff), this->nameArr, this->GetCurrObjId(), colIndex - 1) == 0;
}

UTF8Char *Map::MapLayerReader::GetName(UOSInt colIndex, UTF8Char *buff)
{
	if (colIndex == 0)
		return Text::StrConcatC(buff, UTF8STRC("Shape"));
	return this->layer->GetColumnName(buff, colIndex - 1);
}

DB::DBUtil::ColType Map::MapLayerReader::GetColType(UOSInt colIndex, UOSInt *colSize)
{
	if (colIndex == 0)
		return DB::DBUtil::CT_Vector;
	return this->layer->GetColumnType(colIndex - 1, colSize);
}

Bool Map::MapLayerReader::GetColDef(UOSInt colIndex, DB::ColDef *colDef)
{
	return GetColDefV(colIndex, colDef, this->layer);
}

Bool Map::MapLayerReader::GetColDefV(UOSInt colIndex, DB::ColDef *colDef, Map::MapDrawLayer *layer)
{
	if (colIndex == 0)
	{
		Math::CoordinateSystem *csys = layer->GetCoordinateSystem();
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
		colDef->SetColDP(csys?csys->GetSRID():0);
		colDef->SetAttr(CSTR(""));
		colDef->SetDefVal(CSTR_NULL);
		colDef->SetAutoIncNone();
		colDef->SetNotNull(true);
		colDef->SetPK(false);
		return true;
	}
	return layer->GetColumnDef(colIndex - 1, colDef);
}
