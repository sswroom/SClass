#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Data/ArrayListICaseStrUTF8.h"
#include "Data/ArrayListInt32.h"
#include "DB/ColDef.h"
#include "Map/LayerTools.h"

Map::VectorLayer *Map::LayerTools::CombineLayers(Data::ArrayList<Map::MapDrawLayer*> *layers, Text::String *lyrName)
{
	UOSInt layerCnt = layers->GetCount();
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	OSInt si;
	Map::MapDrawLayer *lyr;
	UTF8Char sbuff[512];
	NotNullPtr<Text::String> sourceName;

	if (layerCnt <= 0)
		return 0;

	lyr = layers->GetItem(0);
	Map::DrawLayerType lyrType = lyr->GetLayerType();
	sourceName = lyr->GetSourceNameObj();
	NotNullPtr<Math::CoordinateSystem> csys = lyr->GetCoordinateSystem();
	i = layerCnt;
	while (i-- > 0)
	{
		lyr = layers->GetItem(i);
		if (lyr->GetLayerType() != lyrType)
			return 0;
	}

	Data::ArrayListStrUTF8 names;
	Data::ArrayListICaseStrUTF8 nameIndex;
	Data::ArrayList<DB::DBUtil::ColType> colTypeArr;
	Data::ArrayList<UOSInt> nameSizes;
	Data::ArrayList<UOSInt> nameDPs;
	DB::ColDef colDef(CSTR_NULL);
	NotNullPtr<DB::DBReader> r;

	i = 0;
	while (i < layerCnt)
	{
		lyr = layers->GetItem(i);
		if (r.Set(lyr->QueryTableData(CSTR_NULL, CSTR_NULL, 0, 0, 0, CSTR_NULL, 0)))
		{
			k = r->ColCount();
			j = 0;
			while (j < k)
			{
				lyr->GetColumnName(sbuff, j);
				r->GetColDef(j, colDef);
				si = nameIndex.SortedIndexOf(sbuff);
				if (si >= 0)
				{
					if (nameSizes.GetItem(i) < colDef.GetColSize())
					{
						nameSizes.SetItem(i, colDef.GetColSize());
					}
				}
				else
				{
					const UTF8Char *name = Text::StrCopyNew(sbuff).Ptr();
					nameIndex.Insert((UOSInt)~si, name);
					nameSizes.Insert((UOSInt)~si, colDef.GetColSize());
					nameDPs.Insert((UOSInt)~si, colDef.GetColDP());
					colTypeArr.Insert((UOSInt)~si, colDef.GetColType());
					names.Add(name);
				}
				j++;
			}

			lyr->CloseReader(r);
		}
		i++;
	}

	Map::VectorLayer *newLyr;
	const UTF8Char **namesArr;
	UOSInt *ofsts;
	DB::DBUtil::ColType *colTypes;
	UOSInt *colSizes;
	UOSInt *colDPs;
	UOSInt nameCol = lyr->GetNameCol();
	i = names.GetCount();
	namesArr = MemAlloc(const UTF8Char *, i);
	ofsts = MemAlloc(UOSInt, i);
	colTypes = MemAlloc(DB::DBUtil::ColType, i);
	colSizes = MemAlloc(UOSInt, i);
	colDPs = MemAlloc(UOSInt, i);
	while (i-- > 0)
	{
		namesArr[i] = names.GetItem(i);
		si = nameIndex.SortedIndexOf(namesArr[i]);
		colTypes[i] = colTypeArr.GetItem((UOSInt)si);
		colSizes[i] = nameSizes.GetItem((UOSInt)si);
		colDPs[i] = nameDPs.GetItem((UOSInt)si);
	}
	NEW_CLASS(newLyr, Map::VectorLayer(lyrType, sourceName, names.GetCount(), namesArr, csys->Clone(), colTypes, colSizes, colDPs, nameCol, lyrName));
	MemFree(colTypes);
	MemFree(colSizes);

	Data::ArrayListInt64 objIds;
	i = 0;
	while (i < layerCnt)
	{
		lyr = layers->GetItem(i);

		k = lyr->GetColumnCnt();
		j = 0;
		while (j < k)
		{
			lyr->GetColumnName(sbuff, j);
			l = names.GetCount();
			while (l-- > 0)
			{
				if (Text::StrCompareICase(names.GetItem(l), sbuff) == 0)
				{
					colDPs[j] = l;
					break;
				}
			}
			j++;
		}

		l = names.GetCount();
		while (l-- > 0)
		{
			namesArr[l] = 0;
			ofsts[l] = INVALID_INDEX;
		}

		Map::NameArray *lyrNameArr;
		Map::GetObjectSess *sess;
		Int64 id;
		Text::StringBuilderUTF8 sb;

		objIds.Clear();
		sess = lyr->BeginGetObject();
		lyr->GetAllObjectIds(objIds, &lyrNameArr);
		l = objIds.GetCount();
		while (l-- > 0)
		{
			id = objIds.GetItem(l);
			sb.ClearStr();
			j = 0;
			while (j < k)
			{
				ofsts[colDPs[j]] = sb.leng;
				if (lyr->GetString(sb, lyrNameArr, id, j))
				{
					sb.AppendUTF8Char(0);
				}
				else
				{
					ofsts[colDPs[j]] = INVALID_INDEX;
				}

				j++;
			}
			NotNullPtr<Math::Geometry::Vector2D> vec;
			if (vec.Set(lyr->GetNewVectorById(sess, id)))
			{
				j = names.GetCount();
				while (j-- > 0)
				{
					if (ofsts[j] == INVALID_INDEX)
						namesArr[j] = 0;
					else
						namesArr[j] = &sb.v[ofsts[j]];
				}
				newLyr->AddVector(vec, namesArr);
			}
		}
		lyr->EndGetObject(sess);
		i++;
	}
	MemFree(colDPs);
	MemFree(ofsts);
	MemFree(namesArr);

	i = names.GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(names.GetItem(i));
	}
	return newLyr;
}
