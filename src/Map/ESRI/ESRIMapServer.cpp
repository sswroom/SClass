#include "Stdafx.h"
#include "Data/ArrayListA.h"
#include "IO/FileStream.h"
#include "IO/StmData/MemoryDataRef.h"
#include "Map/ESRI/ESRIMapServer.h"
#include "Math/CoordinateSystemManager.h"
#include "Net/HTTPClient.h"
#include "Parser/FileParser/PNGParser.h"
#include "Text/Encoding.h"
#include "Text/JSON.h"

Map::ESRI::ESRIMapServer::ESRIMapServer(Text::CString url, Net::SocketFactory *sockf, Net::SSLEngine *ssl)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UInt8 buff[2048];
	UOSInt readSize;
	UInt32 codePage;
	UOSInt i;
	UOSInt j;
	this->url = Text::String::New(url);
	this->sockf = sockf;
	this->ssl = ssl;
	this->bounds = Math::RectAreaDbl(Math::Coord2DDbl(0, 0), Math::Coord2DDbl(0, 0));
	this->tileOrigin = Math::Coord2DDbl(0, 0);
	this->tileWidth = 0;
	this->tileHeight = 0;
	this->supportMap = false;
	this->supportTileMap = false;
	this->supportQuery = false;
	this->supportData = false;
	this->csys = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84);

	sptr = url.ConcatTo(sbuff);
	if (Text::StrEndsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("/MapServer")))
	{
		sptr -= 10;
		*sptr = 0;
	}
	i = Text::StrIndexOfC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("/services/"));
	if (i != INVALID_INDEX)
	{
		this->name = Text::String::NewP(&sbuff[i + 10], sptr);
	}
	else
	{
		i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '/');
		this->name = Text::String::NewP(&sbuff[i + 1], sptr);
	}

	sptr = Text::StrConcatC(url.ConcatTo(sbuff), UTF8STRC("?f=json"));
	Net::HTTPClient *cli = Net::HTTPClient::CreateConnect(sockf, ssl, CSTRP(sbuff, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
	IO::MemoryStream mstm(UTF8STRC("Map.ESRI.ESRITileMap.ESRITileMap"));
	while ((readSize = cli->Read(buff, 2048)) > 0)
	{
		mstm.Write(buff, readSize);
	}
	codePage = cli->GetContentCodePage();
	DEL_CLASS(cli);


	UInt8 *jsonBuff = mstm.GetBuff(&readSize);
	if (jsonBuff && readSize > 0)
	{
		Text::StringBuilderUTF8 sb;
		Text::Encoding enc(codePage);
		UOSInt charsCnt;
		UTF8Char *jsonStr;
		charsCnt = enc.CountUTF8Chars(jsonBuff, readSize);
		jsonStr = MemAlloc(UTF8Char, charsCnt + 1);
		enc.UTF8FromBytes(jsonStr, jsonBuff, readSize, 0);
		
		Text::JSONBase *json = Text::JSONBase::ParseJSONStr(Text::CString(jsonStr, charsCnt));
		if (json)
		{
			if (json->GetType() == Text::JSONType::Object)
			{
				Text::JSONObject *jobj = (Text::JSONObject*)json;
				Text::JSONBase *o = jobj->GetObjectValue(CSTR("initialExtent"));
				Text::JSONBase *v;
				Text::JSONObject *vobj;
				Bool hasInit = false;
				if (o != 0 && o->GetType() == Text::JSONType::Object)
				{
					Text::JSONObject *ext = (Text::JSONObject*)o;
					this->initBounds.tl.x = ext->GetObjectDouble(CSTR("xmin"));
					this->initBounds.tl.y = ext->GetObjectDouble(CSTR("ymin"));
					this->initBounds.br.x = ext->GetObjectDouble(CSTR("xmax"));
					this->initBounds.br.y = ext->GetObjectDouble(CSTR("ymax"));
					hasInit = true;
				}
				o = jobj->GetObjectValue(CSTR("fullExtent"));
				if (o != 0 && o->GetType() == Text::JSONType::Object)
				{
					Text::JSONObject *ext = (Text::JSONObject*)o;
					this->bounds.tl.x = ext->GetObjectDouble(CSTR("xmin"));
					this->bounds.tl.y = ext->GetObjectDouble(CSTR("ymin"));
					this->bounds.br.x = ext->GetObjectDouble(CSTR("xmax"));
					this->bounds.br.y = ext->GetObjectDouble(CSTR("ymax"));
					if (!hasInit)
					{
						this->initBounds = this->bounds;
					}
				}
				else
				{
					if (hasInit)
					{
						this->bounds = this->initBounds;
					}
					else
					{
						this->bounds = Math::RectAreaDbl(-180, -90, 360, 180);
						this->initBounds = this->bounds;
					}
				}

				o = jobj->GetObjectValue(CSTR("spatialReference"));
				if (o != 0 && o->GetType() == Text::JSONType::Object)
				{
					Text::JSONObject *spRef = (Text::JSONObject*)o;
					UInt32 wkid = (UInt32)spRef->GetObjectInt32(CSTR("wkid"));
					SDEL_CLASS(this->csys);
					this->csys = Math::CoordinateSystemManager::SRCreateCSys(wkid);
				}

				o = jobj->GetObjectValue(CSTR("capabilities"));
				if (o != 0 && o->GetType() == Text::JSONType::String)
				{
					sb.ClearStr();
					sb.Append(((Text::JSONString*)o)->GetValue());
					Text::PString sarr[2];
					sarr[1] = sb;
					UOSInt sarrCnt;
					while (true)
					{
						sarrCnt = Text::StrSplitP(sarr, 2, sarr[1], ',');
						if (sarr[0].Equals(UTF8STRC("Map")))
							this->supportMap = true;
						else if (sarr[0].Equals(UTF8STRC("Tilemap")))
							this->supportTileMap = true;
						else if (sarr[0].Equals(UTF8STRC("Query")))
							this->supportQuery = true;
						else if (sarr[0].Equals(UTF8STRC("Data")))
							this->supportData = true;
						if (sarrCnt != 2)
							break;
					}
				}

				o = jobj->GetObjectValue(CSTR("tileInfo"));
				if (o != 0 && o->GetType() == Text::JSONType::Object)
				{
					Text::JSONObject *tinfo = (Text::JSONObject*)o;
					this->tileHeight = (UOSInt)tinfo->GetObjectInt32(CSTR("rows"));
					this->tileWidth = (UOSInt)tinfo->GetObjectInt32(CSTR("cols"));
					v = tinfo->GetObjectValue(CSTR("origin"));
					if (v != 0 && v->GetType() == Text::JSONType::Object)
					{
						Text::JSONObject *origin = (Text::JSONObject*)v;
						this->tileOrigin.x = origin->GetObjectDouble(CSTR("x"));
						this->tileOrigin.y = origin->GetObjectDouble(CSTR("y"));
					}
					v = tinfo->GetObjectValue(CSTR("lods"));
					if (v != 0 && v->GetType() == Text::JSONType::Array)
					{
						Text::JSONArray *levs = (Text::JSONArray*)v;
						i = 0;
						j = levs->GetArrayLength();
						while (i < j)
						{
							v = levs->GetArrayValue(i);
							if (v != 0 && v->GetType() == Text::JSONType::Object)
							{
								vobj = (Text::JSONObject*)v;
								Double lev = vobj->GetObjectDouble(CSTR("resolution"));
								if (lev != 0)
									this->tileLevels.Add(lev);
							}
							i++;
						}
					}
				}
			}
		
			json->EndUse();
		}
		
		MemFree(jsonStr);
	}
}

Map::ESRI::ESRIMapServer::~ESRIMapServer()
{
	SDEL_STRING(this->url);
	SDEL_CLASS(this->csys);
	SDEL_STRING(this->name);
}

Bool Map::ESRI::ESRIMapServer::IsError() const
{
	return (this->bounds.tl.x == this->bounds.br.x || this->bounds.tl.y == this->bounds.br.y);
}

Bool Map::ESRI::ESRIMapServer::HasTile() const
{
	if (this->tileWidth == 0 || this->tileHeight == 0 || this->tileLevels.GetCount() == 0)
		return false;
	return true;
}

Text::String *Map::ESRI::ESRIMapServer::GetURL() const
{
	return this->url;
}

Math::RectAreaDbl Map::ESRI::ESRIMapServer::GetBounds() const
{
	return this->bounds;
}

UOSInt Map::ESRI::ESRIMapServer::TileGetLevelCount() const
{
	return this->tileLevels.GetCount();
}

Double Map::ESRI::ESRIMapServer::TileGetLevelResolution(UOSInt level) const
{
	return this->tileLevels.GetItem(level);
}

UOSInt Map::ESRI::ESRIMapServer::TileGetWidth() const
{
	return this->tileWidth;
}

UOSInt Map::ESRI::ESRIMapServer::TileGetHeight() const
{
	return this->tileHeight;
}

Math::Coord2DDbl Map::ESRI::ESRIMapServer::TileGetOrigin() const
{
	return this->tileOrigin;
}

UTF8Char *Map::ESRI::ESRIMapServer::TileGetURL(UTF8Char *sbuff, UOSInt level, Int32 tileX, Int32 tileY) const
{
	sbuff = this->url->ConcatTo(sbuff);
	sbuff = Text::StrConcatC(sbuff, UTF8STRC("/tile/"));
	sbuff = Text::StrUOSInt(sbuff, level);
	sbuff = Text::StrConcatC(sbuff, UTF8STRC("/"));
	sbuff = Text::StrInt32(sbuff, tileY);
	sbuff = Text::StrConcatC(sbuff, UTF8STRC("/"));
	sbuff = Text::StrInt32(sbuff, tileX);
	return sbuff;
}

Bool Map::ESRI::ESRIMapServer::TileLoadToStream(IO::Stream *stm, UOSInt level, Int32 tileX, Int32 tileY) const
{
	UInt8 dataBuff[2048];
	UTF8Char url[1024];
	UTF8Char *sptr;
	UOSInt readSize;
	sptr = this->TileGetURL(url, level, tileX, tileY);

	Net::HTTPClient *cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, CSTRP(url, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
	Bool succ = cli->GetRespStatus() == Net::WebStatus::SC_OK;
	while ((readSize = cli->Read(dataBuff, 2048)) > 0)
	{
		if (readSize != stm->Write(dataBuff, readSize))
		{
			succ = false;
		}
	}
	DEL_CLASS(cli);
	return succ;
}

Bool Map::ESRI::ESRIMapServer::TileLoadToFile(Text::CString fileName, UOSInt level, Int32 tileX, Int32 tileY) const
{
	UInt8 dataBuff[2048];
	UOSInt readSize;
	UTF8Char url[1024];
	UTF8Char *sptr;
	sptr = this->TileGetURL(url, level, tileX, tileY);

	Net::HTTPClient *cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, CSTRP(url, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
	Bool succ = cli->GetRespStatus() == Net::WebStatus::SC_OK;
	if (succ)
	{
		IO::FileStream fs(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		while ((readSize = cli->Read(dataBuff, 2048)) > 0)
		{
			if (readSize != fs.Write(dataBuff, readSize))
			{
				succ = false;
			}
		}
	}
	DEL_CLASS(cli);
	return succ;
}

Math::Geometry::Vector2D *Map::ESRI::ESRIMapServer::Identify(Math::Coord2DDbl pt, Data::ArrayList<Text::String*> *nameList, Data::ArrayList<Text::String*> *valueList)
{
	Math::RectAreaDbl bounds;
	UInt32 width = 640;
	UInt32 height = 480;
	Math::Coord2DDbl size = Math::Coord2DDbl(width, height);
	if (this->csys == 0 || this->csys->IsProjected())
	{
		bounds.tl = pt - size * 0.5;
		bounds.br = pt + size * 0.5;
	}
	else
	{
		bounds.tl = pt - size * 0.0000025;
		bounds.br = pt + size * 0.0000025;
	}
	return this->QueryInfo(pt, bounds, width, height, 96, nameList, valueList);
}

Text::String *Map::ESRI::ESRIMapServer::GetName() const
{
	return this->name;
}

Math::CoordinateSystem *Map::ESRI::ESRIMapServer::GetCoordinateSystem() const
{
	return this->csys;
}

Math::RectAreaDbl Map::ESRI::ESRIMapServer::GetInitBounds() const
{
	return this->initBounds;
}

Bool Map::ESRI::ESRIMapServer::GetBounds(Math::RectAreaDbl *bounds) const
{
	*bounds = this->bounds;
	return true;
}

Bool Map::ESRI::ESRIMapServer::CanQuery() const
{
	return true;
}

Math::Geometry::Vector2D *Map::ESRI::ESRIMapServer::QueryInfo(Math::Coord2DDbl coord, Math::RectAreaDbl bounds, UInt32 width, UInt32 height, Double dpi, Data::ArrayList<Text::String*> *nameList, Data::ArrayList<Text::String*> *valueList)
{
	// https://services.arcgisonline.com/arcgis/rest/services/World_Imagery/MapServer/identify?geometryType=esriGeometryPoint&geometry=114.2,22.4&sr=4326&tolerance=0&mapExtent=113,22,115,23&imageDisplay=400,300,96&f=json
	UTF8Char url[1024];
	UTF8Char *sptr;
	UInt8 dataBuff[2048];
	UOSInt readSize;
	sptr = this->url->ConcatTo(url);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/identify?geometryType=esriGeometryPoint&geometry="));
	sptr = Text::StrDouble(sptr, coord.x);
	*sptr++ = ',';
	sptr = Text::StrDouble(sptr, coord.y);
	sptr = Text::StrConcatC(sptr, UTF8STRC("&tolerance=0&mapExtent="));
	sptr = Text::StrDouble(sptr, bounds.tl.x);
	*sptr++ = ',';
	sptr = Text::StrDouble(sptr, bounds.tl.y);
	*sptr++ = ',';
	sptr = Text::StrDouble(sptr, bounds.br.x);
	*sptr++ = ',';
	sptr = Text::StrDouble(sptr, bounds.br.y);
	sptr = Text::StrConcatC(sptr, UTF8STRC("&imageDisplay="));
	sptr = Text::StrUInt32(sptr, width);
	*sptr++ = ',';
	sptr = Text::StrUInt32(sptr, height);
	*sptr++ = ',';
	sptr = Text::StrInt32(sptr, Double2Int32(dpi));
	sptr = Text::StrConcatC(sptr, UTF8STRC("&f=json"));

	Math::Geometry::Vector2D *ret = 0;
	Net::HTTPClient *cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, CSTRP(url, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
	Bool succ = cli->GetRespStatus() == Net::WebStatus::SC_OK;
	if (succ)
	{
		IO::MemoryStream mstm(UTF8STRC("Map.ESRI.ESRIMapServer.QueryInfo"));
		while ((readSize = cli->Read(dataBuff, 2048)) > 0)
		{
			mstm.Write(dataBuff, readSize);
		}

		mstm.Write((const UInt8*)"", 1);
		UInt8 *buff = mstm.GetBuff(&readSize);
		Text::JSONBase *json = Text::JSONBase::ParseJSONStr(Text::CString(buff, readSize - 1));
		if (json)
		{
			Text::JSONBase *o = json->GetValue(UTF8STRC("results[0]"));
			if (o && o->GetType() == Text::JSONType::Object)
			{
				Text::JSONObject *result = (Text::JSONObject*)o;
				Text::String *geometryType = result->GetString(UTF8STRC("geometryType"));
				if (geometryType)
				{
					ret = ParseGeometry(this->csys->GetSRID(), geometryType, result->GetObjectValue(CSTR("geometry")));
					if (ret)
					{
						o = result->GetObjectValue(CSTR("attributes"));
						if (o && o->GetType() == Text::JSONType::Object)
						{
							Text::JSONObject *attr = (Text::JSONObject*)o;
							Data::ArrayList<Text::String*> attNames;
							Text::String *name;
							Text::StringBuilderUTF8 sb;
							attr->GetObjectNames(&attNames);
							UOSInt i = 0;
							UOSInt j = attNames.GetCount();
							while (i < j)
							{
								name = attNames.GetItem(i);
								sb.ClearStr();
								attr->GetObjectValue(name->ToCString())->ToString(&sb);
								nameList->Add(name->Clone());
								valueList->Add(Text::String::New(sb.ToCString()));
								i++;
							}
						}
					}
				}
			}
			json->EndUse();
		}
	}
	DEL_CLASS(cli);
	return ret;
}

Media::ImageList *Map::ESRI::ESRIMapServer::DrawMap(Math::RectAreaDbl bounds, UInt32 width, UInt32 height, Double dpi, Text::StringBuilderUTF8 *sbUrl)
{
	UTF8Char url[1024];
	UTF8Char *sptr;
	UInt8 dataBuff[2048];
	UOSInt readSize;
	UInt32 srid = 0;
	if (this->csys)
	{
		srid = this->csys->GetSRID();
	}
	sptr = this->url->ConcatTo(url);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/export?dpi="));
	sptr = Text::StrDouble(sptr, dpi);
	sptr = Text::StrConcatC(sptr, UTF8STRC("&transparent=true&format=png8&bbox="));
	sptr = Text::StrDouble(sptr, bounds.tl.x);
	sptr = Text::StrConcatC(sptr, UTF8STRC("%2C"));
	sptr = Text::StrDouble(sptr, bounds.tl.y);
	sptr = Text::StrConcatC(sptr, UTF8STRC("%2C"));
	sptr = Text::StrDouble(sptr, bounds.br.x);
	sptr = Text::StrConcatC(sptr, UTF8STRC("%2C"));
	sptr = Text::StrDouble(sptr, bounds.br.y);
	if (srid != 0)
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("&bboxSR="));
		sptr = Text::StrUInt32(sptr, srid);
		sptr = Text::StrConcatC(sptr, UTF8STRC("&imageSR="));
		sptr = Text::StrUInt32(sptr, srid);
	}
	sptr = Text::StrConcatC(sptr, UTF8STRC("&size="));
	sptr = Text::StrUOSInt(sptr, width);
	sptr = Text::StrConcatC(sptr, UTF8STRC("%2C"));
	sptr = Text::StrUOSInt(sptr, height);
	sptr = Text::StrConcatC(sptr, UTF8STRC("&f=image"));

	if (sbUrl)
		sbUrl->AppendC(url, (UOSInt)(sptr - url));

	Media::ImageList *ret = 0;
	Net::HTTPClient *cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, CSTRP(url, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
	Bool succ = cli->GetRespStatus() == Net::WebStatus::SC_OK;
	if (succ)
	{
		IO::MemoryStream mstm(UTF8STRC("Map.ESRI.ESRIMapServer.DrawMap"));
		while ((readSize = cli->Read(dataBuff, 2048)) > 0)
		{
			mstm.Write(dataBuff, readSize);
		}
		Parser::FileParser::PNGParser parser;
		UOSInt size;
		IO::StmData::MemoryDataRef mdr(mstm.GetBuff(&size), mstm.GetLength());
		ret = (Media::ImageList*)parser.ParseFile(&mdr, 0, IO::ParserType::ImageList);
	}
	DEL_CLASS(cli);
	return ret;
}

Math::Geometry::Vector2D *Map::ESRI::ESRIMapServer::ParseGeometry(UInt32 srid, Text::String *geometryType, Text::JSONBase *geometry)
{
	if (geometry == 0)
		return 0;
	if (geometryType == 0)
		return 0;
	Text::JSONBase *o;
	o = geometry->GetValue(UTF8STRC("spatialReference.wkid"));
	if (o)
	{
		srid = (UInt32)o->GetAsInt32();
	}

	if (geometryType->Equals(UTF8STRC("esriGeometryPolygon")))
	{
		o = geometry->GetValue(UTF8STRC("rings"));
		if (o && o->GetType() == Text::JSONType::Array)
		{
			Data::ArrayList<UInt32> ptOfstArr;
			Data::ArrayListA<Math::Coord2DDbl> ptArr;
			Text::JSONArray *rings = (Text::JSONArray*)o;
			UOSInt i = 0;
			UOSInt j = rings->GetArrayLength();
			while (i < j)
			{
				o = rings->GetArrayValue(i);
				if (o->GetType() == Text::JSONType::Array)
				{
					Text::JSONArray *ring = (Text::JSONArray*)o;
					ptOfstArr.Add((UInt32)ptArr.GetCount());
					UOSInt k = 0;
					UOSInt l = ring->GetArrayLength();
					while (k < l)
					{
						o = ring->GetArrayValue(k);
						if (o && o->GetType() == Text::JSONType::Array)
						{
							Text::JSONArray *pt = (Text::JSONArray*)o;
							ptArr.Add(Math::Coord2DDbl(pt->GetArrayDouble(0), pt->GetArrayDouble(1)));
						}
						k++;
					}
				}
				i++;
			}
			if (ptArr.GetCount() > 0)
			{
				Math::Geometry::Polygon *pg;
				NEW_CLASS(pg, Math::Geometry::Polygon(srid, ptOfstArr.GetCount(), ptArr.GetCount(), false, false));
				UInt32 *ptOfstList = pg->GetPtOfstList(&i);
				while (i-- > 0)
				{
					ptOfstList[i] = ptOfstArr.GetItem(i);
				}
				Math::Coord2DDbl *ptList = pg->GetPointList(&i);
				while (i-- > 0)
				{
					ptList[i] = ptArr.GetItem(i);
				}
				return pg;
			}
		}
	}
	///////////////////////////////////	
	return 0;
}
