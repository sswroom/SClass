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
	if (pobj->GetParserType() != IO::ParsedObject::PT_MAP_LAYER_PARSER)
	{
		return IO::FileExporter::ST_NOT_SUPPORTED;
	}
	Map::IMapDrawLayer *layer = (Map::IMapDrawLayer *)pobj;
	if (layer->GetLayerType() == Map::DRAW_LAYER_POINT)
	{
		return IO::FileExporter::ST_NORMAL_STREAM;
	}
	else if (layer->GetLayerType() == Map::DRAW_LAYER_POLYLINE)
	{
		return IO::FileExporter::ST_NORMAL_STREAM;
	}
	else if (layer->GetLayerType() == Map::DRAW_LAYER_POINT3D)
	{
		return IO::FileExporter::ST_NORMAL_STREAM;
	}
	else if (layer->GetLayerType() == Map::DRAW_LAYER_POLYLINE3D)
	{
		return IO::FileExporter::ST_NORMAL_STREAM;
	}
	else if (layer->GetLayerType() == Map::DRAW_LAYER_POLYGON)
	{
		return IO::FileExporter::ST_NORMAL_STREAM;
	}
	else if (layer->GetLayerType() == Map::DRAW_LAYER_IMAGE && layer->GetObjectClass() != Map::IMapDrawLayer::OC_TILE_MAP_LAYER)
	{
		return IO::FileExporter::ST_NORMAL_STREAM;
	}
	return IO::FileExporter::ST_NOT_SUPPORTED;
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

void Exporter::KMLExporter::SetCodePage(Int32 codePage)
{
	this->codePage = codePage;
}

void Exporter::KMLExporter::SetEncFactory(Text::EncodingFactory *encFact)
{
	this->encFact = encFact;
}

