#include "Stdafx.h"
#include "IO/DirectoryPackage.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Map/IMapDrawLayer.h"
#include "Map/MapLayerCollection.h"
#include "Map/MapServerHandler.h"
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
		Text::JSText::ToJSTextDQuote(&sb, me->layerMap.GetKey(0)->v);
		i = 1;
		while (i < j)
		{
			sb.AppendUTF8Char(',');
			Text::JSText::ToJSTextDQuote(&sb, me->layerMap.GetKey(i)->v);
			i++;
		}
	}
	sb.AppendUTF8Char(']');
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("application/json"));
	resp->AddContentLength(sb.GetLength());
	resp->Write(sb.ToString(), sb.GetLength());
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
	Map::IMapDrawLayer *layer;
	if (name == 0)
	{
		layer = 0;
	}
	else
	{
		layer = me->layerMap.Get(name);
	}
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
		Text::String *s;
		void *nameArr;
		sb.AppendUTF8Char('[');
		layer->GetAllObjectIds(&objIds, &nameArr);
		if (objIds.GetCount() > 0)
		{
			Math::CoordinateSystem *csys = layer->GetCoordinateSystem();
			void *sess = layer->BeginGetObject();
			i = 0;
			j = objIds.GetCount();
			while (i < j)
			{
				objId = objIds.GetItem(i);
				Math::Vector2D *vec = layer->GetNewVectorById(sess, objId);
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
						s = Text::XML::ToNewHTMLText(sbuff);
						sb.Append(s);
						s->Release();
						sb.AppendC(UTF8STRC(": "));
						sptr = layer->GetString(sbuff, sizeof(sbuff), nameArr, objId, k);
						if (sptr)
						{
							s = Text::XML::ToNewHTMLText(sbuff);
							sb.Append(s);
							s->Release();
						}
						k++;
					}
					sb.AppendC(UTF8STRC("\","));
					if (vec->GetVectorType() == Math::Vector2D::VectorType::Polygon)
					{
						Math::Polygon *pg = (Math::Polygon*)vec;
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
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("application/json"));
	resp->AddContentLength(sb.GetLength());
	resp->Write(sb.ToString(), sb.GetLength());
	return true;
}

void Map::MapServerHandler::AddLayer(Map::IMapDrawLayer *layer)
{
	if (layer->GetObjectClass() == Map::IMapDrawLayer::OC_MAP_LAYER_COLL)
	{
		Map::MapLayerCollection *layerColl = (Map::MapLayerCollection*)layer;
		UOSInt i = 0;
		UOSInt j = layerColl->GetCount();
		while (i < j)
		{
			Map::IMapDrawLayer *sublayer = layerColl->GetItem(i);
			this->AddLayer(sublayer);
			i++;
		}
	}
	else
	{
		this->layerMap.Put(layer->GetName(), layer);
	}
}

Map::MapServerHandler::MapServerHandler(Parser::ParserList *parsers)
{
	this->parsers = parsers;
	this->AddService(CSTR("/getlayers"), Net::WebUtil::RequestMethod::HTTP_GET, GetLayersFunc);
	this->AddService(CSTR("/getlayerdata"), Net::WebUtil::RequestMethod::HTTP_GET, GetLayerDataFunc);
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
}

Bool Map::MapServerHandler::AddAsset(Text::CString filePath)
{
	IO::Path::PathType pt = IO::Path::GetPathType(filePath);
	IO::ParsedObject *pobj;
	IO::ParserType t;
	if (pt == IO::Path::PathType::File)
	{
		IO::StmData::FileData fd(filePath, false);
		pobj = this->parsers->ParseFile(&fd, &t);
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
		Map::IMapDrawLayer *layer = (Map::IMapDrawLayer*)pobj;
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
