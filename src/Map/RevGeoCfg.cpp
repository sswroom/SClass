#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/StreamReader.h"
#include "Map/RevGeoCfg.h"
#include "Text/MyString.h"

Map::RevGeoCfg::RevGeoCfg(Text::CStringNN fileName, Map::MapSearchManager *mapSrchMgr)
{
	UTF8Char filePath[256];
	UTF8Char *filePathName;
	UTF8Char *filePathNameEnd;
	UTF8Char sbuff[512];
	Text::PString sptrs[2];
	UTF8Char *sptr;
	Int32 srchType;
	Int32 srchLyr;
	Map::RevGeoCfg::SearchLayer *layer;
	Map::MapSearchLayer *mdata;

	filePathName = filePath;

	IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (!fs.IsError())
	{
		IO::StreamReader reader(fs);
		while ((sptr = reader.ReadLine(sbuff, 511)) != 0)
		{
			if (Text::StrSplitP(sptrs, 2, {sbuff, (UOSInt)(sptr - sbuff)}, ',') == 2)
			{
				srchType = Text::StrToInt32(sptrs[0].v);
				if (srchType == 0)
				{
					filePathName = sptrs[1].ConcatTo(filePath);
				}
				else if (srchType == 1 || srchType == 2)
				{
					if (Text::StrSplitP(sptrs, 2, sptrs[1], ',') == 2)
					{
						srchLyr = Text::StrToInt32(sptrs[0].v);
						if (srchLyr < 0 || srchLyr >= REVGEO_MAXID)
							continue;

						filePathNameEnd = sptrs[1].ConcatTo(filePathName);
						mdata = mapSrchMgr->LoadLayer(CSTRP(filePath, filePathNameEnd));
						if (!mdata->IsError())
						{
							layer = MemAlloc(Map::RevGeoCfg::SearchLayer, 1);
							layer->layerName = Text::String::NewP(filePath, filePathNameEnd);
							layer->searchType = srchType;
							layer->strIndex = 0;
							layer->usedCnt = 1;
							layer->data = mdata;
							this->layers[srchLyr].Add(layer);
						}
					}
				}
			}
		}
	}
}

Map::RevGeoCfg::~RevGeoCfg()
{
	UOSInt i = REVGEO_MAXID;
	UOSInt j;
	Map::RevGeoCfg::SearchLayer *layer;
	while (i-- > 0)
	{
		j = this->layers[i].GetCount();
		while (j-- > 0)
		{
			layer = this->layers[i].GetItem(j);
			if (--layer->usedCnt <= 0)
			{
				layer->layerName->Release();
				MemFree(layer);
			}
		}
	}
}

UTF8Char *Map::RevGeoCfg::GetStreetName(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl pos)
{
	Text::StringBuilderUTF8 sb;
	if (GetStreetName(sb, pos))
		return sb.ConcatToS(buff, buffSize);
	return 0;
}

Bool Map::RevGeoCfg::GetStreetName(NotNullPtr<Text::StringBuilderUTF8> sb, Math::Coord2DDbl pos)
{
	Text::StringBuilderUTF8 sbTmp;
	UOSInt lastStrPos = 0;
	Bool hasStr = false;
	UOSInt i = 0;
	UOSInt j;
	UOSInt k;
	Math::Coord2DDbl posOut;
	Double minDist;
	Double thisDist;
	Map::RevGeoCfg::SearchLayer *layer;
	Data::ArrayList<Map::RevGeoCfg::SearchLayer*> *layers;
	while (i < REVGEO_MAXID)
	{
		layers = &this->layers[i];
		minDist = -1;
		j = 0;
		k = layers->GetCount();
		while (j < k)
		{
			layer = layers->GetItem(j);
			if (layer->searchType == 1)
			{
				sbTmp.ClearStr();
				if (layer->data->GetPLLabel(sbTmp, pos, posOut, layer->strIndex))
				{
					posOut = posOut - pos;
					posOut = posOut * posOut;
					thisDist = posOut.x + posOut.y;
					if (minDist < 0)
					{
						minDist = thisDist;
						if (hasStr)
						{
							sb->AppendC(UTF8STRC(", "));
							lastStrPos = sb->leng;
							sb->Append(sbTmp);
						}
						else
						{
							sb->Append(sbTmp);
							hasStr = true;
						}
					}
					else if (thisDist < minDist)
					{
						minDist = thisDist;
						sb->TrimToLength(lastStrPos);
						sb->Append(sbTmp);
						if (thisDist == 0)
							break;
					}
				}
			}
			else if (layer->searchType == 2)
			{
				sbTmp.ClearStr();
				if (layer->data->GetPGLabel(sbTmp, pos, posOut, layer->strIndex))
				{
					if (minDist < 0)
					{
						if (hasStr)
						{
							sb->AppendC(UTF8STRC(", "));
							sb->Append(sbTmp);
						}
						else
						{
							sb->Append(sbTmp);
							hasStr = true;
						}
					}
					else
					{
						sb->TrimToLength(lastStrPos);
						sb->Append(sbTmp);
					}
					break;
				}
			}
			j++;
		}
		
		i++;
	}
	return hasStr;
}
