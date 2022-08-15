#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListInt32.h"
#include "Data/ArrayListICaseString.h"
#include "Data/Sort/ArtificialQuickSortC.h"
#include "DB/ColDef.h"
#include "Math/Math.h"
#include "Math/Geometry/Point.h"
#include "Map/CIPLayer2.h"
#include "Map/IMapDrawLayer.h"
#include "Map/ProjectedMapView.h"
#include "Map/ScaledMapView.h"
#include "Map/SPDLayer.h"
#include "Map/VectorLayer.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"

OSInt __stdcall Map::IMapDrawLayer::ObjectCompare(void *obj1, void *obj2)
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

Map::IMapDrawLayer::IMapDrawLayer(Text::String *sourceName, UOSInt nameCol, Text::String *layerName) : DB::ReadingDB(sourceName)//IO::ParsedObject(sourceName)
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

Map::IMapDrawLayer::IMapDrawLayer(Text::CString sourceName, UOSInt nameCol, Text::CString layerName) : DB::ReadingDB(sourceName)//IO::ParsedObject(sourceName)
{
	this->nameCol = nameCol;
	this->layerName = Text::String::New(layerName.v, layerName.leng);
	this->csys = 0;

	this->pgColor = 0;
	this->lineColor = 0;
	this->lineWidth = 0;
	this->iconImg = 0;
	this->iconSpotX = 0;
	this->iconSpotY = 0;
	this->flags = 0;
}

Map::IMapDrawLayer::~IMapDrawLayer()
{
	SDEL_CLASS(this->csys);
	SDEL_CLASS(this->iconImg);
	SDEL_STRING(this->layerName);
}

void Map::IMapDrawLayer::SetCurrScale(Double scale)
{
}

void Map::IMapDrawLayer::SetCurrTimeTS(Int64 timeStamp)
{
}

Int64 Map::IMapDrawLayer::GetTimeStartTS()
{
	return 0;
}

Int64 Map::IMapDrawLayer::GetTimeEndTS()
{
	return 0;
}

Map::MapView *Map::IMapDrawLayer::CreateMapView(Math::Size2D<Double> scnSize)
{
	Map::MapView *view;
	Math::RectAreaDbl minMax;
	this->GetBounds(&minMax);
	if (minMax.br.x > 1000)
	{
		NEW_CLASS(view, Map::ProjectedMapView(scnSize, minMax.GetCenter(), 10000));
	}
	else
	{
		NEW_CLASS(view, Map::ScaledMapView(scnSize, minMax.GetCenter(), 10000));
	}
	return view;
}

void Map::IMapDrawLayer::SetMixedType(Math::Geometry::Vector2D::VectorType mixedType)
{
}

void Map::IMapDrawLayer::AddUpdatedHandler(UpdatedHandler hdlr, void *obj)
{
}

void Map::IMapDrawLayer::RemoveUpdatedHandler(UpdatedHandler hdlr, void *obj)
{
}

UOSInt Map::IMapDrawLayer::QueryTableNames(Text::CString schemaName, Data::ArrayList<Text::String*> *names)
{
	if (schemaName.leng != 0)
		return 0;
	names->Add(this->sourceName->Clone());
	return 1;
}

DB::DBReader *Map::IMapDrawLayer::QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayList<Text::String*> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	DB::DBReader *r;
	NEW_CLASS(r, Map::MapLayerReader(this));
	return r;
}

void Map::IMapDrawLayer::CloseReader(DB::DBReader *r)
{
	Map::MapLayerReader *rdr = (Map::MapLayerReader*)r;
	DEL_CLASS(rdr);
}

void Map::IMapDrawLayer::GetErrorMsg(Text::StringBuilderUTF8 *str)
{
}

void Map::IMapDrawLayer::Reconnect()
{
}

UOSInt Map::IMapDrawLayer::GetNameCol()
{
	return this->nameCol;
}

void Map::IMapDrawLayer::SetNameCol(UOSInt nameCol)
{
	this->nameCol = nameCol;
}

