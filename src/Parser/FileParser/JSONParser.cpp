#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListA.h"
#include "Data/ByteTool.h"
#include "DB/JSONDB.h"
#include "IO/SeleniumIDE.h"
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
	if (t == IO::ParserType::Unknown || t == IO::ParserType::SeleniumIDE)
	{
		selector->AddFilter(CSTR("*.side"), CSTR("Selenium IDE File"));
	}
}

IO::ParserType Parser::FileParser::JSONParser::GetParserType()
{
	return IO::ParserType::MapLayer;
}

Optional<IO::ParsedObject> Parser::FileParser::JSONParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	Text::CStringNN fileName = fd->GetShortName().OrEmpty();
	UOSInt fileType = 0;
	if (fileName.EndsWithICase(UTF8STRC(".geojson")) || fileName.EndsWithICase(UTF8STRC(".json")))
	{
		fileType = 1;
	}
	else if (fileName.EndsWithICase(UTF8STRC(".side")))
	{
		fileType = 2;
	}
	if (fileType == 0)
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
	Optional<Text::JSONBase> fileJSON = Text::JSONBase::ParseJSONStr(Text::CStringNN(fileBuff, buffSize));
	MemFree(fileBuff);

	Optional<IO::ParsedObject> pobj = 0;
	NN<Text::JSONBase> nnfileJSON;
	if (!fileJSON.SetTo(nnfileJSON))
	{
		return 0;
	}

	if (fileType == 2 && nnfileJSON->GetType() == Text::JSONType::Object)
	{
		NEW_CLASSOPT(pobj, IO::SeleniumIDE(fd->GetFullName(), NN<Text::JSONObject>::ConvertFrom(nnfileJSON)));
	}
	else
	{
		pobj = ParseGeoJSON(nnfileJSON, fd->GetFullName(), fd->GetShortName().OrEmpty(), targetType, pkgFile, this->parsers);
	}
	nnfileJSON->EndUse();
	return pobj;
}

