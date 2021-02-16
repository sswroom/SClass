#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListInt32.h"
#include "Data/ArrayListICaseStrUTF8.h"
#include "Data/Sort/ArtificialQuickSortC.h"
#include "DB/ColDef.h"
#include "Math/Math.h"
#include "Math/Point.h"
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

Map::IMapDrawLayer::IMapDrawLayer(const UTF8Char *sourceName, OSInt nameCol, const UTF8Char *layerName) : DB::ReadingDB(sourceName)//IO::ParsedObject(sourceName)
{
	this->nameCol = nameCol;
	if (layerName)
	{
		this->layerName = Text::StrCopyNew(layerName);
	}
	else
	{
		this->layerName = 0;
	}
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
	SDEL_TEXT(this->layerName);
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

Map::MapView *Map::IMapDrawLayer::CreateMapView(OSInt width, OSInt height)
{
	Map::MapView *view;
	Double xMin;
	Double yMin;
	Double xMax;
	Double yMax;
	this->GetBoundsDbl(&xMin, &yMin, &xMax, &yMax);
	if (xMax > 1000)
	{
		NEW_CLASS(view, Map::ProjectedMapView(width, height, (yMin + yMax) * 0.5, (xMin + xMax) * 0.5, 10000));
	}
	else
	{
		NEW_CLASS(view, Map::ScaledMapView(width, height, (yMin + yMax) * 0.5, (xMin + xMax) * 0.5, 10000));
	}
	return view;
}

void Map::IMapDrawLayer::SetMixedType(DrawLayerType mixedType)
{
}

void Map::IMapDrawLayer::AddUpdatedHandler(UpdatedHandler hdlr, void *obj)
{
}

void Map::IMapDrawLayer::RemoveUpdatedHandler(UpdatedHandler hdlr, void *obj)
{
}

UOSInt Map::IMapDrawLayer::GetTableNames(Data::ArrayList<const UTF8Char*> *names)
{
	names->Add(this->sourceName);
	return 1;
}

DB::DBReader *Map::IMapDrawLayer::GetTableData(const UTF8Char *name, UOSInt maxCnt, void *ordering, void *condition)
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

void Map::IMapDrawLayer::GetErrorMsg(Text::StringBuilderUTF *str)
{
}

void Map::IMapDrawLayer::Reconnect()
{
}

OSInt Map::IMapDrawLayer::GetNameCol()
{
	return this->nameCol;
}

void Map::IMapDrawLayer::SetNameCol(OSInt nameCol)
{
	this->nameCol = nameCol;
}

const UTF8Char *Map::IMapDrawLayer::GetName()
{
	if (this->layerName)
	{
		return this->layerName;
	}
	else
	{
		return this->sourceName;
	}
}

IO::ParsedObject::ParserType Map::IMapDrawLayer::GetParserType()
{
	return IO::ParsedObject::PT_MAP_LAYER_PARSER;
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
		Double xMin;
		Double yMin;
		Double xMax;
		Double yMax;
		this->GetBoundsDbl(&xMin, &yMin, &xMax, &yMax);
		
		NEW_CLASS(idList, Data::ArrayListInt64());
		this->GetAllObjectIds(idList, 0);
		
		Double tVal = (yMax - yMin) * (xMax - xMin) / idList->GetCount();
		if (xMax > 180)
		{
			blkSize = Math::Double2Int32(Math::Sqrt(tVal) * 3);
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
			blkSize = Math::Double2Int32(Math::Sqrt(tVal) * 500000);
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

UTF8Char *Map::IMapDrawLayer::GetPGLabelLatLon(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, Double *outLat, Double *outLon, Int32 strIndex)
{
	UTF8Char *retVal = 0;

	if (this->GetLayerType() != DRAW_LAYER_POLYGON)
		return retVal;

	void *sess = BeginGetObject();
	void *names;
	Data::ArrayListInt64 *arr;
	Int64 lastId;
	OSInt i;
	Int64 thisId;
	NEW_CLASS(arr, Data::ArrayListInt64());
	GetObjectIdsMapXY(arr, &names, lon, lat, lon, lat, false);
	lastId = -1;
	i = arr->GetCount();
	while (i-- > 0)
	{
		thisId = arr->GetItem(i);
		if (thisId != lastId)
		{
			lastId = thisId;
			Math::Polygon *pg;
			pg = (Math::Polygon*)this->GetVectorById(sess, thisId);
			if (pg)
			{
				if (pg->InsideVector(lon, lat))
				{
					retVal = this->GetString(buff, buffSize, names, lastId, strIndex);
					if (buff != retVal)
					{
						*outLat = lat;
						*outLon = lon;
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

UTF8Char *Map::IMapDrawLayer::GetPLLabelLatLon(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, Double *outLat, Double *outLon, Int32 strIndex)
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
	OSInt i;
	DrawObjectL *dobj;
	Double dist = 1000.0;
	NEW_CLASS(arr, Data::ArrayListInt64());
	Int32 blkSize = this->CalBlockSize();

	Int32 xBlk = Math::Double2Int32(lon * 200000.0 / blkSize);
	Int32 yBlk = Math::Double2Int32(lat * 200000.0 / blkSize);
	GetObjectIds(arr, &names, 200000.0, (xBlk - 1) * blkSize, (yBlk - 1) * blkSize, (xBlk + 2) * blkSize - 1, (yBlk + 2) * blkSize - 1, false);
	lastId = -1;
	i = arr->GetCount();
	while (i-- > 0)
	{
		thisId = arr->GetItem(i);
		if (thisId != lastId)
		{
			lastId = thisId;
			dobj = this->GetObjectByIdD(sess, thisId);
			UTF8Char *sptr = this->GetString(tmpBuff, buffSize, names, thisId, strIndex);
			if (sptr && sptr != tmpBuff)
			{
				UInt32 k;
				UInt32 l;
				UInt32 m;
				UInt32 *ptOfstArr;
				Double *pointArr;
				Int32 currFound;
				Double mapX = lon;
				Double mapY = lat;

				ptOfstArr = dobj->ptOfstArr;
				pointArr = dobj->pointArr;

				k = dobj->nPtOfst;
				l = dobj->nPoint;

				currFound = 0;
				Double calBase;
				Double calH;
				Double calW;
				Double calX;
				Double calY;
				Double calD;
				Double calPtLat;
				Double calPtLon;

				while (k--)
				{
					m = ptOfstArr[k];
					l--;
					while (l-- > m)
					{
						calH = pointArr[(l << 1) + 1] - pointArr[(l << 1) + 3];
						calW = pointArr[(l << 1) + 0] - pointArr[(l << 1) + 2];

						if (calH == 0)
						{
							calX = mapX;
						}
						else
						{
							calX = (calBase = (calW * calW)) * mapX;
							calBase += calH * calH;
							calX += calH * calH * (pointArr[(l << 1) + 0]);
							calX += (mapY - pointArr[(l << 1) + 1]) * calH * calW;
							calX /= calBase;
						}

						if (calW == 0)
						{
							calY = (float)mapY;
						}
						else
						{
							calY = ((calX - (pointArr[(l << 1) + 0])) * calH / calW) + pointArr[(l << 1) + 1];
						}

						if (calW < 0)
						{
							if (pointArr[(l << 1) + 0] > calX)
								continue;
							if (pointArr[(l << 1) + 2] < calX)
								continue;
						}
						else
						{
							if (pointArr[(l << 1) + 0] < calX)
								continue;
							if (pointArr[(l << 1) + 2] > calX)
								continue;
						}

						if (calH < 0)
						{
							if (pointArr[(l << 1) + 1] > calY)
								continue;
							if (pointArr[(l << 1) + 3] < calY)
								continue;
						}
						else
						{
							if (pointArr[(l << 1) + 1] < calY)
								continue;
							if (pointArr[(l << 1) + 3] > calY)
								continue;
						}

						calH = mapY - calY;
						calW = mapX - calX;
						calD = (calW * calW + calH * calH);
						if (calD < dist)
						{
							currFound = 1;
							dist = calD;
							calPtLat = calY;
							calPtLon = calX;
						}
					}
				}
				if (currFound)
				{
					retVal = Text::StrConcat(buff, tmpBuff);
					if (outLat)
					{
						*outLat = calPtLat;
						*outLon = calPtLon;
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

Int64 Map::IMapDrawLayer::GetNearestObjectId(void *session, Double x, Double y, Double *pointX, Double *pointY)
{
	Data::ArrayListInt64 *objIds;
	NEW_CLASS(objIds, Data::ArrayListInt64());
	Int32 blkSize = this->CalBlockSize();
	if (x > 180 || x < -180)
	{
		this->GetObjectIdsMapXY(objIds, 0, x - blkSize, y - blkSize, x + blkSize, y + blkSize, true);
	}
	else
	{
		this->GetObjectIdsMapXY(objIds, 0, x - (blkSize / 200000.0), y - (blkSize / 200000.0), x + (blkSize / 200000.0), y + (blkSize / 200000.0), true);
	}

	OSInt i = objIds->GetCount();
	Int64 nearObjId = -1;
	Double minDist = 0x7fffffff;
	Double dist;
	Double currPtX;
	Double currPtY;
	Double nearPtX = 0;
	Double nearPtY = 0;

	while (i-- > 0)
	{
		Math::Vector2D *vec = this->GetVectorById(session, objIds->GetItem(i));
		if (vec)
		{
			dist = vec->CalSqrDistance(x, y, &currPtX, &currPtY);
			if (dist < minDist)
			{
				nearObjId = objIds->GetItem(i);
				nearPtX = currPtX;
				nearPtY = currPtY;
				minDist = dist;
			}
			DEL_CLASS(vec);
		}
	}

	DEL_CLASS(objIds);
	if (pointX && pointY)
	{
		*pointX = nearPtX;
		*pointY = nearPtY;
	}
	return nearObjId;
}

OSInt Map::IMapDrawLayer::GetNearObjects(void *session, Data::ArrayList<ObjectInfo*> *objList, Double x, Double y, Double maxDist)
{
	Data::ArrayListInt64 *objIds;
	NEW_CLASS(objIds, Data::ArrayListInt64());
	Int32 blkSize = this->CalBlockSize();
	if (x > 180 || x < -180)
	{
		this->GetObjectIdsMapXY(objIds, 0, x - blkSize, y - blkSize, x + blkSize, y + blkSize, true);
	}
	else
	{
		this->GetObjectIdsMapXY(objIds, 0, x - (blkSize / 200000.0), y - (blkSize / 200000.0), x + (blkSize / 200000.0), y + (blkSize / 200000.0), true);
	}

	UOSInt i = objIds->GetCount();
	Int64 nearObjId = -1;
	Double minDist = 0x7fffffff;
	Double dist;
	Double currPtX;
	Double currPtY;
	Double nearPtX = 0;
	Double nearPtY = 0;
	Double sqrMaxDist = maxDist * maxDist;
	ObjectInfo *objInfo;
	OSInt ret = 0;

	while (i-- > 0)
	{
		Math::Vector2D *vec = this->GetVectorById(session, objIds->GetItem(i));
		dist = vec->CalSqrDistance(x, y, &currPtX, &currPtY);
		if (dist <= sqrMaxDist)
		{
			objInfo = MemAlloc(ObjectInfo, 1);
			objInfo->objId = objIds->GetItem(i);
			objInfo->objX = currPtX;
			objInfo->objY = currPtY;
			objInfo->objDist = Math::Sqrt(dist);
			objList->Add(objInfo);
			ret++;
		}
		if (dist < minDist)
		{
			nearObjId = objIds->GetItem(i);
			nearPtX = currPtX;
			nearPtY = currPtY;
			minDist = dist;
		}
		DEL_CLASS(vec);
	}

	DEL_CLASS(objIds);

	if (ret > 0)
	{
		void **arr = (void**)objList->GetArray(&i);
		ArtificialQuickSort_SortCmp(arr, ObjectCompare, 0, i - 1);
	}
	else if (nearObjId != -1)
	{
		objInfo = MemAlloc(ObjectInfo, 1);
		objInfo->objId = nearObjId;
		objInfo->objX = nearPtX;
		objInfo->objY = nearPtY;
		objInfo->objDist = Math::Sqrt(minDist);
		objList->Add(objInfo);
		ret++;
	}
	return ret;
}

void Map::IMapDrawLayer::FreeObjects(Data::ArrayList<ObjectInfo*> *objList)
{
	ObjectInfo *objInfo;
	OSInt i;
	i = objList->GetCount();
	while (i-- > 0)
	{
		objInfo = objList->GetItem(i);
		MemFree(objInfo);
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
	Math::Vector2D *vec;
	void *nameArr;
	void *sess;
	OSInt i;
	OSInt j;
	OSInt k;
	OSInt l;


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
		vec = this->GetVectorById(sess, objIds->GetItem(i));
		if (vec == 0)
		{
			vec = this->GetVectorById(sess, objIds->GetItem(i));
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

UOSInt Map::IMapDrawLayer::SearchString(Data::ArrayListStrUTF8 *outArr, Text::SearchIndexer *srchInd, void *nameArr, const UTF8Char *srchStr, UOSInt maxResult, UOSInt strIndex)
{
	Data::ArrayListInt64 *objIds;
	Data::ArrayListICaseStrUTF8 *strList;
	UTF8Char sbuff[256];

	if (maxResult <= 0)
		return 0;

	NEW_CLASS(objIds, Data::ArrayListInt64());
	NEW_CLASS(strList, Data::ArrayListICaseStrUTF8());
	srchInd->SearchString(objIds, srchStr, maxResult * 10);
	
	UOSInt i = 0;
	UOSInt j = objIds->GetCount();
	OSInt k;
	UOSInt resCnt = 0;
	while (i < j)
	{
		this->GetString(sbuff, sizeof(sbuff), nameArr, objIds->GetItem(i), strIndex);
		Text::StrTrim(sbuff);
		k = strList->SortedIndexOf(sbuff);
		if (k < 0)
		{
			strList->Insert(~k, Text::StrCopyNew(sbuff));
			if (++resCnt >= maxResult)
				break;
		}
		
		i++;
	}

	outArr->AddRange(strList);
	DEL_CLASS(strList);
	DEL_CLASS(objIds);
	return resCnt;
}

void Map::IMapDrawLayer::ReleaseSearchStr(Data::ArrayListStrUTF8 *strArr)
{
	UOSInt i = strArr->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(strArr->RemoveAt(i));
	}
}

Math::Vector2D *Map::IMapDrawLayer::GetVectorByStr(Text::SearchIndexer *srchInd, void *nameArr, void *session, const UTF8Char *srchStr, UOSInt strIndex)
{
	UTF8Char sbuff[256];
	Data::ArrayListInt64 *objIds;
	Math::Vector2D *vec = 0;

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
				vec = this->GetVectorById(session, objIds->GetItem(i));
			}
			else
			{
				Math::Vector2D *tmpVec = this->GetVectorById(session, objIds->GetItem(i));
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

void Map::IMapDrawLayer::SetLineStyle(Int32 lineColor, Int32 lineWidth)
{
	this->lineColor = lineColor;
	this->lineWidth = lineWidth;
}

void Map::IMapDrawLayer::SetPGStyle(Int32 pgColor)
{
	this->pgColor = pgColor;
}

void Map::IMapDrawLayer::SetIconStyle(Media::ImageList *iconImg, OSInt iconSpotX, OSInt iconSpotY)
{
	SDEL_CLASS(this->iconImg);
	this->iconImg = iconImg;
	this->iconSpotX = iconSpotX;
	this->iconSpotY = iconSpotY;
}

Int32 Map::IMapDrawLayer::GetLineStyleColor()
{
	return this->lineColor;
}

Int32 Map::IMapDrawLayer::GetLineStyleWidth()
{
	return this->lineWidth;
}

Int32 Map::IMapDrawLayer::GetPGStyleColor()
{
	return this->pgColor;
}

Media::ImageList *Map::IMapDrawLayer::GetIconStyleImg()
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

Map::DrawLayerType Map::IMapDrawLayer::VectorType2LayerType(Math::Vector2D::VectorType vtype)
{
	switch (vtype)
	{
	case Math::Vector2D::VT_POINT:
		return Map::DRAW_LAYER_POINT;
	case Math::Vector2D::VT_POLYGON:
		return Map::DRAW_LAYER_POLYGON;
	case Math::Vector2D::VT_POLYLINE:
		return Map::DRAW_LAYER_POLYLINE;
	case Math::Vector2D::VT_IMAGE:
		return Map::DRAW_LAYER_IMAGE;
	default:
		return Map::DRAW_LAYER_UNKNOWN;
	}
}

Map::DrawObjectL *Map::IMapDrawLayer::Vector2DrawObject(Int64 id, Math::Vector2D *vec, Map::DrawLayerType layerType)
{
	if (layerType == Map::DRAW_LAYER_POINT && vec->GetVectorType() == Math::Vector2D::VT_POINT)
	{
		Math::Point *pt = (Math::Point*)vec;
		Map::DrawObjectL *dobj;
		dobj = MemAlloc(Map::DrawObjectL, 1);
		dobj->objId = id;
		dobj->nPtOfst = 1;
		dobj->nPoint = 1;
		dobj->ptOfstArr = MemAlloc(UInt32, 1);
		dobj->ptOfstArr[0] = 0;
		dobj->pointArr = MemAlloc(Double, 2);
		pt->GetCenter(&dobj->pointArr[0], &dobj->pointArr[1]);
		dobj->flags = 0;
		dobj->lineColor = 0;
		return dobj;
	}
	else if (layerType == Map::DRAW_LAYER_POLYGON || layerType == Map::DRAW_LAYER_POLYLINE || layerType == Map::DRAW_LAYER_POLYLINE3D)
	{
		Math::PointCollection *ptColl = (Math::PointCollection*)vec;
		UInt32 *ptOfstArr;
		Double *ptArr;
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
		cnt <<= 1;
		dobj->pointArr = MemAlloc(Double, cnt);
		MemCopyNO(dobj->pointArr, ptArr, cnt << 3);
		dobj->flags = 0;
		dobj->lineColor = 0;
		return dobj;
	}
	else
	{
		return 0;
	}
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
	this->layer->GetString(sbuff, sizeof(sbuff), this->nameArr, this->objIds->GetItem(this->currIndex), colIndex - 1);
	return Text::StrToInt32(sbuff);
}

Int64 Map::MapLayerReader::GetInt64(UOSInt colIndex)
{
	UTF8Char sbuff[256];
	if (colIndex <= 0)
		return 0;
	this->layer->GetString(sbuff, sizeof(sbuff), this->nameArr, this->objIds->GetItem(this->currIndex), colIndex - 1);
	return Text::StrToInt64(sbuff);
}

WChar *Map::MapLayerReader::GetStr(UOSInt colIndex, WChar *buff)
{
	UTF8Char sbuff[256];
	if (colIndex <= 0)
		return 0;
	if (this->layer->GetString(sbuff, sizeof(sbuff), this->nameArr, this->objIds->GetItem(this->currIndex), colIndex - 1))
	{
		return Text::StrUTF8_WChar(buff, sbuff, -1, 0);
	}
	return 0;
}

Bool Map::MapLayerReader::GetStr(UOSInt colIndex, Text::StringBuilderUTF *sb)
{
	UTF8Char sbuff[256];
	if (colIndex <= 0)
		return false;
	this->layer->GetString(sbuff, sizeof(sbuff), this->nameArr, this->objIds->GetItem(this->currIndex), colIndex - 1);
	sb->Append(sbuff);
	return true;
}

const UTF8Char *Map::MapLayerReader::GetNewStr(UOSInt colIndex)
{
	UTF8Char sbuff[256];
	if (colIndex <= 0)
		return 0;
	this->layer->GetString(sbuff, sizeof(sbuff), this->nameArr, this->objIds->GetItem(this->currIndex), colIndex - 1);
	return Text::StrCopyNew(sbuff);
}

UTF8Char *Map::MapLayerReader::GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize)
{
	if (colIndex <= 0)
		return 0;
	return this->layer->GetString(buff, buffSize, this->nameArr, this->objIds->GetItem(this->currIndex), colIndex - 1);
}

DB::DBReader::DateErrType Map::MapLayerReader::GetDate(UOSInt colIndex, Data::DateTime *outVal)
{
	UTF8Char sbuff[256];
	if (colIndex <= 0)
		return DB::DBReader::DET_ERROR;
	this->layer->GetString(sbuff, sizeof(sbuff), this->nameArr, this->objIds->GetItem(this->currIndex), colIndex - 1);
	outVal->SetValue(sbuff);
	return DB::DBReader::DET_OK;
}

Double Map::MapLayerReader::GetDbl(UOSInt colIndex)
{
	UTF8Char sbuff[256];
	if (colIndex <= 0)
		return 0;
	this->layer->GetString(sbuff, sizeof(sbuff), this->nameArr, this->objIds->GetItem(this->currIndex), colIndex - 1);
	return Text::StrToDouble(sbuff);
}

Bool Map::MapLayerReader::GetBool(UOSInt colIndex)
{
	UTF8Char sbuff[256];
	if (colIndex <= 0)
		return 0;
	this->layer->GetString(sbuff, sizeof(sbuff), this->nameArr, this->objIds->GetItem(this->currIndex), colIndex - 1);
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

Math::Vector2D *Map::MapLayerReader::GetVector(UOSInt colIndex)
{
	if (colIndex != 0)
		return 0;
	if ((UOSInt)this->currIndex >= this->objIds->GetCount() || this->currIndex < 0)
		return 0;
	void *sess = this->layer->BeginGetObject();
	Math::Vector2D *vec = this->layer->GetVectorById(sess, this->objIds->GetItem(this->currIndex));
	this->layer->EndGetObject(sess);
	return vec;

}

Bool Map::MapLayerReader::IsNull(UOSInt colIndex)
{
	UTF8Char sbuff[256];
	if (colIndex == 0)
		return false;
	return this->layer->GetString(sbuff, sizeof(sbuff), this->nameArr, this->objIds->GetItem(this->currIndex), colIndex - 1) == 0;
}

UTF8Char *Map::MapLayerReader::GetName(UOSInt colIndex, UTF8Char *buff)
{
	if (colIndex == 0)
		return Text::StrConcat(buff, (const UTF8Char*)"Shape");
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
		colDef->SetColName((const UTF8Char*)"Shape");
		colDef->SetColDP(0);
		colDef->SetColSize(0x7fffffff);
		colDef->SetAttr((const UTF8Char*)"");
		colDef->SetDefVal(0);
		colDef->SetAutoInc(false);
		colDef->SetNotNull(true);
		colDef->SetPK(false);
		return true;
	}
	return this->layer->GetColumnDef(colIndex - 1, colDef);
}

void Map::MapLayerReader::DelNewStr(const UTF8Char *s)
{
	Text::StrDelNew(s);
}
