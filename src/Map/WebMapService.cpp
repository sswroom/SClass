#include "Stdafx.h"
#include "IO/MemoryStream.h"
#include "IO/StmData/MemoryDataRef.h"
#include "Map/OWSFeatureParser.h"
#include "Map/WebMapService.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Geometry/Point.h"
#include "Net/HTTPClient.h"
#include "Parser/FileParser/BMPParser.h"
#include "Parser/FileParser/GIFParser.h"
#include "Parser/FileParser/GUIImgParser.h"
#include "Parser/FileParser/PNGParser.h"
#include "Parser/FileParser/TIFFParser.h"
#include "Text/TextBinEnc/FormEncoding.h"

//#define VERBOSE
#include <stdio.h>

// http://192.168.1.148:8080/geoserver/ows
// http://127.0.0.1:8080/geoserver/ows

void Map::WebMapService::LoadXML(Version version)
{
	Text::StringBuilderUTF8 sb;
	sb.Append(this->wmsURL);
	switch (version)
	{
	case Version::V1_1_1:
		sb.AppendC(UTF8STRC("?SERVICE=WMS&version=1.1.1&REQUEST=GetCapabilities"));
		break;
	case Version::V1_1_1_TILED:
		sb.AppendC(UTF8STRC("?SERVICE=WMS&version=1.1.1&REQUEST=GetCapabilities&tiled=true"));
		break;
	case Version::V1_3_0:
		sb.AppendC(UTF8STRC("?SERVICE=WMS&version=1.3.0&REQUEST=GetCapabilities"));
		break;
	case Version::ANY:
	default:
		sb.AppendC(UTF8STRC("?SERVICE=WMS&REQUEST=GetCapabilities"));
		break;
	}
	IO::MemoryStream mstm;
	if (!Net::HTTPClient::LoadContent(this->sockf, this->ssl, sb.ToCString(), mstm, 1048576))
		return;
	mstm.SeekFromBeginning(0);
	Text::XMLReader reader(this->encFact, mstm, Text::XMLReader::PM_XML);
	NN<Text::String> nodeName;
	if (reader.NextElementName().SetTo(nodeName))
	{
		if (nodeName->Equals(UTF8STRC("WMS_Capabilities")) || nodeName->Equals(UTF8STRC("WMT_MS_Capabilities")))
		{
			UOSInt i = reader.GetAttribCount();
			NN<Text::XMLAttrib> attr;
			while (i-- > 0)
			{
				attr = reader.GetAttribNoCheck(i);
				if (Text::String::OrEmpty(attr->name)->Equals(UTF8STRC("version")))
				{
					OPTSTR_DEL(this->version);
					this->version = Text::String::CopyOrNull(attr->value);
				}
			}
			while (reader.NextElementName().SetTo(nodeName))
			{
				if (nodeName->Equals(UTF8STRC("Service")))
				{
					reader.SkipElement();
				}
				else if (nodeName->Equals(UTF8STRC("Capability")))
				{
					while (reader.NextElementName().SetTo(nodeName))
					{
						if (nodeName->Equals(UTF8STRC("Request")))
						{
							this->LoadXMLRequest(reader);
						}
						else if (nodeName->Equals(UTF8STRC("Exception")))
						{
							reader.SkipElement();
						}
						else if (nodeName->Equals(UTF8STRC("Layer")))
						{
							this->LoadXMLLayers(reader);
						}
						else if (nodeName->Equals(UTF8STRC("UserDefinedSymbolization")))
						{
							reader.SkipElement();
						}
						else
						{
							printf("WMS: Unknown element in Capability: %s\r\n", nodeName->v.Ptr());
							reader.SkipElement();
						}
					}
				}
				else
				{
					printf("WMS: Unknown element in WMS_Capabilities: %s\r\n", nodeName->v.Ptr());
					reader.SkipElement();
				}
			}
		}
	}
	UOSInt errCode = reader.GetErrorCode();
	if (errCode != 0)
	{
		printf("WMS: XML Parse error: code = %d\r\n", (UInt32)errCode);
	}
	this->SetLayer(0);
}

