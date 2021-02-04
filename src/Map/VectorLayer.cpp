#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/ColDef.h"
#include "Map/VectorLayer.h"
#include "Math/Math.h"
#include "Math/Point.h"
#include "Math/Polyline.h"
#include "Text/MyString.h"

const UTF8Char **Map::VectorLayer::CopyStrs(const UTF8Char **strs)
{
	UOSInt i = this->strCnt;
	UOSInt j = 0;
	UOSInt k;
	UTF8Char *sptr = 0;
	while (i-- > 0)
	{
		if (strs[i])
		{
			k = Text::StrCharCnt(strs[i]);
			j += k + 1;
			if (this->maxStrLen[i] < k)
			{
				this->maxStrLen[i] = k;
			}
		}
	}
	const UTF8Char **newStrs = MemAlloc(const UTF8Char*, this->strCnt);
	if (j > 0)
	{
		sptr = MemAlloc(UTF8Char, j);
	}
	i = this->strCnt;
	k = 0;
	while (k < i)
	{
		if (strs[k])
		{
			newStrs[k] = sptr;
			sptr = Text::StrConcat(sptr, strs[k]) + 1;
		}
		else
		{
			newStrs[k] = 0;
		}
		k++;
	}
	return newStrs;
}

void Map::VectorLayer::UpdateMapRate()
{
	if (this->maxX > 200000000 || this->minX < -200000000 || this->maxY > 200000000 || this->minY < -200000000)
	{
		this->mapRate = 1.0;
	}
	else if (this->maxX > 20000000 || this->minX < -20000000 || this->maxY > 20000000 || this->minY < -20000000)
	{
		this->mapRate = 10.0;
	}
	else if (this->maxX > 2000000 || this->minX < -2000000 || this->maxY > 2000000 || this->minY < -2000000)
	{
		this->mapRate = 100.0;
	}
	else if (this->maxX > 200000 || this->minX < -200000 || this->maxY > 200000 || this->minY < -200000)
	{
		this->mapRate = 1000.0;
	}
	else if (this->maxX > 20000 || this->minX < -20000 || this->maxY > 20000 || this->minY < -20000)
	{
		this->mapRate = 10000.0;
	}
	else if (this->maxX > 2000 || this->minX < -2000 || this->maxY > 2000 || this->minY < -2000)
	{
		this->mapRate = 100000.0;
	}
	else if (this->maxX > 200 || this->minX < -200 || this->maxY > 200 || this->minY < -200)
	{
		this->mapRate = 1000000.0;
	}
	else
	{
		this->mapRate = 10000000.0;
	}
}

Map::VectorLayer::VectorLayer(Map::DrawLayerType layerType, const UTF8Char *sourceName, UOSInt strCnt, const UTF8Char **colNames, Math::CoordinateSystem *csys, UOSInt nameCol, const UTF8Char *layerName) : Map::IMapDrawLayer(sourceName, nameCol, layerName)
{
	OSInt i;
	this->layerType = layerType;
	this->strCnt = strCnt;
	this->csys  = csys;
	this->minX = 0;
	this->minY = 0;
	this->maxX = 0;
	this->maxY = 0;
	this->maxStrLen = MemAlloc(UOSInt, strCnt);
	this->colNames = MemAlloc(const UTF8Char*, strCnt);
	this->cols = 0;
	this->mapRate = 10000000.0;
	this->mixedType = Math::Vector2D::VT_UNKNOWN;
	i = strCnt;
	while (i-- > 0)
	{
		maxStrLen[i] = 0;
		if (colNames[i])
		{
			this->colNames[i] = Text::StrCopyNew(colNames[i]);
		}
		else
		{
			this->colNames[i] = 0;
		}
	}
	NEW_CLASS(vectorList, Data::ArrayList<Math::Vector2D*>());
	NEW_CLASS(strList, Data::ArrayList<const UTF8Char**>());
}

