#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "IO/FileStream.h"
#include "IO/StreamReader.h"
#include "Map/MapSearch.h"
#include "Math/GeometryTool.h"
#include "Sync/Event.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"

Map::MapSearch::MapSearch(Text::CStringNN fileName, Map::MapSearchManager *manager)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UTF8Char sbuff2[256];
	UTF8Char *tmp;
	Text::PString strs[5];
	UOSInt i;
	Int32 layerId;
	Int32 layerType;
	Double layerDist;


	this->baseDir = 0;
	this->concatType = 0;
	this->layersArr = MemAlloc(Data::ArrayList<Map::MapSearchLayerInfo*>*, MAPSEARCH_LAYER_TYPES);
	i = MAPSEARCH_LAYER_TYPES;
	while (i-- > 0)
	{
		NEW_CLASS(this->layersArr[i], Data::ArrayList<Map::MapSearchLayerInfo*>());
	}

	IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	IO::StreamReader reader(fs);
	sptr = reader.ReadLine(sbuff, 256);
	while (sptr)
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
					this->baseDir = Text::String::New(strs[1].v, strs[1].leng).Ptr();
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
			if (this->baseDir && layerId >= 0 && layerId < MAPSEARCH_LAYER_TYPES)
			{
				Map::MapSearchLayerInfo *lyr;
				tmp = this->baseDir->ConcatTo(sbuff2);
				tmp = strs[2].ConcatTo(tmp);
				lyr = MemAlloc(Map::MapSearchLayerInfo, 1);
				lyr->searchType = layerType;
				lyr->searchDist = 0;
				lyr->mapLayer = manager->LoadLayer(CSTRP(sbuff2, tmp));
				lyr->searchStr = 0;
				lyr->strIndex = 0;
				this->layersArr[layerId]->Add(lyr);
			}

		}
		else if (i == 4)
		{
			layerId = strs[1].ToInt32();
			layerType = strs[0].ToInt32();
			layerDist = strs[3].ToDouble();
			if (this->baseDir && layerId >= 0 && layerId < MAPSEARCH_LAYER_TYPES)
			{
				Map::MapSearchLayerInfo *lyr;
				tmp = this->baseDir->ConcatTo(sbuff2);
				tmp = strs[2].ConcatTo(tmp);
				lyr = MemAlloc(Map::MapSearchLayerInfo, 1);
				lyr->searchType = layerType;
				lyr->searchDist = layerDist;
				lyr->mapLayer = manager->LoadLayer(CSTRP(sbuff2, tmp));
				lyr->searchStr = 0;
				lyr->strIndex = 0;
				this->layersArr[layerId]->Add(lyr);
			}

		}
		else if (i == 5)
		{
			layerId = strs[1].ToInt32();
			layerType = strs[0].ToInt32();
			layerDist = strs[3].ToDouble();
			if (this->baseDir && layerId >= 0 && layerId < MAPSEARCH_LAYER_TYPES)
			{
				Map::MapSearchLayerInfo *lyr;
				tmp = this->baseDir->ConcatTo(sbuff2);
				tmp = strs[2].ConcatTo(tmp);
				lyr = MemAlloc(Map::MapSearchLayerInfo, 1);
				lyr->searchType = layerType;
				lyr->searchDist = layerDist;
				lyr->mapLayer = manager->LoadLayer(CSTRP(sbuff2, tmp));
				if (strs[4].v[0] == 0)
				{
					lyr->searchStr = 0;
				}
				else
				{
					lyr->searchStr = Text::String::New(strs[4].v, strs[4].leng).Ptr();
				}
				lyr->strIndex = 0;
				this->layersArr[layerId]->Add(lyr);
			}

		}
		sptr = reader.ReadLine(sbuff, 256);
	}
}

Map::MapSearch::~MapSearch()
{
	UOSInt i;
	UOSInt j;
	if (this->baseDir)
	{
		this->baseDir->Release();
		this->baseDir = 0;
	}
	i = MAPSEARCH_LAYER_TYPES;
	while (i-- > 0)
	{
		j = this->layersArr[i]->GetCount();
		while (j-- > 0)
		{
			Map::MapSearchLayerInfo *lyr = (Map::MapSearchLayerInfo*)this->layersArr[i]->RemoveAt(j);
			if (lyr->searchStr)
			{
				lyr->searchStr->Release();
			}
			MemFree(lyr);
		}
		DEL_CLASS(this->layersArr[i]);
	}
	MemFree(this->layersArr);
}

