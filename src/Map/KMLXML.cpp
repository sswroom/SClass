#include "Stdafx.h"
#include "Data/ArrayListA.h"
#include "Data/ArrayListDbl.h"
#include "Data/ICaseStringMap.h"
#include "Map/KMLXML.h"
#include "Map/MapLayerCollection.h"
#include "Map/NetworkLinkLayer.h"
#include "Map/VectorLayer.h"
#include "Map/WebImageLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Geometry/GeometryCollection.h"
#include "Math/Geometry/LineString.h"
#include "Math/Geometry/MultiPolygon.h"
#include "Math/Geometry/PointZ.h"
#include "Math/Geometry/Polygon.h"
#include "Math/Geometry/Polyline.h"
#include "Media/StaticImage.h"
#include "Text/URLString.h"

#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

Optional<Map::MapDrawLayer> Map::KMLXML::ParseKMLRoot(NN<Text::XMLReader> reader, Text::CStringNN fileName, Optional<Parser::ParserList> parsers, Optional<Net::WebBrowser> browser, Optional<IO::PackageFile> pkgFile)
{
	if (reader->GetNodeType() != Text::XMLNode::NodeType::Element)
		return 0;
	if (!Text::String::OrEmpty(reader->GetNodeText())->Equals(UTF8STRC("kml")))
		return 0;
	Data::ICaseStringMap<KMLStyle*> styles;
	Optional<Map::MapDrawLayer> lyr = ParseKMLContainer(reader, &styles, fileName, parsers, browser, pkgFile, true);

	NN<const Data::ArrayList<KMLStyle*>> styleList = styles.GetValues();
	KMLStyle *style;
	UOSInt ui = styleList->GetCount();
	while (ui-- > 0)
	{
		style = styleList->GetItem(ui);
		SDEL_STRING(style->iconURL);
		SDEL_CLASS(style->img);
		MemFree(style);
	}
	return lyr;
}