Map::VectorLayer::VectorLayer(Map::DrawLayerType layerType, const UTF8Char *sourceName, UOSInt strCnt, const UTF8Char **colNames, Math::CoordinateSystem *csys, DB::DBUtil::ColType *colTypes, UOSInt *colSize, UOSInt *colDP, UOSInt nameCol, const UTF8Char *layerName) : Map::IMapDrawLayer(sourceName, nameCol, layerName)
{
	OSInt i;
	this->layerType = layerType;
	this->strCnt = strCnt;
	this->csys = csys;
	this->minX = 0;
	this->minY = 0;
	this->maxX = 0;
	this->maxY = 0;
	this->maxStrLen = MemAlloc(UOSInt, strCnt);
	this->colNames = MemAlloc(const UTF8Char*, strCnt);
	this->cols = MemAlloc(Map::VectorLayer::ColInfo, strCnt);
	this->mapRate = 10000000.0;
	this->mixedType = Math::Vector2D::VT_UNKNOWN;
	i = strCnt;
	while (i-- > 0)
	{
		maxStrLen[i] = 0;
		if (colNames[i])
		{
			this->colNames[i] = Text::StrCopyNew(colNames[i]);
		}
		else
		{
			this->colNames[i] = 0;
		}
		cols[i].colType = colTypes[i];
		cols[i].colSize = colSize[i];
		cols[i].colDP = colDP[i];
	}
	NEW_CLASS(vectorList, Data::ArrayList<Math::Vector2D*>());
	NEW_CLASS(strList, Data::ArrayList<const UTF8Char**>());
}

Map::VectorLayer::~VectorLayer()
{
	UOSInt i;
	UOSInt j;
	if (this->maxStrLen)
	{
		MemFree(this->maxStrLen);
		this->maxStrLen = 0;
	}
	if (this->colNames)
	{
		i = this->strCnt;
		while (i-- > 0)
		{
			if (this->colNames[i])
			{
				Text::StrDelNew(this->colNames[i]);
			}
		}
		MemFree(this->colNames);
		this->colNames = 0;
	}
	if (this->vectorList)
	{
		i = this->vectorList->GetCount();
		while (i-- > 0)
		{
			DEL_CLASS(this->vectorList->GetItem(i));
		}
		DEL_CLASS(this->vectorList);
		this->vectorList = 0;
	}
	if (this->strList)
	{
		const UTF8Char **strs;
		i = this->strList->GetCount();
		while (i-- > 0)
		{
			strs = this->strList->GetItem(i);
			j = 0;
			while (j < this->strCnt)
			{
				if (strs[j])
				{
					MemFree((void*)strs[j]);
					break;
				}
				j++;
			}
			MemFree(strs);
		}
		DEL_CLASS(this->strList);
		this->strList = 0;
	}
	if (this->cols)
	{
		MemFree(this->cols);
	}
}

Map::DrawLayerType Map::VectorLayer::GetLayerType()
{
	return this->layerType;
}

void Map::VectorLayer::SetMixedType(DrawLayerType mixedType)
{
	if (mixedType == Map::DRAW_LAYER_POLYLINE || mixedType == Map::DRAW_LAYER_POLYLINE3D)
	{
		this->mixedType = Math::Vector2D::VT_POLYLINE;
	}
	else if (mixedType == Map::DRAW_LAYER_POLYGON)
	{
		this->mixedType = Math::Vector2D::VT_POLYGON;
	}
	else if (mixedType == Map::DRAW_LAYER_POINT || mixedType == Map::DRAW_LAYER_POINT3D)
	{
		this->mixedType = Math::Vector2D::VT_POINT;
	}
	else
	{
		this->mixedType = Math::Vector2D::VT_UNKNOWN;
	}
}

UOSInt Map::VectorLayer::GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr)
{
	UOSInt i = 0;
	UOSInt j = this->vectorList->GetCount();
	if (this->layerType == Map::DRAW_LAYER_MIXED)
	{
		Math::Vector2D *vec;
		UOSInt ret = 0;
		while (i < j)
		{
			vec = this->vectorList->GetItem(i);
			if (vec->GetVectorType() == this->mixedType)
			{
				outArr->Add(i);
				ret++;
			}
			i++;
		}
		return ret;
	}
	else
	{
		while (i < j)
		{
			outArr->Add(i);
			i++;
		}
		return j;
	}
}

UOSInt Map::VectorLayer::GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Int32 x1, Int32 y1, Int32 x2, Int32 y2, Bool keepEmpty)
{
	return GetObjectIdsMapXY(outArr, nameArr, x1 / mapRate, y1 / mapRate, x2 / mapRate, y2 / mapRate, keepEmpty);
}

UOSInt Map::VectorLayer::GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Double x1, Double y1, Double x2, Double y2, Bool keepEmpty)
{
	Double vx1;
	Double vy1;
	Double vx2;
	Double vy2;
	UOSInt recCnt = 0;
	Math::Vector2D *vec;
	UOSInt i;
	UOSInt j;
	if (this->layerType == Map::DRAW_LAYER_MIXED)
	{
		i = 0;
		j = this->vectorList->GetCount();
		while (i < j)
		{
			vec = this->vectorList->GetItem(i);
			if (vec->GetVectorType() == this->mixedType)
			{
				vec->GetBounds(&vx1, &vy1, &vx2, &vy2);
				if (x1 <= vx2 && y1 <= vy2 && x2 >= vx1 && y2 >= vy1)
				{
					recCnt++;
					outArr->Add(i);
				}
			}
			i++;
		}
	}
	else
	{
		i = 0;
		j = this->vectorList->GetCount();
		while (i < j)
		{
			vec = this->vectorList->GetItem(i);
			vec->GetBounds(&vx1, &vy1, &vx2, &vy2);
			if (x1 <= vx2 && y1 <= vy2 && x2 >= vx1 && y2 >= vy1)
			{
				recCnt++;
				outArr->Add(i);
			}
			i++;
		}
	}
	return recCnt;
}

