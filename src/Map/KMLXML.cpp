#include "Stdafx.h"
#include "Data/ArrayListA.h"
#include "Data/ArrayListDbl.h"
#include "Data/ICaseStringMap.h"
#include "Map/KMLXML.h"
#include "Map/MapLayerCollection.h"
#include "Map/ReloadableMapLayer.h"
#include "Map/VectorLayer.h"
#include "Map/WebImageLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Geometry/LineString.h"
#include "Math/Geometry/PointZ.h"
#include "Media/StaticImage.h"
#include "Text/URLString.h"

Map::IMapDrawLayer *Map::KMLXML::ParseKMLRoot(Text::XMLReader *reader, Text::CString fileName, Parser::ParserList *parsers, Net::WebBrowser *browser, IO::PackageFile *pkgFile)
{
	if (reader->GetNodeType() != Text::XMLNode::NodeType::Element)
		return 0;
	if (!reader->GetNodeText()->Equals(UTF8STRC("kml")))
		return 0;
	Data::ICaseStringMap<KMLStyle*> styles;
/*	Data::ArrayList<Map::IMapDrawLayer *> layers;
	Map::IMapDrawLayer *lyr;

	while (reader->ReadNext())
	{
		if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
		{
			break;
		}
		else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
		{
			if (reader->GetNodeText()->EqualsICase(UTF8STRC("DOCUMENT") || reader->GetNodeText()->EqualsICase(UTF8STRC("FOLDER"))
			{
				lyr = ParseKMLContainer(reader, &styles, fileName, parsers, browser, pkgFile);
				if (lyr)
				{
					layers.Add(lyr);
				}
			}
			else
			{
				reader->SkipElement();
			}
		}
	}*/
	Map::IMapDrawLayer *lyr = ParseKMLContainer(reader, &styles, fileName, parsers, browser, pkgFile);

	const Data::ArrayList<KMLStyle*> *styleList = styles.GetValues();
	KMLStyle *style;
/*		UOSInt i;
	UOSInt j;
	const UTF8Char *shortName;
	i = Text::StrLastIndexOfCharC(fileName, IO::Path::PATH_SEPERATOR);
	if (IO::Path::PATH_SEPERATOR == '\\')
	{
		j = Text::StrLastIndexOfCharC(fileName, '/');
		if (i == INVALID_INDEX || (j != INVALID_INDEX && j > i))
		{
			i = j;
		}
	}
	shortName = &fileName[i + 1];*/

	UOSInt ui = styleList->GetCount();
	while (ui-- > 0)
	{
		style = styleList->GetItem(ui);
		SDEL_STRING(style->iconURL);
		SDEL_CLASS(style->img);
		MemFree(style);
	}

/*	if (layers.GetCount() == 0)
	{
	}
	else
	{
		DEL_CLASS(reader);
		Map::MapLayerCollection *lyrColl;
		NEW_CLASS(lyrColl, Map::MapLayerCollection(fileName, shortName));
		i = 0;
		j = layers.GetCount();
		while (i < j)
		{
			lyrColl->Add(layers.GetItem(i));
			i++;
		}
		return lyrColl;
	}*/
	return lyr;
}


