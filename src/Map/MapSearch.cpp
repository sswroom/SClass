#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "IO/FileStream.h"
#include "IO/StreamReader.h"
#include "Map/MapSearch.h"
#include "Math/Geometry.h"
#include "Sync/Event.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"

Map::MapSearch::MapSearch(const UTF8Char *fileName, Map::MapSearchManager *manager)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UTF8Char u8buff[256];
	UTF8Char *tmp;
	UTF8Char *strs[5];
	IO::FileStream *fs;
	IO::StreamReader *reader;
	UOSInt i;
	Int32 layerId;
	Int32 layerType;
	Double layerDist;


	this->baseDir = 0;
	this->concatType = 0;
	this->layersArr = MemAlloc(Data::ArrayList<Map::MapSearchLayer*>*, MAPSEARCH_LAYER_TYPES);
	i = MAPSEARCH_LAYER_TYPES;
	while (i-- > 0)
	{
		NEW_CLASS(this->layersArr[i], Data::ArrayList<Map::MapSearchLayer*>());
	}

	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	NEW_CLASS(reader, IO::StreamReader(fs));
	sptr = reader->ReadLine(sbuff, 256);
	while (sptr)
	{
		i = Text::StrSplit(strs, 5, sbuff, ',');
		/*
		layerType
		0: baseDir
		1: Near border
		2: Inside object
		3: Concat Type
*/

		if (i == 2)
		{
			if (Text::StrToInt32(strs[0], &layerType))
			{
				if (layerType == 0)
				{
					this->baseDir = Text::StrCopyNew(strs[1]);
				}
				else if (layerType == 3)
				{
					this->concatType = Text::StrToInt32(strs[1]);
				}
			}
		}
		else if (i == 3)
		{
			layerId = Text::StrToInt32(strs[1]);
			layerType = Text::StrToInt32(strs[0]);
			if (this->baseDir && layerId >= 0 && layerId < MAPSEARCH_LAYER_TYPES)
			{
				Map::MapSearchLayer *lyr;
				tmp = Text::StrConcat(u8buff, this->baseDir);
				Text::StrConcat(tmp, strs[2]);
				lyr = MemAlloc(Map::MapSearchLayer, 1);
				lyr->searchType = layerType;
				lyr->searchDist = 0;
				lyr->mapLayer = manager->LoadLayer(u8buff);
				lyr->searchStr = 0;
				this->layersArr[layerId]->Add(lyr);
			}

		}
		else if (i == 4)
		{
			layerId = Text::StrToInt32(strs[1]);
			layerType = Text::StrToInt32(strs[0]);
			layerDist = Text::StrToDouble(strs[3]);
			if (this->baseDir && layerId >= 0 && layerId < MAPSEARCH_LAYER_TYPES)
			{
				Map::MapSearchLayer *lyr;
				tmp = Text::StrConcat(u8buff, this->baseDir);
				Text::StrConcat(tmp, strs[2]);
				lyr = MemAlloc(Map::MapSearchLayer, 1);
				lyr->searchType = layerType;
				lyr->searchDist = layerDist;
				lyr->mapLayer = manager->LoadLayer(u8buff);
				lyr->searchStr = 0;
				this->layersArr[layerId]->Add(lyr);
			}

		}
		else if (i == 5)
		{
			layerId = Text::StrToInt32(strs[1]);
			layerType = Text::StrToInt32(strs[0]);
			layerDist = Text::StrToDouble(strs[3]);
			if (this->baseDir && layerId >= 0 && layerId < MAPSEARCH_LAYER_TYPES)
			{
				Map::MapSearchLayer *lyr;
				tmp = Text::StrConcat(u8buff, this->baseDir);
				Text::StrConcat(tmp, strs[2]);
				lyr = MemAlloc(Map::MapSearchLayer, 1);
				lyr->searchType = layerType;
				lyr->searchDist = layerDist;
				lyr->mapLayer = manager->LoadLayer(u8buff);
				if (strs[4][0] == 0)
				{
					lyr->searchStr = 0;
				}
				else
				{
					lyr->searchStr = Text::StrCopyNew(strs[4]);
				}
				this->layersArr[layerId]->Add(lyr);
			}

		}
		sptr = reader->ReadLine(sbuff, 256);
	}
	DEL_CLASS(reader);
	DEL_CLASS(fs);
}