Int64 Map::VectorLayer::GetObjectIdMax()
{
	return this->vectorList->GetCount() - 1;
}

void Map::VectorLayer::ReleaseNameArr(void *nameArr)
{
}

UTF8Char *Map::VectorLayer::GetString(UTF8Char *buff, UOSInt buffSize, void *nameArr, Int64 id, UOSInt strIndex)
{
	if (id < 0 || (UInt64)id >= this->strList->GetCount())
	{
		*buff = 0;
		return 0;
	}
	if (strIndex >= this->strCnt)
	{
		*buff = 0;
		return 0;
	}
	const UTF8Char **strs = this->strList->GetItem((UOSInt)id);
	if (strs[strIndex] == 0)
	{
		*buff = 0;
		return 0;
	}
	return Text::StrConcatS(buff, strs[strIndex], buffSize);
}

UOSInt Map::VectorLayer::GetColumnCnt()
{
	return this->strCnt;
}

UTF8Char *Map::VectorLayer::GetColumnName(UTF8Char *buff, UOSInt colIndex)
{
	if (colIndex >= this->strCnt)
		return 0;
	const UTF8Char *name = this->colNames[colIndex];
	if (name)
		return Text::StrConcat(buff, name);
	else
		return 0;
}


DB::DBUtil::ColType Map::VectorLayer::GetColumnType(UOSInt colIndex, UOSInt *colSize)
{
	if (colIndex >= this->strCnt)
		return DB::DBUtil::CT_Unknown;
	if (colSize)
	{
		if (this->cols)
		{
			*colSize = this->cols[colIndex].colSize;
		}
		else
		{
			*colSize = (Int32)this->maxStrLen[colIndex];
		}
	}
	if (this->cols)
	{
		return this->cols[colIndex].colType;
	}
	else
	{
		return DB::DBUtil::CT_VarChar;
	}
	
}

Bool Map::VectorLayer::GetColumnDef(UOSInt colIndex, DB::ColDef *colDef)
{
	if (colIndex >= this->strCnt)
		return false;
	if (this->cols)
	{
		colDef->SetColName(this->colNames[colIndex]);
		colDef->SetColType(this->cols[colIndex].colType);
		colDef->SetColSize(this->cols[colIndex].colSize);
		colDef->SetColDP(this->cols[colIndex].colDP);
		colDef->SetAttr(0);
		colDef->SetDefVal(0);
		colDef->SetAutoInc(false);
		colDef->SetNotNull(false);
		colDef->SetPK(false);
	}
	else
	{
		colDef->SetColName(this->colNames[colIndex]);
		colDef->SetColType(DB::DBUtil::CT_VarChar);
		colDef->SetColSize((Int32)this->maxStrLen[colIndex]);
		colDef->SetColDP(0);
		colDef->SetAttr(0);
		colDef->SetDefVal(0);
		colDef->SetAutoInc(false);
		colDef->SetNotNull(false);
		colDef->SetPK(false);
	}
	return true;
}

Int32 Map::VectorLayer::GetCodePage()
{
	return 0;
}

Bool Map::VectorLayer::GetBoundsDbl(Double *minX, Double *minY, Double *maxX, Double *maxY)
{
	*minX = this->minX;
	*minY = this->minY;
	*maxX = this->maxX;
	*maxY = this->maxY;
	return this->minX != 0 || this->minY != 0 || this->maxX != 0 || this->maxY != 0;
}

void *Map::VectorLayer::BeginGetObject()
{
	return (void*)-1;
}

void Map::VectorLayer::EndGetObject(void *session)
{
}

