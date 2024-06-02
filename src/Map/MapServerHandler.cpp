#include "Stdafx.h"
#include "IO/DirectoryPackage.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Map/MapDrawLayer.h"
#include "Map/MapLayerCollection.h"
#include "Map/MapServerHandler.h"
#include "Math/CoordinateSystemConverter.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/GeoJSONWriter.h"
#include "Math/Geometry/Polygon.h"
#include "Net/WebServer/HTTPServerUtil.h"
#include "Text/JSON.h"
#include "Text/JSText.h"
#include "Text/XML.h"

Bool __stdcall Map::MapServerHandler::GetLayersFunc(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> myObj)
{
	NN<Map::MapServerHandler> me = NN<Map::MapServerHandler>::ConvertFrom(myObj);
	Text::StringBuilderUTF8 sb;
	UOSInt i;
	UOSInt j;
	sb.AppendUTF8Char('[');
	j = me->layerMap.GetCount();
	if (j > 0)
	{
		Text::JSText::ToJSTextDQuote(sb, me->layerMap.GetKey(0)->v);
		i = 1;
		while (i < j)
		{
			sb.AppendUTF8Char(',');
			Text::JSText::ToJSTextDQuote(sb, me->layerMap.GetKey(i)->v);
			i++;
		}
	}
	sb.AppendUTF8Char(']');
	resp->EnableWriteBuffer();
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("application/json"));
	Net::WebServer::HTTPServerUtil::SendContent(req, resp, CSTR("application/json"), sb.GetLength(), sb.ToString());
	return true;
}

