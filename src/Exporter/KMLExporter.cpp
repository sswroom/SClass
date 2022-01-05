#include "Stdafx.h"
#include "MyMemory.h"
#include "Exporter/KMLExporter.h"
#include "IO/BufferedOutputStream.h"
#include "IO/StreamWriter.h"
#include "Map/IMapDrawLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math.h"
#include "Math/Point3D.h"
#include "Math/Polyline3D.h"
#include "Math/Polygon.h"
#include "Math/ProjectedCoordinateSystem.h"
#include "Math/VectorImage.h"
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

IO::FileExporter::SupportType Exporter::KMLExporter::IsObjectSupported(IO::ParsedObject *pobj)
{
	if (pobj->GetParserType() != IO::ParserType::MapLayer)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	Map::IMapDrawLayer *layer = (Map::IMapDrawLayer *)pobj;
	if (layer->GetLayerType() == Map::DRAW_LAYER_POINT)
	{
		return IO::FileExporter::SupportType::NormalStream;
	}
	else if (layer->GetLayerType() == Map::DRAW_LAYER_POLYLINE)
	{
		return IO::FileExporter::SupportType::NormalStream;
	}
	else if (layer->GetLayerType() == Map::DRAW_LAYER_POINT3D)
	{
		return IO::FileExporter::SupportType::NormalStream;
	}
	else if (layer->GetLayerType() == Map::DRAW_LAYER_POLYLINE3D)
	{
		return IO::FileExporter::SupportType::NormalStream;
	}
	else if (layer->GetLayerType() == Map::DRAW_LAYER_POLYGON)
	{
		return IO::FileExporter::SupportType::NormalStream;
	}
	else if (layer->GetLayerType() == Map::DRAW_LAYER_IMAGE && layer->GetObjectClass() != Map::IMapDrawLayer::OC_TILE_MAP_LAYER)
	{
		return IO::FileExporter::SupportType::NormalStream;
	}
	return IO::FileExporter::SupportType::NotSupported;
}

Bool Exporter::KMLExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcat(nameBuff, (const UTF8Char*)"KML file");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.kml");
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

