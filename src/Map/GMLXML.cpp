#include "Stdafx.h"
#include "Data/ArrayListDbl.h"
#include "Map/GMLXML.h"
#include "Map/VectorLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Geometry/LineString.h"
#include "Math/Geometry/MultiPolygon.h"
#include "Math/Geometry/Point.h"
#include "Math/Geometry/PointZ.h"

Map::IMapDrawLayer *Map::GMLXML::ParseFeatureCollection(Text::XMLReader *reader, Text::CString fileName)
{
	if (reader->GetNodeType() != Text::XMLNode::NodeType::Element || !reader->GetNodeText()->EndsWith(UTF8STRC(":FeatureCollection")))
		return 0;
	
	Text::XMLNode::NodeType nodeType;
	Text::String *nodeText;
	ParseEnv env;
	env.csys = 0;
	env.srid = 0;
	UOSInt colCnt = 0;
	Data::ArrayList<const UTF8Char *> nameList;
	Data::ArrayList<Text::String *> valList;
	Text::StringBuilderUTF8 sb;
	Map::VectorLayer *lyr = 0;
	Map::DrawLayerType layerType = Map::DRAW_LAYER_UNKNOWN;
	const UTF8Char **ccols;
	UOSInt i;
	Math::Geometry::Vector2D *newVec;
	while (reader->ReadNext())
	{
		if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
		{
			break;
		}
		else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
		{
			if (reader->GetNodeText()->EndsWith(UTF8STRC(":featureMember")) || reader->GetNodeText()->Equals(UTF8STRC(":featureMembers")))
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
												if ((newVec = ParseGeometry(reader, &env)) != 0)
												{
													SDEL_CLASS(vec);
													vec = newVec;
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
												if ((newVec = ParseGeometry(reader, &env)) != 0)
												{
													SDEL_CLASS(vec);
													vec = newVec;
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
												if ((newVec = ParseGeometry(reader, &env)) != 0)
												{
													SDEL_CLASS(vec);
													vec = newVec;
												}
											}
										}
									}
								}
								else if (nodeText->EndsWith(UTF8STRC(":geometryProperty")) || nodeText->EndsWith(UTF8STRC(":geom")))
								{
									if (layerType == Map::DRAW_LAYER_UNKNOWN || layerType == Map::DRAW_LAYER_MIXED)
									{
										layerType = Map::DRAW_LAYER_MIXED;
										while (reader->ReadNext())
										{
											if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
											{
												break;
											}
											else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
											{
												if ((newVec = ParseGeometry(reader, &env)) != 0)
												{
													SDEL_CLASS(vec);
													vec = newVec;
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
								NEW_CLASS(lyr, Map::VectorLayer(layerType, fileName, colCnt, ccols, env.csys, 0, CSTR_NULL));
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
}

Math::Geometry::Vector2D *Map::GMLXML::ParseGeometry(Text::XMLReader *reader, ParseEnv *env)
{
	UTF8Char *sarr[4];
	UTF8Char *sarr2[4];
	UOSInt sarr2Cnt;
	Math::Geometry::Vector2D *vec = 0;
	UOSInt i;
	UOSInt j;
	Text::XMLAttrib *attr;
	UOSInt dimension = 0;
	i = reader->GetAttribCount();
	while (i-- > 0)
	{
		attr = reader->GetAttrib(i);
		if (attr->name->Equals(UTF8STRC("srsName")) && env->csys == 0)
		{
			env->csys = Math::CoordinateSystemManager::CreateFromName(attr->value->ToCString());
			if (env->csys)
			{
				env->srid = env->csys->GetSRID();
			}
		}
		else if (attr->name->Equals(UTF8STRC("srsDimension")))
		{
			dimension = attr->value->ToUOSInt();
		}
	}


	Text::String *nodeName = reader->GetNodeText();
	if (nodeName->Equals(UTF8STRC("gml:Point")))
	{
		Data::ArrayListDbl xPts;
		Data::ArrayListDbl yPts;
		Data::ArrayListDbl zPts;
		Math::Geometry::Point *pt;
		Text::StringBuilderUTF8 sb;
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
						if (i < 2)
						{
							break;
						}
						xPts.Add(Text::StrToDouble(sarr[1]));
						yPts.Add(Text::StrToDouble(sarr[0]));
						if (i < 3)
							break;
						zPts.Add(Text::StrToDouble(sarr[2]));
						if (i < 4)
							break;
					}

					if (xPts.GetCount() == 1)
					{
						if (zPts.GetCount() == 1)
						{
							NEW_CLASS(pt, Math::Geometry::PointZ(env->srid, xPts.GetItem(0), yPts.GetItem(0), zPts.GetItem(0)));
						}
						else
						{
							NEW_CLASS(pt, Math::Geometry::Point(env->srid, xPts.GetItem(0), yPts.GetItem(0)));
						}
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
	else if (nodeName->Equals(UTF8STRC("gml:Surface")))
	{
		Data::ArrayListDbl xPts;
		Data::ArrayListDbl yPts;
		Data::ArrayListDbl zPts;
		Math::Geometry::Polygon *pg;
		Math::Coord2DDbl *ptList;
		Text::StringBuilderUTF8 sb;
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
													NEW_CLASS(pg, Math::Geometry::Polygon(env->srid, 1, xPts.GetCount(), false, false));
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
	else if (nodeName->Equals(UTF8STRC("gml:LineString")))
	{
		Data::ArrayListDbl xPts;
		Data::ArrayListDbl yPts;
		Data::ArrayListDbl zPts;
		Math::Geometry::LineString *pl;
		Math::Coord2DDbl *ptList;
		Double *hList;
		Text::StringBuilderUTF8 sb;
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
						NEW_CLASS(pl, Math::Geometry::LineString(env->srid, xPts.GetCount(), true, false));
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
	else if (reader->GetNodeText()->Equals(UTF8STRC("gml:MultiPolygon")))
	{
		Math::Geometry::MultiPolygon *mpg = 0;
		Math::Geometry::Vector2D *newVec;
		while (reader->ReadNext())
		{
			Text::XMLNode::NodeType nodeType = reader->GetNodeType();
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
					else if (nodeType == Text::XMLNode::NodeType::Element)
					{
						newVec = ParseGeometry(reader, env);
						if (newVec)
						{
							if (newVec->GetVectorType() == Math::Geometry::Vector2D::VectorType::Polygon)
							{
								if (mpg == 0)
								{
									NEW_CLASS(mpg, Math::Geometry::MultiPolygon(env->srid, newVec->HasZ(), false));
									SDEL_CLASS(vec);
									vec = mpg;
								}
								mpg->AddGeometry((Math::Geometry::Polygon*)newVec);
							}
							else
							{
								DEL_CLASS(newVec);
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
		Data::ArrayListDbl xPts;
		Data::ArrayListDbl yPts;
		Data::ArrayListDbl zPts;
		Data::ArrayList<UOSInt> ptOfsts;
		Math::Geometry::Polygon *pg;
		Math::Coord2DDbl *ptList;
		Text::StringBuilderUTF8 sb;
		while (reader->ReadNext())
		{
			Text::XMLNode::NodeType nodeType = reader->GetNodeType();
			if (nodeType == Text::XMLNode::NodeType::ElementEnd)
			{
				if (xPts.GetCount() > 0)
				{
					NEW_CLASS(pg, Math::Geometry::Polygon(env->srid, ptOfsts.GetCount(), xPts.GetCount(), zPts.GetCount() == xPts.GetCount(), false));
					UInt32 *ptOfstList = pg->GetPtOfstList(&i);
					while (i-- > 0)
					{
						ptOfstList[i] = (UInt32)ptOfsts.GetItem(i);
					}
					ptList = pg->GetPointList(&i);
					while (i-- > 0)
					{
						ptList[i].x = xPts.GetItem(i);
						ptList[i].y = yPts.GetItem(i);
					}
					if (xPts.GetCount() == zPts.GetCount())
					{
						Double *altList = pg->GetZList(&i);
						while (i-- > 0)
						{
							altList[i] = zPts.GetItem(i);
						}
					}
					SDEL_CLASS(vec);
					vec = pg;
				}
				break;
			}
			else if (nodeType == Text::XMLNode::NodeType::Element)
			{
				nodeName = reader->GetNodeText();
				if (nodeName->Equals(UTF8STRC("gml:outerBoundaryIs")) || nodeName->Equals(UTF8STRC("gml:exterior")) || nodeName->Equals(UTF8STRC("gml:innerBoundaryIs")))
				{
					ptOfsts.Add(xPts.GetCount());

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
											if ((sarr2Cnt = Text::StrSplit(sarr2, 4, sarr[0], ',')) >= 2)
											{
												xPts.Add(Text::StrToDouble(sarr2[0]));
												yPts.Add(Text::StrToDouble(sarr2[1]));
												if (sarr2Cnt >= 3)
												{
													zPts.Add(Text::StrToDouble(sarr2[2]));
												}
											}
											if (i != 2)
												break;
										}
									}
									else if (reader->GetNodeText()->Equals(UTF8STRC("gml:posList")) && dimension != 0)
									{
										Data::ArrayList<Double> posList;
										sb.ClearStr();
										reader->ReadNodeText(&sb);
										sarr[1] = sb.v;
										while (true)
										{
											i = Text::StrSplit(sarr, 2, sarr[1], ' ');
											posList.Add(Text::StrToDouble(sarr[0]));
											if (i != 2)
												break;
										}
										if (dimension == 2)
										{
											i = 0;
											j = posList.GetCount() - 1;
											while (i < j)
											{
												xPts.Add(posList.GetItem(i));
												yPts.Add(posList.GetItem(i + 1));
												i += 2;
											}
										}
										if (dimension == 3)
										{
											i = 0;
											j = posList.GetCount() - 2;
											while (i < j)
											{
												xPts.Add(posList.GetItem(i));
												yPts.Add(posList.GetItem(i + 1));
												zPts.Add(posList.GetItem(i + 2));
												i += 3;
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
	else
	{
		reader->SkipElement();
	}
	return vec;
}
