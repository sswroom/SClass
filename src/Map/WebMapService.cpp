#include "Stdafx.h"
#include "IO/MemoryStream.h"
#include "IO/StmData/MemoryDataRef.h"
#include "Map/OWSFeatureParser.h"
#include "Map/WebMapService.h"
#include "Math/CoordinateSystemManager.h"
#include "Net/HTTPClient.h"
#include "Parser/FileParser/GIFParser.h"
#include "Parser/FileParser/GUIImgParser.h"
#include "Parser/FileParser/PNGParser.h"
#include "Parser/FileParser/TIFFParser.h"
#include "Text/TextBinEnc/FormEncoding.h"

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
	IO::MemoryStream mstm(UTF8STRC("Map.WebMapService.LoadXML.mstm"));
	if (!Net::HTTPClient::LoadContent(this->sockf, this->ssl, sb.ToCString(), &mstm, 1048576))
		return;
	mstm.SeekFromBeginning(0);
	Text::XMLReader reader(this->encFact, &mstm, Text::XMLReader::PM_XML);
	while (reader.ReadNext())
	{
		if (reader.GetNodeType() == Text::XMLNode::NodeType::Element)
		{
			Text::String *nodeName = reader.GetNodeText();
			if (nodeName->Equals(UTF8STRC("WMS_Capabilities")) || nodeName->Equals(UTF8STRC("WMT_MS_Capabilities")))
			{
				UOSInt i = reader.GetAttribCount();
				Text::XMLAttrib *attr;
				while (i-- > 0)
				{
					attr = reader.GetAttrib(i);
					if (attr->name->Equals(UTF8STRC("version")))
					{
						SDEL_STRING(this->version);
						this->version = SCOPY_STRING(attr->value);
					}
				}
				while (reader.ReadNext())
				{
					Text::XMLNode::NodeType nodeType = reader.GetNodeType();
					if (nodeType == Text::XMLNode::NodeType::ElementEnd)
					{
						break;
					}
					else if (nodeType == Text::XMLNode::NodeType::Element)
					{
						nodeName = reader.GetNodeText();
						if (nodeName->Equals(UTF8STRC("Service")))
						{
							reader.SkipElement();
						}
						else if (nodeName->Equals(UTF8STRC("Capability")))
						{
							while (reader.ReadNext())
							{
								nodeType = reader.GetNodeType();
								if (nodeType == Text::XMLNode::NodeType::ElementEnd)
								{
									break;
								}
								else if (nodeType == Text::XMLNode::NodeType::Element)
								{
									nodeName = reader.GetNodeText();
									if (nodeName->Equals(UTF8STRC("Request")))
									{
										this->LoadXMLRequest(&reader);
									}
									else if (nodeName->Equals(UTF8STRC("Exception")))
									{
										reader.SkipElement();
									}
									else if (nodeName->Equals(UTF8STRC("Layer")))
									{
										this->LoadXMLLayers(&reader);
									}
									else if (nodeName->Equals(UTF8STRC("UserDefinedSymbolization")))
									{
										reader.SkipElement();
									}
									else
									{
										printf("WMS: Unknown element in Capability: %s\r\n", nodeName->v);
										reader.SkipElement();
									}
								}
							}
						}
						else
						{
							printf("WMS: Unknown element in WMS_Capabilities: %s\r\n", nodeName->v);
							reader.SkipElement();
						}
					}
				}
			}
			break;
		}
	}
	UOSInt errCode = reader.GetErrorCode();
	if (errCode != 0)
	{
		printf("WMS: XML Parse error: code = %d\r\n", (UInt32)errCode);
	}
	this->SetLayer(0);
}

