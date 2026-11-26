#include "Stdafx.h"
#include "AI/AIReadDBTool.h"
#include "Data/ChartPlotter.h"
#include "DB/ColDef.h"
#include "DB/DBConn.h"
#include "DB/DBReader.h"
#include "DB/TableDef.h"
#include "Exporter/XLSXExporter.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Math/CSys/CoordinateSystemConverter.h"
#include "Math/CSys/CoordinateSystemManager.h"
#include "Math/Geometry/MultiPoint.h"
#include "Math/Geometry/MultiPolygon.h"
#include "Math/Geometry/Polygon.h"
#include "Math/Geometry/Polyline.h"
#include "Text/JSONBuilder.h"
#include "Text/SpreadSheet/Workbook.h"

#define VERBOSE

Bool AI::AIReadDBTool::IsNumeric(DB::DBUtil::ColType type)
{
	switch (type)
	{
	case DB::DBUtil::CT_Byte:
	case DB::DBUtil::CT_Int16:
	case DB::DBUtil::CT_UInt16:
	case DB::DBUtil::CT_Int32:
	case DB::DBUtil::CT_UInt32:
	case DB::DBUtil::CT_Int64:
	case DB::DBUtil::CT_UInt64:
	case DB::DBUtil::CT_Float:
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Decimal:
		return true;
	case DB::DBUtil::CT_Unknown:
	case DB::DBUtil::CT_UTF8Char:
	case DB::DBUtil::CT_UTF16Char:
	case DB::DBUtil::CT_UTF32Char:
	case DB::DBUtil::CT_VarUTF8Char:
	case DB::DBUtil::CT_VarUTF16Char:
	case DB::DBUtil::CT_VarUTF32Char:
	case DB::DBUtil::CT_Date:
	case DB::DBUtil::CT_DateTime:
	case DB::DBUtil::CT_DateTimeTZ:
	case DB::DBUtil::CT_Bool:
	case DB::DBUtil::CT_Binary:
	case DB::DBUtil::CT_Vector:
	case DB::DBUtil::CT_UUID:
	default:
		return false;
	}
}

Bool AI::AIReadDBTool::SafeFilePart(Text::CStringNN name)
{
	if (name.leng == 0)
		return false;
	UIntOS i = 0;
	while (i < name.leng)
	{
		UTF8Char c = name.v[i++];
		if (c < 32 || c == '/' || c == '\\' || c == ':' || c == '*' || c == '?' || c == '"' || c == '<' || c == '>' || c == '|')
			return false;
	}
	return true;
}

void AI::AIReadDBTool::ChartResult(NN<Data::VariObject> output, Text::CStringNN message)
{
	NN<Text::String> s = Text::String::New(message);
	output->SetItemStr(U8STR("data"), s);
	s->Release();
}