void Map::WebMapService::LoadXMLRequest(NN<Text::XMLReader> reader)
{
	Text::StringBuilderUTF8 sb;
	NN<Text::String> nodeName;
	while (reader->NextElementName().SetTo(nodeName))
	{
		if (nodeName->Equals(UTF8STRC("GetCapabilities")))
		{
			reader->SkipElement();
		}
		else if (nodeName->Equals(UTF8STRC("GetMap")))
		{
			while (reader->NextElementName().SetTo(nodeName))
			{
				if (nodeName->Equals(UTF8STRC("Format")))
				{
					sb.ClearStr();
					if (reader->ReadNodeText(sb))
					{
						if (sb.Equals(UTF8STRC("image/png")))
						{
							this->mapImageType = this->mapImageTypeNames.GetCount();
						}
						this->mapImageTypeNames.Add(Text::String::New(sb.ToCString()));
					}
				}
				else
				{
					reader->SkipElement();
				}
			}
		}
		else if (nodeName->Equals(UTF8STRC("GetFeatureInfo")))
		{
			while (reader->NextElementName().SetTo(nodeName))
			{
				if (nodeName->Equals(UTF8STRC("Format")))
				{
					sb.ClearStr();
					if (reader->ReadNodeText(sb))
					{
						if (sb.Equals(UTF8STRC("application/json")))
						{
							this->infoType = this->infoTypeNames.GetCount();
						}
						this->infoTypeNames.Add(Text::String::New(sb.ToCString()));
					}
				}
				else
				{
					reader->SkipElement();
				}
			}
		}
		else if (nodeName->Equals(UTF8STRC("DescribeLayer")))
		{
			reader->SkipElement();
		}
		else if (nodeName->Equals(UTF8STRC("GetLegendGraphic")))
		{
			reader->SkipElement();
		}
		else if (nodeName->Equals(UTF8STRC("GetStyles")) || nodeName->EndsWith(UTF8STRC(":GetStyles")))
		{
			reader->SkipElement();
		}
		else
		{
			printf("WMS: Unknown element in Request: %s\r\n", nodeName->v.Ptr());
			reader->SkipElement();
		}
	}
}

