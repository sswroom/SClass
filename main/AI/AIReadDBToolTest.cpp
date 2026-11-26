#include "Stdafx.h"
#include "AI/AIReadDBTool.h"
#include "Core/Core.h"
#include "Data/ChartPlotter.h"
#include "DB/ColDef.h"
#include "DB/SQLiteFile.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Media/DrawEngineFactory.h"
#include "Net/MIME.h"
#include "Math/Geometry/MultiPoint.h"
#include "Math/Geometry/MultiPolygon.h"
#include "Math/Geometry/GeometryCollection.h"
#include "Math/Geometry/Point.h"
#include "Math/Geometry/Polyline.h"
#include "Text/JSON.h"

#define CHECK(expr) if (!(expr)) { printf("AIReadDBToolTest: failed at line %d\n", __LINE__); drawEngine.Delete(); return 1; }

class DateReader : public DB::DBReader
{
private:
	NN<DB::DBReader> reader;
public:
	DateReader(NN<DB::DBReader> reader) : reader(reader) {}
	NN<DB::DBReader> GetReader() { return this->reader; }
	virtual Text::CStringNN GetClassName() const { return CSTR("DateReader"); }
	virtual Bool ReadNext() { return this->reader->ReadNext(); }
	virtual UIntOS ColCount() { return this->reader->ColCount(); }
	virtual IntOS GetRowChanged() { return this->reader->GetRowChanged(); }
	virtual Int32 GetInt32(UIntOS col) { return this->reader->GetInt32(col); }
	virtual Int64 GetInt64(UIntOS col) { return this->reader->GetInt64(col); }
	virtual UnsafeArrayOpt<WChar> GetStr(UIntOS col, UnsafeArray<WChar> buff) { return this->reader->GetStr(col, buff); }
	virtual Bool GetStr(UIntOS col, NN<Text::StringBuilderUTF8> sb) { return this->reader->GetStr(col, sb); }
	virtual Optional<Text::String> GetNewStr(UIntOS col) { return this->reader->GetNewStr(col); }
	virtual UnsafeArrayOpt<UTF8Char> GetStr(UIntOS col, UnsafeArray<UTF8Char> buff, UIntOS size) { return this->reader->GetStr(col, buff, size); }
	virtual Data::Timestamp GetTimestamp(UIntOS col) { return this->reader->GetTimestamp(col); }
	virtual Double GetDblOrNAN(UIntOS col) { return this->reader->GetDblOrNAN(col); }
	virtual Bool GetBool(UIntOS col) { return this->reader->GetBool(col); }
	virtual UIntOS GetBinarySize(UIntOS col) { return this->reader->GetBinarySize(col); }
	virtual UIntOS GetBinary(UIntOS col, UnsafeArray<UInt8> buff) { return this->reader->GetBinary(col, buff); }
	virtual Optional<Math::Geometry::Vector2D> GetVector(UIntOS col) { return this->reader->GetVector(col); }
	virtual Bool GetUUID(UIntOS col, NN<Data::UUID> uuid) { return this->reader->GetUUID(col, uuid); }
	virtual Bool IsNull(UIntOS col) { return this->reader->IsNull(col); }
	virtual UnsafeArrayOpt<UTF8Char> GetName(UIntOS col, UnsafeArray<UTF8Char> buff) { return this->reader->GetName(col, buff); }
	virtual DB::DBUtil::ColType GetColType(UIntOS col, OptOut<UIntOS> size) { return col == 0 ? DB::DBUtil::CT_DateTime : this->reader->GetColType(col, size); }
	virtual Bool GetColDef(UIntOS col, NN<DB::ColDef> def) { return this->reader->GetColDef(col, def); }
};

class DateDB : public DB::SQLiteFile
{
public:
	DateDB() : DB::SQLiteFile(CSTR(":memory:")) {}
	virtual Optional<DB::DBReader> ExecuteReader(Text::CStringNN sql)
	{
		NN<DB::DBReader> reader;
		if (!DB::SQLiteFile::ExecuteReader(sql).SetTo(reader))
			return nullptr;
		NN<DateReader> wrapped;
		NEW_CLASSNN(wrapped, DateReader(reader));
		return wrapped;
	}
	virtual void CloseReader(NN<DB::DBReader> reader)
	{
		NN<DateReader> wrapped = NN<DateReader>::ConvertFrom(reader);
		DB::SQLiteFile::CloseReader(wrapped->GetReader());
		wrapped.Delete();
	}
};

