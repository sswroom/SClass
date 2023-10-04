#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListICaseString.h"
#include "Data/ByteTool.h"
#include "Exporter/MEVExporter.h"
#include "IO/Path.h"
#include "Math/Math.h"
#include "Text/MyString.h"
#include "Text/Encoding.h"

Exporter::MEVExporter::MEVExporter()
{
}

Exporter::MEVExporter::~MEVExporter()
{
}

Int32 Exporter::MEVExporter::GetName()
{
	return *(Int32*)"MENV";
}

IO::FileExporter::SupportType Exporter::MEVExporter::IsObjectSupported(IO::ParsedObject *pobj)
{
	if (pobj->GetParserType() != IO::ParserType::MapEnv)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	return IO::FileExporter::SupportType::NormalStream;
}

Bool Exporter::MEVExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("Map Env file"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.mev"));
		return true;
	}
	return false;
}

Bool Exporter::MEVExporter::ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CStringNN fileName, IO::ParsedObject *pobj, void *param)
{
	if (pobj->GetParserType() != IO::ParserType::MapEnv)
	{
		return false;
	}
	Map::MapEnv *env = (Map::MapEnv*)pobj;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	OSInt si;
	UInt32 stmPos;
	UInt8 buff[256];
	Map::MapEnv::ImageInfo imgInfo;
	Exporter::MEVExporter::MEVStrRecord *strRec;
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Text::String *tmpStr;
	NotNullPtr<const Data::ArrayList<Exporter::MEVExporter::MEVStrRecord*>> tmpArr;
	Data::ArrayListICaseString dirArr;
	Data::StringMap<Exporter::MEVExporter::MEVStrRecord*> strArr;

	GetMapDirs(env, &dirArr, 0);
	i = env->GetImageFileCnt();
	while (i-- > 0)
	{
		env->GetImageFileInfo(i, &imgInfo);
		sptr = imgInfo.fileName->ConcatTo(sbuff);
		j = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
		if (j != INVALID_INDEX)
		{
			sbuff[j] = 0;
			si = dirArr.SortedIndexOfPtr(sbuff, j);
			if (si < 0)
			{
				dirArr.Insert((UOSInt)~si, Text::String::New(sbuff, j).Ptr());
			}
		}
	}

	buff[0] = 'S';
	buff[1] = 'M';
	buff[2] = 'E';
	buff[3] = 'v';
	buff[4] = 0x1a;
	buff[5] = 0xfe;
	buff[6] = 0xc0;
	buff[7] = 0x81;

	*(Int32*)&buff[8] = 44;//Init size
	WriteUInt32(&buff[12], env->GetBGColor());
	WriteUInt32(&buff[16], (UInt32)env->GetNString());
	*(Int32*)&buff[20] = 0;
	*(UInt32*)&buff[24] = AddString(&strArr, fileName.v, fileName.leng, 20);
	*(Int32*)&buff[28] = (Int32)dirArr.GetCount();
	*(Int32*)&buff[32] = (Int32)env->GetImageFileCnt();
	*(Int32*)&buff[36] = (Int32)env->GetFontStyleCount();
	*(Int32*)&buff[40] = (Int32)env->GetLineStyleCount();
	*(Int32*)&buff[44] = (Int32)env->GetItemCount(0);
	*(Int32*)&buff[48] = (Int32)env->GetDefLineStyle();
	*(Int32*)&buff[52] = (Int32)env->GetDefFontStyle();

	stm->Write(buff, 56);
	stmPos = 56;
	i = 0;
	j = dirArr.GetCount();
	while (i < j)
	{
		tmpStr = dirArr.GetItem(i);
		*(Int32*)&buff[0] = 0;
		*(UInt32*)&buff[4] = AddString(&strArr, tmpStr, stmPos);

		stm->Write(buff, 8);
		stmPos += 8;

		i++;
	}

	i = 0;
	j = env->GetImageFileCnt();
	while (i < j)
	{
		env->GetImageFileInfo(i, &imgInfo);
		sptr = imgInfo.fileName->ConcatTo(sbuff);
		k = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);

		*(Int32*)&buff[0] = 0;
		WriteUInt32(&buff[4], AddString(&strArr, &sbuff[k + 1], imgInfo.fileName->leng - k - 1, stmPos));
		sbuff[k] = 0;
		*(Int32*)&buff[8] = (Int32)dirArr.SortedIndexOfPtr(sbuff, k);
		*(Int32*)&buff[12] = (Int32)imgInfo.index;

		stm->Write(buff, 16);
		stmPos += 16;

		i++;
	}

	i = 0;
	j = env->GetFontStyleCount();
	while (i < j)
	{
		NotNullPtr<Text::String> fontName;
		Double fontSize;
		Bool bold;
		UInt32 fontColor;
		UOSInt buffSize;
		UInt32 buffColor;

		sptr = env->GetFontStyleName(i, sbuff);
		if (!env->GetFontStyle(i, fontName, fontSize, bold, fontColor, buffSize, buffColor))
		{
			fontName = Text::String::NewEmpty();
		}

		*(Int32*)&buff[0] = 0;
		if (sptr)
		{
			WriteUInt32(&buff[4], AddString(&strArr, sbuff, (UOSInt)(sptr - sbuff), stmPos));
		}
		else
		{
			*(Int32*)&buff[4] = 0;
		}
		*(Int32*)&buff[8] = 0;
		WriteUInt32(&buff[12], AddString(&strArr, fontName.Ptr(), stmPos + 8));
		*(Int32*)&buff[16] = Double2Int32(fontSize / 0.75);
		*(Int32*)&buff[20] = bold?1:0;
		WriteUInt32(&buff[24], fontColor);
		WriteUInt32(&buff[28], (UInt32)buffSize);
		WriteUInt32(&buff[32], buffColor);

		stm->Write(buff, 36);
		stmPos += 36;

		i++;
	}

	i = 0;
	j = env->GetLineStyleCount();
	while (i < j)
	{
		sptr = env->GetLineStyleName(i, sbuff);
		*(Int32*)&buff[0] = 0;
		if (sptr)
		{
			*(UInt32*)&buff[4] = AddString(&strArr, sbuff, (UOSInt)(sptr - sbuff), stmPos);
		}
		else
		{
			*(UInt32*)&buff[4] = 0;
		}
		*(Int32*)&buff[8] = (Int32)(l = env->GetLineStyleLayerCnt(i));

		stm->Write(buff, 12);
		stmPos += 12;

		k = 0;
		while (k < l)
		{
			UInt32 color;
			UOSInt thick;
			UOSInt npattern;
			UInt8 *pattern;

			env->GetLineStyleLayer(i, k, color, thick, pattern, npattern);
			WriteUInt32(&buff[0], color);
			*(Int32*)&buff[4] = (Int32)thick;
			WriteUInt32(&buff[8], (UInt32)npattern);

			stm->Write(buff, 12);
			stmPos += 12;

			if (npattern > 0)
			{
				stm->Write(pattern, npattern);
				stmPos = (UInt32)(stmPos + npattern);
			}

			k++;
		}
		i++;
	}

	WriteGroupItems(env, 0, &stmPos, stm, &strArr, &dirArr);

	tmpArr = strArr.GetValues();
	j = tmpArr->GetCount();
	i = 0;
	while (i < j)
	{
		strRec = tmpArr->GetItem(i);

		stm->Write(strRec->strBytes, strRec->byteSize);
		k = strRec->ofstList.GetCount();
		while (k-- > 0)
		{
			stm->SeekFromBeginning(strRec->ofstList.GetItem(k));
			stm->Write((UInt8*)&stmPos, 4);
		}
		stmPos += strRec->byteSize;
		stm->SeekFromBeginning(stmPos);

		i++;
	}

	LIST_FREE_STRING(&dirArr);

	tmpArr = strArr.GetValues();
	i = tmpArr->GetCount();
	while (i-- > 0)
	{
		strRec = tmpArr->GetItem(i);
		MemFree(strRec->strBytes);
		DEL_CLASS(strRec);
	}
	return true;
}

