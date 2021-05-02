#include "Stdafx.h"
#include "MyMemory.h"
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

void Parser::FileParser::MEVParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_MAP_ENV_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.mev", (const UTF8Char*)"MEV File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::MEVParser::GetParserType()
{
	return IO::ParsedObject::PT_MAP_ENV_PARSER;
}

IO::ParsedObject *Parser::FileParser::MEVParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt8 buff[512];
	Int32 currPos = 0;
	Int32 dirCnt;
	Int32 imgFileCnt;
	Int32 fontStyleCnt;
	Int32 lineStyleCnt;
	Int32 itemCnt;
	Int32 defLineStyle;
	Int32 defFontStyle;
	Parser::FileParser::MEVParser::MEVImageInfo *imgFileArr;
	const WChar **dirArr;
	WChar *sptr;
	UTF8Char *u8ptr;
	WChar *sptr2;
	WChar sbuff[256];
	WChar sbuff2[256];
	UTF8Char u8buff[256];
	UTF8Char u8buff2[256];

	OSInt i;
	OSInt j;
	
	if (this->parsers == 0)
		return 0;
	if (this->mapMgr == 0)
		return 0;
	fd->GetRealData(0, 12, buff);
	currPos = 12;
	if (*(Int32*)&buff[0] != *(Int32*)"SMEv" || *(Int32*)&buff[4] != (Int32)0x81c0fe1a)
	{
		return 0;
	}

	Int32 initSize = *(Int32*)&buff[8];
	fd->GetRealData(currPos, initSize, buff);
	currPos += initSize;
	Map::MapEnv *env;
	NEW_CLASS(env, Map::MapEnv(fd->GetFullName(), *(Int32*)&buff[0], Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::GeographicCoordinateSystem::GCST_WGS84)));
	env->SetNString(*(Int32*)&buff[4]);
	dirCnt = *(Int32*)&buff[16];
	imgFileCnt = *(Int32*)&buff[20];
	fontStyleCnt = *(Int32*)&buff[24];
	lineStyleCnt = *(Int32*)&buff[28];
	itemCnt = *(Int32*)&buff[32];
	defLineStyle = *(Int32*)&buff[36];
	defFontStyle = *(Int32*)&buff[40];

	fd->GetRealData(*(Int32*)&buff[8], *(Int32*)&buff[12], &buff[16]);
	sptr = Text::StrUTF8_WChar(sbuff, &buff[16], *(Int32*)&buff[12], 0);
	sptr2 = Text::StrUTF8_WChar(sbuff2, fd->GetFullName(), -1, 0);
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
		if (*(Int32*)&buff[4] > 0)
		{
			fd->GetRealData(*(Int32*)&buff[0], *(Int32*)&buff[4], &buff[8]);
			Text::StrUTF8_WChar(sbuff, &buff[8], *(Int32*)&buff[4], 0);
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
		imgFileArr[i].fileIndex = *(Int32*)&buff[12];
		fd->GetRealData(*(Int32*)&buff[0], *(Int32*)&buff[4], &buff[16]);
		sptr = Text::StrConcat(sbuff, dirArr[*(Int32*)&buff[8]]);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		Text::StrUTF8_WChar(sptr, &buff[16], *(Int32*)&buff[4], 0);

		const UTF8Char *u8ptr = Text::StrToUTF8New(sbuff);
		imgFileArr[i].envIndex = env->AddImage(u8ptr, this->parsers);
		Text::StrDelNew(u8ptr);
		i++;
		currPos += 16;
	}

	i = 0;
	while (i < fontStyleCnt)
	{
		Double fontSize;
		fd->GetRealData(currPos, 36, buff);

		fd->GetRealData(*(Int32*)&buff[8], *(Int32*)&buff[12], &buff[36]);
		Text::StrConcatC(u8buff, &buff[36], *(Int32*)&buff[12]);
		if (*(Int32*)&buff[4] > 0)
		{
			fd->GetRealData(*(Int32*)&buff[0], *(Int32*)&buff[4], &buff[36]);
			Text::StrConcatC(u8ptr = u8buff2, &buff[36], *(Int32*)&buff[4]);
		}
		else
		{
			u8ptr = 0;
		}
		fontSize = *(Int32*)&buff[16] * 0.75;
		env->AddFontStyle(u8ptr, u8buff, fontSize, *(Int32*)&buff[20] != 0, *(Int32*)&buff[24], *(Int32*)&buff[28], *(Int32*)&buff[32]);
		
		i++;
		currPos += 36;
	}

	i = 0;
	while (i < lineStyleCnt)
	{
		fd->GetRealData(currPos, 12, buff);
		currPos += 12;
		env->AddLineStyle();
		if (*(Int32*)&buff[4] > 0)
		{
			fd->GetRealData(*(Int32*)&buff[0], *(Int32*)&buff[4], &buff[12]);
			Text::StrConcatC(u8buff2, &buff[12], *(Int32*)&buff[4]);
			env->SetLineStyleName(i, u8buff2);
		}

		j = *(Int32*)&buff[8];
		while (j-- > 0)
		{
			fd->GetRealData(currPos, 12, buff);
			currPos += 12;
			if (*(Int32*)&buff[8] > 0)
			{
				fd->GetRealData(currPos, *(Int32*)&buff[8], &buff[12]);
				currPos += *(Int32*)&buff[8];
			}
			env->AddLineStyleLayer(i, *(Int32*)&buff[0], *(Int32*)&buff[4], &buff[12], *(Int32*)&buff[8]);
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

void Parser::FileParser::MEVParser::ReadItems(IO::IStreamData *fd, Map::MapEnv *env, UInt32 itemCnt, Int32 *currPos, Map::MapEnv::GroupItem *group, const WChar **dirArr, MEVImageInfo *imgInfos)
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
			fd->GetRealData(*(Int32*)&buff[0], *(Int32*)&buff[4], &buff[12]);
			Text::StrUTF8_WChar(sbuff, &buff[12], *(Int32*)&buff[4], 0);
			*currPos = 12 + *currPos;
			
			const UTF8Char *u8ptr = Text::StrToUTF8New(sbuff);
			Map::MapEnv::GroupItem *item = env->AddGroup(group, u8ptr);
			Text::StrDelNew(u8ptr);
			ReadItems(fd, env, *(Int32*)&buff[8], currPos, item, dirArr, imgInfos);
		}
		else if (*(Int32*)&buff[0] == Map::MapEnv::IT_LAYER)
		{
			fd->GetRealData(*currPos, 20, buff);
			*currPos = 20 + *currPos;

			fd->GetRealData(*(Int32*)&buff[0], *(Int32*)&buff[4], &buff[20]);
			sptr = Text::StrConcat(Text::StrConcat(sbuff, dirArr[*(Int32*)&buff[8]]), L"\\");
			Text::StrUTF8_WChar(sptr, &buff[20], *(Int32*)&buff[4], 0);
			if (*(Int32*)&buff[12])
			{
				this->parsers->SetCodePage(*(Int32*)&buff[12]);
			}
			const UTF8Char *u8ptr = Text::StrToUTF8New(sbuff);
			Map::IMapDrawLayer *layer = this->mapMgr->LoadLayer(u8ptr, this->parsers, env);
			Text::StrDelNew(u8ptr);
			if (layer)
			{
				Map::MapEnv::LayerItem setting;
				OSInt layerId = env->AddLayer(group, layer, false);
				env->GetLayerProp(&setting, group, layerId);

				if (*(Int32*)&buff[16] == 1)
				{
					fd->GetRealData(*currPos, 32, buff);
					*currPos = 32 + *currPos;

					setting.labelCol = *(Int32*)&buff[0];
					setting.flags = *(Int32*)&buff[4];
					setting.minScale = *(Int32*)&buff[8];
					setting.maxScale = *(Int32*)&buff[12];
					setting.priority = *(Int32*)&buff[16];
					setting.fontStyle = *(Int32*)&buff[20];
					setting.imgIndex = (Int32)(*(Int32*)&buff[28] - imgInfos[*(Int32*)&buff[24]].fileIndex + imgInfos[*(Int32*)&buff[24]].envIndex);

					env->SetLayerProp(&setting, group, layerId);
				}
				else if (*(Int32*)&buff[16] == 3)
				{
					fd->GetRealData(*currPos, 28, buff);
					*currPos = 28 + *currPos;

					setting.labelCol = *(Int32*)&buff[0];
					setting.flags = *(Int32*)&buff[4];
					setting.minScale = *(Int32*)&buff[8];
					setting.maxScale = *(Int32*)&buff[12];
					setting.priority = *(Int32*)&buff[16];
					setting.fontStyle = *(Int32*)&buff[20];
					setting.lineStyle = *(Int32*)&buff[24];

					env->SetLayerProp(&setting, group, layerId);
				}
				else if (*(Int32*)&buff[16] == 5)
				{
					fd->GetRealData(*currPos, 32, buff);
					*currPos = 32 + *currPos;

					setting.labelCol = *(Int32*)&buff[0];
					setting.flags = *(Int32*)&buff[4];
					setting.minScale = *(Int32*)&buff[8];
					setting.maxScale = *(Int32*)&buff[12];
					setting.priority = *(Int32*)&buff[16];
					setting.fontStyle = *(Int32*)&buff[20];
					setting.lineStyle = *(Int32*)&buff[24];
					setting.fillStyle = *(Int32*)&buff[28];

					env->SetLayerProp(&setting, group, layerId);
				}
				else
				{
					fd->GetRealData(*currPos, 24, buff);
					*currPos = 24 + *currPos;
					setting.labelCol = *(Int32*)&buff[0];
					setting.flags = *(Int32*)&buff[4];
					setting.minScale = *(Int32*)&buff[8];
					setting.maxScale = *(Int32*)&buff[12];
					setting.priority = *(Int32*)&buff[16];
					setting.fontStyle = *(Int32*)&buff[20];

					env->SetLayerProp(&setting, group, layerId);
				}
			}
			else
			{
				if (*(Int32*)&buff[16] == 1)
				{
					*currPos = 32 + *currPos;
				}
				else if (*(Int32*)&buff[16] == 3)
				{
					*currPos = 28 + *currPos;
				}
				else if (*(Int32*)&buff[16] == 5)
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