Bool __stdcall Map::MapServerHandler::GetLayerDataFunc(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> myObj)
{
	NN<Map::MapServerHandler> me = NN<Map::MapServerHandler>::ConvertFrom(myObj);
	NN<Text::String> name;
	NN<Text::String> fmt;
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	Map::MapDrawLayer *layer;
	if (!req->GetQueryValue(CSTR("name")).SetTo(name))
	{
		name = Text::String::NewEmpty();
		layer = 0;
	}
	else
	{
		layer = me->layerMap.GetNN(name);
	}
	if (req->GetQueryValue(CSTR("fmt")).SetTo(fmt) && fmt->Equals(UTF8STRC("cesium")))
	{
		Text::JSONBuilder json(Text::JSONBuilder::OT_ARRAY);
		if (layer == 0)
		{
		}
		else //Cesium format
		{
			UOSInt i;
			UOSInt j;
			UOSInt k;
			UOSInt l;
			Data::ArrayListInt64 objIds;
			Int64 objId;
			NN<Text::String> s;
			Map::NameArray *nameArr;
			Text::StringBuilderUTF8 sb;
			layer->GetAllObjectIds(objIds, &nameArr);
			if (objIds.GetCount() > 0)
			{
				Text::StringBuilderUTF8 sbTmp;
				NN<Math::CoordinateSystem> csys = layer->GetCoordinateSystem();
				Map::GetObjectSess *sess = layer->BeginGetObject();
				i = 0;
				j = objIds.GetCount();
				while (i < j)
				{
					objId = objIds.GetItem(i);
					Math::Geometry::Vector2D *vec = layer->GetNewVectorById(sess, objId);
					if (vec)
					{
						json.ArrayBeginObject();
						sptr = name->ConcatTo(sbuff);
						*sptr++ = '_';
						sptr = Text::StrInt64(sptr, objId);
						json.ObjectAddStr(CSTR("id"), CSTRP(sbuff, sptr));
						sptr = Text::StrInt64(sbuff, objId);
						json.ObjectAddStr(CSTR("name"), CSTRP(sbuff, sptr));
						sb.ClearStr();
						k = 0;
						l = layer->GetColumnCnt();
						while (k < l)
						{
							if (k > 0) sb.AppendC(UTF8STRC("<br/>"));
							if (layer->GetColumnName(sbuff, k).SetTo(sptr))
							{
								s = Text::XML::ToNewHTMLBodyText(sbuff);
								sb.Append(s);
								s->Release();
							}
							sb.AppendC(UTF8STRC(": "));
							sbTmp.ClearStr();
							if (layer->GetString(sbTmp, nameArr, objId, k))
							{
								s = Text::XML::ToNewHTMLBodyText(sbTmp.v);
								sb.Append(s);
								s->Release();
							}
							k++;
						}
						json.ObjectAddStr(CSTR("description"), sb.ToCString());
						if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::Polygon)
						{
							Math::Geometry::Polygon *pg = (Math::Geometry::Polygon*)vec;
							json.ObjectBeginObject(CSTR("polygon"));
							json.ObjectBeginArray(CSTR("carr"));
							Data::ArrayListA<Math::Coord2DDbl> pointList;
							k = 0;
							l = pg->GetCoordinates(pointList);
							while (k < l)
							{
								json.ArrayAddVector3(Math::CoordinateSystem::ConvertToCartesianCoord(csys, Math::Vector3(pointList.GetItem(k), 0)));
								k++;
							}
							json.ArrayEnd();
							json.ObjectEnd();
						}
						else
						{

						}
						DEL_CLASS(vec);
						json.ObjectEnd();
					}

					i++;
				}
				layer->EndGetObject(sess);
			}
			layer->ReleaseNameArr(nameArr);
		}
		Text::CStringNN j = json.Build();
		resp->EnableWriteBuffer();
		resp->AddDefHeaders(req);
		resp->AddContentType(CSTR("application/json"));
		Net::WebServer::HTTPServerUtil::SendContent(req, resp, CSTR("application/json"), j.leng, j.v);
		return true;
	}
	else //GeoJSON
	{
		Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
		json.ObjectAddStr(CSTR("type"), CSTR("FeatureCollection"));
		json.ObjectBeginArray(CSTR("features"));
		if (layer)
		{
			Math::GeoJSONWriter writer;
			UOSInt i;
			UOSInt j;
			UOSInt k;
			UOSInt l;
			Data::ArrayListInt64 objIds;
			Int64 objId;
			Map::NameArray *nameArr;
			layer->GetAllObjectIds(objIds, &nameArr);
			if (objIds.GetCount() > 0)
			{
				NN<Math::CoordinateSystem> csys = layer->GetCoordinateSystem();
				NN<Math::CoordinateSystem> wgs84 = Math::CoordinateSystemManager::CreateWGS84Csys();
				Bool needConv = !wgs84->Equals(csys);
				Math::CoordinateSystemConverter converter(csys, wgs84);
				Text::StringBuilderUTF8 sbTmp;
				Map::GetObjectSess *sess = layer->BeginGetObject();
				i = 0;
				j = objIds.GetCount();
				while (i < j)
				{
					objId = objIds.GetItem(i);
					json.ArrayBeginObject();
					json.ObjectAddStr(CSTR("type"), CSTR("Feature"));
					sptr = name->ConcatTo(sbuff);
					*sptr++ = '_';
					sptr = Text::StrInt64(sptr, objId);
					json.ObjectAddStr(CSTR("id"), CSTRP(sbuff, sptr));
					json.ObjectBeginObject(CSTR("properties"));

					k = 0;
					l = layer->GetColumnCnt();
					while (k < l)
					{
						if (layer->GetColumnName(sbuff, k).SetTo(sptr))
						{
							sbTmp.ClearStr();
							if (layer->GetString(sbTmp, nameArr, objId, k))
							{
								json.ObjectAddStr(CSTRP(sbuff, sptr), sbTmp.ToCString());
							}
							else
							{
								json.ObjectAddNull(CSTRP(sbuff, sptr));
							}
						}
						k++;
					}
					json.ObjectEnd();
					NN<Math::Geometry::Vector2D> vec;
					if (vec.Set(layer->GetNewVectorById(sess, objId)))
					{
						json.ObjectBeginObject(CSTR("geometry"));
						if (needConv)
						{
							vec->Convert(converter);
						}
						writer.ToGeometry(json, vec);
						vec.Delete();
						json.ObjectEnd();
					}
					else
					{
						json.ObjectAddNull(CSTR("geometry"));
					}
					json.ObjectEnd();
					i++;
				}
				layer->EndGetObject(sess);
				wgs84.Delete();
			}
			layer->ReleaseNameArr(nameArr);
		}
		json.ArrayEnd();
		Text::CStringNN j = json.Build();
		resp->EnableWriteBuffer();
		resp->AddDefHeaders(req);
		resp->AddContentType(CSTR("application/json"));
		Net::WebServer::HTTPServerUtil::SendContent(req, resp, CSTR("application/json"), j.leng, j.v);
		return true;
	}
}

