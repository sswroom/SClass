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

void Parser::FileParser::MEVParser::SetParserList(Optional<Parser::ParserList> parsers)
{
	this->parsers = parsers;
}

void Parser::FileParser::MEVParser::SetMapManager(Optional<Map::MapManager> mapMgr)
{
	this->mapMgr = mapMgr;
}

void Parser::FileParser::MEVParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MapEnv)
	{
		selector->AddFilter(CSTR("*.mev"), CSTR("MEV File"));
	}
}

IO::ParserType Parser::FileParser::MEVParser::GetParserType()
{
	return IO::ParserType::MapEnv;
}

Optional<IO::ParsedObject> Parser::FileParser::MEVParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
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
	UnsafeArray<UnsafeArrayOpt<const WChar>> dirArr;
	UnsafeArray<WChar> wptr;
	UnsafeArray<WChar> wptr2;
	UnsafeArrayOpt<WChar> optwptr2;
	UnsafeArray<const WChar> cwptr;
	WChar wbuff[256];
	WChar wbuff2[256];
	UTF8Char sbuff[256];
	UTF8Char sbuff2[256];
	UnsafeArray<UTF8Char> u8ptr;
	UnsafeArray<UTF8Char> u8ptr2;
	NN<Parser::ParserList> parsers;
	NN<Map::MapManager> mapMgr;

	UOSInt i;
	UOSInt j;
	
	if (!this->parsers.SetTo(parsers))
		return 0;
	if (!this->mapMgr.SetTo(mapMgr))
		return 0;
	currPos = 12;
	if (*(Int32*)&hdr[0] != *(Int32*)"SMEv" || ReadUInt32(&hdr[4]) != 0x81c0fe1a)
	{
		return 0;
	}

	UInt32 initSize = ReadUInt32(&hdr[8]);
	fd->GetRealData(currPos, initSize, BYTEARR(buff));
	currPos += initSize;
	NN<Map::MapEnv> env;
	NEW_CLASSNN(env, Map::MapEnv(fd->GetFullName()->ToCString(), ReadUInt32(&buff[0]), Math::CoordinateSystemManager::CreateWGS84Csys()));
	env->SetNString(ReadUInt32(&buff[4]));
	dirCnt = ReadUInt32(&buff[16]);
	imgFileCnt = ReadUInt32(&buff[20]);
	fontStyleCnt = ReadUInt32(&buff[24]);
	lineStyleCnt = ReadUInt32(&buff[28]);
	itemCnt = ReadUInt32(&buff[32]);
	defLineStyle = ReadUInt32(&buff[36]);
	defFontStyle = ReadUInt32(&buff[40]);

	fd->GetRealData(ReadUInt32(&buff[8]), ReadUInt32(&buff[12]), BYTEARR(buff).SubArray(16));
	wptr = Text::StrUTF8_WCharC(wbuff, &buff[16], ReadUInt32(&buff[12]), 0);
	wptr2 = Text::StrUTF8_WChar(wbuff2, fd->GetFullName()->v, 0);
	while (wptr > wbuff && wptr2 > wbuff2)
	{
		if (wptr[-1] != wptr2[-1])
			break;
		wptr--;
		wptr2--;
	}
	*wptr = 0;
	*wptr2 = 0;
	if (wptr == wbuff && wptr2 == wbuff2)
	{
		optwptr2 = 0;
	}
	else
	{
		optwptr2 = wptr2;
		Text::StrConcat(wptr2 + 1, wbuff);
	}

	imgFileArr = MemAlloc(MEVImageInfo, imgFileCnt);
	dirArr = MemAllocArr(UnsafeArrayOpt<const WChar>, dirCnt);
	i = 0;
	while (i < dirCnt)
	{
		fd->GetRealData(currPos, 8, BYTEARR(buff));
		if (ReadUInt32(&buff[4]) > 0)
		{
			fd->GetRealData(ReadUInt32(&buff[0]), ReadUInt32(&buff[4]), BYTEARR(buff).SubArray(8));
			Text::StrUTF8_WCharC(wbuff, &buff[8], ReadUInt32(&buff[4]), 0);
			if (optwptr2.SetTo(wptr2))
			{
				Text::StrReplaceW(wbuff, wptr2 + 1, wbuff2);
			}
			dirArr[i] = Text::StrCopyNew(wbuff);
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
		fd->GetRealData(currPos, 16, BYTEARR(buff));
		imgFileArr[i].fileIndex = ReadInt32(&buff[12]);
		fd->GetRealData(ReadUInt32(&buff[0]), ReadUInt32(&buff[4]), BYTEARR(buff).SubArray(16));
		if (dirArr[ReadInt32(&buff[8])].SetTo(cwptr))
			wptr = Text::StrConcat(wbuff, cwptr);
		else
			wptr = wbuff;
		*wptr++ = IO::Path::PATH_SEPERATOR;
		Text::StrUTF8_WCharC(wptr, &buff[16], ReadUInt32(&buff[4]), 0);

		NN<Text::String> s = Text::String::NewNotNull(wbuff);
		imgFileArr[i].envIndex = env->AddImage(s->ToCString(), parsers);
		s->Release();
		i++;
		currPos += 16;
	}

	i = 0;
	while (i < fontStyleCnt)
	{
		Double fontSize;
		fd->GetRealData(currPos, 36, BYTEARR(buff));

		fd->GetRealData(ReadUInt32(&buff[8]), ReadUInt32(&buff[12]), BYTEARR(buff).SubArray(36));
		u8ptr = Text::StrConcatC(sbuff, &buff[36], ReadUInt32(&buff[12]));
		if (ReadUInt32(&buff[4]) > 0)
		{
			fd->GetRealData(ReadUInt32(&buff[0]), ReadUInt32(&buff[4]), BYTEARR(buff).SubArray(36));
			u8ptr2 = Text::StrConcatC(sbuff2, &buff[36], ReadUInt32(&buff[4]));
		}
		else
		{
			u8ptr2 = sbuff2;
			*u8ptr2 = 0;
		}
		fontSize = *(Int32*)&buff[16] * 0.75;
		env->AddFontStyle(CSTRP(sbuff2, u8ptr2), CSTRP(sbuff, u8ptr), fontSize, ReadUInt32(&buff[20]) != 0, ReadUInt32(&buff[24]), ReadUInt32(&buff[28]), ReadUInt32(&buff[32]));
		
		i++;
		currPos += 36;
	}

	i = 0;
	while (i < lineStyleCnt)
	{
		fd->GetRealData(currPos, 12, BYTEARR(buff));
		currPos += 12;
		env->AddLineStyle();
		if (ReadUInt32(&buff[4]) > 0)
		{
			fd->GetRealData(ReadUInt32(&buff[0]), ReadUInt32(&buff[4]), BYTEARR(buff).SubArray(12));
			u8ptr2 = Text::StrConcatC(sbuff2, &buff[12], ReadUInt32(&buff[4]));
			env->SetLineStyleName(i, CSTRP(sbuff2, u8ptr2));
		}

		j = ReadUInt32(&buff[8]);
		while (j-- > 0)
		{
			fd->GetRealData(currPos, 12, BYTEARR(buff));
			currPos += 12;
			if (ReadUInt32(&buff[8]) > 0)
			{
				fd->GetRealData(currPos, ReadUInt32(&buff[8]), BYTEARR(buff).SubArray(12));
				currPos += ReadUInt32(&buff[8]);
			}
			env->AddLineStyleLayer(i, ReadUInt32(&buff[0]), ReadUInt32(&buff[4]), &buff[12], ReadUInt32(&buff[8]));
		}

		i++;
	}
	env->SetDefLineStyle(defLineStyle);
	env->SetDefFontStyle(defFontStyle);

	ReadItems(fd, env, itemCnt, currPos, 0, dirArr, imgFileArr, parsers, mapMgr);

	i = dirCnt;
	while (i-- > 0)
	{
		if (dirArr[i].SetTo(cwptr))
		{
			Text::StrDelNew(cwptr);
		}
	}
	MemFreeArr(dirArr);
	MemFree(imgFileArr);
	return env;
}