class GeometryReader : public DateReader
{
public:
	GeometryReader(NN<DB::DBReader> reader) : DateReader(reader) {}
	virtual DB::DBUtil::ColType GetColType(UIntOS col, OptOut<UIntOS> size)
	{
		if (col == 0) return DB::DBUtil::CT_Vector;
		DB::ColDef def(CSTR(""));
		if (col == 1 && this->GetReader()->GetColDef(col, def) && def.GetColName()->Equals(UTF8STRC("geom2")))
			return DB::DBUtil::CT_Vector;
		if (col == 3) return DB::DBUtil::CT_Bool;
		return this->GetReader()->GetColType(col, size);
	}
	virtual Optional<Math::Geometry::Vector2D> GetVector(UIntOS col)
	{
		if (col != 0) return nullptr;
		Int32 kind = this->GetInt32(col);
		if (kind == 1 || kind == 3 || kind == 4)
		{
			NN<Math::Geometry::Point> point;
			NEW_CLASSNN(point, Math::Geometry::Point(kind == 1 ? 4326 : (kind == 3 ? 3857 : 0), kind == 3 ? 1113194.90793274 : 114.2, kind == 3 ? 0 : 22.3));
			return point;
		}
		if (kind == 2)
		{
			NN<Math::Geometry::MultiPolygon> multi;
			NN<Math::Geometry::Polygon> polygon;
			NN<Math::Geometry::LinearRing> ring;
			NEW_CLASSNN(multi, Math::Geometry::MultiPolygon(4326));
			NEW_CLASSNN(polygon, Math::Geometry::Polygon(4326));
			NEW_CLASSNN(ring, Math::Geometry::LinearRing(4326, 3, false, false));
			UIntOS count;
			UnsafeArray<Math::Coord2DDbl> points = ring->GetPointList(count);
			points[0] = Math::Coord2DDbl(0, 0);
			points[1] = Math::Coord2DDbl(1, 0);
			points[2] = Math::Coord2DDbl(1, 1);
			polygon->AddGeometry(ring);
			multi->AddGeometry(polygon);
			return multi;
		}
		if (kind == 6)
		{
			NN<Math::Geometry::MultiPoint> multi;
			NN<Math::Geometry::Point> point;
			NEW_CLASSNN(multi, Math::Geometry::MultiPoint(4326));
			NEW_CLASSNN(point, Math::Geometry::Point(4326, 1, 2));
			multi->AddGeometry(point);
			NEW_CLASSNN(point, Math::Geometry::Point(4326, 3, 4));
			multi->AddGeometry(point);
			return multi;
		}
		if (kind == 7)
		{
			NN<Math::Geometry::Polyline> lines;
			NN<Math::Geometry::LineString> line;
			NEW_CLASSNN(lines, Math::Geometry::Polyline(4326));
			NEW_CLASSNN(line, Math::Geometry::LineString(4326, 2, false, false));
			UIntOS count;
			UnsafeArray<Math::Coord2DDbl> points = line->GetPointList(count);
			points[0] = Math::Coord2DDbl(0, 0);
			points[1] = Math::Coord2DDbl(1, 2);
			lines->AddGeometry(line);
			return lines;
		}
		if (kind == 8)
		{
			NN<Math::Geometry::GeometryCollection> collection;
			NEW_CLASSNN(collection, Math::Geometry::GeometryCollection(4326));
			return collection;
		}
		return nullptr;
	}
};

class GeometryDB : public DB::SQLiteFile
{
public:
	GeometryDB() : DB::SQLiteFile(CSTR(":memory:")) {}
	virtual Optional<DB::DBReader> ExecuteReader(Text::CStringNN sql)
	{
		NN<DB::DBReader> reader;
		if (!DB::SQLiteFile::ExecuteReader(sql).SetTo(reader))
			return nullptr;
		NN<GeometryReader> wrapped;
		NEW_CLASSNN(wrapped, GeometryReader(reader));
		return wrapped;
	}
	virtual void CloseReader(NN<DB::DBReader> reader)
	{
		NN<GeometryReader> wrapped = NN<GeometryReader>::ConvertFrom(reader);
		DB::SQLiteFile::CloseReader(wrapped->GetReader());
		wrapped.Delete();
	}
};