Map::MapSearch::~MapSearch()
{
	UOSInt i;
	UOSInt j;
	if (this->baseDir)
	{
		Text::StrDelNew(this->baseDir);
		this->baseDir = 0;
	}
	i = MAPSEARCH_LAYER_TYPES;
	while (i-- > 0)
	{
		j = this->layersArr[i]->GetCount();
		while (j-- > 0)
		{
			Map::MapSearchLayer *lyr = (Map::MapSearchLayer*)this->layersArr[i]->RemoveAt(j);
			if (lyr->searchStr)
			{
				Text::StrDelNew(lyr->searchStr);
			}
			MemFree(lyr);
		}
		DEL_CLASS(this->layersArr[i]);
	}
	MemFree(this->layersArr);
}

UTF8Char *Map::MapSearch::SearchName(UTF8Char *buff, Double lat, Double lon)
{
	UTF8Char sbuff[1024];
	UTF8Char *outArrs[MAPSEARCH_LAYER_TYPES];
	Double outPos[2 * MAPSEARCH_LAYER_TYPES];
	Int32 resTypes[MAPSEARCH_LAYER_TYPES];
	SearchNames(sbuff, outArrs, outPos, resTypes, lat, lon);
	UTF8Char *ptr = ConcatNames(buff, outArrs, 0);
	return ptr;
}

Int32 Map::MapSearch::SearchNames(UTF8Char *buff, UTF8Char **outArrs, Double *outPos, Int32 *resTypes, Double lat, Double lon)
{
	UTF8Char sbuff[128];
	UTF8Char sbufftmp[128];
	UTF8Char *inptr;
	UTF8Char *outptr;
	Int32 resType;
	OSInt i;
	UOSInt j;
	UInt32 k;
	Int32 l = 0;
	Double thisDist;
	Double minDist;
	Double xposNear;
	Double yposNear;

	outptr = buff;
	*outptr = 0;
	i = MAPSEARCH_LAYER_TYPES;
	while (i-- > 0)
	{
		xposNear = 0;
		yposNear = 0;
		resType = 0;
		inptr = sbuff;
		*inptr = 0;
		minDist = 63781370;

		j = this->layersArr[i]->GetCount();
		k = 0;
		while (k < j)
		{
			Map::MapSearchLayer *lyr = (Map::MapSearchLayer*)this->layersArr[i]->GetItem(k++);
			if (lyr->searchType == 2)
			{
				if (lyr->mapLayer->GetPGLabelD(sbufftmp, lon, lat))
				{
					if (lyr->searchStr)
					{
						Text::StrConcat(Text::StrConcat(inptr, lyr->searchStr), sbufftmp);
					}
					else
					{
						Text::StrConcat(inptr, sbufftmp);
					}
					xposNear = lon;
					yposNear = lat;
					resType = 2;
					break;
				}
			}
			else if (lyr->searchType == 1)
			{
				Double xposout;
				Double yposout;
				if (lyr->mapLayer->GetPLLabelD(sbufftmp, lon, lat, &xposout, &yposout))
				{
					Double tmp;
					tmp = xposout - lon;
					thisDist = tmp * tmp;
					tmp = yposout - lat;
					thisDist += tmp * tmp;
					if (lyr->searchDist)
					{
						Double meterDist = Math::Geometry::SphereDistDeg(lat, lon, yposout, xposout, 6378137.0);
						if (meterDist < lyr->searchDist)
						{
							minDist = thisDist;
							if (lyr->searchStr)
							{
								Text::StrConcat(Text::StrConcat(inptr, lyr->searchStr), sbufftmp);
							}
							else
							{
								Text::StrConcat(inptr, sbufftmp);
							}
							xposNear = xposout;
							yposNear = yposout;
							resType = 1;
							break;
						}
					}
					else if (thisDist < minDist)
					{
						minDist = thisDist;
						if (lyr->searchStr)
						{
							Text::StrConcat(Text::StrConcat(inptr, lyr->searchStr), sbufftmp);
						}
						else
						{
							Text::StrConcat(inptr, sbufftmp);
						}
						xposNear = xposout;
						yposNear = yposout;
						resType = 1;
					}
				}
			}
		}

		outPos[(i << 1) + 1] = xposNear;
		outPos[(i << 1) + 0] = yposNear;
		resTypes[i] = resType;
		if (*inptr)
		{
			outArrs[i] = outptr;
			outptr = Text::StrConcat(outptr, inptr) + 1;
			l++;
		}
		else
		{
			outArrs[i] = 0;
		}
	}
	return l;
}