Text::String *Map::IMapDrawLayer::GetName()
{
	if (this->layerName && this->layerName->leng > 0)
	{
		return this->layerName;
	}
	else
	{
		return this->sourceName;
	}
}

IO::ParserType Map::IMapDrawLayer::GetParserType() const
{
	return IO::ParserType::MapLayer;
}

Math::CoordinateSystem *Map::IMapDrawLayer::GetCoordinateSystem()
{
	return this->csys;
}

void Map::IMapDrawLayer::SetCoordinateSystem(Math::CoordinateSystem *csys)
{
	SDEL_CLASS(this->csys);
	this->csys = csys;
}

Int32 Map::IMapDrawLayer::CalBlockSize()
{
	if (this->GetLayerType() == Map::DRAW_LAYER_POINT || this->GetLayerType() == Map::DRAW_LAYER_POINT3D)
	{
		return 5000;
	}
	else if (this->GetObjectClass() == Map::IMapDrawLayer::OC_CIP_LAYER)
	{
		return ((Map::CIPLayer2*)this)->GetBlockSize();
	}
	else if (this->GetObjectClass() == Map::IMapDrawLayer::OC_SPD_LAYER)
	{
		return ((Map::SPDLayer*)this)->GetBlockSize();
	}
	else if (this->GetObjectClass() == Map::IMapDrawLayer::OC_TILE_MAP_LAYER)
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

void Map::IMapDrawLayer::SetLayerName(Text::CString name)
{
	SDEL_STRING(this->layerName);
	this->layerName = Text::String::New(name);
}

Bool Map::IMapDrawLayer::IsError()
{
	return false;
}

UTF8Char *Map::IMapDrawLayer::GetPGLabel(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl coord, Math::Coord2DDbl *outCoord, UOSInt strIndex)
{
	UTF8Char *retVal = 0;

	if (this->GetLayerType() != DRAW_LAYER_POLYGON)
		return retVal;

	void *sess = BeginGetObject();
	void *names;
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
			Math::Geometry::Polygon *pg;
			pg = (Math::Geometry::Polygon*)this->GetNewVectorById(sess, thisId);
			if (pg)
			{
				if (pg->InsideVector(coord))
				{
					retVal = this->GetString(buff, buffSize, names, lastId, strIndex);
					if (buff != retVal)
					{
						*outCoord = coord;
						DEL_CLASS(pg);
						break;
					}
				}
				DEL_CLASS(pg);
			}
		}
	}

	DEL_CLASS(arr);
	ReleaseNameArr(names);
	EndGetObject(sess);
	return retVal;
}

