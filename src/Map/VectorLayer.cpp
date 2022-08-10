#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/ColDef.h"
#include "Map/VectorLayer.h"
#include "Math/Math.h"
#include "Math/Geometry/Point.h"
#include "Math/Geometry/Polyline.h"
#include "Text/MyString.h"

const UTF8Char **Map::VectorLayer::CopyStrs(const UTF8Char **strs)
{
	if (this->thisStrLen == 0)
	{
		this->thisStrLen = MemAlloc(UOSInt, this->strCnt);
	}
	UOSInt i = this->strCnt;
	UOSInt j = 0;
	UOSInt k;
	UTF8Char *sptr = 0;
	while (i-- > 0)
	{
		if (strs[i])
		{
			this->thisStrLen[i] = k = Text::StrCharCnt(strs[i]);
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
			sptr = Text::StrConcatC(sptr, strs[k], this->thisStrLen[k]) + 1;
		}
		else
		{
			newStrs[k] = 0;
		}
		k++;
	}
	return newStrs;
}

const UTF8Char **Map::VectorLayer::CopyStrs(Text::String **strs)
{
	UOSInt i = this->strCnt;
	UOSInt j = 0;
	UOSInt k;
	UTF8Char *sptr = 0;
	while (i-- > 0)
	{
		if (strs[i])
		{
			k = strs[i]->leng;
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
			sptr = strs[k]->ConcatTo(sptr) + 1;
		}
		else
		{
			newStrs[k] = 0;
		}
		k++;
	}
	return newStrs;
}


