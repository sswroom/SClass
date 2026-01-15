#include "Stdafx.h"
#include "Data/ArrayListA.hpp"
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

Map::ESRI::ESRIMapServer::ESRIMapServer(Text::CStringNN url, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Bool noResource)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UInt8 buff[2048];
	UIntOS readSize;
	UInt32 codePage;
	UIntOS i;
	UIntOS j;
	this->url = Text::String::New(url);
	this->clif = clif;
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
	if (Text::StrEndsWithC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("/MapServer")))
	{
		sptr -= 10;
		*sptr = 0;
	}
	i = Text::StrIndexOfC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("/services/"));
	if (i != INVALID_INDEX)
	{
		this->name = Text::String::NewP(&sbuff[i + 10], sptr);
	}
	else
	{
		i = Text::StrLastIndexOfCharC(sbuff, (UIntOS)(sptr - sbuff), '/');
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
		NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(clif, ssl, CSTRP(sbuff, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
		IO::MemoryStream mstm;
		while ((readSize = cli->Read(BYTEARR(buff))) > 0)
		{
			mstm.Write(Data::ByteArrayR(buff, readSize));
		}
		codePage = cli->GetContentCodePage();
		cli.Delete();


		UnsafeArray<UInt8> jsonBuff = mstm.GetBuff(readSize);
		if (readSize > 0)
		{
			Text::StringBuilderUTF8 sb;
			Text::Encoding enc(codePage);
			UIntOS charsCnt;
			UnsafeArray<UTF8Char> jsonStr;
			charsCnt = enc.CountUTF8Chars(jsonBuff, readSize);
			jsonStr = MemAllocArr(UTF8Char, charsCnt + 1);
			enc.UTF8FromBytes(jsonStr, jsonBuff, readSize, 0);
			
			NN<Text::JSONBase> json;
			if (Text::JSONBase::ParseJSONStr(Text::CStringNN(jsonStr, charsCnt)).SetTo(json))
			{
				if (json->GetType() == Text::JSONType::Object)
				{
					NN<Text::JSONObject> jobj = NN<Text::JSONObject>::ConvertFrom(json);
					NN<Text::JSONBase> o;
					NN<Text::JSONBase> v;
					NN<Text::JSONObject> vobj;
					Bool hasInit = false;
					if (jobj->GetObjectValue(CSTR("initialExtent")).SetTo(o) && o->GetType() == Text::JSONType::Object)
					{
						NN<Text::JSONObject> ext = NN<Text::JSONObject>::ConvertFrom(o);
						this->initBounds.min.x = Math::NANTo(ext->GetObjectDoubleOrNAN(CSTR("xmin")), 0);
						this->initBounds.min.y = Math::NANTo(ext->GetObjectDoubleOrNAN(CSTR("ymin")), 0);
						this->initBounds.max.x = Math::NANTo(ext->GetObjectDoubleOrNAN(CSTR("xmax")), 0);
						this->initBounds.max.y = Math::NANTo(ext->GetObjectDoubleOrNAN(CSTR("ymax")), 0);
						hasInit = true;
					}
					if (jobj->GetObjectValue(CSTR("fullExtent")).SetTo(o) && o->GetType() == Text::JSONType::Object)
					{
						NN<Text::JSONObject> ext = NN<Text::JSONObject>::ConvertFrom(o);
						this->bounds.min.x = Math::NANTo(ext->GetObjectDoubleOrNAN(CSTR("xmin")), 0);
						this->bounds.min.y = Math::NANTo(ext->GetObjectDoubleOrNAN(CSTR("ymin")), 0);
						this->bounds.max.x = Math::NANTo(ext->GetObjectDoubleOrNAN(CSTR("xmax")), 0);
						this->bounds.max.y = Math::NANTo(ext->GetObjectDoubleOrNAN(CSTR("ymax")), 0);
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

					if (jobj->GetObjectValue(CSTR("spatialReference")).SetTo(o) && o->GetType() == Text::JSONType::Object)
					{
						NN<Text::JSONObject> spRef = NN<Text::JSONObject>::ConvertFrom(o);
						UInt32 wkid = (UInt32)spRef->GetObjectInt32(CSTR("wkid"));
						NN<Math::CoordinateSystem> csys;
						if (Math::CoordinateSystemManager::SRCreateCSys(wkid).SetTo(csys))
						{
							this->csys.Delete();
							this->csys = csys;
						}
					}

					if (jobj->GetObjectValue(CSTR("capabilities")).SetTo(o) && o->GetType() == Text::JSONType::String)
					{
						sb.ClearStr();
						sb.Append(NN<Text::JSONString>::ConvertFrom(o)->GetValue());
						Text::PString sarr[2];
						sarr[1] = sb;
						UIntOS sarrCnt;
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

					if (jobj->GetObjectValue(CSTR("tileInfo")).SetTo(o) && o->GetType() == Text::JSONType::Object)
					{
						NN<Text::JSONObject> tinfo = NN<Text::JSONObject>::ConvertFrom(o);
						this->tileHeight = (UIntOS)tinfo->GetObjectInt32(CSTR("rows"));
						this->tileWidth = (UIntOS)tinfo->GetObjectInt32(CSTR("cols"));
						if (tinfo->GetObjectValue(CSTR("origin")).SetTo(v) && v->GetType() == Text::JSONType::Object)
						{
							NN<Text::JSONObject> origin = NN<Text::JSONObject>::ConvertFrom(v);
							this->tileOrigin.x = Math::NANTo(origin->GetObjectDoubleOrNAN(CSTR("x")), 0);
							this->tileOrigin.y = Math::NANTo(origin->GetObjectDoubleOrNAN(CSTR("y")), 0);
						}
						if (tinfo->GetObjectValue(CSTR("lods")).SetTo(v) && v->GetType() == Text::JSONType::Array)
						{
							NN<Text::JSONArray> levs = NN<Text::JSONArray>::ConvertFrom(v);
							i = 0;
							j = levs->GetArrayLength();
							while (i < j)
							{
								if (levs->GetArrayValue(i).SetTo(v) && v->GetType() == Text::JSONType::Object)
								{
									vobj = NN<Text::JSONObject>::ConvertFrom(v);
									Double lev = vobj->GetObjectDoubleOrNAN(CSTR("resolution"));
									if (!Math::IsNAN(lev))
										this->tileLevels.Add(lev);
								}
								i++;
							}
						}
					}
				}
			
				json->EndUse();
			}
			
			MemFreeArr(jsonStr);
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
			NN<const Math::CoordinateSystemManager::SpatialRefInfo> srinfo;
			if (Math::CoordinateSystemManager::SRGetSpatialRef(srid).SetTo(srinfo))
			{
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
			}
			else
			{
				this->bounds = Math::RectAreaDbl(Math::Coord2DDbl(-180, -90), Math::Coord2DDbl(180, 90));
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

UIntOS Map::ESRI::ESRIMapServer::TileGetLevelCount() const
{
	return this->tileLevels.GetCount();
}

Double Map::ESRI::ESRIMapServer::TileGetLevelResolution(UIntOS level) const
{
	return this->tileLevels.GetItem(level);
}

UIntOS Map::ESRI::ESRIMapServer::TileGetWidth() const
{
	return this->tileWidth;
}

UIntOS Map::ESRI::ESRIMapServer::TileGetHeight() const
{
	return this->tileHeight;
}

Math::Coord2DDbl Map::ESRI::ESRIMapServer::TileGetOrigin() const
{
	return this->tileOrigin;
}

UnsafeArray<UTF8Char> Map::ESRI::ESRIMapServer::TileGetURL(UnsafeArray<UTF8Char> sbuff, UIntOS level, Int32 tileX, Int32 tileY) const
{
	sbuff = this->url->ConcatTo(sbuff);
	sbuff = Text::StrConcatC(sbuff, UTF8STRC("/tile/"));
	sbuff = Text::StrUIntOS(sbuff, level);
	sbuff = Text::StrConcatC(sbuff, UTF8STRC("/"));
	sbuff = Text::StrInt32(sbuff, tileY);
	sbuff = Text::StrConcatC(sbuff, UTF8STRC("/"));
	sbuff = Text::StrInt32(sbuff, tileX);
	return sbuff;
}

Bool Map::ESRI::ESRIMapServer::TileGetURL(NN<Text::StringBuilderUTF8> sb, UIntOS level, Int32 tileX, Int32 tileY) const
{
	sb->Append(this->url);
	sb->AppendC(UTF8STRC("/tile/"));
	sb->AppendUIntOS(level);
	sb->AppendUTF8Char('/');
	sb->AppendI32(tileY);
	sb->AppendUTF8Char('/');
	sb->AppendI32(tileX);
	return true;
}

Bool Map::ESRI::ESRIMapServer::TileLoadToStream(NN<IO::Stream> stm, UIntOS level, Int32 tileX, Int32 tileY) const
{
	UInt8 dataBuff[2048];
	UTF8Char url[1024];
	UnsafeArray<UTF8Char> sptr;
	UIntOS readSize;
	sptr = this->TileGetURL(url, level, tileX, tileY);

	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->clif, this->ssl, CSTRP(url, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
	Bool succ = cli->GetRespStatus() == Net::WebStatus::SC_OK;
	while ((readSize = cli->Read(BYTEARR(dataBuff))) > 0)
	{
		if (readSize != stm->Write(Data::ByteArrayR(dataBuff, readSize)))
		{
			succ = false;
		}
	}
	cli.Delete();
	return succ;
}

Bool Map::ESRI::ESRIMapServer::TileLoadToFile(Text::CStringNN fileName, UIntOS level, Int32 tileX, Int32 tileY) const
{
	UInt8 dataBuff[2048];
	UIntOS readSize;
	UTF8Char url[1024];
	UnsafeArray<UTF8Char> sptr;
	sptr = this->TileGetURL(url, level, tileX, tileY);

	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->clif, this->ssl, CSTRP(url, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
	Bool succ = cli->GetRespStatus() == Net::WebStatus::SC_OK;
	if (succ)
	{
		IO::FileStream fs(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		while ((readSize = cli->Read(BYTEARR(dataBuff))) > 0)
		{
			if (readSize != fs.Write(Data::ByteArrayR(dataBuff, readSize)))
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

Bool Map::ESRI::ESRIMapServer::QueryInfos(Math::Coord2DDbl coord, Math::RectAreaDbl bounds, UInt32 width, UInt32 height, Double dpi, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayListNative<UIntOS>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList)
{
	// https://services.arcgisonline.com/arcgis/rest/services/World_Imagery/MapServer/identify?geometryType=esriGeometryPoint&geometry=114.2,22.4&sr=4326&tolerance=0&mapExtent=113,22,115,23&imageDisplay=400,300,96&f=json
	UTF8Char url[1024];
	UnsafeArray<UTF8Char> sptr;
	UInt8 dataBuff[2048];
	UIntOS readSize;
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
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->clif, this->ssl, CSTRP(url, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
	if (cli->GetRespStatus() == Net::WebStatus::SC_OK)
	{
		IO::MemoryStream mstm;
		while ((readSize = cli->Read(BYTEARR(dataBuff))) > 0)
		{
			mstm.Write(Data::ByteArrayR(dataBuff, readSize));
		}

		mstm.Write(Data::ByteArrayR(U8STR(""), 1));
		UnsafeArray<UInt8> buff = mstm.GetBuff(readSize);
		NN<Text::JSONBase> json;
		if (Text::JSONBase::ParseJSONStr(Text::CStringNN(buff, readSize - 1)).SetTo(json))
		{
			NN<Text::JSONBase> o;
			if (json->GetValue(CSTR("results")).SetTo(o) && o->GetType() == Text::JSONType::Array)
			{
				NN<Math::Geometry::Vector2D> vec;
				NN<Text::JSONArray> results = NN<Text::JSONArray>::ConvertFrom(o);
				succ = true;
				UIntOS i = 0;
				UIntOS j = results->GetArrayLength();
				while (i < j)
				{
					if (results->GetArrayValue(i).SetTo(o) && o->GetType() == Text::JSONType::Object)
					{
						NN<Text::JSONObject> result = NN<Text::JSONObject>::ConvertFrom(o);
						NN<Text::String> geometryType;
						if (result->GetValueString(CSTR("geometryType")).SetTo(geometryType))
						{
							NN<Text::JSONBase> geometryJSON;
							if (result->GetObjectValue(CSTR("geometry")).SetTo(geometryJSON) && ParseGeometry(this->csys->GetSRID(), geometryType, geometryJSON).SetTo(vec))
							{
								valueOfstList->Add(nameList->GetCount());
								if (result->GetObjectValue(CSTR("attributes")).SetTo(o) && o->GetType() == Text::JSONType::Object)
								{
									NN<Text::JSONObject> attr = NN<Text::JSONObject>::ConvertFrom(o);
									Data::ArrayListNN<Text::String> attNames;
									NN<Text::String> name;
									Text::StringBuilderUTF8 sb;
									attr->GetObjectNames(attNames);
									Data::ArrayIterator<NN<Text::String>> it = attNames.Iterator();
									while (it.HasNext())
									{
										name = it.Next();
										sb.ClearStr();
										if (attr->GetObjectValue(name->ToCString()).SetTo(o))
											o->ToString(sb);
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
	UnsafeArray<UTF8Char> sptr;
	UInt8 dataBuff[2048];
	UIntOS readSize;
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
	sptr = Text::StrUIntOS(sptr, width);
	sptr = Text::StrConcatC(sptr, UTF8STRC("%2C"));
	sptr = Text::StrUIntOS(sptr, height);
	sptr = Text::StrConcatC(sptr, UTF8STRC("&f=image"));

	NN<Text::StringBuilderUTF8> sb;
	if (sbUrl.SetTo(sb))
		sb->AppendC(url, (UIntOS)(sptr - url));

	Optional<Media::ImageList> ret = nullptr;
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->clif, this->ssl, CSTRP(url, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
	Bool succ = cli->GetRespStatus() == Net::WebStatus::SC_OK;
	if (succ)
	{
		IO::MemoryStream mstm;
		while ((readSize = cli->Read(BYTEARR(dataBuff))) > 0)
		{
			mstm.Write(Data::ByteArrayR(dataBuff, readSize));
		}
		Parser::FileParser::PNGParser parser;
		IO::StmData::MemoryDataRef mdr(mstm.GetBuff(), (UIntOS)mstm.GetLength());
		ret = Optional<Media::ImageList>::ConvertFrom(parser.ParseFile(mdr, nullptr, IO::ParserType::ImageList));
	}
	cli.Delete();
	return ret;
}

Optional<Math::Geometry::Vector2D> Map::ESRI::ESRIMapServer::ParseGeometry(UInt32 srid, NN<Text::String> geometryType, NN<Text::JSONBase> geometry)
{
	NN<Text::JSONBase> o;
	if (geometry->GetValue(CSTR("spatialReference.wkid")).SetTo(o))
	{
		srid = (UInt32)o->GetAsInt32();
	}

	if (geometryType->Equals(UTF8STRC("esriGeometryPolygon")))
	{
		if (geometry->GetValue(CSTR("rings")).SetTo(o) && o->GetType() == Text::JSONType::Array)
		{
			NN<Math::Geometry::LinearRing> lr;
			Math::Geometry::Polygon *pg;
			NEW_CLASS(pg, Math::Geometry::Polygon(srid));
			Data::ArrayListA<Math::Coord2DDbl> ptArr;
			NN<Text::JSONArray> rings = NN<Text::JSONArray>::ConvertFrom(o);
			UIntOS i = 0;
			UIntOS j = rings->GetArrayLength();
			while (i < j)
			{
				if (rings->GetArrayValue(i).SetTo(o) && o->GetType() == Text::JSONType::Array)
				{
					NN<Text::JSONArray> ring = NN<Text::JSONArray>::ConvertFrom(o);
					ptArr.Clear();
					UIntOS k = 0;
					UIntOS l = ring->GetArrayLength();
					while (k < l)
					{
						if (ring->GetArrayValue(k).SetTo(o) && o->GetType() == Text::JSONType::Array)
						{
							NN<Text::JSONArray> pt = NN<Text::JSONArray>::ConvertFrom(o);
							ptArr.Add(Math::Coord2DDbl(pt->GetArrayDoubleOrNAN(0), pt->GetArrayDoubleOrNAN(1)));
						}
						k++;
					}
					if (ptArr.GetCount() > 0)
					{
						NEW_CLASSNN(lr, Math::Geometry::LinearRing(srid, ptArr.Arr(), ptArr.GetCount(), nullptr, nullptr));
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
		if (geometry->GetValue(CSTR("paths")).SetTo(o) && o->GetType() == Text::JSONType::Array)
		{
			Data::ArrayListNative<UInt32> ptOfstArr;
			Data::ArrayListA<Math::Coord2DDbl> ptArr;
			NN<Text::JSONArray> paths = NN<Text::JSONArray>::ConvertFrom(o);
			UIntOS i = 0;
			UIntOS j = paths->GetArrayLength();
			while (i < j)
			{
				if (paths->GetArrayValue(i).SetTo(o) && o->GetType() == Text::JSONType::Array)
				{
					NN<Text::JSONArray> path = NN<Text::JSONArray>::ConvertFrom(o);
					ptOfstArr.Add((UInt32)ptArr.GetCount());
					UIntOS k = 0;
					UIntOS l = path->GetArrayLength();
					while (k < l)
					{
						if (path->GetArrayValue(k).SetTo(o) && o->GetType() == Text::JSONType::Array)
						{
							NN<Text::JSONArray> pt = NN<Text::JSONArray>::ConvertFrom(o);
							ptArr.Add(Math::Coord2DDbl(pt->GetArrayDoubleOrNAN(0), pt->GetArrayDoubleOrNAN(1)));
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
				pl->AddFromPtOfst(ptOfstArr.Arr().Ptr(), ptOfstArr.GetCount(), ptArr.Arr().Ptr(), ptArr.GetCount(), nullptr, nullptr);
				return pl;
			}
		}
	}
	else if (geometryType->Equals(UTF8STRC("esriGeometryPoint")))
	{
		NN<Text::JSONBase> x;
		NN<Text::JSONBase> y;
		NN<Text::JSONBase> z;
		if (geometry->GetValue(CSTR("x")).SetTo(x) && geometry->GetValue(CSTR("y")).SetTo(y) &&
			x->GetType() == Text::JSONType::Number && y->GetType() == Text::JSONType::Number)
		{
			Math::Geometry::Point *pt;
			if (geometry->GetValue(CSTR("z")).SetTo(z) && z->GetType() == Text::JSONType::Number)
			{
				NEW_CLASS(pt, Math::Geometry::PointZ(srid, x->GetAsDoubleOrNAN(), y->GetAsDoubleOrNAN(), z->GetAsDoubleOrNAN()));
			}
			else
			{
				NEW_CLASS(pt, Math::Geometry::Point(srid, Math::Coord2DDbl(x->GetAsDoubleOrNAN(), y->GetAsDoubleOrNAN())));
			}
			return pt;
		}
	}
	else
	{
		printf("ESRIMapServer: Unknown geometryType: %s\r\n", geometryType->v.Ptr());
	}
	///////////////////////////////////	
	return nullptr;
}