Optional<IO::ParsedObject> Parser::FileParser::JSONParser::ParseGeoJSON(NN<Text::JSONBase> fileJSON, NN<Text::String> sourceName, Text::CStringNN layerName, IO::ParserType targetType, Optional<IO::PackageFile> pkgFile, Optional<Parser::ParserList> parsers)
{
	UInt32 srid = 0;
	IO::ParsedObject *pobj = 0;
	if (fileJSON->GetType() == Text::JSONType::Object)
	{
		NN<IO::PackageFile> nnpkgFile;
		NN<Parser::ParserList> nnparsers;
		NN<Text::JSONObject> jobj = NN<Text::JSONObject>::ConvertFrom(fileJSON);
		NN<Text::JSONBase> jbase;
		if (!jobj->GetObjectValue(CSTR("type")).SetTo(jbase))
		{
		}
		else if (jbase->Equals(CSTR("FeatureCollection")))
		{
			Optional<Math::CoordinateSystem> csys = 0;
			NN<Math::CoordinateSystem> nncsys;
			NN<Text::JSONBase> crs;
			if (jobj->GetObjectValue(CSTR("crs")).SetTo(crs) && crs->GetType() == Text::JSONType::Object)
			{
				NN<Text::JSONBase> crsProp;
				if (NN<Text::JSONObject>::ConvertFrom(crs)->GetObjectValue(CSTR("properties")).SetTo(crsProp) && crsProp->GetType() == Text::JSONType::Object)
				{
					NN<Text::String> crsName;
					if (NN<Text::JSONObject>::ConvertFrom(crsProp)->GetObjectString(CSTR("name")).SetTo(crsName))
					{
						csys = Math::CoordinateSystemManager::CreateFromName(crsName->ToCString());
						if (csys.SetTo(nncsys))
						{
							srid = nncsys->GetSRID();
						}
					}
				}
			}
			if (!csys.SetTo(nncsys))
			{
				nncsys = Math::CoordinateSystemManager::CreateWGS84Csys();
			}

			if (jobj->GetObjectValue(CSTR("features")).SetTo(jbase) && jbase->GetType() == Text::JSONType::Array)
			{
				Map::VectorLayer *lyr = 0;
				UnsafeArrayOpt<const UTF8Char> tabHdrs[32];
				Text::String *tabCols[32];
				Text::String *tabVals[32];
				UOSInt colCnt;
				NN<Text::JSONArray> features = NN<Text::JSONArray>::ConvertFrom(jbase);
				UOSInt i;
				UOSInt j = features->GetArrayLength();
				UOSInt k;
				NN<Text::JSONBase> feature;
				NN<Text::JSONBase> featType;
				NN<Text::JSONBase> featProp;
				NN<Text::JSONBase> featGeom;
				NN<Math::Geometry::Vector2D> vec;
				if (features->GetArrayValue(0).SetTo(feature) && feature->GetType() == Text::JSONType::Object)
				{
					if (NN<Text::JSONObject>::ConvertFrom(feature)->GetObjectValue(CSTR("type")).SetTo(featType) && featType->GetType() == Text::JSONType::String &&
						NN<Text::JSONObject>::ConvertFrom(feature)->GetObjectValue(CSTR("properties")).SetTo(featProp) && featProp->GetType() == Text::JSONType::Object &&
						NN<Text::JSONObject>::ConvertFrom(feature)->GetObjectValue(CSTR("geometry")).SetTo(featGeom) && featGeom->GetType() == Text::JSONType::Object)
					{
						Data::ArrayListNN<Text::String> colNames;
						NN<Text::JSONObject>::ConvertFrom(featProp)->GetObjectNames(colNames);
						colCnt = colNames.GetCount();
						k = 0;
						while (k < colCnt)
						{
							tabCols[k] = colNames.GetItem(k).OrNull();
							tabHdrs[k] = UnsafeArray<const UTF8Char>(tabCols[k]->v);
							k++;
						}
						if (ParseGeomJSON(NN<Text::JSONObject>::ConvertFrom(featGeom), srid).SetTo(vec))
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
						if (features->GetArrayValue(i).SetTo(feature) && feature->GetType() == Text::JSONType::Object)
						{
							if (NN<Text::JSONObject>::ConvertFrom(feature)->GetObjectValue(CSTR("type")).SetTo(featType) && featType->GetType() == Text::JSONType::String &&
								NN<Text::JSONObject>::ConvertFrom(feature)->GetObjectValue(CSTR("properties")).SetTo(featProp) && featProp->GetType() == Text::JSONType::Object &&
								NN<Text::JSONObject>::ConvertFrom(feature)->GetObjectValue(CSTR("geometry")).SetTo(featGeom))
							{
								k = 0;
								while (k < colCnt)
								{
									if (NN<Text::JSONObject>::ConvertFrom(featProp)->GetObjectValue(tabCols[k]->ToCString()).SetTo(jbase) && jbase->GetType() == Text::JSONType::String)
									{
										tabVals[k] = NN<Text::JSONString>::ConvertFrom(jbase)->GetValue().Ptr();
									}
									else
									{
										tabVals[k] = 0;
									}
									k++;
								}
								if (ParseGeomJSON(NN<Text::JSONObject>::ConvertFrom(featGeom), srid).SetTo(vec))
								{
									lyr->AddVector2(vec, tabVals);
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
		else if (jbase->Equals(CSTR("overlay")) && layerName.EndsWith(UTF8STRC("metadata.json")) && pkgFile.SetTo(nnpkgFile) && parsers.SetTo(nnparsers))
		{
			NN<Text::String> name;
			NN<Text::String> format;
			NN<Text::String> sMinZoom;
			NN<Text::String> sMaxZoom;
			UInt32 minZoom;
			UInt32 maxZoom;
			NN<Text::JSONArray> bounds;
			if (jobj->GetObjectString(CSTR("name")).SetTo(name) &&
				jobj->GetObjectString(CSTR("format")).SetTo(format) &&
				jobj->GetObjectString(CSTR("minzoom")).SetTo(sMinZoom) &&
				jobj->GetObjectString(CSTR("maxzoom")).SetTo(sMaxZoom) &&
				jobj->GetObjectArray(CSTR("bounds")).SetTo(bounds) && sMinZoom->ToUInt32(minZoom) && sMaxZoom->ToUInt32(maxZoom) && bounds->GetArrayLength() == 4)
			{
				Math::Coord2DDbl maxCoord;
				Math::Coord2DDbl minCoord;
				Math::Coord2DDbl coord;
				coord = Math::Coord2DDbl(bounds->GetArrayDoubleOrNAN(0), bounds->GetArrayDoubleOrNAN(1));
				maxCoord = Math::Coord2DDbl(bounds->GetArrayDoubleOrNAN(2), bounds->GetArrayDoubleOrNAN(3));
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
	NN<Text::JSONArray> dataArr;
	if (!GetDataArray(fileJSON).SetTo(dataArr))
	{
		return 0;
	}
	NN<DB::JSONDB> db;
	NEW_CLASSNN(db, DB::JSONDB(sourceName, layerName, dataArr));
	if (targetType == IO::ParserType::Unknown || targetType == IO::ParserType::MapLayer)
	{
		Map::DBMapLayer *layer;
		NEW_CLASS(layer, Map::DBMapLayer(sourceName));
		if (layer->SetDatabase(db, nullptr, layerName, true))
		{
			return layer;
		}
		DEL_CLASS(layer);
	}
	return db;
}

Optional<Math::Geometry::Vector2D> Parser::FileParser::JSONParser::ParseGeomJSON(NN<Text::JSONObject> obj, UInt32 srid)
{
	NN<Text::String> sType;
	if (obj->GetObjectString(CSTR("type")).SetTo(sType))
	{
		NN<Text::JSONBase> jbase;
		if (sType->Equals(UTF8STRC("LineString")))
		{
			if (obj->GetObjectValue(CSTR("coordinates")).SetTo(jbase) && jbase->GetType() == Text::JSONType::Array)
			{
				NN<Text::JSONArray> coord = NN<Text::JSONArray>::ConvertFrom(jbase);
				Data::ArrayListA<Double> ptList;
				Data::ArrayList<Double> zList;
				Bool hasZ = false;
				NN<Text::JSONArray> pt;
				UOSInt i = 0;
				UOSInt j = coord->GetArrayLength();
				while (i < j)
				{
					if (coord->GetArrayValue(i).SetTo(jbase) && jbase->GetType() == Text::JSONType::Array)
					{
						pt = NN<Text::JSONArray>::ConvertFrom(jbase);
						if (pt->GetArrayLength() == 3)
						{
							hasZ = true;
							if (pt->GetArrayValue(0).SetTo(jbase) && jbase->GetType() == Text::JSONType::Number)
							{
								ptList.Add(NN<Text::JSONNumber>::ConvertFrom(jbase)->GetValue());
							}
							else
							{
								ptList.Add(0);
							}
							if (pt->GetArrayValue(1).SetTo(jbase) && jbase->GetType() == Text::JSONType::Number)
							{
								ptList.Add(NN<Text::JSONNumber>::ConvertFrom(jbase)->GetValue());
							}
							else
							{
								ptList.Add(0);
							}
							if (pt->GetArrayValue(2).SetTo(jbase) && jbase->GetType() == Text::JSONType::Number)
							{
								zList.Add(NN<Text::JSONNumber>::ConvertFrom(jbase)->GetValue());
							}
							else
							{
								zList.Add(0);
							}
						}
						else if (pt->GetArrayLength() == 2)
						{
							if (pt->GetArrayValue(0).SetTo(jbase) && jbase->GetType() == Text::JSONType::Number)
							{
								ptList.Add(NN<Text::JSONNumber>::ConvertFrom(jbase)->GetValue());
							}
							else
							{
								ptList.Add(0);
							}
							if (pt->GetArrayValue(1).SetTo(jbase) && jbase->GetType() == Text::JSONType::Number)
							{
								ptList.Add(NN<Text::JSONNumber>::ConvertFrom(jbase)->GetValue());
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
						UnsafeArray<Math::Coord2DDbl> ptArr;
						UnsafeArray<Double> altArr;
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
						if (pl->GetZList(i).SetTo(altArr))
						{
							i = 0;
							j = zList.GetCount();
							while (i < j)
							{
								altArr[i] = zList.GetItem(i);
								i++;
							}
						}
						return pl;
					}
					else
					{
						Double *ptArr;
						ptArr = ptList.GetArr(i).Ptr();
						Math::Geometry::LineString *pl;
						NEW_CLASS(pl, Math::Geometry::LineString(srid, (Math::Coord2DDbl*)ptArr, i >> 1, 0, 0));
						return pl;
					}
				}
			}
		}
		else if (sType->Equals(UTF8STRC("Polygon")))
		{
			if (obj->GetObjectValue(CSTR("coordinates")).SetTo(jbase) && jbase->GetType() == Text::JSONType::Array)
			{
				NN<Text::JSONArray> coord = NN<Text::JSONArray>::ConvertFrom(jbase);
				Data::ArrayList<Double> ptList;
				Data::ArrayList<Double> altList;
				Data::ArrayList<UInt32> partList;
				Bool hasData = false;
				Bool hasAlt = false;
				NN<Text::JSONArray> ptArr;
				NN<Text::JSONArray> pt;
				UOSInt i = 0;
				UOSInt j = coord->GetArrayLength();
				UOSInt k;
				UOSInt l;
				UOSInt arrLen;
				while (i < j)
				{
					if (coord->GetArrayValue(i).SetTo(jbase) && jbase->GetType() == Text::JSONType::Array)
					{
						ptArr = NN<Text::JSONArray>::ConvertFrom(jbase);
						hasData = false;
						k = 0;
						l = ptArr->GetArrayLength();
						while (k < l)
						{
							if (ptArr->GetArrayValue(k).SetTo(jbase) && jbase->GetType() == Text::JSONType::Array)
							{
								pt = NN<Text::JSONArray>::ConvertFrom(jbase);
								arrLen = pt->GetArrayLength();
								if (arrLen >= 2)
								{
									if (!hasData)
									{
										hasData = true;
										partList.Add((UInt32)ptList.GetCount() >> 1);
									}
									if (pt->GetArrayValue(0).SetTo(jbase) && jbase->GetType() == Text::JSONType::Number)
									{
										ptList.Add(NN<Text::JSONNumber>::ConvertFrom(jbase)->GetValue());
									}
									else
									{
										ptList.Add(0);
									}
									if (pt->GetArrayValue(1).SetTo(jbase) && jbase->GetType() == Text::JSONType::Number)
									{
										ptList.Add(NN<Text::JSONNumber>::ConvertFrom(jbase)->GetValue());
									}
									else
									{
										ptList.Add(0);
									}

									if (arrLen >= 3 && pt->GetArrayValue(2).SetTo(jbase) && jbase->GetType() == Text::JSONType::Number)
									{
										altList.Add(NN<Text::JSONNumber>::ConvertFrom(jbase)->GetValue());
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
					UnsafeArray<Math::Coord2DDbl> ptArr;
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
						UnsafeArrayOpt<Double> altArr = lr->GetZList(m);
						UnsafeArray<Double> nnaltArr;
						m = 0;
						while (k < l)
						{
							ptArr[m].x = ptList.GetItem((k << 1));
							ptArr[m].y = ptList.GetItem((k << 1) + 1);
							if (altArr.SetTo(nnaltArr))
								nnaltArr[m] = altList.GetItem(k);
							k++;
							m++;
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
			if (obj->GetObjectValue(CSTR("coordinates")).SetTo(jbase) && jbase->GetType() == Text::JSONType::Array)
			{
				Math::Geometry::MultiPolygon *mpg = 0;
				NN<Text::JSONArray> pgCoords = NN<Text::JSONArray>::ConvertFrom(jbase);
				UOSInt pgIndex = 0;
				UOSInt pgCnt = pgCoords->GetArrayLength();
				while (pgIndex < pgCnt)
				{
					if (pgCoords->GetArrayValue(pgIndex).SetTo(jbase) && jbase->GetType() == Text::JSONType::Array)
					{
						NN<Text::JSONArray> coord = NN<Text::JSONArray>::ConvertFrom(jbase);
						Data::ArrayList<Double> ptList;
						Data::ArrayList<Double> altList;
						Data::ArrayList<UInt32> partList;
						Bool hasData = false;
						NN<Text::JSONArray> ptArr;
						NN<Text::JSONArray> pt;
						UOSInt i = 0;
						UOSInt j = coord->GetArrayLength();
						UOSInt k;
						UOSInt l;
						UOSInt nVal;
						while (i < j)
						{
							if (coord->GetArrayValue(i).SetTo(jbase) && jbase->GetType() == Text::JSONType::Array)
							{
								ptArr = NN<Text::JSONArray>::ConvertFrom(jbase);
								hasData = false;
								k = 0;
								l = ptArr->GetArrayLength();
								while (k < l)
								{
									if (ptArr->GetArrayValue(k).SetTo(jbase) && jbase->GetType() == Text::JSONType::Array)
									{
										pt = NN<Text::JSONArray>::ConvertFrom(jbase);
										nVal = pt->GetArrayLength();
										if (nVal >= 2)
										{
											if (!hasData)
											{
												hasData = true;
												partList.Add((UInt32)ptList.GetCount() >> 1);
											}
											ptList.Add(pt->GetArrayDoubleOrNAN(0));
											ptList.Add(pt->GetArrayDoubleOrNAN(1));
											if (nVal >= 3)
											{
												altList.Add(pt->GetArrayDoubleOrNAN(2));
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
							UnsafeArray<Math::Coord2DDbl> ptArr;
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
								UnsafeArrayOpt<Double> altArr = lr->GetZList(m);
								UnsafeArray<Double> nnaltArr;
								m = 0;
								while (k < l)
								{
									ptArr[m].x = ptList.GetItem((k << 1));
									ptArr[m].y = ptList.GetItem((k << 1) + 1);
									if (altArr.SetTo(nnaltArr))
										nnaltArr[m] = altList.GetItem(k);
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
			if (obj->GetObjectValue(CSTR("coordinates")).SetTo(jbase) && jbase->GetType() == Text::JSONType::Array)
			{
				NN<Text::JSONArray> coord = NN<Text::JSONArray>::ConvertFrom(jbase);
				Math::Geometry::Point *pt = 0;
				if (coord->GetArrayLength() == 2)
				{
					NEW_CLASS(pt, Math::Geometry::Point(srid, Math::Coord2DDbl(coord->GetArrayDoubleOrNAN(0), coord->GetArrayDoubleOrNAN(1))));
				}
				else if (coord->GetArrayLength() >= 3)
				{
					NEW_CLASS(pt, Math::Geometry::PointZ(srid, coord->GetArrayDoubleOrNAN(0), coord->GetArrayDoubleOrNAN(1), coord->GetArrayDoubleOrNAN(2)));
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
			printf("JSONParser: GeoJSON unknown type: %s\r\n", sType->v.Ptr());
		}
	}
	return 0;
}

Optional<Text::JSONArray> Parser::FileParser::JSONParser::GetDataArray(NN<Text::JSONBase> fileJSON)
{
	Text::JSONType type = fileJSON->GetType();
	if (type == Text::JSONType::Array)
	{
		return NN<Text::JSONArray>::ConvertFrom(fileJSON);
	}
	else if (type != Text::JSONType::Object)
	{
		return 0;
	}
	Text::String *arrayName = 0;
	NN<Text::JSONObject> obj = NN<Text::JSONObject>::ConvertFrom(fileJSON);
	NN<Text::JSONBase> o;
	Data::ArrayListNN<Text::String> names;
	obj->GetObjectNames(names);
	UOSInt i = names.GetCount();
	while (i-- > 0)
	{
		if (obj->GetObjectValue(names.GetItemNoCheck(i)->ToCString()).SetTo(o) && o->GetType() == Text::JSONType::Array)
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
	return Optional<Text::JSONArray>::ConvertFrom(obj->GetObjectValue(arrayName->ToCString()));
}
