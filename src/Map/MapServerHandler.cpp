#include "Stdafx.h"
#include "IO/DirectoryPackage.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Map/MapDrawLayer.h"
#include "Map/MapLayerCollection.h"
#include "Map/MapServerHandler.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/GeoJSONWriter.h"
#include "Net/WebServer/HTTPServerUtil.h"
#include "Text/JSON.h"
#include "Text/JSText.h"
#include "Text/XML.h"

Bool __stdcall Map::MapServerHandler::GetLayersFunc(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, WebServiceHandler *myObj)
{
	Map::MapServerHandler *me = (Map::MapServerHandler*)myObj;
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

Bool __stdcall Map::MapServerHandler::GetLayerDataFunc(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, WebServiceHandler *myObj)
{
	Map::MapServerHandler *me = (Map::MapServerHandler*)myObj;
	Text::String *name = req->GetQueryValue(CSTR("name"));
	Text::String *fmt = req->GetQueryValue(CSTR("fmt"));
	Text::StringBuilderUTF8 sb;
	Double x;
	Double y;
	Double z;
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Map::MapDrawLayer *layer;
	if (name == 0)
	{
		layer = 0;
	}
	else
	{
		layer = me->layerMap.Get(name);
	}
	if (fmt != 0 && fmt->Equals(UTF8STRC("cesium")))
	{
		if (layer == 0)
		{
			sb.AppendC(UTF8STRC("[]"));
		}
		else //Cesium format
		{
			UOSInt i;
			UOSInt j;
			UOSInt k;
			UOSInt l;
			Data::ArrayListInt64 objIds;
			Int64 objId;
			NotNullPtr<Text::String> s;
			Map::NameArray *nameArr;
			sb.AppendUTF8Char('[');
			layer->GetAllObjectIds(&objIds, &nameArr);
			if (objIds.GetCount() > 0)
			{
				Math::CoordinateSystem *csys = layer->GetCoordinateSystem();
				Map::GetObjectSess *sess = layer->BeginGetObject();
				i = 0;
				j = objIds.GetCount();
				while (i < j)
				{
					objId = objIds.GetItem(i);
					Math::Geometry::Vector2D *vec = layer->GetNewVectorById(sess, objId);
					if (vec)
					{
						if (i > 0)
							sb.AppendUTF8Char(',');
						sb.AppendUTF8Char('{');
						sb.AppendC(UTF8STRC("\"id\":\""));
						sb.Append(name);
						sb.AppendUTF8Char('_');
						sb.AppendI64(objId);
						sb.AppendC(UTF8STRC("\",\"name\":\""));
						sb.AppendI64(objId);
						sb.AppendC(UTF8STRC("\",\"description\":\""));
						k = 0;
						l = layer->GetColumnCnt();
						while (k < l)
						{
							if (k > 0) sb.AppendC(UTF8STRC("<br/>"));
							sptr = layer->GetColumnName(sbuff, k);
							s = Text::XML::ToNewHTMLBodyText(sbuff);
							sb.Append(s);
							s->Release();
							sb.AppendC(UTF8STRC(": "));
							sptr = layer->GetString(sbuff, sizeof(sbuff), nameArr, objId, k);
							if (sptr)
							{
								s = Text::XML::ToNewHTMLBodyText(sbuff);
								sb.Append(s);
								s->Release();
							}
							k++;
						}
						sb.AppendC(UTF8STRC("\","));
						if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::Polygon)
						{
							Math::Geometry::Polygon *pg = (Math::Geometry::Polygon*)vec;
							sb.AppendC(UTF8STRC("\"polygon\":{\"carr\":["));
							k = 0;
							Math::Coord2DDbl *pointList = pg->GetPointList(&l);
							while (k < l)
							{
								Math::CoordinateSystem::ConvertToCartesianCoord(csys, pointList[k].x, pointList[k].y, 0, &x, &y, &z);
								if (k > 0)
								{
									sb.AppendC(UTF8STRC(",\r\n"));
								}
								sb.AppendUTF8Char('[');
								sb.AppendDouble(x);
								sb.AppendUTF8Char(',');
								sb.AppendDouble(y);
								sb.AppendUTF8Char(',');
								sb.AppendDouble(z);
								sb.AppendUTF8Char(']');
								k++;
							}
							sb.AppendC(UTF8STRC("]}"));
						}
						else
						{

						}
						DEL_CLASS(vec);
						sb.AppendUTF8Char('}');
					}

					i++;
				}
				layer->EndGetObject(sess);
			}
			layer->ReleaseNameArr(nameArr);
			sb.AppendUTF8Char(']');
		}
	}
	else //GeoJSON
	{
		sb.AppendUTF8Char('{');
		sb.AppendC(UTF8STRC("\"type\":\"FeatureCollection\","));
		sb.AppendC(UTF8STRC("\"features\":["));
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
			layer->GetAllObjectIds(&objIds, &nameArr);
			if (objIds.GetCount() > 0)
			{
				Math::CoordinateSystem *csys = layer->GetCoordinateSystem();
				Math::CoordinateSystem *wgs84 = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84);
				Bool needConv = (csys != 0) && !wgs84->Equals(csys);
				Map::GetObjectSess *sess = layer->BeginGetObject();
				i = 0;
				j = objIds.GetCount();
				while (i < j)
				{
					objId = objIds.GetItem(i);
					if (i > 0)
						sb.AppendUTF8Char(',');
					sb.AppendC(UTF8STRC("{\"type\":\"Feature\""));
					sb.AppendC(UTF8STRC(",\"id\":\""));
					sb.Append(name);
					sb.AppendUTF8Char('_');
					sb.AppendI64(objId);
					sb.AppendC(UTF8STRC("\",\"properties\":{"));
					k = 0;
					l = layer->GetColumnCnt();
					while (k < l)
					{
						if (k > 0)
							sb.AppendUTF8Char(',');
						sptr = layer->GetColumnName(sbuff, k);
						Text::JSText::ToJSTextDQuote(sb, sbuff);
						sb.AppendUTF8Char(':');
						sptr = layer->GetString(sbuff, sizeof(sbuff), nameArr, objId, k);
						if (sptr)
						{
							Text::JSText::ToJSTextDQuote(sb, sbuff);
						}
						else
						{
							sb.AppendC(UTF8STRC("null"));
						}
						k++;
					}
					sb.AppendC(UTF8STRC("},\"geometry\":"));
					Math::Geometry::Vector2D *vec = layer->GetNewVectorById(sess, objId);
					if (vec && needConv)
					{
						vec->ConvCSys(csys, wgs84);
					}
					writer.ToGeometry(sb, vec);
					SDEL_CLASS(vec);
					sb.AppendUTF8Char('}');
					i++;
				}
				layer->EndGetObject(sess);
				DEL_CLASS(wgs84);
			}
			layer->ReleaseNameArr(nameArr);
		}
		sb.AppendUTF8Char(']');
		sb.AppendUTF8Char('}');
	}
	resp->EnableWriteBuffer();
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("application/json"));
	Net::WebServer::HTTPServerUtil::SendContent(req, resp, CSTR("application/json"), sb.GetLength(), sb.ToString());
	return true;
}

