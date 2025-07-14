#include "Stdafx.h"
#include "IO/DirectoryPackage.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Map/HKRoadNetwork2.h"
#include "Map/HKSpeedLimit.h"
#include "Map/HKTrafficLayer2.h"
#include "Map/VectorLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Geometry/PointZ.h"
#include "Parser/FileParser/ZIPParser.h"
#include "Parser/ObjParser/FileGDB2Parser.h"

Map::HKRoadNetwork2::HKRoadNetwork2(Text::CStringNN fgdbPath, Optional<Math::ArcGISPRJParser> prjParser)
{
	Parser::ObjParser::FileGDB2Parser parser;
	parser.SetArcGISPRJParser(prjParser);
	if (IO::Path::GetPathType(fgdbPath) == IO::Path::PathType::Directory)
	{
		IO::DirectoryPackage pkg(fgdbPath);
		this->fgdb = Optional<DB::ReadingDB>::ConvertFrom(parser.ParseObject(pkg, 0, IO::ParserType::ReadingDB));
	}
	else
	{
		Parser::FileParser::ZIPParser zipParser;
		IO::StmData::FileData fd(fgdbPath, false);
		NN<IO::PackageFile> pkg;
		if (Optional<IO::PackageFile>::ConvertFrom(zipParser.ParseFile(fd, 0, IO::ParserType::PackageFile)).SetTo(pkg))
		{
			this->fgdb = Optional<DB::ReadingDB>::ConvertFrom(parser.ParseObject(pkg, 0, IO::ParserType::ReadingDB));
			pkg.Delete();
		}
	}
}

Map::HKRoadNetwork2::HKRoadNetwork2(NN<DB::ReadingDB> fgdb)
{
	this->fgdb = fgdb;
}

Map::HKRoadNetwork2::~HKRoadNetwork2()
{
	this->fgdb.Delete();
}

Bool Map::HKRoadNetwork2::IsError()
{
	return this->fgdb.IsNull();
}

Optional<DB::ReadingDB> Map::HKRoadNetwork2::GetDB()
{
	return this->fgdb;
}

NN<Math::CoordinateSystem> Map::HKRoadNetwork2::CreateCoordinateSystem()
{
	NN<Math::ProjectedCoordinateSystem> csys;
	if (Math::CoordinateSystemManager::CreateProjCoordinateSystemDefName(Math::CoordinateSystemManager::PCST_HK80).SetTo(csys))
		return csys;
	else
		return Math::CoordinateSystemManager::CreateWGS84Csys();
}

Optional<Map::HKSpeedLimit> Map::HKRoadNetwork2::CreateSpeedLimit()
{
	if (this->fgdb.NotNull())
	{
		return NEW_CLASS_D(Map::HKSpeedLimit(*this));
	}
	return 0;
}

Optional<Map::MapDrawLayer> Map::HKRoadNetwork2::CreateTonnesSignLayer()
{
	NN<DB::ReadingDB> fgdb;
	if (!this->fgdb.SetTo(fgdb))
	{
		return 0;
	}
	
	Map::DrawLayerType layerType = Map::DRAW_LAYER_POINT;
	UnsafeArrayOpt<const UTF8Char> colNames[] = {U8STR("Id"), U8STR("MaxWeight"), U8STR("Remarks")};
	DB::DBUtil::ColType colTypes[] = {DB::DBUtil::CT_Int32, DB::DBUtil::CT_Double, DB::DBUtil::CT_VarUTF8Char};
	UOSInt colSize[] = {11, 32, 255};
	UOSInt colDP[] = {0, 10, 0};
	Map::VectorLayer *lyr = 0;
	NEW_CLASS(lyr, Map::VectorLayer(layerType, CSTR("HKRoadNetwork2"), 3, colNames, this->CreateCoordinateSystem(), colTypes, colSize, colDP, 0, CSTR("VehRestrict")));
	
	NN<DB::DBReader> r;
	if (fgdb->QueryTableData(CSTR_NULL, CSTR("VEHICLE_RESTRICTION"), 0, 0, 0, CSTR_NULL, 0).SetTo(r))
	{
		UTF8Char sbuff[256];
		UnsafeArray<UTF8Char> sptr;
		UTF8Char sbuff2[64];
		OSInt shapeCol = -1;
		OSInt vrIdCol = -1;
		OSInt maxWeightCol = -1;
		OSInt remarksCol = -1;
		UOSInt i;
		UOSInt j;
		UnsafeArray<UTF8Char> strs[3];
		i = 0;
		j = r->ColCount();
		while (i < j)
		{
			if (r->GetName(i, sbuff).SetTo(sptr))
			{
				if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Shape")))
				{
					shapeCol = (OSInt)i;
				}
				else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("VR_ID")))
				{
					vrIdCol = (OSInt)i;
				}
				else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("MAX_WEIGHT")))
				{
					maxWeightCol = (OSInt)i;
				}
				else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("REMARKS")))
				{
					remarksCol = (OSInt)i;
				}
			}
			i++;
		}

		while (r->ReadNext())
		{
			if (r->IsNull((UOSInt)maxWeightCol))
			{
			}
			else
			{
				Double maxWeight = r->GetDblOr((UOSInt)maxWeightCol, 0);
				if (maxWeight != 0)
				{
					Int32 vrId = r->GetInt32((UOSInt)vrIdCol);
					NN<Math::Geometry::Vector2D> vec;
					if (r->GetVector((UOSInt)shapeCol).SetTo(vec))
					{
						sbuff[0] = 0;
						r->GetStr((UOSInt)remarksCol, sbuff, sizeof(sbuff));

						strs[0] = sbuff2;
						strs[1] = Text::StrInt32(sbuff2, vrId) + 1;
						UInt32 iMaxWeight = (UInt32)maxWeight;
						if (maxWeight - iMaxWeight < 0.1)
						{
							Text::StrUInt32(strs[1], iMaxWeight);
						}
						else
						{
							Text::StrDouble(strs[1], maxWeight);
						}
						strs[2] = sbuff;
						lyr->AddVector2(vec, UnsafeArray<UnsafeArrayOpt<const UTF8Char>>::ConvertFrom(UARR(strs)));
					}
				}
			}
		}

		fgdb->CloseReader(r);
	}
	return lyr;
}

