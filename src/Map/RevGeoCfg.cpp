#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/StreamReader.h"
#include "Map/RevGeoCfg.h"
#include "Text/MyString.h"

Map::RevGeoCfg::RevGeoCfg(Text::CString fileName, Map::MapSearchManager *mapSrchMgr)
{
	UTF8Char filePath[256];
	UTF8Char *filePathName;
	UTF8Char sbuff[512];
	UTF8Char *sptrs[2];
	OSInt i = REVGEO_MAXID;
	Int32 srchType;
	Int32 srchLyr;
	IO::StreamReader *reader;
	IO::FileStream *fs;
	Map::RevGeoCfg::SearchLayer *layer;
	Map::IMapSearchLayer *mdata;

	filePathName = filePath;

	while (i-- > 0)
	{
		NEW_CLASS(layers[i], Data::ArrayList<Map::RevGeoCfg::SearchLayer*>());
	}

	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (!fs->IsError())
	{
		NEW_CLASS(reader, IO::StreamReader(fs));
		while (reader->ReadLine(sbuff, 511))
		{
			if (Text::StrSplit(sptrs, 2, sbuff, ',') == 2)
			{
				srchType = Text::StrToInt32(sptrs[0]);
				if (srchType == 0)
				{
					filePathName = Text::StrConcat(filePath, sptrs[1]);
				}
				else if (srchType == 1 || srchType == 2)
				{
					if (Text::StrSplit(sptrs, 2, sptrs[1], ',') == 2)
					{
						srchLyr = Text::StrToInt32(sptrs[0]);
						if (srchLyr < 0 || srchLyr >= REVGEO_MAXID)
							continue;

						Text::StrConcat(filePathName, sptrs[1]);
						mdata = mapSrchMgr->LoadLayer(filePath);
						if (!mdata->IsError())
						{
							layer = MemAlloc(Map::RevGeoCfg::SearchLayer, 1);
							layer->layerName = Text::StrCopyNew(filePath);
							layer->searchType = srchType;
							layer->usedCnt = 1;
							layer->data = mdata;
							this->layers[srchLyr]->Add(layer);
						}
					}
				}
			}
		}
		DEL_CLASS(reader);
	}
	DEL_CLASS(fs);
}

Map::RevGeoCfg::~RevGeoCfg()
{
	UOSInt i = REVGEO_MAXID;
	UOSInt j;
	Map::RevGeoCfg::SearchLayer *layer;
	while (i-- > 0)
	{
		j = layers[i]->GetCount();
		while (j-- > 0)
		{
			layer = (Map::RevGeoCfg::SearchLayer *)layers[i]->GetItem(j);
			if (--layer->usedCnt <= 0)
			{
				Text::StrDelNew(layer->layerName);
				MemFree(layer);
			}
		}
		DEL_CLASS(layers[i]);
	}
}

UTF8Char *Map::RevGeoCfg::GetStreetName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon)
{
	UTF8Char *buffEnd = buff + buffSize;
	UTF8Char sbuff[256];
	UTF8Char *tmpStr = 0;
	Bool hasStr = false;
	UOSInt i = 0;
	UOSInt j;
	UOSInt k;
	Double xposOut;
	Double yposOut;
	Double minDist;
	Double thisDist;
	Map::RevGeoCfg::SearchLayer *layer;
	Data::ArrayList<Map::RevGeoCfg::SearchLayer*> *layers;
	while (i < REVGEO_MAXID)
	{
		layers = this->layers[i];
		minDist = -1;
		j = 0;
		k = layers->GetCount();
		while (j < k)
		{
			layer = (Map::RevGeoCfg::SearchLayer*)layers->GetItem(j);
			if (layer->searchType == 1)
			{
				if (layer->data->GetPLLabelD(sbuff, lon, lat, &xposOut, &yposOut))
				{
					xposOut -= lon;
					yposOut -= lat;
					thisDist = xposOut * xposOut + yposOut * yposOut;
					if (minDist < 0)
					{
						minDist = thisDist;
						if (hasStr)
						{
							tmpStr = Text::StrConcatS(buff, (const UTF8Char*)", ", (UOSInt)(buffEnd - buff));
							buff = Text::StrConcatS(tmpStr, sbuff, (UOSInt)(buffEnd - tmpStr));
						}
						else
						{
							buff = Text::StrConcatS(tmpStr = buff, sbuff, (UOSInt)(buffEnd - buff));
							hasStr = true;
						}
					}
					else if (thisDist < minDist)
					{
						minDist = thisDist;
						buff = Text::StrConcatS(tmpStr, sbuff, (UOSInt)(buffEnd - tmpStr));
						if (thisDist == 0)
							break;
					}
				}
			}
			else if (layer->searchType == 2)
			{
				if (layer->data->GetPGLabelD(sbuff, lon, lat))
				{
					if (minDist < 0)
					{
						if (hasStr)
						{
							buff = Text::StrConcatS(buff, (const UTF8Char*)", ", (UOSInt)(buffEnd - buff));
							buff = Text::StrConcatS(buff, sbuff, (UOSInt)(buffEnd - buff));
						}
						else
						{
							buff = Text::StrConcatS(buff, sbuff, (UOSInt)(buffEnd - buff));
							hasStr = true;
						}
					}
					else
					{
						buff = Text::StrConcatS(tmpStr, sbuff, (UOSInt)(buffEnd - tmpStr));
					}
					break;
				}
			}
			j++;
		}
		
		i++;
	}
	if (hasStr)
		return buff;
	*buff = 0;
	return 0;
}
