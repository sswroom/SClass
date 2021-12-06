#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListDbl.h"
#include "IO/FileStream.h"
#include "IO/StreamReader.h"
#include "IO/StreamDataStream.h"
#include "IO/Path.h"
#include "Map/MapEnv.h"
#include "Map/MapManager.h"
#include "Map/VectorLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Point3D.h"
#include "Math/Polygon.h"
#include "Math/Polyline3D.h"
#include "Parser/FileParser/TXTParser.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

Parser::FileParser::TXTParser::TXTParser()
{
	this->codePage = 0;
	this->parsers = 0;
	this->mapMgr = 0;
}

Parser::FileParser::TXTParser::~TXTParser()
{
}

Int32 Parser::FileParser::TXTParser::GetName()
{
	return *(Int32*)"TXTP";
}

void Parser::FileParser::TXTParser::SetCodePage(UInt32 codePage)
{
	this->codePage = codePage;
}

void Parser::FileParser::TXTParser::SetParserList(Parser::ParserList *parsers)
{
	this->parsers = parsers;
}

void Parser::FileParser::TXTParser::SetMapManager(Map::MapManager *mapMgr)
{
	this->mapMgr = mapMgr;
}

void Parser::FileParser::TXTParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MapEnv)
	{
		selector->AddFilter((const UTF8Char*)"*.txt", (const UTF8Char*)"Maplayer Defination File");
	}
}

IO::ParserType Parser::FileParser::TXTParser::GetParserType()
{
	return IO::ParserType::MapEnv;
}

