#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListA.h"
#include "Data/ByteTool.h"
#include "DB/JSONDB.h"
#include "Map/DBMapLayer.h"
#include "Map/VectorLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Geometry/LineString.h"
#include "Math/Geometry/MultiPolygon.h"
#include "Math/Geometry/PointZ.h"
#include "Media/ImageList.h"
#include "Media/StaticImage.h"
#include "Parser/FileParser/JSONParser.h"
#include "Text/Encoding.h"
#include "Text/JSON.h"

#include <stdio.h>

Parser::FileParser::JSONParser::JSONParser()
{
}

Parser::FileParser::JSONParser::~JSONParser()
{
}

Int32 Parser::FileParser::JSONParser::GetName()
{
	return *(Int32*)"JSON";
}

void Parser::FileParser::JSONParser::PrepareSelector(IO::FileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MapLayer)
	{
		selector->AddFilter(CSTR("*.GeoJSON"), CSTR("GeoJSON File"));
	}
}

IO::ParserType Parser::FileParser::JSONParser::GetParserType()
{
	return IO::ParserType::MapLayer;
}

IO::ParsedObject *Parser::FileParser::JSONParser::ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	Text::CString fileName = fd->GetShortName();
	Bool valid = false;
	if (fileName.EndsWithICase(UTF8STRC(".geojson")) || fileName.EndsWithICase(UTF8STRC(".json")))
	{
		valid = true;
	}
	if (!valid)
		return 0;
	UInt64 fileOfst;
	if (hdr[0] == '{' || hdr[0] == '[')
	{
		fileOfst = 0;
	}
	else if (hdr[0] == 0xEF && hdr[1] == 0xBB && hdr[2] == 0xBF && (hdr[3] == '{' || hdr[3] == '['))
	{
		fileOfst = 3;
	}
	else
	{
		return 0;
	}
	UOSInt buffSize = (UOSInt)(fd->GetDataSize() - fileOfst);
	UInt8 *fileBuff = MemAlloc(UInt8, buffSize + 1);
	fileBuff[fd->GetRealData(fileOfst, buffSize, Data::ByteArray(fileBuff, buffSize + 1))] = 0;
	Text::JSONBase *fileJSON = Text::JSONBase::ParseJSONStr(Text::CStringNN(fileBuff, buffSize));
	MemFree(fileBuff);

	IO::ParsedObject *pobj = 0;
	if (fileJSON == 0)
	{
		return 0;
	}

	pobj = ParseJSON(fileJSON, fd->GetFullName(), fd->GetShortName(), targetType);
	fileJSON->EndUse();
	return pobj;
}