Optional<Map::HKTrafficLayer2> Map::HKRoadNetwork2::CreateTrafficLayer(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Optional<Text::EncodingFactory> encFact)
{
	if (this->fgdb.NotNull())
	{
		return NEW_CLASS_D(Map::HKTrafficLayer2(clif, ssl, encFact, *this));
	}
	return 0;
}

Optional<Map::ShortestPath3D> Map::HKRoadNetwork2::CreateShortestPath()
{
	Optional<Map::ShortestPath3D> ret = 0;
	NN<Map::ShortestPath3D> shortestPath;
	NN<DB::ReadingDB> fgdb;
	NN<DB::DBReader> r;
	UOSInt i;
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	if (this->fgdb.SetTo(fgdb))
	{
		if (fgdb->QueryTableData(CSTR_NULL, CSTR("CENTERLINE"), 0, 0, 0, 0, 0).SetTo(r))
		{
			UOSInt routeIdCol = INVALID_INDEX;
			UOSInt travelDirCol = INVALID_INDEX;
			UOSInt shapeCol = INVALID_INDEX;
			UOSInt colCount = r->ColCount();
			i = colCount;
			while (i-- > 0)
			{
				sptr = r->GetName(i, sbuff).Or(sbuff);
				if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("ROUTE_ID")))
					routeIdCol = i;
				else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("TRAVEL_DIRECTION")))
					travelDirCol = i;
				else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("SHAPE")))
					shapeCol = i;
			}
			Data::Int32FastMapNN<Map::ShortestPath3D::LineInfo> lineMap;
			NN<Map::ShortestPath3D::LineInfo> lineInfo;
			if (routeIdCol != INVALID_INDEX && travelDirCol != INVALID_INDEX && shapeCol != INVALID_INDEX)
			{
				UnsafeArray<Optional<Text::String>> properties = MemAllocArr(Optional<Text::String>, colCount);
				NEW_CLASSNN(shortestPath, Map::ShortestPath3D(Math::CoordinateSystemManager::CreateProjCoordinateSystemDefNameOrDef(Math::CoordinateSystemManager::PCST_HK80), 100));
				ret = shortestPath;
				NN<Math::Geometry::Vector2D> vec;
				Int32 routeId;
				Int32 travelDir;
				while (r->ReadNext())
				{
					routeId = r->GetInt32(routeIdCol);
					travelDir = r->GetInt32(travelDirCol);
					if (r->GetVector(shapeCol).SetTo(vec))
					{
						i = colCount;
						while (i-- > 0)
						{
							if (i == shapeCol)
								properties[i] = 0;
							else
								properties[i] = r->GetNewStr(i);
						}
						if (shortestPath->AddPath(vec, Data::DataArray<Optional<Text::String>>(properties, colCount), travelDir == 1, false).SetTo(lineInfo))
						{
							lineMap.Put(routeId, lineInfo);
						}
						i = colCount;
						while (i-- > 0)
						{
							OPTSTR_DEL(properties[i]);
						}
					}
				}
				MemFreeArr(properties);
			}
			fgdb->CloseReader(r);

			if (ret.SetTo(shortestPath))
			{
				if (fgdb->QueryTableData(CSTR_NULL, CSTR("INTERSECTION"), 0, 0, 0, 0, 0).SetTo(r))
				{
					UOSInt id1 = INVALID_INDEX;
					UOSInt id2 = INVALID_INDEX;
					UOSInt id3 = INVALID_INDEX;
					UOSInt id4 = INVALID_INDEX;
					UOSInt id5 = INVALID_INDEX;
					UOSInt id6 = INVALID_INDEX;
					UOSInt id7 = INVALID_INDEX;
					UOSInt id8 = INVALID_INDEX;
					UOSInt id9 = INVALID_INDEX;
					UOSInt id10 = INVALID_INDEX;
					Int32 routeId;
					shapeCol = INVALID_INDEX;
					colCount = r->ColCount();
					i = colCount;
					while (i-- > 0)
					{
						sptr = r->GetName(i, sbuff).Or(sbuff);
						if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("SHAPE")))
							shapeCol = i;
						else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("RD_ID_1")))
							id1 = i;
						else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("RD_ID_2")))
							id2 = i;
						else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("RD_ID_3")))
							id3 = i;
						else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("RD_ID_4")))
							id4 = i;
						else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("RD_ID_5")))
							id5 = i;
						else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("RD_ID_6")))
							id6 = i;
						else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("RD_ID_7")))
							id7 = i;
						else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("RD_ID_8")))
							id8 = i;
						else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("RD_ID_9")))
							id9 = i;
						else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("RD_ID_10")))
							id10 = i;
					}

					if (shapeCol == INVALID_INDEX)
					{
						ret.Delete();
					}
					else
					{
						NN<Math::Geometry::Vector2D> vec;
						Math::Coord2DDbl pos;
						Double z;
						while (r->ReadNext())
						{
							if (r->GetVector(shapeCol).SetTo(vec))
							{
								pos = vec->GetCenter();
								z = 0;
								if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::Point)
								{
									NN<Math::Geometry::Point> pt = NN<Math::Geometry::Point>::ConvertFrom(vec);
									if (pt->HasZ())
									{
										z = NN<Math::Geometry::PointZ>::ConvertFrom(pt)->GetZ();
									}
								}
								vec.Delete();
								if (id1 != INVALID_INDEX && (routeId = r->GetInt32(id1)) != 0 && lineMap.Get(routeId).SetTo(lineInfo))
								{
									shortestPath->AddNode(pos, z, lineInfo);
								}
								if (id2 != INVALID_INDEX && (routeId = r->GetInt32(id2)) != 0 && lineMap.Get(routeId).SetTo(lineInfo))
								{
									shortestPath->AddNode(pos, z, lineInfo);
								}
								if (id3 != INVALID_INDEX && (routeId = r->GetInt32(id3)) != 0 && lineMap.Get(routeId).SetTo(lineInfo))
								{
									shortestPath->AddNode(pos, z, lineInfo);
								}
								if (id4 != INVALID_INDEX && (routeId = r->GetInt32(id4)) != 0 && lineMap.Get(routeId).SetTo(lineInfo))
								{
									shortestPath->AddNode(pos, z, lineInfo);
								}
								if (id5 != INVALID_INDEX && (routeId = r->GetInt32(id5)) != 0 && lineMap.Get(routeId).SetTo(lineInfo))
								{
									shortestPath->AddNode(pos, z, lineInfo);
								}
								if (id6 != INVALID_INDEX && (routeId = r->GetInt32(id6)) != 0 && lineMap.Get(routeId).SetTo(lineInfo))
								{
									shortestPath->AddNode(pos, z, lineInfo);
								}
								if (id7 != INVALID_INDEX && (routeId = r->GetInt32(id7)) != 0 && lineMap.Get(routeId).SetTo(lineInfo))
								{
									shortestPath->AddNode(pos, z, lineInfo);
								}
								if (id8 != INVALID_INDEX && (routeId = r->GetInt32(id8)) != 0 && lineMap.Get(routeId).SetTo(lineInfo))
								{
									shortestPath->AddNode(pos, z, lineInfo);
								}
								if (id9 != INVALID_INDEX && (routeId = r->GetInt32(id9)) != 0 && lineMap.Get(routeId).SetTo(lineInfo))
								{
									shortestPath->AddNode(pos, z, lineInfo);
								}
								if (id10 != INVALID_INDEX && (routeId = r->GetInt32(id10)) != 0 && lineMap.Get(routeId).SetTo(lineInfo))
								{
									shortestPath->AddNode(pos, z, lineInfo);
								}
							}
						}
					}
					fgdb->CloseReader(r);
					shortestPath->BuildNetwork();
				}
				else
				{
					ret.Delete();
				}
			}
		}
	}
	return ret;
}


Text::CStringNN Map::HKRoadNetwork2::GetDownloadURL()
{
	return CSTR("https://static.data.gov.hk/td/road-network-v2/RdNet_IRNP.gdb.zip");
}

Text::CStringNN Map::HKRoadNetwork2::GetDefFileName()
{
	return CSTR("RdNet_IRNP.gdb");
}
