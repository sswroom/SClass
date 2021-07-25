#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListDbl.h"
#include "Data/Int64Map.h"
#include "DB/TextDB.h"
#include "IO/PackageFile.h"
#include "IO/Path.h"
#include "IO/StreamDataStream.h"
#include "Map/MapLayerCollection.h"
#include "Map/OruxDBLayer.h"
#include "Map/ReloadableMapLayer.h"
#include "Map/VectorLayer.h"
#include "Map/WebImageLayer.h"
#include "Map/OSM/OSMParser.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math.h"
#include "Math/Polygon.h"
#include "Math/Polyline3D.h"
#include "Math/Point3D.h"
#include "Media/StaticImage.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/URLString.h"
#include "Text/VSProject.h"
#include "Parser/FileParser/XMLParser.h"

Parser::FileParser::XMLParser::XMLParser()
{
	this->codePage = 0;
	this->encFact = 0;
	this->parsers = 0;
	this->browser = 0;
}

Parser::FileParser::XMLParser::~XMLParser()
{
}

Int32 Parser::FileParser::XMLParser::GetName()
{
	return *(Int32*)"XMLP";
}

void Parser::FileParser::XMLParser::SetCodePage(UInt32 codePage)
{
	this->codePage = codePage;
}

void Parser::FileParser::XMLParser::SetParserList(Parser::ParserList *parsers)
{
	this->parsers = parsers;
}

void Parser::FileParser::XMLParser::SetWebBrowser(Net::WebBrowser *browser)
{
	this->browser = browser;
}

void Parser::FileParser::XMLParser::SetEncFactory(Text::EncodingFactory *encFact)
{
	this->encFact = encFact;
}

void Parser::FileParser::XMLParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_MAP_LAYER_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.xml", (const UTF8Char*)"XML File");
		selector->AddFilter((const UTF8Char*)"*.gpx", (const UTF8Char*)"GPS Track File");
		selector->AddFilter((const UTF8Char*)"*.kml", (const UTF8Char*)"KML File");
		selector->AddFilter((const UTF8Char*)"*.osm", (const UTF8Char*)"OpenStreetMap File");
		selector->AddFilter((const UTF8Char*)"*.vcproj", (const UTF8Char*)"VC Project File");
		selector->AddFilter((const UTF8Char*)"*.gml", (const UTF8Char*)"GML File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::XMLParser::GetParserType()
{
	return IO::ParsedObject::PT_MAP_LAYER_PARSER;
}

IO::ParsedObject *Parser::FileParser::XMLParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UTF8Char u8buff[512];
	UOSInt i;
	UOSInt j;
	Bool valid = false;

	Text::StrConcat(u8buff, fd->GetFullName());
	i = Text::StrLastIndexOf(u8buff, '.');
	j = Text::StrIndexOf(&u8buff[i + 1], '?');
	if (j != INVALID_INDEX)
	{
		u8buff[i + j + 1] = 0;
	}
	if (i == INVALID_INDEX)
	{

	}
	else if (Text::StrEqualsICase(&u8buff[i], (const UTF8Char*)".XML"))
	{
		valid = true;
	}
	else if (Text::StrEqualsICase(&u8buff[i], (const UTF8Char*)".GPX"))
	{
		valid = true;
	}
	else if (Text::StrEqualsICase(&u8buff[i], (const UTF8Char*)".KML"))
	{
		valid = true;
	}
	else if (Text::StrEqualsICase(&u8buff[i], (const UTF8Char*)".OSM"))
	{
		valid = true;
	}
	else if (Text::StrEqualsICase(&u8buff[i], (const UTF8Char*)".VCPROJ"))
	{
		valid = true;
	}
	else if (Text::StrEqualsICase(&u8buff[i], (const UTF8Char*)".GML"))
	{
		valid = true;
	}
	if (!valid)
	{
		return 0;
	}

	IO::StreamDataStream *stm;
	IO::ParsedObject *pobj;
	NEW_CLASS(stm, IO::StreamDataStream(fd));
	pobj = ParseStream(this->encFact, stm, fd->GetFullName(), this->parsers, this->browser, pkgFile);
	DEL_CLASS(stm);
	return pobj;
}