void Map::WebMapService::LoadXMLLayers(NN<Text::XMLReader> reader)
{
	Text::StringBuilderUTF8 sb;
	NN<Text::String> nodeName;
	while (reader->NextElementName().SetTo(nodeName))
	{
		if (nodeName->Equals(UTF8STRC("Layer")))
		{
			Bool queryable = false;
			Text::String *layerName = 0;
			Text::String *layerTitle = 0;
			Data::ArrayListNN<LayerCRS> layerCRS;
			NN<LayerCRS> crs;
			UOSInt i;
			NN<Text::XMLAttrib> attr;
			NN<Text::String> aname;
			NN<Text::String> avalue;
			i = reader->GetAttribCount();
			while (i-- > 0)
			{
				attr = reader->GetAttribNoCheck(i);
				if (Text::String::OrEmpty(attr->name)->Equals(UTF8STRC("queryable")))
				{
					queryable = (attr->value.SetTo(avalue) && avalue->Equals(UTF8STRC("1")));
				}
			}
			while (reader->NextElementName().SetTo(nodeName))
			{
				if (nodeName->Equals(UTF8STRC("Name")))
				{
					sb.ClearStr();
					if (reader->ReadNodeText(sb))
					{
						SDEL_STRING(layerName);
						layerName = Text::String::New(sb.ToCString()).Ptr();
					}
				}
				else if (nodeName->Equals(UTF8STRC("Title")))
				{
					sb.ClearStr();
					if (reader->ReadNodeText(sb))
					{
						SDEL_STRING(layerTitle);
						layerTitle = Text::String::New(sb.ToCString()).Ptr();
					}
				}
				else if (nodeName->Equals(UTF8STRC("BoundingBox")))
				{
					crs = MemAllocANN(LayerCRS);
					crs->name = 0;
					crs->swapXY = false;
					i = reader->GetAttribCount();
					while (i-- > 0)
					{
						attr = reader->GetAttribNoCheck(i);
						if (attr->value.SetTo(avalue) && attr->name.SetTo(aname))
						{
							if (aname->Equals(UTF8STRC("minx")))
							{
								crs->bounds.min.x = avalue->ToDouble();
							}
							else if (aname->Equals(UTF8STRC("miny")))
							{
								crs->bounds.min.y = avalue->ToDouble();
							}
							else if (aname->Equals(UTF8STRC("maxx")))
							{
								crs->bounds.max.x = avalue->ToDouble();
							}
							else if (aname->Equals(UTF8STRC("maxy")))
							{
								crs->bounds.max.y = avalue->ToDouble();
							}
							else if (aname->Equals(UTF8STRC("CRS")))
							{
								SDEL_STRING(crs->name);
								crs->name = avalue->Clone().Ptr();
							}
							else if (aname->Equals(UTF8STRC("SRS")))
							{
								SDEL_STRING(crs->name);
								crs->name = avalue->Clone().Ptr();
							}
						}
					}
					reader->SkipElement();
					if (crs->name)
					{
						NN<Text::String> version;
						if (crs->name->Equals(UTF8STRC("EPSG:4326")) && this->version.SetTo(version) && version->Equals(UTF8STRC("1.3.0")))
						{
							crs->bounds.min = crs->bounds.min.SwapXY();
							crs->bounds.max = crs->bounds.max.SwapXY();
							crs->swapXY = true;
						}
						layerCRS.Add(crs);
					}
					else
					{
						MemFreeANN(crs);
					}
				}
				else
				{
					reader->SkipElement();
				}
			}
			NN<Text::String> s;
			if (s.Set(layerName) && layerTitle && layerCRS.GetCount() > 0)
			{
				NN<LayerInfo> layer;
				NEW_CLASSNN(layer, LayerInfo());
				layer->name = s;
				layer->title = layerTitle;
				layer->crsList.AddAll(layerCRS);
				layer->queryable = queryable;
				this->layers.Add(layer);
			}
			else
			{
				SDEL_STRING(layerName);
				SDEL_STRING(layerTitle);
				i = layerCRS.GetCount();
				while (i-- > 0)
				{
					crs = layerCRS.GetItemNoCheck(i);
					crs->name->Release();
					MemFreeANN(crs);
				}
			}
		}
		else
		{
			reader->SkipElement();
		}
	}
}

Map::WebMapService::WebMapService(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Optional<Text::EncodingFactory> encFact, Text::CStringNN wmsURL, Version version, NN<Math::CoordinateSystem> envCsys)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->encFact = encFact;
	this->envCsys = envCsys;
	this->wmsURL = Text::String::New(wmsURL);
	this->version = 0;
	this->infoType = 0;
	this->mapImageType = 0;
	this->currCRS = 0;
	this->layer = 0;
	this->csys = Math::CoordinateSystemManager::CreateWGS84Csys();
	this->LoadXML(version);
}

Map::WebMapService::~WebMapService()
{
	UOSInt i = this->mapImageTypeNames.GetCount();
	while (i-- > 0)
	{
		OPTSTR_DEL(this->mapImageTypeNames.GetItem(i));
	}
	i = this->infoTypeNames.GetCount();
	while (i-- > 0)
	{
		OPTSTR_DEL(this->infoTypeNames.GetItem(i));
	}
	i = this->layers.GetCount();
	NN<LayerInfo> layer;
	NN<LayerCRS> crs;
	UOSInt j;
	while (i-- > 0)
	{
		layer = this->layers.GetItemNoCheck(i);
		layer->name->Release();
		SDEL_STRING(layer->title);
		j = layer->crsList.GetCount();
		while (j-- > 0)
		{
			crs = layer->crsList.GetItemNoCheck(j);
			crs->name->Release();
			MemFreeANN(crs);
		}
		layer.Delete();
	}
	this->wmsURL->Release();
	OPTSTR_DEL(this->version);
	this->csys.Delete();
}

