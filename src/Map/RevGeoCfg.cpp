#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/StreamReader.h"
#include "Map/RevGeoCfg.h"
#include "Text/MyString.h"

Map::RevGeoCfg::RevGeoCfg(Text::CStringNN fileName, Map::MapSearchManager *mapSrchMgr)
{
	UTF8Char filePath[256];
	UnsafeArray<UTF8Char> filePathName;
	UnsafeArray<UTF8Char> filePathNameEnd;
	UTF8Char sbuff[512];
	Text::PString sptrs[2];
	UnsafeArray<UTF8Char> sptr;
	Int32 srchType;
	Int32 srchLyr;
	NN<Map::RevGeoCfg::SearchLayer> layer;
	NN<Map::MapSearchLayer> mdata;

	filePathName = filePath;

	IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (!fs.IsError())
	{
		IO::StreamReader reader(fs);
		while (reader.ReadLine(sbuff, 511).SetTo(sptr))
		{
			if (Text::StrSplitP(sptrs, 2, {sbuff, (UIntOS)(sptr - sbuff)}, ',') == 2)
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
							layer = MemAllocNN(Map::RevGeoCfg::SearchLayer);
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
	UIntOS i = REVGEO_MAXID;
	UIntOS j;
	NN<Map::RevGeoCfg::SearchLayer> layer;
	while (i-- > 0)
	{
		j = this->layers[i].GetCount();
		while (j-- > 0)
		{
			layer = this->layers[i].GetItemNoCheck(j);
			if (--layer->usedCnt <= 0)
			{
				layer->layerName->Release();
				MemFreeNN(layer);
			}
		}
	}
}

UnsafeArrayOpt<UTF8Char> Map::RevGeoCfg::GetStreetName(UnsafeArray<UTF8Char> buff, UIntOS buffSize, Math::Coord2DDbl pos)
{
	Text::StringBuilderUTF8 sb;
	if (GetStreetName(sb, pos))
		return sb.ConcatToS(buff, buffSize);
	return nullptr;
}

Bool Map::RevGeoCfg::GetStreetName(NN<Text::StringBuilderUTF8> sb, Math::Coord2DDbl pos)
{
	Text::StringBuilderUTF8 sbTmp;
	UIntOS lastStrPos = 0;
	Bool hasStr = false;
	UIntOS i = 0;
	UIntOS j;
	UIntOS k;
	Math::Coord2DDbl posOut;
	Double minDist;
	Double thisDist;
	NN<Map::RevGeoCfg::SearchLayer> layer;
	NN<Data::ArrayListNN<Map::RevGeoCfg::SearchLayer>> layers;
	while (i < REVGEO_MAXID)
	{
		layers = this->layers[i];
		minDist = -1;
		j = 0;
		k = layers->GetCount();
		while (j < k)
		{
			layer = layers->GetItemNoCheck(j);
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
