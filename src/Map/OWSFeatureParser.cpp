#include "Stdafx.h"
#include "IO/MemoryReadingStream.h"
#include "Map/OWSFeatureParser.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Geometry/Point.h"
#include "Math/Geometry/PointZM.h"
#include "Parser/FileParser/JSONParser.h"
#include "Parser/FileParser/XMLParser.h"
#include "Text/JSON.h"

Bool Map::OWSFeatureParser::ParseText(Text::PString txt, UInt32 srid, Math::Coord2DDbl coord, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayList<UOSInt>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList)
{
	txt.RTrim();
	if (txt.StartsWith('@') && txt.EndsWith(';'))
	{
		Text::PString nameArr[2];
		Text::PString valueArr[2];
		UOSInt nameCnt;
		UOSInt i = txt.IndexOf(' ');
		if (i == INVALID_INDEX)
			return false;
		txt = txt.Substring(i + 1);
		i = txt.IndexOf(CSTR("; "));
		if (i == INVALID_INDEX)
			return false;
		valueArr[1] = txt.Substring(i + 2);
		txt.TrimToLength(i + 1);
		nameArr[1] = txt;
		NN<Math::Geometry::Vector2D> vec;
		NEW_CLASSNN(vec, Math::Geometry::Point(srid, coord));
		vecList->Add(vec);
		valueOfstList->Add(nameList->GetCount());
		while ((nameCnt = Text::StrSplitP(nameArr, 2, nameArr[1], ';')) == 2)
		{
			if (Text::StrSplitP(valueArr, 2, valueArr[1], ';') != 2)
				return false;
			nameList->Add(Text::String::New(nameArr[0].ToCString()));
			valueList->Add(Text::String::New(valueArr[0].ToCString()));
		}
		if (Text::StrSplitP(valueArr, 2, valueArr[1], ';') != 1)
			return false;
		if (nameArr[0].leng != 0)
		{
			nameList->Add(Text::String::New(nameArr[0].ToCString()));
			valueList->Add(Text::String::New(valueArr[0].ToCString()));
		}
		return true;
	}
	else
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
					NN<Math::Geometry::Vector2D> vec;
					NEW_CLASSNN(vec, Math::Geometry::Point(srid, coord));
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
}

