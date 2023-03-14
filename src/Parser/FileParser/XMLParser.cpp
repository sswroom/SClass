#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/TextDB.h"
#include "IO/PackageFile.h"
#include "IO/Path.h"
#include "IO/StreamDataStream.h"
#include "IO/SystemInfoLog.h"
#include "Map/GMLXML.h"
#include "Map/KMLXML.h"
#include "Map/OruxDBLayer.h"
#include "Map/OSM/OSMParser.h"
#include "Math/Math.h"
#include "Media/StaticImage.h"
#include "Media/Jasper/JasperXML.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
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

void Parser::FileParser::XMLParser::PrepareSelector(IO::FileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MapLayer)
	{
		selector->AddFilter(CSTR("*.xml"), CSTR("XML File"));
		selector->AddFilter(CSTR("*.gpx"), CSTR("GPS Track File"));
		selector->AddFilter(CSTR("*.kml"), CSTR("KML File"));
		selector->AddFilter(CSTR("*.osm"), CSTR("OpenStreetMap File"));
		selector->AddFilter(CSTR("*.vcproj"), CSTR("VC Project File"));
		selector->AddFilter(CSTR("*.gml"), CSTR("GML File"));
		selector->AddFilter(CSTR("*.jrxml"), CSTR("Jasper Report File"));
	}
}

IO::ParserType Parser::FileParser::XMLParser::GetParserType()
{
	return IO::ParserType::MapLayer;
}

IO::ParsedObject *Parser::FileParser::XMLParser::ParseFileHdr(IO::StreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
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
	else if (Text::StrEqualsICaseC(&sbuff[i], (UOSInt)(sptr - &sbuff[i]), UTF8STRC(".JRXML")))
	{
		valid = true;
	}
	if (!valid)
	{
		return 0;
	}

	IO::ParsedObject *pobj;
	IO::StreamDataStream stm(fd);
	pobj = ParseStream(this->encFact, &stm, fd->GetFullName()->ToCString(), this->parsers, this->browser, pkgFile);
	return pobj;
}

