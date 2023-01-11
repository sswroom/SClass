#include "Stdafx.h"
#include "IO/MemoryStream.h"
#include "Map/WebFeatureService.h"
#include "Net/HTTPClient.h"
#include "Parser/FileParser/XMLParser.h"
#include "Text/TextBinEnc/FormEncoding.h"

#include <stdio.h>

void Map::WebFeatureService::LoadXML(Version version)
{
	Text::StringBuilderUTF8 sb;
	sb.Append(this->wfsURL);
	switch (version)
	{
	case Version::V1_0_0:
		sb.AppendC(UTF8STRC("?SERVICE=WFS&version=1.0.0&REQUEST=GetCapabilities"));
		break;
	case Version::V1_1_0:
		sb.AppendC(UTF8STRC("?SERVICE=WFS&version=1.1.0&REQUEST=GetCapabilities&tiled=true"));
		break;
	case Version::V2_0_0:
		sb.AppendC(UTF8STRC("?SERVICE=WFS&version=2.0.0&REQUEST=GetCapabilities"));
		break;
	case Version::ANY:
	default:
		sb.AppendC(UTF8STRC("?SERVICE=WFS&REQUEST=GetCapabilities"));
		break;
	}
	Net::HTTPClient *cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, true);
	if (cli == 0)
		return;
	UInt8 buff[2048];
	UOSInt readSize;
	IO::MemoryStream mstm;
	while ((readSize = cli->Read(buff, sizeof(readSize))) > 0)
	{
		mstm.Write(buff, readSize);
	}
	DEL_CLASS(cli);
	mstm.SeekFromBeginning(0);
	Text::XMLReader reader(this->encFact, &mstm, Text::XMLReader::PM_XML);
	while (reader.ReadNext())
	{
		if (reader.GetNodeType() == Text::XMLNode::NodeType::Element)
		{
			Text::String *nodeName = reader.GetNodeText();
			if (nodeName->Equals(UTF8STRC("WFS_Capabilities")) || nodeName->EndsWith(UTF8STRC(":WFS_Capabilities")))
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
						else if (nodeName->Equals(UTF8STRC("FeatureTypeList")))
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
									if (nodeName->Equals(UTF8STRC("FeatureType")))
									{
										this->LoadXMLFeatureType(&reader);
									}
									else if (nodeName->Equals(UTF8STRC("Operations")))
									{
										reader.SkipElement();
									}
									else
									{
										printf("WFS: Unknown element in FeatureTypeList: %s\r\n", nodeName->v);
										reader.SkipElement();
									}
								}
							}
						}
						else if (nodeName->Equals(UTF8STRC("Capability")))
						{
							reader.SkipElement();
						}
						else if (nodeName->EndsWith(UTF8STRC(":Filter_Capabilities")))
						{
							reader.SkipElement();
						}
						else if (nodeName->EndsWith(UTF8STRC(":ServiceIdentification")))
						{
							reader.SkipElement();
						}
						else if (nodeName->EndsWith(UTF8STRC(":ServiceProvider")))
						{
							reader.SkipElement();
						}
						else if (nodeName->EndsWith(UTF8STRC(":OperationsMetadata")))
						{
							reader.SkipElement();
						}
						else
						{
							printf("WFS: Unknown element in WFS_Capabilities: %s\r\n", nodeName->v);
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
		printf("WFS: XML Parse error: code = %d\r\n", (UInt32)errCode);
	}
	this->SetFeature(0);
}

void Map::WebFeatureService::LoadXMLFeatureType(Text::XMLReader *reader)
{
	Text::StringBuilderUTF8 sb;
	Text::String *name = 0;
	Text::String *title = 0;
	Text::String *crs = 0;
	Math::RectAreaDbl wgs84Bounds = Math::RectAreaDbl(Math::Coord2DDbl(0, 0), Math::Coord2DDbl(0, 0));
	Bool hasTL = false;
	Bool hasBR = false;
	Text::XMLNode::NodeType nodeType;
	Text::PString sarr[3];
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
					SDEL_STRING(name);
					name = Text::String::New(sb.ToCString());
				}
			}
			else if (nodeName->Equals(UTF8STRC("Title")))
			{
				sb.ClearStr();
				if (reader->ReadNodeText(&sb))
				{
					SDEL_STRING(title);
					title = Text::String::New(sb.ToCString());
				}
			}
			else if (nodeName->Equals(UTF8STRC("DefaultCRS")))
			{
				sb.ClearStr();
				if (reader->ReadNodeText(&sb))
				{
					SDEL_STRING(crs);
					crs = Text::String::New(sb.ToCString());
				}
			}
			else if (nodeName->Equals(UTF8STRC("DefaultSRS")))
			{
				sb.ClearStr();
				if (reader->ReadNodeText(&sb))
				{
					SDEL_STRING(crs);
					crs = Text::String::New(sb.ToCString());
				}
			}
			else if (nodeName->Equals(UTF8STRC("SRS")))
			{
				sb.ClearStr();
				if (reader->ReadNodeText(&sb))
				{
					SDEL_STRING(crs);
					crs = Text::String::New(sb.ToCString());
				}
			}
			else if (nodeName->EndsWith(UTF8STRC(":WGS84BoundingBox")))
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
						if (nodeName->EndsWith(UTF8STRC(":LowerCorner")))
						{
							sb.ClearStr();
							if (reader->ReadNodeText(&sb))
							{
								if (Text::StrSplitP(sarr, 3, sb, ' ') >= 2)
								{
									hasTL = true;
									wgs84Bounds.tl.x = sarr[0].ToDouble();
									wgs84Bounds.tl.y = sarr[1].ToDouble();
								}
							}
						}
						else if (nodeName->EndsWith(UTF8STRC(":UpperCorner")))
						{
							sb.ClearStr();
							if (reader->ReadNodeText(&sb))
							{
								if (Text::StrSplitP(sarr, 3, sb, ' ') >= 2)
								{
									hasBR = true;
									wgs84Bounds.br.x = sarr[0].ToDouble();
									wgs84Bounds.br.y = sarr[1].ToDouble();
								}
							}
						}
					}
				}
			}
			else if (nodeName->Equals(UTF8STRC("LatLongBoundingBox")))
			{
				UOSInt i = reader->GetAttribCount();
				Text::XMLAttrib *attr;
				while (i-- > 0)
				{
					attr = reader->GetAttrib(i);
					if (attr->name->Equals(UTF8STRC("minx")) && attr->value != 0)
					{
						hasTL = attr->value->ToDouble(&wgs84Bounds.tl.x);
					}
					else if (attr->name->Equals(UTF8STRC("miny")) && attr->value != 0)
					{
						hasTL = attr->value->ToDouble(&wgs84Bounds.tl.y);
					}
					else if (attr->name->Equals(UTF8STRC("maxx")) && attr->value != 0)
					{
						hasBR = attr->value->ToDouble(&wgs84Bounds.br.x);
					}
					else if (attr->name->Equals(UTF8STRC("maxy")) && attr->value != 0)
					{
						hasBR = attr->value->ToDouble(&wgs84Bounds.br.y);
					}
				}
				reader->SkipElement();
			}
			else
			{
				reader->SkipElement();
			}
		}
	}
	if (name && title && crs && hasTL && hasBR)
	{
		FeatureType *feature;
		feature = MemAllocA(FeatureType, 1);
		feature->name = name;
		feature->title = title;
		feature->crs = crs;
		feature->wgs84Bounds = wgs84Bounds;
		this->features.Add(feature);
	}
	else
	{
		SDEL_STRING(name);
		SDEL_STRING(title);
		SDEL_STRING(crs);
	}
}