void Exporter::MEVExporter::GetMapDirs(Map::MapEnv *env, Data::ArrayListString *dirArr, Map::MapEnv::GroupItem *group)
{
	UOSInt i = 0;
	UOSInt j = env->GetItemCount(group);
	UOSInt k;
	OSInt si;
	UTF8Char sbuff[256];
	UTF8Char *sptr;

	while (i < j)
	{
		Map::MapEnv::MapItem *item = env->GetItem(group, i);
		if (item->itemType == Map::MapEnv::IT_GROUP)
		{
			GetMapDirs(env, dirArr, (Map::MapEnv::GroupItem*)item);
		}
		else
		{
			Map::MapEnv::LayerItem *lyr = (Map::MapEnv::LayerItem*)item;
			Map::MapDrawLayer *layer = lyr->layer;
			if ((sptr = layer->GetSourceName(sbuff)) != 0)
			{
				k = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '\\');
				if (k != INVALID_INDEX)
				{
					sbuff[k] = 0;
					si = dirArr->SortedIndexOfPtr(sbuff, k);
					if (si < 0)
					{
						dirArr->Insert((UOSInt)~si, Text::String::New(sbuff, k).Ptr());
					}
				}
			}
		}
		i++;
	}
}

UInt32 Exporter::MEVExporter::AddString(Data::StringMap<MEVStrRecord*> *strArr, Text::String *strVal, UInt32 fileOfst)
{
	MEVStrRecord *strRec = strArr->Get(strVal);
	if (strRec == 0)
	{
		NEW_CLASS(strRec, MEVStrRecord());
		strRec->byteSize = (UInt32)strVal->leng;
		strRec->strBytes = MemAlloc(UInt8, strRec->byteSize + 1);
		MemCopyNO(strRec->strBytes, strVal, strRec->byteSize);
		strArr->Put(strVal, strRec);
	}
	strRec->ofstList.Add(fileOfst);
	return strRec->byteSize;
}