void Map::WebMapService::LoadXMLRequest(Text::XMLReader *reader)
{
	Text::StringBuilderUTF8 sb;
	while (reader->ReadNext())
	{
		Text::XMLNode::NodeType nodeType = reader->GetNodeType();
		if (nodeType == Text::XMLNode::NodeType::ElementEnd)
		{
			break;
		}
		else if (nodeType == Text::XMLNode::NodeType::Element)
		{
			Text::String *nodeName = reader->GetNodeText();
			if (nodeName->Equals(UTF8STRC("GetCapabilities")))
			{
				reader->SkipElement();
			}
			else if (nodeName->Equals(UTF8STRC("GetMap")))
			{
				while (reader->ReadNext())
				{
					nodeType = reader->GetNodeType();
					if (nodeType == Text::XMLNode::NodeType::ElementEnd)
					{
						break;
					}
					else if (nodeType == Text::XMLNode::NodeType::Element)
					{
						nodeName = reader->GetNodeText();
						if (nodeName->Equals(UTF8STRC("Format")))
						{
							sb.ClearStr();
							if (reader->ReadNodeText(&sb))
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
				reader->SkipElement();
			}
			else if (nodeName->Equals(UTF8STRC("GetFeatureInfo")))
			{
				while (reader->ReadNext())
				{
					nodeType = reader->GetNodeType();
					if (nodeType == Text::XMLNode::NodeType::ElementEnd)
					{
						break;
					}
					else if (nodeType == Text::XMLNode::NodeType::Element)
					{
						nodeName = reader->GetNodeText();
						if (nodeName->Equals(UTF8STRC("Format")))
						{
							sb.ClearStr();
							if (reader->ReadNodeText(&sb))
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
				reader->SkipElement();
			}
			else if (nodeName->Equals(UTF8STRC("DescribeLayer")))
			{
				reader->SkipElement();
			}
			else if (nodeName->Equals(UTF8STRC("GetLegendGraphic")))
			{
				reader->SkipElement();
			}
			else if (nodeName->Equals(UTF8STRC("GetStyles")))
			{
				reader->SkipElement();
			}
			else
			{
				printf("WMS: Unknown element in Request: %s\r\n", nodeName->v);
				reader->SkipElement();
			}
		}
	}
}

void Map::WebMapService::LoadXMLLayers(Text::XMLReader *reader)
{
	Text::StringBuilderUTF8 sb;
	while (reader->ReadNext())
	{
		Text::XMLNode::NodeType nodeType = reader->GetNodeType();
		if (nodeType == Text::XMLNode::NodeType::ElementEnd)
		{
			break;
		}
		else if (nodeType == Text::XMLNode::NodeType::Element)
		{
			if (reader->GetNodeText()->Equals(UTF8STRC("Layer")))
			{
				Bool queryable = false;
				Text::String *layerName = 0;
				Text::String *layerTitle = 0;
				Data::ArrayList<LayerCRS*> layerCRS;
				LayerCRS *crs;
				UOSInt i;
				Text::XMLAttrib *attr;
				i = reader->GetAttribCount();
				while (i-- > 0)
				{
					attr = reader->GetAttrib(i);
					if (attr->name->Equals(UTF8STRC("queryable")))
					{
						queryable = (attr->value != 0 && attr->value->Equals(UTF8STRC("1")));
					}
				}
				while (reader->ReadNext())
				{
					nodeType = reader->GetNodeType();
					if (nodeType == Text::XMLNode::NodeType::ElementEnd)
					{
						break;
					}
					else if (nodeType == Text::XMLNode::NodeType::Element)
					{
						Text::String *nodeName = reader->GetNodeText();
						if (nodeName->Equals(UTF8STRC("Name")))
						{
							sb.ClearStr();
							if (reader->ReadNodeText(&sb))
							{
								SDEL_STRING(layerName);
								layerName = Text::String::New(sb.ToCString());
							}
						}
						else if (nodeName->Equals(UTF8STRC("Title")))
						{
							sb.ClearStr();
							if (reader->ReadNodeText(&sb))
							{
								SDEL_STRING(layerTitle);
								layerTitle = Text::String::New(sb.ToCString());
							}
						}
						else if (nodeName->Equals(UTF8STRC("BoundingBox")))
						{
							crs = MemAllocA(LayerCRS, 1);
							crs->name = 0;
							crs->swapXY = false;
							i = reader->GetAttribCount();
							while (i-- > 0)
							{
								attr = reader->GetAttrib(i);
								if (attr->value)
								{
									if (attr->name->Equals(UTF8STRC("minx")))
									{
										crs->bounds.tl.x = attr->value->ToDouble();
									}
									else if (attr->name->Equals(UTF8STRC("miny")))
									{
										crs->bounds.tl.y = attr->value->ToDouble();
									}
									else if (attr->name->Equals(UTF8STRC("maxx")))
									{
										crs->bounds.br.x = attr->value->ToDouble();
									}
									else if (attr->name->Equals(UTF8STRC("maxy")))
									{
										crs->bounds.br.y = attr->value->ToDouble();
									}
									else if (attr->name->Equals(UTF8STRC("CRS")))
									{
										SDEL_STRING(crs->name);
										crs->name = attr->value->Clone();
									}
									else if (attr->name->Equals(UTF8STRC("SRS")))
									{
										SDEL_STRING(crs->name);
										crs->name = attr->value->Clone();
									}
								}
							}
							reader->SkipElement();
							if (crs->name)
							{
								if (crs->name->Equals(UTF8STRC("EPSG:4326")) && this->version->Equals(UTF8STRC("1.3.0")))
								{
									crs->bounds.tl = crs->bounds.tl.SwapXY();
									crs->bounds.br = crs->bounds.br.SwapXY();
									crs->swapXY = true;
								}
								layerCRS.Add(crs);
							}
							else
							{
								MemFreeA(crs);
							}
						}
						else
						{
							reader->SkipElement();
						}
					}
				}
				if (layerName && layerTitle && layerCRS.GetCount() > 0)
				{
					LayerInfo *layer;
					NEW_CLASS(layer, LayerInfo());
					layer->name = layerName;
					layer->title = layerTitle;
					layer->crsList.AddAll(&layerCRS);
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
						crs = layerCRS.GetItem(i);
						crs->name->Release();
						MemFreeA(crs);
					}
				}
			}
			else
			{
				reader->SkipElement();
			}
		}
	}
}

Map::WebMapService::WebMapService(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::EncodingFactory *encFact, Text::CString wmsURL, Version version)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->encFact = encFact;
	this->wmsURL = Text::String::New(wmsURL);
	this->version = 0;
	this->infoType = 0;
	this->mapImageType = 0;
	this->currCRS = 0;
	this->layer = 0;
	this->csys = 0;
	this->LoadXML(version);
}

Map::WebMapService::~WebMapService()
{
	UOSInt i = this->mapImageTypeNames.GetCount();
	while (i-- > 0)
	{
		this->mapImageTypeNames.GetItem(i)->Release();
	}
	i = this->infoTypeNames.GetCount();
	while (i-- > 0)
	{
		this->infoTypeNames.GetItem(i)->Release();
	}
	i = this->layers.GetCount();
	LayerInfo *layer;
	LayerCRS *crs;
	UOSInt j;
	while (i-- > 0)
	{
		layer = this->layers.GetItem(i);
		SDEL_STRING(layer->name);
		SDEL_STRING(layer->title);
		j = layer->crsList.GetCount();
		while (j-- > 0)
		{
			crs = layer->crsList.GetItem(j);
			crs->name->Release();
			MemFreeA(crs);
		}
		DEL_CLASS(layer);
	}
	SDEL_STRING(this->wmsURL);
	SDEL_STRING(this->version);
	SDEL_CLASS(this->csys);
}

Text::String *Map::WebMapService::GetName() const
{
	LayerInfo *layer = this->layers.GetItem(this->layer);
	if (layer == 0)
		return 0;
	return layer->name;
}

Math::CoordinateSystem *Map::WebMapService::GetCoordinateSystem() const
{
	return this->csys;
}

Math::RectAreaDbl Map::WebMapService::GetInitBounds() const
{
	return this->currCRS->bounds;
}

Bool Map::WebMapService::GetBounds(Math::RectAreaDbl *bounds) const
{
	if (this->currCRS)
	{
		*bounds = this->currCRS->bounds;
		return true;
	}
	return false;
}

Bool Map::WebMapService::CanQuery() const
{
	LayerInfo *layer = this->layers.GetItem(this->layer);
	return layer && layer->queryable;
}

Bool Map::WebMapService::QueryInfos(Math::Coord2DDbl coord, Math::RectAreaDbl bounds, UInt32 width, UInt32 height, Double dpi, Data::ArrayList<Math::Geometry::Vector2D*> *vecList, Data::ArrayList<UOSInt> *valueOfstList, Data::ArrayList<Text::String*> *nameList, Data::ArrayList<Text::String*> *valueList)
{
	LayerInfo *layer = this->layers.GetItem(this->layer);
	Text::String *imgFormat = this->mapImageTypeNames.GetItem(this->mapImageType);
	Text::String *infoFormat = this->infoTypeNames.GetItem(this->infoType);
	if (layer == 0 || !layer->queryable || infoFormat == 0 || imgFormat == 0)
		return false;

	Double x = (coord.x - bounds.tl.x) * width / bounds.GetWidth();
	Double y = (bounds.br.y - coord.y) * height / bounds.GetHeight();

	if (this->currCRS->swapXY)
	{
		bounds.tl = bounds.tl.SwapXY();
		bounds.br = bounds.br.SwapXY();
	}
	Text::StringBuilderUTF8 sb;
	if (this->version->Equals(UTF8STRC("1.1.1")))
	{
		sb.Append(this->wmsURL);
		sb.AppendC(UTF8STRC("?service=WMS&version=1.1.1&request=GetFeatureInfo&layers="));
		Text::TextBinEnc::FormEncoding::FormEncode(&sb, layer->name->v, layer->name->leng);
		sb.AppendC(UTF8STRC("&bbox="));
		sb.AppendDouble(bounds.tl.x);
		sb.AppendC(UTF8STRC("%2C"));
		sb.AppendDouble(bounds.tl.y);
		sb.AppendC(UTF8STRC("%2C"));
		sb.AppendDouble(bounds.br.x);
		sb.AppendC(UTF8STRC("%2C"));
		sb.AppendDouble(bounds.br.y);
		sb.AppendC(UTF8STRC("&width="));
		sb.AppendU32(width);
		sb.AppendC(UTF8STRC("&height="));
		sb.AppendU32(height);
		sb.AppendC(UTF8STRC("&srs="));
		Text::TextBinEnc::FormEncoding::FormEncode(&sb, this->currCRS->name->v, this->currCRS->name->leng);
		sb.AppendC(UTF8STRC("&styles=&format="));
		Text::TextBinEnc::FormEncoding::FormEncode(&sb, imgFormat->v, imgFormat->leng);
		sb.AppendC(UTF8STRC("&QUERY_LAYERS="));
		Text::TextBinEnc::FormEncoding::FormEncode(&sb, layer->name->v, layer->name->leng);
		sb.AppendC(UTF8STRC("&INFO_FORMAT="));
		Text::TextBinEnc::FormEncoding::FormEncode(&sb, infoFormat->v, infoFormat->leng);
		sb.AppendC(UTF8STRC("&FEATURE_COUNT=5"));
		sb.AppendC(UTF8STRC("&X="));
		sb.AppendI32(Double2Int32(x));
		sb.AppendC(UTF8STRC("&Y="));
		sb.AppendI32(Double2Int32(y));
	}
	else if (this->version->Equals(UTF8STRC("1.3.0")))
	{
		sb.Append(this->wmsURL);
		sb.AppendC(UTF8STRC("?service=WMS&version=1.3.0&request=GetFeatureInfo&layers="));
		Text::TextBinEnc::FormEncoding::FormEncode(&sb, layer->name->v, layer->name->leng);
		sb.AppendC(UTF8STRC("&bbox="));
		sb.AppendDouble(bounds.tl.x);
		sb.AppendC(UTF8STRC("%2C"));
		sb.AppendDouble(bounds.tl.y);
		sb.AppendC(UTF8STRC("%2C"));
		sb.AppendDouble(bounds.br.x);
		sb.AppendC(UTF8STRC("%2C"));
		sb.AppendDouble(bounds.br.y);
		sb.AppendC(UTF8STRC("&width="));
		sb.AppendU32(width);
		sb.AppendC(UTF8STRC("&height="));
		sb.AppendU32(height);
		sb.AppendC(UTF8STRC("&CRS="));
		Text::TextBinEnc::FormEncoding::FormEncode(&sb, this->currCRS->name->v, this->currCRS->name->leng);
		sb.AppendC(UTF8STRC("&styles=&format="));
		Text::TextBinEnc::FormEncoding::FormEncode(&sb, imgFormat->v, imgFormat->leng);
		sb.AppendC(UTF8STRC("&QUERY_LAYERS="));
		Text::TextBinEnc::FormEncoding::FormEncode(&sb, layer->name->v, layer->name->leng);
		sb.AppendC(UTF8STRC("&INFO_FORMAT="));
		Text::TextBinEnc::FormEncoding::FormEncode(&sb, infoFormat->v, infoFormat->leng);
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
	//printf("WebMapService: Query URL: %s\r\n", sb.ToString());

	UInt8 dataBuff[2048];
	UOSInt readSize;
	Net::HTTPClient *cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, true);
	if (cli->GetRespStatus() == Net::WebStatus::SC_OK)
	{
		Text::StringBuilderUTF8 sbData;
		while ((readSize = cli->Read(dataBuff, 2048)) > 0)
		{
			sbData.AppendC(dataBuff, readSize);
		}
		DEL_CLASS(cli);

		if (infoFormat->Equals(UTF8STRC("application/json")))
		{
			return Map::OWSFeatureParser::ParseJSON(sbData.ToCString(), this->csys->GetSRID(), vecList, valueOfstList, nameList, valueList);
		}
		else if (infoFormat->StartsWith(UTF8STRC("application/vnd.ogc.gml")) || infoFormat->StartsWith(UTF8STRC("text/xml")))
		{
			return Map::OWSFeatureParser::ParseGML(sbData.ToCString(), this->csys->GetSRID(), this->currCRS->swapXY, this->encFact, vecList, valueOfstList, nameList, valueList);
		}
		else if (infoFormat->Equals(UTF8STRC("text/plain")))
		{
			return Map::OWSFeatureParser::ParseText(sbData, this->csys->GetSRID(), coord, vecList, valueOfstList, nameList, valueList);
		}
		else
		{
			printf("WebMapService: Query URL: %s\r\n", sb.ToString());
			printf("%s\r\n", sbData.ToString());
		}
		return false;
	}
	DEL_CLASS(cli);
	return false;
}

Media::ImageList *Map::WebMapService::DrawMap(Math::RectAreaDbl bounds, UInt32 width, UInt32 height, Double dpi, Text::StringBuilderUTF8 *sbUrl)
{
	Text::StringBuilderUTF8 sb;
	Text::String *imgFormat = this->mapImageTypeNames.GetItem(this->mapImageType);
	LayerInfo *layer = this->layers.GetItem(this->layer);
	if (layer == 0 || imgFormat == 0)
		return 0;
	if (this->currCRS->swapXY)
	{
		bounds.tl = bounds.tl.SwapXY();
		bounds.br = bounds.br.SwapXY();
	}
	if (this->version->Equals(UTF8STRC("1.1.1")))
	{
		// http://127.0.0.1:8080/geoserver/Dev/wms?service=WMS&version=1.1.0&request=GetMap&layers=Dev%3Athreed_burial_poly&bbox=113.9587574553149%2C22.34255390361735%2C114.1047088037185%2C22.3992408177216&width=768&height=330&srs=EPSG%3A4326&styles=&format=image%2Fpng
		sb.Append(this->wmsURL);
		sb.AppendC(UTF8STRC("?service=WMS&version=1.1.1&request=GetMap&layers="));
		Text::TextBinEnc::FormEncoding::FormEncode(&sb, layer->name->v, layer->name->leng);
		sb.AppendC(UTF8STRC("&bbox="));
		sb.AppendDouble(bounds.tl.x);
		sb.AppendC(UTF8STRC("%2C"));
		sb.AppendDouble(bounds.tl.y);
		sb.AppendC(UTF8STRC("%2C"));
		sb.AppendDouble(bounds.br.x);
		sb.AppendC(UTF8STRC("%2C"));
		sb.AppendDouble(bounds.br.y);
		sb.AppendC(UTF8STRC("&width="));
		sb.AppendU32(width);
		sb.AppendC(UTF8STRC("&height="));
		sb.AppendU32(height);
		sb.AppendC(UTF8STRC("&TRANSPARENT=TRUE"));
		sb.AppendC(UTF8STRC("&srs="));
		Text::TextBinEnc::FormEncoding::FormEncode(&sb, this->currCRS->name->v, this->currCRS->name->leng);
		sb.AppendC(UTF8STRC("&styles=&format="));
		Text::TextBinEnc::FormEncoding::FormEncode(&sb, imgFormat->v, imgFormat->leng);
	}
	else if (this->version->Equals(UTF8STRC("1.3.0")))
	{
		sb.Append(this->wmsURL);
		sb.AppendC(UTF8STRC("?service=WMS&version=1.3.0&request=GetMap&layers="));
		Text::TextBinEnc::FormEncoding::FormEncode(&sb, layer->name->v, layer->name->leng);
		sb.AppendC(UTF8STRC("&bbox="));
		sb.AppendDouble(bounds.tl.x);
		sb.AppendC(UTF8STRC("%2C"));
		sb.AppendDouble(bounds.tl.y);
		sb.AppendC(UTF8STRC("%2C"));
		sb.AppendDouble(bounds.br.x);
		sb.AppendC(UTF8STRC("%2C"));
		sb.AppendDouble(bounds.br.y);
		sb.AppendC(UTF8STRC("&width="));
		sb.AppendU32(width);
		sb.AppendC(UTF8STRC("&height="));
		sb.AppendU32(height);
		sb.AppendC(UTF8STRC("&TRANSPARENT=TRUE"));
		sb.AppendC(UTF8STRC("&CRS="));
		Text::TextBinEnc::FormEncoding::FormEncode(&sb, this->currCRS->name->v, this->currCRS->name->leng);
		sb.AppendC(UTF8STRC("&styles=&format="));
		Text::TextBinEnc::FormEncoding::FormEncode(&sb, imgFormat->v, imgFormat->leng);
	}
	if (sbUrl)
	{
		sbUrl->Append(sb);
	}

//	printf("URL: %s\r\n", sb.ToString());
	UInt8 dataBuff[2048];
	UOSInt readSize;
	Media::ImageList *ret = 0;
	Net::HTTPClient *cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, true);
	Bool succ = cli->GetRespStatus() == Net::WebStatus::SC_OK;
	if (succ)
	{
		IO::MemoryStream mstm(UTF8STRC("Map.ESRI.ESRIMapServer.DrawMap"));
		while ((readSize = cli->Read(dataBuff, 2048)) > 0)
		{
			mstm.Write(dataBuff, readSize);
		}
		UOSInt size;
		IO::StmData::MemoryDataRef mdr(mstm.GetBuff(&size), mstm.GetLength());
		if (imgFormat->StartsWith(UTF8STRC("image/png")))
		{
			Parser::FileParser::PNGParser parser;
			ret = (Media::ImageList*)parser.ParseFile(&mdr, 0, IO::ParserType::ImageList);
		}
		else if (imgFormat->StartsWith(UTF8STRC("image/geotiff")) || imgFormat->StartsWith(UTF8STRC("image/tiff")))
		{
			Parser::FileParser::TIFFParser parser;
			ret = (Media::ImageList*)parser.ParseFile(&mdr, 0, IO::ParserType::ImageList);
		}
		else if (imgFormat->Equals(UTF8STRC("image/gif")))
		{
			Parser::FileParser::GIFParser parser;
			ret = (Media::ImageList*)parser.ParseFile(&mdr, 0, IO::ParserType::ImageList);
		}
		else if (imgFormat->Equals(UTF8STRC("image/jpeg")))
		{
			Parser::FileParser::GUIImgParser parser;
			ret = (Media::ImageList*)parser.ParseFile(&mdr, 0, IO::ParserType::ImageList);
		}
	}
	DEL_CLASS(cli);
	return ret;
}

Bool Map::WebMapService::IsError() const
{
	return this->version == 0 || this->layers.GetCount() == 0 || this->mapImageTypeNames.GetCount() == 0;
}

UOSInt Map::WebMapService::GetInfoType() const
{
	return this->infoType;
}

UOSInt Map::WebMapService::GetLayerCRS() const
{
	if (this->currCRS == 0)
		return INVALID_INDEX;
	LayerInfo *layer = this->layers.GetItem(this->layer);
	if (layer)
	{
		return layer->crsList.IndexOf(this->currCRS);
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
		LayerInfo *layer = this->layers.GetItem(index);
		LayerCRS *crs;
		Bool found = false;
		UOSInt i = 0;
		UOSInt j = layer->crsList.GetCount();
		while (i < j)
		{
			crs = layer->crsList.GetItem(i);
			if (crs->name->StartsWith(UTF8STRC("CRS:")))
			{

			}
			else
			{
				found = true;
				this->currCRS = crs;
				break;
			}
			i++;
		}
		if (!found)
		{
			this->currCRS = layer->crsList.GetItem(0);
		}
		SDEL_CLASS(this->csys);
		this->csys = Math::CoordinateSystemManager::CreateFromName(this->currCRS->name->ToCString());
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
	LayerInfo *layer = this->layers.GetItem(this->layer);
	if (layer && index < layer->crsList.GetCount())
	{
		this->currCRS = layer->crsList.GetItem(index);
	}
}

UOSInt Map::WebMapService::GetLayerNames(Data::ArrayList<Text::String*> *nameList) const
{
	UOSInt i = 0;
	UOSInt j = this->layers.GetCount();
	while (i < j)
	{
		nameList->Add(this->layers.GetItem(i)->name);
		i++;
	}
	return j;
}

UOSInt Map::WebMapService::GetMapImageTypeNames(Data::ArrayList<Text::String*> *nameList) const
{
	return nameList->AddAll(&this->mapImageTypeNames);
}

UOSInt Map::WebMapService::GetInfoTypeNames(Data::ArrayList<Text::String*> *nameList) const
{
	return nameList->AddAll(&this->infoTypeNames);
}

UOSInt Map::WebMapService::GetLayerCRSNames(Data::ArrayList<Text::String*> *nameList) const
{
	LayerInfo *layer = this->layers.GetItem(this->layer);
	if (layer == 0)
		return 0;
	UOSInt i = 0;
	UOSInt j = layer->crsList.GetCount();
	while (i < j)
	{
		nameList->Add(layer->crsList.GetItem(i)->name);
		i++;
	}
	return j;
}