Optional<Map::MapDrawLayer> Map::KMLXML::ParseKMLContainer(NN<Text::XMLReader> reader, Data::ICaseStringMap<KMLStyle*> *styles, Text::CStringNN sourceName, Optional<Parser::ParserList> parsers, Optional<Net::WebBrowser> browser, Optional<IO::PackageFile> basePF, Bool rootKml)
{
	KMLStyle *style;
	UOSInt i;
	NN<Text::XMLAttrib> attr;
	Text::StringBuilderUTF8 sb;
	Data::DateTime dt;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sbuffEnd;
	Data::ArrayListNN<Map::MapDrawLayer> layers;
	NN<Text::String> nns;

	Map::WebImageLayer *imgLyr = 0;
	Text::StringBuilderUTF8 containerNameSb;
	containerNameSb.Append(sourceName);
	NN<Map::MapDrawLayer> lyr;
	i = Text::StrLastIndexOfCharC(containerNameSb.ToString(), containerNameSb.GetLength(), '/');
	if (i != INVALID_INDEX)
	{
		containerNameSb.SetSubstr(i + 1);
	}
	i = Text::StrLastIndexOfCharC(containerNameSb.ToString(), containerNameSb.GetLength(), '\\');
	if (i != INVALID_INDEX)
	{
		containerNameSb.SetSubstr(i + 1);
	}

	NN<Parser::ParserList> nnparsers;
	NN<Net::WebBrowser> nnbrowser;
	NN<Text::String> nodeName;
	while (reader->NextElementName().SetTo(nodeName))
	{
		if (nodeName->EqualsICase(UTF8STRC("NetworkLink")))
		{
			if (parsers.SetTo(nnparsers) && browser.SetTo(nnbrowser))
			{
				NN<Map::NetworkLinkLayer> lyr;
				NN<Text::String> layerName = Text::String::NewEmpty();
				NN<Text::String> url = Text::String::NewEmpty();
				NN<Text::String> viewFormat = Text::String::NewEmpty();
				Map::NetworkLinkLayer::RefreshMode mode = Map::NetworkLinkLayer::RefreshMode::OnInterval;
				Int32 interval = 0;
				NEW_CLASSNN(lyr, Map::NetworkLinkLayer(sourceName, nnparsers, nnbrowser, CSTR_NULL));
			
				while (reader->NextElementName().SetTo(nodeName))
				{
					if (nodeName->EqualsICase(UTF8STRC("NAME")))
					{
						sb.ClearStr();
						reader->ReadNodeText(sb);
						lyr->SetLayerName(sb.ToCString());
						layerName->Release();
						layerName = Text::String::New(sb.ToCString());
					}
					else if (nodeName->Equals(UTF8STRC("Link")))
					{
						while (reader->NextElementName().SetTo(nodeName))
						{
							if (nodeName->Equals(UTF8STRC("href")))
							{
								sb.ClearStr();
								reader->ReadNodeText(sb);
								url->Release();
								url = Text::String::New(sb.ToCString());
							}
							else if (nodeName->Equals(UTF8STRC("refreshInterval")))
							{
								sb.ClearStr();
								reader->ReadNodeText(sb);
								interval = sb.ToInt32();
								mode = Map::NetworkLinkLayer::RefreshMode::OnInterval;
							}
							else if (nodeName->Equals(UTF8STRC("refreshMode")))
							{
								sb.ClearStr();
								reader->ReadNodeText(sb);
								if (sb.Equals(UTF8STRC("onInterval")))
								{
									mode = Map::NetworkLinkLayer::RefreshMode::OnInterval;
								}
								else if (sb.Equals(UTF8STRC("onChange")))
								{
								}
								else if (sb.Equals(UTF8STRC("onExpire")))
								{
								}
							}
							else if (nodeName->EqualsICase(UTF8STRC("viewRefreshMode")))
							{
								sb.ClearStr();
								reader->ReadNodeText(sb);
								if (sb.Equals(UTF8STRC("never")))
								{
								}
								else if (sb.Equals(UTF8STRC("onStop")))
								{
									mode = Map::NetworkLinkLayer::RefreshMode::OnStop;
								}
								else if (sb.Equals(UTF8STRC("onRequest")))
								{
									mode = Map::NetworkLinkLayer::RefreshMode::OnRequest;
								}
								else if (sb.Equals(UTF8STRC("onRegion")))
								{

								}
							}
							else if (nodeName->Equals(UTF8STRC("viewRefreshTime")))
							{
								sb.ClearStr();
								reader->ReadNodeText(sb);
								interval = sb.ToInt32();
								mode = Map::NetworkLinkLayer::RefreshMode::OnStop;
							}
							else if (nodeName->Equals(UTF8STRC("viewFormat")))
							{
								sb.ClearStr();
								reader->ReadNodeText(sb);
								sb.TrimWSCRLF();
								viewFormat->Release();
								viewFormat = Text::String::New(sb.ToCString());
							}
							else
							{
								reader->SkipElement();
							}
						}
					}
					else if (nodeName->Equals(UTF8STRC("Region")))
					{
						while (reader->NextElementName().SetTo(nodeName))
						{
							if (nodeName->Equals(UTF8STRC("LatLonBox")))
							{
								Double north = 0;
								Double south = 0;
								Double east = 0;
								Double west = 0;
								while (reader->NextElementName().SetTo(nodeName))
								{
									if (nodeName->Equals(UTF8STRC("north")))
									{
										sb.ClearStr();
										reader->ReadNodeText(sb);
										north = sb.ToDoubleOr(0);
									}
									else if (nodeName->Equals(UTF8STRC("south")))
									{
										sb.ClearStr();
										reader->ReadNodeText(sb);
										south = sb.ToDoubleOr(0);
									}
									else if (nodeName->Equals(UTF8STRC("east")))
									{
										sb.ClearStr();
										reader->ReadNodeText(sb);
										east = sb.ToDoubleOr(0);
									}
									else if (nodeName->Equals(UTF8STRC("west")))
									{
										sb.ClearStr();
										reader->ReadNodeText(sb);
										west = sb.ToDoubleOr(0);
									}
									else
									{
										reader->SkipElement();
									}
								}
								lyr->SetBounds(Math::RectAreaDbl(Math::Coord2DDbl(west, south), Math::Coord2DDbl(east, north)));
							}
							else
							{
								reader->SkipElement();
							}
						}
					}
					else
					{
						reader->SkipElement();
					}
				}

				if (url->leng > 0)
				{
					lyr->AddLink(layerName->ToCString(), url->ToCString(), viewFormat->ToCString(), mode, interval);
				}

				layerName->Release();
				viewFormat->Release();
				url->Release();
				layers.Add(lyr);
			}
			else
			{
				reader->SkipElement();
			}
		}
		else if (nodeName->EqualsICase(UTF8STRC("STYLE")))
		{
			Text::String *styleId = 0;
			style = MemAlloc(KMLStyle, 1);
			style->iconSpotX = -1;
			style->iconSpotY = -1;
			style->iconURL = 0;
			style->iconColor = 0;
			style->lineColor = 0;
			style->lineWidth = 0;
			style->fillColor = 0;
			style->flags = 0;
			style->img = 0;
			i = reader->GetAttribCount();
			while (i-- > 0)
			{
				attr = reader->GetAttribNoCheck(i);
				if (attr->name.SetTo(nns) && nns->EqualsICase(UTF8STRC("ID")))
				{
					styleId = Text::String::OrEmpty(attr->value)->Clone().Ptr();
					break;
				}
			}

			while (reader->NextElementName().SetTo(nodeName))
			{
				if (nodeName->EqualsICase(UTF8STRC("LINESTYLE")))
				{
					while (reader->NextElementName().SetTo(nodeName))
					{
						if (nodeName->EqualsICase(UTF8STRC("COLOR")))
						{
							sb.ClearStr();
							reader->ReadNodeText(sb);
							style->lineColor = Text::StrHex2UInt32C(sb.ToString());
							style->lineColor = (style->lineColor & 0xff00ff00) | ((style->lineColor & 0xff) << 16) | ((style->lineColor & 0xff0000) >> 16);
							style->flags |= 1;
						}
						else if (nodeName->EqualsICase(UTF8STRC("WIDTH")))
						{
							sb.ClearStr();
							reader->ReadNodeText(sb);
							sb.ToDouble(style->lineWidth);
							style->flags |= 2;
						}
						else
						{
							reader->SkipElement();
						}
					}
				}
				else if (nodeName->EqualsICase(UTF8STRC("ICONSTYLE")))
				{
					while (reader->NextElementName().SetTo(nodeName))
					{
						if (nodeName->EqualsICase(UTF8STRC("COLOR")))
						{
							sb.ClearStr();
							reader->ReadNodeText(sb);
							style->iconColor = Text::StrHex2UInt32C(sb.ToString());
							style->iconColor = (style->iconColor & 0xff00ff00) | ((style->iconColor & 0xff) << 16) | ((style->iconColor & 0xff0000) >> 16);
						}
						else if (nodeName->EqualsICase(UTF8STRC("ICON")))
						{
							while (reader->NextElementName().SetTo(nodeName))
							{
								if (nodeName->EqualsICase(UTF8STRC("HREF")))
								{
									sb.ClearStr();
									reader->ReadNodeText(sb);
									if (sb.GetLength() > 0)
									{
										SDEL_STRING(style->iconURL);
										style->iconURL = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
									}
								}
								else
								{
									reader->SkipElement();
								}
							}
						}
						else if (nodeName->EqualsICase(UTF8STRC("HOTSPOT")))
						{
							i = reader->GetAttribCount();
							while (i-- > 0)
							{
								attr = reader->GetAttribNoCheck(i);
								if (attr->name.SetTo(nns))
								{
									if (nns->EqualsICase(UTF8STRC("X")))
									{
										style->iconSpotX = Text::String::OrEmpty(attr->value)->ToInt32();
									}
									else if (nns->EqualsICase(UTF8STRC("Y")))
									{
										style->iconSpotY = Text::String::OrEmpty(attr->value)->ToInt32();
									}
								}
							}
							sb.ClearStr();
							reader->ReadNodeText(sb);
							sb.ToDouble(style->lineWidth);
						}
						else
						{
							reader->SkipElement();
						}
					}
				}
				else if (nodeName->EqualsICase(UTF8STRC("POLYSTYLE")))
				{
					while (reader->NextElementName().SetTo(nodeName))
					{
						if (nodeName->EqualsICase(UTF8STRC("COLOR")))
						{
							sb.ClearStr();
							reader->ReadNodeText(sb);
							style->fillColor = Text::StrHex2UInt32C(sb.ToString());
							style->fillColor = (style->fillColor & 0xff00ff00) | ((style->fillColor & 0xff) << 16) | ((style->fillColor & 0xff0000) >> 16);
							style->flags |= 4;
						}
						else
						{
							reader->SkipElement();
						}
					}
				}
				else
				{
					reader->SkipElement();
				}
			}

			if (styleId)
			{
				style = styles->Put(styleId, style);
				styleId->Release();
			}				
			if (style)
			{
				SDEL_STRING(style->iconURL);
				SDEL_CLASS(style->img);
				MemFree(style);
				style = 0;
			}
		}
		else if (nodeName->EqualsICase(UTF8STRC("STYLEMAP")))
		{
			Text::String *styleId = 0;
			style = MemAlloc(KMLStyle, 1);
			style->iconSpotX = -1;
			style->iconSpotY = -1;
			style->iconURL = 0;
			style->iconColor = 0;
			style->lineColor = 0;
			style->lineWidth = 0;
			style->fillColor = 0;
			style->flags = 0;
			style->img = 0;
			i = reader->GetAttribCount();
			while (i-- > 0)
			{
				attr = reader->GetAttribNoCheck(i);
				if (attr->name.SetTo(nns) && nns->EqualsICase(UTF8STRC("ID")))
				{
					styleId = Text::String::OrEmpty(attr->value)->Clone().Ptr();
					break;
				}
			}

			while (reader->NextElementName().SetTo(nodeName))
			{
				if (nodeName->EqualsICase(UTF8STRC("PAIR")))
				{
					Bool isNormal = false;
					while (reader->NextElementName().SetTo(nodeName))
					{
						if (nodeName->EqualsICase(UTF8STRC("KEY")))
						{
							sb.ClearStr();
							reader->ReadNodeText(sb);
							if (sb.Equals(UTF8STRC("normal")))
							{
								isNormal = true;
							}
						}
						else if (nodeName->EqualsICase(UTF8STRC("STYLEURL")))
						{
							sb.ClearStr();
							reader->ReadNodeText(sb);
							if (isNormal && sb.StartsWith(UTF8STRC("#")))
							{
								KMLStyle *style2 = styles->Get({sb.ToString() + 1, sb.GetLength() - 1});
								if (style2)
								{
									style->iconSpotX = style2->iconSpotX;
									style->iconSpotY = style2->iconSpotY;
									style->lineColor = style2->lineColor;
									style->lineWidth = style2->lineWidth;
									SDEL_STRING(style->iconURL);
									style->iconURL = SCOPY_STRING(style2->iconURL);
									style->fillColor = style2->fillColor;
									style->flags = style2->flags;
									SDEL_CLASS(style->img);
									if (style2->img == 0)
									{
										style->img = 0;
									}
									else
									{
										style->img = style2->img->Clone().Ptr();
									}
								}
							}
						}
						else
						{
							reader->SkipElement();
						}
					}
				}
				else
				{
					reader->SkipElement();
				}
			}

			if (styleId)
			{
				style = styles->Put(styleId, style);
				styleId->Release();
			}
			if (style)
			{
				SDEL_STRING(style->iconURL);
				SDEL_CLASS(style->img);
				MemFree(style);
				style = 0;
			}
		}
		else if (nodeName->EqualsICase(UTF8STRC("PLACEMARK")))
		{
			if (ParseKMLPlacemarkLyr(reader, styles, sourceName, parsers, browser, basePF).SetTo(lyr))
			{
				layers.Add(lyr);
			}
		}
		else if (nodeName->EqualsICase(UTF8STRC("SCREENOVERLAY")))
		{
			if (browser.SetTo(nnbrowser) && parsers.SetTo(nnparsers))
			{
				if (imgLyr == 0)
				{
					NEW_CLASS(imgLyr, Map::WebImageLayer(nnbrowser, nnparsers, sourceName, Math::CoordinateSystemManager::CreateWGS84Csys(), containerNameSb.ToCString()));
				}

				NN<Text::String> name = Text::String::NewEmpty();
				Int32 zIndex = 0;
				Double minX = 0;
				Double minY = 0;
				Double oX = 0;
				Double oY = 0;
				Double sizeX = 0;
				Double sizeY = 0;
				Bool hasAltitude = false;
				Double altitude = 0;
				UInt32 color = 0xffffffff;
				Int64 timeStart = 0;
				Int64 timeEnd = 0;
				sbuff[0] = 0;
				sbuffEnd = sbuff;

				while (reader->NextElementName().SetTo(nodeName))
				{
					if (nodeName->EqualsICase(UTF8STRC("NAME")))
					{
						sb.ClearStr();
						reader->ReadNodeText(sb);
						name->Release();
						name = Text::String::New(sb.ToCString());
					}
					else if (nodeName->EqualsICase(UTF8STRC("COLOR")))
					{
						sb.ClearStr();
						reader->ReadNodeText(sb);
						color = Text::StrHex2UInt32C(sb.ToString());
					}
					else if (nodeName->EqualsICase(UTF8STRC("DRAWORDER")))
					{
						sb.ClearStr();
						reader->ReadNodeText(sb);
						zIndex = Text::StrToInt32(sb.ToString());
					}
					else if (nodeName->EqualsICase(UTF8STRC("ICON")))
					{
						while (reader->NextElementName().SetTo(nodeName))
						{
							if (nodeName->EqualsICase(UTF8STRC("HREF")))
							{
								sb.ClearStr();
								reader->ReadNodeText(sb);
								sbuffEnd = imgLyr->GetSourceName(sbuff);
								sbuffEnd = Text::URLString::AppendURLPath(sbuff, sbuffEnd, sb.ToCString()).Or(sbuff);
							}
							else
							{
								reader->SkipElement();
							}
						}
					}
					else if (nodeName->EqualsICase(UTF8STRC("TIMESPAN")))
					{
						while (reader->NextElementName().SetTo(nodeName))
						{
							if (nodeName->EqualsICase(UTF8STRC("BEGIN")))
							{
								sb.ClearStr();
								reader->ReadNodeText(sb);
								dt.SetValue(sb.ToCString());
								timeStart = dt.ToUnixTimestamp();
							}
							else if (nodeName->EqualsICase(UTF8STRC("END")))
							{
								sb.ClearStr();
								reader->ReadNodeText(sb);
								dt.SetValue(sb.ToCString());
								timeEnd = dt.ToUnixTimestamp();
							}
							else
							{
								reader->SkipElement();
							}
						}
					}
					else if (nodeName->EqualsICase(UTF8STRC("SCREENXY")))
					{
						i = reader->GetAttribCount();
						while (i-- > 0)
						{
							attr = reader->GetAttribNoCheck(i);
							if (attr->name.SetTo(nns))
							{
								if (nns->EqualsICase(UTF8STRC("X")))
								{
									minX = Text::String::OrEmpty(attr->value)->ToDoubleOr(0);
								}
								else if (nns->EqualsICase(UTF8STRC("Y")))
								{
									minY = Text::String::OrEmpty(attr->value)->ToDoubleOr(0);
								}
							}
						}
						reader->SkipElement();
					}
					else if (nodeName->EqualsICase(UTF8STRC("OVERLAYXY")))
					{
						i = reader->GetAttribCount();
						while (i-- > 0)
						{
							attr = reader->GetAttribNoCheck(i);
							if (attr->name.SetTo(nns))
							{
								if (nns->EqualsICase(UTF8STRC("X")))
								{
									oX = Text::String::OrEmpty(attr->value)->ToDoubleOr(0);
								}
								else if (nns->EqualsICase(UTF8STRC("Y")))
								{
									oY = Text::String::OrEmpty(attr->value)->ToDoubleOr(0);
								}
							}
						}
						reader->SkipElement();
					}
					else if (nodeName->EqualsICase(UTF8STRC("SIZE")))
					{
						i = reader->GetAttribCount();
						while (i--)
						{
							attr = reader->GetAttribNoCheck(i);
							if (attr->name.SetTo(nns))
							{
								if (nns->EqualsICase(UTF8STRC("X")))
								{
									sizeX = Text::String::OrEmpty(attr->value)->ToDoubleOr(0);
								}
								else if (nns->EqualsICase(UTF8STRC("Y")))
								{
									sizeY = Text::String::OrEmpty(attr->value)->ToDoubleOr(0);
								}
							}
						}
						reader->SkipElement();
					}
					else if (nodeName->EqualsICase(UTF8STRC("ALTITUDE")))
					{
						sb.ClearStr();
						reader->ReadNodeText(sb);
						altitude = sb.ToDoubleOrNAN();
						hasAltitude = true;
					}
					else
					{
						reader->SkipElement();
					}
				}
				if (sbuff[0] != 0)
				{
					imgLyr->AddImage(name->ToCString(), CSTRP(sbuff, sbuffEnd), zIndex, minX, minY, oX, oY, sizeX, sizeY, true, timeStart, timeEnd, ((color >> 24) & 0xff) / 255.0, hasAltitude, altitude);
				}
				name->Release();
			}
			else
			{
				reader->SkipElement();
			}
		}
		else if (nodeName->EqualsICase(UTF8STRC("GROUNDOVERLAY")))
		{
			if (browser.SetTo(nnbrowser) && parsers.SetTo(nnparsers))
			{
				if (imgLyr == 0)
				{
					NEW_CLASS(imgLyr, Map::WebImageLayer(nnbrowser, nnparsers, sourceName, Math::CoordinateSystemManager::CreateWGS84Csys(), containerNameSb.ToCString()));
				}

				NN<Text::String> name = Text::String::NewEmpty();
				Int32 zIndex = 0;
				Double minX = 0;
				Double minY = 0;
				Double maxX = 0;
				Double maxY = 0;
				Double alpha = -1;
				Int64 timeStart = 0;
				Int64 timeEnd = 0;
				Bool hasAltitude = false;
				Double altitude = 0;
				sbuff[0] = 0;
				sbuffEnd = sbuff;
				while (reader->NextElementName().SetTo(nodeName))
				{
					if (nodeName->EqualsICase(UTF8STRC("NAME")))
					{
						sb.ClearStr();
						reader->ReadNodeText(sb);
						name->Release();
						name = Text::String::New(sb.ToCString());
					}
					else if (nodeName->EqualsICase(UTF8STRC("COLOR")))
					{
						sb.ClearStr();
						reader->ReadNodeText(sb);
						alpha = ((Text::StrHex2Int32C(sb.ToString()) >> 24) & 0xff) / 255.0;
					}
					else if (nodeName->EqualsICase(UTF8STRC("DRAWORDER")))
					{
						sb.ClearStr();
						reader->ReadNodeText(sb);
						zIndex = Text::StrToInt32(sb.ToString());
					}
					else if (nodeName->EqualsICase(UTF8STRC("ICON")))
					{
						while (reader->NextElementName().SetTo(nodeName))
						{
							if (nodeName->EqualsICase(UTF8STRC("HREF")))
							{
								sb.ClearStr();
								reader->ReadNodeText(sb);
								sbuffEnd = imgLyr->GetSourceName(sbuff);
								sbuffEnd = Text::URLString::AppendURLPath(sbuff, sbuffEnd, sb.ToCString()).Or(sbuff);
							}
							else
							{
								reader->SkipElement();
							}
						}
					}
					else if (nodeName->EqualsICase(UTF8STRC("TIMESPAN")))
					{
						while (reader->NextElementName().SetTo(nodeName))
						{
							if (nodeName->EqualsICase(UTF8STRC("BEGIN")))
							{
								sb.ClearStr();
								reader->ReadNodeText(sb);
								dt.SetValue(sb.ToCString());
								timeStart = dt.ToUnixTimestamp();
							}
							else if (nodeName->EqualsICase(UTF8STRC("END")))
							{
								sb.ClearStr();
								reader->ReadNodeText(sb);
								dt.SetValue(sb.ToCString());
								timeEnd = dt.ToUnixTimestamp();
							}
							else
							{
								reader->SkipElement();
							}
						}
					}
					else if (nodeName->EqualsICase(UTF8STRC("LATLONBOX")))
					{
						while (reader->NextElementName().SetTo(nodeName))
						{
							if (nodeName->EqualsICase(UTF8STRC("NORTH")))
							{
								sb.ClearStr();
								reader->ReadNodeText(sb);
								maxY = sb.ToDoubleOr(0);
							}
							else if (nodeName->EqualsICase(UTF8STRC("SOUTH")))
							{
								sb.ClearStr();
								reader->ReadNodeText(sb);
								minY = sb.ToDoubleOr(0);
							}
							else if (nodeName->EqualsICase(UTF8STRC("EAST")))
							{
								sb.ClearStr();
								reader->ReadNodeText(sb);
								maxX = sb.ToDoubleOr(0);
							}
							else if (nodeName->EqualsICase(UTF8STRC("WEST")))
							{
								sb.ClearStr();
								reader->ReadNodeText(sb);
								minX = sb.ToDoubleOr(0);
							}
							else
							{
								reader->SkipElement();
							}
						}
						while (maxX < -180)
						{
							minX += 360;
							maxX += 360;
						}
						while (minX >= 180)
						{
							minX -= 360;
							maxX -= 360;
						}
					}
					else if (nodeName->EqualsICase(UTF8STRC("ALTITUDE")))
					{
						sb.ClearStr();
						reader->ReadNodeText(sb);
						hasAltitude = true;
						altitude = sb.ToDoubleOrNAN();
					}
					else
					{
						reader->SkipElement();
					}
				}
				if (sbuff[0] != 0)
				{
					imgLyr->AddImage(name->ToCString(), CSTRP(sbuff, sbuffEnd), zIndex, minX, minY, maxX, maxY, 0, 0, false, timeStart, timeEnd, alpha, hasAltitude, altitude);
				}
				name->Release();
			}
			else
			{
				reader->SkipElement();
			}
		}
		else if (nodeName->EqualsICase(UTF8STRC("FOLDER")))
		{
			if (ParseKMLContainer(reader, styles, sourceName, parsers, browser, basePF, false).SetTo(lyr))
			{
				layers.Add(lyr);
			}
		}
		else if (nodeName->EqualsICase(UTF8STRC("DOCUMENT")))
		{
			if (ParseKMLContainer(reader, styles, sourceName, parsers, browser, basePF, false).SetTo(lyr))
			{
				layers.Add(lyr);
			}
		}
		else if (nodeName->EqualsICase(UTF8STRC("NAME")))
		{
			while (reader->ReadNext())
			{
				if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
				{
					break;
				}
				else if (reader->GetNodeType() == Text::XMLNode::NodeType::Text)
				{
					containerNameSb.ClearStr();
					containerNameSb.AppendOpt(reader->GetNodeText());
				}
				else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
				{
					reader->SkipElement();
				}
			}
		}
		else
		{
			reader->SkipElement();
		}
	}

	if (lyr.Set(imgLyr))
	{
		layers.Add(lyr);
	}
	if (layers.GetCount() <= 0)
	{
		return 0;
	}
	else if (rootKml && layers.GetCount() == 1)
	{
		return layers.GetItem(0);
	}
	else
	{
		Map::MapLayerCollection *lyrColl;
		NEW_CLASS(lyrColl, Map::MapLayerCollection(sourceName, containerNameSb.ToCString()));
		Data::ArrayIterator<NN<Map::MapDrawLayer>> it = layers.Iterator();
		while (it.HasNext())
		{
			lyrColl->Add(it.Next());
			i++;
		}
		return lyrColl;
	}
}