Bool Map::OWSFeatureParser::ParseJSON(Text::CStringNN txt, UInt32 srid, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayList<UOSInt>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList, Math::Coord2DDbl coord)
{
	Text::JSONBase *json = Text::JSONBase::ParseJSONStr(txt);
	if (json)
	{
		if (json->GetType() == Text::JSONType::Object)
		{
			NN<Text::String> crsName;
			if (json->GetValueString(CSTR("crs.properties.name")).SetTo(crsName))
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
				if (geometry)
				{
					Optional<Math::Geometry::Vector2D> vec = 0;
					NN<Math::Geometry::Vector2D> nnvec;
					if (geometry->GetType() == Text::JSONType::Object)
					{
						vec = Parser::FileParser::JSONParser::ParseGeomJSON((Text::JSONObject*)geometry, srid);
					}
					else if (geometry->GetType() == Text::JSONType::Null)
					{
						NEW_CLASSNN(nnvec, Math::Geometry::Point(srid, coord));
						vec = nnvec;
					}
					if (vec.SetTo(nnvec))
					{
						valueOfstList->Add(nameList->GetCount());
						Text::JSONBase *properties = feature->GetValue(CSTR("properties"));
						if (properties && properties->GetType() == Text::JSONType::Object)
						{
							Data::ArrayListNN<Text::String> names;
							NN<Text::String> name;
							Text::StringBuilderUTF8 sb;
							Text::JSONObject *obj = (Text::JSONObject*)properties;
							obj->GetObjectNames(names);
							Data::ArrayIterator<NN<Text::String>> it = names.Iterator();
							while (it.HasNext())
							{
								name = it.Next();
								nameList->Add(name->Clone());
								sb.ClearStr();
								obj->GetValue(name->ToCString())->ToString(sb);
								valueList->Add(Text::String::New(sb.ToCString()));
							}
						}
						vecList->Add(nnvec);
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

Bool Map::OWSFeatureParser::ParseGML(Text::CStringNN txt, UInt32 srid, Bool swapXY, Optional<Text::EncodingFactory> encFact, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayList<UOSInt>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList)
{
	UTF8Char tmpBuff[1024];
	UTF8Char *tmpPtr;

	IO::MemoryReadingStream mstm(txt.v, txt.leng);
	NN<IO::ParsedObject> pobj;
	if (Parser::FileParser::XMLParser::ParseStream(encFact, mstm, CSTR("Temp.gml"), 0, 0, 0).SetTo(pobj))
	{
		if (pobj->GetParserType() == IO::ParserType::MapLayer)
		{
			NN<Map::MapDrawLayer> layer = NN<Map::MapDrawLayer>::ConvertFrom(pobj);
			Map::NameArray *nameArr = 0;
			Data::ArrayListInt64 idArr;
			layer->GetAllObjectIds(idArr, &nameArr);
			if (idArr.GetCount() > 0)
			{
				Text::StringBuilderUTF8 sb;
				Map::GetObjectSess *sess = layer->BeginGetObject();
				UOSInt i = 0;
				UOSInt j = idArr.GetCount();
				while (i < j)
				{
					NN<Math::Geometry::Vector2D> vec;
					if (vec.Set(layer->GetNewVectorById(sess, idArr.GetItem(i))))
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
							sb.ClearStr();
							layer->GetString(sb, nameArr, idArr.GetItem(i), k);
							valueList->Add(Text::String::New(sb.ToCString()));
							k++;
						}
						vecList->Add(vec);
					}
					i++;
				}
				layer->EndGetObject(sess);
				layer->ReleaseNameArr(nameArr);
				pobj.Delete();
				return vecList->GetCount() > 0;
			}
			layer->ReleaseNameArr(nameArr);
		}
		pobj.Delete();
	}
	return false;
}

Bool Map::OWSFeatureParser::ParseESRI_WMS_XML(Text::CStringNN xml, UInt32 srid, Bool swapXY, Optional<Text::EncodingFactory> encFact, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayList<UOSInt>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList, Math::Coord2DDbl coord)
{
	IO::MemoryReadingStream mstm(xml.v, xml.leng);
	NN<Text::String> nodeText;
	Text::XMLReader reader(encFact, mstm, Text::XMLReader::ParseMode::PM_XML);
	while (reader.NextElementName().SetTo(nodeText))
	{
		if (nodeText->Equals(UTF8STRC("esri_wms:FeatureInfoResponse")) || nodeText->Equals(UTF8STRC("FeatureInfoResponse")))
		{
			return ParseESRIFeatureInfoResponse(reader, srid, swapXY, vecList, valueOfstList, nameList, valueList, coord);
		}
		else
		{
			printf("OWSFeatureParser: Unknown element in esri wms xml: %s\r\n", nodeText->v);
			reader.SkipElement();
		}
	}
	return false;
}

Bool Map::OWSFeatureParser::ParseESRIFeatureInfoResponse(NN<Text::XMLReader> reader, UInt32 srid, Bool swapXY, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayList<UOSInt>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList, Math::Coord2DDbl coord)
{
	Bool succ = true;
	Bool found = false;
	NN<Text::String> nodeText;
	while (reader->NextElementName().SetTo(nodeText))
	{
		if (nodeText->Equals(UTF8STRC("esri_wms:FeatureInfoCollection")) || nodeText->Equals(UTF8STRC("FeatureInfoCollection")))
		{
			found = true;
			succ = ParseESRIFeatureInfoCollection(reader, srid, swapXY, vecList, valueOfstList, nameList, valueList, coord);
		}
		else
		{
			printf("OWSFeatureParser: Unknown element in esri FeatureInfoResponse: %s\r\n", nodeText->v);
			reader->SkipElement();
		}
	}
	return found && succ;
}

Bool Map::OWSFeatureParser::ParseESRIFeatureInfoCollection(NN<Text::XMLReader> reader, UInt32 srid, Bool swapXY, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayList<UOSInt>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList, Math::Coord2DDbl coord)
{
	Bool succ = true;
	NN<Text::String> nodeText;
	while (reader->NextElementName().SetTo(nodeText))
	{
		if (nodeText->Equals(UTF8STRC("esri_wms:FeatureInfo")) || nodeText->Equals(UTF8STRC("FeatureInfo")))
		{
			succ = ParseESRIFeatureInfo(reader, srid, swapXY, vecList, valueOfstList, nameList, valueList, coord) && succ;
		}
		else
		{
			printf("OWSFeatureParser: Unknown element in esri FeatureInfoCollection: %s\r\n", nodeText->v);
			reader->SkipElement();
		}
	}
	return succ;
}

Bool Map::OWSFeatureParser::ParseESRIFeatureInfo(NN<Text::XMLReader> reader, UInt32 srid, Bool swapXY, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayList<UOSInt>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList, Math::Coord2DDbl coord)
{
	Bool succ = true;
	Bool found = false;
	NN<Text::String> nodeText;
	NN<Math::Geometry::Vector2D> nnvec;
	Text::StringBuilderUTF8 sb;
	valueOfstList->Add(nameList->GetCount());
	while (reader->NextElementName().SetTo(nodeText))
	{
		if (nodeText->Equals(UTF8STRC("esri_wms:CRS")))
		{
			sb.ClearStr();
			reader->ReadNodeText(sb);
			sb.ToUInt32(srid);
		}
		else if (nodeText->Equals(UTF8STRC("esri_wms:Field")) || nodeText->Equals(UTF8STRC("Field")))
		{
			Optional<Text::String> fieldName = 0;
			Optional<Text::String> fieldValue = 0;
			Optional<Math::Geometry::Vector2D> vec = 0;
			NN<Text::String> nnfieldName;
			NN<Text::String> nnfieldValue;
			while (reader->NextElementName().SetTo(nodeText))
			{
				if (nodeText->Equals(UTF8STRC("esri_wms:FieldName")) || nodeText->Equals(UTF8STRC("FieldName")))
				{
					sb.ClearStr();
					if (reader->ReadNodeText(sb))
					{
						OPTSTR_DEL(fieldName);
						fieldName = Text::String::New(sb.ToCString());
					}
				}
				else if (nodeText->Equals(UTF8STRC("esri_wms:FieldValue")) || nodeText->Equals(UTF8STRC("FieldValue")))
				{
					sb.ClearStr();
					if (reader->ReadNodeText(sb))
					{
						OPTSTR_DEL(fieldValue);
						fieldValue = Text::String::New(sb.ToCString());
					}
				}
				else if (nodeText->Equals(UTF8STRC("esri_wms:FieldGeometry")))
				{
					vec.Delete();
					vec = ParseESRIFieldGeometry(reader, srid, swapXY);
				}
				else
				{
					printf("OWSFeatureParser: Unknown element in esri Field: %s\r\n", nodeText->v);
					reader->SkipElement();
				}
			}
			if (fieldName.SetTo(nnfieldName) && fieldValue.SetTo(nnfieldValue))
			{
				if (vec.SetTo(nnvec))
				{
					if (!found)
					{
						vecList->Add(nnvec);
						found = true;
					}
					else
					{
						nnvec.Delete();
					}
					nnfieldName->Release();
					nnfieldValue->Release();
				}
				else
				{
					nameList->Add(nnfieldName);
					valueList->Add(nnfieldValue);
				}
			}
			else
			{
				OPTSTR_DEL(fieldName);
				OPTSTR_DEL(fieldValue);
				vec.Delete();
				succ = false;
			}
		}
		else
		{
			printf("OWSFeatureParser: Unknown element in esri FeatureInfo: %s\r\n", nodeText->v);
			reader->SkipElement();
		}
	}
	if (!found)
	{
		NEW_CLASSNN(nnvec, Math::Geometry::Point(srid, coord));
		vecList->Add(nnvec);
	}
	return succ;
}

Optional<Math::Geometry::Vector2D> Map::OWSFeatureParser::ParseESRIFieldGeometry(NN<Text::XMLReader> reader, UInt32 srid, Bool swapXY)
{
	NN<Text::String> nodeText;
	Optional<Math::Geometry::Vector2D> vec = 0;
	NN<Math::Geometry::Vector2D> nnvec;
	while (reader->NextElementName().SetTo(nodeText))
	{
		if (nodeText->Equals(UTF8STRC("esri_wms:Point")))
		{
			Text::StringBuilderUTF8 sb;
			reader->ReadNodeText(sb);
			Text::PString sarr[4];
			UOSInt i = Text::StrSplitP(sarr, 4, sb, ',');
			if (i == 2)
			{
				vec.Delete();
				NEW_CLASSNN(nnvec, Math::Geometry::Point(srid, sarr[0].ToDouble(), sarr[1].ToDouble()));
				vec = nnvec;
			}
			else if (i == 3)
			{
				vec.Delete();
				NEW_CLASSNN(nnvec, Math::Geometry::PointZ(srid, sarr[0].ToDouble(), sarr[1].ToDouble(), sarr[2].ToDouble()));
				vec = nnvec;
			}
			else if (i == 4)
			{
				vec.Delete();
				NEW_CLASSNN(nnvec, Math::Geometry::PointZM(srid, sarr[0].ToDouble(), sarr[1].ToDouble(), sarr[2].ToDouble(), sarr[3].ToDouble()));
				vec = nnvec;
			}
		}
		else
		{
			printf("OWSFeatureParser: Unknown element in esri FieldGeometry: %s\r\n", nodeText->v);
			reader->SkipElement();
		}
	}
	return vec;
}

Bool Map::OWSFeatureParser::ParseOGC_WMS_XML(Text::CStringNN xml, UInt32 srid, Math::Coord2DDbl coord, Optional<Text::EncodingFactory> encFact, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayList<UOSInt>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList)
{
	IO::MemoryReadingStream mstm(xml.v, xml.leng);
	NN<Text::String> nodeText;
	NN<Math::Geometry::Vector2D> vec;
	NN<Text::XMLAttrib> attr;
	Bool found = false;
	UOSInt i;
	UOSInt j;
	Text::XMLReader reader(encFact, mstm, Text::XMLReader::ParseMode::PM_XML);
	while (reader.NextElementName().SetTo(nodeText))
	{
		if (nodeText->Equals(UTF8STRC("FeatureInfoResponse")))
		{
			while (reader.NextElementName().SetTo(nodeText))
			{
				if (nodeText->Equals(UTF8STRC("FIELDS")))
				{
					valueOfstList->Add(nameList->GetCount());
					NEW_CLASSNN(vec, Math::Geometry::Point(srid, coord));
					vecList->Add(vec);
					i = 0;
					j = reader.GetAttribCount();
					while (i < j)
					{
						attr = reader.GetAttribNoCheck(i);
						nameList->Add(attr->name->Clone());
						valueList->Add(Text::String::OrEmpty(Text::String::CopyOrNull(attr->value)));
						i++;
					}
					found = true;
				}
				else
				{
					printf("OWSFeatureParser: Unknown element in FeatureInfoResponse: %s\r\n", nodeText->v);
					reader.SkipElement();
				}
			}
		}
		else
		{
			printf("OWSFeatureParser: Unknown element in ogc wms xml: %s\r\n", nodeText->v);
			reader.SkipElement();
		}
	}
	return found;
}
