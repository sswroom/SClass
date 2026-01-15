#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/StreamReader.h"
#include "Map/MapSearch.h"
#include "Math/GeometryTool.h"
#include "Sync/Event.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"

Map::MapSearch::MapSearch(Text::CStringNN fileName, NN<Map::MapSearchManager> manager)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UTF8Char sbuff2[256];
	UnsafeArray<UTF8Char> tmp;
	Text::PString strs[5];
	UOSInt i;
	Int32 layerId;
	Int32 layerType;
	Double layerDist;
	NN<Text::String> baseDir;


	this->baseDir = nullptr;
	this->concatType = 0;
	this->layersArr = MemAllocArr(NN<Data::ArrayListNN<Map::MapSearchLayerInfo>>, MAPSEARCH_LAYER_TYPES);
	i = MAPSEARCH_LAYER_TYPES;
	while (i-- > 0)
	{
		NEW_CLASSNN(this->layersArr[i], Data::ArrayListNN<Map::MapSearchLayerInfo>());
	}

	IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	IO::StreamReader reader(fs);
	while (reader.ReadLine(sbuff, 256).SetTo(sptr))
	{
		i = Text::StrSplitP(strs, 5, {sbuff, (UOSInt)(sptr - sbuff)}, ',');
		/*
		layerType
		0: baseDir
		1: Near border
		2: Inside object
		3: Concat Type
*/

		if (i == 2)
		{
			if (strs[0].ToInt32(layerType))
			{
				if (layerType == 0)
				{
					this->baseDir = Text::String::New(strs[1].v, strs[1].leng);
				}
				else if (layerType == 3)
				{
					this->concatType = strs[1].ToInt32();
				}
			}
		}
		else if (i == 3)
		{
			layerId = strs[1].ToInt32();
			layerType = strs[0].ToInt32();
			if (this->baseDir.SetTo(baseDir) && layerId >= 0 && layerId < MAPSEARCH_LAYER_TYPES)
			{
				NN<Map::MapSearchLayerInfo> lyr;
				tmp = baseDir->ConcatTo(sbuff2);
				tmp = strs[2].ConcatTo(tmp);
				lyr = MemAllocNN(Map::MapSearchLayerInfo);
				lyr->searchType = layerType;
				lyr->searchDist = 0;
				lyr->mapLayer = manager->LoadLayer(CSTRP(sbuff2, tmp));
				lyr->searchStr = nullptr;
				lyr->strIndex = 0;
				this->layersArr[layerId]->Add(lyr);
			}

		}
		else if (i == 4)
		{
			layerId = strs[1].ToInt32();
			layerType = strs[0].ToInt32();
			layerDist = strs[3].ToDoubleOr(0);
			if (this->baseDir.SetTo(baseDir) && layerId >= 0 && layerId < MAPSEARCH_LAYER_TYPES)
			{
				NN<Map::MapSearchLayerInfo> lyr;
				tmp = baseDir->ConcatTo(sbuff2);
				tmp = strs[2].ConcatTo(tmp);
				lyr = MemAllocNN(Map::MapSearchLayerInfo);
				lyr->searchType = layerType;
				lyr->searchDist = layerDist;
				lyr->mapLayer = manager->LoadLayer(CSTRP(sbuff2, tmp));
				lyr->searchStr = nullptr;
				lyr->strIndex = 0;
				this->layersArr[layerId]->Add(lyr);
			}

		}
		else if (i == 5)
		{
			layerId = strs[1].ToInt32();
			layerType = strs[0].ToInt32();
			layerDist = strs[3].ToDoubleOr(0);
			if (this->baseDir.SetTo(baseDir) && layerId >= 0 && layerId < MAPSEARCH_LAYER_TYPES)
			{
				NN<Map::MapSearchLayerInfo> lyr;
				tmp = baseDir->ConcatTo(sbuff2);
				tmp = strs[2].ConcatTo(tmp);
				lyr = MemAllocNN(Map::MapSearchLayerInfo);
				lyr->searchType = layerType;
				lyr->searchDist = layerDist;
				lyr->mapLayer = manager->LoadLayer(CSTRP(sbuff2, tmp));
				if (strs[4].v[0] == 0)
				{
					lyr->searchStr = nullptr;
				}
				else
				{
					lyr->searchStr = Text::String::New(strs[4].v, strs[4].leng).Ptr();
				}
				lyr->strIndex = 0;
				this->layersArr[layerId]->Add(lyr);
			}

		}
	}
}