NN<Text::String> Map::WebMapService::GetName() const
{
	NN<LayerInfo> layer;
	if (!this->layers.GetItem(this->layer).SetTo(layer))
		return Text::String::NewEmpty();
	return layer->name;
}

NN<Math::CoordinateSystem> Map::WebMapService::GetCoordinateSystem() const
{
	return this->csys;
}

Math::RectAreaDbl Map::WebMapService::GetInitBounds() const
{
	NN<LayerCRS> currCRS;
	if (this->currCRS.SetTo(currCRS))
		return currCRS->bounds;
	return Math::RectAreaDbl(0, 0, 0, 0);
}

Bool Map::WebMapService::GetBounds(OutParam<Math::RectAreaDbl> bounds) const
{
	NN<LayerCRS> currCRS;
	if (this->currCRS.SetTo(currCRS))
	{
		bounds.Set(currCRS->bounds);
		return true;
	}
	return false;
}

Bool Map::WebMapService::CanQuery() const
{
	NN<LayerInfo> layer;
	return this->layers.GetItem(this->layer).SetTo(layer) && layer->queryable;
}

Bool Map::WebMapService::QueryInfos(Math::Coord2DDbl coord, Math::RectAreaDbl bounds, UInt32 width, UInt32 height, Double dpi, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayList<UOSInt>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList)
{
	NN<LayerInfo> layer;
	NN<Text::String> imgFormat;
	NN<Text::String> infoFormat;
	NN<LayerCRS> currCRS;
	if (!this->layers.GetItem(this->layer).SetTo(layer) || !layer->queryable || !this->infoTypeNames.GetItem(this->infoType).SetTo(infoFormat) || !this->mapImageTypeNames.GetItem(this->mapImageType).SetTo(imgFormat) || !this->currCRS.SetTo(currCRS))
		return false;

	Double x = (coord.x - bounds.min.x) * width / bounds.GetWidth();
	Double y = (bounds.max.y - coord.y) * height / bounds.GetHeight();

	if (currCRS->swapXY)
	{
		bounds.min = bounds.min.SwapXY();
		bounds.max = bounds.max.SwapXY();
	}
	Text::StringBuilderUTF8 sb;
	NN<Text::String> version;
	if (!this->version.SetTo(version))
		return false;
	if (version->Equals(UTF8STRC("1.1.1")))
	{
		sb.Append(this->wmsURL);
		sb.AppendC(UTF8STRC("?service=WMS&version=1.1.1&request=GetFeatureInfo&layers="));
		Text::TextBinEnc::FormEncoding::FormEncode(sb, layer->name->ToCString());
		sb.AppendC(UTF8STRC("&bbox="));
		sb.AppendDouble(bounds.min.x);
		sb.AppendC(UTF8STRC("%2C"));
		sb.AppendDouble(bounds.min.y);
		sb.AppendC(UTF8STRC("%2C"));
		sb.AppendDouble(bounds.max.x);
		sb.AppendC(UTF8STRC("%2C"));
		sb.AppendDouble(bounds.max.y);
		sb.AppendC(UTF8STRC("&width="));
		sb.AppendU32(width);
		sb.AppendC(UTF8STRC("&height="));
		sb.AppendU32(height);
		sb.AppendC(UTF8STRC("&srs="));
		Text::TextBinEnc::FormEncoding::FormEncode(sb, currCRS->name->ToCString());
		sb.AppendC(UTF8STRC("&styles=&format="));
		Text::TextBinEnc::FormEncoding::FormEncode(sb, imgFormat->ToCString());
		sb.AppendC(UTF8STRC("&QUERY_LAYERS="));
		Text::TextBinEnc::FormEncoding::FormEncode(sb, layer->name->ToCString());
		sb.AppendC(UTF8STRC("&INFO_FORMAT="));
		Text::TextBinEnc::FormEncoding::FormEncode(sb, infoFormat->ToCString());
		sb.AppendC(UTF8STRC("&FEATURE_COUNT=5"));
		sb.AppendC(UTF8STRC("&X="));
		sb.AppendI32(Double2Int32(x));
		sb.AppendC(UTF8STRC("&Y="));
		sb.AppendI32(Double2Int32(y));
	}
	else if (version->Equals(UTF8STRC("1.3.0")))
	{
		sb.Append(this->wmsURL);
		sb.AppendC(UTF8STRC("?service=WMS&version=1.3.0&request=GetFeatureInfo&layers="));
		Text::TextBinEnc::FormEncoding::FormEncode(sb, layer->name->ToCString());
		sb.AppendC(UTF8STRC("&bbox="));
		sb.AppendDouble(bounds.min.x);
		sb.AppendC(UTF8STRC("%2C"));
		sb.AppendDouble(bounds.min.y);
		sb.AppendC(UTF8STRC("%2C"));
		sb.AppendDouble(bounds.max.x);
		sb.AppendC(UTF8STRC("%2C"));
		sb.AppendDouble(bounds.max.y);
		sb.AppendC(UTF8STRC("&width="));
		sb.AppendU32(width);
		sb.AppendC(UTF8STRC("&height="));
		sb.AppendU32(height);
		sb.AppendC(UTF8STRC("&CRS="));
		Text::TextBinEnc::FormEncoding::FormEncode(sb, currCRS->name->ToCString());
		sb.AppendC(UTF8STRC("&styles=&format="));
		Text::TextBinEnc::FormEncoding::FormEncode(sb, imgFormat->ToCString());
		sb.AppendC(UTF8STRC("&QUERY_LAYERS="));
		Text::TextBinEnc::FormEncoding::FormEncode(sb, layer->name->ToCString());
		sb.AppendC(UTF8STRC("&INFO_FORMAT="));
		Text::TextBinEnc::FormEncoding::FormEncode(sb, infoFormat->ToCString());
		sb.AppendC(UTF8STRC("&FEATURE_COUNT=5"));
		sb.AppendC(UTF8STRC("&I="));
		sb.AppendI32(Double2Int32(x));
		sb.AppendC(UTF8STRC("&J="));
		sb.AppendI32(Double2Int32(y));
	}
	else
	{
		return false;
	}
#if defined(VERBOSE)
	printf("WebMapService: Query URL: %s\r\n", sb.ToString());
#endif

	UInt8 dataBuff[2048];
	UOSInt readSize;
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, true);
	if (cli->GetRespStatus() == Net::WebStatus::SC_OK)
	{
		Text::StringBuilderUTF8 sbData;
		while ((readSize = cli->Read(BYTEARR(dataBuff))) > 0)
		{
			sbData.AppendC(dataBuff, readSize);
		}
		cli.Delete();

		if (infoFormat->Equals(UTF8STRC("application/json")) || infoFormat->Equals(UTF8STRC("application/geo+json")))
		{
			return Map::OWSFeatureParser::ParseJSON(sbData.ToCString(), this->csys->GetSRID(), vecList, valueOfstList, nameList, valueList, coord);
		}
		else if (infoFormat->StartsWith(UTF8STRC("application/vnd.ogc.gml")))
		{
			return Map::OWSFeatureParser::ParseGML(sbData.ToCString(), this->csys->GetSRID(), currCRS->swapXY, this->encFact, vecList, valueOfstList, nameList, valueList);
		}
		else if (infoFormat->StartsWith(UTF8STRC("text/xml")))
		{
			return Map::OWSFeatureParser::ParseGML(sbData.ToCString(), this->csys->GetSRID(), currCRS->swapXY, this->encFact, vecList, valueOfstList, nameList, valueList) ||
				Map::OWSFeatureParser::ParseOGC_WMS_XML(sbData.ToCString(), this->csys->GetSRID(), coord, this->encFact, vecList, valueOfstList, nameList, valueList);
		}
		else if (infoFormat->StartsWith(UTF8STRC("application/vnd.esri.wms_raw_xml")) || infoFormat->Equals(UTF8STRC("application/vnd.esri.wms_featureinfo_xml")))
		{
			return Map::OWSFeatureParser::ParseESRI_WMS_XML(sbData.ToCString(), this->csys->GetSRID(), currCRS->swapXY, this->encFact, vecList, valueOfstList, nameList, valueList, coord);
		}
		else if (infoFormat->StartsWith(UTF8STRC("application/vnd.ogc.wms_xml")))
		{
			return Map::OWSFeatureParser::ParseOGC_WMS_XML(sbData.ToCString(), this->csys->GetSRID(), coord, this->encFact, vecList, valueOfstList, nameList, valueList);
		}
		else if (infoFormat->Equals(UTF8STRC("text/plain")))
		{
			return Map::OWSFeatureParser::ParseText(sbData, this->csys->GetSRID(), coord, vecList, valueOfstList, nameList, valueList);
		}
		else
		{
			printf("WebMapService: Query URL: %s\r\n", sb.ToPtr());
			printf("%s\r\n", sbData.ToPtr());
		}
		return false;
	}
	cli.Delete();
	return false;
}