Map::DrawObjectL *Map::VectorLayer::GetObjectByIdD(void *session, Int64 id)
{
	Map::DrawObjectL *obj;
	Math::Vector2D *vec = this->vectorList->GetItem((OSInt)id);
	if (vec)
	{
		obj = MemAlloc(Map::DrawObjectL, 1);
		obj->objId = id;
		if (vec->GetVectorType() == Math::Vector2D::VT_POINT)
		{
			obj->nParts = 0;
			obj->parts = 0;
			obj->nPoints = 1;
			obj->points = MemAlloc(Double, 2);
			vec->GetCenter(&obj->points[0], &obj->points[1]);
		}
		else if (vec->GetVectorType() == Math::Vector2D::VT_POLYLINE || vec->GetVectorType() == Math::Vector2D::VT_POLYGON || vec->GetVectorType() == Math::Vector2D::VT_MULTIPOINT)
		{
			UInt32 *parts;
			Double *points;
			UOSInt i;
			Math::PointCollection *pts = (Math::PointCollection*)vec;
			parts = pts->GetPartList(&i);
			obj->nParts = (UInt32)i;
			obj->parts = MemAlloc(UInt32, i);
			MemCopyNO(obj->parts, parts, sizeof(UInt32) * i);
			points = pts->GetPointList(&i);
			obj->nPoints = (UInt32)i;
			obj->points = MemAlloc(Double, i * 2);
			MemCopyNO(obj->points, points, i * 16);
		}
		obj->flags = 0;
		obj->lineColor = 0;
		return obj;
	}
	else
	{
		return 0;
	}
}

Math::Vector2D *Map::VectorLayer::GetVectorById(void *session, Int64 id)
{
	Math::Vector2D *vec = this->vectorList->GetItem((OSInt)id);
	if (vec)
	{
		return vec->Clone();
	}
	else
	{
		return 0;
	}
}

void Map::VectorLayer::ReleaseObject(void *session, DrawObjectL *obj)
{
	if (obj->parts)
		MemFree(obj->parts);
	if (obj->points)
		MemFree(obj->points);
	MemFree(obj);
}

Map::IMapDrawLayer::ObjectClass Map::VectorLayer::GetObjectClass()
{
	return Map::IMapDrawLayer::OC_VECTOR_LAYER;
}

Bool Map::VectorLayer::AddVector(Math::Vector2D *vec, const UTF8Char **strs)
{
	if (this->layerType == Map::DRAW_LAYER_POINT)
	{
		if (vec->GetVectorType() != Math::Vector2D::VT_POINT)
			return false;
	}
	else if (this->layerType == Map::DRAW_LAYER_POINT3D)
	{
		if (vec->GetVectorType() != Math::Vector2D::VT_POINT || !vec->Support3D())
			return false;
	}
	else if (this->layerType == Map::DRAW_LAYER_POLYLINE)
	{
		if (vec->GetVectorType() != Math::Vector2D::VT_POLYLINE)
			return false;
	}
	else if (this->layerType == Map::DRAW_LAYER_POLYLINE3D)
	{
		if (vec->GetVectorType() != Math::Vector2D::VT_POLYLINE || !vec->Support3D())
			return false;
	}
	else if (this->layerType == Map::DRAW_LAYER_POLYGON)
	{
		if (vec->GetVectorType() != Math::Vector2D::VT_POLYGON)
			return false;
	}
	else if (this->layerType == Map::DRAW_LAYER_IMAGE)
	{
		if (vec->GetVectorType() != Math::Vector2D::VT_IMAGE)
			return false;
	}
	else if (this->layerType == Map::DRAW_LAYER_MIXED)
	{
	}
	else
	{
		return false;
	}
	const UTF8Char **newStrs = CopyStrs(strs);

	Double x1;
	Double y1;
	Double x2;
	Double y2;
	Bool updated = false;
	vec->GetBounds(&x1, &y1, &x2, &y2);
	if (this->vectorList->GetCount() == 0)
	{
		this->minX = x1;
		this->minY = y1;
		this->maxX = x2;
		this->maxY = y2;
		updated = true;
	}
	else
	{
		if (this->minX > x1)
		{
			this->minX = x1;
			updated = true;
		}
		if (this->minY > y1)
		{
			this->minY = y1;
			updated = true;
		}
		if (this->maxX < x2)
		{
			this->maxX = x2;
			updated = true;
		}
		if (this->maxY < y2)
		{
			this->maxY = y2;
			updated = true;
		}
	}

	this->vectorList->Add(vec);
	this->strList->Add(newStrs);
	if (updated)
		this->UpdateMapRate();
	return true;
}

Bool Map::VectorLayer::SplitPolyline(Double x, Double y)
{
	Double nearPtX;
	Double nearPtY;
	Int64 objId;
	if (this->layerType != Map::DRAW_LAYER_POLYLINE)
		return false;
	objId = this->GetNearestObjectId(0, x, y, &nearPtX, &nearPtY);
	if (objId < 0)
		return false;

	Math::Polyline *pl = (Math::Polyline*)this->vectorList->GetItem((OSInt)objId);
	Math::Polyline *pl2;
	if ((pl2 = pl->SplitByPoint(x, y)) != 0)
	{
		this->vectorList->Add(pl2);
		this->strList->Add(CopyStrs(this->strList->GetItem((OSInt)objId)));
	}

	return false;
}

