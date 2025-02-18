#include "Stdafx.h"
#include "Data/ArrayListDbl.h"
#include "Map/GMLXML.h"
#include "Map/VectorLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Geometry/LineString.h"
#include "Math/Geometry/MultiPolygon.h"
#include "Math/Geometry/Point.h"
#include "Math/Geometry/PointZ.h"

Optional<Map::MapDrawLayer> Map::GMLXML::ParseFeatureCollection(NN<Text::XMLReader> reader, Text::CStringNN fileName)
{
	if (reader->GetNodeType() != Text::XMLNode::NodeType::Element || !reader->GetNodeTextNN()->EndsWith(UTF8STRC(":FeatureCollection")))
		return 0;
	
	NN<Text::String> nodeText;
	ParseEnv env;
	env.csys = 0;
	env.srid = 0;
	UOSInt colCnt = 0;
	Data::ArrayListArr<const UTF8Char> nameList;
	Data::ArrayList<Text::String *> valList;
	Text::StringBuilderUTF8 sb;
	Map::VectorLayer *lyr = 0;
	Map::DrawLayerType layerType = Map::DRAW_LAYER_UNKNOWN;
	UnsafeArray<UnsafeArrayOpt<const UTF8Char>> ccols;
	UOSInt i;
	NN<Math::Geometry::Vector2D> newVec;
	while (reader->NextElementName().SetTo(nodeText))
	{
		if (nodeText->EndsWith(UTF8STRC(":featureMember")) || nodeText->EndsWith(UTF8STRC(":featureMembers")))
		{
			while (reader->NextElementName().SetTo(nodeText))
			{
				Text::String *tableName = 0;
				i = nodeText->IndexOf(':');
				if (i != INVALID_INDEX)
				{
					tableName = Text::String::New(nodeText->ToCString().Substring(i + 1)).Ptr();
				}
				Optional<Math::Geometry::Vector2D> vec = 0;
				NN<Math::Geometry::Vector2D> nnvec;
				while (reader->NextElementName().SetTo(nodeText))
				{
					if (nodeText->Equals(UTF8STRC("gml:pointProperty")))
					{
						if (layerType == Map::DRAW_LAYER_UNKNOWN || layerType == Map::DRAW_LAYER_POINT3D || layerType == Map::DRAW_LAYER_POINT)
						{
							while (!reader->NextElementName().IsNull())
							{
								if (ParseGeometry(reader, env).SetTo(newVec))
								{
									vec.Delete();
									vec = newVec;
									if (newVec->HasZ())
										layerType = Map::DRAW_LAYER_POINT3D;
									else
										layerType = Map::DRAW_LAYER_POINT;
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
							while (!reader->NextElementName().IsNull())
							{
								if (ParseGeometry(reader, env).SetTo(newVec))
								{
									vec.Delete();
									vec = newVec;
								}
							}
						}
					}
					else if (nodeText->Equals(UTF8STRC("gml:curveProperty")))
					{
						if (layerType == Map::DRAW_LAYER_UNKNOWN || layerType == Map::DRAW_LAYER_POLYLINE3D || layerType == Map::DRAW_LAYER_POLYLINE)
						{
							while (!reader->NextElementName().IsNull())
							{
								if (ParseGeometry(reader, env).SetTo(newVec))
								{
									vec.Delete();
									vec = newVec;
									if (newVec->HasZ())
										layerType = Map::DRAW_LAYER_POLYLINE3D;
									else
										layerType = Map::DRAW_LAYER_POLYLINE;
								}
							}
						}
					}
					else if (nodeText->EndsWith(UTF8STRC(":geometryProperty")) || nodeText->EndsWith(UTF8STRC(":geom")) || nodeText->EndsWith(UTF8STRC(":geometry")))
					{
						if (layerType == Map::DRAW_LAYER_UNKNOWN || layerType == Map::DRAW_LAYER_MIXED)
						{
							layerType = Map::DRAW_LAYER_MIXED;
							while (!reader->NextElementName().IsNull())
							{
								if (ParseGeometry(reader, env).SetTo(newVec))
								{
									vec.Delete();
									vec = newVec;
								}
							}
						}
					}
					else if (nodeText->Equals(UTF8STRC("gml:boundedBy")))
					{
						reader->SkipElement();
					}
					else
					{
						UOSInt i = nodeText->IndexOf(':');
						if (i != INVALID_INDEX)
						{
							nameList.Add(Text::StrCopyNew(reader->GetNodeTextNN()->v + i + 1).Ptr());
							sb.ClearStr();
							reader->ReadNodeText(sb);
							if (sb.GetLength() > 0)
							{
								valList.Add(Text::String::New(sb.ToString(), sb.GetLength()).Ptr());
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
				if (vec.SetTo(nnvec))
				{
					if (lyr == 0)
					{
						colCnt = nameList.GetCount();
						ccols = UnsafeArray<UnsafeArrayOpt<const UTF8Char>>::ConvertFrom(nameList.Arr());
						NN<Math::CoordinateSystem> csys;
						if (!env.csys.SetTo(csys))
						{
							csys = Math::CoordinateSystemManager::CreateWGS84Csys();
						}
						NEW_CLASS(lyr, Map::VectorLayer(layerType, fileName, colCnt, ccols, csys, 0, CSTR_NULL));
					}

					if (colCnt == valList.GetCount())
					{
						UnsafeArray<Text::String*> scols;
						scols = valList.Arr();
						lyr->AddVector(nnvec, scols.Ptr());
					}
					else
					{
						vec.Delete();
					}
				}

				i = nameList.GetCount();
				while (i-- > 0)
				{
					Text::StrDelNew(nameList.GetItemNoCheck(i));
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

				if (lyr && tableName)
				{
					lyr->SetTableName(tableName);
				}
				SDEL_STRING(tableName);
			}
		}
		else
		{
			reader->SkipElement();
		}
	}
	return lyr;
}

Optional<Math::Geometry::Vector2D> Map::GMLXML::ParseGeometry(NN<Text::XMLReader> reader, NN<ParseEnv> env)
{
	UnsafeArray<UTF8Char> sarr[4];
	UnsafeArray<UTF8Char> sarr2[4];
	UOSInt sarr2Cnt;
	Math::Geometry::Vector2D *vec = 0;
	UOSInt i;
	UOSInt j;
	NN<Text::XMLAttrib> attr;
	NN<Math::CoordinateSystem> csys;
	NN<Text::String> aname;
	UOSInt dimension = 0;
	i = reader->GetAttribCount();
	while (i-- > 0)
	{
		attr = reader->GetAttribNoCheck(i);
		aname = Text::String::OrEmpty(attr->name);
		if (aname->Equals(UTF8STRC("srsName")) && env->csys.IsNull())
		{
			env->csys = Math::CoordinateSystemManager::CreateFromName(Text::String::OrEmpty(attr->value)->ToCString());
			if (env->csys.SetTo(csys))
			{
				env->srid = csys->GetSRID();
			}
		}
		else if (aname->Equals(UTF8STRC("srsDimension")))
		{
			dimension = Text::String::OrEmpty(attr->value)->ToUOSInt();
		}
	}

	NN<Text::String> nodeName = reader->GetNodeTextNN();
	if (nodeName->Equals(UTF8STRC("gml:Point")))
	{
		Double x;
		Double y;
		Data::ArrayListDbl xPts;
		Data::ArrayListDbl yPts;
		Math::Geometry::Point *pt;
		Text::StringBuilderUTF8 sb;
		while (reader->NextElementName().SetTo(nodeName))
		{
			if (nodeName->Equals(UTF8STRC("gml:pos")))
			{
				sb.ClearStr();
				reader->ReadNodeText(sb);
				if (dimension == 2)
				{
					sarr[2] = sb.v;
					while (true)
					{
						i = Text::StrSplit(sarr, 3, sarr[2], ' ');
						if (i < 2)
						{
							break;
						}
						x = Text::StrToDoubleOrNAN(sarr[1]);
						y = Text::StrToDoubleOrNAN(sarr[0]);
						if (x > -90 && x < 90 && ((y >= 90 && y < 180) || (y > -180 && y <= -90)))
						{
							xPts.Add(y);
							yPts.Add(x);
						}
						else
						{
							xPts.Add(x);
							yPts.Add(y);
						}
						if (i < 3)
							break;
					}

					if (xPts.GetCount() == 1)
					{
						NEW_CLASS(pt, Math::Geometry::Point(env->srid, xPts.GetItem(0), yPts.GetItem(0)));
						SDEL_CLASS(vec);
						vec = pt;
					}
				}
				else
				{
					Data::ArrayListDbl zPts;
					sarr[3] = sb.v;
					while (true)
					{
						i = Text::StrSplit(sarr, 4, sarr[3], ' ');
						if (i < 2)
						{
							break;
						}
						x = Text::StrToDoubleOrNAN(sarr[1]);
						y = Text::StrToDoubleOrNAN(sarr[0]);
						if (x > -90 && x < 90 && ((y >= 90 && y < 180) || (y > -180 && y <= -90)))
						{
							xPts.Add(y);
							yPts.Add(x);
						}
						else
						{
							xPts.Add(x);
							yPts.Add(y);
						}
						if (i < 3)
							break;
						zPts.Add(Text::StrToDoubleOrNAN(sarr[2]));
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
			}
			else
			{
				reader->SkipElement();
			}
		}
	}
	else if (nodeName->Equals(UTF8STRC("gml:Surface")))
	{
		Data::ArrayListDbl xPts;
		Data::ArrayListDbl yPts;
		Data::ArrayListDbl zPts;
		Math::Geometry::Polygon *pg;
		NN<Math::Geometry::LinearRing> lr;
		UnsafeArray<Math::Coord2DDbl> ptList;
		Text::StringBuilderUTF8 sb;
		while (reader->NextElementName().SetTo(nodeName))
		{
			if (nodeName->Equals(UTF8STRC("gml:patches")))
			{
				while (reader->NextElementName().SetTo(nodeName))
				{
					if (nodeName->Equals(UTF8STRC("gml:PolygonPatch")))
					{
						NEW_CLASS(pg, Math::Geometry::Polygon(env->srid));
						while (reader->NextElementName().SetTo(nodeName))
						{
							if (nodeName->Equals(UTF8STRC("gml:exterior")))
							{
								while (reader->NextElementName().SetTo(nodeName))
								{
									if (nodeName->Equals(UTF8STRC("gml:LinearRing")))
									{
										while (reader->NextElementName().SetTo(nodeName))
										{
											if (nodeName->Equals(UTF8STRC("gml:posList")))
											{
												sb.ClearStr();
												reader->ReadNodeText(sb);
												if (dimension == 2)
												{
													sarr[2] = sb.v;
													while (true)
													{
														i = Text::StrSplit(sarr, 3, sarr[2], ' ');
														if (i < 2)
														{
															break;
														}
														xPts.Add(Text::StrToDoubleOrNAN(sarr[1]));
														yPts.Add(Text::StrToDoubleOrNAN(sarr[0]));
														if (i < 3)
															break;
													}

													if (xPts.GetCount() > 0)
													{
														NEW_CLASSNN(lr, Math::Geometry::LinearRing(env->srid, xPts.GetCount(), false, false));
														ptList = lr->GetPointList(i);
														while (i-- > 0)
														{
															ptList[i].x = xPts.GetItem(i);
															ptList[i].y = yPts.GetItem(i);
														}
														pg->AddGeometry(lr);
													}
												}
												else
												{
													sarr[3] = sb.v;
													while (true)
													{
														i = Text::StrSplit(sarr, 4, sarr[3], ' ');
														if (i < 3)
														{
															break;
														}
														xPts.Add(Text::StrToDoubleOrNAN(sarr[1]));
														yPts.Add(Text::StrToDoubleOrNAN(sarr[0]));
														zPts.Add(Text::StrToDoubleOrNAN(sarr[2]));
														if (i < 4)
															break;
													}

													if (xPts.GetCount() > 0)
													{
														NEW_CLASSNN(lr, Math::Geometry::LinearRing(env->srid, xPts.GetCount(), false, false));
														ptList = lr->GetPointList(i);
														while (i-- > 0)
														{
															ptList[i].x = xPts.GetItem(i);
															ptList[i].y = yPts.GetItem(i);
														}
														pg->AddGeometry(lr);
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
							}
							else
							{
								reader->SkipElement();
							}
						}
						if (pg->GetCount() > 0)
						{
							SDEL_CLASS(vec);
							vec = pg;
						}
						else
						{
							DEL_CLASS(pg);
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
	else if (nodeName->Equals(UTF8STRC("gml:LineString")))
	{
		Data::ArrayListDbl xPts;
		Data::ArrayListDbl yPts;
		Math::Geometry::LineString *pl;
		UnsafeArray<Math::Coord2DDbl> ptList;
		Text::StringBuilderUTF8 sb;
		while (reader->NextElementName().SetTo(nodeName))
		{
			if (nodeName->Equals(UTF8STRC("gml:posList")))
			{
				sb.ClearStr();
				reader->ReadNodeText(sb);
				if (dimension == 2)
				{
					sarr[2] = sb.v;
					while (true)
					{
						i = Text::StrSplit(sarr, 3, sarr[2], ' ');
						if (i < 2)
						{
							break;
						}
						xPts.Add(Text::StrToDoubleOrNAN(sarr[0]));
						yPts.Add(Text::StrToDoubleOrNAN(sarr[1]));
						if (i < 3)
							break;
					}

					if (xPts.GetCount() > 0)
					{
						NEW_CLASS(pl, Math::Geometry::LineString(env->srid, xPts.GetCount(), false, false));
						ptList = pl->GetPointList(i);
						while (i-- > 0)
						{
							ptList[i] = Math::Coord2DDbl(yPts.GetItem(i), xPts.GetItem(i));
						}
						SDEL_CLASS(vec);
						vec = pl;
					}
				}
				else
				{
					UnsafeArray<Double> hList;
					Data::ArrayListDbl zPts;
					sarr[3] = sb.v;
					while (true)
					{
						i = Text::StrSplit(sarr, 4, sarr[3], ' ');
						if (i < 3)
						{
							break;
						}
						xPts.Add(Text::StrToDoubleOrNAN(sarr[0]));
						yPts.Add(Text::StrToDoubleOrNAN(sarr[1]));
						zPts.Add(Text::StrToDoubleOrNAN(sarr[2]));
						if (i < 4)
							break;
					}

					if (xPts.GetCount() > 0)
					{
						NEW_CLASS(pl, Math::Geometry::LineString(env->srid, xPts.GetCount(), true, false));
						ptList = pl->GetPointList(i);
						if (pl->GetZList(i).SetTo(hList))
						{
							while (i-- > 0)
							{
								hList[i] = zPts.GetItem(i);
								ptList[i].y = xPts.GetItem(i);
								ptList[i].x = yPts.GetItem(i);
							}
							SDEL_CLASS(vec);
							vec = pl;
						}
						else
						{
							DEL_CLASS(pl);
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
	else if (nodeName->Equals(UTF8STRC("gml:MultiPolygon")))
	{
		Math::Geometry::MultiPolygon *mpg = 0;
		NN<Math::Geometry::Vector2D> newVec;
		while (reader->NextElementName().SetTo(nodeName))
		{
			if (nodeName->Equals(UTF8STRC("gml:polygonMember")))
			{
				while (reader->NextElementName().SetTo(nodeName))
				{
					if (ParseGeometry(reader, env).SetTo(newVec))
					{
						if (newVec->GetVectorType() == Math::Geometry::Vector2D::VectorType::Polygon)
						{
							if (mpg == 0)
							{
								NEW_CLASS(mpg, Math::Geometry::MultiPolygon(env->srid));
								SDEL_CLASS(vec);
								vec = mpg;
							}
							mpg->AddGeometry(NN<Math::Geometry::Polygon>::ConvertFrom(newVec));
						}
						else
						{
							newVec.Delete();
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
	else if (nodeName->Equals(UTF8STRC("gml:Polygon")))
	{
		Data::ArrayListDbl xPts;
		Data::ArrayListDbl yPts;
		Data::ArrayListDbl zPts;
		Math::Geometry::Polygon *pg;
		NN<Math::Geometry::LinearRing> lr;
		UnsafeArray<Math::Coord2DDbl> ptList;
		Text::StringBuilderUTF8 sb;
		NEW_CLASS(pg, Math::Geometry::Polygon(env->srid));
		while (reader->NextElementName().SetTo(nodeName))
		{
			if (nodeName->Equals(UTF8STRC("gml:outerBoundaryIs")) || nodeName->Equals(UTF8STRC("gml:exterior")) || nodeName->Equals(UTF8STRC("gml:innerBoundaryIs")))
			{
				xPts.Clear();
				yPts.Clear();
				zPts.Clear();
				while (reader->NextElementName().SetTo(nodeName))
				{
					if (nodeName->Equals(UTF8STRC("gml:LinearRing")))
					{
						while (reader->NextElementName().SetTo(nodeName))
						{
							if (nodeName->Equals(UTF8STRC("gml:coordinates")))
							{
								sb.ClearStr();
								reader->ReadNodeText(sb);
								sarr[1] = sb.v;
								while (true)
								{
									i = Text::StrSplit(sarr, 2, sarr[1], ' ');
									if ((sarr2Cnt = Text::StrSplit(sarr2, 4, sarr[0], ',')) >= 2)
									{
										xPts.Add(Text::StrToDoubleOrNAN(sarr2[0]));
										yPts.Add(Text::StrToDoubleOrNAN(sarr2[1]));
										if (sarr2Cnt >= 3)
										{
											zPts.Add(Text::StrToDoubleOrNAN(sarr2[2]));
										}
									}
									if (i != 2)
										break;
								}
							}
							else if (nodeName->Equals(UTF8STRC("gml:posList")) && dimension != 0)
							{
								Data::ArrayList<Double> posList;
								sb.ClearStr();
								reader->ReadNodeText(sb);
								sarr[1] = sb.v;
								while (true)
								{
									i = Text::StrSplit(sarr, 2, sarr[1], ' ');
									posList.Add(Text::StrToDoubleOrNAN(sarr[0]));
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
					else
					{
						reader->SkipElement();
					}
				}
				if (xPts.GetCount() > 0)
				{
					NEW_CLASSNN(lr, Math::Geometry::LinearRing(env->srid, xPts.GetCount(), zPts.GetCount() == xPts.GetCount(), false));
					ptList = lr->GetPointList(i);
					while (i-- > 0)
					{
						ptList[i].x = xPts.GetItem(i);
						ptList[i].y = yPts.GetItem(i);
					}
					UnsafeArray<Double> altList;
					if (xPts.GetCount() == zPts.GetCount() && lr->GetZList(i).SetTo(altList))
					{
						while (i-- > 0)
						{
							altList[i] = zPts.GetItem(i);
						}
					}
					pg->AddGeometry(lr);
				}
			}
			else
			{
				reader->SkipElement();
			}
		}
		if (pg->GetCount() > 0)
		{
			SDEL_CLASS(vec);
			vec = pg;
		}
		else
		{
			DEL_CLASS(pg);
		}
	}
	else
	{
		reader->SkipElement();
	}
	return vec;
}
