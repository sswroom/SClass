#include "Stdafx.h"
#include "IO/MemoryStream.h"
#include "Map/OWSFeatureParser.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Geometry/Point.h"
#include "Parser/FileParser/JSONParser.h"
#include "Parser/FileParser/XMLParser.h"
#include "Text/JSON.h"

Bool Map::OWSFeatureParser::ParseText(Text::PString txt, UInt32 srid, Math::Coord2DDbl coord, Data::ArrayList<Math::Geometry::Vector2D*> *vecList, Data::ArrayList<UOSInt> *valueOfstList, Data::ArrayList<Text::String*> *nameList, Data::ArrayList<Text::String*> *valueList)
{
	Text::PString lineArr[2];
	Text::PString sarr[2];
	UOSInt lineCnt;
	Bool newVec = true;
	lineArr[1] = txt;
	while (true)
	{
		lineCnt = Text::StrSplitLineP(lineArr, 2, lineArr[1]);
		if (lineArr[0].StartsWith(UTF8STRC("----")))
		{
			newVec = true;
		}
		else if (Text::StrSplitTrimP(sarr, 2, lineArr[0], '=') == 2)
		{
			if (newVec)
			{
				Math::Geometry::Vector2D *vec;
				NEW_CLASS(vec, Math::Geometry::Point(srid, coord));
				vecList->Add(vec);
				valueOfstList->Add(nameList->GetCount());
				newVec = false;
			}
			nameList->Add(Text::String::New(sarr[0].ToCString()));
			valueList->Add(Text::String::New(sarr[1].ToCString()));
		}

		if (lineCnt != 2)
			break;
	}
	return nameList->GetCount() > 0;
}

Bool Map::OWSFeatureParser::ParseJSON(Text::CString txt, UInt32 srid, Data::ArrayList<Math::Geometry::Vector2D*> *vecList, Data::ArrayList<UOSInt> *valueOfstList, Data::ArrayList<Text::String*> *nameList, Data::ArrayList<Text::String*> *valueList)
{
	Text::JSONBase *json = Text::JSONBase::ParseJSONStr(txt);
	if (json)
	{
		if (json->GetType() == Text::JSONType::Object)
		{
			Text::String *crsName = json->GetValueString(CSTR("crs.properties.name"));
			if (crsName)
			{
				Math::CoordinateSystem *csys = Math::CoordinateSystemManager::CreateFromName(crsName->ToCString());
				if (csys)
				{
					srid = csys->GetSRID();
					DEL_CLASS(csys);
				}
			}
		}
		Text::JSONBase *featuresObj = json->GetValue(CSTR("features"));
		if (featuresObj && featuresObj->GetType() == Text::JSONType::Array)
		{
			Text::JSONArray *features = (Text::JSONArray*)featuresObj;
			UOSInt i = 0;
			UOSInt j = features->GetArrayLength();
			while (i < j)
			{
				Text::JSONBase *feature = features->GetArrayValue(i);
				Text::JSONBase *geometry = feature->GetValue(CSTR("geometry"));
				if (geometry && geometry->GetType() == Text::JSONType::Object)
				{
					Math::Geometry::Vector2D *vec = Parser::FileParser::JSONParser::ParseGeomJSON((Text::JSONObject*)geometry, srid);
					if (vec)
					{
						valueOfstList->Add(nameList->GetCount());
						Text::JSONBase *properties = feature->GetValue(CSTR("properties"));
						if (properties && properties->GetType() == Text::JSONType::Object)
						{
							Data::ArrayList<Text::String*> names;
							Text::String *name;
							Text::StringBuilderUTF8 sb;
							Text::JSONObject *obj = (Text::JSONObject*)properties;
							obj->GetObjectNames(&names);
							UOSInt k = 0;
							UOSInt l = names.GetCount();
							while (k < l)
							{
								name = names.GetItem(k);
								nameList->Add(name->Clone());
								sb.ClearStr();
								obj->GetValue(name->ToCString())->ToString(&sb);
								valueList->Add(Text::String::New(sb.ToCString()));
								k++;
							}
						}
						vecList->Add(vec);
					}
				}
				i++;
			}
			json->EndUse();
			return vecList->GetCount() > 0;
		}
		json->EndUse();
	}
	return false;
}

Bool Map::OWSFeatureParser::ParseGML(Text::CString txt, UInt32 srid, Bool swapXY, Text::EncodingFactory *encFact, Data::ArrayList<Math::Geometry::Vector2D*> *vecList, Data::ArrayList<UOSInt> *valueOfstList, Data::ArrayList<Text::String*> *nameList, Data::ArrayList<Text::String*> *valueList)
{
	UTF8Char tmpBuff[1024];
	UTF8Char *tmpPtr;

	IO::MemoryStream mstm((UInt8*)txt.v, txt.leng, UTF8STRC("Map.WebMapTileServiceSource.QueryInfo.mstm"));
	IO::ParsedObject *pobj = Parser::FileParser::XMLParser::ParseStream(encFact, &mstm, CSTR("Temp.gml"), 0, 0, 0);
	if (pobj)
	{
		if (pobj->GetParserType() == IO::ParserType::MapLayer)
		{
			Map::IMapDrawLayer *layer = (Map::IMapDrawLayer*)pobj;
			void *nameArr = 0;
			Data::ArrayListInt64 idArr;
			layer->GetAllObjectIds(&idArr, &nameArr);
			if (idArr.GetCount() > 0)
			{
				void *sess = layer->BeginGetObject();
				UOSInt i = 0;
				UOSInt j = idArr.GetCount();
				while (i < j)
				{
					Math::Geometry::Vector2D *vec = layer->GetNewVectorById(sess, idArr.GetItem(i));
					if (vec)
					{
						if (swapXY)
						{
							vec->SwapXY();
						}
						valueOfstList->Add(nameList->GetCount());
						UOSInt k = 0;
						UOSInt l = layer->GetColumnCnt();
						while (k < l)
						{
							tmpPtr = layer->GetColumnName(tmpBuff, k);
							nameList->Add(Text::String::NewP(tmpBuff, tmpPtr));
							tmpPtr = layer->GetString(tmpBuff, sizeof(tmpBuff), nameArr, idArr.GetItem(i), k);
							valueList->Add(Text::String::NewP(tmpBuff, tmpPtr));
							k++;
						}
						vecList->Add(vec);
					}
					i++;
				}
				layer->ReleaseNameArr(nameArr);
				DEL_CLASS(pobj);
				return vecList->GetCount() > 0;
			}
			layer->ReleaseNameArr(nameArr);
		}
		DEL_CLASS(pobj);
	}
	return false;
}
