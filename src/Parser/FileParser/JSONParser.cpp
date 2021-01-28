#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Map/VectorLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Media/ImageList.h"
#include "Media/StaticImage.h"
#include "Parser/FileParser/JSONParser.h"
#include "Text/Encoding.h"
#include "Text/JSON.h"

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

void Parser::FileParser::JSONParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_MAP_LAYER_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.GeoJSON", (const UTF8Char*)"GeoJSON File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::JSONParser::GetParserType()
{
	return IO::ParsedObject::PT_MAP_LAYER_PARSER;
}

IO::ParsedObject *Parser::FileParser::JSONParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt8 buff[32];
	const UTF8Char *fileName = fd->GetShortName();
	Bool valid = false;
	if (Text::StrEndsWithICase(fileName, (const UTF8Char*)".geojson"))
	{
		valid = true;
	}
	if (!valid)
		return 0;
	if (fd->GetRealData(0, 32, buff) != 32)
		return 0;

	if (buff[0] != '{')
	{
		return 0;
	}
	OSInt buffSize = (OSInt)fd->GetDataSize();
	UInt8 *fileBuff = MemAlloc(UInt8, buffSize + 1);
	fileBuff[fd->GetRealData(0, (OSInt)fd->GetDataSize(), fileBuff)] = 0;
	Text::JSONBase *fileJSON = Text::JSONBase::ParseJSONStr(fileBuff);
	MemFree(fileBuff);

	Int32 srid = 0;
	IO::ParsedObject *pobj = 0;
	if (fileJSON == 0)
	{
		return 0;
	}

	if (fileJSON->GetJSType() == Text::JSONBase::JST_OBJECT)
	{
		Text::JSONObject *jobj = (Text::JSONObject*)fileJSON;
		Text::JSONBase *jbase = jobj->GetObjectValue((const UTF8Char*)"type");
		if (jbase && jbase->Equals((const UTF8Char*)"FeatureCollection"))
		{
			Math::CoordinateSystem *csys = 0;
			Text::JSONBase *crs = jobj->GetObjectValue((const UTF8Char*)"crs");
			if (crs && crs->GetJSType() == Text::JSONBase::JST_OBJECT)
			{
				Text::JSONBase *crsProp = ((Text::JSONObject*)crs)->GetObjectValue((const UTF8Char*)"properties");
				if (crsProp && crsProp->GetJSType() == Text::JSONBase::JST_OBJECT)
				{
					Text::JSONBase *crsName = ((Text::JSONObject*)crsProp)->GetObjectValue((const UTF8Char*)"name");
					if (crsName)
					{
						if (crsName->GetJSType() == Text::JSONBase::JST_STRINGUTF8)
						{
							csys = Math::CoordinateSystemManager::CreateFromName(((Text::JSONStringUTF8*)crsName)->GetValue());
							if (csys)
							{
								srid = csys->GetSRID();
							}
						}
					}
				}
			}

			jbase = jobj->GetObjectValue((const UTF8Char*)"features");
			if (jbase && jbase->GetJSType() == Text::JSONBase::JST_ARRAY)
			{
				Map::VectorLayer *lyr = 0;
				const UTF8Char *tabHdrs[10];
				const UTF8Char *tabCols[10];
				OSInt colCnt;
				Text::JSONArray *features = (Text::JSONArray*)jbase;
				OSInt i;
				OSInt j = features->GetArrayLength();
				OSInt k;
				Text::JSONBase *feature = features->GetArrayValue(0);
				Text::JSONBase *featType;
				Text::JSONBase *featProp;
				Text::JSONBase *featGeom;
				Math::Vector2D *vec;
				if (feature && feature->GetJSType() == Text::JSONBase::JST_OBJECT)
				{
					featType = ((Text::JSONObject*)feature)->GetObjectValue((const UTF8Char*)"type");
					featProp = ((Text::JSONObject*)feature)->GetObjectValue((const UTF8Char*)"properties");
					featGeom = ((Text::JSONObject*)feature)->GetObjectValue((const UTF8Char*)"geometry");
					if (featType && featType->GetJSType() == Text::JSONBase::JST_STRINGUTF8 && featProp && featProp->GetJSType() == Text::JSONBase::JST_OBJECT && featGeom && featGeom->GetJSType() == Text::JSONBase::JST_OBJECT)
					{
						Data::ArrayList<const UTF8Char *> colNames;
						((Text::JSONObject*)featProp)->GetObjectNames(&colNames);
						colCnt = colNames.GetCount();
						k = 0;
						while (k < colCnt)
						{
							tabHdrs[k] = colNames.GetItem(k);
							k++;
						}
						vec = ParseGeomJSON((Text::JSONObject*)featGeom, srid);
						if (vec)
						{
							NEW_CLASS(lyr, Map::VectorLayer(Map::DRAW_LAYER_MIXED, fd->GetFullName(), colCnt, tabHdrs, csys, 0, fd->GetShortName()));
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
						if (feature && feature->GetJSType() == Text::JSONBase::JST_OBJECT)
						{
							featType = ((Text::JSONObject*)feature)->GetObjectValue((const UTF8Char*)"type");
							featProp = ((Text::JSONObject*)feature)->GetObjectValue((const UTF8Char*)"properties");
							featGeom = ((Text::JSONObject*)feature)->GetObjectValue((const UTF8Char*)"geometry");
							if (featType && featType->GetJSType() == Text::JSONBase::JST_STRINGUTF8 && featProp && featProp->GetJSType() == Text::JSONBase::JST_OBJECT && featGeom && featGeom->GetJSType() == Text::JSONBase::JST_OBJECT)
							{
								k = 0;
								while (k < colCnt)
								{
									jbase = ((Text::JSONObject*)featProp)->GetObjectValue(tabHdrs[k]);
									if (jbase && jbase->GetJSType() == Text::JSONBase::JST_STRINGUTF8)
									{
										tabCols[k] = ((Text::JSONStringUTF8*)jbase)->GetValue();
									}
									else
									{
										tabCols[k] = 0;
									}
									k++;
								}
								vec = ParseGeomJSON((Text::JSONObject*)featGeom, srid);
								if (vec)
								{
									lyr->AddVector(vec, tabCols);
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
		}
	}
	fileJSON->EndUse();
	return pobj;
}

Math::Vector2D *Parser::FileParser::JSONParser::ParseGeomJSON(Text::JSONObject *obj, Int32 srid)
{
	Text::JSONBase *jbase = obj->GetObjectValue((const UTF8Char*)"type");
	if (jbase && jbase->GetJSType() == Text::JSONBase::JST_STRINGUTF8)
	{
		const UTF8Char *sType = ((Text::JSONStringUTF8*)jbase)->GetValue();
		if (sType && Text::StrEquals(sType, (const UTF8Char*)"LineString"))
		{
			jbase = obj->GetObjectValue((const UTF8Char*)"coordinates");
			if (jbase && jbase->GetJSType() == Text::JSONBase::JST_ARRAY)
			{
				Text::JSONArray *coord = (Text::JSONArray*)jbase;
				Data::ArrayList<Double> ptList;
				Data::ArrayList<Double> zList;
				Bool hasZ = false;
				Text::JSONArray *pt;
				UOSInt i = 0;
				UOSInt j = coord->GetArrayLength();
				while (i < j)
				{
					jbase = coord->GetArrayValue(i);
					if (jbase && jbase->GetJSType() == Text::JSONBase::JST_ARRAY)
					{
						pt = (Text::JSONArray*)jbase;
						if (pt->GetArrayLength() == 3)
						{
							hasZ = true;
							jbase = pt->GetArrayValue(0);
							if (jbase && jbase->GetJSType() == Text::JSONBase::JST_NUMBER)
							{
								ptList.Add(((Text::JSONNumber*)jbase)->GetValue());
							}
							else
							{
								ptList.Add(0);
							}
							jbase = pt->GetArrayValue(1);
							if (jbase && jbase->GetJSType() == Text::JSONBase::JST_NUMBER)
							{
								ptList.Add(((Text::JSONNumber*)jbase)->GetValue());
							}
							else
							{
								ptList.Add(0);
							}
							jbase = pt->GetArrayValue(2);
							if (jbase && jbase->GetJSType() == Text::JSONBase::JST_NUMBER)
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
							if (jbase && jbase->GetJSType() == Text::JSONBase::JST_NUMBER)
							{
								ptList.Add(((Text::JSONNumber*)jbase)->GetValue());
							}
							else
							{
								ptList.Add(0);
							}
							jbase = pt->GetArrayValue(1);
							if (jbase && jbase->GetJSType() == Text::JSONBase::JST_NUMBER)
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
						Math::Polyline3D *pl;
						Double *ptArr;
						Double *altArr;
						NEW_CLASS(pl, Math::Polyline3D(srid, 1, zList.GetCount()));
						ptArr = pl->GetPointList(&i);
						i = 0;
						j = ptList.GetCount();
						while (i < j)
						{
							ptArr[i] = ptList.GetItem(i);
							i++;
						}
						altArr = pl->GetAltitudeList(&i);
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
						ptArr = ptList.GetArray(&i);
						Math::Polyline *pl;
						NEW_CLASS(pl, Math::Polyline(srid, ptArr, i >> 1));
						return pl;
					}
				}
			}
		}
		else if (sType && Text::StrEquals(sType, (const UTF8Char*)"Polygon"))
		{
			jbase = obj->GetObjectValue((const UTF8Char*)"coordinates");
			if (jbase && jbase->GetJSType() == Text::JSONBase::JST_ARRAY)
			{
				Text::JSONArray *coord = (Text::JSONArray*)jbase;
				Data::ArrayList<Double> ptList;
				Data::ArrayList<Int32> partList;
				Bool hasData = false;
				Text::JSONArray *ptArr;
				Text::JSONArray *pt;
				UOSInt i = 0;
				UOSInt j = coord->GetArrayLength();
				UOSInt k;
				UOSInt l;
				while (i < j)
				{
					jbase = coord->GetArrayValue(i);
					if (jbase && jbase->GetJSType() == Text::JSONBase::JST_ARRAY)
					{
						ptArr = (Text::JSONArray*)jbase;
						hasData = false;
						k = 0;
						l = ptArr->GetArrayLength();
						while (k < l)
						{
							jbase = ptArr->GetArrayValue(k);
							if (jbase && jbase->GetJSType() == Text::JSONBase::JST_ARRAY)
							{
								pt = (Text::JSONArray*)jbase;
								if (pt->GetArrayLength() == 2)
								{
									if (!hasData)
									{
										hasData = true;
										partList.Add((Int32)ptList.GetCount() >> 1);
									}
									jbase = pt->GetArrayValue(0);
									if (jbase && jbase->GetJSType() == Text::JSONBase::JST_NUMBER)
									{
										ptList.Add(((Text::JSONNumber*)jbase)->GetValue());
									}
									else
									{
										ptList.Add(0);
									}
									jbase = pt->GetArrayValue(1);
									if (jbase && jbase->GetJSType() == Text::JSONBase::JST_NUMBER)
									{
										ptList.Add(((Text::JSONNumber*)jbase)->GetValue());
									}
									else
									{
										ptList.Add(0);
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
					Double *ptArr;
					Math::Polygon *pg;
					NEW_CLASS(pg, Math::Polygon(srid, partList.GetCount(), ptList.GetCount() >> 1));
					UInt32 *parts = pg->GetPartList(&j);
					i = 0;
					while (i < j)
					{
						parts[i] = partList.GetItem(i);
						i++;
					}
					ptArr = pg->GetPointList(&j);
					j = j << 1;
					i = 0;
					while (i < j)
					{
						ptArr[i] = ptList.GetItem(i);
						i++;
					}
					return pg;
				}
			}
		}
	}
	return 0;
}