void Map::KMLXML::ParseKMLPlacemarkTrack(NN<Text::XMLReader> reader, NN<Map::GPSTrack> lyr, Data::StringMap<KMLStyle*> *styles)
{
	Data::ArrayListStringNN timeList;
	Data::ArrayListStringNN coordList;
	Bool lastTrack = false;
	NN<Text::String> nodeName;
	while (reader->NextElementName().SetTo(nodeName))
	{
		if (nodeName->EqualsICase(UTF8STRC("GX:MULTITRACK")))
		{
			while (reader->NextElementName().SetTo(nodeName))
			{
				if (nodeName->EqualsICase(UTF8STRC("GX:TRACK")))
				{
					Text::StringBuilderUTF8 sb;
					UTF8Char sbuff[256];
					UnsafeArray<UTF8Char> strs[4];
					if (lastTrack)
					{
						lyr->NewTrack();
					}

					while (reader->NextElementName().SetTo(nodeName))
					{
						if (nodeName->EqualsICase(UTF8STRC("WHEN")))
						{
							sb.ClearStr();
							reader->ReadNodeText(sb);
							timeList.Add(Text::String::New(sb.ToString(), sb.GetLength()));
						}
						else if (nodeName->EqualsICase(UTF8STRC("GX:COORD")))
						{
							sb.ClearStr();
							reader->ReadNodeText(sb);
							coordList.Add(Text::String::New(sb.ToString(), sb.GetLength()));
						}
						else if (nodeName->EqualsICase(UTF8STRC("EXTENDEDDATA")))
						{
							if (timeList.GetCount() == coordList.GetCount())
							{
								Map::GPSTrack::GPSRecord3 rec;
								rec.heading = 0;
								rec.nSateUsed = 0;
								rec.nSateUsedGPS = 0;
								rec.nSateUsedSBAS = 0;
								rec.nSateUsedGLO = 0;
								rec.nSateViewGPS = 0;
								rec.nSateViewGLO = 0;
								rec.nSateViewGA = 0;
								rec.nSateViewQZSS = 0;
								rec.nSateViewBD = 0;
								rec.speed = 0;
								rec.valid = true;
								Data::DateTime dt;
								UOSInt i = 0;
								UOSInt j = timeList.GetCount();
								while (i < j)
								{
									NN<Text::String> s;
									if (timeList.GetItem(i).SetTo(s))
									{
										dt.SetValue(s->ToCString());
										rec.recTime = dt.ToInstant();
									}
									
									if (coordList.GetItem(i).SetTo(s))
									{
										s->ConcatTo(sbuff);
										if (Text::StrSplit(strs, 4, sbuff, ' ') == 3)
										{
											rec.pos = Math::Coord2DDbl(Text::StrToDoubleOrNAN(strs[1]), Text::StrToDoubleOrNAN(strs[0]));
											rec.altitude = Text::StrToDoubleOrNAN(strs[2]);
											lyr->AddRecord(rec);
										}
									}
									OPTSTR_DEL(timeList.GetItem(i));
									OPTSTR_DEL(coordList.GetItem(i));
									i++;
								}
								timeList.Clear();
								coordList.Clear();
							}
							else
							{
								UOSInt i = timeList.GetCount();
								while (i-- > 0)
								{
									OPTSTR_DEL(timeList.GetItem(i));
								}
								i = coordList.GetCount();
								while (i-- > 0)
								{
									OPTSTR_DEL(coordList.GetItem(i));
								}
								timeList.Clear();
								coordList.Clear();
							}
							UOSInt recCnt;
							UnsafeArray<Map::GPSTrack::GPSRecordFull> recs;
							if (lyr->GetTrack(0, recCnt).SetTo(recs))
							{
								while (reader->NextElementName().SetTo(nodeName))
								{
									if (nodeName->EqualsICase(UTF8STRC("SCHEMADATA")))
									{
										while (reader->NextElementName().SetTo(nodeName))
										{
											if (nodeName->EqualsICase(UTF8STRC("GX:SIMPLEARRAYDATA")))
											{
												NN<Text::XMLAttrib> attr;
												Text::StringBuilderUTF8 sb;
												NN<Text::String> nnval;
												Bool found = false;
												UOSInt i;
												UOSInt j;
												i = reader->GetAttribCount();
												while (i-- > 0)
												{
													attr = reader->GetAttribNoCheck(i);
													if (Text::String::OrEmpty(attr->name)->EqualsICase(UTF8STRC("NAME")) && attr->value.SetTo(nnval))
													{
														if (nnval->EqualsICase(UTF8STRC("SPEED")))
														{
															j = 0;
															while (reader->NextElementName().SetTo(nodeName))
															{
																if (nodeName->EqualsICase(UTF8STRC("GX:VALUE")))
																{
																	sb.ClearStr();
																	reader->ReadNodeText(sb);
																	if (j < recCnt)
																	{
																		recs[j].speed = sb.ToDoubleOr(0);
																	}
																	j++;
																}
																else
																{
																	reader->SkipElement();
																}
															}
														}
														else if (nnval->EqualsICase(UTF8STRC("BEARING")))
														{
															j = 0;
															while (reader->NextElementName().SetTo(nodeName))
															{
																if (nodeName->EqualsICase(UTF8STRC("GX:VALUE")))
																{
																	sb.ClearStr();
																	reader->ReadNodeText(sb);
																	if (j < recCnt)
																	{
																		recs[j].heading = sb.ToDoubleOr(0);
																	}
																	j++;
																}
																else
																{
																	reader->SkipElement();
																}
															}
														}
														else if (nnval->EqualsICase(UTF8STRC("ACCURACY")))
														{
															j = 0;
															while (reader->NextElementName().SetTo(nodeName))
															{
																if (nodeName->EqualsICase(UTF8STRC("GX:VALUE")))
																{
																	sb.ClearStr();
																	reader->ReadNodeText(sb);
																	if (j < recCnt)
																	{
																		recs[j].nSateUsed = (UInt8)Double2Int32(sb.ToDoubleOr(0));
																		recs[j].nSateUsedGPS = recs[j].nSateUsed;
																	}
																	j++;
																}
																else
																{
																	reader->SkipElement();
																}
															}
														}
														else
														{
															reader->SkipElement();
														}
														found = true;
														break;
													}
												}
												if (!found)
												{
													reader->SkipElement();
												}
											}
											else
											{
												reader->SkipElement();
											}
										}
									}
									else
									{
										reader->SkipElement();
									}
								}
							}
							else
							{
								reader->SkipElement();
							}
						}
						else
						{
							reader->SkipElement();
						}
					}

					if (timeList.GetCount() == coordList.GetCount())
					{
						Map::GPSTrack::GPSRecord3 rec;
						rec.heading = 0;
						rec.nSateUsed = 0;
						rec.nSateUsedGPS = 0;
						rec.nSateUsedGLO = 0;
						rec.nSateUsedSBAS = 0;
						rec.nSateViewGPS = 0;
						rec.nSateViewGLO = 0;
						rec.nSateViewGA = 0;
						rec.nSateViewQZSS = 0;
						rec.nSateViewBD = 0;
						rec.speed = 0;
						rec.valid = true;
						Data::DateTime dt;
						UOSInt i = 0;
						UOSInt j = timeList.GetCount();
						while (i < j)
						{
							NN<Text::String> s;
							if (timeList.GetItem(i).SetTo(s))
							{
								dt.SetValue(s->ToCString());
								rec.recTime = dt.ToInstant();
							}
							
							if (coordList.GetItem(i).SetTo(s))
							{
								s->ConcatTo(sbuff);
								if (Text::StrSplit(strs, 4, sbuff, ' ') == 3)
								{
									rec.pos = Math::Coord2DDbl(Text::StrToDoubleOrNAN(strs[1]), Text::StrToDoubleOrNAN(strs[0]));
									rec.altitude = Text::StrToDoubleOrNAN(strs[2]);
									lyr->AddRecord(rec);
								}
							}
							OPTSTR_DEL(timeList.GetItem(i));
							OPTSTR_DEL(coordList.GetItem(i));
							i++;
						}
						timeList.Clear();
						coordList.Clear();
					}
					else
					{
						UOSInt i = timeList.GetCount();
						while (i-- > 0)
						{
							OPTSTR_DEL(timeList.GetItem(i));
						}
						i = coordList.GetCount();
						while (i-- > 0)
						{
							OPTSTR_DEL(coordList.GetItem(i));
						}
						timeList.Clear();
						coordList.Clear();
					}

					lastTrack = true;
				}
				else
				{
					reader->SkipElement();
				}
			}
		}
		else if (nodeName->EqualsICase(UTF8STRC("GX:TRACK")))
		{
			Text::StringBuilderUTF8 sb;
			if (lastTrack)
			{
				lyr->NewTrack();
			}
			while (reader->NextElementName().SetTo(nodeName))
			{
				if (nodeName->EqualsICase(UTF8STRC("WHEN")))
				{
					sb.ClearStr();
					reader->ReadNodeText(sb);
					timeList.Add(Text::String::New(sb.ToString(), sb.GetLength()));
				}
				else if (nodeName->EqualsICase(UTF8STRC("GX:COORD")))
				{
					sb.ClearStr();
					reader->ReadNodeText(sb);
					coordList.Add(Text::String::New(sb.ToString(), sb.GetLength()));
				}
				else
				{
					reader->SkipElement();
				}
			}

			if (timeList.GetCount() == coordList.GetCount())
			{
				Map::GPSTrack::GPSRecord3 rec;
				rec.heading = 0;
				rec.nSateUsed = 0;
				rec.nSateUsedGPS = 0;
				rec.nSateUsedGLO = 0;
				rec.nSateUsedSBAS = 0;
				rec.nSateViewGPS = 0;
				rec.nSateViewGLO = 0;
				rec.nSateViewGA = 0;
				rec.nSateViewQZSS = 0;
				rec.nSateViewBD = 0;
				rec.speed = 0;
				rec.valid = true;
				Data::DateTime dt;
				UTF8Char sbuff[256];
				UnsafeArray<UTF8Char> strs[4];
				UOSInt i = 0;
				UOSInt j = timeList.GetCount();
				while (i < j)
				{
					NN<Text::String> s;
					if (timeList.GetItem(i).SetTo(s))
					{
						dt.SetValue(s->ToCString());
						rec.recTime = dt.ToInstant();
					}
					
					if (coordList.GetItem(i).SetTo(s))
					{
						s->ConcatTo(sbuff);
						if (Text::StrSplit(strs, 4, sbuff, ' ') == 3)
						{
							rec.pos.x = Text::StrToDoubleOrNAN(strs[0]);
							rec.pos.y = Text::StrToDoubleOrNAN(strs[1]);
							rec.altitude = Text::StrToDoubleOrNAN(strs[2]);
							lyr->AddRecord(rec);
						}
					}
					OPTSTR_DEL(timeList.GetItem(i));
					OPTSTR_DEL(coordList.GetItem(i));
					i++;
				}
			}
			else
			{
				UOSInt i = timeList.GetCount();
				while (i-- > 0)
				{
					OPTSTR_DEL(timeList.GetItem(i));
				}
				i = coordList.GetCount();
				while (i-- > 0)
				{
					OPTSTR_DEL(coordList.GetItem(i));
				}
			}
			timeList.Clear();
			coordList.Clear();

			lastTrack = true;
		}
		else if (nodeName->EqualsICase(UTF8STRC("STYLEURL")))
		{
			if (!lyr->HasLineStyle())
			{
				Text::StringBuilderUTF8 sb;
				reader->ReadNodeText(sb);
				if (sb.StartsWith(UTF8STRC("#")))
				{
					KMLStyle *style = styles->Get({sb.ToString() + 1, sb.GetLength() - 1});
					if (style && style->lineWidth != 0 && style->flags & 1)
					{
						lyr->SetLineStyle(style->lineColor, style->lineWidth);
					}
				}
			}
		}
		else
		{
			reader->SkipElement();
		}
	}
}