Bool Exporter::KMLExporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
{
	if (pobj->GetParserType() != IO::ParserType::MapLayer)
	{
		return false;
	}
	Double defHeight = 10.0;
	UTF8Char sbuff[256];
	UTF8Char sbuff2[512];
	UTF8Char *sptr;
	Map::IMapDrawLayer *layer = (Map::IMapDrawLayer *)pobj;
	UOSInt nameCol = layer->GetNameCol();

	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	Int64 currId;
	Int64 lastId;
	IO::BufferedOutputStream *cstm;
	IO::StreamWriter *writer;
	Data::ArrayListInt64 *ids;
	void *nameArr;
	void *sess;
	Math::Vector2D *vec;
	Text::Encoding enc(this->codePage);
	Text::StringBuilderUTF8 sb;
	Math::CoordinateSystem *srcCsys = layer->GetCoordinateSystem();
	Math::CoordinateSystem *destCsys = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84);
	Bool needConv = false;
	if (srcCsys && !destCsys->Equals(srcCsys))
	{
		needConv = true;
	}

	if (param)
	{
		defHeight = *(Int32*)param;
	}
	NEW_CLASS(cstm, IO::BufferedOutputStream(stm, 65536));
	NEW_CLASS(writer, IO::StreamWriter(cstm, &enc));

	sptr = Text::StrConcat(sbuff2, (const UTF8Char*)"<?xml version=\"1.0\" encoding=\"");
	sptr = Text::EncodingFactory::GetInternetName(sptr, this->codePage);
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"\"?>");
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"<kml xmlns=\"http://www.opengis.net/kml/2.2\" xmlns:gx=\"http://www.google.com/kml/ext/2.2\" xmlns:kml=\"http://www.opengis.net/kml/2.2\" xmlns:atom=\"http://www.w3.org/2005/Atom\">");
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"<Document>");
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"<Folder>");
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"<name>Points</name>");
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"<open>1</open>");
	writer->WriteStrC(sbuff2, (UOSInt)(sptr - sbuff2));

	NEW_CLASS(ids, Data::ArrayListInt64());
	layer->GetAllObjectIds(ids, &nameArr);

	sess = layer->BeginGetObject();
	lastId = -1;
	i = 0;
	j = ids->GetCount();
	while (i < j)
	{
		currId = ids->GetItem(i);
		if (currId != lastId)
		{
			lastId = currId;

			vec = layer->GetVectorById(sess, currId);
			if (vec->GetVectorType() == Math::Vector2D::VectorType::Point)
			{
				Double x;
				Double y;
				Double z;
				if (vec->Support3D())
				{
					Math::Point3D *pt = (Math::Point3D*)vec;
					pt->GetCenter3D(&x, &y, &z);
				}
				else
				{
					Math::Point *pt = (Math::Point*)vec;
					pt->GetCenter(&x, &y);
					z = 0;
				}

				if (layer->GetString(sbuff, sizeof(sbuff), nameArr, currId, nameCol) == 0)
				{
					Text::StrInt64(sbuff, currId);
				}
				if (needConv)
				{
					Math::CoordinateSystem::ConvertXYZ(srcCsys, destCsys, x, y, z, &x, &y, &z);
				}

				sptr = Text::StrConcat(sbuff2, (const UTF8Char*)"<Placemark>");
				sptr = Text::StrConcat(sptr, (const UTF8Char*)"<name>");
				sptr = Text::XML::ToXMLText(sptr, sbuff);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)"</name>");
				sptr = Text::StrConcat(sptr, (const UTF8Char*)"<Point><coordinates>");
				sptr = Text::StrDouble(sptr, x);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)",");
				sptr = Text::StrDouble(sptr, y);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)",");
				sptr = Text::StrDouble(sptr, z);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)"</coordinates></Point>");
				sptr = Text::StrConcat(sptr, (const UTF8Char*)"</Placemark>");
				writer->WriteLineC(sbuff2, (UOSInt)(sptr - sbuff2));
			}
			else if (vec->GetVectorType() == Math::Vector2D::VectorType::Polyline)
			{
				UOSInt nPoints;
				Math::Polyline *pl = (Math::Polyline*)vec;
				if (layer->GetString(sbuff, sizeof(sbuff), nameArr, currId, nameCol) == 0)
				{
					Text::StrInt64(sbuff, currId);
				}

				sb.ClearStr();
				sb.AppendC(UTF8STRC("<Placemark><name>"));
				sb.Append(sbuff);
				sb.AppendC(UTF8STRC("</name><LineString><coordinates>"));

				Double *points = pl->GetPointList(&nPoints);
				if (needConv)
				{
					Double x;
					Double y;
					Double z;
					if (vec->Support3D())
					{
						Double *alts = ((Math::Polyline3D*)pl)->GetAltitudeList(&nPoints);
						k = 0;
						while (k < nPoints)
						{
							Math::CoordinateSystem::ConvertXYZ(srcCsys, destCsys, points[k << 1], points[(k << 1) + 1], alts[k], &x, &y, &z);

							sptr = sbuff2;
							sptr = Text::StrDouble(sptr, x);
							*sptr++ = ',';
							sptr = Text::StrDouble(sptr, y);
							*sptr++ = ',';
							sptr = Text::StrDouble(sptr, z);
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
							Math::CoordinateSystem::ConvertXYZ(srcCsys, destCsys, points[k << 1], points[(k << 1) + 1], defHeight, &x, &y, &z);
							sptr = sbuff2;
							sptr = Text::StrDouble(sptr, x);
							*sptr++ = ',';
							sptr = Text::StrDouble(sptr, y);
							*sptr++ = ',';
							sptr = Text::StrDouble(sptr, z);
							*sptr++ = ' ';
							*sptr = 0;

							sb.AppendC(sbuff2, (UOSInt)(sptr - sbuff2));
							k++;
						}
					}
				}
				else
				{
					if (vec->Support3D())
					{
						Double *alts = ((Math::Polyline3D*)pl)->GetAltitudeList(&nPoints);
						k = 0;
						while (k < nPoints)
						{
							sptr = sbuff2;
							sptr = Text::StrDouble(sptr, points[k << 1]);
							*sptr++ = ',';
							sptr = Text::StrDouble(sptr, points[(k << 1) + 1]);
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
							sptr = Text::StrDouble(sptr, points[k << 1]);
							*sptr++ = ',';
							sptr = Text::StrDouble(sptr, points[(k << 1) + 1]);
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
				writer->WriteLineC(sb.ToString(), sb.GetLength());
			}
			else if (vec->GetVectorType() == Math::Vector2D::VectorType::Polygon)
			{
				UOSInt nPoints;
				UOSInt nParts;
				Math::Polygon *pg = (Math::Polygon*)vec;
				if (layer->GetString(sbuff, sizeof(sbuff), nameArr, currId, nameCol) == 0)
				{
					Text::StrInt64(sbuff, currId);
				}

				sb.ClearStr();
				sb.AppendC(UTF8STRC("<Placemark>"));
				sb.AppendC(UTF8STRC("<name>"));
				sb.Append(sbuff);
				sb.AppendC(UTF8STRC("</name>"));
				sb.AppendC(UTF8STRC("<Polygon>"));
				sb.AppendC(UTF8STRC("<tessellate>1</tessellate>"));
				sb.AppendC(UTF8STRC("<altitudeMode>relativeToGround</altitudeMode>"));

				Double *points = pg->GetPointList(&nPoints);
				UInt32 *ptOfsts = pg->GetPtOfstList(&nParts);

				if (needConv)
				{
					Double x;
					Double y;
					Double z;
					k = nPoints;
					l = nParts;
					while (l-- > 0)
					{
						sb.AppendC(UTF8STRC("<outerBoundaryIs><LinearRing><coordinates>"));
						while (k-- > ptOfsts[l])
						{
							Math::CoordinateSystem::ConvertXYZ(srcCsys, destCsys, points[k << 1], points[(k << 1) + 1], defHeight, &x, &y, &z);
							sptr = Text::StrDouble(sbuff2, x);
							sptr = Text::StrConcat(sptr, (const UTF8Char*)",");
							sptr = Text::StrDouble(sptr, y);
							sptr = Text::StrConcat(sptr, (const UTF8Char*)",");
							sptr = Text::StrDouble(sptr, z);
							sptr = Text::StrConcat(sptr, (const UTF8Char*)" ");
							sb.AppendC(sbuff2, (UOSInt)(sptr - sbuff2));
						}
						k++;
						sb.AppendC(UTF8STRC("</coordinates></LinearRing></outerBoundaryIs>"));
					}
				}
				else
				{
					k = nPoints;
					l = nParts;
					while (l-- > 0)
					{
						sb.AppendC(UTF8STRC("<outerBoundaryIs><LinearRing><coordinates>"));
						while (k-- > ptOfsts[l])
						{
							sptr = Text::StrDouble(sbuff2, points[k << 1]);
							sptr = Text::StrConcat(sptr, (const UTF8Char*)",");
							sptr = Text::StrDouble(sptr, points[(k << 1) + 1]);
							sptr = Text::StrConcat(sptr, (const UTF8Char*)",");
							sptr = Text::StrDouble(sptr, defHeight);
							sptr = Text::StrConcat(sptr, (const UTF8Char*)" ");
							sb.AppendC(sbuff2, (UOSInt)(sptr - sbuff2));
						}
						k++;
						sb.AppendC(UTF8STRC("</coordinates></LinearRing></outerBoundaryIs>"));
					}
				}

				sb.AppendC(UTF8STRC("</Polygon>"));
				sb.AppendC(UTF8STRC("</Placemark>"));
				writer->WriteLine(sb.ToString());
			}
			else if (vec->GetVectorType() == Math::Vector2D::VectorType::Image)
			{
				Math::VectorImage *img = (Math::VectorImage*)vec;
				if (layer->GetString(sbuff, sizeof(sbuff), nameArr, currId, nameCol) == 0)
				{
					Text::StrInt64(sbuff, currId);
				}

				Double minX;
				Double minY;
				Double maxX;
				Double maxY;
				Int64 timeStart;
				Int64 timeEnd;
				if (img->IsScnCoord())
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("<ScreenOverlay>"));
					sb.AppendC(UTF8STRC("<name>"));
					sb.Append(sbuff);
					sb.AppendC(UTF8STRC("</name>"));
					timeStart = img->GetTimeStart();
					timeEnd = img->GetTimeEnd();
					if (timeStart != 0 && timeEnd != 0)
					{
						Data::DateTime dt;
						dt.ToUTCTime();
						dt.SetUnixTimestamp(timeStart);
						sb.AppendC(UTF8STRC("<TimeStamp><when>"));
						dt.ToString(sbuff, "yyyy-MM-dd");
						sb.Append(sbuff);
						sb.AppendC(UTF8STRC("T"));
						dt.ToString(sbuff, "HH:mm");
						sb.Append(sbuff);
						sb.AppendC(UTF8STRC("Z</when></TimeStamp>"));

						sb.AppendC(UTF8STRC("<TimeSpan><begin>"));
						dt.ToString(sbuff, "yyyy-MM-dd");
						sb.Append(sbuff);
						sb.AppendC(UTF8STRC("T"));
						dt.ToString(sbuff, "HH:mm:ss");
						sb.Append(sbuff);
						sb.AppendC(UTF8STRC("Z</begin>"));
						dt.SetUnixTimestamp(timeEnd);
						sb.AppendC(UTF8STRC("<end>"));
						dt.ToString(sbuff, "yyyy-MM-dd");
						sb.Append(sbuff);
						sb.AppendC(UTF8STRC("T"));
						dt.ToString(sbuff, "HH:mm:ss");
						sb.Append(sbuff);
						sb.AppendC(UTF8STRC("Z</end></TimeSpan>"));
					}
					if (img->HasSrcAlpha())
					{
						sb.AppendC(UTF8STRC("<color>"));
						sb.AppendHex32(((UInt32)Math::Double2Int32(img->GetSrcAlpha() * 255.0) << 24) | 0xffffff);
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

					img->GetBounds(&minX, &minY, &maxX, &maxY);
					sb.AppendC(UTF8STRC("<overlayXY x=\""));
					Text::SBAppendF64(&sb, maxX);
					sb.AppendC(UTF8STRC("\" y=\""));
					Text::SBAppendF64(&sb, maxY);
					sb.AppendC(UTF8STRC("\" xunits=\"fraction\" yunits=\"fraction\"/>"));

					sb.AppendC(UTF8STRC("<screenXY x=\""));
					Text::SBAppendF64(&sb, minX);
					sb.AppendC(UTF8STRC("\" y=\""));
					Text::SBAppendF64(&sb, minY);
					sb.AppendC(UTF8STRC("\" xunits=\"fraction\" yunits=\"fraction\"/>"));

					img->GetVectorSize(&maxX, &maxY);
					sb.AppendC(UTF8STRC("<size x=\""));
					Text::SBAppendF64(&sb, maxX);
					sb.AppendC(UTF8STRC("\" y=\""));
					Text::SBAppendF64(&sb, maxY);
					sb.AppendC(UTF8STRC("\" xunits=\"fraction\" yunits=\"fraction\"/>"));

					sb.AppendC(UTF8STRC("</ScreenOverlay>"));
					writer->WriteLine(sb.ToString());
				}
				else
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("<GroundOverlay>"));
					sb.AppendC(UTF8STRC("<name>"));
					sb.Append(sbuff);
					sb.AppendC(UTF8STRC("</name>"));
					timeStart = img->GetTimeStart();
					timeEnd = img->GetTimeEnd();
					if (timeStart != 0 && timeEnd != 0)
					{
						Data::DateTime dt;
						dt.ToUTCTime();
						dt.SetUnixTimestamp(timeStart);
						sb.AppendC(UTF8STRC("<TimeStamp><when>"));
						dt.ToString(sbuff, "yyyy-MM-dd");
						sb.Append(sbuff);
						sb.AppendC(UTF8STRC("T"));
						dt.ToString(sbuff, "HH:mm");
						sb.Append(sbuff);
						sb.AppendC(UTF8STRC("Z</when></TimeStamp>"));

						sb.AppendC(UTF8STRC("<TimeSpan><begin>"));
						dt.ToString(sbuff, "yyyy-MM-dd");
						sb.Append(sbuff);
						sb.AppendC(UTF8STRC("T"));
						dt.ToString(sbuff, "HH:mm:ss");
						sb.Append(sbuff);
						sb.AppendC(UTF8STRC("Z</begin>"));
						dt.SetUnixTimestamp(timeEnd);
						sb.AppendC(UTF8STRC("<end>"));
						dt.ToString(sbuff, "yyyy-MM-dd");
						sb.Append(sbuff);
						sb.AppendC(UTF8STRC("T"));
						dt.ToString(sbuff, "HH:mm:ss");
						sb.Append(sbuff);
						sb.AppendC(UTF8STRC("Z</end></TimeSpan>"));
					}
					if (img->HasSrcAlpha())
					{
						sb.AppendC(UTF8STRC("<color>"));
						sb.AppendHex32(((UInt32)Math::Double2Int32(img->GetSrcAlpha() * 255.0) << 24) | 0xffffff);
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

					img->GetBounds(&minX, &minY, &maxX, &maxY);
					if (needConv)
					{
						Double z;
						Math::CoordinateSystem::ConvertXYZ(srcCsys, destCsys, minX, minY, defHeight, &minX, &minY, &z);
						Math::CoordinateSystem::ConvertXYZ(srcCsys, destCsys, maxX, maxY, defHeight, &maxX, &maxY, &z);
					}
					sb.AppendC(UTF8STRC("<north>"));
					Text::SBAppendF64(&sb, maxY);
					sb.AppendC(UTF8STRC("</north><south>"));
					Text::SBAppendF64(&sb, minY);
					sb.AppendC(UTF8STRC("</south>"));
					sb.AppendC(UTF8STRC("<east>"));
					Text::SBAppendF64(&sb, maxX);
					sb.AppendC(UTF8STRC("</east><west>"));
					Text::SBAppendF64(&sb, minX);
					sb.AppendC(UTF8STRC("</west>"));
					sb.AppendC(UTF8STRC("</LatLonBox>"));
					if (img->Support3D())
					{
						sb.AppendC(UTF8STRC("<altitude>"));
						Text::SBAppendF64(&sb, img->GetHeight());
						sb.AppendC(UTF8STRC("</altitude>"));
						sb.AppendC(UTF8STRC("<altitudeMode>clampToGround</altitudeMode>"));
					}

					sb.AppendC(UTF8STRC("</GroundOverlay>"));
					writer->WriteLine(sb.ToString());
				}
			}
			DEL_CLASS(vec);
		}

		i++;
	}
	layer->EndGetObject(sess);

	DEL_CLASS(destCsys);
	DEL_CLASS(ids);

	sptr = Text::StrConcat(sbuff2, (const UTF8Char*)"</Folder>");
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"</Document>");
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"</kml>");
	writer->WriteStrC(sbuff2, (UOSInt)(sptr - sbuff2));

	DEL_CLASS(writer);
	DEL_CLASS(cstm);
	return true;
}

UOSInt Exporter::KMLExporter::GetParamCnt()
{
	return 1;
}

void *Exporter::KMLExporter::CreateParam(IO::ParsedObject *pobj)
{
	Int32 *retParam = MemAlloc(Int32, 1);
	*retParam = 0;
	return retParam;
}

void Exporter::KMLExporter::DeleteParam(void *param)
{
	MemFree(param);
}

Bool Exporter::KMLExporter::GetParamInfo(UOSInt index, ParamInfo *info)
{
	if (index != 0)
		return false;
	info->name = (const UTF8Char *)"Default Height";
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