IO::ParsedObject *Parser::FileParser::TXTParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UTF8Char sbuff[512];
	UTF8Char baseDir[256];
	UTF8Char sbuff2[512];
	UTF8Char sbuff3[256];
	UTF8Char u8buff[512];
	UTF8Char *fileName;
	UTF8Char *sarr[20];
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	if (!fd->GetFullName()->EndsWithICase((const UTF8Char*)".TXT"))
	{
		return 0;
	}
	IO::StreamDataStream *stm;
	IO::StreamReader *reader;
	NEW_CLASS(stm, IO::StreamDataStream(fd));
	NEW_CLASS(reader, IO::StreamReader(stm, this->codePage));
	if (reader->ReadLine(sbuff, 255) == 0)
	{
		DEL_CLASS(reader);
		DEL_CLASS(stm);
		return 0;
	}
	if (Text::StrStartsWith(sbuff, (const UTF8Char*)"1,") && Text::StrCountChar(sbuff, ',') == 4 && this->parsers != 0 && this->mapMgr != 0)
	{
		Map::MapEnv *env;
		Map::MapEnv::GroupItem *currGroup = 0;
		if (Text::StrSplitTrim(sarr, 8, sbuff, ',') != 5)
		{
			DEL_CLASS(reader);
			DEL_CLASS(stm);
			return 0;
		}

		Math::CoordinateSystem *csys = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84);
		NEW_CLASS(env, Map::MapEnv(fd->GetFullName()->v, ToColor(Text::StrHex2UInt32C(sarr[1])), csys));
		env->SetNString(Text::StrToUInt32(sarr[4]));
		fileName = baseDir;

		while (reader->ReadLine(sbuff, 255))
		{
			if (Text::StrStartsWith(sbuff, (const UTF8Char*)"2,"))
			{
				if (Text::StrSplitTrim(sarr, 10, sbuff, ',') != 2)
				{
					this->mapMgr->ClearMap(env);
					DEL_CLASS(env);
					DEL_CLASS(reader);
					DEL_CLASS(stm);
					return 0;
				}
				Text::StrConcat(baseDir, u8buff);
				fileName = IO::Path::AppendPath(baseDir, sarr[1]);
			}
			else if (Text::StrStartsWith(sbuff, (const UTF8Char*)"3,"))
			{
				UInt8 *pattern;
				if ((i = Text::StrSplitTrim(sarr, 10, sbuff, ',')) < 5)
				{
					this->mapMgr->ClearMap(env);
					DEL_CLASS(env);
					DEL_CLASS(reader);
					DEL_CLASS(stm);
					return 0;
				}
				j = Text::StrToUInt32(sarr[1]);
				if (env->GetLineStyleCount() <= j)
				{
					j = env->AddLineStyle();
				}
				if (i > 5)
				{
					k = i;
					pattern = MemAlloc(UInt8, k - 5);
					while (k-- > 5)
					{
						pattern[k - 5] = (UInt8)Text::StrToInt32(sarr[k]);
					}
				}
				else
				{
					pattern = 0;
				}
				env->AddLineStyleLayer(j, ToColor(Text::StrHex2UInt32C(sarr[4])), Text::StrToUInt32(sarr[3]), pattern, i - 5);
				if (pattern)
				{
					MemFree(pattern);
					pattern = 0;
				}
			}
			else if (Text::StrStartsWith(sbuff, (const UTF8Char*)"13,"))
			{
				if (Text::StrSplitTrim(sarr, 10, sbuff, ',') != 3)
				{
					this->mapMgr->ClearMap(env);
					DEL_CLASS(env);
					DEL_CLASS(reader);
					DEL_CLASS(stm);
					return 0;
				}
				env->SetLineStyleName(Text::StrToUInt32(sarr[1]), sarr[2]);
			}
			else if (Text::StrStartsWith(sbuff, (const UTF8Char*)"5,"))
			{
				const UTF8Char *fontName;
				Double fontSize;
				Bool bold;
				UInt32 fontColor;
				UOSInt buffSize;
				UInt32 buffColor;
				Int32 addFont;

				if (Text::StrSplitTrim(sarr, 10, sbuff, ',') != 7)
				{
					this->mapMgr->ClearMap(env);
					DEL_CLASS(env);
					DEL_CLASS(reader);
					DEL_CLASS(stm);
					return 0;
				}
				i = Text::StrToUInt32(sarr[1]);
				if (env->GetFontStyle(i, &fontName, &fontSize, &bold, &fontColor, &buffSize, &buffColor))
				{
					addFont = 1;
				}
				else
				{
					addFont = 2;
					fontName = sarr[3];
					fontSize = Text::StrToDouble(sarr[4]);
					bold = false;
					fontColor = 0;
					buffSize = 0;
					buffColor = 0;
				}
				j = Text::StrToUInt32(sarr[2]);
				if (j == 0)
				{
					bold = Text::StrToInt32(sarr[5]) != 0;
					fontColor = ToColor(Text::StrHex2UInt32C(sarr[6]));
				}
				else if (j == 4)
				{
					buffSize = Text::StrToUInt32(sarr[5]);
					buffColor = ToColor(Text::StrHex2UInt32C(sarr[6]));
				}
				else
				{
					addFont = 0;
				}
				if (addFont == 1)
				{
					env->ChgFontStyle(i, fontName, fontSize, bold, fontColor, buffSize, buffColor);
				}
				else if (addFont == 2)
				{
					env->AddFontStyle(0, fontName, fontSize, bold, fontColor, buffSize, buffColor);
				}
			}
			else if (Text::StrStartsWith(sbuff, (const UTF8Char*)"6,"))
			{
				Map::MapEnv::LayerItem setting;
				if (Text::StrSplitTrim(sarr, 10, sbuff, ',') != 5)
				{
					this->mapMgr->ClearMap(env);
					DEL_CLASS(env);
					DEL_CLASS(reader);
					DEL_CLASS(stm);
					return 0;
				}
				Text::StrConcat(Text::StrConcat(fileName, sarr[1]), (const UTF8Char*)".cip");
				Map::IMapDrawLayer *lyr = this->mapMgr->LoadLayer(baseDir, this->parsers, env);
				if (lyr)
				{
					i = env->AddLayer(currGroup, lyr, false);

					if (env->GetLayerProp(&setting, currGroup, i))
					{
						setting.minScale = Text::StrToInt32(sarr[2]);
						setting.maxScale = Text::StrToInt32(sarr[3]);
						setting.lineStyle = Text::StrToUInt32(sarr[4]);
						env->SetLayerProp(&setting, currGroup, i);
					}
				}
			}
			else if (Text::StrStartsWith(sbuff, (const UTF8Char*)"7,"))
			{
				Map::MapEnv::LayerItem setting;
				if (Text::StrSplitTrim(sarr, 10, sbuff, ',') != 6)
				{
					this->mapMgr->ClearMap(env);
					DEL_CLASS(env);
					DEL_CLASS(reader);
					DEL_CLASS(stm);
					return 0;
				}

				Text::StrConcat(Text::StrConcat(fileName, sarr[1]), (const UTF8Char*)".cip");
				Map::IMapDrawLayer *lyr = this->mapMgr->LoadLayer(baseDir, this->parsers, env);
				if (lyr)
				{
					i = env->AddLayer(currGroup, lyr, false);

					if (env->GetLayerProp(&setting, currGroup, i))
					{
						setting.minScale = Text::StrToUInt32(sarr[2]);
						setting.maxScale = Text::StrToUInt32(sarr[3]);
						setting.lineStyle = Text::StrToUInt32(sarr[4]);
						setting.fillStyle = ToColor(Text::StrHex2UInt32C(sarr[5]));
						env->SetLayerProp(&setting, currGroup, i);
					}
				}

			}
			else if (Text::StrStartsWith(sbuff, (const UTF8Char*)"9,"))
			{
				Map::MapEnv::LayerItem setting;
				if (Text::StrSplitTrim(sarr, 10, sbuff, ',') != 7)
				{
					this->mapMgr->ClearMap(env);
					DEL_CLASS(env);
					DEL_CLASS(reader);
					DEL_CLASS(stm);
					return 0;
				}
				Text::StrConcat(Text::StrConcat(fileName, sarr[1]), (const UTF8Char*)".cip");
				Map::IMapDrawLayer *lyr = this->mapMgr->LoadLayer(baseDir, this->parsers, env);
				if (lyr)
				{
					i = env->AddLayer(currGroup, lyr, false);

					if (env->GetLayerProp(&setting, currGroup, i))
					{
						setting.minScale = Text::StrToInt32(sarr[2]);
						setting.maxScale = Text::StrToInt32(sarr[3]);
						setting.priority = Text::StrToInt32(sarr[4]);
						setting.fontStyle = Text::StrToUInt32(sarr[5]);
						j = Text::StrToUInt32(sarr[6]);
						setting.flags = Map::MapEnv::SFLG_HIDESHAPE | Map::MapEnv::SFLG_SHOWLABEL;
						if (j & 1)
						{
							setting.flags |= Map::MapEnv::SFLG_ROTATE;
						}
						if (j & 2)
						{
							setting.flags |= Map::MapEnv::SFLG_SMART;
						}
						if (j & 4)
						{
							setting.flags |= Map::MapEnv::SFLG_ALIGN;
						}
						env->SetLayerProp(&setting, currGroup, i);
					}
				}
			}
			else if (Text::StrStartsWith(sbuff, (const UTF8Char*)"10,"))
			{
				Map::MapEnv::LayerItem setting;
				if (Text::StrSplitTrim(sarr, 10, sbuff, ',') != 5)
				{
					this->mapMgr->ClearMap(env);
					DEL_CLASS(env);
					DEL_CLASS(reader);
					DEL_CLASS(stm);
					return 0;
				}
				OSInt si;
				Text::StrConcat(Text::StrConcat(fileName, sarr[1]), (const UTF8Char*)".cip");
				Map::IMapDrawLayer *lyr = this->mapMgr->LoadLayer(baseDir, this->parsers, env);
				Text::StrConcat(sbuff3, sbuff2);
				IO::Path::AppendPath(sbuff3, sarr[4]);
				si = env->AddImage(sbuff3, this->parsers);
				if (lyr && si != -1)
				{
					i = env->AddLayer(currGroup, lyr, false);

					if (env->GetLayerProp(&setting, currGroup, i))
					{
						setting.minScale = Text::StrToInt32(sarr[2]);
						setting.maxScale = Text::StrToInt32(sarr[3]);
						setting.imgIndex = (UOSInt)si;
						env->SetLayerProp(&setting, currGroup, i);
					}
				}
			}
			else if (Text::StrStartsWith(sbuff, (const UTF8Char*)"15,"))
			{
				currGroup = env->AddGroup(0, &sbuff[3]);
			}
		}

		//////////////////////////////
		DEL_CLASS(reader);
		DEL_CLASS(stm);
		return env;
	}
	else if (fd->IsFullFile() && Text::StrStartsWith(sbuff, (const UTF8Char*)"OBJECTID,") && Text::StrEndsWith(sbuff, (const UTF8Char*)","))
	{
		fileName = fd->GetFullName()->ConcatTo(u8buff);
		Text::StrConcat(&fileName[-4], (const UTF8Char*)"_Coord.txt");
		if (IO::Path::GetPathType(u8buff) != IO::Path::PathType::File)
		{
			DEL_CLASS(reader);
			DEL_CLASS(stm);
			return 0;
		}

		Bool hasPt = false;
		Bool hasPL = false;
		Bool hasPG = false;
		UInt32 srid = 0;
		IO::FileStream *fs2;
		IO::StreamReader *reader2;
		Data::ArrayListDbl ptX;
		Data::ArrayListDbl ptY;
		Data::ArrayListDbl ptZ;
		Data::Int32Map<Math::Vector2D *> vecMap;
		Data::Int32Map<Bool> vecUsed;
		Int32 currId = 0;
		Math::Polygon *pg;
		Math::Polyline3D *pl;
		Math::Point3D *pt;
		Math::Vector2D *vec;
		Double *ptList;
		Double *hList;

		NEW_CLASS(fs2, IO::FileStream(u8buff, IO::FileStream::FileMode::ReadOnly, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		NEW_CLASS(reader2, IO::StreamReader(fs2, 0));
		while (reader2->ReadLine(sbuff2, 512))
		{
			i = Text::StrSplit(sarr, 4, sbuff2, ',');
			if (i == 1)
			{
				if (currId != 0)
				{
					j = ptX.GetCount();
					if (j == 0)
					{
					}
					else if (j == 1)
					{
						hasPt = true;
						NEW_CLASS(pt, Math::Point3D(srid, ptX.GetItem(0), ptY.GetItem(0), ptZ.GetItem(0)));
						vecMap.Put(currId, pt);
					}
					else if (ptX.GetItem(j - 1) == ptX.GetItem(0) && ptY.GetItem(j - 1) == ptY.GetItem(0) && ptZ.GetItem(j - 1) == ptZ.GetItem(0))
					{
						hasPG = true;
						NEW_CLASS(pg, Math::Polygon(srid, 1, j - 1));
						ptList = pg->GetPointList(&k);
						k = 0;
						l = 0;
						while (k < j - 1)
						{
							ptList[l] = ptX.GetItem(k);
							ptList[l+1] = ptY.GetItem(k);
							k++;
							l += 2;
						}
						vecMap.Put(currId, pg);
					}
					else
					{
						hasPL = true;
						NEW_CLASS(pl, Math::Polyline3D(srid, 1, j));
						ptList = pl->GetPointList(&k);
						hList = pl->GetAltitudeList(&k);
						k = 0;
						l = 0;
						while (k < j)
						{
							ptList[l] = ptX.GetItem(k);
							ptList[l+1] = ptY.GetItem(k);
							hList[k] = ptZ.GetItem(k);
							k++;
							l += 2;
						}
						vecMap.Put(currId, pl);
					}
				}
				ptX.Clear();
				ptY.Clear();
				ptZ.Clear();
				currId = Text::StrToInt32(sarr[0]);
			}
			else if (i == 3)
			{
				ptX.Add(Text::StrToDouble(sarr[0]));
				ptY.Add(Text::StrToDouble(sarr[1]));
				ptZ.Add(Text::StrToDouble(sarr[2]));
			}
		}
		DEL_CLASS(reader2);
		DEL_CLASS(fs2);

		if (currId != 0)
		{
			j = ptX.GetCount();
			if (j == 0)
			{
			}
			else if (j == 1)
			{
				hasPt = true;
				NEW_CLASS(pt, Math::Point3D(srid, ptX.GetItem(0), ptY.GetItem(0), ptZ.GetItem(0)));
				vecMap.Put(currId, pt);
			}
			else if (ptX.GetItem(j - 1) == ptX.GetItem(0) && ptY.GetItem(j - 1) == ptY.GetItem(0) && ptZ.GetItem(j - 1) == ptZ.GetItem(0))
			{
				hasPG = true;
				NEW_CLASS(pg, Math::Polygon(srid, 1, j - 1));
				ptList = pg->GetPointList(&k);
				k = 0;
				l = 0;
				while (k < j - 1)
				{
					ptList[l] = ptX.GetItem(k);
					ptList[l+1] = ptY.GetItem(k);
					k++;
					l += 2;
				}
				vecMap.Put(currId, pg);
			}
			else
			{
				hasPL = true;
				NEW_CLASS(pl, Math::Polyline3D(srid, 1, j));
				ptList = pl->GetPointList(&k);
				hList = pl->GetAltitudeList(&k);
				k = 0;
				l = 0;
				while (k < j)
				{
					ptList[l] = ptX.GetItem(k);
					ptList[l+1] = ptY.GetItem(k);
					hList[k] = ptZ.GetItem(k);
					k++;
					l += 2;
				}
				vecMap.Put(currId, pl);
			}
		}
		ptX.Clear();
		ptY.Clear();
		ptZ.Clear();

		Map::VectorLayer *lyr;
		Map::DrawLayerType lyrType;
		Data::ArrayList<Int32> *vecIdList;
		Data::ArrayList<Math::Vector2D *> *vecList;
		if (hasPt && !hasPL && !hasPG)
		{
			lyrType = Map::DRAW_LAYER_POINT3D;
		}
		else if (hasPL && !hasPG && !hasPt)
		{
			lyrType = Map::DRAW_LAYER_POLYLINE3D;
		}
		else if (hasPG && !hasPL && !hasPt)
		{
			lyrType = Map::DRAW_LAYER_POLYGON;
		}
		else
		{
			lyrType = Map::DRAW_LAYER_MIXED;
		}
		j = Text::StrSplit(sarr, 20, sbuff, ',');
		NEW_CLASS(lyr, Map::VectorLayer(lyrType, fd->GetFullFileName(), j, (const UTF8Char **)sarr, Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84), 2, 0));
		while (reader->ReadLine(sbuff, 512))
		{
			i = Text::StrSplit(sarr, 20, sbuff, ',');
			if (i == j)
			{
				currId = Text::StrToInt32(sarr[1]);
				vec = vecMap.Get(currId);
				if (vec)
				{
					vecUsed.Put(currId, true);
					lyr->AddVector(vec, (const UTF8Char **)sarr);
				}
			}
		}
		vecIdList = vecMap.GetKeys();
		vecList = vecMap.GetValues();
		i = vecList->GetCount();
		while (i-- > 0)
		{
			currId = vecIdList->GetItem(i);
			if (!vecUsed.Get(currId))
			{
				vec = vecList->GetItem(i);
				DEL_CLASS(vec);
			}
		}

		DEL_CLASS(reader);
		DEL_CLASS(stm);
		return lyr;
	}
	DEL_CLASS(reader);
	DEL_CLASS(stm);
	return 0;
}

UInt32 Parser::FileParser::TXTParser::ToColor(UInt32 val)
{
	return 0xFF000000 | ((val & 255) << 16) | (val & 0xff00) | ((val & 0xff0000) >> 16);
}