const UTF8Char **Map::VectorLayer::CopyStrs(Text::PString *strs)
{
	UOSInt i = this->strCnt;
	UOSInt j = 0;
	UOSInt k;
	UTF8Char *sptr = 0;
	while (i-- > 0)
	{
		if (strs[i].v)
		{
			k = strs[i].leng;
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
		if (strs[k].v)
		{
			newStrs[k] = sptr;
			sptr = strs[k].ConcatTo(sptr) + 1;
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
	if (this->max.x > 200000000 || this->min.x < -200000000 || this->max.y > 200000000 || this->min.y < -200000000)
	{
		this->mapRate = 1.0;
	}
	else if (this->max.x > 20000000 || this->min.x < -20000000 || this->max.y > 20000000 || this->min.y < -20000000)
	{
		this->mapRate = 10.0;
	}
	else if (this->max.x > 2000000 || this->min.x < -2000000 || this->max.y > 2000000 || this->min.y < -2000000)
	{
		this->mapRate = 100.0;
	}
	else if (this->max.x > 200000 || this->min.x < -200000 || this->max.y > 200000 || this->min.y < -200000)
	{
		this->mapRate = 1000.0;
	}
	else if (this->max.x > 20000 || this->min.x < -20000 || this->max.y > 20000 || this->min.y < -20000)
	{
		this->mapRate = 10000.0;
	}
	else if (this->max.x > 2000 || this->min.x < -2000 || this->max.y > 2000 || this->min.y < -2000)
	{
		this->mapRate = 100000.0;
	}
	else if (this->max.x > 200 || this->min.x < -200 || this->max.y > 200 || this->min.y < -200)
	{
		this->mapRate = 1000000.0;
	}
	else
	{
		this->mapRate = 10000000.0;
	}
}

Map::VectorLayer::VectorLayer(Map::DrawLayerType layerType, Text::String *sourceName, UOSInt strCnt, const UTF8Char **colNames, Math::CoordinateSystem *csys, UOSInt nameCol, Text::String *layerName) : Map::IMapDrawLayer(sourceName, nameCol, layerName)
{
	UOSInt i;
	this->layerType = layerType;
	this->strCnt = strCnt;
	this->csys  = csys;
	this->min = Math::Coord2DDbl(0, 0);
	this->max = Math::Coord2DDbl(0, 0);
	this->maxStrLen = MemAlloc(UOSInt, strCnt);
	this->thisStrLen = 0;
	this->colNames = MemAlloc(const UTF8Char*, strCnt);
	this->cols = 0;
	this->mapRate = 10000000.0;
	this->mixedType = Math::Geometry::Vector2D::VectorType::Unknown;
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
}

Map::VectorLayer::VectorLayer(Map::DrawLayerType layerType, Text::CString sourceName, UOSInt strCnt, const UTF8Char **colNames, Math::CoordinateSystem *csys, UOSInt nameCol, Text::CString layerName) : Map::IMapDrawLayer(sourceName, nameCol, layerName)
{
	UOSInt i;
	this->layerType = layerType;
	this->strCnt = strCnt;
	this->csys  = csys;
	this->min = Math::Coord2DDbl(0, 0);
	this->max = Math::Coord2DDbl(0, 0);
	this->maxStrLen = MemAlloc(UOSInt, strCnt);
	this->thisStrLen = 0;
	this->colNames = MemAlloc(const UTF8Char*, strCnt);
	this->cols = 0;
	this->mapRate = 10000000.0;
	this->mixedType = Math::Geometry::Vector2D::VectorType::Unknown;
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
}

Map::VectorLayer::VectorLayer(Map::DrawLayerType layerType, Text::String *sourceName, UOSInt strCnt, const UTF8Char **colNames, Math::CoordinateSystem *csys, DB::DBUtil::ColType *colTypes, UOSInt *colSize, UOSInt *colDP, UOSInt nameCol, Text::String *layerName) : Map::IMapDrawLayer(sourceName, nameCol, layerName)
{
	UOSInt i;
	this->layerType = layerType;
	this->strCnt = strCnt;
	this->csys = csys;
	this->min = Math::Coord2DDbl(0, 0);
	this->max = Math::Coord2DDbl(0, 0);
	this->maxStrLen = MemAlloc(UOSInt, strCnt);
	this->thisStrLen = 0;
	this->colNames = MemAlloc(const UTF8Char*, strCnt);
	this->cols = MemAlloc(Map::VectorLayer::ColInfo, strCnt);
	this->mapRate = 10000000.0;
	this->mixedType = Math::Geometry::Vector2D::VectorType::Unknown;
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
}

Map::VectorLayer::VectorLayer(Map::DrawLayerType layerType, Text::CString sourceName, UOSInt strCnt, const UTF8Char **colNames, Math::CoordinateSystem *csys, DB::DBUtil::ColType *colTypes, UOSInt *colSize, UOSInt *colDP, UOSInt nameCol, Text::CString layerName) : Map::IMapDrawLayer(sourceName, nameCol, layerName)
{
	UOSInt i;
	this->layerType = layerType;
	this->strCnt = strCnt;
	this->csys = csys;
	this->min = Math::Coord2DDbl(0, 0);
	this->max = Math::Coord2DDbl(0, 0);
	this->maxStrLen = MemAlloc(UOSInt, strCnt);
	this->thisStrLen = 0;
	this->colNames = MemAlloc(const UTF8Char*, strCnt);
	this->cols = MemAlloc(Map::VectorLayer::ColInfo, strCnt);
	this->mapRate = 10000000.0;
	this->mixedType = Math::Geometry::Vector2D::VectorType::Unknown;
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
	if (this->thisStrLen)
	{
		MemFree(this->thisStrLen);
		this->thisStrLen = 0;
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
	i = this->vectorList.GetCount();
	while (i-- > 0)
	{
		DEL_CLASS(this->vectorList.GetItem(i));
	}

	const UTF8Char **strs;
	i = this->strList.GetCount();
	while (i-- > 0)
	{
		strs = this->strList.GetItem(i);
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
		this->mixedType = Math::Geometry::Vector2D::VectorType::Polyline;
	}
	else if (mixedType == Map::DRAW_LAYER_POLYGON)
	{
		this->mixedType = Math::Geometry::Vector2D::VectorType::Polygon;
	}
	else if (mixedType == Map::DRAW_LAYER_POINT || mixedType == Map::DRAW_LAYER_POINT3D)
	{
		this->mixedType = Math::Geometry::Vector2D::VectorType::Point;
	}
	else
	{
		this->mixedType = Math::Geometry::Vector2D::VectorType::Unknown;
	}
}

UOSInt Map::VectorLayer::GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr)
{
	UOSInt i = 0;
	UOSInt j = this->vectorList.GetCount();
	if (this->layerType == Map::DRAW_LAYER_MIXED)
	{
		Math::Geometry::Vector2D *vec;
		UOSInt ret = 0;
		while (i < j)
		{
			vec = this->vectorList.GetItem(i);
			if (vec->GetVectorType() == this->mixedType)
			{
				outArr->Add((OSInt)i);
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
			outArr->Add((OSInt)i);
			i++;
		}
		return j;
	}
}

UOSInt Map::VectorLayer::GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	return GetObjectIdsMapXY(outArr, nameArr, rect.ToDouble() / mapRate, keepEmpty);
}

UOSInt Map::VectorLayer::GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	Math::RectAreaDbl vBounds;
	UOSInt recCnt = 0;
	Math::Geometry::Vector2D *vec;
	UOSInt i;
	UOSInt j;
	if (this->layerType == Map::DRAW_LAYER_MIXED)
	{
		i = 0;
		j = this->vectorList.GetCount();
		while (i < j)
		{
			vec = this->vectorList.GetItem(i);
			if (vec->GetVectorType() == this->mixedType)
			{
				vec->GetBounds(&vBounds);
				if (rect.tl.x <= vBounds.br.x && rect.tl.y <= vBounds.br.y && rect.br.x >= vBounds.tl.x && rect.br.y >= vBounds.tl.y)
				{
					recCnt++;
					outArr->Add((Int64)i);
				}
			}
			i++;
		}
	}
	else
	{
		i = 0;
		j = this->vectorList.GetCount();
		while (i < j)
		{
			vec = this->vectorList.GetItem(i);
			vec->GetBounds(&vBounds);
			if (rect.tl.x <= vBounds.br.x && rect.tl.y <= vBounds.br.y && rect.br.x >= vBounds.tl.x && rect.br.y >= vBounds.tl.y)
			{
				recCnt++;
				outArr->Add((Int64)i);
			}
			i++;
		}
	}
	return recCnt;
}

Int64 Map::VectorLayer::GetObjectIdMax()
{
	return (Int64)this->vectorList.GetCount() - 1;
}

void Map::VectorLayer::ReleaseNameArr(void *nameArr)
{
}

UTF8Char *Map::VectorLayer::GetString(UTF8Char *buff, UOSInt buffSize, void *nameArr, Int64 id, UOSInt strIndex)
{
	if (id < 0 || (UInt64)id >= this->strList.GetCount())
	{
		*buff = 0;
		return 0;
	}
	if (strIndex >= this->strCnt)
	{
		*buff = 0;
		return 0;
	}
	const UTF8Char **strs = this->strList.GetItem((UOSInt)id);
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
			*colSize = this->maxStrLen[colIndex];
		}
	}
	if (this->cols)
	{
		return this->cols[colIndex].colType;
	}
	else
	{
		return DB::DBUtil::CT_VarUTF8Char;
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
		colDef->SetAttr(CSTR_NULL);
		colDef->SetDefVal(CSTR_NULL);
		colDef->SetAutoInc(false);
		colDef->SetNotNull(false);
		colDef->SetPK(false);
	}
	else
	{
		colDef->SetColName(this->colNames[colIndex]);
		colDef->SetColType(DB::DBUtil::CT_VarUTF8Char);
		colDef->SetColSize(this->maxStrLen[colIndex]);
		colDef->SetColDP(0);
		colDef->SetAttr(CSTR_NULL);
		colDef->SetDefVal(CSTR_NULL);
		colDef->SetAutoInc(false);
		colDef->SetNotNull(false);
		colDef->SetPK(false);
	}
	return true;
}