Bool __stdcall Map::MapServerHandler::CesiumDataFunc(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> myObj)
{
	NN<Map::MapServerHandler> me = NN<Map::MapServerHandler>::ConvertFrom(myObj);
	NN<Text::String> file;
	Optional<Text::String> range = req->GetQueryValue(CSTR("range"));
	Double minErr;
	if (!req->GetQueryValueF64(CSTR("minErr"), minErr))
	{
		minErr = me->cesiumMinError;
	}
	Text::StringBuilderUTF8 sb;
	if (!req->GetQueryValue(CSTR("file")).SetTo(file))
	{
		resp->ResponseError(req, Net::WebStatus::SC_FORBIDDEN);
		return true;
	}
	Double x1;
	Double y1;
	Double x2;
	Double y2;
	NN<Text::String> nnrange;
	if (!range.SetTo(nnrange))
	{
		x1 = -180;
		y1 = -90;
		x2 = 180;
		y2 = 90;
	}
	else
	{
		Text::PString sarr[5];
		UOSInt sarrCnt;
		sb.Append(nnrange);
		sarrCnt = Text::StrSplitP(sarr, 5, sb, ',');
		if (sarrCnt != 4)
		{
			resp->ResponseError(req, Net::WebStatus::SC_FORBIDDEN);
			return true;
		}
		if (!sarr[0].ToDouble(x1) || !sarr[1].ToDouble(y1) || !sarr[2].ToDouble(x2) || !sarr[3].ToDouble(y2))
		{
			resp->ResponseError(req, Net::WebStatus::SC_FORBIDDEN);
			return true;
		}
	}
	if (me->cesiumScenePath == 0)
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
		return true;
	}
	sb.ClearStr();
	sb.Append(me->cesiumScenePath);
	if (sb.ToString()[sb.GetLength() - 1] != IO::Path::PATH_SEPERATOR)
	{
		sb.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
	}
	sb.Append(file);
	if (IO::Path::PATH_SEPERATOR == '\\')
	{
		sb.Replace('/', '\\');
	}
	IO::FileStream fs(sb.ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
		return true;
	}
	IO::MemoryStream mstm;
	fs.ReadToEnd(mstm, 8192);
	mstm.Write((const UInt8*)"", 1);

	UOSInt buffSize;
	UInt8 *buff = mstm.GetBuff(buffSize);
	Text::JSONBase *json = Text::JSONBase::ParseJSONStr(Text::CStringNN(buff, buffSize - 1));
	if (json == 0)
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
		return true;
	}
	if (json->GetType() == Text::JSONType::Object)
	{
		Text::JSONObject *jobj = (Text::JSONObject*)json;
		Text::JSONBase *obj = jobj->GetObjectValue(CSTR("root"));
		if (obj)
		{
			if (!me->InObjectRange(obj, x1, y1, x2, y2))
			{
				jobj->RemoveObject(CSTR("root"));
			}
			else
			{
				me->CheckObject(obj, x1, y1, x2, y2, minErr, file, sb);
			}
		}
	}
	sb.ClearStr();
	json->ToJSONString(sb);
	json->EndUse();

	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	dt.AddHour(1);
	resp->EnableWriteBuffer();
	resp->AddDefHeaders(req);
	resp->AddCacheControl(-1);
	resp->AddExpireTime(&dt);
	resp->AddContentType(CSTR("application/json"));
	Net::WebServer::HTTPServerUtil::SendContent(req, resp, CSTR("application/json"), sb.GetLength(), sb.ToString());
	return true;
}

Bool __stdcall Map::MapServerHandler::CesiumB3DMFunc(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> myObj)
{
	NN<Map::MapServerHandler> me = NN<Map::MapServerHandler>::ConvertFrom(myObj);
	NN<Text::String> file;
	Text::StringBuilderUTF8 sb;
	if (!req->GetQueryValue(CSTR("file")).SetTo(file))
	{
		resp->ResponseError(req, Net::WebStatus::SC_FORBIDDEN);
		return true;
	}
	if (me->cesiumScenePath == 0)
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
		return true;
	}
	sb.ClearStr();
	sb.Append(me->cesiumScenePath);
	if (sb.ToString()[sb.GetLength() - 1] != IO::Path::PATH_SEPERATOR)
	{
		sb.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
	}
	sb.Append(file);
	if (IO::Path::PATH_SEPERATOR == '\\')
	{
		sb.Replace('/', '\\');
	}
	return Net::WebServer::HTTPServerUtil::ResponseFile(req, resp, sb.ToCString(), -2);
}

