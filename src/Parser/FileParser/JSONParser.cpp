#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListA.h"
#include "Data/ByteTool.h"
#include "DB/JSONDB.h"
#include "Map/DBMapLayer.h"
#include "Map/TileMapLayer.h"
#include "Map/VectorLayer.h"
#include "Map/OSM/OSMLocalTileMap.h"
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
	this->parsers = 0;
}

Parser::FileParser::JSONParser::~JSONParser()
{
}

Int32 Parser::FileParser::JSONParser::GetName()
{
	return *(Int32*)"JSON";
}

void Parser::FileParser::JSONParser::SetParserList(Optional<Parser::ParserList> parsers)
{
	this->parsers = parsers;
}

void Parser::FileParser::JSONParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
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

IO::ParsedObject *Parser::FileParser::JSONParser::ParseFileHdr(NN<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
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

	pobj = ParseJSON(fileJSON, fd->GetFullName(), fd->GetShortName().OrEmpty(), targetType, pkgFile, this->parsers);
	fileJSON->EndUse();
	return pobj;
}

IO::ParsedObject *Parser::FileParser::JSONParser::ParseJSON(Text::JSONBase *fileJSON, NN<Text::String> sourceName, Text::CStringNN layerName, IO::ParserType targetType, Optional<IO::PackageFile> pkgFile, Optional<Parser::ParserList> parsers)
{
	UInt32 srid = 0;
	IO::ParsedObject *pobj = 0;
	if (fileJSON->GetType() == Text::JSONType::Object)
	{
		NN<IO::PackageFile> nnpkgFile;
		NN<Parser::ParserList> nnparsers;
		Text::JSONObject *jobj = (Text::JSONObject*)fileJSON;
		Text::JSONBase *jbase = jobj->GetObjectValue(CSTR("type"));
		if (jbase && jbase->Equals(CSTR("FeatureCollection")))
		{
			Math::CoordinateSystem *csys = 0;
			NN<Math::CoordinateSystem> nncsys;
			Text::JSONBase *crs = jobj->GetObjectValue(CSTR("crs"));
			if (crs && crs->GetType() == Text::JSONType::Object)
			{
				Text::JSONBase *crsProp = ((Text::JSONObject*)crs)->GetObjectValue(CSTR("properties"));
				if (crsProp && crsProp->GetType() == Text::JSONType::Object)
				{
					NN<Text::String> crsName;
					if (((Text::JSONObject*)crsProp)->GetObjectString(CSTR("name")).SetTo(crsName))
					{
						csys = Math::CoordinateSystemManager::CreateFromName(crsName->ToCString());
						if (csys)
						{
							srid = csys->GetSRID();
						}
					}
				}
			}
			if (!nncsys.Set(csys))
			{
				nncsys = Math::CoordinateSystemManager::CreateDefaultCsys();
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
				NN<Math::Geometry::Vector2D> vec;
				if (feature && feature->GetType() == Text::JSONType::Object)
				{
					featType = ((Text::JSONObject*)feature)->GetObjectValue(CSTR("type"));
					featProp = ((Text::JSONObject*)feature)->GetObjectValue(CSTR("properties"));
					featGeom = ((Text::JSONObject*)feature)->GetObjectValue(CSTR("geometry"));
					if (featType && featType->GetType() == Text::JSONType::String && featProp && featProp->GetType() == Text::JSONType::Object && featGeom && featGeom->GetType() == Text::JSONType::Object)
					{
						Data::ArrayListNN<Text::String> colNames;
						((Text::JSONObject*)featProp)->GetObjectNames(colNames);
						colCnt = colNames.GetCount();
						k = 0;
						while (k < colCnt)
						{
							tabCols[k] = colNames.GetItem(k).OrNull();
							tabHdrs[k] = tabCols[k]->v;
							k++;
						}
						if (vec.Set(ParseGeomJSON((Text::JSONObject*)featGeom, srid)))
						{
							NN<Text::String> s = Text::String::New(layerName);
							NEW_CLASS(lyr, Map::VectorLayer(Map::DRAW_LAYER_MIXED, sourceName, colCnt, tabHdrs, nncsys, 0, s.Ptr()));
							s->Release();
							vec.Delete();
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
										tabVals[k] = ((Text::JSONString*)jbase)->GetValue().Ptr();
									}
									else
									{
										tabVals[k] = 0;
									}
									k++;
								}
								if (vec.Set(ParseGeomJSON((Text::JSONObject*)featGeom, srid)))
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
					nncsys.Delete();
				}
			}
			else
			{
				nncsys.Delete();
			}
			return pobj;
		}
		else if (jbase && jbase->Equals(CSTR("overlay")) && layerName.EndsWith(UTF8STRC("metadata.json")) && pkgFile.SetTo(nnpkgFile) && parsers.SetTo(nnparsers))
		{
			NN<Text::String> name;
			NN<Text::String> format;
			NN<Text::String> sMinZoom;
			NN<Text::String> sMaxZoom;
			UInt32 minZoom;
			UInt32 maxZoom;
			Text::JSONArray *bounds = jobj->GetObjectArray(CSTR("bounds"));
			if (jobj->GetObjectString(CSTR("name")).SetTo(name) &&
				jobj->GetObjectString(CSTR("format")).SetTo(format) &&
				jobj->GetObjectString(CSTR("minzoom")).SetTo(sMinZoom) &&
				jobj->GetObjectString(CSTR("maxzoom")).SetTo(sMaxZoom) && bounds && sMinZoom->ToUInt32(minZoom) && sMaxZoom->ToUInt32(maxZoom) && bounds->GetArrayLength() == 4)
			{
				Math::Coord2DDbl maxCoord;
				Math::Coord2DDbl minCoord;
				Math::Coord2DDbl coord;
				coord = Math::Coord2DDbl(bounds->GetArrayDouble(0), bounds->GetArrayDouble(1));
				maxCoord = Math::Coord2DDbl(bounds->GetArrayDouble(2), bounds->GetArrayDouble(3));
				minCoord = coord.Min(maxCoord);
				maxCoord = coord.Max(maxCoord);
				NN<Map::OSM::OSMLocalTileMap> tileMap;
				NN<Map::TileMapLayer> mapLayer;
				NEW_CLASSNN(tileMap, Map::OSM::OSMLocalTileMap(nnpkgFile->Clone(), name, format, minZoom, maxZoom, minCoord, maxCoord));
				NEW_CLASSNN(mapLayer, Map::TileMapLayer(tileMap, nnparsers));
				return mapLayer.Ptr();
			}
		}
	}
	Text::JSONArray *dataArr = GetDataArray(fileJSON);
	if (dataArr == 0)
	{
		return 0;
	}
	NN<DB::JSONDB> db;
	NEW_CLASSNN(db, DB::JSONDB(sourceName, layerName, dataArr));
	if (targetType == IO::ParserType::Unknown || targetType == IO::ParserType::MapLayer)
	{
		Map::DBMapLayer *layer;
		NEW_CLASS(layer, Map::DBMapLayer(sourceName));
		if (layer->SetDatabase(db, CSTR_NULL, layerName, true))
		{
			return layer;
		}
		DEL_CLASS(layer);
	}
	return db.Ptr();
}

