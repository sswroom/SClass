#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/ColDef.h"
#include "Map/VectorLayer.h"
#include "Math/CoordinateSystemConverter.h"
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

const UTF8Char **Map::VectorLayer::CopyStrs(NN<Data::ArrayListStringNN> strs)
{
	UOSInt i = this->strCnt;
	UOSInt j = 0;
	UOSInt k;
	NN<Text::String> s;
	UTF8Char *sptr = 0;
	while (i-- > 0)
	{
		if (strs->GetItem(i).SetTo(s))
		{
			k = s->leng;
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
		
		if (strs->GetItem(k).SetTo(s))
		{
			newStrs[k] = sptr;
			sptr = s->ConcatTo(sptr) + 1;
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

Map::VectorLayer::VectorLayer(Map::DrawLayerType layerType, NN<Text::String> sourceName, UOSInt strCnt, const UTF8Char **colNames, NN<Math::CoordinateSystem> csys, UOSInt nameCol, Text::String *layerName) : Map::MapDrawLayer(sourceName, nameCol, layerName, csys)
{
	UOSInt i;
	this->layerType = layerType;
	this->strCnt = strCnt;
	this->min = Math::Coord2DDbl(0, 0);
	this->max = Math::Coord2DDbl(0, 0);
	this->maxStrLen = MemAlloc(UOSInt, strCnt);
	this->thisStrLen = 0;
	this->colNames = MemAlloc(Text::String*, strCnt);
	this->cols = 0;
	this->tableName = 0;
	this->mapRate = 10000000.0;
	this->mixedData = MixedData::AllData;
	i = strCnt;
	while (i-- > 0)
	{
		maxStrLen[i] = 0;
		this->colNames[i] = Text::String::NewOrNullSlow(colNames[i]).OrNull();
	}
}

Map::VectorLayer::VectorLayer(Map::DrawLayerType layerType, Text::CStringNN sourceName, UOSInt strCnt, const UTF8Char **colNames, NN<Math::CoordinateSystem> csys, UOSInt nameCol, Text::CString layerName) : Map::MapDrawLayer(sourceName, nameCol, layerName, csys)
{
	UOSInt i;
	this->layerType = layerType;
	this->strCnt = strCnt;
	this->min = Math::Coord2DDbl(0, 0);
	this->max = Math::Coord2DDbl(0, 0);
	this->maxStrLen = MemAlloc(UOSInt, strCnt);
	this->thisStrLen = 0;
	this->colNames = MemAlloc(Text::String*, strCnt);
	this->cols = 0;
	this->tableName = 0;
	this->mapRate = 10000000.0;
	this->mixedData = MixedData::AllData;
	i = strCnt;
	while (i-- > 0)
	{
		maxStrLen[i] = 0;
		this->colNames[i] = Text::String::NewOrNullSlow(colNames[i]).OrNull();
	}
}

Map::VectorLayer::VectorLayer(Map::DrawLayerType layerType, NN<Text::String> sourceName, UOSInt strCnt, const UTF8Char **colNames, NN<Math::CoordinateSystem> csys, DB::DBUtil::ColType *colTypes, UOSInt *colSize, UOSInt *colDP, UOSInt nameCol, Text::String *layerName) : Map::MapDrawLayer(sourceName, nameCol, layerName, csys)
{
	UOSInt i;
	this->layerType = layerType;
	this->strCnt = strCnt;
	this->min = Math::Coord2DDbl(0, 0);
	this->max = Math::Coord2DDbl(0, 0);
	this->maxStrLen = MemAlloc(UOSInt, strCnt);
	this->thisStrLen = 0;
	this->colNames = MemAlloc(Text::String*, strCnt);
	this->cols = MemAlloc(Map::VectorLayer::ColInfo, strCnt);
	this->tableName = 0;
	this->mapRate = 10000000.0;
	this->mixedData = MixedData::AllData;
	i = strCnt;
	while (i-- > 0)
	{
		maxStrLen[i] = 0;
		this->colNames[i] = Text::String::NewOrNullSlow(colNames[i]).OrNull();
		cols[i].colType = colTypes[i];
		cols[i].colSize = colSize[i];
		cols[i].colDP = colDP[i];
	}
}

Map::VectorLayer::VectorLayer(Map::DrawLayerType layerType, Text::CStringNN sourceName, UOSInt strCnt, const UTF8Char **colNames, NN<Math::CoordinateSystem> csys, DB::DBUtil::ColType *colTypes, UOSInt *colSize, UOSInt *colDP, UOSInt nameCol, Text::CString layerName) : Map::MapDrawLayer(sourceName, nameCol, layerName, csys)
{
	UOSInt i;
	this->layerType = layerType;
	this->strCnt = strCnt;
	this->min = Math::Coord2DDbl(0, 0);
	this->max = Math::Coord2DDbl(0, 0);
	this->maxStrLen = MemAlloc(UOSInt, strCnt);
	this->thisStrLen = 0;
	this->colNames = MemAlloc(Text::String*, strCnt);
	this->cols = MemAlloc(Map::VectorLayer::ColInfo, strCnt);
	this->tableName = 0;
	this->mapRate = 10000000.0;
	this->mixedData = MixedData::AllData;
	i = strCnt;
	while (i-- > 0)
	{
		maxStrLen[i] = 0;
		this->colNames[i] = Text::String::NewOrNullSlow(colNames[i]).OrNull();
		cols[i].colType = colTypes[i];
		cols[i].colSize = colSize[i];
		cols[i].colDP = colDP[i];
	}
}

Map::VectorLayer::VectorLayer(Map::DrawLayerType layerType, Text::CStringNN sourceName, NN<Data::ArrayListStringNN> colNames, NN<Math::CoordinateSystem> csys, NN<Data::ArrayList<ColInfo>> colInfos, UOSInt nameCol, Text::CString layerName) : Map::MapDrawLayer(sourceName, nameCol, layerName, csys)
{
	UOSInt i;
	this->layerType = layerType;
	this->strCnt = colNames->GetCount();
	this->min = Math::Coord2DDbl(0, 0);
	this->max = Math::Coord2DDbl(0, 0);
	this->maxStrLen = MemAlloc(UOSInt, this->strCnt);
	this->thisStrLen = 0;
	this->colNames = MemAlloc(Text::String*, this->strCnt);
	this->cols = MemAlloc(Map::VectorLayer::ColInfo, this->strCnt);
	this->tableName = 0;
	this->mapRate = 10000000.0;
	this->mixedData = MixedData::AllData;
	i = strCnt;
	while (i-- > 0)
	{
		maxStrLen[i] = 0;
		this->colNames[i] = Text::String::OrEmpty(colNames->GetItem(i))->Clone().Ptr();
		if (i < colInfos->GetCount())
		{
			cols[i] = colInfos->GetItem(i);
		}
		else
		{
			cols[i].colType = DB::DBUtil::CT_VarUTF8Char;
			cols[i].colSize = 256;
			cols[i].colDP = 0;
		}
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
			SDEL_STRING(this->colNames[i]);
		}
		MemFree(this->colNames);
		this->colNames = 0;
	}
	i = this->vectorList.GetCount();
	while (i-- > 0)
	{
		this->vectorList.GetItem(i).Delete();
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
	SDEL_STRING(this->tableName);
}

Map::DrawLayerType Map::VectorLayer::GetLayerType() const
{
	return this->layerType;
}

void Map::VectorLayer::SetMixedData(MixedData mixedData)
{
	this->mixedData = mixedData;
}

UOSInt Map::VectorLayer::GetAllObjectIds(NN<Data::ArrayListInt64> outArr, NameArray **nameArr)
{
	UOSInt i = 0;
	UOSInt j = this->vectorList.GetCount();
	if (this->layerType == Map::DRAW_LAYER_MIXED && this->mixedData != MixedData::AllData)
	{
		NN<Math::Geometry::Vector2D> vec;
		UOSInt ret = 0;
		while (i < j)
		{
			if (this->vectorList.GetItem(i).SetTo(vec) && Math::Geometry::Vector2D::VectorTypeIsPoint(vec->GetVectorType()) == (this->mixedData == MixedData::PointOnly))
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

UOSInt Map::VectorLayer::GetObjectIds(NN<Data::ArrayListInt64> outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty)
{
	return GetObjectIdsMapXY(outArr, nameArr, rect.ToDouble() / mapRate, keepEmpty);
}

UOSInt Map::VectorLayer::GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty)
{
	Math::RectAreaDbl vBounds;
	UOSInt recCnt = 0;
	NN<Math::Geometry::Vector2D> vec;
	UOSInt i;
	UOSInt j;
	if (this->layerType == Map::DRAW_LAYER_MIXED && this->mixedData != MixedData::AllData)
	{
		i = 0;
		j = this->vectorList.GetCount();
		while (i < j)
		{
			if (this->vectorList.GetItem(i).SetTo(vec) && Math::Geometry::Vector2D::VectorTypeIsPoint(vec->GetVectorType()) == (this->mixedData == MixedData::PointOnly))
			{
				vBounds = vec->GetBounds();
				if (rect.min.x <= vBounds.max.x && rect.min.y <= vBounds.max.y && rect.max.x >= vBounds.min.x && rect.max.y >= vBounds.min.y)
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
			if (this->vectorList.GetItem(i).SetTo(vec))
			{
				vBounds = vec->GetBounds();
				if (rect.min.x <= vBounds.max.x && rect.min.y <= vBounds.max.y && rect.max.x >= vBounds.min.x && rect.max.y >= vBounds.min.y)
				{
					recCnt++;
					outArr->Add((Int64)i);
				}
			}
			i++;
		}
	}
	return recCnt;
}

Int64 Map::VectorLayer::GetObjectIdMax() const
{
	return (Int64)this->vectorList.GetCount() - 1;
}

void Map::VectorLayer::ReleaseNameArr(NameArray *nameArr)
{
}

Bool Map::VectorLayer::GetString(NN<Text::StringBuilderUTF8> sb, NameArray *nameArr, Int64 id, UOSInt strIndex)
{
	if (id < 0 || (UInt64)id >= this->strList.GetCount())
	{
		return false;
	}
	if (strIndex >= this->strCnt)
	{
		return false;
	}
	const UTF8Char **strs = this->strList.GetItem((UOSInt)id);
	if (strs[strIndex] == 0)
	{
		return false;
	}
	sb->AppendSlow(strs[strIndex]);
	return true;
}

UOSInt Map::VectorLayer::GetColumnCnt() const
{
	return this->strCnt;
}

UTF8Char *Map::VectorLayer::GetColumnName(UTF8Char *buff, UOSInt colIndex)
{
	if (colIndex >= this->strCnt)
		return 0;
	Text::String *name = this->colNames[colIndex];
	if (name)
		return name->ConcatTo(buff);
	else
		return 0;
}


DB::DBUtil::ColType Map::VectorLayer::GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize)
{
	if (colIndex >= this->strCnt)
		return DB::DBUtil::CT_Unknown;
	if (colSize.IsNotNull())
	{
		if (this->cols)
		{
			colSize.SetNoCheck(this->cols[colIndex].colSize);
		}
		else
		{
			colSize.SetNoCheck(this->maxStrLen[colIndex]);
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

Bool Map::VectorLayer::GetColumnDef(UOSInt colIndex, NN<DB::ColDef> colDef)
{
	if (colIndex >= this->strCnt)
		return false;
	colDef->SetColName(Text::String::OrEmpty(this->colNames[colIndex]));
	if (this->cols)
	{
		colDef->SetColType(this->cols[colIndex].colType);
		colDef->SetColSize(this->cols[colIndex].colSize);
		colDef->SetColDP(this->cols[colIndex].colDP);
		colDef->SetAttr(CSTR_NULL);
		colDef->SetDefVal(CSTR_NULL);
		colDef->SetAutoIncNone();
		colDef->SetNotNull(false);
		colDef->SetPK(false);
	}
	else
	{
		colDef->SetColType(DB::DBUtil::CT_VarUTF8Char);
		colDef->SetColSize(this->maxStrLen[colIndex]);
		colDef->SetColDP(0);
		colDef->SetAttr(CSTR_NULL);
		colDef->SetDefVal(CSTR_NULL);
		colDef->SetAutoIncNone();
		colDef->SetNotNull(false);
		colDef->SetPK(false);
	}
	return true;
}

UInt32 Map::VectorLayer::GetCodePage() const
{
	return 0;
}

Bool Map::VectorLayer::GetBounds(OutParam<Math::RectAreaDbl> bounds) const
{
	bounds.Set(Math::RectAreaDbl(this->min, this->max));
	return this->min.x != 0 || this->min.y != 0 || this->max.x != 0 || this->max.y != 0;
}

Map::GetObjectSess *Map::VectorLayer::BeginGetObject()
{
	return (Map::GetObjectSess*)-1;
}

void Map::VectorLayer::EndGetObject(Map::GetObjectSess *session)
{
}

Math::Geometry::Vector2D *Map::VectorLayer::GetNewVectorById(Map::GetObjectSess *session, Int64 id)
{
	NN<Math::Geometry::Vector2D> vec;
	if (this->vectorList.GetItem((UOSInt)id).SetTo(vec))
	{
		return vec->Clone().Ptr();
	}
	else
	{
		return 0;
	}
}

void Map::VectorLayer::SetTableName(Text::String *tableName)
{
	SDEL_STRING(this->tableName);
	if (tableName)
	{
		this->tableName = tableName->Clone().Ptr();
	}
}

UOSInt Map::VectorLayer::QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names)
{
	if (schemaName.leng != 0)
		return 0;
	if (this->tableName)
	{
		names->Add(this->tableName->Clone());
	}
	else
	{
		names->Add(this->sourceName->Clone());
	}
	return 1;
}

Map::MapDrawLayer::ObjectClass Map::VectorLayer::GetObjectClass() const
{
	return Map::MapDrawLayer::OC_VECTOR_LAYER;
}

Bool Map::VectorLayer::VectorValid(NN<Math::Geometry::Vector2D> vec)
{
	Math::Geometry::Vector2D::VectorType vecType;
	switch (this->layerType)
	{
	case Map::DRAW_LAYER_POINT:
		vecType = vec->GetVectorType();
		if (vecType != Math::Geometry::Vector2D::VectorType::Point)
			return false;
		break;
	case Map::DRAW_LAYER_POINT3D:
		vecType = vec->GetVectorType();
		if (vecType != Math::Geometry::Vector2D::VectorType::Point || !vec->HasZ())
			return false;
		break;
	case Map::DRAW_LAYER_POLYLINE:
		vecType = vec->GetVectorType();
		if (vecType != Math::Geometry::Vector2D::VectorType::Polyline && vecType != Math::Geometry::Vector2D::VectorType::LineString)
			return false;
		break;
	case Map::DRAW_LAYER_POLYLINE3D:
		vecType = vec->GetVectorType();
		if ((vecType != Math::Geometry::Vector2D::VectorType::Polyline && vecType != Math::Geometry::Vector2D::VectorType::LineString) || !vec->HasZ())
			return false;
		break;
	case Map::DRAW_LAYER_POLYGON:
		vecType = vec->GetVectorType();
		if (vecType != Math::Geometry::Vector2D::VectorType::Polygon && vecType != Math::Geometry::Vector2D::VectorType::MultiPolygon)
			return false;
		break;
	case Map::DRAW_LAYER_IMAGE:
		vecType = vec->GetVectorType();
		if (vecType != Math::Geometry::Vector2D::VectorType::Image)
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

Bool Map::VectorLayer::AddVector(NN<Math::Geometry::Vector2D> vec, Text::String **strs)
{
	if (!this->VectorValid(vec))
		return false;
	const UTF8Char **newStrs = CopyStrs(strs);

	Math::RectAreaDbl bounds;
	Bool updated = false;
	bounds = vec->GetBounds();
	if (this->vectorList.GetCount() == 0)
	{
		this->min = bounds.min;
		this->max = bounds.max;
		updated = true;
	}
	else
	{
		if (this->min.x > bounds.min.x)
		{
			this->min.x = bounds.min.x;
			updated = true;
		}
		if (this->min.y > bounds.min.y)
		{
			this->min.y = bounds.min.y;
			updated = true;
		}
		if (this->max.x < bounds.max.x)
		{
			this->max.x = bounds.max.x;
			updated = true;
		}
		if (this->max.y < bounds.max.y)
		{
			this->max.y = bounds.max.y;
			updated = true;
		}
	}

	this->vectorList.Add(vec);
	this->strList.Add(newStrs);
	if (updated)
		this->UpdateMapRate();
	return true;
}

Bool Map::VectorLayer::AddVector(NN<Math::Geometry::Vector2D> vec, Text::PString *strs)
{
	if (!this->VectorValid(vec))
		return false;
	const UTF8Char **newStrs = CopyStrs(strs);

	Math::RectAreaDbl bounds;
	Bool updated = false;
	bounds = vec->GetBounds();
	if (this->vectorList.GetCount() == 0)
	{
		this->min = bounds.min;
		this->max = bounds.max;
		updated = true;
	}
	else
	{
		if (this->min.x > bounds.min.x)
		{
			this->min.x = bounds.min.x;
			updated = true;
		}
		if (this->min.y > bounds.min.y)
		{
			this->min.y = bounds.min.y;
			updated = true;
		}
		if (this->max.x < bounds.max.x)
		{
			this->max.x = bounds.max.x;
			updated = true;
		}
		if (this->max.y < bounds.max.y)
		{
			this->max.y = bounds.max.y;
			updated = true;
		}
	}

	this->vectorList.Add(vec);
	this->strList.Add(newStrs);
	if (updated)
		this->UpdateMapRate();
	return true;
}

Bool Map::VectorLayer::AddVector(NN<Math::Geometry::Vector2D> vec, const UTF8Char **strs)
{
	if (!this->VectorValid(vec))
		return false;
	const UTF8Char **newStrs = CopyStrs(strs);

	Math::RectAreaDbl bounds;
	Bool updated = false;
	bounds = vec->GetBounds();
	if (this->vectorList.GetCount() == 0)
	{
		this->min = bounds.min;
		this->max = bounds.max;
		updated = true;
	}
	else
	{
		if (this->min.x > bounds.min.x)
		{
			this->min.x = bounds.min.x;
			updated = true;
		}
		if (this->min.y > bounds.min.y)
		{
			this->min.y = bounds.min.y;
			updated = true;
		}
		if (this->max.x < bounds.max.x)
		{
			this->max.x = bounds.max.x;
			updated = true;
		}
		if (this->max.y < bounds.max.y)
		{
			this->max.y = bounds.max.y;
			updated = true;
		}
	}

	this->vectorList.Add(vec);
	this->strList.Add(newStrs);
	if (updated)
		this->UpdateMapRate();
	return true;
}

Bool Map::VectorLayer::AddVector(NN<Math::Geometry::Vector2D> vec, NN<Data::ArrayListStringNN> strs)
{
	if (!this->VectorValid(vec))
		return false;
	const UTF8Char **newStrs = CopyStrs(strs);

	Math::RectAreaDbl bounds;
	Bool updated = false;
	bounds = vec->GetBounds();
	if (this->vectorList.GetCount() == 0)
	{
		this->min = bounds.min;
		this->max = bounds.max;
		updated = true;
	}
	else
	{
		if (this->min.x > bounds.min.x)
		{
			this->min.x = bounds.min.x;
			updated = true;
		}
		if (this->min.y > bounds.min.y)
		{
			this->min.y = bounds.min.y;
			updated = true;
		}
		if (this->max.x < bounds.max.x)
		{
			this->max.x = bounds.max.x;
			updated = true;
		}
		if (this->max.y < bounds.max.y)
		{
			this->max.y = bounds.max.y;
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
	objId = this->GetNearestObjectId(0, pt, nearPt);
	if (objId < 0)
		return false;

	NN<Math::Geometry::Polyline> pl;
	if (Optional<Math::Geometry::Polyline>::ConvertFrom(this->vectorList.GetItem((UOSInt)objId)).SetTo(pl))
	{
		NN<Math::Geometry::Polyline> pl2;
		if (pl2.Set(pl->SplitByPoint(pt)))
		{
			this->vectorList.Add(pl2);
			this->strList.Add(CopyStrs(this->strList.GetItem((UOSInt)objId)));
		}
	}
	return false;
}

void Map::VectorLayer::OptimizePolylinePath()
{
	if (this->layerType != Map::DRAW_LAYER_POLYLINE)
		return;
	const UTF8Char **tmpStr;
	NN<Math::Geometry::Polyline> tmpPL;
	NN<Math::Geometry::LineString> tmpLS;

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
			if (Optional<Math::Geometry::Polyline>::ConvertFrom(this->vectorList.RemoveAt(i)).SetTo(tmpPL) && tmpPL->GetItem(0).SetTo(tmpLS))
			{
				points = tmpLS->GetPointList(nPoints);
				pt = *points;
				objId = this->GetNearestObjectId(0, pt, nearPt);
				if (objId >= 0)
				{
					ix = (Int32)(pt.x * 200000.0);
					iy = (Int32)(pt.y * 200000.0);
					nix = (Int32)(nearPt.x * 200000.0);
					niy = (Int32)(nearPt.y * 200000.0);
					if (ix == nix && iy == niy)
					{
						NN<Math::Geometry::Polyline> pl;
						NN<Math::Geometry::Polyline> pl2;
						if (Optional<Math::Geometry::Polyline>::ConvertFrom(this->vectorList.GetItem((UOSInt)objId)).SetTo(pl) && pl2.Set(pl->SplitByPoint(pt)))
						{
							this->vectorList.Add(pl2);
							this->strList.Add(CopyStrs(this->strList.GetItem((UOSInt)objId)));
							found = true;
						}
					}
				}

				pt = points[nPoints - 1];
				objId = this->GetNearestObjectId(0, pt, nearPt);
				if (objId >= 0)
				{
					ix = (Int32)(pt.x * 200000.0);
					iy = (Int32)(pt.y * 200000.0);
					nix = (Int32)(nearPt.x * 200000.0);
					niy = (Int32)(nearPt.y * 200000.0);
					if (ix == nix && iy == niy)
					{
						NN<Math::Geometry::Polyline> pl;
						NN<Math::Geometry::Polyline> pl2;
						if (Optional<Math::Geometry::Polyline>::ConvertFrom(this->vectorList.GetItem((UOSInt)objId)).SetTo(pl) && pl2.Set(pl->SplitByPoint(pt)))
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

void Map::VectorLayer::ReplaceVector(Int64 id, NN<Math::Geometry::Vector2D> vec)
{
	if (this->vectorList.GetCount() <= (UInt64)id)
	{
		this->vectorList.Add(vec);
	}
	else
	{
		this->vectorList.GetItem((UOSInt)id).Delete();
		this->vectorList.SetItem((UOSInt)id, vec);
	}
}

void Map::VectorLayer::ConvCoordinateSystem(NN<Math::CoordinateSystem> destCsys)
{
	Math::CoordinateSystemConverter converter(this->csys, destCsys);
	Data::ArrayIterator<NN<Math::Geometry::Vector2D>> it = this->vectorList.Iterator();
	while (it.HasNext())
	{
		it.Next()->Convert(converter);
	}

	this->min = converter.Convert2D(this->min);
	this->max = converter.Convert2D(this->max);
	this->UpdateMapRate();

	this->csys.Delete();
	this->csys = destCsys;
}

void Map::VectorLayer::SwapXY()
{
	Data::ArrayIterator<NN<Math::Geometry::Vector2D>> it = this->vectorList.Iterator();
	while (it.HasNext())
	{
		it.Next()->SwapXY();
	}
	this->min = this->min.SwapXY();
	this->max = this->max.SwapXY();
}