void Parser::FileParser::MEVParser::ReadItems(NN<IO::StreamData> fd, NN<Map::MapEnv> env, UInt32 itemCnt, InOutParam<UInt32> currPos, Optional<Map::MapEnv::GroupItem> group, UnsafeArray<UnsafeArrayOpt<const WChar>> dirArr, MEVImageInfo *imgInfos, NN<Parser::ParserList> parsers, NN<Map::MapManager> mapMgr)
{
	UInt8 buff[512];
	WChar wbuff[256];
	UnsafeArray<WChar> wptr;
	UnsafeArray<const WChar> cwptr;
	UInt32 pos = currPos.Get();
	UOSInt i = 0;
	while (i < itemCnt)
	{
		fd->GetRealData(pos, 4, BYTEARR(buff));
		pos = 4 + pos;
		if (*(Int32*)&buff[0] == Map::MapEnv::IT_GROUP)
		{
			fd->GetRealData(pos, 12, BYTEARR(buff));
			fd->GetRealData(ReadUInt32(&buff[0]), ReadUInt32(&buff[4]), BYTEARR(buff).SubArray(12));
			Text::StrUTF8_WCharC(wbuff, &buff[12], ReadUInt32(&buff[4]), 0);
			pos = 12 + pos;
			
			NN<Text::String> s = Text::String::NewNotNull(wbuff);
			NN<Map::MapEnv::GroupItem> item = env->AddGroup(group, s->ToCString());
			s->Release();
			ReadItems(fd, env, ReadUInt32(&buff[8]), currPos, item, dirArr, imgInfos, parsers, mapMgr);
		}
		else if (*(Int32*)&buff[0] == Map::MapEnv::IT_LAYER)
		{
			fd->GetRealData(pos, 20, BYTEARR(buff));
			pos = 20 + pos;

			fd->GetRealData(ReadUInt32(&buff[0]), ReadUInt32(&buff[4]), BYTEARR(buff).SubArray(20));
			if (dirArr[ReadUInt32(&buff[8])].SetTo(cwptr))
				wptr = Text::StrConcat(wbuff, cwptr);
			else
				wptr = wbuff;
			wptr = Text::StrConcat(wptr, L"\\");
			Text::StrUTF8_WCharC(wptr, &buff[20], ReadUInt32(&buff[4]), 0);
			if (ReadUInt32(&buff[12]))
			{
				parsers->SetCodePage(ReadUInt32(&buff[12]));
			}
			NN<Text::String> s = Text::String::NewNotNull(wbuff);
			NN<Map::MapDrawLayer> layer;
			if (mapMgr->LoadLayer(s->ToCString(), parsers, env).SetTo(layer))
			{
				s->Release();
				Map::MapEnv::LayerItem setting;
				UOSInt layerId = env->AddLayer(group, layer, false);
				env->GetLayerProp(setting, group, layerId);

				if (ReadUInt32(&buff[16]) == 1)
				{
					fd->GetRealData(pos, 32, BYTEARR(buff));
					pos = 32 + pos;

					setting.labelCol = ReadUInt32(&buff[0]);
					setting.flags = ReadInt32(&buff[4]);
					setting.minScale = ReadInt32(&buff[8]);
					setting.maxScale = ReadInt32(&buff[12]);
					setting.priority = ReadInt32(&buff[16]);
					setting.fontStyle = ReadUInt32(&buff[20]);
					setting.imgIndex = (UInt32)(ReadInt32(&buff[28]) - imgInfos[ReadInt32(&buff[24])].fileIndex + imgInfos[ReadInt32(&buff[24])].envIndex);

					env->SetLayerProp(setting, group, layerId);
				}
				else if (ReadUInt32(&buff[16]) == 3)
				{
					fd->GetRealData(pos, 28, BYTEARR(buff));
					pos = 28 + pos;

					setting.labelCol = ReadUInt32(&buff[0]);
					setting.flags = ReadInt32(&buff[4]);
					setting.minScale = ReadInt32(&buff[8]);
					setting.maxScale = ReadInt32(&buff[12]);
					setting.priority = ReadInt32(&buff[16]);
					setting.fontStyle = ReadUInt32(&buff[20]);
					setting.lineStyle = ReadUInt32(&buff[24]);

					env->SetLayerProp(setting, group, layerId);
				}
				else if (ReadUInt32(&buff[16]) == 5)
				{
					fd->GetRealData(pos, 32, BYTEARR(buff));
					pos = 32 + pos;

					setting.labelCol = ReadUInt32(&buff[0]);
					setting.flags = ReadInt32(&buff[4]);
					setting.minScale = ReadInt32(&buff[8]);
					setting.maxScale = ReadInt32(&buff[12]);
					setting.priority = ReadInt32(&buff[16]);
					setting.fontStyle = ReadUInt32(&buff[20]);
					setting.lineStyle = ReadUInt32(&buff[24]);
					setting.fillStyle = ReadUInt32(&buff[28]);

					env->SetLayerProp(setting, group, layerId);
				}
				else
				{
					fd->GetRealData(pos, 24, BYTEARR(buff));
					pos = 24 + pos;
					setting.labelCol = ReadUInt32(&buff[0]);
					setting.flags = ReadInt32(&buff[4]);
					setting.minScale = ReadInt32(&buff[8]);
					setting.maxScale = ReadInt32(&buff[12]);
					setting.priority = ReadInt32(&buff[16]);
					setting.fontStyle = ReadUInt32(&buff[20]);

					env->SetLayerProp(setting, group, layerId);
				}
			}
			else
			{
				s->Release();
				if (ReadUInt32(&buff[16]) == 1)
				{
					pos = 32 + pos;
				}
				else if (ReadUInt32(&buff[16]) == 3)
				{
					pos = 28 + pos;
				}
				else if (ReadUInt32(&buff[16]) == 5)
				{
					pos = 32 + pos;
				}
				else
				{
					pos = 24 + pos;
				}
			}
		}
		i++;
	}
	currPos.Set(pos);
}