IO::ParsedObject *Parser::FileParser::JSONParser::ParseJSON(Text::JSONBase *fileJSON, NotNullPtr<Text::String> sourceName, Text::CString layerName, IO::ParserType targetType)
{
	UInt32 srid = 0;
	IO::ParsedObject *pobj = 0;
	if (fileJSON->GetType() == Text::JSONType::Object)
	{
		Text::JSONObject *jobj = (Text::JSONObject*)fileJSON;
		Text::JSONBase *jbase = jobj->GetObjectValue(CSTR("type"));
		if (jbase && jbase->Equals(CSTR("FeatureCollection")))
		{
			Math::CoordinateSystem *csys = 0;
			Text::JSONBase *crs = jobj->GetObjectValue(CSTR("crs"));
			if (crs && crs->GetType() == Text::JSONType::Object)
			{
				Text::JSONBase *crsProp = ((Text::JSONObject*)crs)->GetObjectValue(CSTR("properties"));
				if (crsProp && crsProp->GetType() == Text::JSONType::Object)
				{
					Text::String *crsName = ((Text::JSONObject*)crsProp)->GetObjectString(CSTR("name"));
					if (crsName)
					{
						csys = Math::CoordinateSystemManager::CreateFromName(crsName->ToCString());
						if (csys)
						{
							srid = csys->GetSRID();
						}
					}
				}
			}

			jbase = jobj->GetObjectValue(CSTR("features"));
			if (jbase && jbase->GetType() == Text::JSONType::Array)
			{
				Map::VectorLayer *lyr = 0;
				const UTF8Char *tabHdrs[32];
				Text::String *tabCols[32];
				Text::String *tabVals[32];
				UOSInt colCnt;
				Text::JSONArray *features = (Text::JSONArray*)jbase;
				UOSInt i;
				UOSInt j = features->GetArrayLength();
				UOSInt k;
				Text::JSONBase *feature = features->GetArrayValue(0);
				Text::JSONBase *featType;
				Text::JSONBase *featProp;
				Text::JSONBase *featGeom;
				Math::Geometry::Vector2D *vec;
				if (feature && feature->GetType() == Text::JSONType::Object)
				{
					featType = ((Text::JSONObject*)feature)->GetObjectValue(CSTR("type"));
					featProp = ((Text::JSONObject*)feature)->GetObjectValue(CSTR("properties"));
					featGeom = ((Text::JSONObject*)feature)->GetObjectValue(CSTR("geometry"));
					if (featType && featType->GetType() == Text::JSONType::String && featProp && featProp->GetType() == Text::JSONType::Object && featGeom && featGeom->GetType() == Text::JSONType::Object)
					{
						Data::ArrayList<Text::String *> colNames;
						((Text::JSONObject*)featProp)->GetObjectNames(&colNames);
						colCnt = colNames.GetCount();
						k = 0;
						while (k < colCnt)
						{
							tabCols[k] = colNames.GetItem(k);
							tabHdrs[k] = tabCols[k]->v;
							k++;
						}
						vec = ParseGeomJSON((Text::JSONObject*)featGeom, srid);
						if (vec)
						{
							NotNullPtr<Text::String> s = Text::String::New(layerName);
							NEW_CLASS(lyr, Map::VectorLayer(Map::DRAW_LAYER_MIXED, sourceName, colCnt, tabHdrs, csys, 0, s.Ptr()));
							s->Release();
							DEL_CLASS(vec);
						}
					}
				}

				if (lyr)
				{
					i = 0;
					while (i < j)
					{
						feature = features->GetArrayValue(i);
						if (feature && feature->GetType() == Text::JSONType::Object)
						{
							featType = ((Text::JSONObject*)feature)->GetObjectValue(CSTR("type"));
							featProp = ((Text::JSONObject*)feature)->GetObjectValue(CSTR("properties"));
							featGeom = ((Text::JSONObject*)feature)->GetObjectValue(CSTR("geometry"));
							if (featType && featType->GetType() == Text::JSONType::String && featProp && featProp->GetType() == Text::JSONType::Object && featGeom)
							{
								k = 0;
								while (k < colCnt)
								{
									jbase = ((Text::JSONObject*)featProp)->GetObjectValue(tabCols[k]->ToCString());
									if (jbase && jbase->GetType() == Text::JSONType::String)
									{
										tabVals[k] = ((Text::JSONString*)jbase)->GetValue();
									}
									else
									{
										tabVals[k] = 0;
									}
									k++;
								}
								vec = ParseGeomJSON((Text::JSONObject*)featGeom, srid);
								if (vec)
								{
									lyr->AddVector(vec, tabVals);
								}
							}
						}

						i++;
					}
					pobj = lyr;
				}
				else
				{
					SDEL_CLASS(csys);
				}
			}
			else
			{
				SDEL_CLASS(csys);
			}
			return pobj;
		}
	}
	Text::JSONArray *dataArr = GetDataArray(fileJSON);
	if (dataArr == 0)
	{
		return 0;
	}
	DB::JSONDB *db;
	NEW_CLASS(db, DB::JSONDB(sourceName, layerName, dataArr));
	if (targetType == IO::ParserType::Unknown || targetType == IO::ParserType::MapLayer)
	{
		Map::DBMapLayer *layer;
		NEW_CLASS(layer, Map::DBMapLayer(sourceName));
		if (layer->SetDatabase(db, CSTR_NULL, layerName, true))
		{
			return layer;
		}

	}
	return db;
}

