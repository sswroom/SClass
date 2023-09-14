#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListDbl.h"
#include "Data/FastMap.h"
#include "IO/FileStream.h"
#include "IO/StreamReader.h"
#include "IO/StreamDataStream.h"
#include "IO/Path.h"
#include "Map/MapEnv.h"
#include "Map/MapManager.h"
#include "Map/VectorLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Geometry/LineString.h"
#include "Math/Geometry/PointZ.h"
#include "Math/Geometry/Polygon.h"
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

void Parser::FileParser::TXTParser::PrepareSelector(IO::FileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MapEnv)
	{
		selector->AddFilter(CSTR("*.txt"), CSTR("Maplayer Defination File"));
	}
}

IO::ParserType Parser::FileParser::TXTParser::GetParserType()
{
	return IO::ParserType::MapEnv;
}

IO::ParsedObject *Parser::FileParser::TXTParser::ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	UTF8Char sbuff[512];
	UTF8Char baseDir[256];
	UTF8Char sbuff2[512];
	UTF8Char sbuff3[256];
	UTF8Char sbuff4[512];
	UTF8Char *fileName;
	UTF8Char *baseDirEnd;
	Text::PString sarr[20];
	const UTF8Char *csarr[20];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	if (!fd->GetFullName()->EndsWithICase(UTF8STRC(".TXT")))
	{
		return 0;
	}
	IO::StreamDataStream stm(fd);
	IO::StreamReader reader(stm, this->codePage);
	if ((sptr = reader.ReadLine(sbuff, 255)) == 0)
	{
		return 0;
	}
	if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("1,")) && Text::StrCountChar(sbuff, ',') == 4 && this->parsers != 0 && this->mapMgr != 0)
	{
		Map::MapEnv *env;
		Map::MapEnv::GroupItem *currGroup = 0;
		if (Text::StrSplitTrimP(sarr, 8, {sbuff, (UOSInt)(sptr - sbuff)}, ',') != 5)
		{
			return 0;
		}

		NotNullPtr<Math::CoordinateSystem> csys = Math::CoordinateSystemManager::CreateDefaultCsys();
		NEW_CLASS(env, Map::MapEnv(fd->GetFullName()->ToCString(), ToColor(Text::StrHex2UInt32C(sarr[1].v)), csys));
		env->SetNString(Text::StrToUInt32(sarr[4].v));
		fileName = baseDir;

		while ((sptr = reader.ReadLine(sbuff, 255)) != 0)
		{
			if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("2,")))
			{
				if (Text::StrSplitTrimP(sarr, 10, {sbuff, (UOSInt)(sptr - sbuff)}, ',') != 2)
				{
					this->mapMgr->ClearMap(env);
					DEL_CLASS(env);
					return 0;
				}
				fileName = fd->GetFullFileName()->ConcatTo(baseDir);
				fileName = IO::Path::AppendPath(baseDir, fileName, sarr[1].ToCString());
			}
			else if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("3,")))
			{
				UInt8 *pattern;
				if ((i = Text::StrSplitTrimP(sarr, 10, {sbuff, (UOSInt)(sptr - sbuff)}, ',')) < 5)
				{
					this->mapMgr->ClearMap(env);
					DEL_CLASS(env);
					return 0;
				}
				j = Text::StrToUInt32(sarr[1].v);
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
						pattern[k - 5] = (UInt8)Text::StrToInt32(sarr[k].v);
					}
				}
				else
				{
					pattern = 0;
				}
				env->AddLineStyleLayer(j, ToColor(Text::StrHex2UInt32C(sarr[4].v)), Text::StrToUInt32(sarr[3].v), pattern, i - 5);
				if (pattern)
				{
					MemFree(pattern);
					pattern = 0;
				}
			}
			else if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("13,")))
			{
				if (Text::StrSplitTrimP(sarr, 10, {sbuff, (UOSInt)(sptr - sbuff)}, ',') != 3)
				{
					this->mapMgr->ClearMap(env);
					DEL_CLASS(env);
					return 0;
				}
				env->SetLineStyleName(Text::StrToUInt32(sarr[1].v), sarr[2].ToCString());
			}
			else if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("5,")))
			{
				Text::String *fontName;
				Text::CString pfontName;
				Double fontSize;
				Bool bold;
				UInt32 fontColor;
				UOSInt buffSize;
				UInt32 buffColor;
				Int32 addFont;

				if (Text::StrSplitTrimP(sarr, 10, {sbuff, (UOSInt)(sptr - sbuff)}, ',') != 7)
				{
					this->mapMgr->ClearMap(env);
					DEL_CLASS(env);
					return 0;
				}
				i = Text::StrToUInt32(sarr[1].v);
				if (env->GetFontStyle(i, fontName, fontSize, bold, fontColor, buffSize, buffColor))
				{
					addFont = 1;
				}
				else
				{
					addFont = 2;
					pfontName = sarr[3].ToCString();
					fontSize = Text::StrToDouble(sarr[4].v);
					bold = false;
					fontColor = 0;
					buffSize = 0;
					buffColor = 0;
				}
				j = Text::StrToUInt32(sarr[2].v);
				if (j == 0)
				{
					bold = Text::StrToInt32(sarr[5].v) != 0;
					fontColor = ToColor(Text::StrHex2UInt32C(sarr[6].v));
				}
				else if (j == 4)
				{
					buffSize = Text::StrToUInt32(sarr[5].v);
					buffColor = ToColor(Text::StrHex2UInt32C(sarr[6].v));
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
					env->AddFontStyle(CSTR_NULL, pfontName, fontSize, bold, fontColor, buffSize, buffColor);
				}
			}
			else if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("6,")))
			{
				Map::MapEnv::LayerItem setting;
				if (Text::StrSplitTrimP(sarr, 10, {sbuff, (UOSInt)(sptr - sbuff)}, ',') != 5)
				{
					this->mapMgr->ClearMap(env);
					DEL_CLASS(env);
					return 0;
				}
				baseDirEnd = Text::StrConcatC(sarr[1].ConcatTo(fileName), UTF8STRC(".cip"));
				Map::MapDrawLayer *lyr = this->mapMgr->LoadLayer({baseDir, (UOSInt)(baseDirEnd - baseDir)}, this->parsers, env);
				if (lyr)
				{
					i = env->AddLayer(currGroup, lyr, false);

					if (env->GetLayerProp(&setting, currGroup, i))
					{
						setting.minScale = Text::StrToInt32(sarr[2].v);
						setting.maxScale = Text::StrToInt32(sarr[3].v);
						setting.lineStyle = Text::StrToUInt32(sarr[4].v);
						env->SetLayerProp(&setting, currGroup, i);
					}
				}
			}
			else if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("7,")))
			{
				Map::MapEnv::LayerItem setting;
				if (Text::StrSplitTrimP(sarr, 10, {sbuff, (UOSInt)(sptr - sbuff)}, ',') != 6)
				{
					this->mapMgr->ClearMap(env);
					DEL_CLASS(env);
					return 0;
				}

				baseDirEnd = Text::StrConcatC(sarr[1].ConcatTo(fileName), UTF8STRC(".cip"));
				Map::MapDrawLayer *lyr = this->mapMgr->LoadLayer({baseDir, (UOSInt)(baseDirEnd - baseDir)}, this->parsers, env);
				if (lyr)
				{
					i = env->AddLayer(currGroup, lyr, false);

					if (env->GetLayerProp(&setting, currGroup, i))
					{
						setting.minScale = Text::StrToUInt32(sarr[2].v);
						setting.maxScale = Text::StrToUInt32(sarr[3].v);
						setting.lineStyle = Text::StrToUInt32(sarr[4].v);
						setting.fillStyle = ToColor(Text::StrHex2UInt32C(sarr[5].v));
						env->SetLayerProp(&setting, currGroup, i);
					}
				}

			}
			else if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("9,")))
			{
				Map::MapEnv::LayerItem setting;
				if (Text::StrSplitTrimP(sarr, 10, {sbuff, (UOSInt)(sptr - sbuff)}, ',') != 7)
				{
					this->mapMgr->ClearMap(env);
					DEL_CLASS(env);
					return 0;
				}
				baseDirEnd = Text::StrConcatC(sarr[1].ConcatTo(fileName), UTF8STRC(".cip"));
				Map::MapDrawLayer *lyr = this->mapMgr->LoadLayer({baseDir, (UOSInt)(baseDirEnd - baseDir)}, this->parsers, env);
				if (lyr)
				{
					i = env->AddLayer(currGroup, lyr, false);

					if (env->GetLayerProp(&setting, currGroup, i))
					{
						setting.minScale = Text::StrToInt32(sarr[2].v);
						setting.maxScale = Text::StrToInt32(sarr[3].v);
						setting.priority = Text::StrToInt32(sarr[4].v);
						setting.fontStyle = Text::StrToUInt32(sarr[5].v);
						j = Text::StrToUInt32(sarr[6].v);
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
			else if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("10,")))
			{
				Map::MapEnv::LayerItem setting;
				if (Text::StrSplitTrimP(sarr, 10, {sbuff, (UOSInt)(sptr - sbuff)}, ',') != 5)
				{
					this->mapMgr->ClearMap(env);
					DEL_CLASS(env);
					return 0;
				}
				OSInt si;
				baseDirEnd = Text::StrConcatC(sarr[1].ConcatTo(fileName), UTF8STRC(".cip"));
				Map::MapDrawLayer *lyr = this->mapMgr->LoadLayer({baseDir, (UOSInt)(baseDirEnd - baseDir)}, this->parsers, env);
				baseDirEnd = Text::StrConcat(sbuff3, sbuff2);
				baseDirEnd = IO::Path::AppendPath(sbuff3, baseDirEnd, sarr[4].ToCString());
				si = env->AddImage({sbuff3, (UOSInt)(baseDirEnd - sbuff3)}, this->parsers);
				if (lyr && si != -1)
				{
					i = env->AddLayer(currGroup, lyr, false);

					if (env->GetLayerProp(&setting, currGroup, i))
					{
						setting.minScale = Text::StrToInt32(sarr[2].v);
						setting.maxScale = Text::StrToInt32(sarr[3].v);
						setting.imgIndex = (UOSInt)si;
						env->SetLayerProp(&setting, currGroup, i);
					}
				}
			}
			else if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("15,")))
			{
				currGroup = env->AddGroup(0, CSTRP(&sbuff[3], sptr));
			}
		}

		//////////////////////////////
		return env;
	}
	else if (fd->IsFullFile() && Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("OBJECTID,")) && Text::StrEndsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC(",")))
	{
		fileName = fd->GetFullName()->ConcatTo(sbuff4);
		fileName = Text::StrConcatC(&fileName[-4], UTF8STRC("_Coord.txt"));
		if (IO::Path::GetPathType(CSTRP(sbuff4, fileName)) != IO::Path::PathType::File)
		{
			return 0;
		}

		Bool hasPt = false;
		Bool hasPL = false;
		Bool hasPG = false;
		UInt32 srid = 0;
		Data::ArrayListDbl ptX;
		Data::ArrayListDbl ptY;
		Data::ArrayListDbl ptZ;
		Data::FastMap<Int32, Math::Geometry::Vector2D *> vecMap;
		Data::FastMap<Int32, Bool> vecUsed;
		Int32 currId = 0;
		Math::Geometry::Polygon *pg;
		Math::Geometry::LineString *pl;
		Math::Geometry::PointZ *pt;
		Math::Geometry::Vector2D *vec;
		NotNullPtr<Math::Geometry::Vector2D> nnvec;
		Math::Coord2DDbl *ptList;
		Double *hList;

		{
			IO::FileStream fs2({sbuff4, (UOSInt)(fileName - sbuff4)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			IO::StreamReader reader2(fs2, 0);
			while ((sptr2 = reader2.ReadLine(sbuff2, 512)) != 0)
			{
				i = Text::StrSplitP(sarr, 4, {sbuff2, (UOSInt)(sptr2 - sbuff2)}, ',');
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
							NEW_CLASS(pt, Math::Geometry::PointZ(srid, ptX.GetItem(0), ptY.GetItem(0), ptZ.GetItem(0)));
							vecMap.Put(currId, pt);
						}
						else if (ptX.GetItem(j - 1) == ptX.GetItem(0) && ptY.GetItem(j - 1) == ptY.GetItem(0) && ptZ.GetItem(j - 1) == ptZ.GetItem(0))
						{
							hasPG = true;
							NEW_CLASS(pg, Math::Geometry::Polygon(srid, 1, j - 1, false, false));
							ptList = pg->GetPointList(k);
							k = 0;
							while (k < j - 1)
							{
								ptList[k].x = ptX.GetItem(k);
								ptList[k].y = ptY.GetItem(k);
								k++;
							}
							vecMap.Put(currId, pg);
						}
						else
						{
							hasPL = true;
							NEW_CLASS(pl, Math::Geometry::LineString(srid, j, true, false));
							ptList = pl->GetPointList(k);
							hList = pl->GetZList(k);
							k = 0;
							while (k < j)
							{
								ptList[k].x = ptX.GetItem(k);
								ptList[k].y = ptY.GetItem(k);
								hList[k] = ptZ.GetItem(k);
								k++;
							}
							vecMap.Put(currId, pl);
						}
					}
					ptX.Clear();
					ptY.Clear();
					ptZ.Clear();
					currId = Text::StrToInt32(sarr[0].v);
				}
				else if (i == 3)
				{
					ptX.Add(Text::StrToDouble(sarr[0].v));
					ptY.Add(Text::StrToDouble(sarr[1].v));
					ptZ.Add(Text::StrToDouble(sarr[2].v));
				}
			}
		}

		if (currId != 0)
		{
			j = ptX.GetCount();
			if (j == 0)
			{
			}
			else if (j == 1)
			{
				hasPt = true;
				NEW_CLASS(pt, Math::Geometry::PointZ(srid, ptX.GetItem(0), ptY.GetItem(0), ptZ.GetItem(0)));
				vecMap.Put(currId, pt);
			}
			else if (ptX.GetItem(j - 1) == ptX.GetItem(0) && ptY.GetItem(j - 1) == ptY.GetItem(0) && ptZ.GetItem(j - 1) == ptZ.GetItem(0))
			{
				hasPG = true;
				NEW_CLASS(pg, Math::Geometry::Polygon(srid, 1, j - 1, false, false));
				ptList = pg->GetPointList(k);
				k = 0;
				while (k < j - 1)
				{
					ptList[k].x = ptX.GetItem(k);
					ptList[k].y = ptY.GetItem(k);
					k++;
				}
				vecMap.Put(currId, pg);
			}
			else
			{
				hasPL = true;
				NEW_CLASS(pl, Math::Geometry::LineString(srid, j, true, false));
				ptList = pl->GetPointList(k);
				hList = pl->GetZList(k);
				k = 0;
				while (k < j)
				{
					ptList[k].x = ptX.GetItem(k);
					ptList[k].y = ptY.GetItem(k);
					hList[k] = ptZ.GetItem(k);
					k++;
				}
				vecMap.Put(currId, pl);
			}
		}
		ptX.Clear();
		ptY.Clear();
		ptZ.Clear();

		Map::VectorLayer *lyr;
		Map::DrawLayerType lyrType;
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
		j = Text::StrSplitP(sarr, 20, {sbuff, (UOSInt)(sptr - sbuff)}, ',');
		NEW_CLASS(lyr, Map::VectorLayer(lyrType, fd->GetFullFileName(), j, (const UTF8Char **)sarr, Math::CoordinateSystemManager::CreateDefaultCsys(), 2, 0));
		while ((sptr = reader.ReadLine(sbuff, 512)) != 0)
		{
			i = Text::StrSplitP(sarr, 20, {sbuff, (UOSInt)(sptr - sbuff)}, ',');
			if (i == j)
			{
				currId = Text::StrToInt32(sarr[1].v);
				if (nnvec.Set(vecMap.Get(currId)))
				{
					while (i-- > 0)
					{
						csarr[i] = sarr[i].v;
					}
					vecUsed.Put(currId, true);
					lyr->AddVector(nnvec, csarr);
				}
			}
		}
		i = vecMap.GetCount();
		while (i-- > 0)
		{
			currId = vecMap.GetKey(i);
			if (!vecUsed.Get(currId))
			{
				vec = vecMap.GetItem(i);
				DEL_CLASS(vec);
			}
		}
		return lyr;
	}
	return 0;
}

UInt32 Parser::FileParser::TXTParser::ToColor(UInt32 val)
{
	return 0xFF000000 | ((val & 255) << 16) | (val & 0xff00) | ((val & 0xff0000) >> 16);
}
