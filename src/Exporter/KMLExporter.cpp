#include "Stdafx.h"
#include "MyMemory.h"
#include "Exporter/KMLExporter.h"
#include "IO/BufferedOutputStream.h"
#include "IO/Path.h"
#include "IO/StreamWriter.h"
#include "Map/MapDrawLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math.h"
#include "Math/ProjectedCoordinateSystem.h"
#include "Math/Geometry/LineString.h"
#include "Math/Geometry/PointZ.h"
#include "Math/Geometry/Polyline.h"
#include "Math/Geometry/Polygon.h"
#include "Math/Geometry/VectorImage.h"
#include "Text/XML.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Exporter::KMLExporter::KMLExporter()
{
	this->codePage = 65001;
	this->encFact = 0;
}

Exporter::KMLExporter::~KMLExporter()
{
}

Int32 Exporter::KMLExporter::GetName()
{
	return *(Int32*)"KMLE";
}

IO::FileExporter::SupportType Exporter::KMLExporter::IsObjectSupported(NN<IO::ParsedObject> pobj)
{
	if (pobj->GetParserType() != IO::ParserType::MapLayer)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	NN<Map::MapDrawLayer> layer = NN<Map::MapDrawLayer>::ConvertFrom(pobj);
	Map::DrawLayerType layerType = layer->GetLayerType();
	if (layerType == Map::DRAW_LAYER_POINT)
	{
		return IO::FileExporter::SupportType::NormalStream;
	}
	else if (layerType == Map::DRAW_LAYER_POLYLINE)
	{
		return IO::FileExporter::SupportType::NormalStream;
	}
	else if (layerType == Map::DRAW_LAYER_POINT3D)
	{
		return IO::FileExporter::SupportType::NormalStream;
	}
	else if (layerType == Map::DRAW_LAYER_POLYLINE3D)
	{
		return IO::FileExporter::SupportType::NormalStream;
	}
	else if (layerType == Map::DRAW_LAYER_POLYGON)
	{
		return IO::FileExporter::SupportType::NormalStream;
	}
	else if (layerType == Map::DRAW_LAYER_IMAGE && layer->GetObjectClass() != Map::MapDrawLayer::OC_TILE_MAP_LAYER)
	{
		return IO::FileExporter::SupportType::NormalStream;
	}
	return IO::FileExporter::SupportType::NotSupported;
}

Bool Exporter::KMLExporter::GetOutputName(UOSInt index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("KML file"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.kml"));
		return true;
	}
	return false;
}

void Exporter::KMLExporter::SetCodePage(UInt32 codePage)
{
	this->codePage = codePage;
}

void Exporter::KMLExporter::SetEncFactory(Optional<Text::EncodingFactory> encFact)
{
	this->encFact = encFact;
}