static Bool CheckGeoJSON(NN<AI::AIReadDBTool> tool, Text::CStringNN path, Text::CStringNN sql, Text::CStringNN expected, Bool projected = false)
{
	Data::VariObject input(Data::VariObject::NameType::Field);
	Data::VariObject output(Data::VariObject::NameType::Field);
	input.SetItemStr(U8STR("action"), U8STR("sql2geojson"));
	input.SetItemStr(U8STR("fileName"), U8STR("map"));
	NN<Text::String> s = Text::String::New(sql);
	input.SetItemStr(U8STR("sql"), s);
	s->Release();
	Text::StringBuilderUTF8 expectedPath;
	expectedPath.Append(path);
	expectedPath.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
	expectedPath.AppendTS(Data::Timestamp::Now(), "yyyyMMdd");
	expectedPath.Append(CSTR("/test_map.geojson"));
	if (IO::Path::GetPathType(expectedPath.ToCString()) != IO::Path::PathType::Unknown)
		return false;
	if (!tool->Invoke(CSTR("test"), input, output))
		return false;
	NN<Data::VariItem> item;
	if (!output.GetItem(U8STR("attachment")).SetTo(item) || !item->GetAsNewString().SetTo(s))
	{
		Text::StringBuilderUTF8 message;
		output.ToString(message);
		printf("AIReadDBToolTest: GeoJSON failed: %s\n", message.ToString().Ptr());
		return false;
	}
	Text::StringBuilderUTF8 filePath;
	filePath.Append(path);
	filePath.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
	filePath.Append(s);
	s->Release();
	UInt8 bytes[16384];
	IO::FileStream file(filePath.ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	UIntOS length = file.Read(Data::ByteArray(bytes, sizeof(bytes)));
	file.Close();
	IO::Path::DeleteFile(filePath.ToString());
	NN<Text::JSONBase> actual;
	NN<Text::JSONBase> wanted;
	if (!Text::JSONBase::ParseJSONBytes(bytes, length).SetTo(actual))
		return false;
	if (projected)
	{
		NN<Text::JSONArray> features;
		NN<Text::JSONObject> feature;
		NN<Text::JSONObject> geometry;
		NN<Text::JSONArray> coordinates;
		Bool valid = actual->GetValueArray(CSTR("features")).SetTo(features) && features->GetArrayLength() == 1 &&
			features->GetArrayObject(0).SetTo(feature) && feature->GetObjectObject(CSTR("geometry")).SetTo(geometry) &&
			geometry->GetObjectArray(CSTR("coordinates")).SetTo(coordinates) && coordinates->GetArrayLength() == 2 &&
			coordinates->GetArrayDoubleOrNAN(0) > 9.99999 && coordinates->GetArrayDoubleOrNAN(0) < 10.00001 &&
			coordinates->GetArrayDoubleOrNAN(1) > -0.00001 && coordinates->GetArrayDoubleOrNAN(1) < 0.00001;
		actual->EndUse();
		return valid;
	}
	if (!Text::JSONBase::ParseJSONStr(expected).SetTo(wanted))
	{
		actual->EndUse();
		return false;
	}
	Text::StringBuilderUTF8 actualText;
	Text::StringBuilderUTF8 wantedText;
	actual->ToJSONString(actualText);
	wanted->ToJSONString(wantedText);
	Bool same = actualText.ToCString().Equals(wantedText.ToCString());
	if (!same)
		printf("AIReadDBToolTest: unexpected GeoJSON: %s\n", actualText.ToString().Ptr());
	actual->EndUse();
	wanted->EndUse();
	return same;
}

static Bool CheckGeoError(NN<AI::AIReadDBTool> tool, Text::CStringNN sql, Text::CStringNN fileName, Text::CStringNN message)
{
	Data::VariObject input(Data::VariObject::NameType::Field);
	Data::VariObject output(Data::VariObject::NameType::Field);
	input.SetItemStr(U8STR("action"), U8STR("sql2geojson"));
	NN<Text::String> s = Text::String::New(sql);
	input.SetItemStr(U8STR("sql"), s);
	s->Release();
	s = Text::String::New(fileName);
	input.SetItemStr(U8STR("fileName"), s);
	s->Release();
	if (!tool->Invoke(CSTR("test"), input, output) || output.HasItem(U8STR("attachment")))
		return false;
	NN<Data::VariItem> item;
	if (!output.GetItem(U8STR("data")).SetTo(item) || !item->GetAsNewString().SetTo(s))
		return false;
	Bool result = s->Equals(message);
	s->Release();
	return result;
}

static Bool CheckChart(NN<Media::DrawEngine> engine, NN<AI::AIReadDBTool> tool, Text::CStringNN path, Text::CStringNN type, Text::CStringNN sql, Bool customStyles, UIntOS seriesCount, Bool checkScatterPosition = false)
{
	Data::VariObject input(Data::VariObject::NameType::Field);
	Data::VariObject output(Data::VariObject::NameType::Field);
	input.SetItemStr(U8STR("action"), U8STR("sql2chart"));
	NN<Text::String> s = Text::String::New(sql);
	input.SetItemStr(U8STR("sql"), s);
	s->Release();
	input.SetItemStr(U8STR("fileName"), U8STR("chart"));
	s = Text::String::New(type);
	input.SetItemStr(U8STR("chartType"), s);
	s->Release();
	if (customStyles)
	{
		input.SetItemStr(U8STR("dateStyle"), U8STR("yyyy/MM/dd"));
		input.SetItemStr(U8STR("timeStyle"), U8STR("HH:mm:ss"));
	}
	Text::StringBuilderUTF8 expectedPath;
	expectedPath.Append(path);
	expectedPath.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
	expectedPath.AppendTS(Data::Timestamp::Now(), "yyyyMMdd");
	expectedPath.Append(CSTR("/test_chart.png"));
	if (IO::Path::GetPathType(expectedPath.ToCString()) != IO::Path::PathType::Unknown)
		return false;
	if (!tool->Invoke(CSTR("test"), input, output))
		return false;
	NN<Data::VariItem> item;
	NN<Text::String> attachment;
	if (!output.GetItem(U8STR("attachment")).SetTo(item) || !item->GetAsNewString().SetTo(attachment))
	{
		Text::StringBuilderUTF8 sb;
		output.ToString(sb);
		printf("AIReadDBToolTest: chart failed: %s\n", sb.ToString().Ptr());
		return false;
	}
	Text::StringBuilderUTF8 filePath;
	filePath.Append(path);
	filePath.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
	filePath.Append(attachment);
	attachment->Release();
	UInt8 header[24];
	IO::FileStream file(filePath.ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	UIntOS len = file.Read(Data::ByteArray(header, sizeof(header)));
	file.Close();
	Bool valid = len == sizeof(header) && ReadMUInt32(header) == 0x89504e47 && ReadMUInt32(header + 4) == 0x0d0a1a0a &&
		ReadMUInt32(header + 16) == 1024 && ReadMUInt32(header + 20) == 768 + seriesCount * 24;
	NN<Media::DrawImage> image;
	if (valid && engine->LoadImage(filePath.ToCString()).SetTo(image))
	{
		UIntOS i = 0;
		while (i < seriesCount)
		{
			UInt32 color = image->PixelGet32(20, (IntOS)(780 + i * 24)) & 0x00ffffff;
			if (color == 0 || color == 0x00ffffff)
				valid = false;
			if (type.Equals(UTF8STRC("scatter")))
			{
				Bool found = false;
				IntOS y = 0;
				while (y < 768 && !found)
				{
					IntOS x = 0;
					while (x < 1024)
					{
						if ((image->PixelGet32(x, y) & 0x00ffffff) == color)
						{
							found = true;
							break;
						}
						x += 2;
					}
					y += 2;
				}
				if (!found)
					valid = false;
			}
			i++;
		}
		if (checkScatterPosition)
		{
			UInt32 color = image->PixelGet32(20, 780) & 0x00ffffff;
			Bool upperLeft = false;
			Bool lowerRight = false;
			Bool connectingLine = false;
			IntOS y = 50;
			while (y < 300)
			{
				IntOS x = 120;
				while (x < 400)
				{
					if ((image->PixelGet32(x, y) & 0x00ffffff) == color)
						upperLeft = true;
					x += 2;
				}
				y += 2;
			}
			y = 450;
			while (y < 680)
			{
				IntOS x = 650;
				while (x < 950)
				{
					if ((image->PixelGet32(x, y) & 0x00ffffff) == color)
						lowerRight = true;
					x += 2;
				}
				y += 2;
			}
			y = 330;
			while (y < 570)
			{
				IntOS x = 120;
				while (x < 190)
				{
					if ((image->PixelGet32(x, y) & 0x00ffffff) == color)
						connectingLine = true;
					x += 2;
				}
				y += 2;
			}
			valid = valid && upperLeft && !lowerRight && !connectingLine;
		}
		engine->DeleteImage(image);
	}
	else
	{
		valid = false;
	}
	IO::Path::DeleteFile(filePath.ToString());
	return valid;
}

static Bool CheckError(NN<AI::AIReadDBTool> tool, Text::CStringNN sql, Text::CStringNN fileName, Text::CStringNN chartType, Text::CStringNN message)
{
	Data::VariObject input(Data::VariObject::NameType::Field);
	Data::VariObject output(Data::VariObject::NameType::Field);
	input.SetItemStr(U8STR("action"), U8STR("sql2chart"));
	NN<Text::String> s = Text::String::New(sql);
	input.SetItemStr(U8STR("sql"), s);
	s->Release();
	s = Text::String::New(fileName);
	input.SetItemStr(U8STR("fileName"), s);
	s->Release();
	s = Text::String::New(chartType);
	input.SetItemStr(U8STR("chartType"), s);
	s->Release();
	if (!tool->Invoke(CSTR("test"), input, output) || output.HasItem(U8STR("attachment")))
		return false;
	NN<Data::VariItem> item;
	if (!output.GetItem(U8STR("data")).SetTo(item) || !item->GetAsNewString().SetTo(s))
		return false;
	Bool result = s->Equals(message);
	s->Release();
	return result;
}

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UIntOS argc;
	NN<Media::DrawEngine> drawEngine = Media::DrawEngineFactory::CreateDrawEngine();
	UnsafeArray<UnsafeArray<UTF8Char>> argv = progCtrl->GetCommandLines(progCtrl, argc);
	CHECK(argc == 2);
	Text::CStringNN path = Text::CStringNN::FromPtr(argv[1]);
	CHECK(IO::Path::IsDirectoryExist(path));
	CHECK(Net::MIME::GetMIMEFromExt(CSTR("geojson")).Equals(UTF8STRC("application/geo+json")));
	CHECK(Net::MIME::GetMIMEFromExt(CSTR("GEOJSON")).Equals(UTF8STRC("application/geo+json")));
	DB::SQLiteFile db(CSTR(":memory:"));
	CHECK(!db.IsError());
	AI::AIReadDBTool tool(db, CSTR(""), path, false, drawEngine);
	GeometryDB geometryDB;
	AI::AIReadDBTool geoTool(geometryDB, CSTR(""), path, false, nullptr);
	CHECK(CheckGeoJSON(geoTool, path,
		CSTR("SELECT 1 AS geom, 'A\"B' AS name, 12 AS n, 1 AS flag UNION ALL SELECT NULL, 'none', 0, 0 UNION ALL SELECT 2, 'polygon', 5, 1 UNION ALL SELECT 6, 'points', 6, 1"),
		CSTR("{\"type\":\"FeatureCollection\",\"features\":[{\"type\":\"Feature\",\"geometry\":{\"type\":\"Point\",\"coordinates\":[114.2,22.3]},\"properties\":{\"name\":\"A\\\"B\",\"n\":12,\"flag\":true}},{\"type\":\"Feature\",\"geometry\":null,\"properties\":{\"name\":\"none\",\"n\":0,\"flag\":false}},{\"type\":\"Feature\",\"geometry\":{\"type\":\"MultiPolygon\",\"coordinates\":[[[[0,0],[1.0,0],[1.0,1.0],[0,0]]]]},\"properties\":{\"name\":\"polygon\",\"n\":5.0,\"flag\":true}},{\"type\":\"Feature\",\"geometry\":{\"type\":\"MultiPoint\",\"coordinates\":[[1.0,2.0],[3.0,4.0]]},\"properties\":{\"name\":\"points\",\"n\":6.0,\"flag\":true}}]}")));
	CHECK(CheckGeoJSON(geoTool, path, CSTR("SELECT 1 AS geom WHERE 0"), CSTR("{\"type\":\"FeatureCollection\",\"features\":[]}")));
	CHECK(CheckGeoJSON(geoTool, path, CSTR("SELECT 3 AS geom"), CSTR("{}"), true));
	CHECK(CheckGeoJSON(geoTool, path, CSTR("SELECT 7 AS geom"), CSTR("{\"type\":\"FeatureCollection\",\"features\":[{\"type\":\"Feature\",\"geometry\":{\"type\":\"MultiLineString\",\"coordinates\":[[[0,0],[1.0,2.0]]]},\"properties\":{}}]}")));
	CHECK(CheckGeoError(geoTool, CSTR("SELECT 1 AS geom"), CSTR("../map"), CSTR("sql2geojson: invalid fileName or sessionId")));
	CHECK(CheckGeoError(geoTool, CSTR("SELECT 4 AS geom"), CSTR("map"), CSTR("sql2geojson: unknown geometry SRID; select geometry in EPSG:4326")));
	CHECK(CheckGeoError(geoTool, CSTR("SELECT 5 AS geom"), CSTR("map"), CSTR("sql2geojson: failed to read geometry")));
	CHECK(CheckGeoError(geoTool, CSTR("SELECT 8 AS geom"), CSTR("map"), CSTR("sql2geojson: unsupported geometry type")));
	CHECK(CheckGeoError(geoTool, CSTR("SELECT 1 AS geom, 2 AS geom2"), CSTR("map"), CSTR("sql2geojson: select exactly one geometry column")));
	CHECK(CheckGeoError(geoTool, CSTR("SELECT 1 AS geom, 2 AS value, 3 AS value"), CSTR("map"), CSTR("sql2geojson: duplicate column name; use unique aliases")));
	CHECK(CheckGeoError(tool, CSTR("SELECT 1 AS value"), CSTR("map"), CSTR("sql2geojson: select exactly one geometry column")));
	AI::AIReadDBTool geoDisabled(geometryDB, CSTR(""), nullptr, false, nullptr);
	CHECK(CheckGeoError(geoDisabled, CSTR("SELECT 1 AS geom"), CSTR("map"), CSTR("sql2geojson is not enabled")));
	CHECK(CheckChart(drawEngine, tool, path, CSTR("line"), CSTR("SELECT 1 AS x, 2 AS y, 4 AS z UNION ALL SELECT 2, 3, 5"), false, 2));
	Text::StringBuilderUTF8 longNameSQL;
	longNameSQL.Append(CSTR("SELECT 1 AS x, 2 AS \""));
	UIntOS n = 0;
	while (n++ < 600)
		longNameSQL.AppendUTF8Char('a');
	longNameSQL.Append(CSTR("\" UNION ALL SELECT 2, 3"));
	CHECK(CheckChart(drawEngine, tool, path, CSTR("line"), longNameSQL.ToCString(), false, 1));
	CHECK(CheckChart(drawEngine, tool, path, CSTR("filledLine"), CSTR("SELECT 5 AS x, 7 AS y UNION ALL SELECT 6, 8"), false, 1));
	CHECK(CheckChart(drawEngine, tool, path, CSTR("line"), CSTR("SELECT 1000000000 AS x, 7 AS y UNION ALL SELECT 1000000001, 7"), false, 1));
	CHECK(CheckChart(drawEngine, tool, path, CSTR("line"), CSTR("SELECT 0.001 AS x, 0.003 AS y UNION ALL SELECT 0.002, 0.004"), false, 1));
	CHECK(CheckChart(drawEngine, tool, path, CSTR("scatter"), CSTR("SELECT 0 AS x, 0 AS y UNION ALL SELECT 2, 8 UNION ALL SELECT 10, 10"), false, 1, true));
	CHECK(CheckChart(drawEngine, tool, path, CSTR("scatter"), CSTR("SELECT 5 AS x, 7 AS y, 9 AS z"), false, 2));
	DateDB dateDB;
	AI::AIReadDBTool dateTool(dateDB, CSTR(""), path, false, drawEngine);
	CHECK(CheckChart(drawEngine, dateTool, path, CSTR("line"), CSTR("SELECT '2026-01-01 10:00:00' AS x, 3 AS y UNION ALL SELECT '2026-01-02 10:00:00', 4"), true, 1));
	CHECK(CheckChart(drawEngine, dateTool, path, CSTR("filledLine"), CSTR("SELECT '2026-01-01 10:00:00' AS x, 3 AS y UNION ALL SELECT '2026-01-01 10:00:00', 4"), true, 1));
	CHECK(CheckChart(drawEngine, dateTool, path, CSTR("scatter"), CSTR("SELECT '2026-01-01 10:00:00' AS x, 3 AS y"), true, 1));
	CHECK(CheckError(tool, CSTR("SELECT 1, 2"), CSTR("chart"), CSTR("bar"), CSTR("sql2chart: chartType must be line, filledLine or scatter")));
	CHECK(CheckError(tool, CSTR("SELECT 1, 2 WHERE 0"), CSTR("chart"), CSTR("scatter"), CSTR("sql2chart: query returned no rows")));
	CHECK(CheckError(tool, CSTR("SELECT 1, 2 UNION ALL SELECT 2, NULL"), CSTR("chart"), CSTR("scatter"), CSTR("sql2chart: NULL values are not supported")));
	CHECK(CheckError(tool, CSTR("SELECT 1, 2"), CSTR("../chart"), CSTR("line"), CSTR("sql2chart: invalid fileName or sessionId")));
	CHECK(CheckError(tool, CSTR("SELECT 1"), CSTR("chart"), CSTR("line"), CSTR("sql2chart requires an X column and at least one numeric Y column")));
	CHECK(CheckError(tool, CSTR("SELECT 1, 2 WHERE 0"), CSTR("chart"), CSTR("line"), CSTR("sql2chart: query returned no rows")));
	CHECK(CheckError(tool, CSTR("SELECT 1, 2"), CSTR("chart"), CSTR("line"), CSTR("sql2chart: query must return at least two rows")));
	CHECK(CheckError(tool, CSTR("SELECT 1, 2 UNION ALL SELECT 2, NULL"), CSTR("chart"), CSTR("line"), CSTR("sql2chart: NULL values are not supported")));
	CHECK(CheckError(tool, CSTR("SELECT 1, 'abc'"), CSTR("chart"), CSTR("line"), CSTR("sql2chart: all Y columns and a numeric X column must contain numbers")));
	CHECK(CheckError(tool, CSTR("SELECT 1, 1e101 UNION ALL SELECT 2, 3"), CSTR("chart"), CSTR("line"), CSTR("sql2chart: numeric value exceeds supported range")));
	CHECK(CheckError(tool, CSTR("SELECT 1, 1e999 UNION ALL SELECT 2, 3"), CSTR("chart"), CSTR("line"), CSTR("sql2chart: non-finite numeric value")));
	CHECK(CheckError(tool, CSTR("SELECT 1, 0 UNION ALL SELECT 2, 1e-16"), CSTR("chart"), CSTR("line"), CSTR("sql2chart: numeric range is too small to label accurately")));
	AI::AIReadDBTool disabled(db, CSTR(""), path, false, nullptr);
	CHECK(CheckError(disabled, CSTR("SELECT 1, 2"), CSTR("chart"), CSTR("line"), CSTR("sql2chart is not enabled")));
	Text::StringBuilderUTF8 missingPath;
	missingPath.Append(path);
	missingPath.Append(CSTR("/chart-test-blocker"));
	CHECK(IO::Path::GetPathType(missingPath.ToCString()) == IO::Path::PathType::Unknown);
	{
		IO::FileStream blocker(missingPath.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		CHECK(!blocker.IsError());
	}
	AI::AIReadDBTool badPath(db, CSTR(""), missingPath.ToCString(), false, drawEngine);
	Bool exportFailed = CheckError(badPath, CSTR("SELECT 1, 2 UNION ALL SELECT 2, 3"), CSTR("chart"), CSTR("line"), CSTR("sql2chart: failed to export PNG"));
	AI::AIReadDBTool badGeoPath(geometryDB, CSTR(""), missingPath.ToCString(), false, nullptr);
	Bool geoExportFailed = CheckGeoError(badGeoPath, CSTR("SELECT 1 AS geom"), CSTR("map"), CSTR("sql2geojson: failed to export GeoJSON"));
	CHECK(IO::Path::DeleteFile(missingPath.ToString()));
	CHECK(exportFailed);
	CHECK(geoExportFailed);

	Data::ArrayListDbl locations;
	Data::ArrayListStringNN labels;
	Data::ChartPlotter::CalScaleMarkDbl(locations, labels, 1000000000.0, 1000000001.0, 800, 50, "0.00", 0.01, nullptr);
	CHECK(locations.GetCount() > 2);
	labels.FreeAll();
	locations.Clear();
	Data::ChartPlotter::CalScaleMarkDbl(locations, labels, -0.65, -0.11, 800, 50, "0.00", 0.01, nullptr);
	CHECK(locations.GetCount() > 2);
	labels.FreeAll();
	drawEngine.Delete();
	return 0;
}