IO::ParsedObject *Parser::FileParser::XMLParser::ParseStream(Text::EncodingFactory *encFact, IO::Stream *stm, const UTF8Char *fileName, Parser::ParserList *parsers, Net::WebBrowser *browser, IO::PackageFile *pkgFile)
{
	Text::XMLReader *reader;
	NEW_CLASS(reader, Text::XMLReader(encFact, stm, Text::XMLReader::PM_XML));

	while (true)
	{
		if (!reader->ReadNext())
		{
			DEL_CLASS(reader);
			return 0;
		}
		if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
		{
			break;
		}
	}

	if (Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"kml"))
	{
		Data::ICaseStringUTF8Map<KMLStyle*> styles;
/*		Data::ArrayList<Map::IMapDrawLayer *> layers;
		Map::IMapDrawLayer *lyr;

		while (reader->ReadNext())
		{
			if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
			{
				break;
			}
			else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
			{
				if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"DOCUMENT") || Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"FOLDER"))
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

		Data::ArrayList<KMLStyle*> *styleList = styles.GetValues();
		KMLStyle *style;
/*		UOSInt i;
		UOSInt j;
		const UTF8Char *shortName;
		i = Text::StrLastIndexOf(fileName, IO::Path::PATH_SEPERATOR);
		if (IO::Path::PATH_SEPERATOR == '\\')
		{
			j = Text::StrLastIndexOf(fileName, '/');
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
			SDEL_TEXT(style->iconURL);
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
		if (lyr)
		{
			DEL_CLASS(reader);
			return lyr;
		}
	}
	else if (Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"gpx"))
	{
		UOSInt i;
		UOSInt j;
		const UTF8Char *shortName;
		i = Text::StrLastIndexOf(fileName, IO::Path::PATH_SEPERATOR);
		if (IO::Path::PATH_SEPERATOR == '\\')
		{
			j = Text::StrLastIndexOf(fileName, '/');
			if (i == INVALID_INDEX || (j != INVALID_INDEX && j > i))
			{
				i = j;
			}
		}
		shortName = &fileName[i + 1];
		while (reader->ReadNext())
		{
			if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
			{
				break;
			}
			else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
			{
				if (Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"trk")) // /gpx/trk/trkseg
				{
					Map::GPSTrack *track;
					NEW_CLASS(track, Map::GPSTrack(fileName, true, 0, 0));
					track->SetTrackName(shortName);
					while (reader->ReadNext())
					{
						if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
						{
							break;
						}
						else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
						{
							if (Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"trkseg"))
							{
								track->NewTrack();
								while (reader->ReadNext())
								{
									if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
									{
										break;
									}
									else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
									{
										if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"TRKPT"))
										{
											Map::GPSTrack::GPSRecord rec;
											if (ParseGPXPoint(reader, &rec))
											{
												track->AddRecord(&rec);
											}
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
								if (!reader->SkipElement())
								{
									break;
								}
							}
						}
					}
					DEL_CLASS(reader);
					return track;
				}
/*				else if (Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"rte")) // /gpx/rte
				{
					Map::GPSTrack *track;
					NEW_CLASS(track, Map::GPSTrack(fileName, true, 0, 0));
					j = 0;
					while (j < i)
					{
						track->NewTrack();

						size = nodeResult[j]->GetChildCnt();
						k = 0;
						while (k < size)
						{
							node1 = nodeResult[j]->GetChild(k);
							if (node1->GetNodeType() == Text::XMLNode::NT_ELEMENT)
							{
								if (Text::StrEqualsICase(node1->name, (const UTF8Char*)"RTEPT"))
								{
									Map::GPSTrack::GPSRecord rec;
									ParseGPXPoint(node1, &rec);
									track->AddRecord(&rec);
								}
								else if (Text::StrEqualsICase(node1->name, (const UTF8Char*)"NAME"))
								{
									Text::StringBuilderUTF8 sb;
									node1->GetInnerText(&sb);
									track->SetTrackName(sb.ToString());
								}
							}

							k++;
						}

						j++;
					}
					DEL_CLASS(reader);
					return track;
				}*/
/*				else if (Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"wpt")) // /gpx/wpt
				{
					Map::GPSTrack *track;
					NEW_CLASS(track, Map::GPSTrack(fileName, true, 0, 0));
					track->NewTrack();
					track->SetTrackName(shortName);
					j = 0;
					while (j < i)
					{
						Map::GPSTrack::GPSRecord rec;
						ParseGPXPoint(nodeResult[j], &rec);
						track->AddRecord(&rec);
						j++;
					}

					DEL_CLASS(reader);
					return track;
				}*/
//				else
				{
					if (!reader->SkipElement())
					{
						DEL_CLASS(reader);
						return 0;
					}
				}
			}
		}
		DEL_CLASS(reader);
		return 0;
	}
	else if (Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"osm"))
	{
		Map::IMapDrawLayer *lyr = Map::OSM::OSMParser::ParseLayerNode(reader, fileName);
		if (lyr == 0)
		{
			DEL_CLASS(reader);
			return 0;
		}
		while (reader->ReadNext())
		{
			if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
			{
				DEL_CLASS(lyr)
				lyr = 0;
				break;
			}
		}
		if (!reader->IsComplete())
		{
			SDEL_CLASS(lyr);
		}
		DEL_CLASS(reader);
		return lyr;
	}
	else if (Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"OruxTracker"))
	{
		Map::OruxDBLayer *lyr = 0;
		while (reader->ReadNext())
		{
			if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
			{
				break;
			}
			else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"MapCalibration"))
			{
				while (reader->ReadNext())
				{
					if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
					{
						break;
					}
					else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
					{
						if (Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"OruxTracker"))
						{
							if (lyr)
							{
								while (reader->ReadNext())
								{
									if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
									{
										break;
									}
									else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"MapCalibration"))
									{
										UInt32 layerId = (UInt32)-1;
										UInt32 maxX = 0;
										UInt32 maxY = 0;
										UInt32 tileSize = 0;
										Int32 flags = 0;
										Double mapXMin;
										Double mapYMin;
										Double mapXMax;
										Double mapYMax;
										UOSInt i;
										Text::XMLAttrib *attr;
										i = reader->GetAttribCount();
										while (i-- > 0)
										{
											attr = reader->GetAttrib(i);
											if (Text::StrEquals(attr->name, (const UTF8Char*)"layerLevel"))
											{
												if (Text::StrToUInt32(attr->value, &layerId))
												{
													flags |= 1;
													break;
												}
											}
										}

										while (reader->ReadNext())
										{
											if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
											{
												break;
											}
											else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
											{
												if (Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"MapChunks"))
												{
													i = reader->GetAttribCount();
													while (i-- > 0)
													{
														attr = reader->GetAttrib(i);
														if (Text::StrEquals(attr->name, (const UTF8Char*)"xMax"))
														{
															if (Text::StrToUInt32(attr->value, &maxX))
																flags |= 2;
														}
														else if (Text::StrEquals(attr->name, (const UTF8Char*)"yMax"))
														{
															if (Text::StrToUInt32(attr->value, &maxY))
																flags |= 4;
														}
														else if (Text::StrEquals(attr->name, (const UTF8Char*)"img_width"))
														{
															if (Text::StrToUInt32(attr->value, &tileSize))
																flags |= 8;
														}
													}
													reader->SkipElement();
												}
												else if (Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"MapBounds"))
												{
													i = reader->GetAttribCount();
													while (i-- > 0)
													{
														attr = reader->GetAttrib(i);
														if (Text::StrEquals(attr->name, (const UTF8Char*)"minLat"))
														{
															if (Text::StrToDouble(attr->value, &mapYMin))
																flags |= 16;
														}
														else if (Text::StrEquals(attr->name, (const UTF8Char*)"maxLat"))
														{
															if (Text::StrToDouble(attr->value, &mapYMax))
																flags |= 32;
														}
														else if (Text::StrEquals(attr->name, (const UTF8Char*)"minLon"))
														{
															if (Text::StrToDouble(attr->value, &mapXMin))
																flags |= 64;
														}
														else if (Text::StrEquals(attr->name, (const UTF8Char*)"maxLon"))
														{
															if (Text::StrToDouble(attr->value, &mapXMax))
																flags |= 128;
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

										if (flags == 255)
										{
											lyr->AddLayer(layerId, mapXMin, mapYMin, mapXMax, mapYMax, maxX, maxY, tileSize);
										}
									}
									else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
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
						else if (Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"MapName"))
						{
							if (lyr == 0)
							{
								Text::StringBuilderUTF8 sb;
								reader->ReadNodeText(&sb);
								NEW_CLASS(lyr, Map::OruxDBLayer(fileName, sb.ToString(), parsers));
								if (lyr->IsError())
								{
									DEL_CLASS(lyr);
									lyr = 0;
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
				}

				break;
			}
			else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
			{
				reader->SkipElement();
			}
		}
		DEL_CLASS(reader);
		return lyr;
	}
	else if (Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"VisualStudioProject"))
	{
		Text::XMLAttrib *attr;
		Text::VSProject *proj = 0;
		Text::VSProject::VisualStudioVersion ver = Text::VSProject::VSV_UNKNOWN;
		const UTF8Char *projName = 0;
		UOSInt i;
		i = reader->GetAttribCount();
		while (i-- > 0)
		{
			attr = reader->GetAttrib(i);
			if (Text::StrEquals(attr->name, (const UTF8Char*)"Version"))
			{
				if (Text::StrEquals(attr->value, (const UTF8Char*)"7.10"))
				{
					ver = Text::VSProject::VSV_VS71;
				}
				else if (Text::StrEquals(attr->value, (const UTF8Char*)"8.00"))
				{
					ver = Text::VSProject::VSV_VS8;
				}
				else if (Text::StrEquals(attr->value, (const UTF8Char*)"9.00"))
				{
					ver = Text::VSProject::VSV_VS9;
				}
			}
			else if (Text::StrEquals(attr->name, (const UTF8Char*)"Name"))
			{
				projName = attr->value;
			}
		}
		if (projName && ver != Text::VSProject::VSV_UNKNOWN)
		{
			NEW_CLASS(proj, Text::VSProject(fileName, ver));
			proj->SetProjectName(projName);
			i = 0;
			while (reader->ReadNext())
			{
				if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
				{
					break;
				}
				else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
				{
					if (Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"Configurations"))
					{
						if (!ParseVSConfFile(reader, proj))
						{
							break;
						}
					}
					else if (Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"Files"))
					{
						if (!ParseVSProjFile(reader, proj))
						{
							break;
						}
					}
					else
					{
						if (!reader->SkipElement())
						{
							break;
						}
					}
				}
			}
		}
		DEL_CLASS(reader);
		return proj;
	}
	else if (Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"gml:FeatureCollection"))
	{
		Math::CoordinateSystem *csys = 0;
		UInt32 srid = 0;
		UOSInt colCnt = 0;
		Data::ArrayList<const UTF8Char *> nameList;
		Data::ArrayList<const UTF8Char *> valList;
		Text::StringBuilderUTF8 sb;
		Map::VectorLayer *lyr = 0;
		Map::DrawLayerType layerType = Map::DRAW_LAYER_UNKNOWN;
		const UTF8Char **cols;
		UOSInt i;
		Text::XMLAttrib *attr;
		UTF8Char *sarr[4];
		while (reader->ReadNext())
		{
			if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
			{
				break;
			}
			else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
			{
				if (Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"gml:featureMember"))
				{
					while (reader->ReadNext())
					{
						if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
						{
							break;
						}
						else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
						{
							while (reader->ReadNext())
							{
								if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
								{
									break;
								}
								else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
								{
									if (Text::StrStartsWith(reader->GetNodeText(), (const UTF8Char*)"fme:"))
									{
										nameList.Add(Text::StrCopyNew(reader->GetNodeText() + 4));
										sb.ClearStr();
										reader->ReadNodeText(&sb);
										if (sb.GetLength() > 0)
										{
											valList.Add(Text::StrCopyNew(sb.ToString()));
										}
										else
										{
											valList.Add(0);
										}
									}
									else if (Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"gml:pointProperty"))
									{
										if (layerType == Map::DRAW_LAYER_UNKNOWN || layerType == Map::DRAW_LAYER_POINT3D)
										{
											layerType = Map::DRAW_LAYER_POINT3D;
											while (reader->ReadNext())
											{
												if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
												{
													break;
												}
												else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
												{
													if (Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"gml:Point"))
													{
														if (csys == 0)
														{
															i = reader->GetAttribCount();
															while (i-- > 0)
															{
																attr = reader->GetAttrib(i);
																if (Text::StrEquals(attr->name, (const UTF8Char*)"srsName"))
																{
																	csys = Math::CoordinateSystemManager::CreateFromName(attr->value);
																	if (csys)
																	{
																		srid = csys->GetSRID();
																	}
																	break;
																}
															}
														}
														if (lyr == 0)
														{
															colCnt = nameList.GetCount();
															cols = nameList.GetArray(&i);
															NEW_CLASS(lyr, Map::VectorLayer(layerType, fileName, colCnt, cols, csys, 0, 0));
														}
														if (colCnt == valList.GetCount())
														{
															Data::ArrayListDbl xPts;
															Data::ArrayListDbl yPts;
															Data::ArrayListDbl zPts;
															Math::Point3D *pt;
															cols = valList.GetArray(&i);
															while (reader->ReadNext())
															{
																if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
																{
																	break;
																}
																else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
																{
																	if (Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"gml:pos"))
																	{
																		sb.ClearStr();
																		reader->ReadNodeText(&sb);
																		sarr[3] = sb.ToString();
																		while (true)
																		{
																			i = Text::StrSplit(sarr, 4, sarr[3], ' ');
																			if (i < 3)
																			{
																				break;
																			}
																			xPts.Add(Text::StrToDouble(sarr[1]));
																			yPts.Add(Text::StrToDouble(sarr[0]));
																			zPts.Add(Text::StrToDouble(sarr[2]));
																			if (i < 4)
																				break;
																		}

																		if (xPts.GetCount() == 1)
																		{
																			NEW_CLASS(pt, Math::Point3D(srid, xPts.GetItem(0), yPts.GetItem(0), zPts.GetItem(0)));
																			lyr->AddVector(pt, cols);
																		}
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
									else if (Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"gml:surfaceProperty"))
									{
										if (layerType == Map::DRAW_LAYER_UNKNOWN || layerType == Map::DRAW_LAYER_POLYGON)
										{
											layerType = Map::DRAW_LAYER_POLYGON;
											while (reader->ReadNext())
											{
												if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
												{
													break;
												}
												else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
												{
													if (Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"gml:Surface"))
													{
														if (csys == 0)
														{
															i = reader->GetAttribCount();
															while (i-- > 0)
															{
																attr = reader->GetAttrib(i);
																if (Text::StrEquals(attr->name, (const UTF8Char*)"srsName"))
																{
																	csys = Math::CoordinateSystemManager::CreateFromName(attr->value);
																	if (csys)
																	{
																		srid = csys->GetSRID();
																	}
																	break;
																}
															}
														}
														if (lyr == 0)
														{
															colCnt = nameList.GetCount();
															cols = nameList.GetArray(&i);
															NEW_CLASS(lyr, Map::VectorLayer(layerType, fileName, colCnt, cols, csys, 0, 0));
														}
														if (colCnt == valList.GetCount())
														{
															Data::ArrayListDbl xPts;
															Data::ArrayListDbl yPts;
															Data::ArrayListDbl zPts;
															Math::Polygon *pg;
															Double *ptList;
															cols = valList.GetArray(&i);
															while (reader->ReadNext())
															{
																if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
																{
																	break;
																}
																else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"gml:patches"))
																{
																	while (reader->ReadNext())
																	{
																		if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
																		{
																			break;
																		}
																		else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"gml:PolygonPatch"))
																		{
																			while (reader->ReadNext())
																			{
																				if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
																				{
																					break;
																				}
																				else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"gml:exterior"))
																				{
																					while (reader->ReadNext())
																					{
																						if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
																						{
																							break;
																						}
																						else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"gml:LinearRing"))
																						{
																							while (reader->ReadNext())
																							{
																								if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
																								{
																									break;
																								}
																								else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"gml:posList"))
																								{
																									sb.ClearStr();
																									reader->ReadNodeText(&sb);
																									sarr[3] = sb.ToString();
																									while (true)
																									{
																										i = Text::StrSplit(sarr, 4, sarr[3], ' ');
																										if (i < 3)
																										{
																											break;
																										}
																										xPts.Add(Text::StrToDouble(sarr[1]));
																										yPts.Add(Text::StrToDouble(sarr[0]));
																										zPts.Add(Text::StrToDouble(sarr[2]));
																										if (i < 4)
																											break;
																									}

																									if (xPts.GetCount() > 0)
																									{
																										NEW_CLASS(pg, Math::Polygon(srid, 1, xPts.GetCount()));
																										ptList = pg->GetPointList(&i);
																										while (i-- > 0)
																										{
																											ptList[(i << 1)] = xPts.GetItem(i);
																											ptList[(i << 1) + 1] = yPts.GetItem(i);
																										}
																										lyr->AddVector(pg, cols);
																									}
																								}
																								else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
																								{
																									reader->SkipElement();
																								}
																							}
																						}
																						else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
																						{
																							reader->SkipElement();
																						}
																					}
																				}
																				else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
																				{
																					reader->SkipElement();
																				}
																			}
																		}
																		else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
																		{
																			reader->SkipElement();
																		}
																	}
																}
																else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
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
											}
										}
									}
									else if (Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"gml:curveProperty"))
									{
										if (layerType == Map::DRAW_LAYER_UNKNOWN || layerType == Map::DRAW_LAYER_POLYLINE3D)
										{
											layerType = Map::DRAW_LAYER_POLYLINE3D;
											while (reader->ReadNext())
											{
												if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
												{
													break;
												}
												else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
												{
													if (Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"gml:LineString"))
													{
														if (csys == 0)
														{
															i = reader->GetAttribCount();
															while (i-- > 0)
															{
																attr = reader->GetAttrib(i);
																if (Text::StrEquals(attr->name, (const UTF8Char*)"srsName"))
																{
																	csys = Math::CoordinateSystemManager::CreateFromName(attr->value);
																	if (csys)
																	{
																		srid = csys->GetSRID();
																	}
																	break;
																}
															}
														}
														if (lyr == 0)
														{
															colCnt = nameList.GetCount();
															cols = nameList.GetArray(&i);
															NEW_CLASS(lyr, Map::VectorLayer(layerType, fileName, colCnt, cols, csys, 0, 0));
														}
														if (colCnt == valList.GetCount())
														{
															Data::ArrayListDbl xPts;
															Data::ArrayListDbl yPts;
															Data::ArrayListDbl zPts;
															Math::Polyline3D *pl;
															Double *ptList;
															Double *hList;
															cols = valList.GetArray(&i);
															while (reader->ReadNext())
															{
																if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
																{
																	break;
																}
																else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
																{
																	if (Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"gml:posList"))
																	{
																		sb.ClearStr();
																		reader->ReadNodeText(&sb);
																		sarr[3] = sb.ToString();
																		while (true)
																		{
																			i = Text::StrSplit(sarr, 4, sarr[3], ' ');
																			if (i < 3)
																			{
																				break;
																			}
																			xPts.Add(Text::StrToDouble(sarr[1]));
																			yPts.Add(Text::StrToDouble(sarr[0]));
																			zPts.Add(Text::StrToDouble(sarr[2]));
																			if (i < 4)
																				break;
																		}

																		if (xPts.GetCount() > 0)
																		{
																			NEW_CLASS(pl, Math::Polyline3D(srid, 1, xPts.GetCount()));
																			ptList = pl->GetPointList(&i);
																			hList = pl->GetAltitudeList(&i);
																			while (i-- > 0)
																			{
																				hList[i] = zPts.GetItem(i);
																				ptList[(i << 1)] = xPts.GetItem(i);
																				ptList[(i << 1) + 1] = yPts.GetItem(i);
																			}
																			lyr->AddVector(pl, cols);
																		}
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
													else
													{
														reader->SkipElement();
													}
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
							i = nameList.GetCount();
							while (i-- > 0)
							{
								Text::StrDelNew(nameList.GetItem(i));
							}
							nameList.Clear();
							i = valList.GetCount();
							while (i-- > 0)
							{
								if (valList.GetItem(i))
								{
									Text::StrDelNew(valList.GetItem(i));
								}
							}
							valList.Clear();
						}
					}
				}
				else
				{
					reader->SkipElement();
				}
			}
		}
		DEL_CLASS(reader);
		return lyr;
	}
	else if (Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"fme:xml-tables"))
	{
		UOSInt i;
		UOSInt j;
		const UTF8Char *shortName;
		i = Text::StrLastIndexOf(fileName, IO::Path::PATH_SEPERATOR);
		if (IO::Path::PATH_SEPERATOR == '\\')
		{
			j = Text::StrLastIndexOf(fileName, '/');
			if (i == INVALID_INDEX || (j != INVALID_INDEX && j > i))
			{
				i = j;
			}
		}
		shortName = &fileName[i + 1];
		Text::StringBuilderUTF8 sbTableName;
		i = Text::StrIndexOf(shortName, '.');
		if (i != INVALID_INDEX && i > 0)
		{
			sbTableName.AppendC(shortName, i);
		}
		else
		{
			sbTableName.Append(shortName);
		}

		while (reader->ReadNext())
		{
			if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
			{
				break;
			}
			else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
			{
				if (Text::StrStartsWith(reader->GetNodeText(), (const UTF8Char*)"fme:") && Text::StrStartsWith(reader->GetNodeText() + 4, sbTableName.ToString()) && Text::StrEquals(reader->GetNodeText() + 4 + sbTableName.GetLength(), (const UTF8Char*)"-table"))
				{
					UOSInt i;
					UOSInt j;
					DB::TextDB *db = 0;
					Text::StringBuilderUTF8 sb;
					Data::ArrayList<const UTF8Char *> colList;
					Data::ArrayList<const UTF8Char *> nameList;
					Data::ArrayList<const UTF8Char *> valList;
					Bool succ = false;
					while (reader->ReadNext())
					{
						if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
						{
							succ = true;
							break;
						}
						else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
						{
							if (Text::StrStartsWith(reader->GetNodeText(), (const UTF8Char*)"fme:") && Text::StrEquals(reader->GetNodeText() + 4, sbTableName.ToString()))
							{
								while (reader->ReadNext())
								{
									if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
									{
										if (nameList.GetCount() > 0)
										{
											if (colList.GetCount() == 0)
											{
												i = 0;
												j = nameList.GetCount();
												while (i < j)
												{
													colList.Add(Text::StrCopyNew(nameList.GetItem(i)));
													i++;
												}
												NEW_CLASS(db, DB::TextDB(fileName));
												db->AddTable(sbTableName.ToString(), &colList);
											}
											Bool eq = true;
											if (colList.GetCount() == nameList.GetCount())
											{
												i = colList.GetCount();
												while (i-- > 0)
												{
													if (!Text::StrEquals(colList.GetItem(i), nameList.GetItem(i)))
													{
														eq = false;
														break;
													}
												}
											}
											else
											{
												eq = false;
											}
											
											if (eq)
											{
												db->AddTableData(&valList);
											}
											i = nameList.GetCount();
											while (i-- > 0)
											{
												Text::StrDelNew(nameList.GetItem(i));
											}
											i = valList.GetCount();
											while (i-- > 0)
											{
												Text::StrDelNew(valList.GetItem(i));
											}
											nameList.Clear();
											valList.Clear();
										}
										break;
									}
									else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
									{
										if (Text::StrStartsWith(reader->GetNodeText(), (const UTF8Char*)"fme:"))
										{
											nameList.Add(Text::StrCopyNew(reader->GetNodeText() + 4));
											sb.ClearStr();
											reader->ReadNodeText(&sb);
											valList.Add(Text::StrCopyNew(sb.ToString()));
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
					i = colList.GetCount();
					while (i-- > 0)
					{
						Text::StrDelNew(colList.GetItem(i));
					}
					i = nameList.GetCount();
					while (i-- > 0)
					{
						Text::StrDelNew(nameList.GetItem(i));
					}
					i = valList.GetCount();
					while (i-- > 0)
					{
						Text::StrDelNew(valList.GetItem(i));
					}
					if (!succ)
					{
						SDEL_CLASS(db);
					}
					DEL_CLASS(reader);
					return db;
				}
				else
				{
					reader->SkipElement();
				}
			}
		}
	}
	DEL_CLASS(reader);
	return 0;
}

Map::IMapDrawLayer *Parser::FileParser::XMLParser::ParseKMLContainer(Text::XMLReader *reader, Data::ICaseStringUTF8Map<KMLStyle*> *styles, const UTF8Char *sourceName, Parser::ParserList *parsers, Net::WebBrowser *browser, IO::PackageFile *basePF)
{
	KMLStyle *style;
	UOSInt i;
	Text::XMLAttrib *attr;
	Text::StringBuilderUTF8 sb;
	Data::DateTime dt;
	UTF8Char sbuff[512];
	Data::ArrayList<Map::IMapDrawLayer *> layers;

	Map::WebImageLayer *imgLyr = 0;
	Text::StringBuilderUTF8 containerNameSb;
	containerNameSb.Append(sourceName);
	Map::IMapDrawLayer *lyr;
	i = Text::StrLastIndexOf(containerNameSb.ToString(), '/');
	if (i != INVALID_INDEX)
	{
		containerNameSb.SetSubstr(i + 1);
	}
	i = Text::StrLastIndexOf(containerNameSb.ToString(), '\\');
	if (i != INVALID_INDEX)
	{
		containerNameSb.SetSubstr(i + 1);
	}

	while (reader->ReadNext())
	{
		if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
		{
			break;
		}
		else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
		{
			if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"NetworkLink"))
			{
				if (parsers && browser)
				{
					Map::ReloadableMapLayer *lyr;
					const UTF8Char *layerName = 0;
					const UTF8Char *url = 0;
					Int32 interval = 0;
					NEW_CLASS(lyr, Map::ReloadableMapLayer(sourceName, parsers, browser, 0));
				
					while (reader->ReadNext())
					{
						if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
						{
							break;
						}
						else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
						{
							if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"NAME"))
							{
								sb.ClearStr();
								reader->ReadNodeText(&sb);
								SDEL_TEXT(layerName);
								layerName = Text::StrCopyNew(sb.ToString());
							}
							else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"LINK"))
							{
								while (reader->ReadNext())
								{
									if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
									{
										break;
									}
									else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
									{
										if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"HREF"))
										{
											sb.ClearStr();
											reader->ReadNodeText(&sb);
											SDEL_TEXT(url);
											url = Text::StrCopyNew(sb.ToString());
										}
										else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"REFRESHINTERVAL"))
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
						lyr->AddInnerLayer(layerName, url, interval);
					}

					SDEL_TEXT(layerName);
					SDEL_TEXT(url);
					layers.Add(lyr);
				}
				else
				{
					reader->SkipElement();
				}
			}
			else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"STYLE"))
			{
				const UTF8Char *styleId = 0;
				style = MemAlloc(KMLStyle, 1);
				style->iconSpotX = -1;
				style->iconSpotY = -1;
				style->iconURL = 0;
				style->iconColor = 0;
				style->lineColor = 0;
				style->lineWidth = 0;
				style->fillColor = 0;
				style->flags = 0;
				i = reader->GetAttribCount();
				while (i-- > 0)
				{
					attr = reader->GetAttrib(i);
					if (Text::StrEqualsICase(attr->name, (const UTF8Char*)"ID"))
					{
						styleId = Text::StrCopyNew(attr->value);
						break;
					}
				}

				while (reader->ReadNext())
				{
					if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
					{
						break;
					}
					else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
					{
						if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"LINESTYLE"))
						{
							while (reader->ReadNext())
							{
								if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
								{
									break;
								}
								else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
								{
									if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"COLOR"))
									{
										sb.ClearStr();
										reader->ReadNodeText(&sb);
										style->lineColor = Text::StrHex2UInt32C(sb.ToString());
										style->lineColor = (style->lineColor & 0xff00ff00) | ((style->lineColor & 0xff) << 16) | ((style->lineColor & 0xff0000) >> 16);
										style->flags |= 1;
									}
									else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"WIDTH"))
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
						else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"ICONSTYLE"))
						{
							while (reader->ReadNext())
							{
								if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
								{
									break;
								}
								else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
								{
									if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"COLOR"))
									{
										sb.ClearStr();
										reader->ReadNodeText(&sb);
										style->iconColor = Text::StrHex2UInt32C(sb.ToString());
										style->iconColor = (style->iconColor & 0xff00ff00) | ((style->iconColor & 0xff) << 16) | ((style->iconColor & 0xff0000) >> 16);
									}
									else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"ICON"))
									{
										while (reader->ReadNext())
										{
											if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
											{
												break;
											}
											else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"HREF"))
											{
												sb.ClearStr();
												reader->ReadNodeText(&sb);
												if (sb.GetLength() > 0)
												{
													SDEL_TEXT(style->iconURL);
													style->iconURL = Text::StrCopyNew(sb.ToString());
												}
											}
											else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
											{
												reader->SkipElement();
											}
										}
									}
									else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"HOTSPOT"))
									{
										i = reader->GetAttribCount();
										while (i-- > 0)
										{
											attr = reader->GetAttrib(i);
											if (Text::StrEqualsICase(attr->name, (const UTF8Char*)"X"))
											{
												style->iconSpotX = Text::StrToInt32(attr->value);
											}
											else if (Text::StrEqualsICase(attr->name, (const UTF8Char*)"Y"))
											{
												style->iconSpotY = Text::StrToInt32(attr->value);
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
						else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"POLYSTYLE"))
						{
							while (reader->ReadNext())
							{
								if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
								{
									break;
								}
								else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"COLOR"))
								{
									sb.ClearStr();
									reader->ReadNodeText(&sb);
									style->fillColor = Text::StrHex2UInt32C(sb.ToString());
									style->fillColor = (style->fillColor & 0xff00ff00) | ((style->fillColor & 0xff) << 16) | ((style->fillColor & 0xff0000) >> 16);
									style->flags |= 4;
								}
								else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
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
					Text::StrDelNew(styleId);
				}				
				if (style)
				{
					SDEL_TEXT(style->iconURL);
					MemFree(style);
					style = 0;
				}
			}
			else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"STYLEMAP"))
			{
				const UTF8Char *styleId = 0;
				style = MemAlloc(KMLStyle, 1);
				style->iconSpotX = -1;
				style->iconSpotY = -1;
				style->iconURL = 0;
				style->iconColor = 0;
				style->lineColor = 0;
				style->lineWidth = 0;
				style->fillColor = 0;
				style->flags = 0;
				i = reader->GetAttribCount();
				while (i-- > 0)
				{
					attr = reader->GetAttrib(i);
					if (Text::StrEqualsICase(attr->name, (const UTF8Char*)"ID"))
					{
						styleId = Text::StrCopyNew(attr->value);
						break;
					}
				}

				while (reader->ReadNext())
				{
					if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
					{
						break;
					}
					else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"PAIR"))
					{
						Bool isNormal = false;
						while (reader->ReadNext())
						{
							if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
							{
								break;
							}
							else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
							{
								if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"KEY"))
								{
									sb.ClearStr();
									reader->ReadNodeText(&sb);
									if (sb.Equals((const UTF8Char*)"normal"))
									{
										isNormal = true;
									}
								}
								else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"STYLEURL"))
								{
									sb.ClearStr();
									reader->ReadNodeText(&sb);
									if (isNormal && sb.StartsWith((const UTF8Char*)"#"))
									{
										KMLStyle *style2 = styles->Get(sb.ToString() + 1);
										if (style2)
										{
											style->iconSpotX = style2->iconSpotX;
											style->iconSpotY = style2->iconSpotY;
											style->lineColor = style2->lineColor;
											style->lineWidth = style2->lineWidth;
											SDEL_TEXT(style->iconURL);
											if (style2->iconURL)
											{
												style->iconURL = Text::StrCopyNew(style2->iconURL);
											}
											style->fillColor = style2->fillColor;
											style->flags = style2->flags;
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
					else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
					{
						reader->SkipElement();
					}
				}

				if (styleId)
				{
					style = styles->Put(styleId, style);
					Text::StrDelNew(styleId);
				}
				if (style)
				{
					SDEL_TEXT(style->iconURL);
					MemFree(style);
					style = 0;
				}
			}
			else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"PLACEMARK"))
			{
				lyr = ParseKMLPlacemarkLyr(reader, styles, sourceName, parsers, browser, basePF);
				if (lyr)
				{
					layers.Add(lyr);
				}
			}
			else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"SCREENOVERLAY"))
			{
				if (imgLyr == 0)
				{
					NEW_CLASS(imgLyr, Map::WebImageLayer(browser, parsers, sourceName, Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::GeographicCoordinateSystem::GCST_WGS84), containerNameSb.ToString()));
				}

				const UTF8Char *name = 0;
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

				while (reader->ReadNext())
				{
					if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
					{
						break;
					}
					else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
					{
						if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"NAME"))
						{
							sb.ClearStr();
							reader->ReadNodeText(&sb);
							if (name)
							{
								Text::StrDelNew(name);
							}
							name = Text::StrCopyNew(sb.ToString());
						}
						else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"COLOR"))
						{
							sb.ClearStr();
							reader->ReadNodeText(&sb);
							color = Text::StrHex2UInt32C(sb.ToString());
						}
						else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"DRAWORDER"))
						{
							sb.ClearStr();
							reader->ReadNodeText(&sb);
							zIndex = Text::StrToInt32(sb.ToString());
						}
						else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"ICON"))
						{
							while (reader->ReadNext())
							{
								if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
								{
									break;
								}
								else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"HREF"))
								{
									sb.ClearStr();
									reader->ReadNodeText(&sb);
									imgLyr->GetSourceName(sbuff);
									Text::URLString::AppendURLPath(sbuff, sb.ToString());
								}
								else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
								{
									reader->SkipElement();
								}
							}
						}
						else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"TIMESPAN"))
						{
							while (reader->ReadNext())
							{
								if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
								{
									break;
								}
								else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
								{
									if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"BEGIN"))
									{
										sb.ClearStr();
										reader->ReadNodeText(&sb);
										dt.SetValue(sb.ToString());
										timeStart = dt.ToUnixTimestamp();
									}
									else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"END"))
									{
										sb.ClearStr();
										reader->ReadNodeText(&sb);
										dt.SetValue(sb.ToString());
										timeEnd = dt.ToUnixTimestamp();
									}
									else
									{
										reader->SkipElement();
									}
								}
							}
						}
						else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"SCREENXY"))
						{
							i = reader->GetAttribCount();
							while (i-- > 0)
							{
								attr = reader->GetAttrib(i);
								if (Text::StrEqualsICase(attr->name, (const UTF8Char*)"X"))
								{
									minX = Text::StrToDouble(attr->value);
								}
								else if (Text::StrEqualsICase(attr->name, (const UTF8Char*)"Y"))
								{
									minY = Text::StrToDouble(attr->value);
								}
							}
							reader->SkipElement();
						}
						else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"OVERLAYXY"))
						{
							i = reader->GetAttribCount();
							while (i-- > 0)
							{
								attr = reader->GetAttrib(i);
								if (Text::StrEqualsICase(attr->name, (const UTF8Char*)"X"))
								{
									oX = Text::StrToDouble(attr->value);
								}
								else if (Text::StrEqualsICase(attr->name, (const UTF8Char*)"Y"))
								{
									oY = Text::StrToDouble(attr->value);
								}
							}
							reader->SkipElement();
						}
						else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"SIZE"))
						{
							i = reader->GetAttribCount();
							while (i--)
							{
								attr = reader->GetAttrib(i);
								if (Text::StrEqualsICase(attr->name, (const UTF8Char*)"X"))
								{
									sizeX = Text::StrToDouble(attr->value);
								}
								else if (Text::StrEqualsICase(attr->name, (const UTF8Char*)"Y"))
								{
									sizeY = Text::StrToDouble(attr->value);
								}
							}
							reader->SkipElement();
						}
						else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"ALTITUDE"))
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
					imgLyr->AddImage(name, sbuff, zIndex, minX, minY, oX, oY, sizeX, sizeY, true, timeStart, timeEnd, ((color >> 24) & 0xff) / 255.0, hasAltitude, altitude);
				}
				if (name)
				{
					Text::StrDelNew(name);
				}
			}
			else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"GROUNDOVERLAY"))
			{
				if (imgLyr == 0)
				{
					NEW_CLASS(imgLyr, Map::WebImageLayer(browser, parsers, sourceName, Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::GeographicCoordinateSystem::GCST_WGS84), containerNameSb.ToString()));
				}

				const UTF8Char *name = 0;
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
				while (reader->ReadNext())
				{
					if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
					{
						break;
					}
					else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
					{
						if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"NAME"))
						{
							sb.ClearStr();
							reader->ReadNodeText(&sb);
							if (name)
							{
								Text::StrDelNew(name);
							}
							name = Text::StrCopyNew(sb.ToString());
						}
						else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"COLOR"))
						{
							sb.ClearStr();
							reader->ReadNodeText(&sb);
							alpha = ((Text::StrHex2Int32C(sb.ToString()) >> 24) & 0xff) / 255.0;
						}
						else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"DRAWORDER"))
						{
							sb.ClearStr();
							reader->ReadNodeText(&sb);
							zIndex = Text::StrToInt32(sb.ToString());
						}
						else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"ICON"))
						{
							while (reader->ReadNext())
							{
								if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
								{
									break;
								}
								else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"HREF"))
								{
									sb.ClearStr();
									reader->ReadNodeText(&sb);
									imgLyr->GetSourceName(sbuff);
									Text::URLString::AppendURLPath(sbuff, sb.ToString());
								}
								else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
								{
									reader->SkipElement();
								}
							}
						}
						else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"TIMESPAN"))
						{
							while (reader->ReadNext())
							{
								if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
								{
									break;
								}
								else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
								{
									if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"BEGIN"))
									{
										sb.ClearStr();
										reader->ReadNodeText(&sb);
										dt.SetValue(sb.ToString());
										timeStart = dt.ToUnixTimestamp();
									}
									else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"END"))
									{
										sb.ClearStr();
										reader->ReadNodeText(&sb);
										dt.SetValue(sb.ToString());
										timeEnd = dt.ToUnixTimestamp();
									}
									else
									{
										reader->SkipElement();
									}
								}
							}
						}
						else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"LATLONBOX"))
						{
							while (reader->ReadNext())
							{
								if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
								{
									break;
								}
								else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
								{
									if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"NORTH"))
									{
										sb.ClearStr();
										reader->ReadNodeText(&sb);
										maxY = Text::StrToDouble(sb.ToString());
									}
									else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"SOUTH"))
									{
										sb.ClearStr();
										reader->ReadNodeText(&sb);
										minY = Text::StrToDouble(sb.ToString());
									}
									else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"EAST"))
									{
										sb.ClearStr();
										reader->ReadNodeText(&sb);
										maxX = Text::StrToDouble(sb.ToString());
									}
									else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"WEST"))
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
						else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"ALTITUDE"))
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
					imgLyr->AddImage(name, sbuff, zIndex, minX, minY, maxX, maxY, 0, 0, false, timeStart, timeEnd, alpha, hasAltitude, altitude);
				}
				SDEL_TEXT(name);
			}
			else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"FOLDER"))
			{
				lyr = ParseKMLContainer(reader, styles, sourceName, parsers, browser, basePF);
				if (lyr)
				{
					layers.Add(lyr);
				}
			}
			else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"DOCUMENT"))
			{
				lyr = ParseKMLContainer(reader, styles, sourceName, parsers, browser, basePF);
				if (lyr)
				{
					layers.Add(lyr);
				}
			}
			else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"NAME"))
			{
				while (reader->ReadNext())
				{
					if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
					{
						break;
					}
					else if (reader->GetNodeType() == Text::XMLNode::NT_TEXT)
					{
						containerNameSb.ClearStr();
						containerNameSb.Append(reader->GetNodeText());
					}
					else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
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
		NEW_CLASS(lyrColl, Map::MapLayerCollection(sourceName, containerNameSb.ToString()));
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

void Parser::FileParser::XMLParser::ParseKMLPlacemarkTrack(Text::XMLReader *reader, Map::GPSTrack *lyr, Data::StringUTF8Map<KMLStyle*> *styles)
{
	Data::ArrayList<const UTF8Char*> timeList;
	Data::ArrayList<const UTF8Char*> coordList;
	Bool lastTrack = false;
	while (reader->ReadNext())
	{
		if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
		{
			break;
		}
		else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
		{
			if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"GX:MULTITRACK"))
			{
				while (reader->ReadNext())
				{
					if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
					{
						break;
					}
					else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"GX:TRACK"))
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
							if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
							{
								break;
							}
							else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
							{
								if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"WHEN"))
								{
									sb.ClearStr();
									reader->ReadNodeText(&sb);
									timeList.Add(Text::StrCopyNew(sb.ToString()));
								}
								else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"GX:COORD"))
								{
									sb.ClearStr();
									reader->ReadNodeText(&sb);
									coordList.Add(Text::StrCopyNew(sb.ToString()));
								}
								else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"EXTENDEDDATA"))
								{
									if (timeList.GetCount() == coordList.GetCount())
									{
										Map::GPSTrack::GPSRecord rec;
										rec.heading = 0;
										rec.nSateUsed = 0;
										rec.nSateView = 0;
										rec.speed = 0;
										rec.valid = true;
										Data::DateTime dt;
										UOSInt i = 0;
										UOSInt j = timeList.GetCount();
										while (i < j)
										{
											dt.SetValue(timeList.GetItem(i)); 
											rec.utcTimeTicks = dt.ToTicks();
											
											Text::StrConcat(sbuff, coordList.GetItem(i));
											if (Text::StrSplit(strs, 4, sbuff, ' ') == 3)
											{
												rec.lon = Text::StrToDouble(strs[0]);
												rec.lat = Text::StrToDouble(strs[1]);
												rec.altitude = Text::StrToDouble(strs[2]);
												lyr->AddRecord(&rec);
											}
											Text::StrDelNew(timeList.GetItem(i));
											Text::StrDelNew(coordList.GetItem(i));
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
											Text::StrDelNew(timeList.GetItem(i));
										}
										i = coordList.GetCount();
										while (i-- > 0)
										{
											Text::StrDelNew(coordList.GetItem(i));
										}
										timeList.Clear();
										coordList.Clear();
									}
									UOSInt recCnt;
									Map::GPSTrack::GPSRecord *recs = lyr->GetTrack(0, &recCnt);
									while (reader->ReadNext())
									{
										if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
										{
											break;
										}
										else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"SCHEMADATA"))
										{
											while (reader->ReadNext())
											{
												if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
												{
													break;
												}
												else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"GX:SIMPLEARRAYDATA"))
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
														if (Text::StrEqualsICase(attr->name, (const UTF8Char*)"NAME"))
														{
															if (Text::StrEqualsICase(attr->value, (const UTF8Char*)"SPEED"))
															{
																j = 0;
																while (reader->ReadNext())
																{
																	if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
																	{
																		break;
																	}
																	else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"GX:VALUE"))
																	{
																		sb.ClearStr();
																		reader->ReadNodeText(&sb);
																		if (j < recCnt)
																		{
																			recs[j].speed = Text::StrToDouble(sb.ToString());
																		}
																		j++;
																	}
																	else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
																	{
																		reader->SkipElement();
																	}
																}
															}
															else if (Text::StrEqualsICase(attr->value, (const UTF8Char*)"BEARING"))
															{
																j = 0;
																while (reader->ReadNext())
																{
																	if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
																	{
																		break;
																	}
																	else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"GX:VALUE"))
																	{
																		sb.ClearStr();
																		reader->ReadNodeText(&sb);
																		if (j < recCnt)
																		{
																			recs[j].heading = Text::StrToDouble(sb.ToString());
																		}
																		j++;
																	}
																	else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
																	{
																		reader->SkipElement();
																	}
																}
															}
															else if (Text::StrEqualsICase(attr->value, (const UTF8Char*)"ACCURACY"))
															{
																j = 0;
																while (reader->ReadNext())
																{
																	if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
																	{
																		break;
																	}
																	else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"GX:VALUE"))
																	{
																		sb.ClearStr();
																		reader->ReadNodeText(&sb);
																		if (j < recCnt)
																		{
																			recs[j].nSateUsed = Math::Double2Int32(Text::StrToDouble(sb.ToString()));
																		}
																		j++;
																	}
																	else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
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
												else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
												{
													reader->SkipElement();
												}
											}
										}
										else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
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
							Map::GPSTrack::GPSRecord rec;
							rec.heading = 0;
							rec.nSateUsed = 0;
							rec.nSateView = 0;
							rec.speed = 0;
							rec.valid = true;
							Data::DateTime dt;
							UOSInt i = 0;
							UOSInt j = timeList.GetCount();
							while (i < j)
							{
								dt.SetValue(timeList.GetItem(i)); 
								rec.utcTimeTicks = dt.ToTicks();
								
								Text::StrConcat(sbuff, coordList.GetItem(i));
								if (Text::StrSplit(strs, 4, sbuff, ' ') == 3)
								{
									rec.lon = Text::StrToDouble(strs[0]);
									rec.lat = Text::StrToDouble(strs[1]);
									rec.altitude = Text::StrToDouble(strs[2]);
									lyr->AddRecord(&rec);
								}
								Text::StrDelNew(timeList.GetItem(i));
								Text::StrDelNew(coordList.GetItem(i));
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
								Text::StrDelNew(timeList.GetItem(i));
							}
							i = coordList.GetCount();
							while (i-- > 0)
							{
								Text::StrDelNew(coordList.GetItem(i));
							}
							timeList.Clear();
							coordList.Clear();
						}

						lastTrack = true;
					}
					else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
					{
						reader->SkipElement();
					}
				}
			}
			else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"GX:TRACK"))
			{
				Text::StringBuilderUTF8 sb;
				if (lastTrack)
				{
					lyr->NewTrack();
				}
				while (reader->ReadNext())
				{
					if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
					{
						break;
					}
					else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
					{
						if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"WHEN"))
						{
							sb.ClearStr();
							reader->ReadNodeText(&sb);
							timeList.Add(Text::StrCopyNew(sb.ToString()));
						}
						else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"GX:COORD"))
						{
							sb.ClearStr();
							reader->ReadNodeText(&sb);
							coordList.Add(Text::StrCopyNew(sb.ToString()));
						}
						else
						{
							reader->SkipElement();
						}
					}

				}

				if (timeList.GetCount() == coordList.GetCount())
				{
					Map::GPSTrack::GPSRecord rec;
					rec.heading = 0;
					rec.nSateUsed = 0;
					rec.nSateView = 0;
					rec.speed = 0;
					rec.valid = true;
					Data::DateTime dt;
					UTF8Char sbuff[256];
					UTF8Char *strs[4];
					UOSInt i = 0;
					UOSInt j = timeList.GetCount();
					while (i < j)
					{
						dt.SetValue(timeList.GetItem(i)); 
						rec.utcTimeTicks = dt.ToTicks();
						
						Text::StrConcat(sbuff, coordList.GetItem(i));
						if (Text::StrSplit(strs, 4, sbuff, ' ') == 3)
						{
							rec.lon = Text::StrToDouble(strs[0]);
							rec.lat = Text::StrToDouble(strs[1]);
							rec.altitude = Text::StrToDouble(strs[2]);
							lyr->AddRecord(&rec);
						}
						Text::StrDelNew(timeList.GetItem(i));
						Text::StrDelNew(coordList.GetItem(i));
						i++;
					}
				}
				else
				{
					UOSInt i = timeList.GetCount();
					while (i-- > 0)
					{
						Text::StrDelNew(timeList.GetItem(i));
					}
					i = coordList.GetCount();
					while (i-- > 0)
					{
						Text::StrDelNew(coordList.GetItem(i));
					}
				}
				timeList.Clear();
				coordList.Clear();

				lastTrack = true;
			}
			else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"STYLEURL"))
			{
				if (!lyr->HasLineStyle())
				{
					Text::StringBuilderUTF8 sb;
					reader->ReadNodeText(&sb);
					if (sb.StartsWith((const UTF8Char*)"#"))
					{
						KMLStyle *style = styles->Get(sb.ToString() + 1);
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

void Parser::FileParser::XMLParser::ParseCoordinates(Text::XMLReader *reader, Data::ArrayList<Double> *coordList, Data::ArrayList<Double> *altList)
{
	UOSInt i;
	UTF8Char *sptr;
	UTF8Char *sptr2;
	UTF8Char c;
	UTF8Char sbuff[256];
	UTF8Char *sarr[4];

	while (reader->ReadNext())
	{
		if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
		{
			break;
		}
		else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
		{
			if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"LINEARRING"))
			{
				ParseCoordinates(reader, coordList, altList);
			}
			else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"COORDINATES"))
			{
				Text::StringBuilderUTF8 sb;
				reader->ReadNodeText(&sb);
				sptr = sb.ToString();
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
					Text::StrConcat(sbuff, sptr);
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

Map::IMapDrawLayer *Parser::FileParser::XMLParser::ParseKMLPlacemarkLyr(Text::XMLReader *reader, Data::StringUTF8Map<KMLStyle*> *styles, const UTF8Char *sourceName, Parser::ParserList *parsers, Net::WebBrowser *browser, IO::PackageFile *basePF)
{
	Text::StringBuilderUTF8 lyrNameSb;
	Text::StringBuilderUTF8 sb;
	lyrNameSb.Append((const UTF8Char*)"Layer");
	KMLStyle *style = 0;
	Data::ArrayList<Map::IMapDrawLayer*> layers;
	while (reader->ReadNext())
	{
		if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
		{
			break;
		}
		else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
		{
			if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"NAME"))
			{
				lyrNameSb.ClearStr();
				reader->ReadNodeText(&lyrNameSb);
			}
			else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"STYLEURL"))
			{
				sb.ClearStr();
				reader->ReadNodeText(&sb);
				if (sb.ToString()[0] == '#')
				{
					style = styles->Get(sb.ToString() + 1);
				}
			}
			else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"LINESTRING"))
			{
				Map::VectorLayer *lyr;
				const UTF8Char *cols = (const UTF8Char*)"Name";
				DB::DBUtil::ColType colType = DB::DBUtil::CT_VarChar;
				UOSInt colSize = 256;
				UOSInt colDP = 0;
				NEW_CLASS(lyr, Map::VectorLayer(Map::DRAW_LAYER_POLYLINE3D, sourceName, 1, &cols, Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::GeographicCoordinateSystem::GCST_WGS84), &colType, &colSize, &colDP, 0, lyrNameSb.ToString()));

				while (reader->ReadNext())
				{
					if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
					{
						break;
					}
					else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"COORDINATES"))
					{
						Data::ArrayListDbl *coord;
						Data::ArrayListDbl *altList;
						UTF8Char c;
						UTF8Char *sptr;
						UTF8Char *sptr2;
						UTF8Char sbuff[256];
						UTF8Char *sarr[4];
						UOSInt i;

						sb.ClearStr();
						reader->ReadNodeText(&sb);

						NEW_CLASS(coord, Data::ArrayListDbl());
						NEW_CLASS(altList, Data::ArrayListDbl());
						sptr = sb.ToString();
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
							Text::StrConcat(sbuff, sptr);
							*sptr2 = c;
							sptr = sptr2;
							i = Text::StrSplit(sarr, 4, sbuff, ',');
							if (i == 3)
							{
								coord->Add(Text::StrToDouble(sarr[0]));
								coord->Add(Text::StrToDouble(sarr[1]));
								altList->Add(Text::StrToDouble(sarr[2]));
							}
							else if (i == 2)
							{
								coord->Add(Text::StrToDouble(sarr[0]));
								coord->Add(Text::StrToDouble(sarr[1]));
								altList->Add(0);
							}
						}
						if (coord->GetCount() > 0)
						{
							Math::Polyline3D *pl;
							UOSInt nPoints;
							Double *ptArr;
							Double *altArr;
							const UTF8Char *csptr;
							UOSInt j;

							NEW_CLASS(pl, Math::Polyline3D(4326, 1, altList->GetCount()));
							pl->GetPtOfstList(&nPoints)[0] = 0;
							altArr = pl->GetAltitudeList(&nPoints);
							ptArr = pl->GetPointList(&nPoints);
							i = altList->GetCount();
							MemCopyNO(ptArr, coord->GetArray(&j), sizeof(Double) * 2 * i);
							MemCopyNO(altArr, altList->GetArray(&j), sizeof(Double) * i);
							csptr = sb.ToString();
							lyr->AddVector(pl, &csptr);
						}
						DEL_CLASS(coord);
						DEL_CLASS(altList);
					}
					else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
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
			else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"POINT"))
			{
				Map::VectorLayer *lyr;
				const UTF8Char *cols = (const UTF8Char*)"Name";
				DB::DBUtil::ColType colType = DB::DBUtil::CT_VarChar;
				UOSInt colSize = 256;
				UOSInt colDP = 0;
				NEW_CLASS(lyr, Map::VectorLayer(Map::DRAW_LAYER_POINT, sourceName, 1, &cols, Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::GeographicCoordinateSystem::GCST_WGS84), &colType, &colSize, &colDP, 0, lyrNameSb.ToString()));
				lyr->SetLabelVisible(true);
				while (reader->ReadNext())
				{
					if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
					{
						break;
					}
					else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"COORDINATES"))
					{
						sb.ClearStr();
						reader->ReadNodeText(&sb);

						Double x;
						Double y;
						Double z;
						UOSInt i;
						UTF8Char *sarr[4];
						i = Text::StrSplitTrim(sarr, 4, sb.ToString(), ',');
						if (i == 3)
						{
							Math::Point3D *pt;
							x = Text::StrToDouble(sarr[0]);
							y = Text::StrToDouble(sarr[1]);
							z = Text::StrToDouble(sarr[2]);
							NEW_CLASS(pt, Math::Point3D(4326, x, y, z));
							const UTF8Char *csptr;
							csptr = sb.ToString();
							lyr->AddVector(pt, &csptr);
						}
						else if (i == 2)
						{
							Math::Point3D *pt;
							x = Text::StrToDouble(sarr[0]);
							y = Text::StrToDouble(sarr[1]);
							NEW_CLASS(pt, Math::Point3D(4326, x, y, 0));
							const UTF8Char *csptr;
							csptr = sb.ToString();
							lyr->AddVector(pt, &csptr);
						}
					}
					else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
					{
						reader->SkipElement();
					}
				}

				if (style && style->iconURL && parsers)
				{
					IO::IStreamData *fd = 0;
					if (basePF)
					{
						fd = basePF->OpenStreamData(style->iconURL);
					}
					if (fd == 0 && browser)
					{
						fd = browser->GetData(style->iconURL, false, 0);
					}
					if (fd)
					{
						Media::ImageList *imgList = (Media::ImageList*)parsers->ParseFileType(fd, IO::ParsedObject::PT_IMAGE_LIST_PARSER);
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
							Media::Image *img = imgList->GetImage(0, 0);
							if (style->iconSpotX == -1 || style->iconSpotY == -1)
							{
								lyr->SetIconStyle(imgList, (OSInt)(img->info->dispWidth >> 1), (OSInt)(img->info->dispHeight >> 1));
							}
							else
							{
								lyr->SetIconStyle(imgList, style->iconSpotX, (OSInt)img->info->dispHeight - style->iconSpotY);
							}
						}
						DEL_CLASS(fd);
					}
				}
				layers.Add(lyr);
			}
			else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"POLYGON"))
			{
				Map::VectorLayer *lyr;
				const UTF8Char *cols = (const UTF8Char*)"Name";
				DB::DBUtil::ColType colType = DB::DBUtil::CT_VarChar;
				UOSInt colSize = 256;
				UOSInt colDP = 0;
				Data::ArrayListDbl *coord;
				Data::ArrayListDbl *altList;
				NEW_CLASS(lyr, Map::VectorLayer(Map::DRAW_LAYER_POLYGON, sourceName, 1, &cols, Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::GeographicCoordinateSystem::GCST_WGS84), &colType, &colSize, &colDP, 0, lyrNameSb.ToString()));

				NEW_CLASS(coord, Data::ArrayListDbl());
				NEW_CLASS(altList, Data::ArrayListDbl());
				while (reader->ReadNext())
				{
					if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
					{
						break;
					}
					else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
					{
						if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"OUTERBOUNDARYIS"))
						{
							ParseCoordinates(reader, coord, 0);
						}
						else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"INNERBOUNDARYIS"))
						{
							ParseCoordinates(reader, altList, 0);
						}
						else
						{
							reader->SkipElement();
						}
					}
				}
				if (coord->GetCount() > 0)
				{
					Math::Polygon *pg;
					UOSInt nPoints;
					Double *ptArr;
					UInt32 *ptList;
					const UTF8Char *csptr;
					UOSInt i;

					if (altList->GetCount() > 0)
					{
						NEW_CLASS(pg, Math::Polygon(4326, 2, (coord->GetCount() + altList->GetCount()) >> 1));
						ptList = pg->GetPtOfstList(&nPoints);
						ptList[0] = 0;
						ptList[1] = (UInt32)(coord->GetCount() >> 1);
						ptArr = pg->GetPointList(&nPoints);
						MemCopyNO(ptArr, coord->GetArray(&i), sizeof(Double) * coord->GetCount());
						MemCopyNO(&ptArr[coord->GetCount()], altList->GetArray(&i), sizeof(Double) * altList->GetCount());
						csptr = sb.ToString();
						lyr->AddVector(pg, &csptr);
					}
					else
					{
						NEW_CLASS(pg, Math::Polygon(4326, 1, coord->GetCount() >> 1));
						ptList = pg->GetPtOfstList(&nPoints);
						ptList[0] = 0;
						ptArr = pg->GetPointList(&nPoints);
						MemCopyNO(ptArr, coord->GetArray(&i), sizeof(Double) * coord->GetCount());
						csptr = sb.ToString();
						lyr->AddVector(pg, &csptr);
					}
				}
				DEL_CLASS(coord);
				DEL_CLASS(altList);

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
			else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"GX:MULTITRACK"))
			{
				Map::GPSTrack *lyr;
				NEW_CLASS(lyr, Map::GPSTrack(sourceName, true, 65001, lyrNameSb.ToString()));
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

Bool Parser::FileParser::XMLParser::ParseGPXPoint(Text::XMLReader *reader, Map::GPSTrack::GPSRecord *rec)
{
	UOSInt i;
	UOSInt j;
	Text::XMLAttrib *attr;
	Text::StringBuilderUTF8 sb;
	Bool succ;

	i = 0;
	j = reader->GetAttribCount();
	while (i < j)
	{
		attr = reader->GetAttrib(i);
		if (Text::StrEqualsICase(attr->name, (const UTF8Char*)"LAT"))
		{
			rec->lat = Text::StrToDouble(attr->value);
		}
		else if (Text::StrEqualsICase(attr->name, (const UTF8Char*)"LON"))
		{
			rec->lon = Text::StrToDouble(attr->value);
		}
		i++;
	}

	succ = true;
	while (true)
	{
		if (!reader->ReadNext())
		{
			succ = false;
			break;
		}
		if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
		{
			break;
		}
		else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
		{
			if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"ELE"))
			{
				reader->ReadNodeText(&sb);
				rec->altitude = Text::StrToDouble(sb.ToString());
				sb.ClearStr();
			}
			else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"TIME"))
			{
				reader->ReadNodeText(&sb);
				Data::DateTime dt;
				dt.SetValue(sb.ToString());
				sb.ClearStr();
				rec->utcTimeTicks = dt.ToTicks();
			}
			else if (Text::StrEqualsICase(reader->GetNodeText(), (const UTF8Char*)"SPEED"))
			{
				reader->ReadNodeText(&sb);
				rec->speed = Text::StrToDouble(sb.ToString()) * 3.6 / 1.852;
				sb.ClearStr();
			}
			else
			{
				reader->SkipElement();
			}
		}
		i++;
	}
	rec->heading = 0;
	rec->nSateUsed = -1;
	rec->nSateView = -1;
	rec->valid = true;
	return succ;
}