Map::IMapDrawLayer *Map::KMLXML::ParseKMLContainer(Text::XMLReader *reader, Data::ICaseStringMap<KMLStyle*> *styles, Text::CString sourceName, Parser::ParserList *parsers, Net::WebBrowser *browser, IO::PackageFile *basePF)
{
	KMLStyle *style;
	UOSInt i;
	Text::XMLAttrib *attr;
	Text::StringBuilderUTF8 sb;
	Data::DateTime dt;
	UTF8Char sbuff[512];
	UTF8Char *sbuffEnd;
	Data::ArrayList<Map::IMapDrawLayer *> layers;

	Map::WebImageLayer *imgLyr = 0;
	Text::StringBuilderUTF8 containerNameSb;
	containerNameSb.Append(sourceName);
	Map::IMapDrawLayer *lyr;
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

	while (reader->ReadNext())
	{
		if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
		{
			break;
		}
		else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
		{
			if (reader->GetNodeText()->EqualsICase(UTF8STRC("NetworkLink")))
			{
				if (parsers && browser)
				{
					Map::ReloadableMapLayer *lyr;
					Text::String *layerName = 0;
					Text::String *url = 0;
					Int32 interval = 0;
					NEW_CLASS(lyr, Map::ReloadableMapLayer(sourceName, parsers, browser, CSTR_NULL));
				
					while (reader->ReadNext())
					{
						if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
						{
							break;
						}
						else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
						{
							if (reader->GetNodeText()->EqualsICase(UTF8STRC("NAME")))
							{
								sb.ClearStr();
								reader->ReadNodeText(&sb);
								SDEL_STRING(layerName);
								layerName = Text::String::New(sb.ToCString());
							}
							else if (reader->GetNodeText()->EqualsICase(UTF8STRC("LINK")))
							{
								while (reader->ReadNext())
								{
									if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
									{
										break;
									}
									else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
									{
										if (reader->GetNodeText()->EqualsICase(UTF8STRC("HREF")))
										{
											sb.ClearStr();
											reader->ReadNodeText(&sb);
											SDEL_STRING(url);
											url = Text::String::New(sb.ToCString());
										}
										else if (reader->GetNodeText()->EqualsICase(UTF8STRC("REFRESHINTERVAL")))
										{
											sb.ClearStr();
											reader->ReadNodeText(&sb);
											interval = sb.ToInt32();
										}
										else
										{
											reader->SkipElement();
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

					if (url)
					{
						lyr->AddInnerLayer(STR_CSTR(layerName), STR_CSTR(url), interval);
					}

					SDEL_STRING(layerName);
					SDEL_STRING(url);
					layers.Add(lyr);
				}
				else
				{
					reader->SkipElement();
				}
			}
			else if (reader->GetNodeText()->EqualsICase(UTF8STRC("STYLE")))
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
					attr = reader->GetAttrib(i);
					if (attr->name->EqualsICase(UTF8STRC("ID")))
					{
						styleId = attr->value->Clone();
						break;
					}
				}

				while (reader->ReadNext())
				{
					if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
					{
						break;
					}
					else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
					{
						if (reader->GetNodeText()->EqualsICase(UTF8STRC("LINESTYLE")))
						{
							while (reader->ReadNext())
							{
								if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
								{
									break;
								}
								else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
								{
									if (reader->GetNodeText()->EqualsICase(UTF8STRC("COLOR")))
									{
										sb.ClearStr();
										reader->ReadNodeText(&sb);
										style->lineColor = Text::StrHex2UInt32C(sb.ToString());
										style->lineColor = (style->lineColor & 0xff00ff00) | ((style->lineColor & 0xff) << 16) | ((style->lineColor & 0xff0000) >> 16);
										style->flags |= 1;
									}
									else if (reader->GetNodeText()->EqualsICase(UTF8STRC("WIDTH")))
									{
										sb.ClearStr();
										reader->ReadNodeText(&sb);
										sb.ToUInt32S(&style->lineWidth, 0);
										style->flags |= 2;
									}
									else
									{
										reader->SkipElement();
									}
								}
							}
						}
						else if (reader->GetNodeText()->EqualsICase(UTF8STRC("ICONSTYLE")))
						{
							while (reader->ReadNext())
							{
								if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
								{
									break;
								}
								else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
								{
									if (reader->GetNodeText()->EqualsICase(UTF8STRC("COLOR")))
									{
										sb.ClearStr();
										reader->ReadNodeText(&sb);
										style->iconColor = Text::StrHex2UInt32C(sb.ToString());
										style->iconColor = (style->iconColor & 0xff00ff00) | ((style->iconColor & 0xff) << 16) | ((style->iconColor & 0xff0000) >> 16);
									}
									else if (reader->GetNodeText()->EqualsICase(UTF8STRC("ICON")))
									{
										while (reader->ReadNext())
										{
											if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
											{
												break;
											}
											else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element && reader->GetNodeText()->EqualsICase(UTF8STRC("HREF")))
											{
												sb.ClearStr();
												reader->ReadNodeText(&sb);
												if (sb.GetLength() > 0)
												{
													SDEL_STRING(style->iconURL);
													style->iconURL = Text::String::New(sb.ToString(), sb.GetLength());
												}
											}
											else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
											{
												reader->SkipElement();
											}
										}
									}
									else if (reader->GetNodeText()->EqualsICase(UTF8STRC("HOTSPOT")))
									{
										i = reader->GetAttribCount();
										while (i-- > 0)
										{
											attr = reader->GetAttrib(i);
											if (attr->name->EqualsICase(UTF8STRC("X")))
											{
												style->iconSpotX = attr->value->ToInt32();
											}
											else if (attr->name->EqualsICase(UTF8STRC("Y")))
											{
												style->iconSpotY = attr->value->ToInt32();
											}
										}
										sb.ClearStr();
										reader->ReadNodeText(&sb);
										sb.ToUInt32S(&style->lineWidth, 0);
									}
									else
									{
										reader->SkipElement();
									}
								}
							}
						}
						else if (reader->GetNodeText()->EqualsICase(UTF8STRC("POLYSTYLE")))
						{
							while (reader->ReadNext())
							{
								if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
								{
									break;
								}
								else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element && reader->GetNodeText()->EqualsICase(UTF8STRC("COLOR")))
								{
									sb.ClearStr();
									reader->ReadNodeText(&sb);
									style->fillColor = Text::StrHex2UInt32C(sb.ToString());
									style->fillColor = (style->fillColor & 0xff00ff00) | ((style->fillColor & 0xff) << 16) | ((style->fillColor & 0xff0000) >> 16);
									style->flags |= 4;
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
			else if (reader->GetNodeText()->EqualsICase(UTF8STRC("STYLEMAP")))
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
					attr = reader->GetAttrib(i);
					if (attr->name->EqualsICase(UTF8STRC("ID")))
					{
						styleId = attr->value->Clone();
						break;
					}
				}

				while (reader->ReadNext())
				{
					if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
					{
						break;
					}
					else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element && reader->GetNodeText()->EqualsICase(UTF8STRC("PAIR")))
					{
						Bool isNormal = false;
						while (reader->ReadNext())
						{
							if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
							{
								break;
							}
							else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
							{
								if (reader->GetNodeText()->EqualsICase(UTF8STRC("KEY")))
								{
									sb.ClearStr();
									reader->ReadNodeText(&sb);
									if (sb.Equals(UTF8STRC("normal")))
									{
										isNormal = true;
									}
								}
								else if (reader->GetNodeText()->EqualsICase(UTF8STRC("STYLEURL")))
								{
									sb.ClearStr();
									reader->ReadNodeText(&sb);
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
												style->img = style2->img->Clone();
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
					}
					else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
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
			else if (reader->GetNodeText()->EqualsICase(UTF8STRC("PLACEMARK")))
			{
				lyr = ParseKMLPlacemarkLyr(reader, styles, sourceName, parsers, browser, basePF);
				if (lyr)
				{
					layers.Add(lyr);
				}
			}
			else if (reader->GetNodeText()->EqualsICase(UTF8STRC("SCREENOVERLAY")))
			{
				if (imgLyr == 0)
				{
					NEW_CLASS(imgLyr, Map::WebImageLayer(browser, parsers, sourceName, Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84), containerNameSb.ToCString()));
				}

				Text::String *name = 0;
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

				while (reader->ReadNext())
				{
					if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
					{
						break;
					}
					else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
					{
						if (reader->GetNodeText()->EqualsICase(UTF8STRC("NAME")))
						{
							sb.ClearStr();
							reader->ReadNodeText(&sb);
							SDEL_STRING(name);
							name = Text::String::New(sb.ToCString());
						}
						else if (reader->GetNodeText()->EqualsICase(UTF8STRC("COLOR")))
						{
							sb.ClearStr();
							reader->ReadNodeText(&sb);
							color = Text::StrHex2UInt32C(sb.ToString());
						}
						else if (reader->GetNodeText()->EqualsICase(UTF8STRC("DRAWORDER")))
						{
							sb.ClearStr();
							reader->ReadNodeText(&sb);
							zIndex = Text::StrToInt32(sb.ToString());
						}
						else if (reader->GetNodeText()->EqualsICase(UTF8STRC("ICON")))
						{
							while (reader->ReadNext())
							{
								if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
								{
									break;
								}
								else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element && reader->GetNodeText()->EqualsICase(UTF8STRC("HREF")))
								{
									sb.ClearStr();
									reader->ReadNodeText(&sb);
									sbuffEnd = imgLyr->GetSourceName(sbuff);
									sbuffEnd = Text::URLString::AppendURLPath(sbuff, sbuffEnd, sb.ToCString());
								}
								else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
								{
									reader->SkipElement();
								}
							}
						}
						else if (reader->GetNodeText()->EqualsICase(UTF8STRC("TIMESPAN")))
						{
							while (reader->ReadNext())
							{
								if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
								{
									break;
								}
								else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
								{
									if (reader->GetNodeText()->EqualsICase(UTF8STRC("BEGIN")))
									{
										sb.ClearStr();
										reader->ReadNodeText(&sb);
										dt.SetValue(sb.ToCString());
										timeStart = dt.ToUnixTimestamp();
									}
									else if (reader->GetNodeText()->EqualsICase(UTF8STRC("END")))
									{
										sb.ClearStr();
										reader->ReadNodeText(&sb);
										dt.SetValue(sb.ToCString());
										timeEnd = dt.ToUnixTimestamp();
									}
									else
									{
										reader->SkipElement();
									}
								}
							}
						}
						else if (reader->GetNodeText()->EqualsICase(UTF8STRC("SCREENXY")))
						{
							i = reader->GetAttribCount();
							while (i-- > 0)
							{
								attr = reader->GetAttrib(i);
								if (attr->name->EqualsICase(UTF8STRC("X")))
								{
									minX = attr->value->ToDouble();
								}
								else if (attr->name->EqualsICase(UTF8STRC("Y")))
								{
									minY = attr->value->ToDouble();
								}
							}
							reader->SkipElement();
						}
						else if (reader->GetNodeText()->EqualsICase(UTF8STRC("OVERLAYXY")))
						{
							i = reader->GetAttribCount();
							while (i-- > 0)
							{
								attr = reader->GetAttrib(i);
								if (attr->name->EqualsICase(UTF8STRC("X")))
								{
									oX = attr->value->ToDouble();
								}
								else if (attr->name->EqualsICase(UTF8STRC("Y")))
								{
									oY = attr->value->ToDouble();
								}
							}
							reader->SkipElement();
						}
						else if (reader->GetNodeText()->EqualsICase(UTF8STRC("SIZE")))
						{
							i = reader->GetAttribCount();
							while (i--)
							{
								attr = reader->GetAttrib(i);
								if (attr->name->EqualsICase(UTF8STRC("X")))
								{
									sizeX = attr->value->ToDouble();
								}
								else if (attr->name->EqualsICase(UTF8STRC("Y")))
								{
									sizeY = attr->value->ToDouble();
								}
							}
							reader->SkipElement();
						}
						else if (reader->GetNodeText()->EqualsICase(UTF8STRC("ALTITUDE")))
						{
							sb.ClearStr();
							reader->ReadNodeText(&sb);
							hasAltitude = true;
							altitude = Text::StrToDouble(sb.ToString());
						}
						else
						{
							reader->SkipElement();
						}
					}
				}
				if (sbuff[0] != 0)
				{
					imgLyr->AddImage(STR_CSTR(name), CSTRP(sbuff, sbuffEnd), zIndex, minX, minY, oX, oY, sizeX, sizeY, true, timeStart, timeEnd, ((color >> 24) & 0xff) / 255.0, hasAltitude, altitude);
				}
				SDEL_STRING(name);
			}
			else if (reader->GetNodeText()->EqualsICase(UTF8STRC("GROUNDOVERLAY")))
			{
				if (imgLyr == 0)
				{
					NEW_CLASS(imgLyr, Map::WebImageLayer(browser, parsers, sourceName, Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84), containerNameSb.ToCString()));
				}

				Text::String *name = 0;
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
				while (reader->ReadNext())
				{
					if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
					{
						break;
					}
					else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
					{
						if (reader->GetNodeText()->EqualsICase(UTF8STRC("NAME")))
						{
							sb.ClearStr();
							reader->ReadNodeText(&sb);
							SDEL_STRING(name);
							name = Text::String::New(sb.ToCString());
						}
						else if (reader->GetNodeText()->EqualsICase(UTF8STRC("COLOR")))
						{
							sb.ClearStr();
							reader->ReadNodeText(&sb);
							alpha = ((Text::StrHex2Int32C(sb.ToString()) >> 24) & 0xff) / 255.0;
						}
						else if (reader->GetNodeText()->EqualsICase(UTF8STRC("DRAWORDER")))
						{
							sb.ClearStr();
							reader->ReadNodeText(&sb);
							zIndex = Text::StrToInt32(sb.ToString());
						}
						else if (reader->GetNodeText()->EqualsICase(UTF8STRC("ICON")))
						{
							while (reader->ReadNext())
							{
								if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
								{
									break;
								}
								else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element && reader->GetNodeText()->EqualsICase(UTF8STRC("HREF")))
								{
									sb.ClearStr();
									reader->ReadNodeText(&sb);
									sbuffEnd = imgLyr->GetSourceName(sbuff);
									sbuffEnd = Text::URLString::AppendURLPath(sbuff, sbuffEnd, sb.ToCString());
								}
								else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
								{
									reader->SkipElement();
								}
							}
						}
						else if (reader->GetNodeText()->EqualsICase(UTF8STRC("TIMESPAN")))
						{
							while (reader->ReadNext())
							{
								if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
								{
									break;
								}
								else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
								{
									if (reader->GetNodeText()->EqualsICase(UTF8STRC("BEGIN")))
									{
										sb.ClearStr();
										reader->ReadNodeText(&sb);
										dt.SetValue(sb.ToCString());
										timeStart = dt.ToUnixTimestamp();
									}
									else if (reader->GetNodeText()->EqualsICase(UTF8STRC("END")))
									{
										sb.ClearStr();
										reader->ReadNodeText(&sb);
										dt.SetValue(sb.ToCString());
										timeEnd = dt.ToUnixTimestamp();
									}
									else
									{
										reader->SkipElement();
									}
								}
							}
						}
						else if (reader->GetNodeText()->EqualsICase(UTF8STRC("LATLONBOX")))
						{
							while (reader->ReadNext())
							{
								if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
								{
									break;
								}
								else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
								{
									if (reader->GetNodeText()->EqualsICase(UTF8STRC("NORTH")))
									{
										sb.ClearStr();
										reader->ReadNodeText(&sb);
										maxY = Text::StrToDouble(sb.ToString());
									}
									else if (reader->GetNodeText()->EqualsICase(UTF8STRC("SOUTH")))
									{
										sb.ClearStr();
										reader->ReadNodeText(&sb);
										minY = Text::StrToDouble(sb.ToString());
									}
									else if (reader->GetNodeText()->EqualsICase(UTF8STRC("EAST")))
									{
										sb.ClearStr();
										reader->ReadNodeText(&sb);
										maxX = Text::StrToDouble(sb.ToString());
									}
									else if (reader->GetNodeText()->EqualsICase(UTF8STRC("WEST")))
									{
										sb.ClearStr();
										reader->ReadNodeText(&sb);
										minX = Text::StrToDouble(sb.ToString());
									}
									else
									{
										reader->SkipElement();
									}
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
						else if (reader->GetNodeText()->EqualsICase(UTF8STRC("ALTITUDE")))
						{
							sb.ClearStr();
							reader->ReadNodeText(&sb);
							hasAltitude = true;
							altitude = Text::StrToDouble(sb.ToString());
						}
						else
						{
							reader->SkipElement();
						}
					}
				}
				if (sbuff[0] != 0)
				{
					imgLyr->AddImage(STR_CSTR(name), CSTRP(sbuff, sbuffEnd), zIndex, minX, minY, maxX, maxY, 0, 0, false, timeStart, timeEnd, alpha, hasAltitude, altitude);
				}
				SDEL_STRING(name);
			}
			else if (reader->GetNodeText()->EqualsICase(UTF8STRC("FOLDER")))
			{
				lyr = ParseKMLContainer(reader, styles, sourceName, parsers, browser, basePF);
				if (lyr)
				{
					layers.Add(lyr);
				}
			}
			else if (reader->GetNodeText()->EqualsICase(UTF8STRC("DOCUMENT")))
			{
				lyr = ParseKMLContainer(reader, styles, sourceName, parsers, browser, basePF);
				if (lyr)
				{
					layers.Add(lyr);
				}
			}
			else if (reader->GetNodeText()->EqualsICase(UTF8STRC("NAME")))
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
						containerNameSb.Append(reader->GetNodeText());
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
	}

	if (imgLyr)
	{
		layers.Add(imgLyr);
	}
	if (layers.GetCount() <= 0)
	{
		return 0;
	}
	else
	{
		Map::MapLayerCollection *lyrColl;
		UOSInt j;
		NEW_CLASS(lyrColl, Map::MapLayerCollection(sourceName, containerNameSb.ToCString()));
		i = 0;
		j = layers.GetCount();
		while (i < j)
		{
			lyr = layers.GetItem(i);
			lyrColl->Add(lyr);
			i++;
		}
		return lyrColl;
	}
}

void Map::KMLXML::ParseKMLPlacemarkTrack(Text::XMLReader *reader, Map::GPSTrack *lyr, Data::StringMap<KMLStyle*> *styles)
{
	Data::ArrayList<Text::String*> timeList;
	Data::ArrayList<Text::String*> coordList;
	Bool lastTrack = false;
	while (reader->ReadNext())
	{
		if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
		{
			break;
		}
		else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
		{
			if (reader->GetNodeText()->EqualsICase(UTF8STRC("GX:MULTITRACK")))
			{
				while (reader->ReadNext())
				{
					if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
					{
						break;
					}
					else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element && reader->GetNodeText()->EqualsICase(UTF8STRC("GX:TRACK")))
					{
						Text::StringBuilderUTF8 sb;
						UTF8Char sbuff[256];
						UTF8Char *strs[4];
						if (lastTrack)
						{
							lyr->NewTrack();
						}

						while (reader->ReadNext())
						{
							if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
							{
								break;
							}
							else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
							{
								if (reader->GetNodeText()->EqualsICase(UTF8STRC("WHEN")))
								{
									sb.ClearStr();
									reader->ReadNodeText(&sb);
									timeList.Add(Text::String::New(sb.ToString(), sb.GetLength()));
								}
								else if (reader->GetNodeText()->EqualsICase(UTF8STRC("GX:COORD")))
								{
									sb.ClearStr();
									reader->ReadNodeText(&sb);
									coordList.Add(Text::String::New(sb.ToString(), sb.GetLength()));
								}
								else if (reader->GetNodeText()->EqualsICase(UTF8STRC("EXTENDEDDATA")))
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
											Text::String *s = timeList.GetItem(i);
											dt.SetValue(s->ToCString());
											rec.utcTimeTicks = dt.ToTicks();
											
											coordList.GetItem(i)->ConcatTo(sbuff);
											if (Text::StrSplit(strs, 4, sbuff, ' ') == 3)
											{
												rec.pos = Math::Coord2DDbl(Text::StrToDouble(strs[1]), Text::StrToDouble(strs[0]));
												rec.altitude = Text::StrToDouble(strs[2]);
												lyr->AddRecord(&rec);
											}
											timeList.GetItem(i)->Release();
											coordList.GetItem(i)->Release();
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
											timeList.GetItem(i)->Release();
										}
										i = coordList.GetCount();
										while (i-- > 0)
										{
											coordList.GetItem(i)->Release();
										}
										timeList.Clear();
										coordList.Clear();
									}
									UOSInt recCnt;
									Map::GPSTrack::GPSRecord3 *recs = lyr->GetTrack(0, &recCnt);
									while (reader->ReadNext())
									{
										if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
										{
											break;
										}
										else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element && reader->GetNodeText()->EqualsICase(UTF8STRC("SCHEMADATA")))
										{
											while (reader->ReadNext())
											{
												if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
												{
													break;
												}
												else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element && reader->GetNodeText()->EqualsICase(UTF8STRC("GX:SIMPLEARRAYDATA")))
												{
													Text::XMLAttrib *attr;
													Text::StringBuilderUTF8 sb;
													Bool found = false;
													UOSInt i;
													UOSInt j;
													i = reader->GetAttribCount();
													while (i-- > 0)
													{
														attr = reader->GetAttrib(i);
														if (attr->name->EqualsICase(UTF8STRC("NAME")))
														{
															if (attr->value->EqualsICase(UTF8STRC("SPEED")))
															{
																j = 0;
																while (reader->ReadNext())
																{
																	if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
																	{
																		break;
																	}
																	else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element && reader->GetNodeText()->EqualsICase(UTF8STRC("GX:VALUE")))
																	{
																		sb.ClearStr();
																		reader->ReadNodeText(&sb);
																		if (j < recCnt)
																		{
																			recs[j].speed = Text::StrToDouble(sb.ToString());
																		}
																		j++;
																	}
																	else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
																	{
																		reader->SkipElement();
																	}
																}
															}
															else if (attr->value->EqualsICase(UTF8STRC("BEARING")))
															{
																j = 0;
																while (reader->ReadNext())
																{
																	if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
																	{
																		break;
																	}
																	else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element && reader->GetNodeText()->EqualsICase(UTF8STRC("GX:VALUE")))
																	{
																		sb.ClearStr();
																		reader->ReadNodeText(&sb);
																		if (j < recCnt)
																		{
																			recs[j].heading = Text::StrToDouble(sb.ToString());
																		}
																		j++;
																	}
																	else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
																	{
																		reader->SkipElement();
																	}
																}
															}
															else if (attr->value->EqualsICase(UTF8STRC("ACCURACY")))
															{
																j = 0;
																while (reader->ReadNext())
																{
																	if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
																	{
																		break;
																	}
																	else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element && reader->GetNodeText()->EqualsICase(UTF8STRC("GX:VALUE")))
																	{
																		sb.ClearStr();
																		reader->ReadNodeText(&sb);
																		if (j < recCnt)
																		{
																			recs[j].nSateUsed = (UInt8)Double2Int32(Text::StrToDouble(sb.ToString()));
																			recs[j].nSateUsedGPS = recs[j].nSateUsed;
																		}
																		j++;
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
															found = true;
															break;
														}
													}
													if (!found)
													{
														reader->SkipElement();
													}
												}
												else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
												{
													reader->SkipElement();
												}
											}
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
								Text::String *s = timeList.GetItem(i);
								dt.SetValue(s->ToCString());
								rec.utcTimeTicks = dt.ToTicks();
								
								coordList.GetItem(i)->ConcatTo(sbuff);
								if (Text::StrSplit(strs, 4, sbuff, ' ') == 3)
								{
									rec.pos = Math::Coord2DDbl(Text::StrToDouble(strs[1]), Text::StrToDouble(strs[0]));
									rec.altitude = Text::StrToDouble(strs[2]);
									lyr->AddRecord(&rec);
								}
								timeList.GetItem(i)->Release();
								coordList.GetItem(i)->Release();
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
								timeList.GetItem(i)->Release();
							}
							i = coordList.GetCount();
							while (i-- > 0)
							{
								coordList.GetItem(i)->Release();
							}
							timeList.Clear();
							coordList.Clear();
						}

						lastTrack = true;
					}
					else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
					{
						reader->SkipElement();
					}
				}
			}
			else if (reader->GetNodeText()->EqualsICase(UTF8STRC("GX:TRACK")))
			{
				Text::StringBuilderUTF8 sb;
				if (lastTrack)
				{
					lyr->NewTrack();
				}
				while (reader->ReadNext())
				{
					if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
					{
						break;
					}
					else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
					{
						if (reader->GetNodeText()->EqualsICase(UTF8STRC("WHEN")))
						{
							sb.ClearStr();
							reader->ReadNodeText(&sb);
							timeList.Add(Text::String::New(sb.ToString(), sb.GetLength()));
						}
						else if (reader->GetNodeText()->EqualsICase(UTF8STRC("GX:COORD")))
						{
							sb.ClearStr();
							reader->ReadNodeText(&sb);
							coordList.Add(Text::String::New(sb.ToString(), sb.GetLength()));
						}
						else
						{
							reader->SkipElement();
						}
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
					UTF8Char *strs[4];
					UOSInt i = 0;
					UOSInt j = timeList.GetCount();
					while (i < j)
					{
						Text::String *s = timeList.GetItem(i);
						dt.SetValue(s->ToCString());
						rec.utcTimeTicks = dt.ToTicks();
						
						coordList.GetItem(i)->ConcatTo(sbuff);
						if (Text::StrSplit(strs, 4, sbuff, ' ') == 3)
						{
							rec.pos.x = Text::StrToDouble(strs[0]);
							rec.pos.y = Text::StrToDouble(strs[1]);
							rec.altitude = Text::StrToDouble(strs[2]);
							lyr->AddRecord(&rec);
						}
						timeList.GetItem(i)->Release();
						coordList.GetItem(i)->Release();
						i++;
					}
				}
				else
				{
					UOSInt i = timeList.GetCount();
					while (i-- > 0)
					{
						timeList.GetItem(i)->Release();
					}
					i = coordList.GetCount();
					while (i-- > 0)
					{
						coordList.GetItem(i)->Release();
					}
				}
				timeList.Clear();
				coordList.Clear();

				lastTrack = true;
			}
			else if (reader->GetNodeText()->EqualsICase(UTF8STRC("STYLEURL")))
			{
				if (!lyr->HasLineStyle())
				{
					Text::StringBuilderUTF8 sb;
					reader->ReadNodeText(&sb);
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
}

Map::IMapDrawLayer *Map::KMLXML::ParseKMLPlacemarkLyr(Text::XMLReader *reader, Data::StringMap<KMLStyle*> *styles, Text::CString sourceName, Parser::ParserList *parsers, Net::WebBrowser *browser, IO::PackageFile *basePF)
{
	Text::StringBuilderUTF8 lyrNameSb;
	Text::StringBuilderUTF8 sb;
	lyrNameSb.AppendC(UTF8STRC("Layer"));
	KMLStyle *style = 0;
	Data::ArrayList<Map::IMapDrawLayer*> layers;
	while (reader->ReadNext())
	{
		if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
		{
			break;
		}
		else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
		{
			if (reader->GetNodeText()->EqualsICase(UTF8STRC("NAME")))
			{
				lyrNameSb.ClearStr();
				reader->ReadNodeText(&lyrNameSb);
			}
			else if (reader->GetNodeText()->EqualsICase(UTF8STRC("STYLEURL")))
			{
				sb.ClearStr();
				reader->ReadNodeText(&sb);
				if (sb.ToString()[0] == '#')
				{
					style = styles->Get({sb.ToString() + 1, sb.GetLength() - 1});
				}
			}
			else if (reader->GetNodeText()->EqualsICase(UTF8STRC("LINESTRING")))
			{
				Map::VectorLayer *lyr;
				const UTF8Char *cols = (const UTF8Char*)"Name";
				DB::DBUtil::ColType colType = DB::DBUtil::CT_VarUTF8Char;
				UOSInt colSize = 256;
				UOSInt colDP = 0;
				NEW_CLASS(lyr, Map::VectorLayer(Map::DRAW_LAYER_POLYLINE3D, sourceName, 1, &cols, Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84), &colType, &colSize, &colDP, 0, lyrNameSb.ToCString()));

				while (reader->ReadNext())
				{
					if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
					{
						break;
					}
					else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element && reader->GetNodeText()->EqualsICase(UTF8STRC("COORDINATES")))
					{
						Data::ArrayListA<Math::Coord2DDbl> coord;
						Data::ArrayListDbl altList;
						UTF8Char c;
						UTF8Char *sptr;
						UTF8Char *sptr2;
						UTF8Char sbuff[256];
						UTF8Char *sarr[4];
						UOSInt i;

						sb.ClearStr();
						reader->ReadNodeText(&sb);
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
								coord.Add(Math::Coord2DDbl(Text::StrToDouble(sarr[0]), Text::StrToDouble(sarr[1])));
								altList.Add(Text::StrToDouble(sarr[2]));
							}
							else if (i == 2)
							{
								coord.Add(Math::Coord2DDbl(Text::StrToDouble(sarr[0]), Text::StrToDouble(sarr[1])));
								altList.Add(0);
							}
						}
						if (coord.GetCount() > 0)
						{
							Math::Geometry::LineString *pl;
							UOSInt nPoints;
							Math::Coord2DDbl *ptArr;
							Double *altArr;
							UOSInt j;

							NEW_CLASS(pl, Math::Geometry::LineString(4326, altList.GetCount(), true, false));
							altArr = pl->GetZList(&nPoints);
							ptArr = pl->GetPointList(&nPoints);
							i = altList.GetCount();
							MemCopyAC(ptArr, coord.GetArray(&j), sizeof(Math::Coord2DDbl) * i);
							MemCopyAC(altArr, altList.GetArray(&j), sizeof(Double) * i);
							lyr->AddVector(pl, &lyrNameSb);
						}
					}
					else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
					{
						reader->SkipElement();
					}
				}

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
			else if (reader->GetNodeText()->EqualsICase(UTF8STRC("POINT")))
			{
				Map::VectorLayer *lyr;
				const UTF8Char *cols = (const UTF8Char*)"Name";
				DB::DBUtil::ColType colType = DB::DBUtil::CT_VarUTF8Char;
				UOSInt colSize = 256;
				UOSInt colDP = 0;
				NEW_CLASS(lyr, Map::VectorLayer(Map::DRAW_LAYER_POINT, sourceName, 1, &cols, Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84), &colType, &colSize, &colDP, 0, lyrNameSb.ToCString()));
				lyr->SetLabelVisible(true);
				while (reader->ReadNext())
				{
					if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
					{
						break;
					}
					else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element && reader->GetNodeText()->EqualsICase(UTF8STRC("COORDINATES")))
					{
						sb.ClearStr();
						reader->ReadNodeText(&sb);

						Double x;
						Double y;
						Double z;
						UOSInt i;
						UTF8Char *sarr[4];
						i = Text::StrSplitTrim(sarr, 4, sb.v, ',');
						if (i == 3)
						{
							Math::Geometry::PointZ *pt;
							x = Text::StrToDouble(sarr[0]);
							y = Text::StrToDouble(sarr[1]);
							z = Text::StrToDouble(sarr[2]);
							NEW_CLASS(pt, Math::Geometry::PointZ(4326, x, y, z));
							lyr->AddVector(pt, &lyrNameSb);
						}
						else if (i == 2)
						{
							Math::Geometry::PointZ *pt;
							x = Text::StrToDouble(sarr[0]);
							y = Text::StrToDouble(sarr[1]);
							NEW_CLASS(pt, Math::Geometry::PointZ(4326, x, y, 0));
							lyr->AddVector(pt, &lyrNameSb);
						}
					}
					else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
					{
						reader->SkipElement();
					}
				}

				if (style && style->iconURL && parsers)
				{
					if (style->img == 0)
					{
						IO::IStreamData *fd = 0;
						if (basePF)
						{
							fd = basePF->OpenStreamData(style->iconURL->ToCString());
						}
						if (fd == 0 && browser)
						{
							fd = browser->GetData(style->iconURL->ToCString(), false, 0);
						}
						if (fd)
						{
							Media::ImageList *imgList = (Media::ImageList*)parsers->ParseFileType(fd, IO::ParserType::ImageList);
							if (imgList)
							{
								if (style->iconColor != 0)
								{
									UOSInt j = imgList->GetCount();
									while (j-- > 0)
									{
										imgList->ToStaticImage(j);
										Media::StaticImage *img = (Media::StaticImage *)imgList->GetImage(j, 0);
										img->MultiplyColor(style->iconColor);
									}
								}
								NEW_CLASS(style->img, Media::SharedImage(imgList, false));
							}
							DEL_CLASS(fd);
						}
					}
					if (style->img)
					{
						Media::Image *img = style->img->GetImage(0);
						if (style->iconSpotX == -1 || style->iconSpotY == -1)
						{
							lyr->SetIconStyle(style->img, (OSInt)(img->info.dispWidth >> 1), (OSInt)(img->info.dispHeight >> 1));
						}
						else
						{
							lyr->SetIconStyle(style->img, style->iconSpotX, (OSInt)img->info.dispHeight - style->iconSpotY);
						}
					}
				}
				layers.Add(lyr);
			}
			else if (reader->GetNodeText()->EqualsICase(UTF8STRC("POLYGON")))
			{
				Map::VectorLayer *lyr;
				const UTF8Char *cols = (const UTF8Char*)"Name";
				DB::DBUtil::ColType colType = DB::DBUtil::CT_VarUTF8Char;
				UOSInt colSize = 256;
				UOSInt colDP = 0;
				Data::ArrayListDbl coord;
				Data::ArrayListDbl altList;
				NEW_CLASS(lyr, Map::VectorLayer(Map::DRAW_LAYER_POLYGON, sourceName, 1, &cols, Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84), &colType, &colSize, &colDP, 0, lyrNameSb.ToCString()));

				while (reader->ReadNext())
				{
					if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
					{
						break;
					}
					else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
					{
						if (reader->GetNodeText()->EqualsICase(UTF8STRC("OUTERBOUNDARYIS")))
						{
							ParseCoordinates(reader, &coord, 0);
						}
						else if (reader->GetNodeText()->EqualsICase(UTF8STRC("INNERBOUNDARYIS")))
						{
							ParseCoordinates(reader, &altList, 0);
						}
						else
						{
							reader->SkipElement();
						}
					}
				}
				if (coord.GetCount() > 0)
				{
					Math::Geometry::Polygon *pg;
					UOSInt nPoints;
					Math::Coord2DDbl *ptArr;
					UInt32 *ptList;
					UOSInt i;

					if (altList.GetCount() > 0)
					{
						NEW_CLASS(pg, Math::Geometry::Polygon(4326, 2, (coord.GetCount() + altList.GetCount()) >> 1, false, false));
						ptList = pg->GetPtOfstList(&nPoints);
						ptList[0] = 0;
						ptList[1] = (UInt32)(coord.GetCount() >> 1);
						ptArr = pg->GetPointList(&nPoints);
						MemCopyNO(ptArr, coord.GetArray(&i), sizeof(Double) * coord.GetCount());
						MemCopyNO(&ptArr[coord.GetCount()], altList.GetArray(&i), sizeof(Double) * altList.GetCount());
						lyr->AddVector(pg, &lyrNameSb);
					}
					else
					{
						NEW_CLASS(pg, Math::Geometry::Polygon(4326, 1, coord.GetCount() >> 1, false, false));
						ptList = pg->GetPtOfstList(&nPoints);
						ptList[0] = 0;
						ptArr = pg->GetPointList(&nPoints);
						MemCopyNO(ptArr, coord.GetArray(&i), sizeof(Double) * coord.GetCount());
						lyr->AddVector(pg, &lyrNameSb);
					}
				}

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
			else if (reader->GetNodeText()->EqualsICase(UTF8STRC("GX:MULTITRACK")))
			{
				Map::GPSTrack *lyr;
				NEW_CLASS(lyr, Map::GPSTrack(sourceName, true, 65001, lyrNameSb.ToCString()));
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
			else
			{
				reader->SkipElement();
			}
		}
	}
	if (layers.GetCount() == 1)
	{
		return layers.GetItem(0);
	}
	UOSInt i = layers.GetCount();
	Map::IMapDrawLayer *lyr;
	while (i-- > 0)
	{
		lyr = layers.GetItem(i);
		DEL_CLASS(lyr);
	}
	return 0;
}

void Map::KMLXML::ParseCoordinates(Text::XMLReader *reader, Data::ArrayList<Double> *coordList, Data::ArrayList<Double> *altList)
{
	UOSInt i;
	UTF8Char *sptr;
	UTF8Char *sptr2;
	UTF8Char c;
	UTF8Char sbuff[256];
	UTF8Char *sarr[4];

	while (reader->ReadNext())
	{
		if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
		{
			break;
		}
		else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
		{
			if (reader->GetNodeText()->EqualsICase(UTF8STRC("LINEARRING")))
			{
				ParseCoordinates(reader, coordList, altList);
			}
			else if (reader->GetNodeText()->EqualsICase(UTF8STRC("COORDINATES")))
			{
				Text::StringBuilderUTF8 sb;
				reader->ReadNodeText(&sb);
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
						coordList->Add(Text::StrToDouble(sarr[0]));
						coordList->Add(Text::StrToDouble(sarr[1]));
						if (altList)
						{
							altList->Add(Text::StrToDouble(sarr[2]));
						}
					}
					else if (i == 2)
					{
						coordList->Add(Text::StrToDouble(sarr[0]));
						coordList->Add(Text::StrToDouble(sarr[1]));
						if (altList)
						{
							altList->Add(0);
						}
					}
				}
			}
		}
	}
}