UInt32 Exporter::MEVExporter::AddString(Data::StringMap<MEVStrRecord*> *strArr, const UTF8Char *strVal, UOSInt strLen, UInt32 fileOfst)
{
	MEVStrRecord *strRec = strArr->Get({strVal, strLen});
	if (strRec == 0)
	{
		NEW_CLASS(strRec, MEVStrRecord());
		strRec->byteSize = (UInt32)strLen;
		strRec->strBytes = MemAlloc(UInt8, strRec->byteSize + 1);
		MemCopyNO(strRec->strBytes, strVal, strRec->byteSize);
		strArr->Put({strVal, strLen}, strRec);
	}
	strRec->ofstList.Add(fileOfst);
	return strRec->byteSize;
}

void Exporter::MEVExporter::WriteGroupItems(Map::MapEnv *env, Map::MapEnv::GroupItem *group, UInt32 *stmPos, NotNullPtr<IO::SeekableStream> stm, Data::StringMap<Exporter::MEVExporter::MEVStrRecord*> *strArr, Data::ArrayListString *dirArr)
{
	UInt8 buff[256];
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Map::MapEnv::LayerItem setting;
	UOSInt i = 0;
	UOSInt j = env->GetItemCount(group);
	UOSInt k;
	while (i < j)
	{
		Map::MapEnv::MapItem *item = env->GetItem(group, i);

		if (item->itemType == Map::MapEnv::IT_GROUP)
		{
			*(Int32*)&buff[0] = item->itemType;

			NotNullPtr<Text::String> groupName = env->GetGroupName((Map::MapEnv::GroupItem*)item);
			*(Int32*)&buff[4] = 0;
			*(UInt32*)&buff[8] = AddString(strArr, groupName->v, groupName->leng, 4 + *stmPos);
			*(Int32*)&buff[12] = (Int32)env->GetItemCount((Map::MapEnv::GroupItem*)item);
			stm->Write(buff, 16);
			*stmPos = 16 + *stmPos;

			WriteGroupItems(env, (Map::MapEnv::GroupItem*)item, stmPos, stm, strArr, dirArr);
		}
		else if (item->itemType == Map::MapEnv::IT_LAYER)
		{
			Map::DrawLayerType ltype;
			Map::MapEnv::LayerItem *lyr = (Map::MapEnv::LayerItem*)item;
			Map::MapDrawLayer *layer = lyr->layer;
			*(Int32*)&buff[0] = item->itemType;
			sptr = layer->GetSourceName(sbuff);
			*(Int32*)&buff[4] = 0;
			k = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
			*(UInt32*)&buff[8] = AddString(strArr, &sbuff[k + 1], (UOSInt)(sptr - &sbuff[k + 1]), 4 + *stmPos);
			if (k != INVALID_INDEX)
			{
				sbuff[k] = 0;
				*(Int32*)&buff[12] = (Int32)dirArr->SortedIndexOfPtr(sbuff, k);
			}
			else
			{
				*(Int32*)&buff[12] = -1;
			}
			env->GetLayerProp(setting, group, i);
			ltype = layer->GetLayerType();
			*(UInt32*)&buff[16] = layer->GetCodePage();
			WriteUInt32(&buff[24], (UInt32)setting.labelCol);
			*(Int32*)&buff[28] = setting.flags;
			*(Int32*)&buff[32] = Double2Int32(setting.minScale);
			*(Int32*)&buff[36] = Double2Int32(setting.maxScale);
			*(Int32*)&buff[40] = setting.priority;
			*(Int32*)&buff[44] = (Int32)setting.fontStyle;
			if (ltype == Map::DRAW_LAYER_POINT || ltype == Map::DRAW_LAYER_POINT3D)
			{
				*(Int32*)&buff[20] = 1;
				UInt32 i = (UInt32)env->GetImageFileIndex(setting.imgIndex);
				Map::MapEnv::ImageInfo imgInfo;
				env->GetImageFileInfo(i, &imgInfo);
				*(UInt32*)&buff[48] = i;
				*(Int32*)&buff[52] = (Int32)(setting.imgIndex - imgInfo.index);

				stm->Write(buff, 56);
				*stmPos = 56 + *stmPos;
			}
			else if (ltype == Map::DRAW_LAYER_POLYLINE || ltype == Map::DRAW_LAYER_POLYLINE3D)
			{
				*(Int32*)&buff[20] = 3;
				*(Int32*)&buff[48] = (Int32)setting.lineStyle;

				stm->Write(buff, 52);
				*stmPos = 52 + *stmPos;
			}
			else if (ltype == Map::DRAW_LAYER_POLYGON)
			{
				*(Int32*)&buff[20] = 5;
				*(Int32*)&buff[48] = (Int32)setting.lineStyle;
				*(Int32*)&buff[52] = (Int32)setting.fillStyle;

				stm->Write(buff, 56);
				*stmPos = 56 + *stmPos;
			}
			else
			{
				*(Int32*)&buff[20] = 0;
				stm->Write(buff, 48);
				*stmPos = 48 + *stmPos;
			}
		}
		else
		{
			*(Int32*)&buff[0] = item->itemType;
			stm->Write(buff, 4);
			*stmPos = 4 + *stmPos;
		}
		i++;
	}
}