void Map::VectorLayer::OptimizePolylinePath()
{
	if (this->layerType != Map::DRAW_LAYER_POLYLINE)
		return;
	const UTF8Char **tmpStr;
	Math::Polyline *tmpPL;

	Double x;
	Double y;
	Double nearPtX;
	Double nearPtY;
	Double *points;
	UOSInt nPoints;
	Int32 ix;
	Int32 iy;
	Int32 nix;
	Int32 niy;
	Int64 objId;

	Int32 loopCnt = 3;
	Bool found = true;
	while (found)
	{
		found = false;
		if (loopCnt-- <= 0)
		{
			break;
		}

		UOSInt i = this->vectorList->GetCount();
		while (i-- > 0)
		{
			tmpStr = this->strList->RemoveAt(i);
			tmpPL = (Math::Polyline*)this->vectorList->RemoveAt(i);

			points = tmpPL->GetPointList(&nPoints);
			x = points[0];
			y = points[1];
			objId = this->GetNearestObjectId(0, x, y, &nearPtX, &nearPtY);
			if (objId >= 0)
			{
				ix = (Int32)(x * 200000.0);
				iy = (Int32)(y * 200000.0);
				nix = (Int32)(nearPtX * 200000.0);
				niy = (Int32)(nearPtY * 200000.0);
				if (ix == nix && iy == niy)
				{
					Math::Polyline *pl = (Math::Polyline*)this->vectorList->GetItem((OSInt)objId);
					Math::Polyline *pl2;
					if ((pl2 = pl->SplitByPoint(x, y)) != 0)
					{
						this->vectorList->Add(pl2);
						this->strList->Add(CopyStrs(this->strList->GetItem((OSInt)objId)));
						found = true;
					}
				}
			}

			x = points[(nPoints << 1) - 2];
			y = points[(nPoints << 1) - 1];
			objId = this->GetNearestObjectId(0, x, y, &nearPtX, &nearPtY);
			if (objId >= 0)
			{
				ix = (Int32)(x * 200000.0);
				iy = (Int32)(y * 200000.0);
				nix = (Int32)(nearPtX * 200000.0);
				niy = (Int32)(nearPtY * 200000.0);
				if (ix == nix && iy == niy)
				{
					Math::Polyline *pl = (Math::Polyline*)this->vectorList->GetItem((OSInt)objId);
					Math::Polyline *pl2;
					if ((pl2 = pl->SplitByPoint(x, y)) != 0)
					{
						this->vectorList->Add(pl2);
						this->strList->Add(CopyStrs(this->strList->GetItem((OSInt)objId)));
						found = true;
					}
				}
			}
			this->strList->Add(tmpStr);
			this->vectorList->Add(tmpPL);
		}
	}
/*(	Bool found = true;
	while (found)
	{
		found = false;
		objIds->Clear();
		lyr->GetAllObjectIds(objIds, &nameArr);
		i = objIds->GetCount();
		while (i-- > 0)
		{
			Math::Polyline *pl = lyr->RemoveVector(objIds->GetItem(i));
		}
	}*/
}

void Map::VectorLayer::ReplaceVector(Int64 id, Math::Vector2D *vec)
{
	if (vectorList->GetCount() <= (UInt64)id)
	{
		vectorList->Add(vec);
	}
	else
	{
		Math::Vector2D *v = this->vectorList->GetItem((OSInt)id);
		this->vectorList->SetItem((OSInt)id, vec);
		DEL_CLASS(v);
	}
}

void Map::VectorLayer::ConvCoordinateSystem(Math::CoordinateSystem *csys)
{
	if (csys == 0)
		return;
	if (this->csys == 0)
	{
		this->csys = csys;
	}
	else
	{
		Math::Vector2D *v;
		OSInt i;
		i = this->vectorList->GetCount();
		while (i-- > 0)
		{
			v = this->vectorList->GetItem(i);
			v->ConvCSys(this->csys, csys);
		}

		Math::CoordinateSystem::ConvertXYZ(this->csys, csys, this->minX, this->minY, 0, &this->minX, &this->minY, 0);
		Math::CoordinateSystem::ConvertXYZ(this->csys, csys, this->maxX, this->maxY, 0, &this->maxX, &this->maxY, 0);
		this->UpdateMapRate();

		SDEL_CLASS(this->csys);
		this->csys = csys;
	}
}