IO::ParsedObject *Parser::FileParser::XMLParser::ParseStream(Text::EncodingFactory *encFact, IO::Stream *stm, Text::CString fileName, Parser::ParserList *parsers, Net::WebBrowser *browser, IO::PackageFile *pkgFile)
{
	Text::String *nodeText;
	Text::XMLReader reader(encFact, stm, Text::XMLReader::PM_XML);

	if (!reader.NextElement())
		return 0;
	nodeText = reader.GetNodeText();
	if (nodeText->Equals(UTF8STRC("kml")))
	{
		return Map::KMLXML::ParseKMLRoot(&reader, fileName, parsers, browser, pkgFile);
	}
	else if (nodeText->Equals(UTF8STRC("gpx")))
	{
		UOSInt i;
		UOSInt j;
		const UTF8Char *shortName;
		i = fileName.LastIndexOf(IO::Path::PATH_SEPERATOR);
		if (IO::Path::PATH_SEPERATOR == '\\')
		{
			j = fileName.LastIndexOf('/');
			if (i == INVALID_INDEX || (j != INVALID_INDEX && j > i))
			{
				i = j;
			}
		}
		shortName = &fileName.v[i + 1];
		while (reader.ReadNext())
		{
			if (reader.GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
			{
				break;
			}
			else if (reader.GetNodeType() == Text::XMLNode::NodeType::Element)
			{
				if (reader.GetNodeText()->Equals(UTF8STRC("trk"))) // /gpx/trk/trkseg
				{
					Map::GPSTrack *track;
					NEW_CLASS(track, Map::GPSTrack(fileName, true, 0, CSTR_NULL));
					track->SetTrackName({shortName, (UOSInt)(fileName.v + fileName.leng - shortName)});
					while (reader.ReadNext())
					{
						if (reader.GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
						{
							break;
						}
						else if (reader.GetNodeType() == Text::XMLNode::NodeType::Element)
						{
							if (reader.GetNodeText()->Equals(UTF8STRC("trkseg")))
							{
								track->NewTrack();
								while (reader.ReadNext())
								{
									if (reader.GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
									{
										break;
									}
									else if (reader.GetNodeType() == Text::XMLNode::NodeType::Element)
									{
										if (reader.GetNodeText()->EqualsICase(UTF8STRC("TRKPT")))
										{
											Map::GPSTrack::GPSRecord3 rec;
											if (ParseGPXPoint(&reader, &rec))
											{
												track->AddRecord(&rec);
											}
										}
										else
										{
											reader.SkipElement();
										}
									}
								}
							}
							else
							{
								if (!reader.SkipElement())
								{
									break;
								}
							}
						}
					}
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
							if (node1->GetNodeType() == Text::XMLNode::NodeType::Element)
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
					if (!reader.SkipElement())
					{
						return 0;
					}
				}
			}
		}
		return 0;
	}
	else if (nodeText->Equals(UTF8STRC("osm")))
	{
		Map::MapDrawLayer *lyr = Map::OSM::OSMParser::ParseLayerNode(&reader, fileName);
		if (lyr == 0)
		{
			return 0;
		}
		while (reader.ReadNext())
		{
			if (reader.GetNodeType() == Text::XMLNode::NodeType::Element)
			{
				DEL_CLASS(lyr)
				lyr = 0;
				break;
			}
		}
		if (!reader.IsComplete())
		{
			SDEL_CLASS(lyr);
		}
		return lyr;
	}
	else if (nodeText->Equals(UTF8STRC("OruxTracker")))
	{
		Map::OruxDBLayer *lyr = 0;
		while (reader.ReadNext())
		{
			if (reader.GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
			{
				break;
			}
			else if (reader.GetNodeType() == Text::XMLNode::NodeType::Element && reader.GetNodeText()->Equals(UTF8STRC("MapCalibration")))
			{
				while (reader.ReadNext())
				{
					if (reader.GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
					{
						break;
					}
					else if (reader.GetNodeType() == Text::XMLNode::NodeType::Element)
					{
						if (reader.GetNodeText()->Equals(UTF8STRC("OruxTracker")))
						{
							if (lyr)
							{
								while (reader.ReadNext())
								{
									if (reader.GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
									{
										break;
									}
									else if (reader.GetNodeType() == Text::XMLNode::NodeType::Element && reader.GetNodeText()->Equals(UTF8STRC("MapCalibration")))
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
										i = reader.GetAttribCount();
										while (i-- > 0)
										{
											attr = reader.GetAttrib(i);
											if (attr->name->Equals(UTF8STRC("layerLevel")))
											{
												if (attr->value->ToUInt32(&layerId))
												{
													flags |= 1;
													break;
												}
											}
										}

										while (reader.ReadNext())
										{
											if (reader.GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
											{
												break;
											}
											else if (reader.GetNodeType() == Text::XMLNode::NodeType::Element)
											{
												if (reader.GetNodeText()->Equals(UTF8STRC("MapChunks")))
												{
													i = reader.GetAttribCount();
													while (i-- > 0)
													{
														attr = reader.GetAttrib(i);
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
													reader.SkipElement();
												}
												else if (reader.GetNodeText()->Equals(UTF8STRC("MapBounds")))
												{
													i = reader.GetAttribCount();
													while (i-- > 0)
													{
														attr = reader.GetAttrib(i);
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
													reader.SkipElement();
												}
												else
												{
													reader.SkipElement();
												}
											}
										}

										if (flags == 255)
										{
											lyr->AddLayer(layerId, mapXMin, mapYMin, mapXMax, mapYMax, maxX, maxY, tileSize);
										}
									}
									else if (reader.GetNodeType() == Text::XMLNode::NodeType::Element)
									{
										reader.SkipElement();
									}
								}
							}
							else
							{
								reader.SkipElement();
							}
						}
						else if (reader.GetNodeText()->Equals(UTF8STRC("MapName")))
						{
							if (lyr == 0)
							{
								Text::StringBuilderUTF8 sb;
								reader.ReadNodeText(&sb);
								NEW_CLASS(lyr, Map::OruxDBLayer(fileName, sb.ToCString(), parsers));
								if (lyr->IsError())
								{
									DEL_CLASS(lyr);
									lyr = 0;
								}
							}
							else
							{
								reader.SkipElement();
							}
						}
						else
						{
							reader.SkipElement();
						}
					}
				}

				break;
			}
			else if (reader.GetNodeType() == Text::XMLNode::NodeType::Element)
			{
				reader.SkipElement();
			}
		}
		return lyr;
	}
	else if (nodeText->Equals(UTF8STRC("VisualStudioProject")))
	{
		Text::XMLAttrib *attr;
		Text::VSProject *proj = 0;
		Text::VSProject::VisualStudioVersion ver = Text::VSProject::VSV_UNKNOWN;
		Text::String *projName = 0;
		UOSInt i;
		i = reader.GetAttribCount();
		while (i-- > 0)
		{
			attr = reader.GetAttrib(i);
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
			while (reader.ReadNext())
			{
				if (reader.GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
				{
					break;
				}
				else if (reader.GetNodeType() == Text::XMLNode::NodeType::Element)
				{
					if (reader.GetNodeText()->Equals(UTF8STRC("Configurations")))
					{
						if (!ParseVSConfFile(&reader, proj))
						{
							break;
						}
					}
					else if (reader.GetNodeText()->Equals(UTF8STRC("Files")))
					{
						if (!ParseVSProjFile(&reader, proj))
						{
							break;
						}
					}
					else
					{
						if (!reader.SkipElement())
						{
							break;
						}
					}
				}
			}
		}
		return proj;
	}
	else if (nodeText->EndsWith(UTF8STRC(":FeatureCollection")))
	{
		return Map::GMLXML::ParseFeatureCollection(&reader, fileName);
	}
	else if (nodeText->Equals(UTF8STRC("fme:xml-tables")))
	{
		UOSInt i;
		UOSInt j;
		const UTF8Char *shortName;
		i = fileName.LastIndexOf(IO::Path::PATH_SEPERATOR);
		if (IO::Path::PATH_SEPERATOR == '\\')
		{
			j = fileName.LastIndexOf('/');
			if (i == INVALID_INDEX || (j != INVALID_INDEX && j > i))
			{
				i = j;
			}
		}
		shortName = &fileName.v[i + 1];
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

		while (reader.ReadNext())
		{
			if (reader.GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
			{
				break;
			}
			else if (reader.GetNodeType() == Text::XMLNode::NodeType::Element)
			{
				if (reader.GetNodeText()->StartsWith(UTF8STRC("fme:")) && Text::StrStartsWith(reader.GetNodeText()->v + 4, sbTableName.ToString()) && Text::StrEquals(reader.GetNodeText()->v + 4 + sbTableName.GetLength(), (const UTF8Char*)"-table"))
				{
					UOSInt i;
					UOSInt j;
					DB::TextDB *db = 0;
					Text::StringBuilderUTF8 sb;
					Data::ArrayList<const UTF8Char *> colList;
					Data::ArrayList<const UTF8Char *> nameList;
					Data::ArrayList<const UTF8Char *> valList;
					Bool succ = false;
					while (reader.ReadNext())
					{
						if (reader.GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
						{
							succ = true;
							break;
						}
						else if (reader.GetNodeType() == Text::XMLNode::NodeType::Element)
						{
							if (reader.GetNodeText()->StartsWith(UTF8STRC("fme:")) && Text::StrEquals(reader.GetNodeText()->v + 4, sbTableName.ToString()))
							{
								while (reader.ReadNext())
								{
									if (reader.GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
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
												db->AddTable(sbTableName.ToCString(), &colList);
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
									else if (reader.GetNodeType() == Text::XMLNode::NodeType::Element)
									{
										if (reader.GetNodeText()->StartsWith(UTF8STRC("fme:")))
										{
											Text::String *txt = reader.GetNodeText();
											nameList.Add(Text::StrCopyNewC(txt->v + 4, txt->leng - 4));
											sb.ClearStr();
											reader.ReadNodeText(&sb);
											valList.Add(Text::StrCopyNewC(sb.ToString(), sb.GetLength()));
										}
										else
										{
											reader.SkipElement();
										}
									}
								}
							}
							else
							{
								reader.SkipElement();
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
					return db;
				}
				else
				{
					reader.SkipElement();
				}
			}
		}
	}
	else if (nodeText->Equals(UTF8STRC("SYSTEMINFO")))
	{
		Text::StringBuilderUTF8 sb;
		IO::SystemInfoLog *sysInfo;
		NEW_CLASS(sysInfo, IO::SystemInfoLog(fileName));
		while (reader.ReadNext())
		{
			if (reader.GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
			{
				break;
			}
			else if (reader.GetNodeType() == Text::XMLNode::NodeType::Element)
			{
				if (reader.GetNodeText()->Equals(UTF8STRC("SYSTEM")))
				{
					while (reader.ReadNext())
					{
						if (reader.GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
						{
							break;
						}
						else if (reader.GetNodeType() == Text::XMLNode::NodeType::Element)
						{
							if (reader.GetNodeText()->Equals(UTF8STRC("OSNAME")))
							{
								sb.ClearStr();
								if (reader.ReadNodeText(&sb))
								{
									sysInfo->SetOSName(sb.ToCString());
								}
							}
							else if (reader.GetNodeText()->Equals(UTF8STRC("OSVER")))
							{
								sb.ClearStr();
								if (reader.ReadNodeText(&sb))
								{
									sysInfo->SetOSVer(sb.ToCString());
								}
							}
							else if (reader.GetNodeText()->Equals(UTF8STRC("OSLANGUAGE")))
							{
								sb.ClearStr();
								if (reader.ReadNodeText(&sb))
								{
									sysInfo->SetOSLocale(Text::StrToUInt32(sb.ToString()));
								}
							}
							else if (reader.GetNodeText()->Equals(UTF8STRC("ARCHITECTURE")))
							{
								sb.ClearStr();
								if (reader.ReadNodeText(&sb))
								{
									sysInfo->SetArchitecture(Text::StrToUInt32(sb.ToString()));
								}
							}
							else if (reader.GetNodeText()->Equals(UTF8STRC("PRODUCTTYPE")))
							{
								sb.ClearStr();
								if (reader.ReadNodeText(&sb))
								{
									sysInfo->SetProductType(Text::StrToUInt32(sb.ToString()));
								}
							}
						}
					}
				}
				else if (reader.GetNodeText()->Equals(UTF8STRC("SERVERROLES")))
				{
					while (reader.ReadNext())
					{
						if (reader.GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
						{
							break;
						}
						else if (reader.GetNodeType() == Text::XMLNode::NodeType::Element)
						{
							if (reader.GetNodeText()->Equals(UTF8STRC("REG_VALUE")))
							{
								const UTF8Char *roleName = 0;
								const UTF8Char *roleData = 0;
								while (reader.ReadNext())
								{
									if (reader.GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
									{
										break;
									}
									else if (reader.GetNodeType() == Text::XMLNode::NodeType::Element)
									{
										if (reader.GetNodeText()->Equals(UTF8STRC("NAME")))
										{
											SDEL_TEXT(roleName);
											sb.ClearStr();
											if (reader.ReadNodeText(&sb))
											{
												roleName = Text::StrCopyNewC(sb.ToString(), sb.GetLength());
											}
										}
										else if (reader.GetNodeText()->Equals(UTF8STRC("DATA")))
										{
											SDEL_TEXT(roleData);
											sb.ClearStr();
											if (reader.ReadNodeText(&sb))
											{
												roleData = Text::StrCopyNewC(sb.ToString(), sb.GetLength());
											}
										}
										else
										{
											reader.SkipElement();
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
								reader.SkipElement();
							}
						}
					}
				}
				else if (reader.GetNodeText()->Equals(UTF8STRC("DEVICES")))
				{
					while (reader.ReadNext())
					{
						if (reader.GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
						{
							break;
						}
						else if (reader.GetNodeType() == Text::XMLNode::NodeType::Element)
						{
							if (reader.GetNodeText()->Equals(UTF8STRC("DEVICE")))
							{
								const UTF8Char *desc = 0;
								const UTF8Char *hwId = 0;
								const UTF8Char *service = 0;
								const UTF8Char *driver = 0;
								while (reader.ReadNext())
								{
									if (reader.GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
									{
										break;
									}
									else if (reader.GetNodeType() == Text::XMLNode::NodeType::Element)
									{
										if (reader.GetNodeText()->Equals(UTF8STRC("DESCRIPTION")))
										{
											SDEL_TEXT(desc);
											sb.ClearStr();
											if (reader.ReadNodeText(&sb))
											{
												desc = Text::StrCopyNewC(sb.ToString(), sb.GetLength());
											}
										}
										else if (reader.GetNodeText()->Equals(UTF8STRC("HARDWAREID")))
										{
											SDEL_TEXT(hwId);
											sb.ClearStr();
											if (reader.ReadNodeText(&sb))
											{
												hwId = Text::StrCopyNewC(sb.ToString(), sb.GetLength());
											}
										}
										else if (reader.GetNodeText()->Equals(UTF8STRC("SERVICE")))
										{
											SDEL_TEXT(service);
											sb.ClearStr();
											if (reader.ReadNodeText(&sb))
											{
												service = Text::StrCopyNewC(sb.ToString(), sb.GetLength());
											}
										}
										else if (reader.GetNodeText()->Equals(UTF8STRC("DRIVER")))
										{
											SDEL_TEXT(driver);
											sb.ClearStr();
											if (reader.ReadNodeText(&sb))
											{
												driver = Text::StrCopyNewC(sb.ToString(), sb.GetLength());
											}
										}
										else
										{
											reader.SkipElement();
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
								reader.SkipElement();
							}
						}
					}
				}
				else if (reader.GetNodeText()->Equals(UTF8STRC("DRIVERS")))
				{
					while (reader.ReadNext())
					{
						if (reader.GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
						{
							break;
						}
						else if (reader.GetNodeType() == Text::XMLNode::NodeType::Element)
						{
							if (reader.GetNodeText()->Equals(UTF8STRC("DRIVER")))
							{
								const UTF8Char *fileName = 0;
								UInt64 fileSize = 0;
								const UTF8Char *createDate = 0;
								const UTF8Char *version = 0;
								const UTF8Char *manufacturer = 0;
								const UTF8Char *productName = 0;
								const UTF8Char *group = 0;
								UInt32 altitude = 0;
								while (reader.ReadNext())
								{
									if (reader.GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
									{
										break;
									}
									else if (reader.GetNodeType() == Text::XMLNode::NodeType::Element)
									{
										if (reader.GetNodeText()->Equals(UTF8STRC("FILENAME")))
										{
											SDEL_TEXT(fileName);
											sb.ClearStr();
											if (reader.ReadNodeText(&sb))
											{
												fileName = Text::StrCopyNewC(sb.ToString(), sb.GetLength());
											}
										}
										else if (reader.GetNodeText()->Equals(UTF8STRC("FILESIZE")))
										{
											sb.ClearStr();
											if (reader.ReadNodeText(&sb))
											{
												fileSize = Text::StrToUInt64(sb.ToString());
											}
										}
										else if (reader.GetNodeText()->Equals(UTF8STRC("CREATIONDATE")))
										{
											SDEL_TEXT(createDate);
											sb.ClearStr();
											if (reader.ReadNodeText(&sb))
											{
												createDate = Text::StrCopyNewC(sb.ToString(), sb.GetLength());
											}
										}
										else if (reader.GetNodeText()->Equals(UTF8STRC("VERSION")))
										{
											SDEL_TEXT(version);
											sb.ClearStr();
											if (reader.ReadNodeText(&sb))
											{
												version = Text::StrCopyNewC(sb.ToString(), sb.GetLength());
											}
										}
										else if (reader.GetNodeText()->Equals(UTF8STRC("MANUFACTURER")))
										{
											SDEL_TEXT(manufacturer);
											sb.ClearStr();
											if (reader.ReadNodeText(&sb))
											{
												manufacturer = Text::StrCopyNewC(sb.ToString(), sb.GetLength());
											}
										}
										else if (reader.GetNodeText()->Equals(UTF8STRC("PRODUCTNAME")))
										{
											SDEL_TEXT(productName);
											sb.ClearStr();
											if (reader.ReadNodeText(&sb))
											{
												productName = Text::StrCopyNewC(sb.ToString(), sb.GetLength());
											}
										}
										else if (reader.GetNodeText()->Equals(UTF8STRC("GROUP")))
										{
											SDEL_TEXT(group);
											sb.ClearStr();
											if (reader.ReadNodeText(&sb))
											{
												group = Text::StrCopyNewC(sb.ToString(), sb.GetLength());
											}
										}
										else if (reader.GetNodeText()->Equals(UTF8STRC("ALTITUDE")))
										{
											sb.ClearStr();
											if (reader.ReadNodeText(&sb))
											{
												altitude = Text::StrToUInt32(sb.ToString());
											}
										}
										else
										{
											reader.SkipElement();
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
								reader.SkipElement();
							}
						}
					}
				}
				else
				{
					reader.SkipElement();
				}
			}
		}
		return sysInfo;
	}
	else if (nodeText->Equals(UTF8STRC("jasperReport")))
	{
		return Media::Jasper::JasperXML::ParseJasperReport(&reader, fileName);
	}
	return 0;
}

Bool Parser::FileParser::XMLParser::ParseGPXPoint(Text::XMLReader *reader, Map::GPSTrack::GPSRecord3 *rec)
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
			rec->pos.lat = attr->value->ToDouble();
		}
		else if (attr->name->EqualsICase(UTF8STRC("LON")))
		{
			rec->pos.lon = attr->value->ToDouble();
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
		if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
		{
			break;
		}
		else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
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
				dt.SetValue(sb.ToCString());
				sb.ClearStr();
				rec->recTime = dt.ToInstant();
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
		if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
		{
			return true;
		}
		else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
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
		if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
		{
			return true;
		}
		else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
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
						if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
						{
							break;
						}
						else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
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