Optional<Map::MapDrawLayer> Map::KMLXML::ParseKMLPlacemarkLyr(NN<Text::XMLReader> reader, Data::StringMap<KMLStyle*> *styles, Text::CStringNN sourceName, Optional<Parser::ParserList> parsers, Optional<Net::WebBrowser> browser, Optional<IO::PackageFile> basePF)
{
	Text::StringBuilderUTF8 sb;
	Data::ArrayListStringNN colNames;
	Data::ArrayListStringNN colValues;
	Data::ArrayList<Map::VectorLayer::ColInfo> colInfos;
	KMLStyle *style = 0;
	Data::ArrayListNN<Map::MapDrawLayer> layers;
	NN<Math::Geometry::Vector2D> vec;
	colNames.Add(Text::String::New(UTF8STRC("Name")));
	colValues.Add(Text::String::New(UTF8STRC("Layer")));
	colInfos.Add(Map::VectorLayer::ColInfo(DB::DBUtil::CT_VarUTF8Char, 256, 0));
	NN<Text::String> s;
	NN<Text::String> nodeText;
	while (reader->NextElementName().SetTo(nodeText))
	{
		if (nodeText->EqualsICase(UTF8STRC("NAME")))
		{
			sb.ClearStr();
			reader->ReadNodeText(sb);
			if (colValues.SetItem(0, Text::String::New(sb.ToCString())).SetTo(s))
			{
				s->Release();
			}
		}
		else if (nodeText->EqualsICase(UTF8STRC("STYLEURL")))
		{
			sb.ClearStr();
			reader->ReadNodeText(sb);
			if (sb.ToString()[0] == '#')
			{
				style = styles->Get({sb.ToString() + 1, sb.GetLength() - 1});
			}
		}
		else if (nodeText->EqualsICase(UTF8STRC("GX:MULTITRACK")))
		{
			NN<Map::GPSTrack> lyr;
			if (colValues.GetItem(0).SetTo(s))
			{
				NEW_CLASSNN(lyr, Map::GPSTrack(sourceName, true, 65001, s->ToCString()));
				ParseKMLPlacemarkTrack(reader, lyr, styles);
				if (style)
				{
					if (style->lineColor != 0 && style->flags & 1)
					{
						lyr->SetLineStyle(style->lineColor, style->lineWidth);
					}
				}
				layers.Add(lyr);
			}
		}
		else if (ParseKMLVector(reader, colNames, colValues, colInfos).SetTo(vec))
		{
			Math::Geometry::Vector2D::VectorType vecType = vec->GetVectorType();
			if (vecType == Math::Geometry::Vector2D::VectorType::LineString || vecType == Math::Geometry::Vector2D::VectorType::Polyline)
			{
				NN<Map::VectorLayer> lyr;
				if (colValues.GetItem(0).SetTo(s))
				{
					NEW_CLASSNN(lyr, Map::VectorLayer(vec->HasZ()?Map::DRAW_LAYER_POLYLINE3D:Map::DRAW_LAYER_POLYLINE, sourceName, colNames, Math::CoordinateSystemManager::CreateWGS84Csys(), colInfos, 0, s->ToCString()));
					lyr->AddVector2(vec, colValues);
					if (style)
					{
						if (style->flags & 1)
						{
							if (style->lineWidth == 0)
							{
								lyr->SetLineStyle(style->lineColor, 1);
							}
							else
							{
								lyr->SetLineStyle(style->lineColor, style->lineWidth);
							}
						}
					}
					layers.Add(lyr);
				}
			}
			else if (Math::Geometry::Vector2D::VectorTypeIsPoint(vecType))
			{
				NN<Map::VectorLayer> lyr;
				NN<Parser::ParserList> nnparsers;
				NN<Net::WebBrowser> nnbrowser;
				if (colValues.GetItem(0).SetTo(s))
				{
					NEW_CLASSNN(lyr, Map::VectorLayer(Map::DRAW_LAYER_POINT, sourceName, colNames, Math::CoordinateSystemManager::CreateWGS84Csys(), colInfos, 0, s->ToCString()));
					lyr->SetLabelVisible(true);
					lyr->AddVector2(vec, colValues);

					if (style && style->iconURL && parsers.SetTo(nnparsers))
					{
						if (style->img == 0)
						{
							Optional<IO::StreamData> fd = 0;
							NN<IO::PackageFile> nnbasePF;
							if (basePF.SetTo(nnbasePF))
							{
								fd = nnbasePF->OpenStreamData(style->iconURL->ToCString());
							}
							if (fd.IsNull() && browser.SetTo(nnbrowser))
							{
								fd = nnbrowser->GetData(style->iconURL->ToCString(), false, 0);
							}
							NN<IO::StreamData> nnfd;
							if (fd.SetTo(nnfd))
							{
								NN<Media::ImageList> imgList;
								if (Optional<Media::ImageList>::ConvertFrom(nnparsers->ParseFileType(nnfd, IO::ParserType::ImageList)).SetTo(imgList))
								{
									if (style->iconColor != 0)
									{
										UOSInt j = imgList->GetCount();
										while (j-- > 0)
										{
											imgList->ToStaticImage(j);
											NN<Media::StaticImage> img;
											if (Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(j, 0)).SetTo(img))
												img->MultiplyColor(style->iconColor);
										}
									}
									NEW_CLASS(style->img, Media::SharedImage(imgList, 0));
								}
								nnfd.Delete();
							}
						}
						NN<Media::SharedImage> shimg;
						NN<Media::StaticImage> img;
						if (shimg.Set(style->img) && shimg->GetImage(0).SetTo(img))
						{
							if (style->iconSpotX == -1 || style->iconSpotY == -1)
							{
								lyr->SetIconStyle(shimg, (OSInt)(img->info.dispSize.x >> 1), (OSInt)(img->info.dispSize.y >> 1));
							}
							else
							{
								lyr->SetIconStyle(shimg, style->iconSpotX, (OSInt)img->info.dispSize.y - style->iconSpotY);
							}
						}
					}
					layers.Add(lyr);
				}
			}
			else if (vecType == Math::Geometry::Vector2D::VectorType::Polygon || vecType == Math::Geometry::Vector2D::VectorType::MultiPolygon)
			{
				NN<Map::VectorLayer> lyr;
				if (colValues.GetItem(0).SetTo(s))
				{
					NEW_CLASSNN(lyr, Map::VectorLayer(Map::DRAW_LAYER_POLYGON, sourceName, colNames, Math::CoordinateSystemManager::CreateWGS84Csys(), colInfos, 0, s->ToCString()));
					lyr->AddVector2(vec, colValues);

					if (style)
					{
						if (style->flags & 1)
						{
							if (style->lineWidth == 0)
							{
								lyr->SetLineStyle(style->lineColor, 1);
							}
							else
							{
								lyr->SetLineStyle(style->lineColor, style->lineWidth);
							}
						}
						if (style->flags & 4)
						{
							lyr->SetPGStyle(style->fillColor);
						}
					}
					layers.Add(lyr);
				}
			}
			else
			{
#if defined(VERBOSE)
				printf("KMLXML: ParseKMLPlacemarkLyr unsupport vector type: %s\r\n", Math::Geometry::Vector2D::VectorTypeGetName(vec->GetVectorType()).v.Ptr());
#endif
				vec.Delete();
			}
		}
	}
	colNames.FreeAll();
	colValues.FreeAll();
	if (layers.GetCount() == 1)
	{
		return layers.GetItem(0);
	}
	UOSInt i = layers.GetCount();
	while (i-- > 0)
	{
		layers.GetItem(i).Delete();
	}
	return 0;
}