Map::MapSearch::~MapSearch()
{
	UOSInt i;
	UOSInt j;
	OPTSTR_DEL(this->baseDir);
	i = MAPSEARCH_LAYER_TYPES;
	while (i-- > 0)
	{
		j = this->layersArr[i]->GetCount();
		while (j-- > 0)
		{
			NN<Map::MapSearchLayerInfo> lyr = this->layersArr[i]->GetItemNoCheck(j);
			OPTSTR_DEL(lyr->searchStr);
			MemFreeNN(lyr);
			this->layersArr[i]->RemoveAt(j);
		}
		this->layersArr[i].Delete();
	}
	MemFreeArr(this->layersArr);
}

UnsafeArrayOpt<UTF8Char> Map::MapSearch::SearchName(UnsafeArray<UTF8Char> buff, Math::Coord2DDbl pos)
{
	UTF8Char sbuff[1024];
	Text::PString outArrs[MAPSEARCH_LAYER_TYPES];
	Math::Coord2DDbl outPos[MAPSEARCH_LAYER_TYPES];
	Int32 resTypes[MAPSEARCH_LAYER_TYPES];
	SearchNames(sbuff, outArrs, outPos, resTypes, pos);
	return ConcatNames(buff, outArrs, 0);
}

Int32 Map::MapSearch::SearchNames(UnsafeArray<UTF8Char> buff, UnsafeArray<Text::PString> outArrs, UnsafeArray<Math::Coord2DDbl> outPos, UnsafeArray<Int32> resTypes, Math::Coord2DDbl pos)
{
	Text::StringBuilderUTF8 sbTmp;
	UnsafeArrayOpt<UTF8Char> sptr;
	UnsafeArray<UTF8Char> nnsptr;
	UnsafeArray<UTF8Char> outptr;
	Int32 resType;
	OSInt i;
	UOSInt j;
	UInt32 k;
	Int32 l = 0;
	Double thisDist;
	Double minDist;
	Math::Coord2DDbl posNear;
	NN<Text::String> searchStr;

	outptr = buff;
	*outptr = 0;
	i = MAPSEARCH_LAYER_TYPES;
	while (i-- > 0)
	{
		posNear = Math::Coord2DDbl(0, 0);
		resType = 0;
		sptr = nullptr;
		minDist = 63781370;

		j = this->layersArr[i]->GetCount();
		k = 0;
		while (k < j)
		{
			NN<Map::MapSearchLayerInfo> lyr = this->layersArr[i]->GetItemNoCheck(k++);
			if (lyr->searchType == 2)
			{
				sbTmp.ClearStr();
				if (lyr->mapLayer->GetPGLabel(sbTmp, pos, 0, lyr->strIndex))
				{
					if (lyr->searchStr.SetTo(searchStr))
					{
						sptr = sbTmp.ConcatTo(searchStr->ConcatTo(outptr));
					}
					else
					{
						sptr = sbTmp.ConcatTo(outptr);
					}
					posNear = pos;
					resType = 2;
					break;
				}
			}
			else if (lyr->searchType == 1)
			{
				Math::Coord2DDbl posout;
				sbTmp.ClearStr();
				if (lyr->mapLayer->GetPLLabel(sbTmp, pos, posout, lyr->strIndex))
				{
					Math::Coord2DDbl tmp = posout - pos;
					tmp = tmp * tmp;
					thisDist = tmp.x + tmp.y;
					if (lyr->searchDist)
					{
						Double meterDist = Math::GeometryTool::SphereDistDeg(pos.GetLat(), pos.GetLon(), posout.GetLat(), posout.GetLon(), 6378137.0);
						if (meterDist < lyr->searchDist)
						{
							minDist = thisDist;
							if (lyr->searchStr.SetTo(searchStr))
							{
								sptr = sbTmp.ConcatTo(searchStr->ConcatTo(outptr));
							}
							else
							{
								sptr = sbTmp.ConcatTo(outptr);
							}
							posNear = posout;
							resType = 1;
							break;
						}
					}
					else if (thisDist < minDist)
					{
						minDist = thisDist;
						if (lyr->searchStr.SetTo(searchStr))
						{
							sptr = sbTmp.ConcatTo(searchStr->ConcatTo(outptr));
						}
						else
						{
							sptr = sbTmp.ConcatTo(outptr);
						}
						posNear = posout;
						resType = 1;
					}
				}
			}
		}

		outPos[i] = posNear;
		resTypes[i] = resType;
		if (sptr.SetTo(nnsptr) && *outptr)
		{
			outArrs[i].v = outptr;
			outArrs[i].leng = (UOSInt)(nnsptr - outptr);
			outptr = nnsptr + 1;
			l++;
		}
		else
		{
			outArrs[i].v = outptr;
			outArrs[i].leng = 0;
		}
	}
	return l;
}