UTF8Char *Map::MapSearch::SearchName(UTF8Char *buff, Math::Coord2DDbl pos)
{
	UTF8Char sbuff[1024];
	Text::PString outArrs[MAPSEARCH_LAYER_TYPES];
	Math::Coord2DDbl outPos[MAPSEARCH_LAYER_TYPES];
	Int32 resTypes[MAPSEARCH_LAYER_TYPES];
	SearchNames(sbuff, outArrs, outPos, resTypes, pos);
	UTF8Char *ptr = ConcatNames(buff, outArrs, 0);
	return ptr;
}

Int32 Map::MapSearch::SearchNames(UTF8Char *buff, Text::PString *outArrs, Math::Coord2DDbl *outPos, Int32 *resTypes, Math::Coord2DDbl pos)
{
	Text::StringBuilderUTF8 sbTmp;
	UTF8Char *sptr;
	UTF8Char *outptr;
	Int32 resType;
	OSInt i;
	UOSInt j;
	UInt32 k;
	Int32 l = 0;
	Double thisDist;
	Double minDist;
	Math::Coord2DDbl posNear;

	outptr = buff;
	*outptr = 0;
	i = MAPSEARCH_LAYER_TYPES;
	while (i-- > 0)
	{
		posNear = Math::Coord2DDbl(0, 0);
		resType = 0;
		sptr = 0;
		minDist = 63781370;

		j = this->layersArr[i]->GetCount();
		k = 0;
		while (k < j)
		{
			Map::MapSearchLayerInfo *lyr = this->layersArr[i]->GetItem(k++);
			if (lyr->searchType == 2)
			{
				sbTmp.ClearStr();
				if (lyr->mapLayer->GetPGLabel(sbTmp, pos, 0, lyr->strIndex))
				{
					if (lyr->searchStr)
					{
						sptr = sbTmp.ConcatTo(lyr->searchStr->ConcatTo(outptr));
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
							if (lyr->searchStr)
							{
								sptr = sbTmp.ConcatTo(lyr->searchStr->ConcatTo(outptr));
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
						if (lyr->searchStr)
						{
							sptr = sbTmp.ConcatTo(lyr->searchStr->ConcatTo(outptr));
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
		if (sptr && *outptr)
		{
			outArrs[i].v = outptr;
			outArrs[i].leng = (UOSInt)(sptr - outptr);
			outptr = sptr + 1;
			l++;
		}
		else
		{
			outArrs[i] = {0, 0};
		}
	}
	return l;
}

UTF8Char *Map::MapSearch::ConcatNames(UTF8Char *buff, Text::PString *strArrs, Int32 concatType)
{
	UTF8Char *outptr = 0;
	UOSInt i = 0;
	UTF8Char sbufftmp[128];
	UTF8Char *sptrtmp;
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
	if (outptr == 0)
	{
		*buff = 0;
		return 0;
	}
	Int32 langType = 0;
	while (*outptr)
	{
		if (*outptr++ >= 128)
		{
			langType = 1;
			break;
		}
	}

	outptr = buff;
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
							*outptr = 0;
						}
						else
						{
							outptr = strArrs[6].ConcatTo(outptr);
						}
					}
					else
					{
						outptr = strArrs[2].ConcatTo(outptr);
					}
				}
				else
				{
					if (strArrs[2].leng != 0)
					{
						outptr = strArrs[2].ConcatTo(outptr);
						outptr = Text::StrConcatC(outptr, UTF8STRC(", "));
					}
					outptr = strArrs[0].ConcatTo(outptr);
				}
			}
			else
			{
                if (strArrs[2].leng != 0)
				{
					outptr = strArrs[2].ConcatTo(outptr);
					outptr = Text::StrConcatC(outptr, UTF8STRC(", "));
				}
				outptr = strArrs[1].ConcatTo(outptr);
			}
		}
		else
		{
			if (strArrs[2].leng == 0)
			{
				outptr = Text::StrConcatC(outptr, UTF8STRC("Near "));
				outptr = strArrs[3].ConcatTo(outptr);
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
					outptr = strArrs[3].ConcatTo(outptr);
				}
				else
				{
					if (strArrs[1].leng == 0)
					{
						if (strArrs[0].leng == 0)
						{
							outptr = strArrs[2].ConcatTo(outptr);
						}
						else
						{
							if (strArrs[2].leng != 0)
							{
								outptr = strArrs[2].ConcatTo(outptr);
								outptr = Text::StrConcatC(outptr, UTF8STRC(", "));
							}
							outptr = strArrs[0].ConcatTo(outptr);
						}
					}
					else
					{
						if (strArrs[2].leng != 0)
						{
							outptr = strArrs[2].ConcatTo(outptr);
							outptr = Text::StrConcatC(outptr, UTF8STRC(", "));
						}
						outptr = strArrs[1].ConcatTo(outptr);
						if (strArrs[0].leng != 0)
						{
							outptr = Text::StrConcatC(outptr, UTF8STRC(", "));
							outptr = strArrs[0].ConcatTo(outptr);
						}
					}

					if (buff[0] == 0)
					{
						outptr = Text::StrConcatC(outptr, UTF8STRC("Near "));
						outptr = strArrs[3].ConcatTo(outptr);
					}
					else
					{
						outptr = Text::StrConcatC(outptr, UTF8STRC(", Near "));
						outptr = strArrs[3].ConcatTo(outptr);
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
							outptr = strArrs[5].ConcatTo(outptr);
						}
						else if (strArrs[6].leng != 0)
						{
							outptr = strArrs[6].ConcatTo(outptr);
						}
						else
						{
							*outptr = 0;
						}
					}
					else
					{
						if (strArrs[5].leng != 0)
						{
							outptr = strArrs[5].ConcatTo(outptr);
							outptr = Text::StrConcatC(outptr, UTF8STRC(", "));
						}
						outptr = strArrs[2].ConcatTo(outptr);
					}
				}
				else
				{
					outptr = strArrs[0].ConcatTo(outptr);
					if (strArrs[5].leng != 0)
					{
						outptr = Text::StrConcatC(outptr, UTF8STRC(", "));
						outptr = strArrs[5].ConcatTo(outptr);
					}
					if (strArrs[2].leng != 0)
					{
						outptr = Text::StrConcatC(outptr, UTF8STRC(", "));
						outptr = strArrs[2].ConcatTo(outptr);
					}
				}
			}
			else
			{
				if (strArrs[0].leng != 0)
				{
					outptr = strArrs[0].ConcatTo(outptr);
					outptr = Text::StrConcatC(outptr, UTF8STRC(", "));
				}
				outptr = strArrs[1].ConcatTo(outptr);
				if (strArrs[5].leng != 0)
				{
					outptr = Text::StrConcatC(outptr, UTF8STRC(", "));
					outptr = strArrs[5].ConcatTo(outptr);
				}
				if (strArrs[2].leng != 0)
				{
					outptr = Text::StrConcatC(outptr, UTF8STRC(", "));
					outptr = strArrs[2].ConcatTo(outptr);
				}
			}
		}
		else
		{
			if (strArrs[2].leng == 0)
			{
				outptr = Text::StrWriteChar(outptr, 0x8FD1);
				outptr = strArrs[3].ConcatTo(outptr);
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
					outptr = strArrs[3].ConcatTo(outptr);
				}
				else
				{
					if (strArrs[1].leng == 0)
					{
						if (strArrs[0].leng == 0)
						{
							outptr = strArrs[2].ConcatTo(outptr);
						}
						else
						{
							outptr = strArrs[0].ConcatTo(outptr);
							if (strArrs[2].leng != 0)
							{
								outptr = Text::StrConcatC(outptr, UTF8STRC(", "));
								outptr = strArrs[2].ConcatTo(outptr);
							}
						}
					}
					else
					{
						if (strArrs[0].leng != 0)
						{
							outptr = strArrs[0].ConcatTo(outptr);
							outptr = Text::StrConcatC(outptr, UTF8STRC(", "));
						}
						outptr = strArrs[1].ConcatTo(outptr);
						if (strArrs[2].leng != 0)
						{
							outptr = Text::StrConcatC(outptr, UTF8STRC(", "));
							outptr = strArrs[2].ConcatTo(outptr);
						}
					}

					if (buff[0] == 0)
					{
						outptr = Text::StrWriteChar(outptr, 0x8FD1);
						outptr = strArrs[3].ConcatTo(outptr);
					}
					else
					{
						outptr = Text::StrConcatC(outptr, UTF8STRC(", "));
						outptr = Text::StrWriteChar(outptr, 0x8FD1);
						outptr = strArrs[3].ConcatTo(outptr);
					}
				}
			}
		}
	}
	else// if (concatType == 3)
	{
		*outptr = 0;
		i = 0;
		while (i < MAPSEARCH_LAYER_TYPES)
		{
			if (strArrs[i].leng != 0)
			{
				if (outptr != buff)
				{
					outptr = Text::StrConcatC(outptr, UTF8STRC(", "));
				}
				outptr = strArrs[i].ConcatTo(outptr);
			}
			i++;
		}
	}
	return outptr;
}

Bool Map::MapSearch::IsError()
{
	return this->baseDir == 0;
}

Int32 Map::MapSearch::GetConcatType()
{
	return this->concatType;
}
