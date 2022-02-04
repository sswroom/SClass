#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/Path.h"
#include "IO/StreamReader.h"
#include "IO/StreamDataStream.h"
#include "Map/MapEnv.h"
#include "Map/MapManager.h"
#include "Math/CoordinateSystemManager.h"
#include "Parser/FileParser/MEVParser.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

Parser::FileParser::MEVParser::MEVParser()
{
	this->parsers = 0;
	this->mapMgr = 0;
}

Parser::FileParser::MEVParser::~MEVParser()
{
}

Int32 Parser::FileParser::MEVParser::GetName()
{
	return *(Int32*)"MENV";
}

void Parser::FileParser::MEVParser::SetParserList(Parser::ParserList *parsers)
{
	this->parsers = parsers;
}

void Parser::FileParser::MEVParser::SetMapManager(Map::MapManager *mapMgr)
{
	this->mapMgr = mapMgr;
}

void Parser::FileParser::MEVParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MapEnv)
	{
		selector->AddFilter((const UTF8Char*)"*.mev", (const UTF8Char*)"MEV File");
	}
}

IO::ParserType Parser::FileParser::MEVParser::GetParserType()
{
	return IO::ParserType::MapEnv;
}

IO::ParsedObject *Parser::FileParser::MEVParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UInt8 buff[512];
	UInt32 currPos = 0;
	UInt32 dirCnt;
	UInt32 imgFileCnt;
	UInt32 fontStyleCnt;
	UInt32 lineStyleCnt;
	UInt32 itemCnt;
	UInt32 defLineStyle;
	UInt32 defFontStyle;
	Parser::FileParser::MEVParser::MEVImageInfo *imgFileArr;
	const WChar **dirArr;
	WChar *sptr;
	UTF8Char *u8ptr;
	WChar *sptr2;
	WChar sbuff[256];
	WChar sbuff2[256];
	UTF8Char u8buff[256];
	UTF8Char u8buff2[256];

	UOSInt i;
	UOSInt j;
	
	if (this->parsers == 0)
		return 0;
	if (this->mapMgr == 0)
		return 0;
	fd->GetRealData(0, 12, buff);
	currPos = 12;
	if (*(Int32*)&buff[0] != *(Int32*)"SMEv" || ReadUInt32(&buff[4]) != 0x81c0fe1a)
	{
		return 0;
	}

	UInt32 initSize = ReadUInt32(&buff[8]);
	fd->GetRealData(currPos, initSize, buff);
	currPos += initSize;
	Map::MapEnv *env;
	NEW_CLASS(env, Map::MapEnv(fd->GetFullName()->v, ReadUInt32(&buff[0]), Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84)));
	env->SetNString(ReadUInt32(&buff[4]));
	dirCnt = ReadUInt32(&buff[16]);
	imgFileCnt = ReadUInt32(&buff[20]);
	fontStyleCnt = ReadUInt32(&buff[24]);
	lineStyleCnt = ReadUInt32(&buff[28]);
	itemCnt = ReadUInt32(&buff[32]);
	defLineStyle = ReadUInt32(&buff[36]);
	defFontStyle = ReadUInt32(&buff[40]);

	fd->GetRealData(ReadUInt32(&buff[8]), ReadUInt32(&buff[12]), &buff[16]);
	sptr = Text::StrUTF8_WCharC(sbuff, &buff[16], ReadUInt32(&buff[12]), 0);
	sptr2 = Text::StrUTF8_WChar(sbuff2, fd->GetFullName()->v, 0);
	while (sptr > sbuff && sptr2 > sbuff2)
	{
		if (sptr[-1] != sptr2[-1])
			break;
		sptr--;
		sptr2--;
	}
	*sptr = 0;
	*sptr2 = 0;
	if (sptr == sbuff && sptr2 == sbuff2)
	{
		sptr2 = 0;
	}
	else
	{
		Text::StrConcat(sptr2 + 1, sbuff);
	}

	imgFileArr = MemAlloc(MEVImageInfo, imgFileCnt);
	dirArr = MemAlloc(const WChar*, dirCnt);
	i = 0;
	while (i < dirCnt)
	{
		fd->GetRealData(currPos, 8, buff);
		if (ReadUInt32(&buff[4]) > 0)
		{
			fd->GetRealData(ReadUInt32(&buff[0]), ReadUInt32(&buff[4]), &buff[8]);
			Text::StrUTF8_WCharC(sbuff, &buff[8], ReadUInt32(&buff[4]), 0);
			if (sptr2)
			{
				Text::StrReplace(sbuff, sptr2 + 1, sbuff2);
			}
			dirArr[i] = Text::StrCopyNew(sbuff);
		}
		else
		{
			dirArr[i] = 0;
		}
		currPos += 8;
		i++;
	}
	
	i = 0;
	while (i < imgFileCnt)
	{
		fd->GetRealData(currPos, 16, buff);
		imgFileArr[i].fileIndex = ReadInt32(&buff[12]);
		fd->GetRealData(ReadUInt32(&buff[0]), ReadUInt32(&buff[4]), &buff[16]);
		sptr = Text::StrConcat(sbuff, dirArr[ReadInt32(&buff[8])]);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		Text::StrUTF8_WCharC(sptr, &buff[16], ReadUInt32(&buff[4]), 0);

		Text::String *s = Text::String::NewNotNull(sbuff);
		imgFileArr[i].envIndex = env->AddImage(s->ToCString(), this->parsers);
		s->Release();
		i++;
		currPos += 16;
	}

	i = 0;
	while (i < fontStyleCnt)
	{
		Double fontSize;
		fd->GetRealData(currPos, 36, buff);

		fd->GetRealData(ReadUInt32(&buff[8]), ReadUInt32(&buff[12]), &buff[36]);
		Text::StrConcatC(u8buff, &buff[36], ReadUInt32(&buff[12]));
		if (ReadUInt32(&buff[4]) > 0)
		{
			fd->GetRealData(ReadUInt32(&buff[0]), ReadUInt32(&buff[4]), &buff[36]);
			Text::StrConcatC(u8ptr = u8buff2, &buff[36], ReadUInt32(&buff[4]));
		}
		else
		{
			u8ptr = 0;
		}
		fontSize = *(Int32*)&buff[16] * 0.75;
		env->AddFontStyle(u8ptr, u8buff, fontSize, ReadUInt32(&buff[20]) != 0, ReadUInt32(&buff[24]), ReadUInt32(&buff[28]), ReadUInt32(&buff[32]));
		
		i++;
		currPos += 36;
	}

	i = 0;
	while (i < lineStyleCnt)
	{
		fd->GetRealData(currPos, 12, buff);
		currPos += 12;
		env->AddLineStyle();
		if (ReadUInt32(&buff[4]) > 0)
		{
			fd->GetRealData(ReadUInt32(&buff[0]), ReadUInt32(&buff[4]), &buff[12]);
			Text::StrConcatC(u8buff2, &buff[12], ReadUInt32(&buff[4]));
			env->SetLineStyleName(i, u8buff2);
		}

		j = ReadUInt32(&buff[8]);
		while (j-- > 0)
		{
			fd->GetRealData(currPos, 12, buff);
			currPos += 12;
			if (ReadUInt32(&buff[8]) > 0)
			{
				fd->GetRealData(currPos, ReadUInt32(&buff[8]), &buff[12]);
				currPos += ReadUInt32(&buff[8]);
			}
			env->AddLineStyleLayer(i, ReadUInt32(&buff[0]), ReadUInt32(&buff[4]), &buff[12], ReadUInt32(&buff[8]));
		}

		i++;
	}
	env->SetDefLineStyle(defLineStyle);
	env->SetDefFontStyle(defFontStyle);

	ReadItems(fd, env, itemCnt, &currPos, 0, dirArr, imgFileArr);

	i = dirCnt;
	while (i-- > 0)
	{
		if (dirArr[i])
		{
			Text::StrDelNew(dirArr[i]);
		}
	}
	MemFree(dirArr);
	MemFree(imgFileArr);
	return env;
}