UnsafeArrayOpt<UTF8Char> Map::MapSearch::ConcatNames(UnsafeArray<UTF8Char> buff, UnsafeArray<Text::PString> strArrs, Int32 concatType)
{
	UnsafeArrayOpt<UTF8Char> outptr = nullptr;
	UnsafeArray<UTF8Char> nnoutptr;
	UOSInt i = 0;
	UTF8Char sbufftmp[128];
	UnsafeArray<UTF8Char> sptrtmp;
	Text::PString stmp[2];
	while (i < MAPSEARCH_LAYER_TYPES)
	{
		if (strArrs[i].leng != 0)
		{
			outptr = strArrs[i].v;
			break;
		}
		i++;
	}
	if (!outptr.SetTo(nnoutptr))
	{
		*buff = 0;
		return nullptr;
	}
	Int32 langType = 0;
	while (*nnoutptr)
	{
		if (*nnoutptr++ >= 128)
		{
			langType = 1;
			break;
		}
	}

	nnoutptr = buff;
	buff[0] = 0;
	if (concatType == 1 || (concatType == 0 && langType == 0))
	{
		if (strArrs[3].leng == 0)
		{
			if (strArrs[1].leng == 0)
			{
				if (strArrs[0].leng == 0)
				{
					if (strArrs[2].leng == 0)
					{
						if (strArrs[6].leng == 0)
						{
							*nnoutptr = 0;
						}
						else
						{
							nnoutptr = strArrs[6].ConcatTo(nnoutptr);
						}
					}
					else
					{
						nnoutptr = strArrs[2].ConcatTo(nnoutptr);
					}
				}
				else
				{
					if (strArrs[2].leng != 0)
					{
						nnoutptr = strArrs[2].ConcatTo(nnoutptr);
						nnoutptr = Text::StrConcatC(nnoutptr, UTF8STRC(", "));
					}
					nnoutptr = strArrs[0].ConcatTo(nnoutptr);
				}
			}
			else
			{
                if (strArrs[2].leng != 0)
				{
					nnoutptr = strArrs[2].ConcatTo(nnoutptr);
					nnoutptr = Text::StrConcatC(nnoutptr, UTF8STRC(", "));
				}
				nnoutptr = strArrs[1].ConcatTo(nnoutptr);
			}
		}
		else
		{
			if (strArrs[2].leng == 0)
			{
				nnoutptr = Text::StrConcatC(nnoutptr, UTF8STRC("Near "));
				nnoutptr = strArrs[3].ConcatTo(nnoutptr);
			}
			else
			{
				i = strArrs[2].IndexOf(' ');
				if (i != INVALID_INDEX)
					sptrtmp = Text::StrConcatC(sbufftmp, strArrs[2].v, (UOSInt)i);
				else
					sptrtmp = strArrs[2].ConcatTo(sbufftmp);

				i = strArrs[3].IndexOf(sbufftmp, (UOSInt)(sptrtmp - sbufftmp));
				if (i != INVALID_INDEX)
				{
					nnoutptr = strArrs[3].ConcatTo(nnoutptr);
				}
				else
				{
					if (strArrs[1].leng == 0)
					{
						if (strArrs[0].leng == 0)
						{
							nnoutptr = strArrs[2].ConcatTo(nnoutptr);
						}
						else
						{
							if (strArrs[2].leng != 0)
							{
								nnoutptr = strArrs[2].ConcatTo(nnoutptr);
								nnoutptr = Text::StrConcatC(nnoutptr, UTF8STRC(", "));
							}
							nnoutptr = strArrs[0].ConcatTo(nnoutptr);
						}
					}
					else
					{
						if (strArrs[2].leng != 0)
						{
							nnoutptr = strArrs[2].ConcatTo(nnoutptr);
							nnoutptr = Text::StrConcatC(nnoutptr, UTF8STRC(", "));
						}
						nnoutptr = strArrs[1].ConcatTo(nnoutptr);
						if (strArrs[0].leng != 0)
						{
							nnoutptr = Text::StrConcatC(nnoutptr, UTF8STRC(", "));
							nnoutptr = strArrs[0].ConcatTo(nnoutptr);
						}
					}

					if (buff[0] == 0)
					{
						nnoutptr = Text::StrConcatC(nnoutptr, UTF8STRC("Near "));
						nnoutptr = strArrs[3].ConcatTo(nnoutptr);
					}
					else
					{
						nnoutptr = Text::StrConcatC(nnoutptr, UTF8STRC(", Near "));
						nnoutptr = strArrs[3].ConcatTo(nnoutptr);
					}
				}
			}
		}
	}
	else if (concatType == 2 || (concatType == 0 && langType != 0))
	{
		if (strArrs[3].leng == 0)
		{
			if (strArrs[1].leng == 0)
			{
				if (strArrs[0].leng == 0)
				{
					if (strArrs[2].leng == 0)
					{
						if (strArrs[5].leng != 0)
						{
							nnoutptr = strArrs[5].ConcatTo(nnoutptr);
						}
						else if (strArrs[6].leng != 0)
						{
							nnoutptr = strArrs[6].ConcatTo(nnoutptr);
						}
						else
						{
							*nnoutptr = 0;
						}
					}
					else
					{
						if (strArrs[5].leng != 0)
						{
							nnoutptr = strArrs[5].ConcatTo(nnoutptr);
							nnoutptr = Text::StrConcatC(nnoutptr, UTF8STRC(", "));
						}
						nnoutptr = strArrs[2].ConcatTo(nnoutptr);
					}
				}
				else
				{
					nnoutptr = strArrs[0].ConcatTo(nnoutptr);
					if (strArrs[5].leng != 0)
					{
						nnoutptr = Text::StrConcatC(nnoutptr, UTF8STRC(", "));
						nnoutptr = strArrs[5].ConcatTo(nnoutptr);
					}
					if (strArrs[2].leng != 0)
					{
						nnoutptr = Text::StrConcatC(nnoutptr, UTF8STRC(", "));
						nnoutptr = strArrs[2].ConcatTo(nnoutptr);
					}
				}
			}
			else
			{
				if (strArrs[0].leng != 0)
				{
					nnoutptr = strArrs[0].ConcatTo(nnoutptr);
					nnoutptr = Text::StrConcatC(nnoutptr, UTF8STRC(", "));
				}
				nnoutptr = strArrs[1].ConcatTo(nnoutptr);
				if (strArrs[5].leng != 0)
				{
					nnoutptr = Text::StrConcatC(nnoutptr, UTF8STRC(", "));
					nnoutptr = strArrs[5].ConcatTo(nnoutptr);
				}
				if (strArrs[2].leng != 0)
				{
					nnoutptr = Text::StrConcatC(nnoutptr, UTF8STRC(", "));
					nnoutptr = strArrs[2].ConcatTo(nnoutptr);
				}
			}
		}
		else
		{
			if (strArrs[2].leng == 0)
			{
				nnoutptr = Text::StrWriteChar(nnoutptr, 0x8FD1);
				nnoutptr = strArrs[3].ConcatTo(nnoutptr);
			}
			else
			{
				sptrtmp = strArrs[2].ConcatTo(sbufftmp);
				Text::StrSplitP(stmp, 2, {sbufftmp, (UOSInt)(sptrtmp - sbufftmp)}, ' ');
				i = Text::StrIndexOfCharC(sbufftmp, (UOSInt)(sptrtmp - sbufftmp), '-');
				if (i != INVALID_INDEX)
				{
					sptrtmp = Text::StrConcatC(sbufftmp, &sbufftmp[i + 1], (UOSInt)(sptrtmp - &sbufftmp[i + 1]));
				}

				i = strArrs[3].IndexOf(sbufftmp, (UOSInt)(sptrtmp - sbufftmp));
				if (i != INVALID_INDEX)
				{
					nnoutptr = strArrs[3].ConcatTo(nnoutptr);
				}
				else
				{
					if (strArrs[1].leng == 0)
					{
						if (strArrs[0].leng == 0)
						{
							nnoutptr = strArrs[2].ConcatTo(nnoutptr);
						}
						else
						{
							nnoutptr = strArrs[0].ConcatTo(nnoutptr);
							if (strArrs[2].leng != 0)
							{
								nnoutptr = Text::StrConcatC(nnoutptr, UTF8STRC(", "));
								nnoutptr = strArrs[2].ConcatTo(nnoutptr);
							}
						}
					}
					else
					{
						if (strArrs[0].leng != 0)
						{
							nnoutptr = strArrs[0].ConcatTo(nnoutptr);
							nnoutptr = Text::StrConcatC(nnoutptr, UTF8STRC(", "));
						}
						nnoutptr = strArrs[1].ConcatTo(nnoutptr);
						if (strArrs[2].leng != 0)
						{
							nnoutptr = Text::StrConcatC(nnoutptr, UTF8STRC(", "));
							nnoutptr = strArrs[2].ConcatTo(nnoutptr);
						}
					}

					if (buff[0] == 0)
					{
						nnoutptr = Text::StrWriteChar(nnoutptr, 0x8FD1);
						nnoutptr = strArrs[3].ConcatTo(nnoutptr);
					}
					else
					{
						nnoutptr = Text::StrConcatC(nnoutptr, UTF8STRC(", "));
						nnoutptr = Text::StrWriteChar(nnoutptr, 0x8FD1);
						nnoutptr = strArrs[3].ConcatTo(nnoutptr);
					}
				}
			}
		}
	}
	else// if (concatType == 3)
	{
		*nnoutptr = 0;
		i = 0;
		while (i < MAPSEARCH_LAYER_TYPES)
		{
			if (strArrs[i].leng != 0)
			{
				if (nnoutptr != buff)
				{
					nnoutptr = Text::StrConcatC(nnoutptr, UTF8STRC(", "));
				}
				nnoutptr = strArrs[i].ConcatTo(nnoutptr);
			}
			i++;
		}
	}
	return nnoutptr;
}

Bool Map::MapSearch::IsError()
{
	return this->baseDir.IsNull();
}

Int32 Map::MapSearch::GetConcatType()
{
	return this->concatType;
}