Map::WebFeatureService::WebFeatureService(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::EncodingFactory *encFact, Text::CString wfsURL, Version version)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->encFact = encFact;
	this->wfsURL = Text::String::New(wfsURL);
	this->version = 0;
	this->LoadXML(version);
}

Map::WebFeatureService::~WebFeatureService()
{
	SDEL_STRING(this->wfsURL);
	SDEL_STRING(this->version);
	FeatureType *feature;
	UOSInt i = this->features.GetCount();
	while (i-- > 0)
	{
		feature = this->features.GetItem(i);
		feature->name->Release();
		feature->title->Release();
		feature->crs->Release();
		MemFreeA(feature);
	}
}

Bool Map::WebFeatureService::IsError() const
{
	return this->features.GetCount() == 0;
}

void Map::WebFeatureService::SetFeature(UOSInt index)
{
	this->currFeature = this->features.GetItem(index);
}

UOSInt Map::WebFeatureService::GetFeatureNames(Data::ArrayList<Text::String*> *nameList) const
{
	UOSInt i = 0;
	UOSInt j = this->features.GetCount();
	while (i < j)
	{
		nameList->Add(this->features.GetItem(i)->name);
		i++;
	}
	return j;
}

Map::IMapDrawLayer *Map::WebFeatureService::LoadAsLayer()
{
	Bool needSwapXY = false;
	if (this->currFeature)
	{
		Text::StringBuilderUTF8 sb;
		if (this->version->Equals(UTF8STRC("1.0.0")))
		{
			sb.Append(this->wfsURL);
			sb.AppendC(UTF8STRC("?service=wfs&version=1.0.0&request=GetFeature&outputFormat=GML2&typeName="));
			Text::TextBinEnc::FormEncoding::FormEncode(&sb, this->currFeature->name->v, this->currFeature->name->leng);
		}
		else if (this->version->Equals(UTF8STRC("1.1.0")))
		{
			sb.Append(this->wfsURL);
			sb.AppendC(UTF8STRC("?service=wfs&version=1.1.0&request=GetFeature&outputFormat=GML2&typeName="));
			Text::TextBinEnc::FormEncoding::FormEncode(&sb, this->currFeature->name->v, this->currFeature->name->leng);
			needSwapXY = true;
		}
		else if (this->version->Equals(UTF8STRC("2.0.0")))
		{
			sb.Append(this->wfsURL);
			sb.AppendC(UTF8STRC("?service=wfs&version=2.0.0&request=GetFeature&outputFormat=GML2&typeName="));
			Text::TextBinEnc::FormEncoding::FormEncode(&sb, this->currFeature->name->v, this->currFeature->name->leng);
			needSwapXY = true;
		}
		else
		{
			return 0;
		}
		IO::MemoryStream mstm;
		if (!Net::HTTPClient::LoadContent(this->sockf, this->ssl, sb.ToCString(), &mstm, 104857600))
		{
			return 0;
		}
		mstm.SeekFromBeginning(0);
		sb.ClearStr();
		sb.Append(this->currFeature->title);
		sb.AppendC(UTF8STRC(".gml"));
		IO::ParsedObject *pobj = Parser::FileParser::XMLParser::ParseStream(this->encFact, &mstm, sb.ToCString(), 0, 0, 0);
		if (pobj)
		{
			if (pobj->GetParserType() == IO::ParserType::MapLayer)
			{
				if (needSwapXY)
				{
					Map::VectorLayer *layer = (Map::VectorLayer*)pobj;
					layer->SwapXY();
					return layer;
				}
				else
				{
					return (Map::IMapDrawLayer*)pobj;
				}
			}
			DEL_CLASS(pobj);
			return 0;
		}
	}
	return 0;
}