Bool Exporter::KMLExporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
{
	if (pobj->GetParserType() != IO::ParsedObject::PT_MAP_LAYER_PARSER)
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
	Math::CoordinateSystem *destCsys = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::GeographicCoordinateSystem::GCST_WGS84);
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
	writer->Write(sbuff2, (UOSInt)(sptr - sbuff2));

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
			if (vec->GetVectorType() == Math::Vector2D::VT_POINT)
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
				writer->WriteLine(sbuff2, (UOSInt)(sptr - sbuff2));
			}
			else if (vec->GetVectorType() == Math::Vector2D::VT_POLYLINE)
			{
				UOSInt nPoints;
				Math::Polyline *pl = (Math::Polyline*)vec;
				if (layer->GetString(sbuff, sizeof(sbuff), nameArr, currId, nameCol) == 0)
				{
					Text::StrInt64(sbuff, currId);
				}

				sb.ClearStr();
				sb.Append((const UTF8Char*)"<Placemark><name>");
				sb.Append(sbuff);
				sb.Append((const UTF8Char*)"</name><LineString><coordinates>");

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

				sb.Append((const UTF8Char*)"</coordinates></LineString></Placemark>");
				writer->WriteLine(sb.ToString(), sb.GetLength());
			}
			else if (vec->GetVectorType() == Math::Vector2D::VT_POLYGON)
			{
				UOSInt nPoints;
				UOSInt nParts;
				Math::Polygon *pg = (Math::Polygon*)vec;
				if (layer->GetString(sbuff, sizeof(sbuff), nameArr, currId, nameCol) == 0)
				{
					Text::StrInt64(sbuff, currId);
				}

				sb.ClearStr();
				sb.Append((const UTF8Char*)"<Placemark>");
				sb.Append((const UTF8Char*)"<name>");
				sb.Append(sbuff);
				sb.Append((const UTF8Char*)"</name>");
				sb.Append((const UTF8Char*)"<Polygon>");
				sb.Append((const UTF8Char*)"<tessellate>1</tessellate>");
				sb.Append((const UTF8Char*)"<altitudeMode>relativeToGround</altitudeMode>");

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
						sb.Append((const UTF8Char*)"<outerBoundaryIs><LinearRing><coordinates>");
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
						sb.Append((const UTF8Char*)"</coordinates></LinearRing></outerBoundaryIs>");
					}
				}
				else
				{
					k = nPoints;
					l = nParts;
					while (l-- > 0)
					{
						sb.Append((const UTF8Char*)"<outerBoundaryIs><LinearRing><coordinates>");
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
						sb.Append((const UTF8Char*)"</coordinates></LinearRing></outerBoundaryIs>");
					}
				}

				sb.Append((const UTF8Char*)"</Polygon>");
				sb.Append((const UTF8Char*)"</Placemark>");
				writer->WriteLine(sb.ToString());
			}
			else if (vec->GetVectorType() == Math::Vector2D::VT_IMAGE)
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
					sb.Append((const UTF8Char*)"<ScreenOverlay>");
					sb.Append((const UTF8Char*)"<name>");
					sb.Append(sbuff);
					sb.Append((const UTF8Char*)"</name>");
					timeStart = img->GetTimeStart();
					timeEnd = img->GetTimeEnd();
					if (timeStart != 0 && timeEnd != 0)
					{
						Data::DateTime dt;
						dt.ToUTCTime();
						dt.SetUnixTimestamp(timeStart);
						sb.Append((const UTF8Char*)"<TimeStamp><when>");
						dt.ToString(sbuff, "yyyy-MM-dd");
						sb.Append(sbuff);
						sb.Append((const UTF8Char*)"T");
						dt.ToString(sbuff, "HH:mm");
						sb.Append(sbuff);
						sb.Append((const UTF8Char*)"Z</when></TimeStamp>");

						sb.Append((const UTF8Char*)"<TimeSpan><begin>");
						dt.ToString(sbuff, "yyyy-MM-dd");
						sb.Append(sbuff);
						sb.Append((const UTF8Char*)"T");
						dt.ToString(sbuff, "HH:mm:ss");
						sb.Append(sbuff);
						sb.Append((const UTF8Char*)"Z</begin>");
						dt.SetUnixTimestamp(timeEnd);
						sb.Append((const UTF8Char*)"<end>");
						dt.ToString(sbuff, "yyyy-MM-dd");
						sb.Append(sbuff);
						sb.Append((const UTF8Char*)"T");
						dt.ToString(sbuff, "HH:mm:ss");
						sb.Append(sbuff);
						sb.Append((const UTF8Char*)"Z</end></TimeSpan>");
					}
					if (img->HasSrcAlpha())
					{
						sb.Append((const UTF8Char*)"<color>");
						sb.AppendHex32(((UInt32)Math::Double2Int32(img->GetSrcAlpha() * 255.0) << 24) | 0xffffff);
						sb.Append((const UTF8Char*)"</color>");
					}
					if (img->HasZIndex())
					{
						sb.Append((const UTF8Char*)"<drawOrder>");
						sb.AppendI32(img->GetZIndex());
						sb.Append((const UTF8Char*)"</drawOrder>");
					}
					sb.Append((const UTF8Char*)"<Icon><href>");
					//////////////////////////////////////////////////////
					sb.Append(img->GetSourceAddr());
					sb.Append((const UTF8Char*)"</href></Icon>");

					img->GetBounds(&minX, &minY, &maxX, &maxY);
					sb.Append((const UTF8Char*)"<overlayXY x=\"");
					Text::SBAppendF64(&sb, maxX);
					sb.Append((const UTF8Char*)"\" y=\"");
					Text::SBAppendF64(&sb, maxY);
					sb.Append((const UTF8Char*)"\" xunits=\"fraction\" yunits=\"fraction\"/>");

					sb.Append((const UTF8Char*)"<screenXY x=\"");
					Text::SBAppendF64(&sb, minX);
					sb.Append((const UTF8Char*)"\" y=\"");
					Text::SBAppendF64(&sb, minY);
					sb.Append((const UTF8Char*)"\" xunits=\"fraction\" yunits=\"fraction\"/>");

					img->GetVectorSize(&maxX, &maxY);
					sb.Append((const UTF8Char*)"<size x=\"");
					Text::SBAppendF64(&sb, maxX);
					sb.Append((const UTF8Char*)"\" y=\"");
					Text::SBAppendF64(&sb, maxY);
					sb.Append((const UTF8Char*)"\" xunits=\"fraction\" yunits=\"fraction\"/>");

					sb.Append((const UTF8Char*)"</ScreenOverlay>");
					writer->WriteLine(sb.ToString());
				}
				else
				{
					sb.ClearStr();
					sb.Append((const UTF8Char*)"<GroundOverlay>");
					sb.Append((const UTF8Char*)"<name>");
					sb.Append(sbuff);
					sb.Append((const UTF8Char*)"</name>");
					timeStart = img->GetTimeStart();
					timeEnd = img->GetTimeEnd();
					if (timeStart != 0 && timeEnd != 0)
					{
						Data::DateTime dt;
						dt.ToUTCTime();
						dt.SetUnixTimestamp(timeStart);
						sb.Append((const UTF8Char*)"<TimeStamp><when>");
						dt.ToString(sbuff, "yyyy-MM-dd");
						sb.Append(sbuff);
						sb.Append((const UTF8Char*)"T");
						dt.ToString(sbuff, "HH:mm");
						sb.Append(sbuff);
						sb.Append((const UTF8Char*)"Z</when></TimeStamp>");

						sb.Append((const UTF8Char*)"<TimeSpan><begin>");
						dt.ToString(sbuff, "yyyy-MM-dd");
						sb.Append(sbuff);
						sb.Append((const UTF8Char*)"T");
						dt.ToString(sbuff, "HH:mm:ss");
						sb.Append(sbuff);
						sb.Append((const UTF8Char*)"Z</begin>");
						dt.SetUnixTimestamp(timeEnd);
						sb.Append((const UTF8Char*)"<end>");
						dt.ToString(sbuff, "yyyy-MM-dd");
						sb.Append(sbuff);
						sb.Append((const UTF8Char*)"T");
						dt.ToString(sbuff, "HH:mm:ss");
						sb.Append(sbuff);
						sb.Append((const UTF8Char*)"Z</end></TimeSpan>");
					}
					if (img->HasSrcAlpha())
					{
						sb.Append((const UTF8Char*)"<color>");
						sb.AppendHex32(((UInt32)Math::Double2Int32(img->GetSrcAlpha() * 255.0) << 24) | 0xffffff);
						sb.Append((const UTF8Char*)"</color>");
					}
					if (img->HasZIndex())
					{
						sb.Append((const UTF8Char*)"<drawOrder>");
						sb.AppendI32(img->GetZIndex());
						sb.Append((const UTF8Char*)"</drawOrder>");
					}
					sb.Append((const UTF8Char*)"<Icon><href>");
					///////////////////////////////////////////////////////
					sb.Append(img->GetSourceAddr());
					sb.Append((const UTF8Char*)"</href></Icon>");
					sb.Append((const UTF8Char*)"<LatLonBox>");

					img->GetBounds(&minX, &minY, &maxX, &maxY);
					if (needConv)
					{
						Double z;
						Math::CoordinateSystem::ConvertXYZ(srcCsys, destCsys, minX, minY, defHeight, &minX, &minY, &z);
						Math::CoordinateSystem::ConvertXYZ(srcCsys, destCsys, maxX, maxY, defHeight, &maxX, &maxY, &z);
					}
					sb.Append((const UTF8Char*)"<north>");
					Text::SBAppendF64(&sb, maxY);
					sb.Append((const UTF8Char*)"</north><south>");
					Text::SBAppendF64(&sb, minY);
					sb.Append((const UTF8Char*)"</south>");
					sb.Append((const UTF8Char*)"<east>");
					Text::SBAppendF64(&sb, maxX);
					sb.Append((const UTF8Char*)"</east><west>");
					Text::SBAppendF64(&sb, minX);
					sb.Append((const UTF8Char*)"</west>");
					sb.Append((const UTF8Char*)"</LatLonBox>");
					if (img->Support3D())
					{
						sb.Append((const UTF8Char*)"<altitude>");
						Text::SBAppendF64(&sb, img->GetHeight());
						sb.Append((const UTF8Char*)"</altitude>");
						sb.Append((const UTF8Char*)"<altitudeMode>clampToGround</altitudeMode>");
					}

					sb.Append((const UTF8Char*)"</GroundOverlay>");
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
	writer->Write(sbuff2, (UOSInt)(sptr - sbuff2));

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
	info->paramType = IO::FileExporter::PT_INT32;
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
