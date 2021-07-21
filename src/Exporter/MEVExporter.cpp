#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListICaseStrUTF8.h"
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
	if (pobj->GetParserType() != IO::ParsedObject::PT_MAP_ENV_PARSER)
	{
		return IO::FileExporter::ST_NOT_SUPPORTED;
	}
	return IO::FileExporter::ST_NORMAL_STREAM;
}

Bool Exporter::MEVExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcat(nameBuff, (const UTF8Char*)"Map Env file");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.mev");
		return true;
	}
	return false;
}

Bool Exporter::MEVExporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
{
	if (pobj->GetParserType() != IO::ParsedObject::PT_MAP_ENV_PARSER)
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
	Data::ArrayListICaseStrUTF8 *dirArr;
	Data::StringUTF8Map<Exporter::MEVExporter::MEVStrRecord*> *strArr;
	UInt8 buff[256];
	Map::MapEnv::ImageInfo imgInfo;
	Exporter::MEVExporter::MEVStrRecord *strRec;
	UTF8Char u8buff[256];
	UTF8Char *u8ptr;
	const UTF8Char *tmpStr;
	Data::ArrayList<Exporter::MEVExporter::MEVStrRecord*> *tmpArr;

	NEW_CLASS(strArr, Data::StringUTF8Map<Exporter::MEVExporter::MEVStrRecord*>());
	NEW_CLASS(dirArr, Data::ArrayListICaseStrUTF8());

	GetMapDirs(env, dirArr, 0);
	i = env->GetImageFileCnt();
	while (i-- > 0)
	{
		env->GetImageFileInfo(i, &imgInfo);
		Text::StrConcat(u8buff, imgInfo.fileName);
		si = Text::StrLastIndexOf(u8buff, IO::Path::PATH_SEPERATOR);
		if (si >= 0)
		{
			u8buff[si] = 0;
			si = dirArr->SortedIndexOf(u8buff);
			if (si < 0)
			{
				dirArr->Insert((UOSInt)~si, Text::StrCopyNew(u8buff));
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
	*(UInt32*)&buff[24] = AddString(strArr, fileName, 20);
	*(Int32*)&buff[28] = (Int32)dirArr->GetCount();
	*(Int32*)&buff[32] = (Int32)env->GetImageFileCnt();
	*(Int32*)&buff[36] = (Int32)env->GetFontStyleCount();
	*(Int32*)&buff[40] = (Int32)env->GetLineStyleCount();
	*(Int32*)&buff[44] = (Int32)env->GetItemCount(0);
	*(Int32*)&buff[48] = (Int32)env->GetDefLineStyle();
	*(Int32*)&buff[52] = (Int32)env->GetDefFontStyle();

	stm->Write(buff, 56);
	stmPos = 56;
	i = 0;
	j = dirArr->GetCount();
	while (i < j)
	{
		tmpStr = dirArr->GetItem(i);
		*(Int32*)&buff[0] = 0;
		*(UInt32*)&buff[4] = AddString(strArr, tmpStr, stmPos);

		stm->Write(buff, 8);
		stmPos += 8;

		i++;
	}

	i = 0;
	j = env->GetImageFileCnt();
	while (i < j)
	{
		env->GetImageFileInfo(i, &imgInfo);
		Text::StrConcat(u8buff, imgInfo.fileName);
		si = Text::StrLastIndexOf(u8buff, IO::Path::PATH_SEPERATOR);

		*(Int32*)&buff[0] = 0;
		WriteUInt32(&buff[4], AddString(strArr, &u8buff[si + 1], stmPos));
		u8buff[si] = 0;
		*(Int32*)&buff[8] = (Int32)dirArr->SortedIndexOf(u8buff);
		*(Int32*)&buff[12] = (Int32)imgInfo.index;

		stm->Write(buff, 16);
		stmPos += 16;

		i++;
	}

	i = 0;
	j = env->GetFontStyleCount();
	while (i < j)
	{
		const UTF8Char *fontName;
		Double fontSize;
		Bool bold;
		UInt32 fontColor;
		UOSInt buffSize;
		UInt32 buffColor;

		u8ptr = env->GetFontStyleName(i, u8buff);
		env->GetFontStyle(i, &fontName, &fontSize, &bold, &fontColor, &buffSize, &buffColor);

		*(Int32*)&buff[0] = 0;
		if (u8ptr)
		{
			WriteUInt32(&buff[4], AddString(strArr, u8buff, stmPos));
		}
		else
		{
			*(Int32*)&buff[4] = 0;
		}
		*(Int32*)&buff[8] = 0;
		WriteUInt32(&buff[12], AddString(strArr, fontName, stmPos + 8));
		*(Int32*)&buff[16] = Math::Double2Int32(fontSize / 0.75);
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
		u8ptr = env->GetLineStyleName(i, u8buff);
		*(Int32*)&buff[0] = 0;
		if (u8ptr)
		{
			*(UInt32*)&buff[4] = AddString(strArr, u8buff, stmPos);
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

			env->GetLineStyleLayer(i, k, &color, &thick, &pattern, &npattern);
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

	WriteGroupItems(env, 0, &stmPos, stm, strArr, dirArr);

	tmpArr = strArr->GetValues();
	j = tmpArr->GetCount();
	i = 0;
	while (i < j)
	{
		strRec = tmpArr->GetItem(i);

		stm->Write(strRec->strBytes, strRec->byteSize);
		k = strRec->ofstList->GetCount();
		while (k-- > 0)
		{
			stm->SeekFromBeginning(strRec->ofstList->GetItem(k));
			stm->Write((UInt8*)&stmPos, 4);
		}
		stmPos += strRec->byteSize;
		stm->SeekFromBeginning(stmPos);

		i++;
	}



	i = dirArr->GetCount();
	while (i-- > 0)
	{
		tmpStr = dirArr->GetItem(i);
		Text::StrDelNew(tmpStr);
	}
	DEL_CLASS(dirArr);

	tmpArr = strArr->GetValues();
	i = tmpArr->GetCount();
	while (i-- > 0)
	{
		strRec = tmpArr->GetItem(i);
		DEL_CLASS(strRec->ofstList);
		MemFree(strRec->strBytes);
		MemFree(strRec);
	}
	DEL_CLASS(strArr);
	return true;
}

void Exporter::MEVExporter::GetMapDirs(Map::MapEnv *env, Data::ArrayListStrUTF8 *dirArr, Map::MapEnv::GroupItem *group)
{
	UOSInt i = 0;
	UOSInt j = env->GetItemCount(group);
	OSInt k;
	UTF8Char sbuff[256];

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
			Map::IMapDrawLayer *layer = lyr->layer;
			if (layer->GetSourceName(sbuff))
			{
				k = Text::StrLastIndexOf(sbuff, '\\');
				if (k >= 0)
				{
					sbuff[k] = 0;
					k = dirArr->SortedIndexOf(sbuff);
					if (k < 0)
					{
						dirArr->Insert((UOSInt)~k, Text::StrCopyNew(sbuff));
					}
				}
			}
		}
		i++;
	}
}

UInt32 Exporter::MEVExporter::AddString(Data::StringUTF8Map<MEVStrRecord*> *strArr, const UTF8Char *strVal, UInt32 fileOfst)
{
	MEVStrRecord *strRec = strArr->Get(strVal);
	if (strRec == 0)
	{
		strRec = MemAlloc(MEVStrRecord, 1);
		strRec->byteSize = (UInt32)Text::StrCharCnt(strVal);
		strRec->strBytes = MemAlloc(UInt8, strRec->byteSize + 1);
		NEW_CLASS(strRec->ofstList, Data::ArrayListUInt32());
		MemCopyNO(strRec->strBytes, strVal, strRec->byteSize);
		strArr->Put(strVal, strRec);
	}
	strRec->ofstList->Add(fileOfst);
	return strRec->byteSize;
}

void Exporter::MEVExporter::WriteGroupItems(Map::MapEnv *env, Map::MapEnv::GroupItem *group, UInt32 *stmPos, IO::SeekableStream *stm, Data::StringUTF8Map<Exporter::MEVExporter::MEVStrRecord*> *strArr, Data::ArrayListStrUTF8 *dirArr)
{
	UInt8 buff[256];
	UTF8Char u8buff[256];
	const UTF8Char *u8ptr;
	Map::MapEnv::LayerItem setting;
	UOSInt i = 0;
	UOSInt j = env->GetItemCount(group);
	OSInt k;
	while (i < j)
	{
		Map::MapEnv::MapItem *item = env->GetItem(group, i);

		if (item->itemType == Map::MapEnv::IT_GROUP)
		{
			*(Int32*)&buff[0] = item->itemType;

			u8ptr = env->GetGroupName((Map::MapEnv::GroupItem*)item);
			*(Int32*)&buff[4] = 0;
			if (u8ptr)
			{
				*(UInt32*)&buff[8] = AddString(strArr, u8ptr, 4 + *stmPos);
			}
			else
			{
				*(UInt32*)&buff[8] = 0;
			}
			*(Int32*)&buff[12] = (Int32)env->GetItemCount((Map::MapEnv::GroupItem*)item);
			stm->Write(buff, 16);
			*stmPos = 16 + *stmPos;

			WriteGroupItems(env, (Map::MapEnv::GroupItem*)item, stmPos, stm, strArr, dirArr);
		}
		else if (item->itemType == Map::MapEnv::IT_LAYER)
		{
			Map::DrawLayerType ltype;
			Map::MapEnv::LayerItem *lyr = (Map::MapEnv::LayerItem*)item;
			Map::IMapDrawLayer *layer = lyr->layer;
			*(Int32*)&buff[0] = item->itemType;
			layer->GetSourceName(u8buff);
			*(Int32*)&buff[4] = 0;
			k = Text::StrLastIndexOf(u8buff, IO::Path::PATH_SEPERATOR);
			*(UInt32*)&buff[8] = AddString(strArr, &u8buff[k + 1], 4 + *stmPos);
			if (k >= 0)
			{
				u8buff[k] = 0;
				*(Int32*)&buff[12] = (Int32)dirArr->SortedIndexOf(u8buff);
			}
			else
			{
				*(Int32*)&buff[12] = -1;
			}
			env->GetLayerProp(&setting, group, i);
			ltype = layer->GetLayerType();
			*(UInt32*)&buff[16] = layer->GetCodePage();
			WriteUInt32(&buff[24], (UInt32)setting.labelCol);
			*(Int32*)&buff[28] = setting.flags;
			*(Int32*)&buff[32] = Math::Double2Int32(setting.minScale);
			*(Int32*)&buff[36] = Math::Double2Int32(setting.maxScale);
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