Optional<Media::ImageList> Map::WebMapService::DrawMap(Math::RectAreaDbl bounds, UInt32 width, UInt32 height, Double dpi, Optional<Text::StringBuilderUTF8> sbUrl)
{
	Text::StringBuilderUTF8 sb;
	NN<Text::String> imgFormat;
	NN<LayerInfo> layer;
	NN<LayerCRS> currCRS;
	if (!this->layers.GetItem(this->layer).SetTo(layer) || !this->mapImageTypeNames.GetItem(this->mapImageType).SetTo(imgFormat) || !this->currCRS.SetTo(currCRS))
		return 0;
	if (currCRS->swapXY)
	{
		bounds.min = bounds.min.SwapXY();
		bounds.max = bounds.max.SwapXY();
	}
	NN<Text::String> version = Text::String::OrEmpty(this->version);
	if (version->Equals(UTF8STRC("1.1.1")))
	{
		// http://127.0.0.1:8080/geoserver/Dev/wms?service=WMS&version=1.1.0&request=GetMap&layers=Dev%3Athreed_burial_poly&bbox=113.9587574553149%2C22.34255390361735%2C114.1047088037185%2C22.3992408177216&width=768&height=330&srs=EPSG%3A4326&styles=&format=image%2Fpng
		sb.Append(this->wmsURL);
		sb.AppendC(UTF8STRC("?service=WMS&version=1.1.1&request=GetMap&layers="));
		Text::TextBinEnc::FormEncoding::FormEncode(sb, layer->name->ToCString());
		sb.AppendC(UTF8STRC("&bbox="));
		sb.AppendDouble(bounds.min.x);
		sb.AppendC(UTF8STRC("%2C"));
		sb.AppendDouble(bounds.min.y);
		sb.AppendC(UTF8STRC("%2C"));
		sb.AppendDouble(bounds.max.x);
		sb.AppendC(UTF8STRC("%2C"));
		sb.AppendDouble(bounds.max.y);
		sb.AppendC(UTF8STRC("&width="));
		sb.AppendU32(width);
		sb.AppendC(UTF8STRC("&height="));
		sb.AppendU32(height);
		sb.AppendC(UTF8STRC("&TRANSPARENT=TRUE"));
		sb.AppendC(UTF8STRC("&srs="));
		Text::TextBinEnc::FormEncoding::FormEncode(sb, currCRS->name->ToCString());
		sb.AppendC(UTF8STRC("&styles=&format="));
		Text::TextBinEnc::FormEncoding::FormEncode(sb, imgFormat->ToCString());
	}
	else if (version->Equals(UTF8STRC("1.3.0")))
	{
		sb.Append(this->wmsURL);
		sb.AppendC(UTF8STRC("?service=WMS&version=1.3.0&request=GetMap&layers="));
		Text::TextBinEnc::FormEncoding::FormEncode(sb, layer->name->ToCString());
		sb.AppendC(UTF8STRC("&bbox="));
		sb.AppendDouble(bounds.min.x);
		sb.AppendC(UTF8STRC("%2C"));
		sb.AppendDouble(bounds.min.y);
		sb.AppendC(UTF8STRC("%2C"));
		sb.AppendDouble(bounds.max.x);
		sb.AppendC(UTF8STRC("%2C"));
		sb.AppendDouble(bounds.max.y);
		sb.AppendC(UTF8STRC("&width="));
		sb.AppendU32(width);
		sb.AppendC(UTF8STRC("&height="));
		sb.AppendU32(height);
		sb.AppendC(UTF8STRC("&TRANSPARENT=TRUE"));
		sb.AppendC(UTF8STRC("&CRS="));
		Text::TextBinEnc::FormEncoding::FormEncode(sb, currCRS->name->ToCString());
		sb.AppendC(UTF8STRC("&styles=&format="));
		Text::TextBinEnc::FormEncoding::FormEncode(sb, imgFormat->ToCString());
	}
	NN<Text::StringBuilderUTF8> nnsb;
	if (sbUrl.SetTo(nnsb))
	{
		nnsb->Append(sb);
	}

#if defined(VERBOSE)
	printf("URL: %s\r\n", sb.ToString());
#endif
	UInt8 dataBuff[2048];
	UOSInt readSize;
	Optional<Media::ImageList> ret = 0;
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, true);
	Bool succ = cli->GetRespStatus() == Net::WebStatus::SC_OK;
	if (succ)
	{
		IO::MemoryStream mstm;
		while ((readSize = cli->Read(BYTEARR(dataBuff))) > 0)
		{
			mstm.Write(Data::ByteArrayR(dataBuff, readSize));
		}
		IO::StmData::MemoryDataRef mdr(mstm.GetBuff(), (UOSInt)mstm.GetLength());
		if (imgFormat->StartsWith(UTF8STRC("image/png")))
		{
			Parser::FileParser::PNGParser parser;
			ret = Optional<Media::ImageList>::ConvertFrom(parser.ParseFile(mdr, 0, IO::ParserType::ImageList));
		}
		else if (imgFormat->StartsWith(UTF8STRC("image/geotiff")) || imgFormat->StartsWith(UTF8STRC("image/tiff")))
		{
			Parser::FileParser::TIFFParser parser;
			ret = Optional<Media::ImageList>::ConvertFrom(parser.ParseFile(mdr, 0, IO::ParserType::ImageList));
		}
		else if (imgFormat->Equals(UTF8STRC("image/gif")))
		{
			Parser::FileParser::GIFParser parser;
			ret = Optional<Media::ImageList>::ConvertFrom(parser.ParseFile(mdr, 0, IO::ParserType::ImageList));
		}
		else if (imgFormat->Equals(UTF8STRC("image/jpeg")))
		{
			Parser::FileParser::GUIImgParser parser;
			ret = Optional<Media::ImageList>::ConvertFrom(parser.ParseFile(mdr, 0, IO::ParserType::ImageList));
		}
		else if (imgFormat->Equals(UTF8STRC("image/bmp")))
		{
			Parser::FileParser::BMPParser parser;
			ret = Optional<Media::ImageList>::ConvertFrom(parser.ParseFile(mdr, 0, IO::ParserType::ImageList));
		}
		else
		{
			printf("WMS: Unsupported image format: %s\r\n", imgFormat->v.Ptr());
		}
	}
	else
	{
		printf("WMS: Failed in getting Image, URL: %s\r\n", sb.v.Ptr());
	}
	cli.Delete();
	return ret;
}