Bool Parser::FileParser::XMLParser::ParseVSProjFile(Text::XMLReader *reader, Text::VSProjContainer *container)
{
	UOSInt i;
	Bool found;
	Text::XMLAttrib *attr;
	while (reader->ReadNext())
	{
		if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
		{
			return true;
		}
		else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
		{
			if (Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"Filter"))
			{
				found = false;
				i = reader->GetAttribCount();
				while (i-- > 0)
				{
					attr = reader->GetAttrib(i);
					if (Text::StrEquals(attr->name, (const UTF8Char*)"Name"))
					{
						Text::VSContainer *childCont;
						found = true;
						NEW_CLASS(childCont, Text::VSContainer(attr->value));
						container->AddChild(childCont);
						if (!ParseVSProjFile(reader, childCont))
						{
							return false;
						}
						break;
					}
				}
				if (!found)
				{
					if (!reader->SkipElement())
					{
						return false;
					}
				}
			}
			else if (Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"File"))
			{
				i = reader->GetAttribCount();
				while (i-- > 0)
				{
					attr = reader->GetAttrib(i);
					if (Text::StrEquals(attr->name, (const UTF8Char*)"RelativePath"))
					{
						Text::VSFile *childFile;
						NEW_CLASS(childFile, Text::VSFile(attr->value));
						container->AddChild(childFile);
						break;
					}
				}
				if (!reader->SkipElement())
				{
					return false;
				}
			}
			else
			{
				if (!reader->SkipElement())
				{
					return false;
				}
			}
		}
	}
	return false;
}