Optional<Math::Geometry::Vector2D> Map::KMLXML::ParseKMLVector(NN<Text::XMLReader> reader, NN<Data::ArrayListStringNN> colNames, NN<Data::ArrayListStringNN> colValues, NN<Data::ArrayList<Map::VectorLayer::ColInfo>> colInfos)
{
	NN<Text::String> nodeText = Text::String::OrEmpty(reader->GetNodeOriText());
	Optional<Math::Geometry::Vector2D> vec = 0;
	if (nodeText->EqualsICase(UTF8STRC("LINESTRING")))
	{
		while (reader->NextElementName().SetTo(nodeText))
		{
			if (nodeText->EqualsICase(UTF8STRC("COORDINATES")))
			{
				Data::ArrayListA<Math::Coord2DDbl> coord;
				Data::ArrayListDbl altList;
				UTF8Char c;
				UnsafeArray<UTF8Char> sptr;
				UnsafeArray<UTF8Char> sptr2;
				UTF8Char sbuff[256];
				UnsafeArray<UTF8Char> sarr[4];
				UOSInt i;

				Text::StringBuilderUTF8 sb;
				reader->ReadNodeText(sb);
				sptr = sb.v;
				while (true)
				{
					while ((c = *sptr) != 0)
					{
						if (c == 0x20 || c == 13 || c == 10)
						{
							sptr++;
						}
						else
						{
							break;
						}
					}
					if (c == 0)
						break;

					sptr2 = sptr;
					while ((c = *sptr2) != 0)
					{
						if (c == 0x20 || c == 13 || c == 10)
							break;
						sptr2++;
					}
					*sptr2 = 0;
					Text::StrConcatC(sbuff, sptr, (UOSInt)(sptr2 - sptr));
					*sptr2 = c;
					sptr = sptr2;
					i = Text::StrSplit(sarr, 4, sbuff, ',');
					if (i == 3)
					{
						coord.Add(Math::Coord2DDbl(Text::StrToDoubleOrNAN(sarr[0]), Text::StrToDoubleOrNAN(sarr[1])));
						altList.Add(Text::StrToDoubleOrNAN(sarr[2]));
					}
					else if (i == 2)
					{
						coord.Add(Math::Coord2DDbl(Text::StrToDoubleOrNAN(sarr[0]), Text::StrToDoubleOrNAN(sarr[1])));
					}
				}
				if (coord.GetCount() > 0)
				{
					NN<Math::Geometry::LineString> pl;
					UOSInt nPoints;
					UnsafeArray<Math::Coord2DDbl> ptArr;
					UnsafeArray<Double> altArr;

					NEW_CLASSNN(pl, Math::Geometry::LineString(4326, coord.GetCount(), coord.GetCount() == altList.GetCount(), false));
					ptArr = pl->GetPointList(nPoints);
					i = coord.GetCount();
					MemCopyAC(ptArr.Ptr(), coord.Arr().Ptr(), sizeof(Math::Coord2DDbl) * i);
					if (pl->GetZList(nPoints).SetTo(altArr))
					{
						MemCopyAC(altArr.Ptr(), altList.Arr().Ptr(), sizeof(Double) * i);
					}
					vec.Delete();
					vec = Optional<Math::Geometry::Vector2D>(pl);
				}
			}
			else
			{
				reader->SkipElement();
			}
		}
		return vec;
	}
	else if (nodeText->EqualsICase(UTF8STRC("POINT")))
	{
		while (reader->NextElementName().SetTo(nodeText))
		{
			if (nodeText->EqualsICase(UTF8STRC("COORDINATES")))
			{
				Text::StringBuilderUTF8 sb;
				reader->ReadNodeText(sb);

				Double x;
				Double y;
				Double z;
				UOSInt i;
				UnsafeArray<UTF8Char> sarr[4];
				i = Text::StrSplitTrim(sarr, 4, sb.v, ',');
				if (i == 3)
				{
					NN<Math::Geometry::PointZ> pt;
					x = Text::StrToDoubleOrNAN(sarr[0]);
					y = Text::StrToDoubleOrNAN(sarr[1]);
					z = Text::StrToDoubleOrNAN(sarr[2]);
					NEW_CLASSNN(pt, Math::Geometry::PointZ(4326, x, y, z));
					vec.Delete();
					vec = Optional<Math::Geometry::Vector2D>(pt);
				}
				else if (i == 2)
				{
					NN<Math::Geometry::PointZ> pt;
					x = Text::StrToDoubleOrNAN(sarr[0]);
					y = Text::StrToDoubleOrNAN(sarr[1]);
					NEW_CLASSNN(pt, Math::Geometry::PointZ(4326, x, y, 0));
					vec.Delete();
					vec = Optional<Math::Geometry::Vector2D>(pt);
				}
			}
			else
			{
				reader->SkipElement();
			}
		}
		return vec;
	}
	else if (nodeText->EqualsICase(UTF8STRC("POLYGON")))
	{
		NN<Math::Geometry::Polygon> pg;
		NEW_CLASSNN(pg, Math::Geometry::Polygon(4326));

		Data::ArrayListA<Math::Coord2DDbl> coord;
		Data::ArrayListDbl altList;
		while (reader->NextElementName().SetTo(nodeText))
		{
			coord.Clear();
			altList.Clear();
			if (nodeText->EqualsICase(UTF8STRC("OUTERBOUNDARYIS")))
			{
				ParseCoordinates(reader, coord, altList);
			}
			else if (nodeText->EqualsICase(UTF8STRC("INNERBOUNDARYIS")))
			{
				ParseCoordinates(reader, coord, altList);
			}
			else
			{
				reader->SkipElement();
			}
			if (coord.GetCount() > 0)
			{
				NN<Math::Geometry::LinearRing> lr;
				UOSInt nPoints;
				UnsafeArray<Math::Coord2DDbl> ptArr;
				UnsafeArray<Double> zList;

				NEW_CLASSNN(lr, Math::Geometry::LinearRing(4326, coord.GetCount(), altList.GetCount() == coord.GetCount(), false));
				ptArr = lr->GetPointList(nPoints);
				MemCopyNO(ptArr.Ptr(), coord.Arr().Ptr(), sizeof(Math::Coord2DDbl) * coord.GetCount());
				if (altList.GetCount() == coord.GetCount() && lr->GetZList(nPoints).SetTo(zList))
				{
					MemCopyNO(zList.Ptr(), altList.Arr().Ptr(), sizeof(Double) * nPoints);
				}
				pg->AddGeometry(lr);
			}
		}
		if (pg->GetCount() > 0)
		{
			vec.Delete();
			vec = Optional<Math::Geometry::Vector2D>(pg);
		}
		else
		{
			pg.Delete();
		}
		return vec;
	}
	else if (nodeText->EqualsICase(UTF8STRC("MULTIGEOMETRY")))
	{
		Data::ArrayListNN<Math::Geometry::Vector2D> vecList;
		NN<Math::Geometry::Vector2D> innerVec;
		while (!reader->NextElementName().IsNull())
		{
			if (ParseKMLVector(reader, colNames, colValues, colInfos).SetTo(innerVec))
			{
				vecList.Add(innerVec);
			}
		}
		if (vecList.GetCount() == 0)
			return 0;
		
		Bool allPG = true;
		Bool allPL = true;
//		Bool allPT = true;
		NN<Math::Geometry::Vector2D> v;
		Data::ArrayIterator<NN<Math::Geometry::Vector2D>> it = vecList.Iterator();
		while (it.HasNext())
		{
			v = it.Next();
			if (Math::Geometry::Vector2D::VectorTypeIsPoint(v->GetVectorType()))
			{
				allPG = false;
				allPL = false;
			}
			else if (v->GetVectorType() == Math::Geometry::Vector2D::VectorType::LineString)
			{
				allPG = false;
//				allPT = false;
			}
			else if (v->GetVectorType() == Math::Geometry::Vector2D::VectorType::Polygon)
			{
				allPL = false;
//				allPT = false;
			}
			else
			{
				allPG = false;
				allPL = false;
//				allPT = false;
			}
		}
		if (allPG)
		{
			NN<Math::Geometry::MultiPolygon> mpg;
			NEW_CLASSNN(mpg, Math::Geometry::MultiPolygon(4326));
			it = vecList.Iterator();
			while (it.HasNext())
			{
				mpg->AddGeometry(NN<Math::Geometry::Polygon>::ConvertFrom(it.Next()));
			}
			return Optional<Math::Geometry::MultiPolygon>(mpg);
		}
		else if (allPL)
		{
			NN<Math::Geometry::Polyline> pl;
			NEW_CLASSNN(pl, Math::Geometry::Polyline(4326));
			it = vecList.Iterator();
			while (it.HasNext())
			{
				pl->AddGeometry(NN<Math::Geometry::LineString>::ConvertFrom(it.Next()));
			}
			return Optional<Math::Geometry::Polyline>(pl);
		}
		else
		{
			NN<Math::Geometry::GeometryCollection> mpg;
			NEW_CLASSNN(mpg, Math::Geometry::GeometryCollection(4326));
			it = vecList.Iterator();
			while (it.HasNext())
			{
				mpg->AddGeometry(it.Next());
			}
			return Optional<Math::Geometry::GeometryCollection>(mpg);
		}
	}
	else
	{
		colNames->Add(nodeText->Clone());
		Text::StringBuilderUTF8 sb;
		reader->ReadNodeText(sb);
		colValues->Add(Text::String::New(sb.ToCString()));
		UOSInt len = sb.leng;
		if (len < 256)
			len = 256;
		colInfos->Add(Map::VectorLayer::ColInfo(DB::DBUtil::CT_VarUTF8Char, len, 0));
	}
	return 0;
}

