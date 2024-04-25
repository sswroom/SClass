#include "Stdafx.h"
#include "IO/DirectoryPackage.h"
#include "Map/HKRoadNetwork2.h"
#include "Map/HKSpeedLimit.h"
#include "Map/HKTrafficLayer2.h"
#include "Map/VectorLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Parser/ObjParser/FileGDB2Parser.h"

Map::HKRoadNetwork2::HKRoadNetwork2(Text::CStringNN fgdbPath, Math::ArcGISPRJParser *prjParser)
{
	IO::DirectoryPackage pkg(fgdbPath);
	Parser::ObjParser::FileGDB2Parser parser;
	parser.SetArcGISPRJParser(prjParser);
	this->fgdb = (DB::ReadingDB*)parser.ParseObject(pkg, 0, IO::ParserType::ReadingDB);
	
}

Map::HKRoadNetwork2::HKRoadNetwork2(DB::ReadingDB *fgdb)
{
	this->fgdb = fgdb;
}

Map::HKRoadNetwork2::~HKRoadNetwork2()
{
	SDEL_CLASS(this->fgdb);
}

Bool Map::HKRoadNetwork2::IsError()
{
	return this->fgdb == 0;
}

DB::ReadingDB *Map::HKRoadNetwork2::GetDB()
{
	return this->fgdb;
}

NN<Math::CoordinateSystem> Map::HKRoadNetwork2::CreateCoordinateSystem()
{
	NN<Math::CoordinateSystem> csys;
	if (csys.Set(Math::CoordinateSystemManager::CreateProjCoordinateSystemDefName(Math::CoordinateSystemManager::PCST_HK80)))
		return csys;
	else
		return Math::CoordinateSystemManager::CreateDefaultCsys();
}

Map::HKSpeedLimit *Map::HKRoadNetwork2::CreateSpeedLimit()
{
	if (this->fgdb)
	{
		return NEW_CLASS_D(Map::HKSpeedLimit(this));
	}
	return 0;
}

Map::MapDrawLayer *Map::HKRoadNetwork2::CreateTonnesSignLayer()
{
	if (this->fgdb == 0)
	{
		return 0;
	}
	
	Map::DrawLayerType layerType = Map::DRAW_LAYER_POINT;
	const UTF8Char *colNames[] = {(const UTF8Char*)"Id", (const UTF8Char*)"MaxWeight", (const UTF8Char*)"Remarks"};
	DB::DBUtil::ColType colTypes[] = {DB::DBUtil::CT_Int32, DB::DBUtil::CT_Double, DB::DBUtil::CT_VarUTF8Char};
	UOSInt colSize[] = {11, 32, 255};
	UOSInt colDP[] = {0, 10, 0};
	Map::VectorLayer *lyr = 0;
	NEW_CLASS(lyr, Map::VectorLayer(layerType, CSTR("HKRoadNetwork2"), 3, colNames, this->CreateCoordinateSystem(), colTypes, colSize, colDP, 0, CSTR("VehRestrict")));
	
	NN<DB::DBReader> r;
	if (this->fgdb->QueryTableData(CSTR_NULL, CSTR("VEHICLE_RESTRICTION"), 0, 0, 0, CSTR_NULL, 0).SetTo(r))
	{
		UTF8Char sbuff[256];
		UTF8Char *sptr;
		UTF8Char sbuff2[64];
		OSInt shapeCol = -1;
		OSInt vrIdCol = -1;
		OSInt maxWeightCol = -1;
		OSInt remarksCol = -1;
		UOSInt i;
		UOSInt j;
		UTF8Char *strs[3];
		i = 0;
		j = r->ColCount();
		while (i < j)
		{
			if ((sptr = r->GetName(i, sbuff)) != 0)
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
				Double maxWeight = r->GetDbl((UOSInt)maxWeightCol);
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
						lyr->AddVector(vec, (const UTF8Char**)strs);
					}
				}
			}
		}

		this->fgdb->CloseReader(r);
	}
	return lyr;
}

Map::HKTrafficLayer2 *Map::HKRoadNetwork2::CreateTrafficLayer(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Optional<Text::EncodingFactory> encFact)
{
	if (this->fgdb)
	{
		return NEW_CLASS_D(Map::HKTrafficLayer2(sockf, ssl, encFact, this));
	}
	return 0;
}

Text::CStringNN Map::HKRoadNetwork2::GetDownloadURL()
{
	return CSTR("https://static.data.gov.hk/td/road-network-v2/RdNet_IRNP.gdb.zip");
}

Text::CStringNN Map::HKRoadNetwork2::GetDefFileName()
{
	return CSTR("RdNet_IRNP.gdb");
}