UTF8Char *Map::IMapDrawLayer::GetPLLabel(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl coord, Math::Coord2DDbl *outCoord, UOSInt strIndex)
{
	UTF8Char *retVal = 0;
	UTF8Char *tmpBuff;

	if (this->GetLayerType() != DRAW_LAYER_POLYLINE && this->GetLayerType() != DRAW_LAYER_POINT)
		return retVal;

	void *sess = BeginGetObject();
	tmpBuff = MemAlloc(UTF8Char, buffSize);
	void *names;
	Data::ArrayListInt64 *arr;
	Int64 lastId;
	Int64 thisId;
	UOSInt i;
	DrawObjectL *dobj;
	Double dist = 1000.0;
	NEW_CLASS(arr, Data::ArrayListInt64());
	Int32 blkSize = this->CalBlockSize();

	Int32 xBlk = Double2Int32(coord.x * 200000.0 / blkSize);
	Int32 yBlk = Double2Int32(coord.y * 200000.0 / blkSize);
	GetObjectIds(arr, &names, 200000.0, Math::RectArea<Int32>((xBlk - 1) * blkSize, (yBlk - 1) * blkSize, 3 * blkSize - 1, 3 * blkSize - 1), false);
	lastId = -1;
	i = arr->GetCount();
	while (i-- > 0)
	{
		thisId = arr->GetItem(i);
		if (thisId != lastId)
		{
			lastId = thisId;
			dobj = this->GetNewObjectById(sess, thisId);
			UTF8Char *sptr = this->GetString(tmpBuff, buffSize, names, thisId, strIndex);
			if (sptr && sptr != tmpBuff)
			{
				UInt32 k;
				UInt32 l;
				UInt32 m;
				UInt32 *ptOfstArr;
				Math::Coord2DDbl *pointArr;
				Int32 currFound;

				ptOfstArr = dobj->ptOfstArr;
				pointArr = dobj->pointArr;

				k = dobj->nPtOfst;
				l = dobj->nPoint;

				currFound = 0;
				Double calBase;
				Math::Coord2DDbl calDiff;
				Math::Coord2DDbl calSqDiff;
				Double calX;
				Double calY;
				Double calD;
				Double calPtX;
				Double calPtY;

				while (k--)
				{
					m = ptOfstArr[k];
					l--;
					while (l-- > m)
					{
						calDiff = pointArr[l] - pointArr[l + 1];

						if (calDiff.y == 0)
						{
							calX = coord.x;
						}
						else
						{
							calSqDiff = calDiff * calDiff;
							calX = (calBase = calSqDiff.x) * coord.x;
							calBase += calSqDiff.y;
							calX += calSqDiff.y * pointArr[l].x;
							calX += (coord.y - pointArr[l].y) * calDiff.y * calDiff.x;
							calX /= calBase;
						}

						if (calDiff.x == 0)
						{
							calY = coord.y;
						}
						else
						{
							calY = ((calX - pointArr[l].x) * calDiff.y / calDiff.x) + pointArr[l].y;
						}

						if (calDiff.x < 0)
						{
							if (pointArr[l].x > calX)
								continue;
							if (pointArr[l + 1].x < calX)
								continue;
						}
						else
						{
							if (pointArr[l].x < calX)
								continue;
							if (pointArr[l + 1].x > calX)
								continue;
						}

						if (calDiff.y < 0)
						{
							if (pointArr[l].y > calY)
								continue;
							if (pointArr[l + 1].y < calY)
								continue;
						}
						else
						{
							if (pointArr[l].y < calY)
								continue;
							if (pointArr[l + 1].y > calY)
								continue;
						}

						calDiff = coord - Math::Coord2DDbl(Math::Coord2DDbl(calX, calY));
						calSqDiff = calDiff * calDiff;
						calD = (calSqDiff.x + calSqDiff.y);
						if (calD < dist)
						{
							currFound = 1;
							dist = calD;
							calPtX = calX;
							calPtY = calY;
						}
					}
				}
				if (currFound)
				{
					retVal = Text::StrConcat(buff, tmpBuff);
					if (outCoord)
					{
						*outCoord = Math::Coord2DDbl(calPtX, calPtY);
					}
				}
			}
			this->ReleaseObject(sess, dobj);
		}
	}

	DEL_CLASS(arr);
	ReleaseNameArr(names);
	EndGetObject(sess);
	MemFree(tmpBuff);
	return retVal;
}

Int64 Map::IMapDrawLayer::GetNearestObjectId(void *session, Math::Coord2DDbl pt, Math::Coord2DDbl *nearPt)
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

OSInt Map::IMapDrawLayer::GetNearObjects(void *session, Data::ArrayList<ObjectInfo*> *objList, Math::Coord2DDbl pt, Double maxDist)
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

void Map::IMapDrawLayer::FreeObjects(Data::ArrayList<ObjectInfo*> *objList)
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

