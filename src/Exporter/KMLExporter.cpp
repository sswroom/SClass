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
}

Exporter::KMLExporter::~KMLExporter()
{
}

Int32 Exporter::KMLExporter::GetName()
{
	return *(Int32*)"KMLE";
}

IO::FileExporter::SupportType Exporter::KMLExporter::IsObjectSupported(NotNullPtr<IO::ParsedObject> pobj)
{
	if (pobj->GetParserType() != IO::ParserType::MapLayer)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	NotNullPtr<Map::MapDrawLayer> layer = NotNullPtr<Map::MapDrawLayer>::ConvertFrom(pobj);
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

Bool Exporter::KMLExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
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

void Exporter::KMLExporter::SetEncFactory(Text::EncodingFactory *encFact)
{
	this->encFact = encFact;
}

Bool Exporter::KMLExporter::ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CStringNN fileName, NotNullPtr<IO::ParsedObject> pobj, void *param)
{
	if (pobj->GetParserType() != IO::ParserType::MapLayer)
	{
		return false;
	}
	Double defHeight = 10.0;
	UTF8Char sbuff[256];
	UTF8Char sbuff2[512];
	UTF8Char *sptr;
	NotNullPtr<Map::MapDrawLayer> layer = NotNullPtr<Map::MapDrawLayer>::ConvertFrom(pobj);
	UOSInt nameCol = layer->GetNameCol();

	UOSInt i;
	UOSInt j;
	UOSInt k;
	Int64 currId;
	Int64 lastId;
	Map::NameArray *nameArr;
	Map::GetObjectSess *sess;
	Math::Geometry::Vector2D *vec;
	Text::Encoding enc(this->codePage);
	Text::StringBuilderUTF8 sb;
	NotNullPtr<Text::String> s;
	NotNullPtr<Math::CoordinateSystem> srcCsys = layer->GetCoordinateSystem();
	NotNullPtr<Math::CoordinateSystem> destCsys = Math::CoordinateSystemManager::CreateDefaultCsys();
	Bool needConv = false;
	if (!destCsys->Equals(srcCsys))
	{
		needConv = true;
	}

	if (param)
	{
		defHeight = *(Int32*)param;
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
	sptr = Text::StrConcatC(sptr, UTF8STRC("<name>Points</name>"));
	sptr = Text::StrConcatC(sptr, UTF8STRC("<open>1</open>"));
	writer.WriteStrC(sbuff2, (UOSInt)(sptr - sbuff2));

	writer.WriteStrC(UTF8STRC("<Style id=\"lineLabel\"><LineStyle><gx:labelVisibility>1</gx:labelVisibility></LineStyle></Style>"));

	Data::ArrayListInt64 ids;
	layer->GetAllObjectIds(ids, &nameArr);

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
			vec = layer->GetNewVectorById(sess, currId);
			if (vec == 0)
			{

			}
			else
			{
				Math::Geometry::Vector2D::VectorType vecType = vec->GetVectorType();
				if (vecType == Math::Geometry::Vector2D::VectorType::Point)
				{
					Math::Geometry::Point *pt = (Math::Geometry::Point*)vec;
					Math::Coord2DDbl coord = pt->GetCenter();
					Double z;
					if (pt->HasZ())
					{
						z = ((Math::Geometry::PointZ*)pt)->GetZ();
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
					writer.WriteLineC(sbuff2, (UOSInt)(sptr - sbuff2));
				}
				else if (vecType == Math::Geometry::Vector2D::VectorType::LineString)
				{
					UOSInt nPoints;
					Math::Geometry::LineString *pl = (Math::Geometry::LineString*)vec;
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

					Math::Coord2DDbl *points = pl->GetPointList(nPoints);
					if (needConv)
					{
						Math::Vector3 v;
						if (vec->HasZ())
						{
							Double *alts = pl->GetZList(nPoints);
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
						if (vec->HasZ())
						{
							Double *alts = pl->GetZList(nPoints);
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
					writer.WriteLineC(sb.ToString(), sb.GetLength());					
				}
				else if (vecType == Math::Geometry::Vector2D::VectorType::Polyline)
				{
					UOSInt nPoints;
					Math::Geometry::Polyline *pl = (Math::Geometry::Polyline*)vec;
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

					NotNullPtr<Math::Geometry::LineString> lineString;
					Data::ArrayIterator<NotNullPtr<Math::Geometry::LineString>> it = pl->Iterator();
					while (it.HasNext())
					{
						lineString = it.Next();
						Math::Coord2DDbl *points = lineString->GetPointList(nPoints);
						if (needConv)
						{
							Math::Vector3 v;
							if (lineString->HasZ())
							{
								Double *alts = lineString->GetZList(nPoints);
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
							if (lineString->HasZ())
							{
								Double *alts = lineString->GetZList(nPoints);
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
						i++;
					}
					sb.AppendC(UTF8STRC("</coordinates></LineString></Placemark>"));
					writer.WriteLineC(sb.ToString(), sb.GetLength());
				}
				else if (vecType == Math::Geometry::Vector2D::VectorType::Polygon)
				{
					UOSInt nPoints;
					Math::Geometry::Polygon *pg = (Math::Geometry::Polygon*)vec;
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

					Math::Coord2DDbl *points;
					NotNullPtr<Math::Geometry::LinearRing> lr;

					if (needConv)
					{
						Math::Vector3 v;
						Data::ArrayIterator<NotNullPtr<Math::Geometry::LinearRing>> it = pg->Iterator();
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
						Data::ArrayIterator<NotNullPtr<Math::Geometry::LinearRing>> it = pg->Iterator();
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
					writer.WriteLineC(sb.ToString(), sb.GetLength());
				}
				else if (vecType == Math::Geometry::Vector2D::VectorType::Image)
				{
					Math::Geometry::VectorImage *img = (Math::Geometry::VectorImage*)vec;
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
						sb.Append(img->GetSourceAddr());
						sb.AppendC(UTF8STRC("</href></Icon>"));

						bounds = img->GetBounds();
						sb.AppendC(UTF8STRC("<overlayXY x=\""));
						sb.AppendDouble(bounds.br.x);
						sb.AppendC(UTF8STRC("\" y=\""));
						sb.AppendDouble(bounds.br.y);
						sb.AppendC(UTF8STRC("\" xunits=\"fraction\" yunits=\"fraction\"/>"));

						sb.AppendC(UTF8STRC("<screenXY x=\""));
						sb.AppendDouble(bounds.tl.x);
						sb.AppendC(UTF8STRC("\" y=\""));
						sb.AppendDouble(bounds.tl.y);
						sb.AppendC(UTF8STRC("\" xunits=\"fraction\" yunits=\"fraction\"/>"));

						img->GetVectorSize(&bounds.tl.x, &bounds.tl.x);
						sb.AppendC(UTF8STRC("<size x=\""));
						sb.AppendDouble(bounds.tl.x);
						sb.AppendC(UTF8STRC("\" y=\""));
						sb.AppendDouble(bounds.tl.y);
						sb.AppendC(UTF8STRC("\" xunits=\"fraction\" yunits=\"fraction\"/>"));

						sb.AppendC(UTF8STRC("</ScreenOverlay>"));
						writer.WriteLineC(sb.ToString(), sb.GetLength());
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
						sb.Append(img->GetSourceAddr());
						sb.AppendC(UTF8STRC("</href></Icon>"));
						sb.AppendC(UTF8STRC("<LatLonBox>"));

						bounds = img->GetBounds();
						if (needConv)
						{
							bounds.tl = Math::CoordinateSystem::Convert3D(srcCsys, destCsys, Math::Vector3(bounds.tl, defHeight)).GetXY();
							bounds.br = Math::CoordinateSystem::Convert3D(srcCsys, destCsys, Math::Vector3(bounds.br, defHeight)).GetXY();
						}
						sb.AppendC(UTF8STRC("<north>"));
						sb.AppendDouble(bounds.br.y);
						sb.AppendC(UTF8STRC("</north><south>"));
						sb.AppendDouble(bounds.tl.y);
						sb.AppendC(UTF8STRC("</south>"));
						sb.AppendC(UTF8STRC("<east>"));
						sb.AppendDouble(bounds.br.x);
						sb.AppendC(UTF8STRC("</east><west>"));
						sb.AppendDouble(bounds.tl.x);
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
						writer.WriteLineC(sb.ToString(), sb.GetLength());
					}
				}
			}
			SDEL_CLASS(vec);
		}

		i++;
	}
	layer->EndGetObject(sess);
	layer->ReleaseNameArr(nameArr);

	destCsys.Delete();

	sptr = Text::StrConcatC(sbuff2, UTF8STRC("</Folder>"));
	sptr = Text::StrConcatC(sptr, UTF8STRC("</Document>"));
	sptr = Text::StrConcatC(sptr, UTF8STRC("</kml>"));
	writer.WriteStrC(sbuff2, (UOSInt)(sptr - sbuff2));
	return true;
}

UOSInt Exporter::KMLExporter::GetParamCnt()
{
	return 1;
}

void *Exporter::KMLExporter::CreateParam(NotNullPtr<IO::ParsedObject> pobj)
{
	Int32 *retParam = MemAlloc(Int32, 1);
	*retParam = 0;
	return retParam;
}

void Exporter::KMLExporter::DeleteParam(void *param)
{
	MemFree(param);
}

Bool Exporter::KMLExporter::GetParamInfo(UOSInt index, NotNullPtr<ParamInfo> info)
{
	if (index != 0)
		return false;
	info->name = CSTR("Default Height");
	info->paramType = IO::FileExporter::ParamType::INT32;
	info->allowNull = false;
	return true;
}

Bool Exporter::KMLExporter::SetParamInt32(void *param, UOSInt index, Int32 val)
{
	if (index != 0)
		return false;
	Int32 *iParam = (Int32*)param;
	*iParam = val;
	return true;
}

Int32 Exporter::KMLExporter::GetParamInt32(void *param, UOSInt index)
{
	if (index != 0)
		return 0;
	return *(Int32*)param;
}