Bool __stdcall Map::MapServerHandler::CesiumDataFunc(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, WebServiceHandler *myObj)
{
	Map::MapServerHandler *me = (Map::MapServerHandler*)myObj;
	Text::String *file = req->GetQueryValue(CSTR("file"));
	Text::String *range = req->GetQueryValue(CSTR("range"));
	Double minErr;
	if (!req->GetQueryValueF64(CSTR("minErr"), &minErr))
	{
		minErr = me->cesiumMinError;
	}
	Text::StringBuilderUTF8 sb;
	if (file == 0)
	{
		resp->ResponseError(req, Net::WebStatus::SC_FORBIDDEN);
		return true;
	}
	Double x1;
	Double y1;
	Double x2;
	Double y2;
	if (range == 0)
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
		sb.Append(range);
		sarrCnt = Text::StrSplitP(sarr, 5, sb, ',');
		if (sarrCnt != 4)
		{
			resp->ResponseError(req, Net::WebStatus::SC_FORBIDDEN);
			return true;
		}
		if (!sarr[0].ToDouble(&x1) || !sarr[1].ToDouble(&y1) || !sarr[2].ToDouble(&x2) || !sarr[3].ToDouble(&y2))
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
	fs.ReadToEnd(&mstm, 8192);
	mstm.Write((const UInt8*)"", 1);

	UOSInt buffSize;
	UInt8 *buff = mstm.GetBuff(&buffSize);
	Text::JSONBase *json = Text::JSONBase::ParseJSONStr(Text::CString(buff, buffSize));
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

Bool __stdcall Map::MapServerHandler::CesiumB3DMFunc(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, WebServiceHandler *myObj)
{
	Map::MapServerHandler *me = (Map::MapServerHandler*)myObj;
	Text::String *file = req->GetQueryValue(CSTR("file"));
	Text::StringBuilderUTF8 sb;
	if (file == 0)
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

void Map::MapServerHandler::CheckObject(Text::JSONBase *obj, Double x1, Double y1, Double x2, Double y2, Double minErr, Text::String *fileName, NotNullPtr<Text::StringBuilderUTF8> tmpSb)
{
	if (obj->GetType() != Text::JSONType::Object)
	{
		return;
	}
	UOSInt i;
	Text::String *s;
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
	Double lat;
	Double lon;
	Double h;
	Double radius = arr->GetArrayDouble(3);
	this->wgs84->FromCartesianCoordDeg(arr->GetArrayDouble(0), arr->GetArrayDouble(1), arr->GetArrayDouble(2), &lat, &lon, &h);
	if (x1 <= lon && x2 >= lon)
	{
	}
	else if (x1 > lon)
	{
		if (this->wgs84->CalSurfaceDistanceXY(Math::Coord2DDbl(lon, lat), Math::Coord2DDbl(x1, lat), Math::Unit::Distance::DU_METER) > radius)
		{
			return false;
		}
	}
	else
	{
		if (this->wgs84->CalSurfaceDistanceXY(Math::Coord2DDbl(lon, lat), Math::Coord2DDbl(x2, lat), Math::Unit::Distance::DU_METER) > radius)
		{
			return false;
		}
	}

	if (y1 <= lat && y2 >= lat)
	{
	}
	else if (y1 > lat)
	{
		if (this->wgs84->CalSurfaceDistanceXY(Math::Coord2DDbl(lon, lat), Math::Coord2DDbl(lon, y1), Math::Unit::Distance::DU_METER) > radius)
		{
			return false;
		}
	}
	else
	{
		if (this->wgs84->CalSurfaceDistanceXY(Math::Coord2DDbl(lon, lat), Math::Coord2DDbl(lon, y2), Math::Unit::Distance::DU_METER) > radius)
		{
			return false;
		}
	}
	return true;
}

void Map::MapServerHandler::AddLayer(Map::MapDrawLayer *layer)
{
	if (layer->GetObjectClass() == Map::MapDrawLayer::OC_MAP_LAYER_COLL)
	{
		Map::MapLayerCollection *layerColl = (Map::MapLayerCollection*)layer;
		UOSInt i = 0;
		UOSInt j = layerColl->GetCount();
		while (i < j)
		{
			Map::MapDrawLayer *sublayer = layerColl->GetItem(i);
			this->AddLayer(sublayer);
			i++;
		}
	}
	else
	{
		this->layerMap.PutNN(layer->GetName(), layer);
	}
}

Map::MapServerHandler::MapServerHandler(Parser::ParserList *parsers)
{
	this->parsers = parsers;
	this->cesiumScenePath = 0;
	this->cesiumMinError = 0;
	this->wgs84 = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84);
	this->AddService(CSTR("/getlayers"), Net::WebUtil::RequestMethod::HTTP_GET, GetLayersFunc);
	this->AddService(CSTR("/getlayerdata"), Net::WebUtil::RequestMethod::HTTP_GET, GetLayerDataFunc);
	this->AddService(CSTR("/cesiumdata"), Net::WebUtil::RequestMethod::HTTP_GET, CesiumDataFunc);
	this->AddService(CSTR("/cesiumb3dm"), Net::WebUtil::RequestMethod::HTTP_GET, CesiumB3DMFunc);
}

Map::MapServerHandler::~MapServerHandler()
{
	IO::ParsedObject *pobj;
	UOSInt i = this->assets.GetCount();
	while (i-- > 0)
	{
		pobj = this->assets.GetItem(i);
		DEL_CLASS(pobj);
	}
	SDEL_STRING(this->cesiumScenePath);
	DEL_CLASS(this->wgs84);
}

Bool Map::MapServerHandler::AddAsset(Text::CStringNN filePath)
{
	IO::Path::PathType pt = IO::Path::GetPathType(filePath);
	IO::ParsedObject *pobj;
	IO::ParserType t;
	if (pt == IO::Path::PathType::File)
	{
		IO::StmData::FileData fd(filePath, false);
		pobj = this->parsers->ParseFile(fd, &t);
	}
	else if (pt == IO::Path::PathType::Directory)
	{
		IO::DirectoryPackage dpkg(filePath);
		pobj = this->parsers->ParseObject(&dpkg, &t);
	}
	else
	{
		return false;
	}
	if (pobj == 0)
	{
		return false;
	}

	if (t == IO::ParserType::MapLayer)
	{
		Map::MapDrawLayer *layer = (Map::MapDrawLayer*)pobj;
		this->AddLayer(layer);
		this->assets.Add(pobj);
		return true;
	}
	else
	{
		DEL_CLASS(pobj);
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