UTF8Char *Map::MapSearch::ConcatNames(UTF8Char *buff, UTF8Char **strArrs, Int32 concatType)
{
	UTF8Char *outptr = 0;
	OSInt i = 0;
	UTF8Char sbufftmp[128];
	UTF8Char *stmp[2];
	while (i < MAPSEARCH_LAYER_TYPES)
	{
		if (strArrs[i] != 0)
		{
			outptr = strArrs[i];
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
		if (strArrs[3] == 0)
		{
			if (strArrs[1] == 0)
			{
				if (strArrs[0] == 0)
				{
					if (strArrs[2] == 0)
					{
						if (strArrs[6] == 0)
						{
							*outptr = 0;
						}
						else
						{
							outptr = Text::StrConcat(outptr, strArrs[6]);
						}
					}
					else
					{
						outptr = Text::StrConcat(outptr, strArrs[2]);
					}
				}
				else
				{
					if (strArrs[2] != 0)
					{
						outptr = Text::StrConcat(outptr, strArrs[2]);
						outptr = Text::StrConcat(outptr, (const UTF8Char*)", ");
					}
					outptr = Text::StrConcat(outptr, strArrs[0]);
				}
			}
			else
			{
                if (strArrs[2] != 0)
				{
					outptr = Text::StrConcat(outptr, strArrs[2]);
					outptr = Text::StrConcat(outptr, (const UTF8Char*)", ");
				}
				outptr = Text::StrConcat(outptr, strArrs[1]);
			}
		}
		else
		{
			if (strArrs[2] == 0)
			{
				outptr = Text::StrConcat(outptr, (const UTF8Char*)"Near ");
				outptr = Text::StrConcat(outptr, strArrs[3]);
			}
			else
			{
				i = Text::StrIndexOf(strArrs[2], ' ');
				if (i >= 0)
					Text::StrConcatC(sbufftmp, strArrs[2], (UOSInt)i);
				else
					Text::StrConcat(sbufftmp, strArrs[2]);

				i = Text::StrIndexOf(strArrs[3], sbufftmp);
				if (i >= 0)
				{
					outptr = Text::StrConcat(outptr, strArrs[3]);
				}
				else
				{
					if (strArrs[1] == 0)
					{
						if (strArrs[0] == 0)
						{
							outptr = Text::StrConcat(outptr, strArrs[2]);
						}
						else
						{
							if (strArrs[2] != 0)
							{
								outptr = Text::StrConcat(outptr, strArrs[2]);
								outptr = Text::StrConcat(outptr, (const UTF8Char*)", ");
							}
							outptr = Text::StrConcat(outptr, strArrs[0]);
						}
					}
					else
					{
						if (strArrs[2] != 0)
						{
							outptr = Text::StrConcat(outptr, strArrs[2]);
							outptr = Text::StrConcat(outptr, (const UTF8Char*)", ");
						}
						outptr = Text::StrConcat(outptr, strArrs[1]);
						if (strArrs[0] != 0)
						{
							outptr = Text::StrConcat(outptr, (const UTF8Char*)", ");
							outptr = Text::StrConcat(outptr, strArrs[0]);
						}
					}

					if (buff[0] == 0)
					{
						outptr = Text::StrConcat(outptr, (const UTF8Char*)"Near ");
						outptr = Text::StrConcat(outptr, strArrs[3]);
					}
					else
					{
						outptr = Text::StrConcat(outptr, (const UTF8Char*)", Near ");
						outptr = Text::StrConcat(outptr, strArrs[3]);
					}
				}
			}
		}
	}
	else if (concatType == 2 || (concatType == 0 && langType != 0))
	{
		if (strArrs[3] == 0)
		{
			if (strArrs[1] == 0)
			{
				if (strArrs[0] == 0)
				{
					if (strArrs[2] == 0)
					{
						if (strArrs[5] != 0)
						{
							outptr = Text::StrConcat(outptr, strArrs[5]);
						}
						else if (strArrs[6] != 0)
						{
							outptr = Text::StrConcat(outptr, strArrs[6]);
						}
						else
						{
							*outptr = 0;
						}
					}
					else
					{
						if (strArrs[5] != 0)
						{
							outptr = Text::StrConcat(outptr, strArrs[5]);
							outptr = Text::StrConcat(outptr, (const UTF8Char*)", ");
						}
						outptr = Text::StrConcat(outptr, strArrs[2]);
					}
				}
				else
				{
					outptr = Text::StrConcat(outptr, strArrs[0]);
					if (strArrs[5] != 0)
					{
						outptr = Text::StrConcat(outptr, (const UTF8Char*)", ");
						outptr = Text::StrConcat(outptr, strArrs[5]);
					}
					if (strArrs[2] != 0)
					{
						outptr = Text::StrConcat(outptr, (const UTF8Char*)", ");
						outptr = Text::StrConcat(outptr, strArrs[2]);
					}
				}
			}
			else
			{
				if (strArrs[0] != 0)
				{
					outptr = Text::StrConcat(outptr, strArrs[0]);
					outptr = Text::StrConcat(outptr, (const UTF8Char*)", ");
				}
				outptr = Text::StrConcat(outptr, strArrs[1]);
				if (strArrs[5] != 0)
				{
					outptr = Text::StrConcat(outptr, (const UTF8Char*)", ");
					outptr = Text::StrConcat(outptr, strArrs[5]);
				}
				if (strArrs[2] != 0)
				{
					outptr = Text::StrConcat(outptr, (const UTF8Char*)", ");
					outptr = Text::StrConcat(outptr, strArrs[2]);
				}
			}
		}
		else
		{
			if (strArrs[2] == 0)
			{
				WChar tbuff[2];
				tbuff[0] = 0x8FD1;
				tbuff[1] = 0;
				outptr = Text::StrWChar_UTF8(outptr, tbuff, -1);
				outptr = Text::StrConcat(outptr, strArrs[3]);
			}
			else
			{
				Text::StrConcat(sbufftmp, strArrs[2]);
				Text::StrSplit(stmp, 2, sbufftmp, ' ');
				i = Text::StrIndexOf(sbufftmp, '-');
				if (i >= 0)
				{
					Text::StrConcat(sbufftmp, &sbufftmp[i + 1]);
				}

				i = Text::StrIndexOf(strArrs[3], sbufftmp);
				if (i >= 0)
				{
					outptr = Text::StrConcat(outptr, strArrs[3]);
				}
				else
				{
					if (strArrs[1] == 0)
					{
						if (strArrs[0] == 0)
						{
							outptr = Text::StrConcat(outptr, strArrs[2]);
						}
						else
						{
							outptr = Text::StrConcat(outptr, strArrs[0]);
							if (strArrs[2] != 0)
							{
								outptr = Text::StrConcat(outptr, (const UTF8Char*)", ");
								outptr = Text::StrConcat(outptr, strArrs[2]);
							}
						}
					}
					else
					{
						if (strArrs[0] != 0)
						{
							outptr = Text::StrConcat(outptr, strArrs[0]);
							outptr = Text::StrConcat(outptr, (const UTF8Char*)", ");
						}
						outptr = Text::StrConcat(outptr, strArrs[1]);
						if (strArrs[2] != 0)
						{
							outptr = Text::StrConcat(outptr, (const UTF8Char*)", ");
							outptr = Text::StrConcat(outptr, strArrs[2]);
						}
					}

					WChar tbuff[2];
					tbuff[0] = 0x8FD1;
					tbuff[1] = 0;
					if (buff[0] == 0)
					{
						outptr = Text::StrWChar_UTF8(outptr, tbuff, -1);
						outptr = Text::StrConcat(outptr, strArrs[3]);
					}
					else
					{
						outptr = Text::StrConcat(outptr, (const UTF8Char*)", ");
						outptr = Text::StrWChar_UTF8(outptr, tbuff, -1);
						outptr = Text::StrConcat(outptr, strArrs[3]);
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
			if (strArrs[i] != 0)
			{
				if (outptr != buff)
				{
					outptr = Text::StrConcat(outptr, (const UTF8Char*)", ");
				}
				outptr = Text::StrConcat(outptr, strArrs[i]);
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