Bool Parser::FileParser::XMLParser::ParseVSConfFile(Text::XMLReader *reader, Text::CodeProject *proj)
{
	UOSInt i;
	Text::CodeProjectCfg *cfg;
	Text::XMLAttrib *attr;
	const UTF8Char *cfgName;
	while (reader->ReadNext())
	{
		if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
		{
			return true;
		}
		else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
		{
			if (Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"Configuration"))
			{
				cfg = 0;
				cfgName = 0;
				i = reader->GetAttribCount();
				while (i-- > 0)
				{
					attr = reader->GetAttrib(i);
					if (Text::StrEquals(attr->name, (const UTF8Char*)"Name"))
					{
						cfgName = attr->value;
						break;
					}
				}
				if (cfgName)
				{
					NEW_CLASS(cfg, Text::CodeProjectCfg(cfgName));
					while (true)
					{
						if (!reader->ReadNext())
						{
							return false;
						}
						if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
						{
							break;
						}
						else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
						{
							if (Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"Tool"))
							{
								i = reader->GetAttribCount();
								while (i-- > 0)
								{
									attr = reader->GetAttrib(i);
									if (Text::StrEquals(attr->name, (const UTF8Char*)"Name"))
									{
									}
									else
									{
										cfg->SetValue(0, attr->name, attr->value);
									}
								}
							}
							if (!reader->SkipElement())
							{
								return false;
							}
						}
					}
					proj->AddConfig(cfg);
				}
				else
				{
					if (!reader->SkipElement())
					{
						return false;
					}
				}
			}
			else
			{
				if (!reader->SkipElement())
				{
					return false;
				}
			}
		}
	}
	return false;
}