void Map::MapServerHandler::CheckObject(Text::JSONBase *obj, Double x1, Double y1, Double x2, Double y2, Double minErr, NN<Text::String> fileName, NN<Text::StringBuilderUTF8> tmpSb)
{
	if (obj->GetType() != Text::JSONType::Object)
	{
		return;
	}
	UOSInt i;
	NN<Text::String> s;
	Text::JSONObject *jobj = (Text::JSONObject*)obj;
	obj = jobj->GetObjectValue(CSTR("content"));
	if (obj && obj->GetType() == Text::JSONType::Object)
	{
		Text::JSONObject *content = (Text::JSONObject*)obj;
		obj = content->GetObjectValue(CSTR("url"));
		if (obj && obj->GetType() == Text::JSONType::String)
		{
			Text::JSONString *url = (Text::JSONString*)obj;
			s = url->GetValue();
			if (s->EndsWith(UTF8STRC(".json")))
			{
				tmpSb->ClearStr();
				tmpSb->AppendC(UTF8STRC("cesiumdata?file="));
				i = fileName->LastIndexOf('/');
				if (i != INVALID_INDEX)
				{
					tmpSb->AppendC(fileName->v, i + 1);
				}
				tmpSb->Append(s);
				tmpSb->AppendC(UTF8STRC("&range="));
				tmpSb->AppendDouble(x1);
				tmpSb->AppendUTF8Char(',');
				tmpSb->AppendDouble(y1);
				tmpSb->AppendUTF8Char(',');
				tmpSb->AppendDouble(x2);
				tmpSb->AppendUTF8Char(',');
				tmpSb->AppendDouble(y2);
				tmpSb->AppendC(UTF8STRC("&minErr="));
				tmpSb->AppendDouble(minErr);
				content->SetObjectString(CSTR("url"), tmpSb->ToCString());
			}
			else if (s->EndsWith(UTF8STRC(".b3dm")))
			{
				tmpSb->ClearStr();
				tmpSb->AppendC(UTF8STRC("cesiumb3dm?file="));
				i = fileName->LastIndexOf('/');
				if (i != INVALID_INDEX)
				{
					tmpSb->AppendC(fileName->v, i + 1);
				}
				tmpSb->Append(s);
				content->SetObjectString(CSTR("url"), tmpSb->ToCString());
			}
		}
	}
	if (minErr != 0)
	{
		Double err = jobj->GetObjectDouble(CSTR("geometricError"));
		if (err != 0 && err * 0.5 < minErr)
		{
			jobj->RemoveObject(CSTR("children"));
		}
	}

	obj = jobj->GetObjectValue(CSTR("children"));
	if (obj && obj->GetType() == Text::JSONType::Array)
	{
		Text::JSONArray *children = (Text::JSONArray*)obj;
		i = children->GetArrayLength();
		while (i-- > 0)
		{
			obj = children->GetArrayValue(i);
			if (!this->InObjectRange(obj, x1, y1, x2, y2))
			{
				children->RemoveArrayItem(i);
			}
			else
			{
				this->CheckObject(obj, x1, y1, x2, y2, minErr, fileName, tmpSb);
			}
		}
	}
}

Bool Map::MapServerHandler::InObjectRange(Text::JSONBase *obj, Double x1, Double y1, Double x2, Double y2)
{
	if (obj->GetType() != Text::JSONType::Object)
	{
		return false;
	}
	Text::JSONObject *jobj = (Text::JSONObject*)obj;
	obj = jobj->GetObjectValue(CSTR("boundingVolume"));
	if (obj == 0)
	{
		return false;
	}
	jobj = (Text::JSONObject*)obj;
	obj = jobj->GetObjectValue(CSTR("sphere"));
	if (obj != 0)
	{
		return this->InSphereRange(obj, x1, y1, x2, y2);
	}
	return false;
}