void Map::KMLXML::ParseCoordinates(NN<Text::XMLReader> reader, NN<Data::ArrayListA<Math::Coord2DDbl>> coordList, NN<Data::ArrayList<Double>> altList)
{
	UOSInt i;
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	UTF8Char c;
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sarr[4];
	NN<Text::String> nodeName;

	while (reader->NextElementName().SetTo(nodeName))
	{
		if (nodeName->EqualsICase(UTF8STRC("LINEARRING")))
		{
			ParseCoordinates(reader, coordList, altList);
		}
		else if (nodeName->EqualsICase(UTF8STRC("COORDINATES")))
		{
			Text::StringBuilderUTF8 sb;
			reader->ReadNodeText(sb);
			sptr = sb.v;
			while (true)
			{
				while ((c = *sptr) != 0)
				{
					if (c == 0x20 || c == 13 || c == 10 || c == 9)
					{
						sptr++;
					}
					else
					{
						break;
					}
				}
				if (c == 0)
					break;

				sptr2 = sptr;
				while ((c = *sptr2) != 0)
				{
					if (c == 0x20 || c == 13 || c == 10 || c == 9)
						break;
					sptr2++;
				}
				*sptr2 = 0;
				Text::StrConcatC(sbuff, sptr, (UOSInt)(sptr2 - sptr));
				*sptr2 = c;
				sptr = sptr2;
				i = Text::StrSplit(sarr, 4, sbuff, ',');
				if (i == 3)
				{
					coordList->Add(Math::Coord2DDbl(Text::StrToDoubleOrNAN(sarr[0]), Text::StrToDoubleOrNAN(sarr[1])));
					altList->Add(Text::StrToDoubleOrNAN(sarr[2]));
				}
				else if (i == 2)
				{
					coordList->Add(Math::Coord2DDbl(Text::StrToDoubleOrNAN(sarr[0]), Text::StrToDoubleOrNAN(sarr[1])));
				}
			}
		}
		else
		{
			reader->SkipElement();
		}
	}
}