Math::Geometry::Vector2D *Parser::FileParser::JSONParser::ParseGeomJSON(Text::JSONObject *obj, UInt32 srid)
{
	NN<Text::String> sType;
	if (obj->GetObjectString(CSTR("type")).SetTo(sType))
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
						altArr = pl->GetZList(i);
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
						ptArr = ptList.GetPtr(i).Ptr();
						Math::Geometry::LineString *pl;
						NEW_CLASS(pl, Math::Geometry::LineString(srid, (Math::Coord2DDbl*)ptArr, i >> 1, 0, 0));
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
					NN<Math::Geometry::LinearRing> lr;
					UOSInt m;
					NEW_CLASS(pg, Math::Geometry::Polygon(srid));
					i = 0;
					j = partList.GetCount();
					k = 0;
					while (i < j)
					{
						i++;
						if (i >= j)
							l = ptList.GetCount() >> 1;
						else
							l = partList.GetItem(i);
						NEW_CLASSNN(lr, Math::Geometry::LinearRing(srid, (l - k), hasAlt, false));
						ptArr = lr->GetPointList(m);
						Double *altArr = lr->GetZList(m);
						m = 0;
						while (k < l)
						{
							ptArr[m].x = ptList.GetItem((k << 1));
							ptArr[m].y = ptList.GetItem((k << 1) + 1);
							if (altArr)
								altArr[m] = altList.GetItem(k);
							k++;
						}
						pg->AddGeometry(lr);
						i++;
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
							NN<Math::Geometry::Polygon> pg;
							Bool hasZ = ptList.GetCount() == altList.GetCount() * 2;
							NN<Math::Geometry::LinearRing> lr;
							UOSInt m;
							NEW_CLASSNN(pg, Math::Geometry::Polygon(srid));
							i = 0;
							j = partList.GetCount();
							k = 0;
							while (i < j)
							{
								i++;
								if (i >= j)
									l = ptList.GetCount() >> 1;
								else
									l = partList.GetItem(i);
								NEW_CLASSNN(lr, Math::Geometry::LinearRing(srid, (l - k), hasZ, false));
								ptArr = lr->GetPointList(m);
								Double *altArr = lr->GetZList(m);
								m = 0;
								while (k < l)
								{
									ptArr[m].x = ptList.GetItem((k << 1));
									ptArr[m].y = ptList.GetItem((k << 1) + 1);
									if (altArr)
										altArr[m] = altList.GetItem(k);
									k++;
									m++;
								}
								pg->AddGeometry(lr);
								i++;
							}
							if (mpg == 0)
							{
								NEW_CLASS(mpg, Math::Geometry::MultiPolygon(srid));
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
	Data::ArrayListNN<Text::String> names;
	obj->GetObjectNames(names);
	UOSInt i = names.GetCount();
	while (i-- > 0)
	{
		o = obj->GetObjectValue(names.GetItemNoCheck(i)->ToCString());
		if (o && o->GetType() == Text::JSONType::Array)
		{
			if (arrayName == 0)
			{
				arrayName = names.GetItem(i).OrNull();
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