Bool Exporter::KMLExporter::ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
{
	if (pobj->GetParserType() != IO::ParserType::MapLayer)
	{
		return false;
	}
	Double defHeight = 10.0;
	UTF8Char sbuff[256];
	UTF8Char sbuff2[512];
	UnsafeArray<UTF8Char> sptr;
	NN<Map::MapDrawLayer> layer = NN<Map::MapDrawLayer>::ConvertFrom(pobj);
	UOSInt nameCol = layer->GetNameCol();

	UOSInt i;
	UOSInt j;
	UOSInt k;
	Int64 currId;
	Int64 lastId;
	Optional<Map::NameArray> nameArr;
	NN<Map::GetObjectSess> sess;
	NN<Math::Geometry::Vector2D> vec;
	Text::Encoding enc(this->codePage);
	Text::StringBuilderUTF8 sb;
	NN<Text::String> s;
	NN<Math::CoordinateSystem> srcCsys = layer->GetCoordinateSystem();
	NN<Math::CoordinateSystem> destCsys = Math::CoordinateSystemManager::CreateWGS84Csys();
	Bool needConv = false;
	if (!destCsys->Equals(srcCsys))
	{
		needConv = true;
	}

	NN<ParamData> para;
	if (param.SetTo(para))
	{
		defHeight = *(Int32*)para.Ptr();
	}
	IO::BufferedOutputStream cstm(stm, 65536);
	IO::StreamWriter writer(cstm, &enc);

	sptr = Text::StrConcatC(sbuff2, UTF8STRC("<?xml version=\"1.0\" encoding=\""));
	sptr = Text::EncodingFactory::GetInternetName(sptr, this->codePage);
	sptr = Text::StrConcatC(sptr, UTF8STRC("\"?>"));
	sptr = Text::StrConcatC(sptr, UTF8STRC("<kml xmlns=\"http://www.opengis.net/kml/2.2\" xmlns:gx=\"http://www.google.com/kml/ext/2.2\" xmlns:kml=\"http://www.opengis.net/kml/2.2\" xmlns:atom=\"http://www.w3.org/2005/Atom\">"));
	sptr = Text::StrConcatC(sptr, UTF8STRC("<Document>"));
	i = fileName.LastIndexOf(IO::Path::PATH_SEPERATOR);
	sb.ClearStr();
	sb.Append(fileName.Substring(i + 1));
	i = sb.LastIndexOf('.');
	if (i != INVALID_INDEX)
		sb.TrimToLength(i);
	s = Text::XML::ToNewXMLText(sb.ToString());
	sptr = Text::StrConcatC(sptr, UTF8STRC("<name>"));
	sptr = s->ConcatTo(sptr);
	sptr = Text::StrConcatC(sptr, UTF8STRC("</name>"));
	s->Release();
	sptr = Text::StrConcatC(sptr, UTF8STRC("<Folder>"));
	sptr = Text::StrConcatC(sptr, UTF8STRC("<name>"));
	Map::DrawLayerType layerType = layer->GetLayerType();
	if (layerType == Map::DRAW_LAYER_POINT3D || layerType == Map::DRAW_LAYER_POINT3D)
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("Points"));
	}
	else if (layerType == Map::DRAW_LAYER_POLYLINE || layerType == Map::DRAW_LAYER_POLYLINE3D)
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("Lines"));
	}
	else if (layerType == Map::DRAW_LAYER_POLYGON)
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("Areas"));
	}
	else if (layerType == Map::DRAW_LAYER_IMAGE)
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("Images"));
	}
	else
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("Geometries"));
	}
	sptr = Text::StrConcatC(sptr, UTF8STRC("</name>"));
	sptr = Text::StrConcatC(sptr, UTF8STRC("<open>1</open>"));
	writer.Write(CSTRP(sbuff2, sptr));

	writer.Write(CSTR("<Style id=\"lineLabel\"><LineStyle><gx:labelVisibility>1</gx:labelVisibility></LineStyle></Style>"));

	Data::ArrayListInt64 ids;
	layer->GetAllObjectIds(ids, nameArr);

	sess = layer->BeginGetObject();
	lastId = -1;
	i = 0;
	j = ids.GetCount();
	while (i < j)
	{
		currId = ids.GetItem(i);
		if (currId != lastId)
		{
			lastId = currId;
			if (!layer->GetNewVectorById(sess, currId).SetTo(vec))
			{

			}
			else
			{
				Math::Geometry::Vector2D::VectorType vecType = vec->GetVectorType();
				if (vecType == Math::Geometry::Vector2D::VectorType::Point)
				{
					NN<Math::Geometry::Point> pt = NN<Math::Geometry::Point>::ConvertFrom(vec);
					Math::Coord2DDbl coord = pt->GetCenter();
					Double z;
					if (pt->HasZ())
					{
						z = NN<Math::Geometry::PointZ>::ConvertFrom(pt)->GetZ();
					}
					else
					{
						z = 0;
					}
					sb.ClearStr();
					if (!layer->GetString(sb, nameArr, currId, nameCol))
					{
						sb.AppendI64(currId);
					}
					if (needConv)
					{
						Math::Vector3 vec = Math::CoordinateSystem::Convert3D(srcCsys, destCsys, Math::Vector3(coord, z));
						coord = vec.GetXY();
						z = vec.GetZ();
					}

					sptr = Text::StrConcatC(sbuff2, UTF8STRC("<Placemark>"));
					sptr = Text::StrConcatC(sptr, UTF8STRC("<name>"));
					sptr = Text::XML::ToXMLText(sptr, sb.v);
					sptr = Text::StrConcatC(sptr, UTF8STRC("</name>"));
					sptr = Text::StrConcatC(sptr, UTF8STRC("<Point><coordinates>"));
					sptr = Text::StrDouble(sptr, coord.x);
					sptr = Text::StrConcatC(sptr, UTF8STRC(","));
					sptr = Text::StrDouble(sptr, coord.y);
					sptr = Text::StrConcatC(sptr, UTF8STRC(","));
					sptr = Text::StrDouble(sptr, z);
					sptr = Text::StrConcatC(sptr, UTF8STRC("</coordinates></Point>"));
					sptr = Text::StrConcatC(sptr, UTF8STRC("</Placemark>"));
					writer.WriteLine(CSTRP(sbuff2, sptr));
				}
				else if (vecType == Math::Geometry::Vector2D::VectorType::LineString)
				{
					UOSInt nPoints;
					NN<Math::Geometry::LineString> pl = NN<Math::Geometry::LineString>::ConvertFrom(vec);
					sb.ClearStr();
					if (!layer->GetString(sb, nameArr, currId, nameCol))
					{
						sb.AppendI64(currId);
					}

					sptr = Text::XML::ToXMLText(sbuff2, sb.v);
					sb.ClearStr();
					sb.AppendC(UTF8STRC("<Placemark><name>"));
					sb.AppendP(sbuff2, sptr);
					sb.AppendC(UTF8STRC("</name><styleUrl>#lineLabel</styleUrl><LineString><coordinates>"));

					UnsafeArray<Math::Coord2DDbl> points = pl->GetPointList(nPoints);
					UnsafeArray<Double> alts;
					if (needConv)
					{
						Math::Vector3 v;
						if (vec->HasZ() && pl->GetZList(nPoints).SetTo(alts))
						{
							k = 0;
							while (k < nPoints)
							{
								v = Math::CoordinateSystem::Convert3D(srcCsys, destCsys, Math::Vector3(points[k], alts[k]));

								sptr = sbuff2;
								sptr = Text::StrDouble(sptr, v.GetX());
								*sptr++ = ',';
								sptr = Text::StrDouble(sptr, v.GetY());
								*sptr++ = ',';
								sptr = Text::StrDouble(sptr, v.GetZ());
								*sptr++ = ' ';
								*sptr = 0;

								sb.AppendC(sbuff2, (UOSInt)(sptr - sbuff2));
								k++;
							}
						}
						else
						{
							k = 0;
							while (k < nPoints)
							{
								v = Math::CoordinateSystem::Convert3D(srcCsys, destCsys, Math::Vector3(points[k], defHeight));
								sptr = sbuff2;
								sptr = Text::StrDouble(sptr, v.GetX());
								*sptr++ = ',';
								sptr = Text::StrDouble(sptr, v.GetY());
								*sptr++ = ',';
								sptr = Text::StrDouble(sptr, v.GetZ());
								*sptr++ = ' ';
								*sptr = 0;

								sb.AppendC(sbuff2, (UOSInt)(sptr - sbuff2));
								k++;
							}
						}
					}
					else
					{
						if (vec->HasZ() && pl->GetZList(nPoints).SetTo(alts))
						{
							k = 0;
							while (k < nPoints)
							{
								sptr = sbuff2;
								sptr = Text::StrDouble(sptr, points[k].x);
								*sptr++ = ',';
								sptr = Text::StrDouble(sptr, points[k].y);
								*sptr++ = ',';
								sptr = Text::StrDouble(sptr, alts[k]);
								*sptr++ = ' ';
								*sptr = 0;

								sb.AppendC(sbuff2, (UOSInt)(sptr - sbuff2));
								k++;
							}
						}
						else
						{
							k = 0;
							while (k < nPoints)
							{
								sptr = sbuff2;
								sptr = Text::StrDouble(sptr, points[k].x);
								*sptr++ = ',';
								sptr = Text::StrDouble(sptr, points[k].y);
								*sptr++ = ',';
								sptr = Text::StrDouble(sptr, defHeight);
								*sptr++ = ' ';
								*sptr = 0;

								sb.AppendC(sbuff2, (UOSInt)(sptr - sbuff2));
								k++;
							}
						}
					}

					sb.AppendC(UTF8STRC("</coordinates></LineString></Placemark>"));
					writer.WriteLine(sb.ToCString());					
				}
				else if (vecType == Math::Geometry::Vector2D::VectorType::Polyline)
				{
					UOSInt nPoints;
					NN<Math::Geometry::Polyline> pl = NN<Math::Geometry::Polyline>::ConvertFrom(vec);
					sb.ClearStr();
					if (!layer->GetString(sb, nameArr, currId, nameCol))
					{
						sb.AppendI64(currId);
					}

					sptr = Text::XML::ToXMLText(sbuff2, sb.v);
					sb.ClearStr();
					sb.AppendC(UTF8STRC("<Placemark><name>"));
					sb.AppendP(sbuff2, sptr);
					sb.AppendC(UTF8STRC("</name><styleUrl>#lineLabel</styleUrl><LineString><coordinates>"));

					NN<Math::Geometry::LineString> lineString;
					Data::ArrayIterator<NN<Math::Geometry::LineString>> it = pl->Iterator();
					while (it.HasNext())
					{
						lineString = it.Next();
						UnsafeArray<Math::Coord2DDbl> points = lineString->GetPointList(nPoints);
						UnsafeArray<Double> alts;
						if (needConv)
						{
							Math::Vector3 v;
							if (lineString->HasZ() && lineString->GetZList(nPoints).SetTo(alts))
							{
								k = 0;
								while (k < nPoints)
								{
									v = Math::CoordinateSystem::Convert3D(srcCsys, destCsys, Math::Vector3(points[k], alts[k]));

									sptr = sbuff2;
									sptr = Text::StrDouble(sptr, v.GetX());
									*sptr++ = ',';
									sptr = Text::StrDouble(sptr, v.GetY());
									*sptr++ = ',';
									sptr = Text::StrDouble(sptr, v.GetZ());
									*sptr++ = ' ';
									*sptr = 0;

									sb.AppendC(sbuff2, (UOSInt)(sptr - sbuff2));
									k++;
								}
							}
							else
							{
								k = 0;
								while (k < nPoints)
								{
									v = Math::CoordinateSystem::Convert3D(srcCsys, destCsys, Math::Vector3(points[k], defHeight));
									sptr = sbuff2;
									sptr = Text::StrDouble(sptr, v.GetX());
									*sptr++ = ',';
									sptr = Text::StrDouble(sptr, v.GetY());
									*sptr++ = ',';
									sptr = Text::StrDouble(sptr, v.GetZ());
									*sptr++ = ' ';
									*sptr = 0;

									sb.AppendC(sbuff2, (UOSInt)(sptr - sbuff2));
									k++;
								}
							}
						}
						else
						{
							if (lineString->HasZ() && lineString->GetZList(nPoints).SetTo(alts))
							{
								k = 0;
								while (k < nPoints)
								{
									sptr = sbuff2;
									sptr = Text::StrDouble(sptr, points[k].x);
									*sptr++ = ',';
									sptr = Text::StrDouble(sptr, points[k].y);
									*sptr++ = ',';
									sptr = Text::StrDouble(sptr, alts[k]);
									*sptr++ = ' ';
									*sptr = 0;

									sb.AppendC(sbuff2, (UOSInt)(sptr - sbuff2));
									k++;
								}
							}
							else
							{
								k = 0;
								while (k < nPoints)
								{
									sptr = sbuff2;
									sptr = Text::StrDouble(sptr, points[k].x);
									*sptr++ = ',';
									sptr = Text::StrDouble(sptr, points[k].y);
									*sptr++ = ',';
									sptr = Text::StrDouble(sptr, defHeight);
									*sptr++ = ' ';
									*sptr = 0;

									sb.AppendC(sbuff2, (UOSInt)(sptr - sbuff2));
									k++;
								}
							}
						}
					}
					sb.AppendC(UTF8STRC("</coordinates></LineString></Placemark>"));
					writer.WriteLine(sb.ToCString());
				}
				else if (vecType == Math::Geometry::Vector2D::VectorType::Polygon)
				{
					UOSInt nPoints;
					NN<Math::Geometry::Polygon> pg = NN<Math::Geometry::Polygon>::ConvertFrom(vec);
					sb.ClearStr();
					if (!layer->GetString(sb, nameArr, currId, nameCol))
					{
						sb.AppendI64(currId);
					}

					sptr = Text::XML::ToXMLText(sbuff2, sb.v);
					sb.ClearStr();
					sb.AppendC(UTF8STRC("<Placemark>"));
					sb.AppendC(UTF8STRC("<name>"));
					sb.AppendP(sbuff2, sptr);
					sb.AppendC(UTF8STRC("</name>"));
					sb.AppendC(UTF8STRC("<Polygon>"));
					sb.AppendC(UTF8STRC("<tessellate>1</tessellate>"));
					sb.AppendC(UTF8STRC("<altitudeMode>relativeToGround</altitudeMode>"));

					UnsafeArray<Math::Coord2DDbl> points;
					NN<Math::Geometry::LinearRing> lr;

					if (needConv)
					{
						Math::Vector3 v;
						Data::ArrayIterator<NN<Math::Geometry::LinearRing>> it = pg->Iterator();
						while (it.HasNext())
						{
							lr = it.Next();
							sb.AppendC(UTF8STRC("<outerBoundaryIs><LinearRing><coordinates>"));
							k = 0;
							points = lr->GetPointList(nPoints);
							while (k < nPoints)
							{
								v = Math::CoordinateSystem::Convert3D(srcCsys, destCsys, Math::Vector3(points[k], defHeight));
								sptr = Text::StrDouble(sbuff2, v.GetX());
								sptr = Text::StrConcatC(sptr, UTF8STRC(","));
								sptr = Text::StrDouble(sptr, v.GetY());
								sptr = Text::StrConcatC(sptr, UTF8STRC(","));
								sptr = Text::StrDouble(sptr, v.GetZ());
								sptr = Text::StrConcatC(sptr, UTF8STRC(" "));
								sb.AppendC(sbuff2, (UOSInt)(sptr - sbuff2));
								k++;
							}
							sb.AppendC(UTF8STRC("</coordinates></LinearRing></outerBoundaryIs>"));
						}
					}
					else
					{
						Data::ArrayIterator<NN<Math::Geometry::LinearRing>> it = pg->Iterator();
						while (it.HasNext())
						{
							lr = it.Next();
							sb.AppendC(UTF8STRC("<outerBoundaryIs><LinearRing><coordinates>"));
							k = 0;
							points = lr->GetPointList(nPoints);
							while (k < nPoints)
							{
								sptr = Text::StrDouble(sbuff2, points[k].x);
								sptr = Text::StrConcatC(sptr, UTF8STRC(","));
								sptr = Text::StrDouble(sptr, points[k].y);
								sptr = Text::StrConcatC(sptr, UTF8STRC(","));
								sptr = Text::StrDouble(sptr, defHeight);
								sptr = Text::StrConcatC(sptr, UTF8STRC(" "));
								sb.AppendC(sbuff2, (UOSInt)(sptr - sbuff2));
								k++;
							}
							sb.AppendC(UTF8STRC("</coordinates></LinearRing></outerBoundaryIs>"));
						}
					}

					sb.AppendC(UTF8STRC("</Polygon>"));
					sb.AppendC(UTF8STRC("</Placemark>"));
					writer.WriteLine(sb.ToCString());
				}
				else if (vecType == Math::Geometry::Vector2D::VectorType::Image)
				{
					NN<Math::Geometry::VectorImage> img = NN<Math::Geometry::VectorImage>::ConvertFrom(vec);
					sb.ClearStr();
					if (!layer->GetString(sb, nameArr, currId, nameCol))
					{
						sb.AppendI64(currId);
					}

					Math::RectAreaDbl bounds;
					Int64 timeStart;
					Int64 timeEnd;
					sptr = Text::XML::ToXMLText(sbuff2, sb.v);
					if (img->IsScnCoord())
					{
						sb.ClearStr();
						sb.AppendC(UTF8STRC("<ScreenOverlay>"));
						sb.AppendC(UTF8STRC("<name>"));
						sb.AppendP(sbuff2, sptr);
						sb.AppendC(UTF8STRC("</name>"));
						timeStart = img->GetTimeStart();
						timeEnd = img->GetTimeEnd();
						if (timeStart != 0 && timeEnd != 0)
						{
							Data::DateTime dt;
							dt.ToUTCTime();
							dt.SetUnixTimestamp(timeStart);
							sb.AppendC(UTF8STRC("<TimeStamp><when>"));
							sptr = dt.ToString(sbuff, "yyyy-MM-dd");
							sb.AppendP(sbuff, sptr);
							sb.AppendC(UTF8STRC("T"));
							sptr = dt.ToString(sbuff, "HH:mm");
							sb.AppendP(sbuff, sptr);
							sb.AppendC(UTF8STRC("Z</when></TimeStamp>"));

							sb.AppendC(UTF8STRC("<TimeSpan><begin>"));
							sptr = dt.ToString(sbuff, "yyyy-MM-dd");
							sb.AppendP(sbuff, sptr);
							sb.AppendC(UTF8STRC("T"));
							sptr = dt.ToString(sbuff, "HH:mm:ss");
							sb.AppendP(sbuff, sptr);
							sb.AppendC(UTF8STRC("Z</begin>"));
							dt.SetUnixTimestamp(timeEnd);
							sb.AppendC(UTF8STRC("<end>"));
							sptr = dt.ToString(sbuff, "yyyy-MM-dd");
							sb.AppendP(sbuff, sptr);
							sb.AppendC(UTF8STRC("T"));
							sptr = dt.ToString(sbuff, "HH:mm:ss");
							sb.AppendP(sbuff, sptr);
							sb.AppendC(UTF8STRC("Z</end></TimeSpan>"));
						}
						if (img->HasSrcAlpha())
						{
							sb.AppendC(UTF8STRC("<color>"));
							sb.AppendHex32(((UInt32)Double2Int32(img->GetSrcAlpha() * 255.0) << 24) | 0xffffff);
							sb.AppendC(UTF8STRC("</color>"));
						}
						if (img->HasZIndex())
						{
							sb.AppendC(UTF8STRC("<drawOrder>"));
							sb.AppendI32(img->GetZIndex());
							sb.AppendC(UTF8STRC("</drawOrder>"));
						}
						sb.AppendC(UTF8STRC("<Icon><href>"));
						//////////////////////////////////////////////////////
						sb.AppendOpt(img->GetSourceAddr());
						sb.AppendC(UTF8STRC("</href></Icon>"));

						bounds = img->GetBounds();
						sb.AppendC(UTF8STRC("<overlayXY x=\""));
						sb.AppendDouble(bounds.max.x);
						sb.AppendC(UTF8STRC("\" y=\""));
						sb.AppendDouble(bounds.max.y);
						sb.AppendC(UTF8STRC("\" xunits=\"fraction\" yunits=\"fraction\"/>"));

						sb.AppendC(UTF8STRC("<screenXY x=\""));
						sb.AppendDouble(bounds.min.x);
						sb.AppendC(UTF8STRC("\" y=\""));
						sb.AppendDouble(bounds.min.y);
						sb.AppendC(UTF8STRC("\" xunits=\"fraction\" yunits=\"fraction\"/>"));

						bounds.min = img->GetVectorSize();
						sb.AppendC(UTF8STRC("<size x=\""));
						sb.AppendDouble(bounds.min.x);
						sb.AppendC(UTF8STRC("\" y=\""));
						sb.AppendDouble(bounds.min.y);
						sb.AppendC(UTF8STRC("\" xunits=\"fraction\" yunits=\"fraction\"/>"));

						sb.AppendC(UTF8STRC("</ScreenOverlay>"));
						writer.WriteLine(sb.ToCString());
					}
					else
					{
						sb.ClearStr();
						sb.AppendC(UTF8STRC("<GroundOverlay>"));
						sb.AppendC(UTF8STRC("<name>"));
						sb.AppendP(sbuff2, sptr);
						sb.AppendC(UTF8STRC("</name>"));
						timeStart = img->GetTimeStart();
						timeEnd = img->GetTimeEnd();
						if (timeStart != 0 && timeEnd != 0)
						{
							Data::DateTime dt;
							dt.ToUTCTime();
							dt.SetUnixTimestamp(timeStart);
							sb.AppendC(UTF8STRC("<TimeStamp><when>"));
							sptr = dt.ToString(sbuff, "yyyy-MM-dd");
							sb.AppendP(sbuff, sptr);
							sb.AppendC(UTF8STRC("T"));
							sptr = dt.ToString(sbuff, "HH:mm");
							sb.AppendP(sbuff, sptr);
							sb.AppendC(UTF8STRC("Z</when></TimeStamp>"));

							sb.AppendC(UTF8STRC("<TimeSpan><begin>"));
							sptr = dt.ToString(sbuff, "yyyy-MM-dd");
							sb.AppendP(sbuff, sptr);
							sb.AppendC(UTF8STRC("T"));
							sptr = dt.ToString(sbuff, "HH:mm:ss");
							sb.AppendP(sbuff, sptr);
							sb.AppendC(UTF8STRC("Z</begin>"));
							dt.SetUnixTimestamp(timeEnd);
							sb.AppendC(UTF8STRC("<end>"));
							sptr = dt.ToString(sbuff, "yyyy-MM-dd");
							sb.AppendP(sbuff, sptr);
							sb.AppendC(UTF8STRC("T"));
							sptr = dt.ToString(sbuff, "HH:mm:ss");
							sb.AppendP(sbuff, sptr);
							sb.AppendC(UTF8STRC("Z</end></TimeSpan>"));
						}
						if (img->HasSrcAlpha())
						{
							sb.AppendC(UTF8STRC("<color>"));
							sb.AppendHex32(((UInt32)Double2Int32(img->GetSrcAlpha() * 255.0) << 24) | 0xffffff);
							sb.AppendC(UTF8STRC("</color>"));
						}
						if (img->HasZIndex())
						{
							sb.AppendC(UTF8STRC("<drawOrder>"));
							sb.AppendI32(img->GetZIndex());
							sb.AppendC(UTF8STRC("</drawOrder>"));
						}
						sb.AppendC(UTF8STRC("<Icon><href>"));
						///////////////////////////////////////////////////////
						sb.AppendOpt(img->GetSourceAddr());
						sb.AppendC(UTF8STRC("</href></Icon>"));
						sb.AppendC(UTF8STRC("<LatLonBox>"));

						bounds = img->GetBounds();
						if (needConv)
						{
							bounds.min = Math::CoordinateSystem::Convert3D(srcCsys, destCsys, Math::Vector3(bounds.min, defHeight)).GetXY();
							bounds.max = Math::CoordinateSystem::Convert3D(srcCsys, destCsys, Math::Vector3(bounds.max, defHeight)).GetXY();
						}
						sb.AppendC(UTF8STRC("<north>"));
						sb.AppendDouble(bounds.max.y);
						sb.AppendC(UTF8STRC("</north><south>"));
						sb.AppendDouble(bounds.min.y);
						sb.AppendC(UTF8STRC("</south>"));
						sb.AppendC(UTF8STRC("<east>"));
						sb.AppendDouble(bounds.max.x);
						sb.AppendC(UTF8STRC("</east><west>"));
						sb.AppendDouble(bounds.min.x);
						sb.AppendC(UTF8STRC("</west>"));
						sb.AppendC(UTF8STRC("</LatLonBox>"));
						if (img->HasZ())
						{
							sb.AppendC(UTF8STRC("<altitude>"));
							sb.AppendDouble(img->GetHeight());
							sb.AppendC(UTF8STRC("</altitude>"));
							sb.AppendC(UTF8STRC("<altitudeMode>clampToGround</altitudeMode>"));
						}

						sb.AppendC(UTF8STRC("</GroundOverlay>"));
						writer.WriteLine(sb.ToCString());
					}
				}
				vec.Delete();
			}
		}

		i++;
	}
	layer->EndGetObject(sess);
	layer->ReleaseNameArr(nameArr);

	destCsys.Delete();

	sptr = Text::StrConcatC(sbuff2, UTF8STRC("</Folder>"));
	sptr = Text::StrConcatC(sptr, UTF8STRC("</Document>"));
	sptr = Text::StrConcatC(sptr, UTF8STRC("</kml>"));
	writer.Write(CSTRP(sbuff2, sptr));
	return true;
}