Map::VectorLayer *Map::IMapDrawLayer::CreateEditableLayer()
{
	UTF8Char *sbuff = MemAlloc(UTF8Char, 65536);
	UTF8Char *sptr;
	UTF8Char *sptr2;
	const UTF8Char **sptrs;
	Map::VectorLayer *lyr;
	Data::ArrayListInt64 *objIds;
	Math::Geometry::Vector2D *vec;
	void *nameArr;
	void *sess;
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

Text::SearchIndexer *Map::IMapDrawLayer::CreateSearchIndexer(Text::TextAnalyzer *ta, UOSInt strIndex)
{
	if (strIndex >= this->GetColumnCnt())
		return 0;

	Text::SearchIndexer *searching;
	Data::ArrayListInt64 *objIds;
	void *nameArr;
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

UOSInt Map::IMapDrawLayer::SearchString(Data::ArrayListString *outArr, Text::SearchIndexer *srchInd, void *nameArr, const UTF8Char *srchStr, UOSInt maxResult, UOSInt strIndex)
{
	Data::ArrayListInt64 *objIds;
	Data::ArrayListICaseString *strList;
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Text::PString s;

	if (maxResult <= 0)
		return 0;

	NEW_CLASS(objIds, Data::ArrayListInt64());
	NEW_CLASS(strList, Data::ArrayListICaseString());
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
				strList->Insert((UOSInt)~k, Text::String::New(s.v, s.leng));
				if (++resCnt >= maxResult)
					break;
			}
		}
		
		i++;
	}

	outArr->AddAll(strList);
	DEL_CLASS(strList);
	DEL_CLASS(objIds);
	return resCnt;
}

void Map::IMapDrawLayer::ReleaseSearchStr(Data::ArrayListString *strArr)
{
	LIST_FREE_STRING(strArr);
}

Math::Geometry::Vector2D *Map::IMapDrawLayer::GetVectorByStr(Text::SearchIndexer *srchInd, void *nameArr, void *session, const UTF8Char *srchStr, UOSInt strIndex)
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

Bool Map::IMapDrawLayer::HasLineStyle()
{
	return this->lineWidth > 0;
}

Bool Map::IMapDrawLayer::HasPGStyle()
{
	return this->pgColor != 0;
}

Bool Map::IMapDrawLayer::HasIconStyle()
{
	return this->iconImg != 0;
}

void Map::IMapDrawLayer::SetLineStyle(UInt32 lineColor, UInt32 lineWidth)
{
	this->lineColor = lineColor;
	this->lineWidth = lineWidth;
}

void Map::IMapDrawLayer::SetPGStyle(UInt32 pgColor)
{
	this->pgColor = pgColor;
}

void Map::IMapDrawLayer::SetIconStyle(Media::SharedImage *iconImg, OSInt iconSpotX, OSInt iconSpotY)
{
	SDEL_CLASS(this->iconImg);
	if (iconImg)
	{
		this->iconImg = iconImg->Clone();
	}
	this->iconSpotX = iconSpotX;
	this->iconSpotY = iconSpotY;
}

UInt32 Map::IMapDrawLayer::GetLineStyleColor()
{
	return this->lineColor;
}

UInt32 Map::IMapDrawLayer::GetLineStyleWidth()
{
	return this->lineWidth;
}

UInt32 Map::IMapDrawLayer::GetPGStyleColor()
{
	return this->pgColor;
}

Media::SharedImage *Map::IMapDrawLayer::GetIconStyleImg()
{
	return this->iconImg;
}

OSInt Map::IMapDrawLayer::GetIconStyleSpotX()
{
	return this->iconSpotX;
}

OSInt Map::IMapDrawLayer::GetIconStyleSpotY()
{
	return this->iconSpotY;
}

Bool Map::IMapDrawLayer::IsLabelVisible()
{
	return this->flags & 1;
}

void Map::IMapDrawLayer::SetLabelVisible(Bool labelVisible)
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

Map::DrawLayerType Map::IMapDrawLayer::VectorType2LayerType(Math::Geometry::Vector2D::VectorType vtype)
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

