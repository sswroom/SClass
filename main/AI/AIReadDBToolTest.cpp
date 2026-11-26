#include "Stdafx.h"
#include "AI/AIReadDBTool.h"
#include "Core/Core.h"
#include "Data/ChartPlotter.h"
#include "DB/SQLiteFile.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Media/DrawEngineFactory.h"

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
	DB::SQLiteFile db(CSTR(":memory:"));
	CHECK(!db.IsError());
	AI::AIReadDBTool tool(db, CSTR(""), path, false, drawEngine);
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
	CHECK(IO::Path::DeleteFile(missingPath.ToString()));
	CHECK(exportFailed);

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