Bool Map::WebMapService::IsError() const
{
	return this->version.IsNull() || this->layers.GetCount() == 0 || this->mapImageTypeNames.GetCount() == 0;
}

UOSInt Map::WebMapService::GetInfoType() const
{
	return this->infoType;
}

UOSInt Map::WebMapService::GetLayerCRS() const
{
	NN<LayerCRS> currCRS;
	if (!this->currCRS.SetTo(currCRS))
		return INVALID_INDEX;
	NN<LayerInfo> layer;
	if (this->layers.GetItem(this->layer).SetTo(layer))
	{
		return layer->crsList.IndexOf(currCRS);
	}
	return INVALID_INDEX;
}
		
void Map::WebMapService::SetInfoType(UOSInt index)
{
	if (index < this->infoTypeNames.GetCount())
	{
		this->infoType = index;
	}
}

void Map::WebMapService::SetLayer(UOSInt index)
{
	if (index < this->layers.GetCount())
	{
		this->layer = index;
		NN<Math::CoordinateSystem> csys;
		NN<LayerInfo> layer = this->layers.GetItemNoCheck(index);
		NN<LayerCRS> crs;
		Bool found = false;
		UOSInt i = 0;
		UOSInt j = layer->crsList.GetCount();
		while (i < j)
		{
			crs = layer->crsList.GetItemNoCheck(i);
			if (Math::CoordinateSystemManager::CreateFromName(crs->name->ToCString()).SetTo(csys))
			{
				if (csys->Equals(this->envCsys))
				{
					csys.Delete();
					found = true;
					this->currCRS = crs;
					break;
				}
				else if (csys->GetCoordSysType() == this->envCsys->GetCoordSysType())
				{
					csys.Delete();
					found = true;
					this->currCRS = crs;
					break;
				}
				csys.Delete();
			}
			i++;
		}
		if (!found)
		{
			this->currCRS = layer->crsList.GetItem(0);
		}
		if (this->currCRS.SetTo(crs))
		{
			this->csys.Delete();
			this->csys = Math::CoordinateSystemManager::CreateFromNameOrDef(crs->name->ToCString());
		}
	}
}