UInt32 Map::VectorLayer::GetCodePage()
{
	return 0;
}

Bool Map::VectorLayer::GetBounds(Math::RectAreaDbl *bounds)
{
	*bounds = Math::RectAreaDbl(this->min, this->max);
	return this->min.x != 0 || this->min.y != 0 || this->max.x != 0 || this->max.y != 0;
}

void *Map::VectorLayer::BeginGetObject()
{
	return (void*)-1;
}

void Map::VectorLayer::EndGetObject(void *session)
{
}

Map::DrawObjectL *Map::VectorLayer::GetNewObjectById(void *session, Int64 id)
{
	Map::DrawObjectL *obj;
	Math::Geometry::Vector2D *vec = this->vectorList.GetItem((UOSInt)id);
	if (vec)
	{
		obj = MemAlloc(Map::DrawObjectL, 1);
		obj->objId = id;
		if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::Point)
		{
			obj->nPtOfst = 0;
			obj->ptOfstArr = 0;
			obj->nPoint = 1;
			obj->pointArr = MemAllocA(Math::Coord2DDbl, 1);
			obj->pointArr[0] = vec->GetCenter();
		}
		else if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::Polyline || vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::Polygon)
		{
			UInt32 *ptOfsts;
			Math::Coord2DDbl *points;
			UOSInt i;
			Math::Geometry::PointOfstCollection *pts = (Math::Geometry::PointOfstCollection*)vec;
			ptOfsts = pts->GetPtOfstList(&i);
			obj->nPtOfst = (UInt32)i;
			obj->ptOfstArr = MemAlloc(UInt32, i);
			MemCopyNO(obj->ptOfstArr, ptOfsts, sizeof(UInt32) * i);
			points = pts->GetPointList(&i);
			obj->nPoint = (UInt32)i;
			obj->pointArr = MemAllocA(Math::Coord2DDbl, i);
			MemCopyNO(obj->pointArr, points, i * 16);
		}
		else if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::MultiPoint)
		{
			Math::Coord2DDbl *points;
			UOSInt i;
			Math::Geometry::PointCollection *pts = (Math::Geometry::PointCollection*)vec;
			obj->nPtOfst = (UInt32)1;
			obj->ptOfstArr = MemAlloc(UInt32, 1);
			obj->ptOfstArr[0] = 0;
			points = pts->GetPointList(&i);
			obj->nPoint = (UInt32)i;
			obj->pointArr = MemAllocA(Math::Coord2DDbl, i);
			MemCopyNO(obj->pointArr, points, i * 16);
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

Math::Geometry::Vector2D *Map::VectorLayer::GetNewVectorById(void *session, Int64 id)
{
	Math::Geometry::Vector2D *vec = this->vectorList.GetItem((UOSInt)id);
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
	if (obj->ptOfstArr)
		MemFree(obj->ptOfstArr);
	if (obj->pointArr)
		MemFreeA(obj->pointArr);
	MemFree(obj);
}

Map::IMapDrawLayer::ObjectClass Map::VectorLayer::GetObjectClass()
{
	return Map::IMapDrawLayer::OC_VECTOR_LAYER;
}

Bool Map::VectorLayer::VectorValid(Math::Geometry::Vector2D *vec)
{
	switch (this->layerType)
	{
	case Map::DRAW_LAYER_POINT:
		if (vec->GetVectorType() != Math::Geometry::Vector2D::VectorType::Point)
			return false;
		break;
	case Map::DRAW_LAYER_POINT3D:
		if (vec->GetVectorType() != Math::Geometry::Vector2D::VectorType::Point || !vec->HasZ())
			return false;
		break;
	case Map::DRAW_LAYER_POLYLINE:
		if (vec->GetVectorType() != Math::Geometry::Vector2D::VectorType::Polyline)
			return false;
		break;
	case Map::DRAW_LAYER_POLYLINE3D:
		if (vec->GetVectorType() != Math::Geometry::Vector2D::VectorType::Polyline || !vec->HasZ())
			return false;
		break;
	case Map::DRAW_LAYER_POLYGON:
		if (vec->GetVectorType() != Math::Geometry::Vector2D::VectorType::Polygon)
			return false;
		break;
	case Map::DRAW_LAYER_IMAGE:
		if (vec->GetVectorType() != Math::Geometry::Vector2D::VectorType::Image)
			return false;
		break;
	case Map::DRAW_LAYER_MIXED:
		break;
	case Map::DRAW_LAYER_UNKNOWN:
	default:
		return false;
	}
	return true;
}

Bool Map::VectorLayer::AddVector(Math::Geometry::Vector2D *vec, Text::String **strs)
{
	if (!this->VectorValid(vec))
		return false;
	const UTF8Char **newStrs = CopyStrs(strs);

	Math::RectAreaDbl bounds;
	Bool updated = false;
	vec->GetBounds(&bounds);
	if (this->vectorList.GetCount() == 0)
	{
		this->min = bounds.tl;
		this->max = bounds.br;
		updated = true;
	}
	else
	{
		if (this->min.x > bounds.tl.x)
		{
			this->min.x = bounds.tl.x;
			updated = true;
		}
		if (this->min.y > bounds.tl.y)
		{
			this->min.y = bounds.tl.y;
			updated = true;
		}
		if (this->max.x < bounds.br.x)
		{
			this->max.x = bounds.br.x;
			updated = true;
		}
		if (this->max.y < bounds.br.y)
		{
			this->max.y = bounds.br.y;
			updated = true;
		}
	}

	this->vectorList.Add(vec);
	this->strList.Add(newStrs);
	if (updated)
		this->UpdateMapRate();
	return true;
}

Bool Map::VectorLayer::AddVector(Math::Geometry::Vector2D *vec, Text::PString *strs)
{
	if (!this->VectorValid(vec))
		return false;
	const UTF8Char **newStrs = CopyStrs(strs);

	Math::RectAreaDbl bounds;
	Bool updated = false;
	vec->GetBounds(&bounds);
	if (this->vectorList.GetCount() == 0)
	{
		this->min = bounds.tl;
		this->max = bounds.br;
		updated = true;
	}
	else
	{
		if (this->min.x > bounds.tl.x)
		{
			this->min.x = bounds.tl.x;
			updated = true;
		}
		if (this->min.y > bounds.tl.y)
		{
			this->min.y = bounds.tl.y;
			updated = true;
		}
		if (this->max.x < bounds.br.x)
		{
			this->max.x = bounds.br.x;
			updated = true;
		}
		if (this->max.y < bounds.br.y)
		{
			this->max.y = bounds.br.y;
			updated = true;
		}
	}

	this->vectorList.Add(vec);
	this->strList.Add(newStrs);
	if (updated)
		this->UpdateMapRate();
	return true;
}

Bool Map::VectorLayer::AddVector(Math::Geometry::Vector2D *vec, const UTF8Char **strs)
{
	if (!this->VectorValid(vec))
		return false;
	const UTF8Char **newStrs = CopyStrs(strs);

	Math::RectAreaDbl bounds;
	Bool updated = false;
	vec->GetBounds(&bounds);
	if (this->vectorList.GetCount() == 0)
	{
		this->min = bounds.tl;
		this->max = bounds.br;
		updated = true;
	}
	else
	{
		if (this->min.x > bounds.tl.x)
		{
			this->min.x = bounds.tl.x;
			updated = true;
		}
		if (this->min.y > bounds.tl.y)
		{
			this->min.y = bounds.tl.y;
			updated = true;
		}
		if (this->max.x < bounds.br.x)
		{
			this->max.x = bounds.br.x;
			updated = true;
		}
		if (this->max.y < bounds.br.y)
		{
			this->max.y = bounds.br.y;
			updated = true;
		}
	}

	this->vectorList.Add(vec);
	this->strList.Add(newStrs);
	if (updated)
		this->UpdateMapRate();
	return true;
}

Bool Map::VectorLayer::SplitPolyline(Math::Coord2DDbl pt)
{
	Math::Coord2DDbl nearPt;
	Int64 objId;
	if (this->layerType != Map::DRAW_LAYER_POLYLINE)
		return false;
	objId = this->GetNearestObjectId(0, pt, &nearPt);
	if (objId < 0)
		return false;

	Math::Geometry::Polyline *pl = (Math::Geometry::Polyline*)this->vectorList.GetItem((UOSInt)objId);
	Math::Geometry::Polyline *pl2;
	if ((pl2 = pl->SplitByPoint(pt)) != 0)
	{
		this->vectorList.Add(pl2);
		this->strList.Add(CopyStrs(this->strList.GetItem((UOSInt)objId)));
	}

	return false;
}

void Map::VectorLayer::OptimizePolylinePath()
{
	if (this->layerType != Map::DRAW_LAYER_POLYLINE)
		return;
	const UTF8Char **tmpStr;
	Math::Geometry::Polyline *tmpPL;

	Math::Coord2DDbl pt;
	Math::Coord2DDbl nearPt;
	Math::Coord2DDbl *points;
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

		UOSInt i = this->vectorList.GetCount();
		while (i-- > 0)
		{
			tmpStr = this->strList.RemoveAt(i);
			tmpPL = (Math::Geometry::Polyline*)this->vectorList.RemoveAt(i);

			points = tmpPL->GetPointList(&nPoints);
			pt = *points;
			objId = this->GetNearestObjectId(0, pt, &nearPt);
			if (objId >= 0)
			{
				ix = (Int32)(pt.x * 200000.0);
				iy = (Int32)(pt.y * 200000.0);
				nix = (Int32)(nearPt.x * 200000.0);
				niy = (Int32)(nearPt.y * 200000.0);
				if (ix == nix && iy == niy)
				{
					Math::Geometry::Polyline *pl = (Math::Geometry::Polyline*)this->vectorList.GetItem((UOSInt)objId);
					Math::Geometry::Polyline *pl2;
					if ((pl2 = pl->SplitByPoint(pt)) != 0)
					{
						this->vectorList.Add(pl2);
						this->strList.Add(CopyStrs(this->strList.GetItem((UOSInt)objId)));
						found = true;
					}
				}
			}

			pt = points[nPoints - 1];
			objId = this->GetNearestObjectId(0, pt, &nearPt);
			if (objId >= 0)
			{
				ix = (Int32)(pt.x * 200000.0);
				iy = (Int32)(pt.y * 200000.0);
				nix = (Int32)(nearPt.x * 200000.0);
				niy = (Int32)(nearPt.y * 200000.0);
				if (ix == nix && iy == niy)
				{
					Math::Geometry::Polyline *pl = (Math::Geometry::Polyline*)this->vectorList.GetItem((UOSInt)objId);
					Math::Geometry::Polyline *pl2;
					if ((pl2 = pl->SplitByPoint(pt)) != 0)
					{
						this->vectorList.Add(pl2);
						this->strList.Add(CopyStrs(this->strList.GetItem((UOSInt)objId)));
						found = true;
					}
				}
			}
			this->strList.Add(tmpStr);
			this->vectorList.Add(tmpPL);
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

void Map::VectorLayer::ReplaceVector(Int64 id, Math::Geometry::Vector2D *vec)
{
	if (this->vectorList.GetCount() <= (UInt64)id)
	{
		this->vectorList.Add(vec);
	}
	else
	{
		Math::Geometry::Vector2D *v = this->vectorList.GetItem((UOSInt)id);
		this->vectorList.SetItem((UOSInt)id, vec);
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
		Math::Geometry::Vector2D *v;
		UOSInt i;
		i = this->vectorList.GetCount();
		while (i-- > 0)
		{
			v = this->vectorList.GetItem(i);
			v->ConvCSys(this->csys, csys);
		}

		Math::CoordinateSystem::ConvertXYZ(this->csys, csys, this->min.x, this->min.y, 0, &this->min.x, &this->min.y, 0);
		Math::CoordinateSystem::ConvertXYZ(this->csys, csys, this->max.x, this->max.y, 0, &this->max.x, &this->max.y, 0);
		this->UpdateMapRate();

		SDEL_CLASS(this->csys);
		this->csys = csys;
	}
}