Map::DrawObjectL *Map::IMapDrawLayer::Vector2DrawObject(Int64 id, Math::Geometry::Vector2D *vec, Map::DrawLayerType layerType)
{
	if (layerType == Map::DRAW_LAYER_POINT && vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::Point)
	{
		Math::Geometry::Point *pt = (Math::Geometry::Point*)vec;
		Map::DrawObjectL *dobj;
		dobj = MemAlloc(Map::DrawObjectL, 1);
		dobj->objId = id;
		dobj->nPtOfst = 1;
		dobj->nPoint = 1;
		dobj->ptOfstArr = MemAlloc(UInt32, 1);
		dobj->ptOfstArr[0] = 0;
		dobj->pointArr = MemAllocA(Math::Coord2DDbl, 1);
		dobj->pointArr[0] = pt->GetCenter();
		dobj->flags = 0;
		dobj->lineColor = 0;
		return dobj;
	}
	else if (layerType == Map::DRAW_LAYER_POLYGON || layerType == Map::DRAW_LAYER_POLYLINE || layerType == Map::DRAW_LAYER_POLYLINE3D)
	{
		Math::Geometry::PointOfstCollection *ptColl = (Math::Geometry::PointOfstCollection*)vec;
		UInt32 *ptOfstArr;
		Math::Coord2DDbl *ptArr;
		UOSInt cnt;

		Map::DrawObjectL *dobj;
		dobj = MemAlloc(Map::DrawObjectL, 1);
		dobj->objId = id;

		ptOfstArr = ptColl->GetPtOfstList(&cnt);
		dobj->nPtOfst = (UInt32)cnt;
		dobj->ptOfstArr = MemAlloc(UInt32, cnt);
		MemCopyNO(dobj->ptOfstArr, ptOfstArr, sizeof(UInt32) * cnt);

		ptArr = ptColl->GetPointList(&cnt);
		dobj->nPoint = (UInt32)cnt;
		dobj->pointArr = MemAllocA(Math::Coord2DDbl, cnt);
		MemCopyNO(dobj->pointArr, ptArr, cnt * sizeof(Math::Coord2DDbl));
		dobj->flags = 0;
		dobj->lineColor = 0;
		return dobj;
	}
	else
	{
		return 0;
	}
}

Int64 Map::MapLayerReader::GetCurrObjId()
{
	return this->objIds->GetItem((UOSInt)this->currIndex);
}

Map::MapLayerReader::MapLayerReader(Map::IMapDrawLayer *layer) : DB::DBReader()
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
	if (colIndex <= 0)
		return 0;
	this->layer->GetString(sbuff, sizeof(sbuff), this->nameArr, this->GetCurrObjId(), colIndex - 1);
	return Text::StrToInt64(sbuff);
}

WChar *Map::MapLayerReader::GetStr(UOSInt colIndex, WChar *buff)
{
	UTF8Char sbuff[256];
	if (colIndex <= 0)
		return 0;
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
		return false;
	sptr = this->layer->GetString(sbuff, sizeof(sbuff), this->nameArr, this->GetCurrObjId(), colIndex - 1);
	sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	return true;
}

Text::String *Map::MapLayerReader::GetNewStr(UOSInt colIndex)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	if (colIndex <= 0)
		return 0;
	sptr = this->layer->GetString(sbuff, sizeof(sbuff), this->nameArr, this->GetCurrObjId(), colIndex - 1);
	return Text::String::NewP(sbuff, sptr);
}

UTF8Char *Map::MapLayerReader::GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize)
{
	if (colIndex <= 0)
		return 0;
	return this->layer->GetString(buff, buffSize, this->nameArr, this->GetCurrObjId(), colIndex - 1);
}

Data::Timestamp Map::MapLayerReader::GetTimestamp(UOSInt colIndex)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	if (colIndex <= 0)
		return Data::Timestamp(0, 0);
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
	void *sess = this->layer->BeginGetObject();
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
	if (colIndex == 0)
	{
		colDef->SetColType(DB::DBUtil::CT_Vector);
		colDef->SetColName(CSTR("Shape"));
		colDef->SetColDP(0);
		colDef->SetColSize(0x7fffffff);
		colDef->SetAttr(CSTR(""));
		colDef->SetDefVal(CSTR_NULL);
		colDef->SetAutoInc(false);
		colDef->SetNotNull(true);
		colDef->SetPK(false);
		return true;
	}
	return this->layer->GetColumnDef(colIndex - 1, colDef);
}