void Parser::FileParser::MEVParser::ReadItems(IO::IStreamData *fd, Map::MapEnv *env, UInt32 itemCnt, UInt32 *currPos, Map::MapEnv::GroupItem *group, const WChar **dirArr, MEVImageInfo *imgInfos)
{
	UInt8 buff[512];
	WChar sbuff[256];
	WChar *sptr;
	UOSInt i = 0;
	while (i < itemCnt)
	{
		fd->GetRealData(*currPos, 4, buff);
		*currPos = 4 + *currPos;
		if (*(Int32*)&buff[0] == Map::MapEnv::IT_GROUP)
		{
			fd->GetRealData(*currPos, 12, buff);
			fd->GetRealData(ReadUInt32(&buff[0]), ReadUInt32(&buff[4]), &buff[12]);
			Text::StrUTF8_WCharC(sbuff, &buff[12], ReadUInt32(&buff[4]), 0);
			*currPos = 12 + *currPos;
			
			const UTF8Char *u8ptr = Text::StrToUTF8New(sbuff);
			Map::MapEnv::GroupItem *item = env->AddGroup(group, u8ptr);
			Text::StrDelNew(u8ptr);
			ReadItems(fd, env, ReadUInt32(&buff[8]), currPos, item, dirArr, imgInfos);
		}
		else if (*(Int32*)&buff[0] == Map::MapEnv::IT_LAYER)
		{
			fd->GetRealData(*currPos, 20, buff);
			*currPos = 20 + *currPos;

			fd->GetRealData(ReadUInt32(&buff[0]), ReadUInt32(&buff[4]), &buff[20]);
			sptr = Text::StrConcat(Text::StrConcat(sbuff, dirArr[ReadUInt32(&buff[8])]), L"\\");
			Text::StrUTF8_WCharC(sptr, &buff[20], ReadUInt32(&buff[4]), 0);
			if (ReadUInt32(&buff[12]))
			{
				this->parsers->SetCodePage(ReadUInt32(&buff[12]));
			}
			Text::String *s = Text::String::NewNotNull(sbuff);
			Map::IMapDrawLayer *layer = this->mapMgr->LoadLayer(s->ToCString(), this->parsers, env);
			s->Release();
			if (layer)
			{
				Map::MapEnv::LayerItem setting;
				UOSInt layerId = env->AddLayer(group, layer, false);
				env->GetLayerProp(&setting, group, layerId);

				if (ReadUInt32(&buff[16]) == 1)
				{
					fd->GetRealData(*currPos, 32, buff);
					*currPos = 32 + *currPos;

					setting.labelCol = ReadUInt32(&buff[0]);
					setting.flags = ReadInt32(&buff[4]);
					setting.minScale = ReadInt32(&buff[8]);
					setting.maxScale = ReadInt32(&buff[12]);
					setting.priority = ReadInt32(&buff[16]);
					setting.fontStyle = ReadUInt32(&buff[20]);
					setting.imgIndex = (UInt32)(ReadInt32(&buff[28]) - imgInfos[ReadInt32(&buff[24])].fileIndex + imgInfos[ReadInt32(&buff[24])].envIndex);

					env->SetLayerProp(&setting, group, layerId);
				}
				else if (ReadUInt32(&buff[16]) == 3)
				{
					fd->GetRealData(*currPos, 28, buff);
					*currPos = 28 + *currPos;

					setting.labelCol = ReadUInt32(&buff[0]);
					setting.flags = ReadInt32(&buff[4]);
					setting.minScale = ReadInt32(&buff[8]);
					setting.maxScale = ReadInt32(&buff[12]);
					setting.priority = ReadInt32(&buff[16]);
					setting.fontStyle = ReadUInt32(&buff[20]);
					setting.lineStyle = ReadUInt32(&buff[24]);

					env->SetLayerProp(&setting, group, layerId);
				}
				else if (ReadUInt32(&buff[16]) == 5)
				{
					fd->GetRealData(*currPos, 32, buff);
					*currPos = 32 + *currPos;

					setting.labelCol = ReadUInt32(&buff[0]);
					setting.flags = ReadInt32(&buff[4]);
					setting.minScale = ReadInt32(&buff[8]);
					setting.maxScale = ReadInt32(&buff[12]);
					setting.priority = ReadInt32(&buff[16]);
					setting.fontStyle = ReadUInt32(&buff[20]);
					setting.lineStyle = ReadUInt32(&buff[24]);
					setting.fillStyle = ReadUInt32(&buff[28]);

					env->SetLayerProp(&setting, group, layerId);
				}
				else
				{
					fd->GetRealData(*currPos, 24, buff);
					*currPos = 24 + *currPos;
					setting.labelCol = ReadUInt32(&buff[0]);
					setting.flags = ReadInt32(&buff[4]);
					setting.minScale = ReadInt32(&buff[8]);
					setting.maxScale = ReadInt32(&buff[12]);
					setting.priority = ReadInt32(&buff[16]);
					setting.fontStyle = ReadUInt32(&buff[20]);

					env->SetLayerProp(&setting, group, layerId);
				}
			}
			else
			{
				if (ReadUInt32(&buff[16]) == 1)
				{
					*currPos = 32 + *currPos;
				}
				else if (ReadUInt32(&buff[16]) == 3)
				{
					*currPos = 28 + *currPos;
				}
				else if (ReadUInt32(&buff[16]) == 5)
				{
					*currPos = 32 + *currPos;
				}
				else
				{
					*currPos = 24 + *currPos;
				}
			}
		}
		i++;
	}
}
