#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListDbl.h"
#include "Data/Int64Map.h"
#include "DB/TextDB.h"
#include "IO/PackageFile.h"
#include "IO/Path.h"
#include "IO/StreamDataStream.h"
#include "IO/SystemInfoLog.h"
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

void Parser::FileParser::XMLParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MapLayer)
	{
		selector->AddFilter(CSTR("*.xml"), CSTR("XML File"));
		selector->AddFilter(CSTR("*.gpx"), CSTR("GPS Track File"));
		selector->AddFilter(CSTR("*.kml"), CSTR("KML File"));
		selector->AddFilter(CSTR("*.osm"), CSTR("OpenStreetMap File"));
		selector->AddFilter(CSTR("*.vcproj"), CSTR("VC Project File"));
		selector->AddFilter(CSTR("*.gml"), CSTR("GML File"));
	}
}

IO::ParserType Parser::FileParser::XMLParser::GetParserType()
{
	return IO::ParserType::MapLayer;
}

IO::ParsedObject *Parser::FileParser::XMLParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	Bool valid = false;

	sptr = fd->GetFullName()->ConcatTo(sbuff);
	i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '.');
	j = Text::StrIndexOfCharC(&sbuff[i + 1], (UOSInt)(sptr - &sbuff[i + 1]), '?');
	if (j != INVALID_INDEX)
	{
		sbuff[i + j + 1] = 0;
		sptr = &sbuff[i + j + 1];
	}
	if (i == INVALID_INDEX)
	{

	}
	else if (Text::StrEqualsICaseC(&sbuff[i], (UOSInt)(sptr - &sbuff[i]), UTF8STRC(".XML")))
	{
		valid = true;
	}
	else if (Text::StrEqualsICaseC(&sbuff[i], (UOSInt)(sptr - &sbuff[i]), UTF8STRC(".GPX")))
	{
		valid = true;
	}
	else if (Text::StrEqualsICaseC(&sbuff[i], (UOSInt)(sptr - &sbuff[i]), UTF8STRC(".KML")))
	{
		valid = true;
	}
	else if (Text::StrEqualsICaseC(&sbuff[i], (UOSInt)(sptr - &sbuff[i]), UTF8STRC(".OSM")))
	{
		valid = true;
	}
	else if (Text::StrEqualsICaseC(&sbuff[i], (UOSInt)(sptr - &sbuff[i]), UTF8STRC(".VCPROJ")))
	{
		valid = true;
	}
	else if (Text::StrEqualsICaseC(&sbuff[i], (UOSInt)(sptr - &sbuff[i]), UTF8STRC(".GML")))
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
	pobj = ParseStream(this->encFact, stm, fd->GetFullName()->v, this->parsers, this->browser, pkgFile);
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
	UOSInt fileNameLen = Text::StrCharCnt(fileName);

	if (reader->GetNodeText()->Equals(UTF8STRC("kml")))
	{
		Data::ICaseStringMap<KMLStyle*> styles;
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
		Map::IMapDrawLayer *lyr = ParseKMLContainer(reader, &styles, {fileName, fileNameLen}, parsers, browser, pkgFile);

		Data::ArrayList<KMLStyle*> *styleList = styles.GetValues();
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
	else if (reader->GetNodeText()->Equals(UTF8STRC("gpx")))
	{
		UOSInt i;
		UOSInt j;
		const UTF8Char *shortName;
		i = Text::StrLastIndexOfCharC(fileName, fileNameLen, IO::Path::PATH_SEPERATOR);
		if (IO::Path::PATH_SEPERATOR == '\\')
		{
			j = Text::StrLastIndexOfCharC(fileName, fileNameLen, '/');
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
				if (reader->GetNodeText()->Equals(UTF8STRC("trk"))) // /gpx/trk/trkseg
				{
					Map::GPSTrack *track;
					NEW_CLASS(track, Map::GPSTrack({fileName, fileNameLen}, true, 0, CSTR_NULL));
					track->SetTrackName({shortName, (UOSInt)(fileName + fileNameLen - shortName)});
					while (reader->ReadNext())
					{
						if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
						{
							break;
						}
						else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
						{
							if (reader->GetNodeText()->Equals(UTF8STRC("trkseg")))
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
										if (reader->GetNodeText()->EqualsICase(UTF8STRC("TRKPT")))
										{
											Map::GPSTrack::GPSRecord2 rec;
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
/*				else if (reader->GetNodeText()->Equals(UTF8STRC("rte")) // /gpx/rte
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
								if (Text::StrEqualsICaseC(node1->name->v, node1->name->leng, UTF8STRC("RTEPT")))
								{
									Map::GPSTrack::GPSRecord rec;
									ParseGPXPoint(node1, &rec);
									track->AddRecord(&rec);
								}
								else if (Text::StrEqualsICaseC(node1->name->v, node1->name->leng, UTF8STRC("NAME")))
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
/*				else if (reader->GetNodeText()->Equals(UTF8STRC("wpt")) // /gpx/wpt
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
	else if (reader->GetNodeText()->Equals(UTF8STRC("osm")))
	{
		Map::IMapDrawLayer *lyr = Map::OSM::OSMParser::ParseLayerNode(reader, {fileName, fileNameLen});
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
	else if (reader->GetNodeText()->Equals(UTF8STRC("OruxTracker")))
	{
		Map::OruxDBLayer *lyr = 0;
		while (reader->ReadNext())
		{
			if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
			{
				break;
			}
			else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && reader->GetNodeText()->Equals(UTF8STRC("MapCalibration")))
			{
				while (reader->ReadNext())
				{
					if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
					{
						break;
					}
					else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
					{
						if (reader->GetNodeText()->Equals(UTF8STRC("OruxTracker")))
						{
							if (lyr)
							{
								while (reader->ReadNext())
								{
									if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
									{
										break;
									}
									else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && reader->GetNodeText()->Equals(UTF8STRC("MapCalibration")))
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
											if (attr->name->Equals(UTF8STRC("layerLevel")))
											{
												if (attr->value->ToUInt32(&layerId))
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
												if (reader->GetNodeText()->Equals(UTF8STRC("MapChunks")))
												{
													i = reader->GetAttribCount();
													while (i-- > 0)
													{
														attr = reader->GetAttrib(i);
														if (attr->name->Equals(UTF8STRC("xMax")))
														{
															if (attr->value->ToUInt32(&maxX))
																flags |= 2;
														}
														else if (attr->name->Equals(UTF8STRC("yMax")))
														{
															if (attr->value->ToUInt32(&maxY))
																flags |= 4;
														}
														else if (attr->name->Equals(UTF8STRC("img_width")))
														{
															if (attr->value->ToUInt32(&tileSize))
																flags |= 8;
														}
													}
													reader->SkipElement();
												}
												else if (reader->GetNodeText()->Equals(UTF8STRC("MapBounds")))
												{
													i = reader->GetAttribCount();
													while (i-- > 0)
													{
														attr = reader->GetAttrib(i);
														if (attr->name->Equals(UTF8STRC("minLat")))
														{
															if (attr->value->ToDouble(&mapYMin))
																flags |= 16;
														}
														else if (attr->name->Equals(UTF8STRC("maxLat")))
														{
															if (attr->value->ToDouble(&mapYMax))
																flags |= 32;
														}
														else if (attr->name->Equals(UTF8STRC("minLon")))
														{
															if (attr->value->ToDouble(&mapXMin))
																flags |= 64;
														}
														else if (attr->name->Equals(UTF8STRC("maxLon")))
														{
															if (attr->value->ToDouble(&mapXMax))
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
						else if (reader->GetNodeText()->Equals(UTF8STRC("MapName")))
						{
							if (lyr == 0)
							{
								Text::StringBuilderUTF8 sb;
								reader->ReadNodeText(&sb);
								NEW_CLASS(lyr, Map::OruxDBLayer({fileName, fileNameLen}, sb.ToCString(), parsers));
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
	else if (reader->GetNodeText()->Equals(UTF8STRC("VisualStudioProject")))
	{
		Text::XMLAttrib *attr;
		Text::VSProject *proj = 0;
		Text::VSProject::VisualStudioVersion ver = Text::VSProject::VSV_UNKNOWN;
		Text::String *projName = 0;
		UOSInt i;
		i = reader->GetAttribCount();
		while (i-- > 0)
		{
			attr = reader->GetAttrib(i);
			if (attr->name->Equals(UTF8STRC("Version")))
			{
				if (attr->value->Equals(UTF8STRC("7.10")))
				{
					ver = Text::VSProject::VSV_VS71;
				}
				else if (attr->value->Equals(UTF8STRC("8.00")))
				{
					ver = Text::VSProject::VSV_VS8;
				}
				else if (attr->value->Equals(UTF8STRC("9.00")))
				{
					ver = Text::VSProject::VSV_VS9;
				}
			}
			else if (attr->name->Equals(UTF8STRC("Name")))
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
					if (reader->GetNodeText()->Equals(UTF8STRC("Configurations")))
					{
						if (!ParseVSConfFile(reader, proj))
						{
							break;
						}
					}
					else if (reader->GetNodeText()->Equals(UTF8STRC("Files")))
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
	else if (reader->GetNodeText()->Equals(UTF8STRC("gml:FeatureCollection")))
	{
		Math::CoordinateSystem *csys = 0;
		UInt32 srid = 0;
		UOSInt colCnt = 0;
		Data::ArrayList<const UTF8Char *> nameList;
		Data::ArrayList<Text::String *> valList;
		Text::StringBuilderUTF8 sb;
		Map::VectorLayer *lyr = 0;
		Map::DrawLayerType layerType = Map::DRAW_LAYER_UNKNOWN;
		Text::String **scols;
		const UTF8Char **ccols;
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
				if (reader->GetNodeText()->Equals(UTF8STRC("gml:featureMember")))
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
									if (reader->GetNodeText()->StartsWith(UTF8STRC("fme:")))
									{
										nameList.Add(Text::StrCopyNew(reader->GetNodeText()->v + 4));
										sb.ClearStr();
										reader->ReadNodeText(&sb);
										if (sb.GetLength() > 0)
										{
											valList.Add(Text::String::New(sb.ToString(), sb.GetLength()));
										}
										else
										{
											valList.Add(0);
										}
									}
									else if (reader->GetNodeText()->Equals(UTF8STRC("gml:pointProperty")))
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
													if (reader->GetNodeText()->Equals(UTF8STRC("gml:Point")))
													{
														if (csys == 0)
														{
															i = reader->GetAttribCount();
															while (i-- > 0)
															{
																attr = reader->GetAttrib(i);
																if (attr->name->Equals(UTF8STRC("srsName")))
																{
																	csys = Math::CoordinateSystemManager::CreateFromName(attr->value->ToCString());
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
															ccols = nameList.GetArray(&i);
															NEW_CLASS(lyr, Map::VectorLayer(layerType, {fileName, fileNameLen}, colCnt, ccols, csys, 0, CSTR_NULL));
														}
														if (colCnt == valList.GetCount())
														{
															Data::ArrayListDbl xPts;
															Data::ArrayListDbl yPts;
															Data::ArrayListDbl zPts;
															Math::Point3D *pt;
															scols = valList.GetArray(&i);
															while (reader->ReadNext())
															{
																if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
																{
																	break;
																}
																else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
																{
																	if (reader->GetNodeText()->Equals(UTF8STRC("gml:pos")))
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
																			lyr->AddVector(pt, scols);
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
									else if (reader->GetNodeText()->Equals(UTF8STRC("gml:surfaceProperty")))
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
													if (reader->GetNodeText()->Equals(UTF8STRC("gml:Surface")))
													{
														if (csys == 0)
														{
															i = reader->GetAttribCount();
															while (i-- > 0)
															{
																attr = reader->GetAttrib(i);
																if (attr->name->Equals(UTF8STRC("srsName")))
																{
																	csys = Math::CoordinateSystemManager::CreateFromName(attr->value->ToCString());
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
															ccols = nameList.GetArray(&i);
															NEW_CLASS(lyr, Map::VectorLayer(layerType, {fileName, fileNameLen}, colCnt, ccols, csys, 0, CSTR_NULL));
														}
														if (colCnt == valList.GetCount())
														{
															Data::ArrayListDbl xPts;
															Data::ArrayListDbl yPts;
															Data::ArrayListDbl zPts;
															Math::Polygon *pg;
															Double *ptList;
															scols = valList.GetArray(&i);
															while (reader->ReadNext())
															{
																if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
																{
																	break;
																}
																else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && reader->GetNodeText()->Equals(UTF8STRC("gml:patches")))
																{
																	while (reader->ReadNext())
																	{
																		if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
																		{
																			break;
																		}
																		else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && reader->GetNodeText()->Equals(UTF8STRC("gml:PolygonPatch")))
																		{
																			while (reader->ReadNext())
																			{
																				if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
																				{
																					break;
																				}
																				else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && reader->GetNodeText()->Equals(UTF8STRC("gml:exterior")))
																				{
																					while (reader->ReadNext())
																					{
																						if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
																						{
																							break;
																						}
																						else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && reader->GetNodeText()->Equals(UTF8STRC("gml:LinearRing")))
																						{
																							while (reader->ReadNext())
																							{
																								if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
																								{
																									break;
																								}
																								else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && reader->GetNodeText()->Equals(UTF8STRC("gml:posList")))
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
																										lyr->AddVector(pg, scols);
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
									else if (reader->GetNodeText()->Equals(UTF8STRC("gml:curveProperty")))
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
													if (reader->GetNodeText()->Equals(UTF8STRC("gml:LineString")))
													{
														if (csys == 0)
														{
															i = reader->GetAttribCount();
															while (i-- > 0)
															{
																attr = reader->GetAttrib(i);
																if (attr->name->Equals(UTF8STRC("srsName")))
																{
																	csys = Math::CoordinateSystemManager::CreateFromName(attr->value->ToCString());
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
															ccols = nameList.GetArray(&i);
															NEW_CLASS(lyr, Map::VectorLayer(layerType, {fileName, fileNameLen}, colCnt, ccols, csys, 0, CSTR_NULL));
														}
														if (colCnt == valList.GetCount())
														{
															Data::ArrayListDbl xPts;
															Data::ArrayListDbl yPts;
															Data::ArrayListDbl zPts;
															Math::Polyline3D *pl;
															Double *ptList;
															Double *hList;
															scols = valList.GetArray(&i);
															while (reader->ReadNext())
															{
																if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
																{
																	break;
																}
																else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
																{
																	if (reader->GetNodeText()->Equals(UTF8STRC("gml:posList")))
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
																			lyr->AddVector(pl, scols);
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
									valList.GetItem(i)->Release();
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
	else if (reader->GetNodeText()->Equals(UTF8STRC("fme:xml-tables")))
	{
		UOSInt i;
		UOSInt j;
		const UTF8Char *shortName;
		i = Text::StrLastIndexOfCharC(fileName, fileNameLen, IO::Path::PATH_SEPERATOR);
		if (IO::Path::PATH_SEPERATOR == '\\')
		{
			j = Text::StrLastIndexOfCharC(fileName, fileNameLen, '/');
			if (i == INVALID_INDEX || (j != INVALID_INDEX && j > i))
			{
				i = j;
			}
		}
		shortName = &fileName[i + 1];
		Text::StringBuilderUTF8 sbTableName;
		i = Text::StrIndexOfChar(shortName, '.');
		if (i != INVALID_INDEX && i > 0)
		{
			sbTableName.AppendC(shortName, i);
		}
		else
		{
			sbTableName.AppendSlow(shortName);
		}

		while (reader->ReadNext())
		{
			if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
			{
				break;
			}
			else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
			{
				if (reader->GetNodeText()->StartsWith(UTF8STRC("fme:")) && Text::StrStartsWith(reader->GetNodeText()->v + 4, sbTableName.ToString()) && Text::StrEquals(reader->GetNodeText()->v + 4 + sbTableName.GetLength(), (const UTF8Char*)"-table"))
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
							if (reader->GetNodeText()->StartsWith(UTF8STRC("fme:")) && Text::StrEquals(reader->GetNodeText()->v + 4, sbTableName.ToString()))
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
										if (reader->GetNodeText()->StartsWith(UTF8STRC("fme:")))
										{
											Text::String *txt = reader->GetNodeText();
											nameList.Add(Text::StrCopyNewC(txt->v + 4, txt->leng - 4));
											sb.ClearStr();
											reader->ReadNodeText(&sb);
											valList.Add(Text::StrCopyNewC(sb.ToString(), sb.GetLength()));
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
	else if (reader->GetNodeText()->Equals(UTF8STRC("SYSTEMINFO")))
	{
		Text::StringBuilderUTF8 sb;
		IO::SystemInfoLog *sysInfo;
		NEW_CLASS(sysInfo, IO::SystemInfoLog(Text::CString(fileName, fileNameLen)));
		while (reader->ReadNext())
		{
			if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
			{
				break;
			}
			else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
			{
				if (reader->GetNodeText()->Equals(UTF8STRC("SYSTEM")))
				{
					while (reader->ReadNext())
					{
						if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
						{
							break;
						}
						else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
						{
							if (reader->GetNodeText()->Equals(UTF8STRC("OSNAME")))
							{
								sb.ClearStr();
								if (reader->ReadNodeText(&sb))
								{
									sysInfo->SetOSName(sb.ToCString());
								}
							}
							else if (reader->GetNodeText()->Equals(UTF8STRC("OSVER")))
							{
								sb.ClearStr();
								if (reader->ReadNodeText(&sb))
								{
									sysInfo->SetOSVer(sb.ToCString());
								}
							}
							else if (reader->GetNodeText()->Equals(UTF8STRC("OSLANGUAGE")))
							{
								sb.ClearStr();
								if (reader->ReadNodeText(&sb))
								{
									sysInfo->SetOSLocale(Text::StrToUInt32(sb.ToString()));
								}
							}
							else if (reader->GetNodeText()->Equals(UTF8STRC("ARCHITECTURE")))
							{
								sb.ClearStr();
								if (reader->ReadNodeText(&sb))
								{
									sysInfo->SetArchitecture(Text::StrToUInt32(sb.ToString()));
								}
							}
							else if (reader->GetNodeText()->Equals(UTF8STRC("PRODUCTTYPE")))
							{
								sb.ClearStr();
								if (reader->ReadNodeText(&sb))
								{
									sysInfo->SetProductType(Text::StrToUInt32(sb.ToString()));
								}
							}
						}
					}
				}
				else if (reader->GetNodeText()->Equals(UTF8STRC("SERVERROLES")))
				{
					while (reader->ReadNext())
					{
						if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
						{
							break;
						}
						else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
						{
							if (reader->GetNodeText()->Equals(UTF8STRC("REG_VALUE")))
							{
								const UTF8Char *roleName = 0;
								const UTF8Char *roleData = 0;
								while (reader->ReadNext())
								{
									if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
									{
										break;
									}
									else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
									{
										if (reader->GetNodeText()->Equals(UTF8STRC("NAME")))
										{
											SDEL_TEXT(roleName);
											sb.ClearStr();
											if (reader->ReadNodeText(&sb))
											{
												roleName = Text::StrCopyNewC(sb.ToString(), sb.GetLength());
											}
										}
										else if (reader->GetNodeText()->Equals(UTF8STRC("DATA")))
										{
											SDEL_TEXT(roleData);
											sb.ClearStr();
											if (reader->ReadNodeText(&sb))
											{
												roleData = Text::StrCopyNewC(sb.ToString(), sb.GetLength());
											}
										}
										else
										{
											reader->SkipElement();
										}
									}
								}
								if (roleName && roleData)
								{
									sysInfo->AddServerRole(roleName, roleData);
								}
								SDEL_TEXT(roleName);
								SDEL_TEXT(roleData);
							}
							else
							{
								reader->SkipElement();
							}
						}
					}
				}
				else if (reader->GetNodeText()->Equals(UTF8STRC("DEVICES")))
				{
					while (reader->ReadNext())
					{
						if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
						{
							break;
						}
						else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
						{
							if (reader->GetNodeText()->Equals(UTF8STRC("DEVICE")))
							{
								const UTF8Char *desc = 0;
								const UTF8Char *hwId = 0;
								const UTF8Char *service = 0;
								const UTF8Char *driver = 0;
								while (reader->ReadNext())
								{
									if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
									{
										break;
									}
									else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
									{
										if (reader->GetNodeText()->Equals(UTF8STRC("DESCRIPTION")))
										{
											SDEL_TEXT(desc);
											sb.ClearStr();
											if (reader->ReadNodeText(&sb))
											{
												desc = Text::StrCopyNewC(sb.ToString(), sb.GetLength());
											}
										}
										else if (reader->GetNodeText()->Equals(UTF8STRC("HARDWAREID")))
										{
											SDEL_TEXT(hwId);
											sb.ClearStr();
											if (reader->ReadNodeText(&sb))
											{
												hwId = Text::StrCopyNewC(sb.ToString(), sb.GetLength());
											}
										}
										else if (reader->GetNodeText()->Equals(UTF8STRC("SERVICE")))
										{
											SDEL_TEXT(service);
											sb.ClearStr();
											if (reader->ReadNodeText(&sb))
											{
												service = Text::StrCopyNewC(sb.ToString(), sb.GetLength());
											}
										}
										else if (reader->GetNodeText()->Equals(UTF8STRC("DRIVER")))
										{
											SDEL_TEXT(driver);
											sb.ClearStr();
											if (reader->ReadNodeText(&sb))
											{
												driver = Text::StrCopyNewC(sb.ToString(), sb.GetLength());
											}
										}
										else
										{
											reader->SkipElement();
										}
									}
								}
								if (desc && hwId)
								{
									sysInfo->AddDeviceInfo(desc, hwId, service, driver);
								}
								SDEL_TEXT(desc);
								SDEL_TEXT(hwId);
								SDEL_TEXT(service);
								SDEL_TEXT(driver);
							}
							else
							{
								reader->SkipElement();
							}
						}
					}
				}
				else if (reader->GetNodeText()->Equals(UTF8STRC("DRIVERS")))
				{
					while (reader->ReadNext())
					{
						if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
						{
							break;
						}
						else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
						{
							if (reader->GetNodeText()->Equals(UTF8STRC("DRIVER")))
							{
								const UTF8Char *fileName = 0;
								UInt64 fileSize = 0;
								const UTF8Char *createDate = 0;
								const UTF8Char *version = 0;
								const UTF8Char *manufacturer = 0;
								const UTF8Char *productName = 0;
								const UTF8Char *group = 0;
								UInt32 altitude = 0;
								while (reader->ReadNext())
								{
									if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
									{
										break;
									}
									else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
									{
										if (reader->GetNodeText()->Equals(UTF8STRC("FILENAME")))
										{
											SDEL_TEXT(fileName);
											sb.ClearStr();
											if (reader->ReadNodeText(&sb))
											{
												fileName = Text::StrCopyNewC(sb.ToString(), sb.GetLength());
											}
										}
										else if (reader->GetNodeText()->Equals(UTF8STRC("FILESIZE")))
										{
											sb.ClearStr();
											if (reader->ReadNodeText(&sb))
											{
												fileSize = Text::StrToUInt64(sb.ToString());
											}
										}
										else if (reader->GetNodeText()->Equals(UTF8STRC("CREATIONDATE")))
										{
											SDEL_TEXT(createDate);
											sb.ClearStr();
											if (reader->ReadNodeText(&sb))
											{
												createDate = Text::StrCopyNewC(sb.ToString(), sb.GetLength());
											}
										}
										else if (reader->GetNodeText()->Equals(UTF8STRC("VERSION")))
										{
											SDEL_TEXT(version);
											sb.ClearStr();
											if (reader->ReadNodeText(&sb))
											{
												version = Text::StrCopyNewC(sb.ToString(), sb.GetLength());
											}
										}
										else if (reader->GetNodeText()->Equals(UTF8STRC("MANUFACTURER")))
										{
											SDEL_TEXT(manufacturer);
											sb.ClearStr();
											if (reader->ReadNodeText(&sb))
											{
												manufacturer = Text::StrCopyNewC(sb.ToString(), sb.GetLength());
											}
										}
										else if (reader->GetNodeText()->Equals(UTF8STRC("PRODUCTNAME")))
										{
											SDEL_TEXT(productName);
											sb.ClearStr();
											if (reader->ReadNodeText(&sb))
											{
												productName = Text::StrCopyNewC(sb.ToString(), sb.GetLength());
											}
										}
										else if (reader->GetNodeText()->Equals(UTF8STRC("GROUP")))
										{
											SDEL_TEXT(group);
											sb.ClearStr();
											if (reader->ReadNodeText(&sb))
											{
												group = Text::StrCopyNewC(sb.ToString(), sb.GetLength());
											}
										}
										else if (reader->GetNodeText()->Equals(UTF8STRC("ALTITUDE")))
										{
											sb.ClearStr();
											if (reader->ReadNodeText(&sb))
											{
												altitude = Text::StrToUInt32(sb.ToString());
											}
										}
										else
										{
											reader->SkipElement();
										}
									}
								}
								if (fileName)
								{
									sysInfo->AddDriverInfo(fileName, fileSize, createDate, version, manufacturer, productName, group, altitude);
								}
								SDEL_TEXT(fileName);
								SDEL_TEXT(createDate);
								SDEL_TEXT(version);
								SDEL_TEXT(manufacturer);
								SDEL_TEXT(productName);
								SDEL_TEXT(group);
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
		DEL_CLASS(reader);
		return sysInfo;
	}
	DEL_CLASS(reader);
	return 0;
}

Map::IMapDrawLayer *Parser::FileParser::XMLParser::ParseKMLContainer(Text::XMLReader *reader, Data::ICaseStringMap<KMLStyle*> *styles, Text::CString sourceName, Parser::ParserList *parsers, Net::WebBrowser *browser, IO::PackageFile *basePF)
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
		if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
		{
			break;
		}
		else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
		{
			if (reader->GetNodeText()->EqualsICase(UTF8STRC("NetworkLink")))
			{
				if (parsers && browser)
				{
					Map::ReloadableMapLayer *lyr;
					const UTF8Char *layerName = 0;
					const UTF8Char *url = 0;
					Int32 interval = 0;
					NEW_CLASS(lyr, Map::ReloadableMapLayer(sourceName, parsers, browser, CSTR_NULL));
				
					while (reader->ReadNext())
					{
						if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
						{
							break;
						}
						else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
						{
							if (reader->GetNodeText()->EqualsICase(UTF8STRC("NAME")))
							{
								sb.ClearStr();
								reader->ReadNodeText(&sb);
								SDEL_TEXT(layerName);
								layerName = Text::StrCopyNewC(sb.ToString(), sb.GetLength());
							}
							else if (reader->GetNodeText()->EqualsICase(UTF8STRC("LINK")))
							{
								while (reader->ReadNext())
								{
									if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
									{
										break;
									}
									else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
									{
										if (reader->GetNodeText()->EqualsICase(UTF8STRC("HREF")))
										{
											sb.ClearStr();
											reader->ReadNodeText(&sb);
											SDEL_TEXT(url);
											url = Text::StrCopyNewC(sb.ToString(), sb.GetLength());
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
					if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
					{
						break;
					}
					else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
					{
						if (reader->GetNodeText()->EqualsICase(UTF8STRC("LINESTYLE")))
						{
							while (reader->ReadNext())
							{
								if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
								{
									break;
								}
								else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
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
								if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
								{
									break;
								}
								else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
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
											if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
											{
												break;
											}
											else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && reader->GetNodeText()->EqualsICase(UTF8STRC("HREF")))
											{
												sb.ClearStr();
												reader->ReadNodeText(&sb);
												if (sb.GetLength() > 0)
												{
													SDEL_STRING(style->iconURL);
													style->iconURL = Text::String::New(sb.ToString(), sb.GetLength());
												}
											}
											else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
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
								if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
								{
									break;
								}
								else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && reader->GetNodeText()->EqualsICase(UTF8STRC("COLOR")))
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
					styleId->Release();
				}				
				if (style)
				{
					SDEL_STRING(style->iconURL);
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
					if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
					{
						break;
					}
					else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && reader->GetNodeText()->EqualsICase(UTF8STRC("PAIR")))
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
					styleId->Release();
				}
				if (style)
				{
					SDEL_STRING(style->iconURL);
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
					if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
					{
						break;
					}
					else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
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
								if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
								{
									break;
								}
								else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && reader->GetNodeText()->EqualsICase(UTF8STRC("HREF")))
								{
									sb.ClearStr();
									reader->ReadNodeText(&sb);
									imgLyr->GetSourceName(sbuff);
									sbuffEnd = Text::URLString::AppendURLPath(sbuff, sb.ToString(), sb.GetLength());
								}
								else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
								{
									reader->SkipElement();
								}
							}
						}
						else if (reader->GetNodeText()->EqualsICase(UTF8STRC("TIMESPAN")))
						{
							while (reader->ReadNext())
							{
								if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
								{
									break;
								}
								else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
								{
									if (reader->GetNodeText()->EqualsICase(UTF8STRC("BEGIN")))
									{
										sb.ClearStr();
										reader->ReadNodeText(&sb);
										dt.SetValue(sb.ToString(), sb.GetLength());
										timeStart = dt.ToUnixTimestamp();
									}
									else if (reader->GetNodeText()->EqualsICase(UTF8STRC("END")))
									{
										sb.ClearStr();
										reader->ReadNodeText(&sb);
										dt.SetValue(sb.ToString(), sb.GetLength());
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
					if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
					{
						break;
					}
					else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
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
								if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
								{
									break;
								}
								else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && reader->GetNodeText()->EqualsICase(UTF8STRC("HREF")))
								{
									sb.ClearStr();
									reader->ReadNodeText(&sb);
									imgLyr->GetSourceName(sbuff);
									sbuffEnd = Text::URLString::AppendURLPath(sbuff, sb.ToString(), sb.GetLength());
								}
								else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
								{
									reader->SkipElement();
								}
							}
						}
						else if (reader->GetNodeText()->EqualsICase(UTF8STRC("TIMESPAN")))
						{
							while (reader->ReadNext())
							{
								if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
								{
									break;
								}
								else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
								{
									if (reader->GetNodeText()->EqualsICase(UTF8STRC("BEGIN")))
									{
										sb.ClearStr();
										reader->ReadNodeText(&sb);
										dt.SetValue(sb.ToString(), sb.GetLength());
										timeStart = dt.ToUnixTimestamp();
									}
									else if (reader->GetNodeText()->EqualsICase(UTF8STRC("END")))
									{
										sb.ClearStr();
										reader->ReadNodeText(&sb);
										dt.SetValue(sb.ToString(), sb.GetLength());
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
								if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
								{
									break;
								}
								else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
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

void Parser::FileParser::XMLParser::ParseKMLPlacemarkTrack(Text::XMLReader *reader, Map::GPSTrack *lyr, Data::StringMap<KMLStyle*> *styles)
{
	Data::ArrayList<Text::String*> timeList;
	Data::ArrayList<Text::String*> coordList;
	Bool lastTrack = false;
	while (reader->ReadNext())
	{
		if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
		{
			break;
		}
		else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
		{
			if (reader->GetNodeText()->EqualsICase(UTF8STRC("GX:MULTITRACK")))
			{
				while (reader->ReadNext())
				{
					if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
					{
						break;
					}
					else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && reader->GetNodeText()->EqualsICase(UTF8STRC("GX:TRACK")))
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
										Map::GPSTrack::GPSRecord2 rec;
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
											dt.SetValue(s->v, s->leng); 
											rec.utcTimeTicks = dt.ToTicks();
											
											coordList.GetItem(i)->ConcatTo(sbuff);
											if (Text::StrSplit(strs, 4, sbuff, ' ') == 3)
											{
												rec.lon = Text::StrToDouble(strs[0]);
												rec.lat = Text::StrToDouble(strs[1]);
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
									Map::GPSTrack::GPSRecord2 *recs = lyr->GetTrack(0, &recCnt);
									while (reader->ReadNext())
									{
										if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
										{
											break;
										}
										else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && reader->GetNodeText()->EqualsICase(UTF8STRC("SCHEMADATA")))
										{
											while (reader->ReadNext())
											{
												if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
												{
													break;
												}
												else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && reader->GetNodeText()->EqualsICase(UTF8STRC("GX:SIMPLEARRAYDATA")))
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
																	if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
																	{
																		break;
																	}
																	else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && reader->GetNodeText()->EqualsICase(UTF8STRC("GX:VALUE")))
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
															else if (attr->value->EqualsICase(UTF8STRC("BEARING")))
															{
																j = 0;
																while (reader->ReadNext())
																{
																	if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
																	{
																		break;
																	}
																	else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && reader->GetNodeText()->EqualsICase(UTF8STRC("GX:VALUE")))
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
															else if (attr->value->EqualsICase(UTF8STRC("ACCURACY")))
															{
																j = 0;
																while (reader->ReadNext())
																{
																	if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
																	{
																		break;
																	}
																	else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && reader->GetNodeText()->EqualsICase(UTF8STRC("GX:VALUE")))
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
							Map::GPSTrack::GPSRecord2 rec;
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
								dt.SetValue(s->v, s->leng);
								rec.utcTimeTicks = dt.ToTicks();
								
								coordList.GetItem(i)->ConcatTo(sbuff);
								if (Text::StrSplit(strs, 4, sbuff, ' ') == 3)
								{
									rec.lon = Text::StrToDouble(strs[0]);
									rec.lat = Text::StrToDouble(strs[1]);
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
					else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
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
					if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
					{
						break;
					}
					else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
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
					Map::GPSTrack::GPSRecord2 rec;
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
						dt.SetValue(s->v, s->leng); 
						rec.utcTimeTicks = dt.ToTicks();
						
						coordList.GetItem(i)->ConcatTo(sbuff);
						if (Text::StrSplit(strs, 4, sbuff, ' ') == 3)
						{
							rec.lon = Text::StrToDouble(strs[0]);
							rec.lat = Text::StrToDouble(strs[1]);
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
			if (reader->GetNodeText()->EqualsICase(UTF8STRC("LINEARRING")))
			{
				ParseCoordinates(reader, coordList, altList);
			}
			else if (reader->GetNodeText()->EqualsICase(UTF8STRC("COORDINATES")))
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

Map::IMapDrawLayer *Parser::FileParser::XMLParser::ParseKMLPlacemarkLyr(Text::XMLReader *reader, Data::StringMap<KMLStyle*> *styles, Text::CString sourceName, Parser::ParserList *parsers, Net::WebBrowser *browser, IO::PackageFile *basePF)
{
	Text::StringBuilderUTF8 lyrNameSb;
	Text::StringBuilderUTF8 sb;
	lyrNameSb.AppendC(UTF8STRC("Layer"));
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
				DB::DBUtil::ColType colType = DB::DBUtil::CT_VarChar;
				UOSInt colSize = 256;
				UOSInt colDP = 0;
				NEW_CLASS(lyr, Map::VectorLayer(Map::DRAW_LAYER_POLYLINE3D, sourceName, 1, &cols, Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84), &colType, &colSize, &colDP, 0, lyrNameSb.ToCString()));

				while (reader->ReadNext())
				{
					if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
					{
						break;
					}
					else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && reader->GetNodeText()->EqualsICase(UTF8STRC("COORDINATES")))
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
							Text::StrConcatC(sbuff, sptr, (UOSInt)(sptr2 - sptr));
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
							UOSInt j;

							NEW_CLASS(pl, Math::Polyline3D(4326, 1, altList->GetCount()));
							pl->GetPtOfstList(&nPoints)[0] = 0;
							altArr = pl->GetAltitudeList(&nPoints);
							ptArr = pl->GetPointList(&nPoints);
							i = altList->GetCount();
							MemCopyNO(ptArr, coord->GetArray(&j), sizeof(Double) * 2 * i);
							MemCopyNO(altArr, altList->GetArray(&j), sizeof(Double) * i);
							lyr->AddVector(pl, &sb);
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
			else if (reader->GetNodeText()->EqualsICase(UTF8STRC("POINT")))
			{
				Map::VectorLayer *lyr;
				const UTF8Char *cols = (const UTF8Char*)"Name";
				DB::DBUtil::ColType colType = DB::DBUtil::CT_VarChar;
				UOSInt colSize = 256;
				UOSInt colDP = 0;
				NEW_CLASS(lyr, Map::VectorLayer(Map::DRAW_LAYER_POINT, sourceName, 1, &cols, Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84), &colType, &colSize, &colDP, 0, lyrNameSb.ToCString()));
				lyr->SetLabelVisible(true);
				while (reader->ReadNext())
				{
					if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
					{
						break;
					}
					else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && reader->GetNodeText()->EqualsICase(UTF8STRC("COORDINATES")))
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
							lyr->AddVector(pt, &sb);
						}
						else if (i == 2)
						{
							Math::Point3D *pt;
							x = Text::StrToDouble(sarr[0]);
							y = Text::StrToDouble(sarr[1]);
							NEW_CLASS(pt, Math::Point3D(4326, x, y, 0));
							lyr->AddVector(pt, &sb);
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
						fd = basePF->OpenStreamData(style->iconURL->v);
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
			else if (reader->GetNodeText()->EqualsICase(UTF8STRC("POLYGON")))
			{
				Map::VectorLayer *lyr;
				const UTF8Char *cols = (const UTF8Char*)"Name";
				DB::DBUtil::ColType colType = DB::DBUtil::CT_VarChar;
				UOSInt colSize = 256;
				UOSInt colDP = 0;
				Data::ArrayListDbl *coord;
				Data::ArrayListDbl *altList;
				NEW_CLASS(lyr, Map::VectorLayer(Map::DRAW_LAYER_POLYGON, sourceName, 1, &cols, Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84), &colType, &colSize, &colDP, 0, lyrNameSb.ToCString()));

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
						if (reader->GetNodeText()->EqualsICase(UTF8STRC("OUTERBOUNDARYIS")))
						{
							ParseCoordinates(reader, coord, 0);
						}
						else if (reader->GetNodeText()->EqualsICase(UTF8STRC("INNERBOUNDARYIS")))
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
						lyr->AddVector(pg, &sb);
					}
					else
					{
						NEW_CLASS(pg, Math::Polygon(4326, 1, coord->GetCount() >> 1));
						ptList = pg->GetPtOfstList(&nPoints);
						ptList[0] = 0;
						ptArr = pg->GetPointList(&nPoints);
						MemCopyNO(ptArr, coord->GetArray(&i), sizeof(Double) * coord->GetCount());
						lyr->AddVector(pg, &sb);
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

Bool Parser::FileParser::XMLParser::ParseGPXPoint(Text::XMLReader *reader, Map::GPSTrack::GPSRecord2 *rec)
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
		if (attr->name->EqualsICase(UTF8STRC("LAT")))
		{
			rec->lat = attr->value->ToDouble();
		}
		else if (attr->name->EqualsICase(UTF8STRC("LON")))
		{
			rec->lon = attr->value->ToDouble();
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
			if (reader->GetNodeText()->EqualsICase(UTF8STRC("ELE")))
			{
				reader->ReadNodeText(&sb);
				rec->altitude = Text::StrToDouble(sb.ToString());
				sb.ClearStr();
			}
			else if (reader->GetNodeText()->EqualsICase(UTF8STRC("TIME")))
			{
				reader->ReadNodeText(&sb);
				Data::DateTime dt;
				dt.SetValue(sb.ToString(), sb.GetLength());
				sb.ClearStr();
				rec->utcTimeTicks = dt.ToTicks();
			}
			else if (reader->GetNodeText()->EqualsICase(UTF8STRC("SPEED")))
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
	rec->nSateUsed = 0;
	rec->nSateUsedGPS = 0;
	rec->nSateUsedGLO = 0;
	rec->nSateUsedSBAS = 0;
	rec->nSateViewGPS = 0;
	rec->nSateViewGLO = 0;
	rec->nSateViewGA = 0;
	rec->nSateViewQZSS = 0;
	rec->nSateViewBD = 0;
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
			if (reader->GetNodeText()->Equals(UTF8STRC("Filter")))
			{
				found = false;
				i = reader->GetAttribCount();
				while (i-- > 0)
				{
					attr = reader->GetAttrib(i);
					if (attr->name->Equals(UTF8STRC("Name")))
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
			else if (reader->GetNodeText()->Equals(UTF8STRC("File")))
			{
				i = reader->GetAttribCount();
				while (i-- > 0)
				{
					attr = reader->GetAttrib(i);
					if (attr->name->Equals(UTF8STRC("RelativePath")))
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
	Text::String *cfgName;
	while (reader->ReadNext())
	{
		if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
		{
			return true;
		}
		else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
		{
			if (reader->GetNodeText()->Equals(UTF8STRC("Configuration")))
			{
				cfg = 0;
				cfgName = 0;
				i = reader->GetAttribCount();
				while (i-- > 0)
				{
					attr = reader->GetAttrib(i);
					if (attr->name->Equals(UTF8STRC("Name")))
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
							if (reader->GetNodeText()->Equals(UTF8STRC("Tool")))
							{
								i = reader->GetAttribCount();
								while (i-- > 0)
								{
									attr = reader->GetAttrib(i);
									if (attr->name->Equals(UTF8STRC("Name")))
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
