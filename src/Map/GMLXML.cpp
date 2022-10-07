#include "Stdafx.h"
#include "Data/ArrayListDbl.h"
#include "Map/GMLXML.h"
#include "Map/VectorLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Geometry/LineString.h"
#include "Math/Geometry/PointZ.h"

Map::IMapDrawLayer *Map::GMLXML::ParseFeatureCollection(Text::XMLReader *reader, Text::CString fileName)
{
	if (reader->GetNodeType() != Text::XMLNode::NodeType::Element || !reader->GetNodeText()->EndsWith(UTF8STRC(":FeatureCollection")))
		return 0;
	
	Text::XMLNode::NodeType nodeType;
	Text::String *nodeText;
	Math::CoordinateSystem *csys = 0;
	UInt32 srid = 0;
	UOSInt colCnt = 0;
	Data::ArrayList<const UTF8Char *> nameList;
	Data::ArrayList<Text::String *> valList;
	Text::StringBuilderUTF8 sb;
	Map::VectorLayer *lyr = 0;
	Map::DrawLayerType layerType = Map::DRAW_LAYER_UNKNOWN;
	const UTF8Char **ccols;
	UOSInt i;
	Text::XMLAttrib *attr;
	UTF8Char *sarr[4];
	UTF8Char *sarr2[3];
	while (reader->ReadNext())
	{
		if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
		{
			break;
		}
		else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
		{
			if (reader->GetNodeText()->Equals(UTF8STRC("gml:featureMember")) || reader->GetNodeText()->Equals(UTF8STRC("gml:featureMembers")))
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
						Math::Geometry::Vector2D *vec = 0;

						while (reader->ReadNext())
						{
							nodeType = reader->GetNodeType();
							if (nodeType == Text::XMLNode::NodeType::ElementEnd)
							{
								break;
							}
							else if (nodeType == Text::XMLNode::NodeType::Element)
							{
								nodeText = reader->GetNodeText();
								if (nodeText->Equals(UTF8STRC("gml:pointProperty")))
								{
									if (layerType == Map::DRAW_LAYER_UNKNOWN || layerType == Map::DRAW_LAYER_POINT3D)
									{
										layerType = Map::DRAW_LAYER_POINT3D;
										while (reader->ReadNext())
										{
											if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
											{
												break;
											}
											else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
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
													Data::ArrayListDbl xPts;
													Data::ArrayListDbl yPts;
													Data::ArrayListDbl zPts;
													Math::Geometry::PointZ *pt;
													while (reader->ReadNext())
													{
														if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
														{
															break;
														}
														else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
														{
															if (reader->GetNodeText()->Equals(UTF8STRC("gml:pos")))
															{
																sb.ClearStr();
																reader->ReadNodeText(&sb);
																sarr[3] = sb.v;
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
																	NEW_CLASS(pt, Math::Geometry::PointZ(srid, xPts.GetItem(0), yPts.GetItem(0), zPts.GetItem(0)));
																	SDEL_CLASS(vec);
																	vec = pt;
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
										}
									}
									else
									{
										reader->SkipElement();
									}
								}
								else if (nodeText->Equals(UTF8STRC("gml:surfaceProperty")))
								{
									if (layerType == Map::DRAW_LAYER_UNKNOWN || layerType == Map::DRAW_LAYER_POLYGON)
									{
										layerType = Map::DRAW_LAYER_POLYGON;
										while (reader->ReadNext())
										{
											if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
											{
												break;
											}
											else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
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
													Data::ArrayListDbl xPts;
													Data::ArrayListDbl yPts;
													Data::ArrayListDbl zPts;
													Math::Geometry::Polygon *pg;
													Math::Coord2DDbl *ptList;
													while (reader->ReadNext())
													{
														if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
														{
															break;
														}
														else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element && reader->GetNodeText()->Equals(UTF8STRC("gml:patches")))
														{
															while (reader->ReadNext())
															{
																if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
																{
																	break;
																}
																else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element && reader->GetNodeText()->Equals(UTF8STRC("gml:PolygonPatch")))
																{
																	while (reader->ReadNext())
																	{
																		if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
																		{
																			break;
																		}
																		else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element && reader->GetNodeText()->Equals(UTF8STRC("gml:exterior")))
																		{
																			while (reader->ReadNext())
																			{
																				if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
																				{
																					break;
																				}
																				else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element && reader->GetNodeText()->Equals(UTF8STRC("gml:LinearRing")))
																				{
																					while (reader->ReadNext())
																					{
																						if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
																						{
																							break;
																						}
																						else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element && reader->GetNodeText()->Equals(UTF8STRC("gml:posList")))
																						{
																							sb.ClearStr();
																							reader->ReadNodeText(&sb);
																							sarr[3] = sb.v;
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
																								NEW_CLASS(pg, Math::Geometry::Polygon(srid, 1, xPts.GetCount(), false, false));
																								ptList = pg->GetPointList(&i);
																								while (i-- > 0)
																								{
																									ptList[i].x = xPts.GetItem(i);
																									ptList[i].y = yPts.GetItem(i);
																								}
																								SDEL_CLASS(vec);
																								vec = pg;
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
									}
								}
								else if (nodeText->Equals(UTF8STRC("gml:curveProperty")))
								{
									if (layerType == Map::DRAW_LAYER_UNKNOWN || layerType == Map::DRAW_LAYER_POLYLINE3D)
									{
										layerType = Map::DRAW_LAYER_POLYLINE3D;
										while (reader->ReadNext())
										{
											if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
											{
												break;
											}
											else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
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
													Data::ArrayListDbl xPts;
													Data::ArrayListDbl yPts;
													Data::ArrayListDbl zPts;
													Math::Geometry::LineString *pl;
													Math::Coord2DDbl *ptList;
													Double *hList;
													while (reader->ReadNext())
													{
														if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
														{
															break;
														}
														else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
														{
															if (reader->GetNodeText()->Equals(UTF8STRC("gml:posList")))
															{
																sb.ClearStr();
																reader->ReadNodeText(&sb);
																sarr[3] = sb.v;
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
																	NEW_CLASS(pl, Math::Geometry::LineString(srid, xPts.GetCount(), true, false));
																	ptList = pl->GetPointList(&i);
																	hList = pl->GetZList(&i);
																	while (i-- > 0)
																	{
																		hList[i] = zPts.GetItem(i);
																		ptList[i].x = xPts.GetItem(i);
																		ptList[i].y = yPts.GetItem(i);
																	}
																	SDEL_CLASS(vec);
																	vec = pl;
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
										}
									}
								}
								else if (nodeText->EndsWith(UTF8STRC(":geometryProperty")) || nodeText->EndsWith(UTF8STRC(":geom")))
								{
									if (layerType == Map::DRAW_LAYER_UNKNOWN || layerType == Map::DRAW_LAYER_POLYGON)
									{
										layerType = Map::DRAW_LAYER_POLYGON;
										while (reader->ReadNext())
										{
											if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
											{
												break;
											}
											else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
											{
												if (reader->GetNodeText()->Equals(UTF8STRC("gml:MultiPolygon")))
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
													Data::ArrayListDbl xPts;
													Data::ArrayListDbl yPts;
													Data::ArrayListDbl zPts;
													Math::Geometry::Polygon *pg;
													Math::Coord2DDbl *ptList;
													while (reader->ReadNext())
													{
														nodeType = reader->GetNodeType();
														if (nodeType == Text::XMLNode::NodeType::ElementEnd)
														{
															break;
														}
														else if (nodeType == Text::XMLNode::NodeType::Element && reader->GetNodeText()->Equals(UTF8STRC("gml:polygonMember")))
														{
															while (reader->ReadNext())
															{
																nodeType = reader->GetNodeType();
																if (nodeType == Text::XMLNode::NodeType::ElementEnd)
																{
																	break;
																}
																else if (nodeType == Text::XMLNode::NodeType::Element && reader->GetNodeText()->Equals(UTF8STRC("gml:Polygon")))
																{
																	while (reader->ReadNext())
																	{
																		nodeType = reader->GetNodeType();
																		if (nodeType == Text::XMLNode::NodeType::ElementEnd)
																		{
																			break;
																		}
																		else if (nodeType == Text::XMLNode::NodeType::Element && reader->GetNodeText()->Equals(UTF8STRC("gml:outerBoundaryIs")))
																		{
																			while (reader->ReadNext())
																			{
																				nodeType = reader->GetNodeType();
																				if (nodeType == Text::XMLNode::NodeType::ElementEnd)
																				{
																					break;
																				}
																				else if (nodeType == Text::XMLNode::NodeType::Element && reader->GetNodeText()->Equals(UTF8STRC("gml:LinearRing")))
																				{
																					while (reader->ReadNext())
																					{
																						nodeType = reader->GetNodeType();
																						if (nodeType == Text::XMLNode::NodeType::ElementEnd)
																						{
																							break;
																						}
																						else if (nodeType == Text::XMLNode::NodeType::Element && reader->GetNodeText()->Equals(UTF8STRC("gml:coordinates")))
																						{
																							sb.ClearStr();
																							reader->ReadNodeText(&sb);
																							sarr[1] = sb.v;
																							while (true)
																							{
																								i = Text::StrSplit(sarr, 2, sarr[1], ' ');
																								if (Text::StrSplit(sarr2, 3, sarr[0], ',') == 2)
																								{
																									xPts.Add(Text::StrToDouble(sarr2[0]));
																									yPts.Add(Text::StrToDouble(sarr2[1]));
																								}
																								if (i != 2)
																									break;
																							}

																							if (xPts.GetCount() > 0)
																							{
																								NEW_CLASS(pg, Math::Geometry::Polygon(srid, 1, xPts.GetCount(), false, false));
																								ptList = pg->GetPointList(&i);
																								while (i-- > 0)
																								{
																									ptList[i].x = xPts.GetItem(i);
																									ptList[i].y = yPts.GetItem(i);
																								}
																								SDEL_CLASS(vec);
																								vec = pg;
																							}
																						}
																						else if (nodeType == Text::XMLNode::NodeType::Element)
																						{
																							reader->SkipElement();
																						}
																					}
																				}
																				else if (nodeType == Text::XMLNode::NodeType::Element)
																				{
																					reader->SkipElement();
																				}
																			}
																		}
																		else if (nodeType == Text::XMLNode::NodeType::Element)
																		{
																			reader->SkipElement();
																		}
																	}
																}
																else if (nodeType == Text::XMLNode::NodeType::Element)
																{
																	reader->SkipElement();
																}
															}
														}
														else if (nodeType == Text::XMLNode::NodeType::Element)
														{
															reader->SkipElement();
														}
													}
												}
												else if (reader->GetNodeText()->Equals(UTF8STRC("gml:Polygon")))
												{
													UOSInt dimension = 0;
													i = reader->GetAttribCount();
													while (i-- > 0)
													{
														attr = reader->GetAttrib(i);
														if (attr->name->Equals(UTF8STRC("srsName")) && csys == 0)
														{
															csys = Math::CoordinateSystemManager::CreateFromName(attr->value->ToCString());
															if (csys)
															{
																srid = csys->GetSRID();
															}
														}
														else if (attr->name->Equals(UTF8STRC("srsDimension")))
														{
															dimension = attr->value->ToUOSInt();
														}
													}
													Data::ArrayListDbl xPts;
													Data::ArrayListDbl yPts;
													Data::ArrayListDbl zPts;
													Math::Geometry::Polygon *pg;
													Math::Coord2DDbl *ptList;
													while (reader->ReadNext())
													{
														nodeType = reader->GetNodeType();
														if (nodeType == Text::XMLNode::NodeType::ElementEnd)
														{
															break;
														}
														else if (nodeType == Text::XMLNode::NodeType::Element && (reader->GetNodeText()->Equals(UTF8STRC("gml:outerBoundaryIs")) || reader->GetNodeText()->Equals(UTF8STRC("gml:exterior"))))
														{
															while (reader->ReadNext())
															{
																nodeType = reader->GetNodeType();
																if (nodeType == Text::XMLNode::NodeType::ElementEnd)
																{
																	break;
																}
																else if (nodeType == Text::XMLNode::NodeType::Element && reader->GetNodeText()->Equals(UTF8STRC("gml:LinearRing")))
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
																			if (reader->GetNodeText()->Equals(UTF8STRC("gml:coordinates")))
																			{
																				sb.ClearStr();
																				reader->ReadNodeText(&sb);
																				sarr[1] = sb.v;
																				while (true)
																				{
																					i = Text::StrSplit(sarr, 2, sarr[1], ' ');
																					if (Text::StrSplit(sarr2, 3, sarr[0], ',') >= 2)
																					{
																						xPts.Add(Text::StrToDouble(sarr2[0]));
																						yPts.Add(Text::StrToDouble(sarr2[1]));
																					}
																					if (i != 2)
																						break;
																				}

																				if (xPts.GetCount() > 0)
																				{
																					NEW_CLASS(pg, Math::Geometry::Polygon(srid, 1, xPts.GetCount(), false, false));
																					ptList = pg->GetPointList(&i);
																					while (i-- > 0)
																					{
																						ptList[i].x = xPts.GetItem(i);
																						ptList[i].y = yPts.GetItem(i);
																					}
																					SDEL_CLASS(vec);
																					vec = pg;
																				}
																			}
																			else if (reader->GetNodeText()->Equals(UTF8STRC("gml:posList")) && dimension != 0)
																			{
																				sb.ClearStr();
																				reader->ReadNodeText(&sb);
																				sarr[1] = sb.v;
																				while (true)
																				{
																					i = Text::StrSplit(sarr, 2, sarr[1], ' ');
																					xPts.Add(Text::StrToDouble(sarr[0]));
																					if (i != 2)
																						break;
																				}

																				if (xPts.GetCount() > 0 && (xPts.GetCount() % dimension) == 0)
																				{
																					NEW_CLASS(pg, Math::Geometry::Polygon(srid, 1, xPts.GetCount() / dimension, dimension > 2, false));
																					UOSInt j;
																					UOSInt k;
																					Double *zList = pg->GetZList(&j);
																					ptList = pg->GetPointList(&j);
																					i = 0;
																					k = 0;
																					while (i < j)
																					{
																						ptList[i].x = xPts.GetItem(k);
																						ptList[i].y = xPts.GetItem(k + 1);
																						if (dimension > 2)
																						{
																							zList[i] = xPts.GetItem(k + 2);
																						}
																						k += dimension;
																						i++;
																					}
																					SDEL_CLASS(vec);
																					vec = pg;
																				}
																			}
																			else
																			{
																				reader->SkipElement();
																			}
																		}
																	}
																}
																else if (nodeType == Text::XMLNode::NodeType::Element)
																{
																	reader->SkipElement();
																}
															}
														}
														else if (nodeType == Text::XMLNode::NodeType::Element)
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
									}
								}
								else
								{
									UOSInt i = nodeText->IndexOf(':');
									if (i != INVALID_INDEX)
									{
										nameList.Add(Text::StrCopyNew(reader->GetNodeText()->v + i + 1));
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
									else 
									{
										reader->SkipElement();
									}
								}
							}
						}
						if (vec)
						{
							if (lyr == 0)
							{
								colCnt = nameList.GetCount();
								ccols = nameList.GetArray(&i);
								NEW_CLASS(lyr, Map::VectorLayer(layerType, fileName, colCnt, ccols, csys, 0, CSTR_NULL));
							}

							if (colCnt == valList.GetCount())
							{
								Text::String **scols;
								scols = valList.GetArray(&i);
								lyr->AddVector(vec, scols);
							}
							else
							{
								DEL_CLASS(vec);
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
	return lyr;
};//, Parser::ParserList *parsers, Net::WebBrowser *browser, IO::PackageFile *pkgFile);