Math::Geometry::Vector2D *Parser::FileParser::JSONParser::ParseGeomJSON(Text::JSONObject *obj, UInt32 srid)
{
	Text::String *sType = obj->GetObjectString(CSTR("type"));
	if (sType)
	{
		Text::JSONBase *jbase;
		if (sType->Equals(UTF8STRC("LineString")))
		{
			jbase = obj->GetObjectValue(CSTR("coordinates"));
			if (jbase && jbase->GetType() == Text::JSONType::Array)
			{
				Text::JSONArray *coord = (Text::JSONArray*)jbase;
				Data::ArrayListA<Double> ptList;
				Data::ArrayList<Double> zList;
				Bool hasZ = false;
				Text::JSONArray *pt;
				UOSInt i = 0;
				UOSInt j = coord->GetArrayLength();
				while (i < j)
				{
					jbase = coord->GetArrayValue(i);
					if (jbase && jbase->GetType() == Text::JSONType::Array)
					{
						pt = (Text::JSONArray*)jbase;
						if (pt->GetArrayLength() == 3)
						{
							hasZ = true;
							jbase = pt->GetArrayValue(0);
							if (jbase && jbase->GetType() == Text::JSONType::Number)
							{
								ptList.Add(((Text::JSONNumber*)jbase)->GetValue());
							}
							else
							{
								ptList.Add(0);
							}
							jbase = pt->GetArrayValue(1);
							if (jbase && jbase->GetType() == Text::JSONType::Number)
							{
								ptList.Add(((Text::JSONNumber*)jbase)->GetValue());
							}
							else
							{
								ptList.Add(0);
							}
							jbase = pt->GetArrayValue(2);
							if (jbase && jbase->GetType() == Text::JSONType::Number)
							{
								zList.Add(((Text::JSONNumber*)jbase)->GetValue());
							}
							else
							{
								zList.Add(0);
							}
						}
						else if (pt->GetArrayLength() == 2)
						{
							jbase = pt->GetArrayValue(0);
							if (jbase && jbase->GetType() == Text::JSONType::Number)
							{
								ptList.Add(((Text::JSONNumber*)jbase)->GetValue());
							}
							else
							{
								ptList.Add(0);
							}
							jbase = pt->GetArrayValue(1);
							if (jbase && jbase->GetType() == Text::JSONType::Number)
							{
								ptList.Add(((Text::JSONNumber*)jbase)->GetValue());
							}
							else
							{
								ptList.Add(0);
							}
							zList.Add(0);
						}
					}
					i++;
				}
				if (ptList.GetCount() >= 4)
				{
					if (hasZ)
					{
						Math::Geometry::LineString *pl;
						Math::Coord2DDbl *ptArr;
						Double *altArr;
						NEW_CLASS(pl, Math::Geometry::LineString(srid, zList.GetCount(), true, false));
						ptArr = pl->GetPointList(i);
						i = 0;
						j = ptList.GetCount() >> 1;
						while (i < j)
						{
							ptArr[i].x = ptList.GetItem(i << 1);
							ptArr[i].y = ptList.GetItem((i << 1) + 1);
							i++;
						}
						altArr = pl->GetZList(&i);
						i = 0;
						j = zList.GetCount();
						while (i < j)
						{
							altArr[i] = zList.GetItem(i);
							i++;
						}
						return pl;
					}
					else
					{
						Double *ptArr;
						ptArr = ptList.GetPtr(i);
						Math::Geometry::LineString *pl;
						NEW_CLASS(pl, Math::Geometry::LineString(srid, (Math::Coord2DDbl*)ptArr, i >> 1, false, false));
						return pl;
					}
				}
			}
		}
		else if (sType->Equals(UTF8STRC("Polygon")))
		{
			jbase = obj->GetObjectValue(CSTR("coordinates"));
			if (jbase && jbase->GetType() == Text::JSONType::Array)
			{
				Text::JSONArray *coord = (Text::JSONArray*)jbase;
				Data::ArrayList<Double> ptList;
				Data::ArrayList<Double> altList;
				Data::ArrayList<UInt32> partList;
				Bool hasData = false;
				Bool hasAlt = false;
				Text::JSONArray *ptArr;
				Text::JSONArray *pt;
				UOSInt i = 0;
				UOSInt j = coord->GetArrayLength();
				UOSInt k;
				UOSInt l;
				UOSInt arrLen;
				while (i < j)
				{
					jbase = coord->GetArrayValue(i);
					if (jbase && jbase->GetType() == Text::JSONType::Array)
					{
						ptArr = (Text::JSONArray*)jbase;
						hasData = false;
						k = 0;
						l = ptArr->GetArrayLength();
						while (k < l)
						{
							jbase = ptArr->GetArrayValue(k);
							if (jbase && jbase->GetType() == Text::JSONType::Array)
							{
								pt = (Text::JSONArray*)jbase;
								arrLen = pt->GetArrayLength();
								if (arrLen >= 2)
								{
									if (!hasData)
									{
										hasData = true;
										partList.Add((UInt32)ptList.GetCount() >> 1);
									}
									jbase = pt->GetArrayValue(0);
									if (jbase && jbase->GetType() == Text::JSONType::Number)
									{
										ptList.Add(((Text::JSONNumber*)jbase)->GetValue());
									}
									else
									{
										ptList.Add(0);
									}
									jbase = pt->GetArrayValue(1);
									if (jbase && jbase->GetType() == Text::JSONType::Number)
									{
										ptList.Add(((Text::JSONNumber*)jbase)->GetValue());
									}
									else
									{
										ptList.Add(0);
									}

									if (arrLen >= 3 && (jbase = pt->GetArrayValue(2)) != 0 && jbase->GetType() == Text::JSONType::Number)
									{
										altList.Add(((Text::JSONNumber*)jbase)->GetValue());
										hasAlt = true;
									}
									else
									{
										altList.Add(0.0);
									}
								}
							}
							k++;
						}
					}
					i++;
				}
				if (ptList.GetCount() >= 4)
				{
					Math::Coord2DDbl *ptArr;
					Math::Geometry::Polygon *pg;
					NEW_CLASS(pg, Math::Geometry::Polygon(srid, partList.GetCount(), ptList.GetCount() >> 1, hasAlt, false));
					UInt32 *ptOfsts = pg->GetPtOfstList(j);
					i = 0;
					while (i < j)
					{
						ptOfsts[i] = partList.GetItem(i);
						i++;
					}
					ptArr = pg->GetPointList(j);
					i = 0;
					while (i < j)
					{
						ptArr[i].x = ptList.GetItem((i << 1));
						ptArr[i].y = ptList.GetItem((i << 1) + 1);
						i++;
					}
					if (hasAlt)
					{
						Double *altArr = pg->GetZList(&j);
						i = 0;
						while (i < j)
						{
							altArr[i] = altList.GetItem(i);
							i++;
						}
					}
					return pg;
				}
			}
		}
		else if (sType->Equals(UTF8STRC("MultiPolygon")))
		{
			jbase = obj->GetObjectValue(CSTR("coordinates"));
			if (jbase && jbase->GetType() == Text::JSONType::Array)
			{
				Math::Geometry::MultiPolygon *mpg = 0;
				Text::JSONArray *pgCoords = (Text::JSONArray*)jbase;
				UOSInt pgIndex = 0;
				UOSInt pgCnt = pgCoords->GetArrayLength();
				while (pgIndex < pgCnt)
				{
					jbase = pgCoords->GetArrayValue(pgIndex);
					if (jbase && jbase->GetType() == Text::JSONType::Array)
					{
						Text::JSONArray *coord = (Text::JSONArray*)jbase;
						Data::ArrayList<Double> ptList;
						Data::ArrayList<Double> altList;
						Data::ArrayList<UInt32> partList;
						Bool hasData = false;
						Text::JSONArray *ptArr;
						Text::JSONArray *pt;
						UOSInt i = 0;
						UOSInt j = coord->GetArrayLength();
						UOSInt k;
						UOSInt l;
						UOSInt nVal;
						while (i < j)
						{
							jbase = coord->GetArrayValue(i);
							if (jbase && jbase->GetType() == Text::JSONType::Array)
							{
								ptArr = (Text::JSONArray*)jbase;
								hasData = false;
								k = 0;
								l = ptArr->GetArrayLength();
								while (k < l)
								{
									jbase = ptArr->GetArrayValue(k);
									if (jbase && jbase->GetType() == Text::JSONType::Array)
									{
										pt = (Text::JSONArray*)jbase;
										nVal = pt->GetArrayLength();
										if (nVal >= 2)
										{
											if (!hasData)
											{
												hasData = true;
												partList.Add((UInt32)ptList.GetCount() >> 1);
											}
											ptList.Add(pt->GetArrayDouble(0));
											ptList.Add(pt->GetArrayDouble(1));
											if (nVal >= 3)
											{
												altList.Add(pt->GetArrayDouble(2));
											}
										}
									}
									k++;
								}
							}
							i++;
						}
						if (ptList.GetCount() >= 4)
						{
							Math::Coord2DDbl *ptArr;
							Math::Geometry::Polygon *pg;
							Bool hasZ = ptList.GetCount() == altList.GetCount() * 2;
							NEW_CLASS(pg, Math::Geometry::Polygon(srid, partList.GetCount(), ptList.GetCount() >> 1, hasZ, false));
							UInt32 *ptOfsts = pg->GetPtOfstList(j);
							i = 0;
							while (i < j)
							{
								ptOfsts[i] = partList.GetItem(i);
								i++;
							}
							ptArr = pg->GetPointList(j);
							i = 0;
							while (i < j)
							{
								ptArr[i].x = ptList.GetItem((i << 1));
								ptArr[i].y = ptList.GetItem((i << 1) + 1);
								i++;
							}
							if (hasZ)
							{
								Double *altArr = pg->GetZList(&j);
								i = 0;
								while (i < j)
								{
									altArr[i] = altList.GetItem(i);
									i++;
								}
							}
							if (mpg == 0)
							{
								NEW_CLASS(mpg, Math::Geometry::MultiPolygon(srid, hasZ, false));
							}
							mpg->AddGeometry(pg);
						}
					}
					pgIndex++;
				}
				return mpg;
			}
		}
		else if (sType->Equals(UTF8STRC("Point")))
		{
			jbase = obj->GetObjectValue(CSTR("coordinates"));
			if (jbase && jbase->GetType() == Text::JSONType::Array)
			{
				Text::JSONArray *coord = (Text::JSONArray*)jbase;
				Math::Geometry::Point *pt = 0;
				if (coord->GetArrayLength() == 2)
				{
					NEW_CLASS(pt, Math::Geometry::Point(srid, Math::Coord2DDbl(coord->GetArrayDouble(0), coord->GetArrayDouble(1))));
				}
				else if (coord->GetArrayLength() >= 3)
				{
					NEW_CLASS(pt, Math::Geometry::PointZ(srid, coord->GetArrayDouble(0), coord->GetArrayDouble(1), coord->GetArrayDouble(2)));
				}
				else
				{
					printf("JSONParser: GeoJSON Point unknown coordinates:: %d\r\n", (UInt32)coord->GetArrayLength());
				}
				return pt;
			}
			else
			{
				printf("JSONParser: GeoJSON Point unknown type\r\n");
			}
		}
		else
		{
			printf("JSONParser: GeoJSON unknown type: %s\r\n", sType->v);
		}
	}
	return 0;
}

Text::JSONArray *Parser::FileParser::JSONParser::GetDataArray(Text::JSONBase *fileJSON)
{
	Text::JSONType type = fileJSON->GetType();
	if (type == Text::JSONType::Array)
	{
		return (Text::JSONArray*)fileJSON;
	}
	else if (type != Text::JSONType::Object)
	{
		return 0;
	}
	Text::String *arrayName = 0;
	Text::JSONObject *obj = (Text::JSONObject*)fileJSON;
	Text::JSONBase *o;
	Data::ArrayList<Text::String*> names;
	obj->GetObjectNames(&names);
	UOSInt i = names.GetCount();
	while (i-- > 0)
	{
		o = obj->GetObjectValue(names.GetItem(i)->ToCString());
		if (o && o->GetType() == Text::JSONType::Array)
		{
			if (arrayName == 0)
			{
				arrayName = names.GetItem(i);
			}
			else
			{
				return 0;
			}
		}
	}
	if (arrayName == 0)
	{
		return 0;
	}
	return (Text::JSONArray*)obj->GetObjectValue(arrayName->ToCString());
}
