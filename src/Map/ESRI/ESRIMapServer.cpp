#include "Stdafx.h"
#include "Data/ArrayListA.h"
#include "IO/FileStream.h"
#include "IO/StmData/MemoryDataRef.h"
#include "Map/ESRI/ESRIMapServer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Geometry/PointZ.h"
#include "Math/Geometry/Polygon.h"
#include "Math/Geometry/Polyline.h"
#include "Net/HTTPClient.h"
#include "Parser/FileParser/PNGParser.h"
#include "Text/Encoding.h"
#include "Text/JSON.h"

#include <stdio.h>

Map::ESRI::ESRIMapServer::ESRIMapServer(Text::CString url, NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Bool noResource)
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
	this->noResource = noResource;
	this->csys = Math::CoordinateSystemManager::CreateWGS84Csys();

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

	if (noResource)
	{
		this->supportMap = true;
		this->supportQuery = true;
	}
	else
	{
		sptr = Text::StrConcatC(url.ConcatTo(sbuff), UTF8STRC("?f=json"));
		NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(sockf, ssl, CSTRP(sbuff, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
		IO::MemoryStream mstm;
		while ((readSize = cli->Read(BYTEARR(buff))) > 0)
		{
			mstm.Write(buff, readSize);
		}
		codePage = cli->GetContentCodePage();
		cli.Delete();


		UInt8 *jsonBuff = mstm.GetBuff(readSize);
		if (jsonBuff && readSize > 0)
		{
			Text::StringBuilderUTF8 sb;
			Text::Encoding enc(codePage);
			UOSInt charsCnt;
			UTF8Char *jsonStr;
			charsCnt = enc.CountUTF8Chars(jsonBuff, readSize);
			jsonStr = MemAlloc(UTF8Char, charsCnt + 1);
			enc.UTF8FromBytes(jsonStr, jsonBuff, readSize, 0);
			
			Text::JSONBase *json = Text::JSONBase::ParseJSONStr(Text::CStringNN(jsonStr, charsCnt));
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
						this->initBounds.min.x = ext->GetObjectDouble(CSTR("xmin"));
						this->initBounds.min.y = ext->GetObjectDouble(CSTR("ymin"));
						this->initBounds.max.x = ext->GetObjectDouble(CSTR("xmax"));
						this->initBounds.max.y = ext->GetObjectDouble(CSTR("ymax"));
						hasInit = true;
					}
					o = jobj->GetObjectValue(CSTR("fullExtent"));
					if (o != 0 && o->GetType() == Text::JSONType::Object)
					{
						Text::JSONObject *ext = (Text::JSONObject*)o;
						this->bounds.min.x = ext->GetObjectDouble(CSTR("xmin"));
						this->bounds.min.y = ext->GetObjectDouble(CSTR("ymin"));
						this->bounds.max.x = ext->GetObjectDouble(CSTR("xmax"));
						this->bounds.max.y = ext->GetObjectDouble(CSTR("ymax"));
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
						NN<Math::CoordinateSystem> csys;
						if (Math::CoordinateSystemManager::SRCreateCSys(wkid).SetTo(csys))
						{
							this->csys.Delete();
							this->csys = csys;
						}
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
}

Map::ESRI::ESRIMapServer::~ESRIMapServer()
{
	this->url->Release();
	this->csys.Delete();
	this->name->Release();
}

Bool Map::ESRI::ESRIMapServer::IsError() const
{
	return (this->bounds.min.x == this->bounds.max.x || this->bounds.min.y == this->bounds.max.y);
}

Bool Map::ESRI::ESRIMapServer::HasTile() const
{
	if (this->tileWidth == 0 || this->tileHeight == 0 || this->tileLevels.GetCount() == 0)
		return false;
	return true;
}

void Map::ESRI::ESRIMapServer::SetSRID(UInt32 srid)
{
	if (this->noResource)
	{
		NN<Math::CoordinateSystem> csys;
		if (Math::CoordinateSystemManager::SRCreateCSys(srid).SetTo(csys))
		{
			this->csys.Delete();
			this->csys = csys;
			const Math::CoordinateSystemManager::SpatialRefInfo *srinfo = Math::CoordinateSystemManager::SRGetSpatialRef(srid);
			if (csys->IsProjected())
			{
				NN<Math::CoordinateSystem> wgs84Csys = Math::CoordinateSystemManager::CreateWGS84Csys();
				Math::Coord2DDbl tl = Math::CoordinateSystem::Convert(wgs84Csys, csys, Math::Coord2DDbl(srinfo->minXGeo, srinfo->minYGeo));
				Math::Coord2DDbl br = Math::CoordinateSystem::Convert(wgs84Csys, csys, Math::Coord2DDbl(srinfo->maxXGeo, srinfo->maxYGeo));
				this->bounds = Math::RectAreaDbl(tl, br);
				wgs84Csys.Delete();
			}
			else
			{
				this->bounds = Math::RectAreaDbl(Math::Coord2DDbl(srinfo->minXGeo, srinfo->minYGeo), Math::Coord2DDbl(srinfo->maxXGeo, srinfo->maxYGeo));
			}
			this->initBounds = this->bounds;
		}
	}
}

NN<Text::String> Map::ESRI::ESRIMapServer::GetURL() const
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

Bool Map::ESRI::ESRIMapServer::TileGetURL(NN<Text::StringBuilderUTF8> sb, UOSInt level, Int32 tileX, Int32 tileY) const
{
	sb->Append(this->url);
	sb->AppendC(UTF8STRC("/tile/"));
	sb->AppendUOSInt(level);
	sb->AppendUTF8Char('/');
	sb->AppendI32(tileY);
	sb->AppendUTF8Char('/');
	sb->AppendI32(tileX);
	return true;
}

Bool Map::ESRI::ESRIMapServer::TileLoadToStream(IO::Stream *stm, UOSInt level, Int32 tileX, Int32 tileY) const
{
	UInt8 dataBuff[2048];
	UTF8Char url[1024];
	UTF8Char *sptr;
	UOSInt readSize;
	sptr = this->TileGetURL(url, level, tileX, tileY);

	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, CSTRP(url, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
	Bool succ = cli->GetRespStatus() == Net::WebStatus::SC_OK;
	while ((readSize = cli->Read(BYTEARR(dataBuff))) > 0)
	{
		if (readSize != stm->Write(dataBuff, readSize))
		{
			succ = false;
		}
	}
	cli.Delete();
	return succ;
}

Bool Map::ESRI::ESRIMapServer::TileLoadToFile(Text::CStringNN fileName, UOSInt level, Int32 tileX, Int32 tileY) const
{
	UInt8 dataBuff[2048];
	UOSInt readSize;
	UTF8Char url[1024];
	UTF8Char *sptr;
	sptr = this->TileGetURL(url, level, tileX, tileY);

	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, CSTRP(url, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
	Bool succ = cli->GetRespStatus() == Net::WebStatus::SC_OK;
	if (succ)
	{
		IO::FileStream fs(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		while ((readSize = cli->Read(BYTEARR(dataBuff))) > 0)
		{
			if (readSize != fs.Write(dataBuff, readSize))
			{
				succ = false;
			}
		}
	}
	cli.Delete();
	return succ;
}

NN<Text::String> Map::ESRI::ESRIMapServer::GetName() const
{
	return this->name;
}

NN<Math::CoordinateSystem> Map::ESRI::ESRIMapServer::GetCoordinateSystem() const
{
	return this->csys;
}

Math::RectAreaDbl Map::ESRI::ESRIMapServer::GetInitBounds() const
{
	return this->initBounds;
}

Bool Map::ESRI::ESRIMapServer::GetBounds(OutParam<Math::RectAreaDbl> bounds) const
{
	bounds.Set(this->bounds);
	return true;
}

Bool Map::ESRI::ESRIMapServer::CanQuery() const
{
	return true;
}

Bool Map::ESRI::ESRIMapServer::QueryInfos(Math::Coord2DDbl coord, Math::RectAreaDbl bounds, UInt32 width, UInt32 height, Double dpi, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayList<UOSInt>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList)
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
	Double tolerance = 5 * dpi / 96.0;
	sptr = Text::StrConcatC(sptr, UTF8STRC("&tolerance="));
	sptr = Text::StrInt32(sptr, Double2Int32(tolerance));
	sptr = Text::StrConcatC(sptr, UTF8STRC("&mapExtent="));
	sptr = Text::StrDouble(sptr, bounds.min.x);
	*sptr++ = ',';
	sptr = Text::StrDouble(sptr, bounds.min.y);
	*sptr++ = ',';
	sptr = Text::StrDouble(sptr, bounds.max.x);
	*sptr++ = ',';
	sptr = Text::StrDouble(sptr, bounds.max.y);
	sptr = Text::StrConcatC(sptr, UTF8STRC("&imageDisplay="));
	sptr = Text::StrUInt32(sptr, width);
	*sptr++ = ',';
	sptr = Text::StrUInt32(sptr, height);
	*sptr++ = ',';
	sptr = Text::StrInt32(sptr, Double2Int32(dpi));
	sptr = Text::StrConcatC(sptr, UTF8STRC("&f=json"));

	Bool succ = false;
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, CSTRP(url, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
	if (cli->GetRespStatus() == Net::WebStatus::SC_OK)
	{
		IO::MemoryStream mstm;
		while ((readSize = cli->Read(BYTEARR(dataBuff))) > 0)
		{
			mstm.Write(dataBuff, readSize);
		}

		mstm.Write((const UInt8*)"", 1);
		UInt8 *buff = mstm.GetBuff(readSize);
		Text::JSONBase *json = Text::JSONBase::ParseJSONStr(Text::CStringNN(buff, readSize - 1));
		if (json)
		{
			Text::JSONBase *o = json->GetValue(CSTR("results"));
			if (o && o->GetType() == Text::JSONType::Array)
			{
				NN<Math::Geometry::Vector2D> vec;
				Text::JSONArray *results = (Text::JSONArray*)o;
				succ = true;
				UOSInt i = 0;
				UOSInt j = results->GetArrayLength();
				while (i < j)
				{
					o = results->GetArrayValue(i);
					if (o && o->GetType() == Text::JSONType::Object)
					{
						Text::JSONObject *result = (Text::JSONObject*)o;
						NN<Text::String> geometryType;
						if (result->GetValueString(CSTR("geometryType")).SetTo(geometryType))
						{
							NN<Text::JSONBase> geometryJSON;
							if (geometryJSON.Set(result->GetObjectValue(CSTR("geometry"))) && ParseGeometry(this->csys->GetSRID(), geometryType, geometryJSON).SetTo(vec))
							{
								valueOfstList->Add(nameList->GetCount());
								o = result->GetObjectValue(CSTR("attributes"));
								if (o && o->GetType() == Text::JSONType::Object)
								{
									Text::JSONObject *attr = (Text::JSONObject*)o;
									Data::ArrayListNN<Text::String> attNames;
									NN<Text::String> name;
									Text::StringBuilderUTF8 sb;
									attr->GetObjectNames(attNames);
									Data::ArrayIterator<NN<Text::String>> it = attNames.Iterator();
									while (it.HasNext())
									{
										name = it.Next();
										sb.ClearStr();
										attr->GetObjectValue(name->ToCString())->ToString(sb);
										nameList->Add(name->Clone());
										valueList->Add(Text::String::New(sb.ToCString()));
									}
								}
								vecList->Add(vec);
							}
							else
							{
								printf("ESRIMapServer: URL: %s\r\n", url);
							}
						}
					}
					i++;
				}
			}
			json->EndUse();
		}
		succ = vecList->GetCount() > 0;
	}
	cli.Delete();
	return succ;
}

Optional<Media::ImageList> Map::ESRI::ESRIMapServer::DrawMap(Math::RectAreaDbl bounds, UInt32 width, UInt32 height, Double dpi, Optional<Text::StringBuilderUTF8> sbUrl)
{
	UTF8Char url[1024];
	UTF8Char *sptr;
	UInt8 dataBuff[2048];
	UOSInt readSize;
	UInt32 srid = 0;
	srid = this->csys->GetSRID();
	sptr = this->url->ConcatTo(url);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/export?dpi="));
	sptr = Text::StrDouble(sptr, dpi);
	sptr = Text::StrConcatC(sptr, UTF8STRC("&transparent=true&format=png8&bbox="));
	sptr = Text::StrDouble(sptr, bounds.min.x);
	sptr = Text::StrConcatC(sptr, UTF8STRC("%2C"));
	sptr = Text::StrDouble(sptr, bounds.min.y);
	sptr = Text::StrConcatC(sptr, UTF8STRC("%2C"));
	sptr = Text::StrDouble(sptr, bounds.max.x);
	sptr = Text::StrConcatC(sptr, UTF8STRC("%2C"));
	sptr = Text::StrDouble(sptr, bounds.max.y);
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

	NN<Text::StringBuilderUTF8> sb;
	if (sbUrl.SetTo(sb))
		sb->AppendC(url, (UOSInt)(sptr - url));

	Optional<Media::ImageList> ret = 0;
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, CSTRP(url, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
	Bool succ = cli->GetRespStatus() == Net::WebStatus::SC_OK;
	if (succ)
	{
		IO::MemoryStream mstm;
		while ((readSize = cli->Read(BYTEARR(dataBuff))) > 0)
		{
			mstm.Write(dataBuff, readSize);
		}
		Parser::FileParser::PNGParser parser;
		IO::StmData::MemoryDataRef mdr(mstm.GetBuff(), (UOSInt)mstm.GetLength());
		ret = Optional<Media::ImageList>::ConvertFrom(parser.ParseFile(mdr, 0, IO::ParserType::ImageList));
	}
	cli.Delete();
	return ret;
}

Optional<Math::Geometry::Vector2D> Map::ESRI::ESRIMapServer::ParseGeometry(UInt32 srid, NN<Text::String> geometryType, NN<Text::JSONBase> geometry)
{
	Text::JSONBase *o;
	o = geometry->GetValue(CSTR("spatialReference.wkid"));
	if (o)
	{
		srid = (UInt32)o->GetAsInt32();
	}

	if (geometryType->Equals(UTF8STRC("esriGeometryPolygon")))
	{
		o = geometry->GetValue(CSTR("rings"));
		if (o && o->GetType() == Text::JSONType::Array)
		{
			NN<Math::Geometry::LinearRing> lr;
			Math::Geometry::Polygon *pg;
			NEW_CLASS(pg, Math::Geometry::Polygon(srid));
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
					ptArr.Clear();
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
					if (ptArr.GetCount() > 0)
					{
						NEW_CLASSNN(lr, Math::Geometry::LinearRing(srid, ptArr.Arr(), ptArr.GetCount(), 0, 0));
						pg->AddGeometry(lr);
					}
				}
				i++;
			}
			if (pg->GetCount() > 0)
			{
				return pg;
			}
			DEL_CLASS(pg);
		}
	}
	else if (geometryType->Equals(UTF8STRC("esriGeometryPolyline")))
	{
		o = geometry->GetValue(CSTR("paths"));
		if (o && o->GetType() == Text::JSONType::Array)
		{
			Data::ArrayList<UInt32> ptOfstArr;
			Data::ArrayListA<Math::Coord2DDbl> ptArr;
			Text::JSONArray *paths = (Text::JSONArray*)o;
			UOSInt i = 0;
			UOSInt j = paths->GetArrayLength();
			while (i < j)
			{
				o = paths->GetArrayValue(i);
				if (o->GetType() == Text::JSONType::Array)
				{
					Text::JSONArray *path = (Text::JSONArray*)o;
					ptOfstArr.Add((UInt32)ptArr.GetCount());
					UOSInt k = 0;
					UOSInt l = path->GetArrayLength();
					while (k < l)
					{
						o = path->GetArrayValue(k);
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
				Math::Geometry::Polyline *pl;
				NEW_CLASS(pl, Math::Geometry::Polyline(srid));
				pl->AddFromPtOfst(ptOfstArr.Arr().Ptr(), ptOfstArr.GetCount(), ptArr.Arr().Ptr(), ptArr.GetCount(), 0, 0);
				return pl;
			}
		}
	}
	else if (geometryType->Equals(UTF8STRC("esriGeometryPoint")))
	{
		Text::JSONBase *x = geometry->GetValue(CSTR("x"));
		Text::JSONBase *y = geometry->GetValue(CSTR("y"));
		Text::JSONBase *z = geometry->GetValue(CSTR("z"));
		if (x && y && x->GetType() == Text::JSONType::Number && y->GetType() == Text::JSONType::Number)
		{
			Math::Geometry::Point *pt;
			if (z && z->GetType() == Text::JSONType::Number)
			{
				NEW_CLASS(pt, Math::Geometry::PointZ(srid, x->GetAsDouble(), y->GetAsDouble(), z->GetAsDouble()));
			}
			else
			{
				NEW_CLASS(pt, Math::Geometry::Point(srid, Math::Coord2DDbl(x->GetAsDouble(), y->GetAsDouble())));
			}
			return pt;
		}
	}
	else
	{
		printf("ESRIMapServer: Unknown geometryType: %s\r\n", geometryType->v);
	}
	///////////////////////////////////	
	return 0;
}