Bool Map::MapServerHandler::InSphereRange(Text::JSONBase *sphere, Double x1, Double y1, Double x2, Double y2)
{
	if (sphere->GetType() != Text::JSONType::Array || ((Text::JSONArray*)sphere)->GetArrayLength() != 4)
	{
		return false;
	}
	Text::JSONArray *arr = (Text::JSONArray*)sphere;
	Math::Vector3 pos;
	Double radius = arr->GetArrayDouble(3);
	pos = this->wgs84->FromCartesianCoordDeg(Math::Vector3(arr->GetArrayDouble(0), arr->GetArrayDouble(1), arr->GetArrayDouble(2)));
	if (x1 <= pos.GetLon() && x2 >= pos.GetLon())
	{
	}
	else if (x1 > pos.GetLon())
	{
		if (this->wgs84->CalSurfaceDistance(pos.GetXY(), Math::Coord2DDbl(x1, pos.GetLat()), Math::Unit::Distance::DU_METER) > radius)
		{
			return false;
		}
	}
	else
	{
		if (this->wgs84->CalSurfaceDistance(pos.GetXY(), Math::Coord2DDbl(x2, pos.GetLat()), Math::Unit::Distance::DU_METER) > radius)
		{
			return false;
		}
	}

	if (y1 <= pos.GetLat() && y2 >= pos.GetLat())
	{
	}
	else if (y1 > pos.GetLat())
	{
		if (this->wgs84->CalSurfaceDistance(pos.GetXY(), Math::Coord2DDbl(pos.GetLon(), y1), Math::Unit::Distance::DU_METER) > radius)
		{
			return false;
		}
	}
	else
	{
		if (this->wgs84->CalSurfaceDistance(pos.GetXY(), Math::Coord2DDbl(pos.GetLon(), y2), Math::Unit::Distance::DU_METER) > radius)
		{
			return false;
		}
	}
	return true;
}

void Map::MapServerHandler::AddLayer(NN<Map::MapDrawLayer> layer)
{
	if (layer->GetObjectClass() == Map::MapDrawLayer::OC_MAP_LAYER_COLL)
	{
		NN<Map::MapLayerCollection> layerColl = NN<Map::MapLayerCollection>::ConvertFrom(layer);
		UOSInt i = 0;
		UOSInt j = layerColl->GetCount();
		while (i < j)
		{
			NN<Map::MapDrawLayer> sublayer;
			if (layerColl->GetItem(i).SetTo(sublayer))
			{
				this->AddLayer(sublayer);
			}
			i++;
		}
	}
	else
	{
		this->layerMap.PutNN(layer->GetName(), layer.Ptr());
	}
}

Map::MapServerHandler::MapServerHandler(NN<Parser::ParserList> parsers)
{
	this->parsers = parsers;
	this->cesiumScenePath = 0;
	this->cesiumMinError = 0;
	this->wgs84 = Math::CoordinateSystemManager::CreateWGS84Csys();
	this->AddService(CSTR("/getlayers"), Net::WebUtil::RequestMethod::HTTP_GET, GetLayersFunc);
	this->AddService(CSTR("/getlayerdata"), Net::WebUtil::RequestMethod::HTTP_GET, GetLayerDataFunc);
	this->AddService(CSTR("/cesiumdata"), Net::WebUtil::RequestMethod::HTTP_GET, CesiumDataFunc);
	this->AddService(CSTR("/cesiumb3dm"), Net::WebUtil::RequestMethod::HTTP_GET, CesiumB3DMFunc);
}

Map::MapServerHandler::~MapServerHandler()
{
	this->assets.DeleteAll();
	SDEL_STRING(this->cesiumScenePath);
	this->wgs84.Delete();
}

Bool Map::MapServerHandler::AddAsset(Text::CStringNN filePath)
{
	IO::Path::PathType pt = IO::Path::GetPathType(filePath);
	Optional<IO::ParsedObject> pobj;
	NN<IO::ParsedObject> nnpobj;
	if (pt == IO::Path::PathType::File)
	{
		IO::StmData::FileData fd(filePath, false);
		pobj = this->parsers->ParseFile(fd);
	}
	else if (pt == IO::Path::PathType::Directory)
	{
		IO::DirectoryPackage dpkg(filePath);
		pobj = this->parsers->ParseObject(dpkg);
	}
	else
	{
		return false;
	}
	if (!pobj.SetTo(nnpobj))
	{
		return false;
	}

	if (nnpobj->GetParserType() == IO::ParserType::MapLayer)
	{
		NN<Map::MapDrawLayer> layer = NN<Map::MapDrawLayer>::ConvertFrom(nnpobj);
		this->AddLayer(layer);
		this->assets.Add(nnpobj);
		return true;
	}
	else
	{
		nnpobj.Delete();
		return false;
	}
}

void Map::MapServerHandler::SetCesiumScenePath(Text::CString cesiumScenePath)
{
	SDEL_STRING(this->cesiumScenePath);
	this->cesiumScenePath = Text::String::New(cesiumScenePath).Ptr();
}

void Map::MapServerHandler::SetCesiumMinError(Double minError)
{
	this->cesiumMinError = minError;
}