void Map::WebMapService::SetMapImageType(UOSInt index)
{
	if (index < this->mapImageTypeNames.GetCount())
	{
		this->mapImageType = index;
	}
}

void Map::WebMapService::SetLayerCRS(UOSInt index)
{
	NN<LayerInfo> layer;
	if (this->layers.GetItem(this->layer).SetTo(layer) && index < layer->crsList.GetCount())
	{
		NN<LayerCRS> crs = layer->crsList.GetItemNoCheck(index);
		this->currCRS = crs;
		this->csys.Delete();
		this->csys = Math::CoordinateSystemManager::CreateFromNameOrDef(crs->name->ToCString());
	}
}

UOSInt Map::WebMapService::GetLayerNames(NN<Data::ArrayListStringNN> nameList) const
{
	UOSInt i = 0;
	UOSInt j = this->layers.GetCount();
	while (i < j)
	{
		nameList->Add(this->layers.GetItemNoCheck(i)->name);
		i++;
	}
	return j;
}

UOSInt Map::WebMapService::GetMapImageTypeNames(NN<Data::ArrayListStringNN> nameList) const
{
	return nameList->AddAll(this->mapImageTypeNames);
}

UOSInt Map::WebMapService::GetInfoTypeNames(NN<Data::ArrayListStringNN> nameList) const
{
	return nameList->AddAll(this->infoTypeNames);
}

UOSInt Map::WebMapService::GetLayerCRSNames(NN<Data::ArrayListNN<Text::String>> nameList) const
{
	NN<LayerInfo> layer;
	if (!this->layers.GetItem(this->layer).SetTo(layer))
		return 0;
	UOSInt i = 0;
	UOSInt j = layer->crsList.GetCount();
	while (i < j)
	{
		nameList->Add(Text::String::OrEmpty(layer->crsList.GetItemNoCheck(i)->name));
		i++;
	}
	return j;
}