UOSInt Exporter::KMLExporter::GetParamCnt()
{
	return 1;
}

Optional<IO::FileExporter::ParamData> Exporter::KMLExporter::CreateParam(NN<IO::ParsedObject> pobj)
{
	Int32 *retParam = MemAlloc(Int32, 1);
	*retParam = 0;
	return (ParamData*)retParam;
}

void Exporter::KMLExporter::DeleteParam(Optional<ParamData> param)
{
	NN<ParamData> para;
	if (param.SetTo(para))
	{
		MemFree(para.Ptr());
	}
}

Bool Exporter::KMLExporter::GetParamInfo(UOSInt index, NN<ParamInfo> info)
{
	if (index != 0)
		return false;
	info->name = CSTR("Default Height");
	info->paramType = IO::FileExporter::ParamType::INT32;
	info->allowNull = false;
	return true;
}

Bool Exporter::KMLExporter::SetParamInt32(Optional<ParamData> param, UOSInt index, Int32 val)
{
	NN<ParamData> para;
	if (index != 0 || !param.SetTo(para))
		return false;
	Int32 *iParam = (Int32*)para.Ptr();
	*iParam = val;
	return true;
}

Int32 Exporter::KMLExporter::GetParamInt32(Optional<ParamData> param, UOSInt index)
{
	NN<ParamData> para;
	if (index != 0 || !param.SetTo(para))
		return 0;
	return *(Int32*)para.Ptr();
}