Bool AI::AIReadDBTool::SaveChart(NN<Media::DrawEngine> engine, NN<Data::ChartPlotter> chart, Text::CStringNN fileName)
{
	NN<Media::DrawImage> image;
	UIntOS count = chart->GetChartCount();
	if (!engine->CreateImage32({1024, 768 + count * 24}, Media::AlphaType::AT_IGNORE_ALPHA).SetTo(image))
	{
		return false;
	}
	chart->Plot(image, 0, 0, 1024, 768);
	NN<Media::DrawBrush> background = image->NewBrushARGB(0xffffffff);
	image->DrawRect({0, 768}, {1024, UIntOS2Double(count * 24)}, nullptr, background);
	image->DelBrush(background);
	NN<Media::DrawBrush> text = image->NewBrushARGB(0xff000000);
	NN<Media::DrawFont> font = image->NewFontPt(CSTR("SimHei"), 12, Media::DrawEngine::DFS_ANTIALIAS, 0);
	UIntOS i = 0;
	NN<Data::ChartPlotter::ChartParam> series;
	while (i < count && chart->GetChart(i).SetTo(series))
	{
		Double y = 768 + UIntOS2Double(i * 24);
		NN<Media::DrawPen> pen = image->NewPenARGB(series->lineColor, 3, nullptr, 0);
		image->DrawLine(12, y + 12, 42, y + 12, pen);
		image->DelPen(pen);
		image->DrawString({50, y}, series->name, font, text);
		i++;
	}
	image->DelFont(font);
	image->DelBrush(text);
	Bool saved = false;
	if (i == count)
	{
		IO::FileStream file(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (!file.IsError())
			saved = image->SavePng(file) > 0;
	}
	engine->DeleteImage(image);
	return saved;
}

void AI::AIReadDBTool::MakeChart(NN<Media::DrawEngine> drawEngine, NN<DB::DBReader> r, Text::CStringNN sessionId, Text::CStringNN path, NN<Data::VariObject> input, NN<Data::VariObject> output)
{
	NN<Data::VariItem> item;
	NN<Text::String> fileName;
	if (!input->GetItem(U8STR("fileName")).SetTo(item) || !item->GetAsNewString().SetTo(fileName))
	{
		ChartResult(output, CSTR("sql2chart requires a fileName"));
		return;
	}
	Bool validName = SafeFilePart(fileName->ToCString()) && SafeFilePart(sessionId);
	Text::StringBuilderUTF8 exportFileName;
	exportFileName.Append(sessionId);
	exportFileName.AppendUTF8Char('_');
	exportFileName.Append(fileName);
	fileName->Release();
	if (!validName)
	{
		ChartResult(output, CSTR("sql2chart: invalid fileName or sessionId"));
		return;
	}
	NN<Text::String> chartType;
	if (!input->GetItem(U8STR("chartType")).SetTo(item) || !item->GetAsNewString().SetTo(chartType))
	{
		ChartResult(output, CSTR("sql2chart requires chartType: line, filledLine or scatter"));
		return;
	}
	Bool filled = chartType->Equals(UTF8STRC("filledLine"));
	Bool scatter = chartType->Equals(UTF8STRC("scatter"));
	Bool validType = filled || scatter || chartType->Equals(UTF8STRC("line"));
	chartType->Release();
	if (!validType)
	{
		ChartResult(output, CSTR("sql2chart: chartType must be line, filledLine or scatter"));
		return;
	}
	Data::ChartPlotter chart(nullptr);
	if (scatter)
		chart.SetPointType(Data::ChartPlotter::PointType::Circle, 4);
	chart.SetDateFormat(CSTR("yyyy-MM-dd"));
	chart.SetTimeFormat(CSTR("HH:mm"));
	NN<Text::String> style;
	if (input->GetItem(U8STR("dateStyle")).SetTo(item))
	{
		if (!item->GetAsNewString().SetTo(style))
		{
			ChartResult(output, CSTR("sql2chart: invalid dateStyle"));
			return;
		}
		if (style->leng == 0 || style->leng > 32)
		{
			style->Release();
			ChartResult(output, CSTR("sql2chart: invalid dateStyle"));
			return;
		}
		chart.SetDateFormat(style->ToCString());
		style->Release();
	}
	if (input->GetItem(U8STR("timeStyle")).SetTo(item))
	{
		if (!item->GetAsNewString().SetTo(style))
		{
			ChartResult(output, CSTR("sql2chart: invalid timeStyle"));
			return;
		}
		if (style->leng == 0 || style->leng > 32)
		{
			style->Release();
			ChartResult(output, CSTR("sql2chart: invalid timeStyle"));
			return;
		}
		chart.SetTimeFormat(style->ToCString());
		style->Release();
	}
	UIntOS colCount = r->ColCount();
	if (colCount < 2)
	{
		ChartResult(output, CSTR("sql2chart requires an X column and at least one numeric Y column"));
		return;
	}
	if (!r->ReadNext())
	{
		ChartResult(output, CSTR("sql2chart: query returned no rows"));
		return;
	}
	DB::DBUtil::ColType xType = r->GetColType(0, nullptr);
	Bool timeX = xType == DB::DBUtil::CT_Date || xType == DB::DBUtil::CT_DateTime || xType == DB::DBUtil::CT_DateTimeTZ;
	if (!timeX && !IsNumeric(xType))
	{
		ChartResult(output, CSTR("sql2chart: X column must be numeric or a date/time"));
		return;
	}
	Data::ArrayListStringNN names;
	Data::ArrayListNN<Data::ArrayListDbl> values;
	Data::ArrayListNative<Data::Timestamp> times;
	Text::CString error = nullptr;
	UIntOS i = 0;
	while (i < colCount)
	{
		if (r->IsNull(i))
		{
			error = CSTR("sql2chart: NULL values are not supported");
			break;
		}
		DB::ColDef colDef(CSTR(""));
		if (!r->GetColDef(i, colDef))
		{
			error = CSTR("sql2chart: failed to get a column name");
			break;
		}
		names.Add(colDef.GetColName()->Clone());
		NN<Data::ArrayListDbl> column;
		NEW_CLASSNN(column, Data::ArrayListDbl());
		values.Add(column);
		i++;
	}
	if (error.IsNull())
	{
		do
		{
			i = 0;
			while (i < colCount)
			{
				DB::DBUtil::ColType type = r->GetColType(i, nullptr);
				if (r->IsNull(i))
				{
					error = CSTR("sql2chart: NULL values are not supported");
					break;
				}
				if (i == 0 && timeX)
				{
					if (type != DB::DBUtil::CT_Date && type != DB::DBUtil::CT_DateTime && type != DB::DBUtil::CT_DateTimeTZ)
					{
						error = CSTR("sql2chart: X column contains a non-date value");
						break;
					}
					Data::Timestamp ts = r->GetTimestamp(i);
					if (ts.IsNull())
					{
						error = CSTR("sql2chart: invalid date/time value");
						break;
					}
					times.Add(ts);
				}
				else
				{
					if (!IsNumeric(type))
					{
						error = CSTR("sql2chart: all Y columns and a numeric X column must contain numbers");
						break;
					}
					Double value = r->GetDblOrNAN(i);
					if (Math::IsNAN(value) || Math::IsInfinity(value))
					{
						error = CSTR("sql2chart: non-finite numeric value");
						break;
					}
					if (value < -1e100 || value > 1e100)
					{
						error = CSTR("sql2chart: numeric value exceeds supported range");
						break;
					}
					values.GetItemNoCheck(i)->Add(value);
				}
				i++;
			}
			if (error.NotNull())
				break;
		} while (r->ReadNext());
	}
	if (error.IsNull() && !scatter && (timeX ? times.GetCount() : values.GetItemNoCheck(0)->GetCount()) < 2)
		error = CSTR("sql2chart: query must return at least two rows");
	if (error.IsNull())
	{
		NN<Data::ChartPlotter::ChartData> xData;
		if (timeX)
			xData = Data::ChartPlotter::NewData(times);
		else
			xData = Data::ChartPlotter::NewData(values.GetItemNoCheck(0)->Arr(), values.GetItemNoCheck(0)->GetCount());
		i = 1;
		while (i < colCount)
		{
			UInt32 color = chart.GetRndColor();
			NN<Data::ArrayListDbl> column = values.GetItemNoCheck(i);
			Bool added;
			if (filled)
				added = chart.AddFilledLineChart(names.GetItemNoCheck(i), Data::ChartPlotter::NewData(column->Arr(), column->GetCount()), xData->Clone(), color, (color & 0x00ffffff) | 0x80000000);
			else if (scatter)
				added = chart.AddScatter(names.GetItemNoCheck(i)->ToCString(), Data::ChartPlotter::NewData(column->Arr(), column->GetCount()), xData->Clone(), color);
			else
				added = chart.AddLineChart(names.GetItemNoCheck(i), Data::ChartPlotter::NewData(column->Arr(), column->GetCount()), xData->Clone(), color);
			if (!added)
			{
				error = CSTR("sql2chart: failed to add chart series");
				break;
			}
			i++;
		}
		xData.Delete();
	}
	if (error.IsNull())
	{
		chart.SetXAxisName(names.GetItemNoCheck(0)->ToCString());
		NN<Data::ChartPlotter::Axis> axis;
		Double minRange = 0;
		if (chart.GetXAxis().SetTo(axis))
		{
			if (timeX)
			{
				NN<Data::ChartPlotter::TimeAxis> tAxis = NN<Data::ChartPlotter::TimeAxis>::ConvertFrom(axis);
				if (tAxis->GetMin() == tAxis->GetMax())
					tAxis->ExtendRange(tAxis->GetMin().AddSecond(60));
			}
			else
			{
				NN<Data::ChartPlotter::DoubleAxis> dAxis = NN<Data::ChartPlotter::DoubleAxis>::ConvertFrom(axis);
				if (dAxis->GetMin() == dAxis->GetMax())
					dAxis->ExtendRange(dAxis->GetMin() == 0 ? 1 : 0);
				minRange = dAxis->GetMax() - dAxis->GetMin();
			}
		}
		if (chart.GetY1Axis().SetTo(axis))
		{
			NN<Data::ChartPlotter::DoubleAxis> dAxis = NN<Data::ChartPlotter::DoubleAxis>::ConvertFrom(axis);
			if (dAxis->GetMin() == dAxis->GetMax())
				dAxis->ExtendRange(dAxis->GetMin() == 0 ? 1 : 0);
			Double yRange = dAxis->GetMax() - dAxis->GetMin();
			if (minRange == 0 || yRange < minRange)
				minRange = yRange;
		}
		if (minRange < 1e-14)
			error = CSTR("sql2chart: numeric range is too small to label accurately");
		else
		{
			Text::StringBuilderUTF8 format;
			format.Append(CSTR("0.00"));
			Double precision = 0.01;
			UIntOS decimals = 2;
			while (precision > minRange / 10 && decimals < 15)
			{
				format.AppendUTF8Char('0');
				precision *= 0.1;
				decimals++;
			}
			chart.SetDblFormat(format.ToCString());
		}
	}
	if (error.IsNull())
	{
		Text::StringBuilderUTF8 sb;
		sb.Append(path);
		if (!sb.EndsWith(IO::Path::PATH_SEPERATOR))
			sb.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
		Data::Timestamp fileTime = Data::Timestamp::Now();
		sb.AppendTS(fileTime, "yyyyMMdd");
		IO::Path::CreateDirectory(sb.ToCString());
		sb.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
		sb.Append(exportFileName.ToCString());
		sb.Append(CSTR(".png"));
		Bool saved = SaveChart(drawEngine, chart, sb.ToCString());
		if (!saved)
		{
			error = CSTR("sql2chart: failed to export PNG");
		}
		else
		{
			sb.ClearStr();
			sb.Append(CSTR("Attachment "));
			sb.Append(exportFileName.ToCString());
			sb.Append(CSTR(".png added to user with "));
			sb.AppendUIntOS(values.GetItemNoCheck(0)->GetCount());
			sb.Append(CSTR(" records"));
			ChartResult(output, sb.ToCString());
			sb.ClearStr();
			sb.AppendTS(fileTime, "yyyyMMdd");
			sb.AppendUTF8Char('/');
			sb.Append(exportFileName.ToCString());
			sb.Append(CSTR(".png"));
			NN<Text::String> attachment = Text::String::New(sb.ToCString());
			output->SetItemStr(U8STR("attachment"), attachment);
			attachment->Release();
		}
	}
	UnsafeArray<const UTF8Char> errorText;
	if (error.v.SetTo(errorText))
		ChartResult(output, Text::CStringNN(errorText, error.leng));
	names.FreeAll();
	values.DeleteAll();
}

void AI::AIReadDBTool::MakeGeoJSON(NN<DB::DBReader> r, Text::CStringNN sessionId, Text::CStringNN path, NN<Data::VariObject> input, NN<Data::VariObject> output)
{
	NN<Data::VariItem> item;
	NN<Text::String> fileName;
	if (!input->GetItem(U8STR("fileName")).SetTo(item) || !item->GetAsNewString().SetTo(fileName))
	{
		ChartResult(output, CSTR("sql2geojson requires a fileName"));
		return;
	}
	Bool validName = SafeFilePart(fileName->ToCString()) && SafeFilePart(sessionId);
	Text::StringBuilderUTF8 exportName;
	exportName.Append(sessionId);
	exportName.AppendUTF8Char('_');
	exportName.Append(fileName);
	exportName.Append(CSTR(".geojson"));
	fileName->Release();
	if (!validName)
	{
		ChartResult(output, CSTR("sql2geojson: invalid fileName or sessionId"));
		return;
	}

	Bool hasRow = r->ReadNext();
	UIntOS colCount = r->ColCount();
	UIntOS geomCol = INVALID_INDEX;
	Data::ArrayListStringNN names;
	Text::CString error = nullptr;
	UIntOS i = 0;
	while (i < colCount)
	{
		DB::ColDef col(CSTR(""));
		if (!r->GetColDef(i, col))
		{
			error = CSTR("sql2geojson: failed to get a column name");
			break;
		}
		UIntOS j = 0;
		while (j < names.GetCount())
		{
			if (names.GetItemNoCheck(j++)->Equals(col.GetColName()->ToCString()))
			{
				error = CSTR("sql2geojson: duplicate column name; use unique aliases");
				break;
			}
		}
		if (!error.IsNull())
			break;
		names.Add(col.GetColName()->Clone());
		if (r->GetColType(i, nullptr) == DB::DBUtil::CT_Vector)
		{
			if (geomCol != INVALID_INDEX)
			{
				error = CSTR("sql2geojson: select exactly one geometry column");
				break;
			}
			geomCol = i;
		}
		i++;
	}
	if (error.IsNull() && geomCol == INVALID_INDEX)
		error = CSTR("sql2geojson: select exactly one geometry column");

	UIntOS recCount = 0;
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	json.ObjectAddStr(CSTR("type"), CSTR("FeatureCollection"));
	json.ObjectBeginArray(CSTR("features"));
	while (error.IsNull() && hasRow)
	{
		json.ArrayBeginObject();
		recCount++;
		json.ObjectAddStr(CSTR("type"), CSTR("Feature"));
		if (r->IsNull(geomCol))
		{
			json.ObjectAddNull(CSTR("geometry"));
		}
		else
		{
			NN<Math::Geometry::Vector2D> vec;
			if (!r->GetVector(geomCol).SetTo(vec))
			{
				error = CSTR("sql2geojson: failed to read geometry");
				break;
			}
			if (vec->GetSRID() != 4326)
			{
				NN<Math::CSys::CoordinateSystem> source;
				if (vec->GetSRID() == 0 || !Math::CSys::CoordinateSystemManager::SRCreateCSys(vec->GetSRID()).SetTo(source))
				{
					error = CSTR("sql2geojson: unknown geometry SRID; select geometry in EPSG:4326");
				}
				else
				{
					NN<Math::CSys::GeographicCoordinateSystem> dest = Math::CSys::CoordinateSystemManager::CreateWGS84Csys();
					Math::CSys::CoordinateSystemConverter converter(source, dest);
					vec->Convert(converter);
					source.Delete();
					dest.Delete();
				}
			}
			Data::ArrayListA<Math::Coord2DDbl> coords;
			if (error.IsNull())
			{
				vec->GetCoordinates(coords);
				i = 0;
				while (i < coords.GetCount())
				{
					Math::Coord2DDbl p = coords.GetItem(i++);
					if (Math::IsNAN(p.x) || Math::IsNAN(p.y) || Math::IsInfinity(p.x) || Math::IsInfinity(p.y))
					{
						error = CSTR("sql2geojson: non-finite geometry coordinate");
						break;
					}
				}
			}
			if (error.IsNull())
			{
				if (!json.ObjectAddGeometry(CSTR("geometry"), vec))
					error = CSTR("sql2geojson: unsupported geometry type");
			}
			vec.Delete();
			if (!error.IsNull())
				break;
		}
		json.ObjectBeginObject(CSTR("properties"));
		Data::VariItem value;
		i = 0;
		while (i < colCount)
		{
			if (i != geomCol)
			{
				Text::CStringNN name = names.GetItemNoCheck(i)->ToCString();
				if (r->IsNull(i))
					json.ObjectAddNull(name);
				else if (r->GetColType(i, nullptr) == DB::DBUtil::CT_Bool)
					json.ObjectAddBool(name, r->GetBool(i));
				else if (r->GetColType(i, nullptr) == DB::DBUtil::CT_Binary || r->GetColType(i, nullptr) == DB::DBUtil::CT_UUID)
				{
					NN<Text::String> str;
					if (!r->GetNewStr(i).SetTo(str))
					{
						error = CSTR("sql2geojson: failed to read property");
						break;
					}
					json.ObjectAddStr(name, str);
					str->Release();
				}
				else if (!r->GetVariItem(i, value) || ((value.GetItemType() == Data::VariItem::ItemType::F32 || value.GetItemType() == Data::VariItem::ItemType::F64) && (Math::IsNAN(value.GetAsF64()) || Math::IsInfinity(value.GetAsF64()))) || !json.ObjectAddVarItem(name, value))
				{
					error = CSTR("sql2geojson: failed to read property");
					break;
				}
			}
			i++;
		}
		json.ObjectEnd();
		json.ObjectEnd();
		if (error.IsNull())
			hasRow = r->ReadNext();
	}
	names.FreeAll();
	if (!error.IsNull())
	{
		UnsafeArray<const UTF8Char> errorText;
		if (error.v.SetTo(errorText))
			ChartResult(output, Text::CStringNN(errorText, error.leng));
		return;
	}
	if (NN<DB::DBConn>::ConvertFrom(this->db)->GetLastDataError() != DB::DBConn::DataError::NoError)
	{
		Text::StringBuilderUTF8 sbError;
		this->db->GetLastErrorMsg(sbError);
		ChartResult(output, sbError.ToCString());
		return;
	}
	json.ArrayEnd();
	Text::CStringNN content = json.Build();
	Text::StringBuilderUTF8 filePath;
	filePath.Append(path);
	if (!filePath.EndsWith(IO::Path::PATH_SEPERATOR))
		filePath.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
	Data::Timestamp fileTime = Data::Timestamp::Now();
	filePath.AppendTS(fileTime, "yyyyMMdd");
	IO::Path::CreateDirectory(filePath.ToCString());
	filePath.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
	filePath.Append(exportName.ToCString());
	Bool saved = false;
	{
		IO::FileStream file(filePath.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (!file.IsError())
			saved = file.Write(Data::ByteArrayR(content.v, content.leng)) == content.leng;
	}
	if (!saved)
	{
		ChartResult(output, CSTR("sql2geojson: failed to export GeoJSON"));
		return;
	}
	Text::StringBuilderUTF8 result;
	result.Append(CSTR("Attachment "));
	result.Append(exportName.ToCString());
	result.Append(CSTR(" added to user with "));
	result.AppendUIntOS(recCount);
	result.AppendC(UTF8STRC(" rows."));
	ChartResult(output, result.ToCString());
	result.ClearStr();
	result.AppendTS(fileTime, "yyyyMMdd");
	result.AppendUTF8Char('/');
	result.Append(exportName.ToCString());
	NN<Text::String> attachment = Text::String::New(result.ToCString());
	output->SetItemStr(U8STR("attachment"), attachment);
	attachment->Release();
}

AI::AIReadDBTool::AIReadDBTool(NN<DB::ReadingDB> db, Text::CStringNN schemaName, Text::CString filePath, Bool allowXlsx, Optional<Media::DrawEngine> drawEngine)
{
	this->db = db;
	this->schemaName = Text::String::New(schemaName);
	this->filePath = Text::String::NewOrNull(filePath);
	this->allowXlsx = allowXlsx;
	this->drawEngine = drawEngine;
	this->maxToolSize = 8192;
}

AI::AIReadDBTool::~AIReadDBTool()
{
	this->schemaName->Release();
	OPTSTR_DEL(this->filePath)
}

Text::CStringNN AI::AIReadDBTool::GetName() const
{
	return CSTR("read_database");
}

void AI::AIReadDBTool::GetDescription(NN<Text::StringBuilderUTF8> sb) const
{
	sb->AppendC(UTF8STRC("Read tables from schema "));
	sb->Append(this->schemaName);
	sb->AppendC(UTF8STRC(". Use list_tables to list tables. Use read_table with tableName, optional SQL-like condition, offset and maxCount to inspect rows."));
	sb->AppendC(UTF8STRC(" Conditions support comparisons, IN, BETWEEN and AND; for example site_code = 'SK-03'."));
	sb->AppendC(UTF8STRC(" Filter by condition instead of reading many rows. maxCount is limited to 100 and defaults to 20. Results are returned as JSON in data."));
}

void AI::AIReadDBTool::GetParameters(NN<Net::OpenAIToolParameters> parameters) const
{
	if (this->db->IsFullConn())
	{
		DB::SQLType sqlType = NN<DB::DBConn>::ConvertFrom(this->db)->GetSQLType();
		Text::StringBuilderUTF8 sbSQL;
		Text::StringBuilderUTF8 sbParam;
		sbSQL.Append(CSTR("SQL query in "));
		sbSQL.Append(DB::SQLTypeGetName(sqlType));
		sbSQL.Append(CSTR(" syntax; required for execute_sql"));

		sbParam.AppendC(UTF8STRC("list_tables, execute_sql"));
		if (this->filePath.NotNull())
		{
			sbParam.AppendC(UTF8STRC(", sql2geojson"));
			sbSQL.Append(CSTR(", sql2geojson (select exactly one geometry column and any property columns; known SRIDs are converted to EPSG:4326, unknown SRIDs must be transformed in SQL; supports Point, LineString, MultiLineString, Polygon, MultiPolygon and CurvePolygon; CurvePolygon is converted to Polygon, and coordinates are exported in 2D)"));
			if (this->allowXlsx)
			{
				sbParam.AppendC(UTF8STRC(", sql2xlsx"));
				sbSQL.Append(CSTR(", sql2xlsx"));
			}
			if (this->drawEngine.NotNull())
			{
				sbParam.AppendC(UTF8STRC(", sql2chart"));
				sbSQL.Append(CSTR(", sql2chart"));
			}
		}
		sbParam.AppendC(UTF8STRC(", or read_table"));
		parameters->AddParameter(CSTR("action"), sbParam.ToCString(), Net::OpenAIToolParameterType::String, true);

		if (DB::DBUtil::HasSchema(sqlType))
		{
			sbSQL.Append(CSTR("; requires schema name"));
		}
		parameters->AddParameter(CSTR("sql"), sbSQL.ToCString(), Net::OpenAIToolParameterType::String, false);
		if (this->filePath.NotNull())
		{
			{
				sbParam.ClearStr();
				sbParam.Append(CSTR("File Name without extension; required for sql2geojson"));
				if (this->allowXlsx)
				{
					sbParam.Append(CSTR(", sql2xlsx"));
				}
				if (this->drawEngine.NotNull())
				{
					sbParam.Append(CSTR(", sql2chart"));
				}
				parameters->AddParameter(CSTR("fileName"), sbParam.ToCString(), Net::OpenAIToolParameterType::String, false);
			}
			if (this->allowXlsx)
			{
				parameters->AddParameter(CSTR("sheetName"), CSTR("Sheet Name; required for sql2xlsx"), Net::OpenAIToolParameterType::String, false);
			}
			if (this->drawEngine.NotNull())
			{
				parameters->AddParameter(CSTR("chartType"), CSTR("line, filledLine or scatter; required for sql2chart. Select a numeric or date/time X column first, followed by numeric Y columns; order rows by X for line charts. Line charts require at least two rows; scatter requires at least one. NULL and non-finite values are not supported. Numeric magnitudes must not exceed 1e100 and axis ranges must be at least 1e-14."), Net::OpenAIToolParameterType::String, false);
				parameters->AddParameter(CSTR("dateStyle"), CSTR("Date style in format of yMd, default is yyyy-MM-dd; optional for sql2chart"), Net::OpenAIToolParameterType::String, false);
				parameters->AddParameter(CSTR("timeStyle"), CSTR("Time style in format of Hmsf, default is HH:mm; optional for sql2chart"), Net::OpenAIToolParameterType::String, false);
			}
		}
	}
	else
	{
		parameters->AddParameter(CSTR("action"), CSTR("list_tables or read_table"), Net::OpenAIToolParameterType::String, true);
	}
	parameters->AddParameter(CSTR("tableName"), CSTR("Table name; required for read_table"), Net::OpenAIToolParameterType::String, false);
	parameters->AddParameter(CSTR("condition"), CSTR("Optional SQL-like filter, e.g. site_code = 'SK-03'"), Net::OpenAIToolParameterType::String, false);
	parameters->AddParameter(CSTR("ordering"), CSTR("Optional ordering, e.g. columnName ASC"), Net::OpenAIToolParameterType::String, false);
	parameters->AddParameter(CSTR("offset"), CSTR("Row offset; defaults to 0"), Net::OpenAIToolParameterType::Integer, false);
	parameters->AddParameter(CSTR("maxCount"), CSTR("Maximum rows to return (1-100); defaults to 20"), Net::OpenAIToolParameterType::Integer, false);
}

Text::CStringNN AI::AIReadDBTool::GetSchemaName() const
{
	return this->schemaName->ToCString();
}

UIntOS AI::AIReadDBTool::QueryTableNames(NN<Data::ArrayListStringNN> names) const
{
	return this->db->QueryTableNames(this->schemaName->ToCString(), names);
}

Optional<DB::TableDef> AI::AIReadDBTool::GetTableDef(Text::CStringNN tableName) const
{
	return this->db->GetTableDef(this->schemaName->ToCString(), tableName);
}

Optional<DB::DBReader> AI::AIReadDBTool::QueryTableData(Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UIntOS dataOfst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition) const
{
#ifdef VERBOSE
	printf("AIReadDBTool: QueryTableData tableName = %s, dataOfst = %llu, maxCnt = %llu\n", tableName.v.Ptr(), dataOfst, maxCnt);
#endif
	return this->db->QueryTableData(this->schemaName->ToCString(), tableName, columnNames, dataOfst, maxCnt, ordering, condition);
}

void AI::AIReadDBTool::CloseReader(NN<DB::DBReader> reader) const
{
	this->db->CloseReader(reader);
}

Bool AI::AIReadDBTool::Invoke(Text::CStringNN sessionId, NN<Data::VariObject> input, NN<Data::VariObject> output)
{
	NN<Data::VariItem> actionItem;
	NN<Text::String> action;
	if (!input->GetItem(U8STR("action")).SetTo(actionItem) || !actionItem->GetAsNewString().SetTo(action))
	{
		ChartResult(output, CSTR("action not specified or invalid"));
		return true;
	}

#ifdef VERBOSE
	printf("AIReadDBTool: action = %s\n", action->v.Ptr());
#endif
	Bool sqlAction = action->Equals(UTF8STRC("execute_sql"));
	Bool listTables = action->Equals(UTF8STRC("list_tables"));
	Bool readTable = action->Equals(UTF8STRC("read_table"));
	Bool sql2xlsx = action->Equals(UTF8STRC("sql2xlsx"));
	Bool sql2chart = action->Equals(UTF8STRC("sql2chart"));
	Bool sql2geojson = action->Equals(UTF8STRC("sql2geojson"));
	action->Release();
	if (listTables)
	{
		Data::ArrayListStringNN names;
		this->QueryTableNames(names);
		Text::JSONBuilder json(Text::JSONBuilder::OT_ARRAY);
		UIntOS i = 0;
		UIntOS count = names.GetCount();
		while (i < count)
		{
			json.ArrayAddStr(names.GetItemNoCheck(i));
			i++;
		}
		names.FreeAll();
		NN<Text::String> dataStr = Text::String::New(json.Build());
		output->SetItemStr(U8STR("data"), dataStr);
		dataStr->Release();
		return true;
	}

	if (sqlAction || sql2xlsx || sql2chart || sql2geojson)
	{
		NN<Media::DrawEngine> drawEngine;
		if (sql2geojson && this->filePath.IsNull())
		{
			ChartResult(output, CSTR("sql2geojson is not enabled"));
			return true;
		}
		if (sql2chart && (!this->drawEngine.SetTo(drawEngine) || this->filePath.IsNull()))
		{
			ChartResult(output, CSTR("sql2chart is not enabled"));
			return true;
		}
		if (!this->db->IsFullConn())
		{
			ChartResult(output, CSTR("SQL actions is not supported"));
			return true;
		}
		NN<DB::DBConn> conn = NN<DB::DBConn>::ConvertFrom(this->db);
		NN<Text::String> sql;
		NN<Data::VariItem> sqlItem;
		if (!input->GetItem(U8STR("sql")).SetTo(sqlItem) || !sqlItem->GetAsNewString().SetTo(sql))
		{
			ChartResult(output, CSTR("sql is not specified or invalid"));
			return true;
		}
		if (!sql->StartsWithICase(CSTR("SELECT")))
		{
			sql->Release();
			ChartResult(output, CSTR("sql is not specified or invalid"));
			return true;
		}
#ifdef VERBOSE
		printf("AIReadDBTool: Execute SQL: %s\n", sql->v.Ptr());
#endif
		NN<DB::DBReader> r;
		if (!conn->ExecuteReader(sql->ToCString()).SetTo(r))
		{
			Text::StringBuilderUTF8 sb;
			sb.Append(CSTR("Failed to execute SQL: "));
			conn->GetLastErrorMsg(sb);
			NN<Text::String> dataStr = Text::String::New(sb.ToCString());
			output->SetItemStr(U8STR("data"), dataStr);
			dataStr->Release();
			sql->Release();
#ifdef VERBOSE
			printf("AIReadDBTool: %s\n", sb.v.Ptr());
#endif
			return true;
		}
		if (sql2xlsx)
		{
			NN<Text::String> fileName;
			NN<Text::String> sheetName;
			if (!input->GetItem(U8STR("fileName")).SetTo(actionItem) || !actionItem->GetAsNewString().SetTo(fileName))
			{
				sql->Release();
				ChartResult(output, CSTR("fileName is not specified"));
				return true;
			}
			if (!input->GetItem(U8STR("sheetName")).SetTo(actionItem) || !actionItem->GetAsNewString().SetTo(sheetName))
			{
				sql->Release();
				fileName->Release();
				ChartResult(output, CSTR("sheetName is not specified"));
				return true;
			}
			Text::SpreadSheet::Workbook workbook;
			NN<Text::SpreadSheet::Worksheet> sheet = workbook.AddWorksheet(sheetName);
			UTF8Char sbuff[512];
			UnsafeArray<UTF8Char> sptr;
			NN<Text::String> s;
			UIntOS recCount = 0;
			UIntOS colCount = r->ColCount();
			UIntOS row = 0;
			UIntOS col = 0;
			UnsafeArray<DB::DBUtil::ColType> colTypes = MemAllocArr(DB::DBUtil::ColType, colCount);
			while (col < colCount)
			{
				if (r->GetName(col, sbuff).SetTo(sptr))
				{
					sheet->SetCellString(row, col, CSTRP(sbuff, sptr));
				}
				colTypes[col] = r->GetColType(col, nullptr);
				col++;
			}
			row++;
			while (r->ReadNext())
			{
				recCount++;
				col = 0;
				while (col < colCount)
				{
					if (!r->IsNull(col))
					{
						switch (colTypes[col])
						{
						case DB::DBUtil::CT_Byte:
						case DB::DBUtil::CT_Int16:
						case DB::DBUtil::CT_UInt16:
						case DB::DBUtil::CT_Int32:
							sheet->SetCellInt32(row, col, r->GetInt32(col));
							break;
						case DB::DBUtil::CT_UInt32:
						case DB::DBUtil::CT_UInt64:
						case DB::DBUtil::CT_Int64:
							sheet->SetCellInt64(row, col, r->GetInt64(col));
							break;
						case DB::DBUtil::CT_Float:
						case DB::DBUtil::CT_Double:
						case DB::DBUtil::CT_Decimal:
							sheet->SetCellDouble(row, col, r->GetDblOrNAN(col));
							break;
						case DB::DBUtil::CT_Bool:
							sheet->SetCellString(row, col, r->GetBool(col) ? CSTR("Y") : CSTR("N"));
							break;
						case DB::DBUtil::CT_Date:
						case DB::DBUtil::CT_DateTimeTZ:
						case DB::DBUtil::CT_DateTime:
							sheet->SetCellTS(row, col, r->GetTimestamp(col).ToLocalTime());
							break;
						case DB::DBUtil::CT_Unknown:
						case DB::DBUtil::CT_UTF8Char:
						case DB::DBUtil::CT_UTF16Char:
						case DB::DBUtil::CT_UTF32Char:
						case DB::DBUtil::CT_VarUTF8Char:
						case DB::DBUtil::CT_VarUTF16Char:
						case DB::DBUtil::CT_VarUTF32Char:
						case DB::DBUtil::CT_Binary:
						case DB::DBUtil::CT_Vector:
						case DB::DBUtil::CT_UUID:
						default:
							if (r->GetNewStr(col).SetTo(s))
							{
								sheet->SetCellString(row, col, s);
								s->Release();
							}
							break;
						}
					}
					col++;
				}
				row++;
			}
			MemFreeArr(colTypes);
			Text::StringBuilderUTF8 sb;
			sb.AppendOpt(this->filePath);
			if (!sb.EndsWith(IO::Path::PATH_SEPERATOR))
			{
				sb.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
			}
			Data::Timestamp fileTime = Data::Timestamp::Now();
			sb.AppendTS(fileTime, "yyyyMMdd");
			IO::Path::CreateDirectory(sb.ToCString());
			sb.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
			Text::StringBuilderUTF8 exportFileName;
			exportFileName.Append(sessionId);
			exportFileName.AppendUTF8Char('_');
			exportFileName.Append(fileName);
			sb.Append(exportFileName.ToCString());
			sb.Append(CSTR(".xlsx"));
			Bool succ = false;
			{
				Exporter::XLSXExporter exporter;
				succ = exporter.ExportNewFile(sb.ToCString(), workbook, nullptr);
			}
			if (!succ)
			{
				sb.ClearStr();
				sb.Append(CSTR("Failed to export to "));
				sb.Append(exportFileName.ToCString());
				sb.Append(CSTR(".xlsx file"));
				s = Text::String::New(sb.ToCString());
				output->SetItemStr(U8STR("data"), s);
				s->Release();
			}
			else
			{
				sb.ClearStr();
				sb.Append(CSTR("Attachment "));
				sb.Append(exportFileName.ToCString());
				sb.Append(CSTR(".xlsx added to user with "));
				sb.AppendUIntOS(recCount);
				sb.Append(CSTR(" rows."));
				s = Text::String::New(sb.ToCString());
				output->SetItemStr(U8STR("data"), s);
				s->Release();

				sb.ClearStr();
				sb.AppendTS(fileTime, "yyyyMMdd");
				sb.AppendUTF8Char('/');
				sb.Append(exportFileName.ToCString());
				sb.Append(CSTR(".xlsx"));
				s = Text::String::New(sb.ToCString());
				output->SetItemStr(U8STR("attachment"), s);
				s->Release();
			}

			conn->CloseReader(r);
			fileName->Release();
			sheetName->Release();
			sql->Release();
			return true;
		}
		else if (sql2geojson)
		{
			NN<Text::String> geoPath;
			if (this->filePath.SetTo(geoPath))
				MakeGeoJSON(r, sessionId, geoPath->ToCString(), input, output);
			else
				ChartResult(output, CSTR("sql2geojson is not enabled"));
			sql->Release();
			conn->CloseReader(r);
			return true;
		}
		else if (sql2chart)
		{
			NN<Text::String> chartPath;
			if (this->filePath.SetTo(chartPath) && this->drawEngine.SetTo(drawEngine))
				MakeChart(drawEngine, r, sessionId, chartPath->ToCString(), input, output);
			else
				ChartResult(output, CSTR("sql2chart is not enabled"));
			sql->Release();
			conn->CloseReader(r);
			return true;
		}
		else
		{
			Data::VariItem item;
			Text::JSONBuilder json(Text::JSONBuilder::OT_ARRAY);
			UIntOS colCount = r->ColCount();
			while (r->ReadNext())
			{
				json.ArrayBeginObject();
				UIntOS i = 0;
				while (i < colCount)
				{
					UTF8Char nameBuff[512];
					UnsafeArray<UTF8Char> nameEnd;
					if (!r->GetName(i, nameBuff).SetTo(nameEnd))
					{
						conn->CloseReader(r);
						printf("AIReadDBTool: failed to get column name at index %d\n", (Int32)i);
						return false;
					}
					if (!r->GetVariItem(i, item) || !json.ObjectAddVarItem(CSTRP(nameBuff, nameEnd), item))
					{
						conn->CloseReader(r);
						printf("AIReadDBTool: failed to get column value at index %d\n", (Int32)i);
						return false;
					}
					i++;
				}
				json.ObjectEnd();
			}
			conn->CloseReader(r);
			NN<Text::String> dataStr;
			dataStr = Text::String::New(json.Build());
			output->SetItemStr(U8STR("data"), dataStr);
			dataStr->Release();
			sql->Release();
			return true;
		}
	}

	if (!readTable)
	{
		ChartResult(output, CSTR("action is not specified or invalid"));
		return true;
	}

	NN<Data::VariItem> tableNameItem;
	NN<Text::String> tableName;
	Text::CString ordering = nullptr;
	Text::StringBuilderUTF8 orderingSb;
	if (!input->GetItem(U8STR("tableName")).SetTo(tableNameItem) || !tableNameItem->GetAsNewString().SetTo(tableName))
	{
		ChartResult(output, CSTR("tableName not specified or invalid"));
		return true;
	}

	NN<Data::VariItem> orderingItem;
	if (input->GetItem(U8STR("ordering")).SetTo(orderingItem))
	{
		orderingItem->GetAsString(orderingSb);
		if (orderingSb.GetLength() > 0)
		{
			ordering = orderingSb.ToCString();
		}
	}

	UIntOS dataOfst;
	UIntOS maxCnt;
	if (!GetUIntOSInput(input, U8STR("offset"), 0, dataOfst) || !GetUIntOSInput(input, U8STR("maxCount"), 20, maxCnt) || maxCnt == 0 || maxCnt > 100)
	{
		tableName->Release();
		ChartResult(output, CSTR("invalid offset or maxCount"));
		return true;
	}

	Optional<Data::QueryConditions> condition = nullptr;
	NN<Data::VariItem> conditionItem;
	if (input->GetItem(U8STR("condition")).SetTo(conditionItem))
	{
		NN<Text::String> conditionText;
		NN<DB::TableDef> tableDef;
		if (!conditionItem->GetAsNewString().SetTo(conditionText))
		{
			tableName->Release();
			ChartResult(output, CSTR("condition is not a valid string"));
			return true;
		}
		if (!this->GetTableDef(tableName->ToCString()).SetTo(tableDef))
		{
			conditionText->Release();
			tableName->Release();
			ChartResult(output, CSTR("table not found"));
			return true;
		}
		condition = Data::QueryConditions::ParseStr(conditionText->ToCString(), tableDef->GetSQLType(), this->db->GetTzQhr());
		conditionText->Release();
		tableDef.Delete();
		if (condition.IsNull())
		{
			tableName->Release();
			ChartResult(output, CSTR("failed to parse condition"));
			return true;
		}
	}

	NN<DB::DBReader> reader;
	if (!this->QueryTableData(tableName->ToCString(), nullptr, dataOfst, maxCnt, ordering, condition).SetTo(reader))
	{
		condition.Delete();
		tableName->Release();
		ChartResult(output, CSTR("failed to query table data"));
		return true;
	}
	condition.Delete();
	tableName->Release();

	Text::JSONBuilder json(Text::JSONBuilder::OT_ARRAY);
	Data::VariItem item;
	UIntOS colCount = reader->ColCount();
	while (reader->ReadNext())
	{
		json.ArrayBeginObject();
		UIntOS i = 0;
		while (i < colCount)
		{
			UTF8Char nameBuff[512];
			UnsafeArray<UTF8Char> nameEnd;
			if (!reader->GetName(i, nameBuff).SetTo(nameEnd))
			{
				this->CloseReader(reader);
				printf("AIReadDBTool: failed to get column name at index %d\n", (Int32)i);
				return false;
			}
			if (!reader->GetVariItem(i, item) || !json.ObjectAddVarItem(CSTRP(nameBuff, nameEnd), item))
			{
				this->CloseReader(reader);
				printf("AIReadDBTool: failed to get column value at index %d\n", (Int32)i);
				return false;
			}
			i++;
		}
		json.ObjectEnd();
		if (json.GetLength() > this->maxToolSize)
		{
			break;
		}
	}
	this->CloseReader(reader);
	NN<Text::String> dataStr;
	dataStr = Text::String::New(json.Build());
	output->SetItemStr(U8STR("data"), dataStr);
	dataStr->Release();
	return true;
}

void AI::AIReadDBTool::SetMaxToolSize(UIntOS maxToolSize)
{
	this->maxToolSize = maxToolSize;
}