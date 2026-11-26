#include "Stdafx.h"
#include "AI/AICSysTool.h"
#include "Math/Math_C.h"
#include "Math/CSys/CoordinateSystemManager.h"
#include "Text/JSONBuilder.h"

Bool AI::AICSysTool::GetNumber(NN<Data::VariObject> input, UnsafeArray<const UTF8Char> name, OutParam<Double> value)
{
	NN<Data::VariItem> item;
	if (!input->GetItem(name).SetTo(item))
		return false;
	switch (item->GetItemType())
	{
	case Data::VariItem::ItemType::F32:
	case Data::VariItem::ItemType::F64:
	case Data::VariItem::ItemType::I8:
	case Data::VariItem::ItemType::U8:
	case Data::VariItem::ItemType::I16:
	case Data::VariItem::ItemType::U16:
	case Data::VariItem::ItemType::I32:
	case Data::VariItem::ItemType::U32:
	case Data::VariItem::ItemType::I64:
	case Data::VariItem::ItemType::U64:
		break;
	case Data::VariItem::ItemType::Unknown:
	case Data::VariItem::ItemType::Null:
	case Data::VariItem::ItemType::Timestamp:
	case Data::VariItem::ItemType::BOOL:
	case Data::VariItem::ItemType::CStr:
	case Data::VariItem::ItemType::Date:
	case Data::VariItem::ItemType::NI32:
	case Data::VariItem::ItemType::Str:
	case Data::VariItem::ItemType::ByteArr:
	case Data::VariItem::ItemType::Vector:
	case Data::VariItem::ItemType::UUID:
	default:
		return false;
	}
	Double number = item->GetAsF64();
	if (Math::IsNAN(number) || Math::IsInfinity(number))
		return false;
	value.Set(number);
	return true;
}

Bool AI::AICSysTool::GetUInt32(NN<Data::VariObject> input, UnsafeArray<const UTF8Char> name, OutParam<UInt32> value)
{
	Double number;
	if (!GetNumber(input, name, number) || number < 0 || number > 4294967295.0)
		return false;
	UInt32 result = (UInt32)number;
	if ((Double)result != number)
		return false;
	value.Set(result);
	return true;
}

Bool AI::AICSysTool::SetData(NN<Data::VariObject> output, NN<Text::JSONBuilder> json)
{
	NN<Text::String> data = Text::String::New(json->Build());
	output->SetItemStr(U8STR("data"), data);
	data->Release();
	return true;
}

AI::AICSysTool::AICSysTool() : maxToolSize(8192)
{
}

AI::AICSysTool::~AICSysTool()
{
}

Text::CStringNN AI::AICSysTool::GetName() const
{
	return CSTR("coordinate_system");
}

void AI::AICSysTool::GetDescription(NN<Text::StringBuilderUTF8> sb) const
{
	sb->Append(CSTR("List supported coordinate systems, get information by SRID, or convert a point. Results are JSON in data. list_srids returns systems and nextOffset (null on the last page); get_info returns name, type, unit and details; convert_point returns sourceSrid, targetSrid, x and y. The convertible field identifies systems usable for conversion; Cassini-Soldner conversion is unavailable. Coordinates use X=longitude/easting and Y=latitude/northing, regardless of EPSG axis order. Geographic coordinates are in degrees; projected coordinates use the system's units. Conversions use the framework's built-in definitions and do not check areas of use."));
}

void AI::AICSysTool::GetParameters(NN<Net::OpenAIToolParameters> parameters) const
{
	parameters->AddParameter(CSTR("action"), CSTR("list_srids, get_info, or convert_point"), Net::OpenAIToolParameterType::String, true);
	parameters->AddParameter(CSTR("offset"), CSTR("Zero-based list offset, defaults to 0; use nextOffset from the previous page"), Net::OpenAIToolParameterType::Integer, false);
	parameters->AddParameter(CSTR("count"), CSTR("Maximum list entries, 1-100; defaults to 20"), Net::OpenAIToolParameterType::Integer, false);
	parameters->AddParameter(CSTR("srid"), CSTR("Coordinate system SRID; required for get_info"), Net::OpenAIToolParameterType::Integer, false);
	parameters->AddParameter(CSTR("sourceSrid"), CSTR("Source coordinate system SRID; required for convert_point"), Net::OpenAIToolParameterType::Integer, false);
	parameters->AddParameter(CSTR("targetSrid"), CSTR("Target coordinate system SRID; required for convert_point"), Net::OpenAIToolParameterType::Integer, false);
	parameters->AddParameter(CSTR("x"), CSTR("Source longitude or easting; required for convert_point"), Net::OpenAIToolParameterType::Number, false);
	parameters->AddParameter(CSTR("y"), CSTR("Source latitude or northing; required for convert_point"), Net::OpenAIToolParameterType::Number, false);
}

void AI::AICSysTool::SetMaxToolSize(UIntOS maxToolSize)
{
	this->maxToolSize = maxToolSize;
}

Bool AI::AICSysTool::Invoke(Text::CStringNN sessionId, NN<Data::VariObject> input, NN<Data::VariObject> output)
{
	NN<Data::VariItem> actionItem;
	NN<Text::String> action;
	if (!input->GetItem(U8STR("action")).SetTo(actionItem) ||
		(actionItem->GetItemType() != Data::VariItem::ItemType::Str && actionItem->GetItemType() != Data::VariItem::ItemType::CStr) ||
		!actionItem->GetAsNewString().SetTo(action))
	{
		printf("AICSysTool: action is missing or invalid\n");
		return false;
	}
	Bool list = action->Equals(UTF8STRC("list_srids"));
	Bool info = action->Equals(UTF8STRC("get_info"));
	Bool convert = action->Equals(UTF8STRC("convert_point"));
	action->Release();

	if (list)
	{
		UInt32 offset = 0;
		UInt32 count = 20;
		if ((input->HasItem(U8STR("offset")) && !GetUInt32(input, U8STR("offset"), offset)) ||
			(input->HasItem(U8STR("count")) && !GetUInt32(input, U8STR("count"), count)) || count == 0 || count > 100)
		{
			printf("AICSysTool: invalid offset or count\n");
			return false;
		}
		Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
		json.ObjectBeginArray(CSTR("systems"));
		UIntOS index = 0;
		UIntOS supportedIndex = 0;
		UIntOS added = 0;
		NN<const Math::CSys::CoordinateSystemManager::SpatialRefInfo> ref;
		while (Math::CSys::CoordinateSystemManager::SRGetSpatialRefByIndex(index++).SetTo(ref))
		{
			if (ref->srType != Math::CSys::CoordinateSystemManager::SRT_GEOGCS && ref->srType != Math::CSys::CoordinateSystemManager::SRT_PROJCS)
				continue;
			NN<Math::CSys::CoordinateSystem> csys;
			if (!Math::CSys::CoordinateSystemManager::SRCreateCSys(ref->srid).SetTo(csys))
				continue;
			if (supportedIndex++ < offset)
			{
				csys.Delete();
				continue;
			}
			if (added >= count || (added > 0 && json.GetLength() > this->maxToolSize))
			{
				csys.Delete();
				json.ArrayEnd();
				json.ObjectAddUInt64(CSTR("nextOffset"), (UInt64)offset + added);
				return SetData(output, json);
			}
			json.ArrayBeginObject();
			json.ObjectAddUInt64(CSTR("srid"), ref->srid);
			json.ObjectAddStr(CSTR("name"), csys->GetCSysName());
			json.ObjectAddStr(CSTR("type"), csys->IsProjected() ? CSTR("projected") : CSTR("geographic"));
			json.ObjectAddBool(CSTR("convertible"), csys->GetCoordSysType() != Math::CSys::CoordinateSystem::CoordinateSystemType::CassiniSoldner);
			json.ObjectEnd();
			csys.Delete();
			added++;
		}
		json.ArrayEnd();
		json.ObjectAddNull(CSTR("nextOffset"));
		return SetData(output, json);
	}

	if (info)
	{
		UInt32 srid;
		NN<Math::CSys::CoordinateSystem> csys;
		if (!GetUInt32(input, U8STR("srid"), srid) || !Math::CSys::CoordinateSystemManager::SRCreateCSys(srid).SetTo(csys))
		{
			printf("AICSysTool: invalid or unsupported SRID\n");
			return false;
		}
		Text::StringBuilderUTF8 details;
		csys->ToString(details);
		Math::CSys::CoordinateSystem::UnitType unit;
		if (csys->IsProjected())
			unit = NN<Math::CSys::ProjectedCoordinateSystem>::ConvertFrom(csys)->GetUnit();
		else
			unit = NN<Math::CSys::GeographicCoordinateSystem>::ConvertFrom(csys)->GetUnit();
		Text::CStringNN unitName = CSTR("degree");
		switch (unit)
		{
		case Math::CSys::CoordinateSystem::UT_METRE:
			unitName = CSTR("metre");
			break;
		case Math::CSys::CoordinateSystem::UT_CLARKE_FOOT:
			unitName = CSTR("Clarke's foot");
			break;
		case Math::CSys::CoordinateSystem::UT_DEGREE:
			break;
		default:
			csys.Delete();
			printf("AICSysTool: unsupported coordinate system unit\n");
			return false;
		}
		Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
		json.ObjectAddUInt64(CSTR("srid"), srid);
		json.ObjectAddStr(CSTR("name"), csys->GetCSysName());
		json.ObjectAddStr(CSTR("type"), csys->IsProjected() ? CSTR("projected") : CSTR("geographic"));
		json.ObjectAddBool(CSTR("convertible"), csys->GetCoordSysType() != Math::CSys::CoordinateSystem::CoordinateSystemType::CassiniSoldner);
		json.ObjectAddStr(CSTR("unit"), unitName);
		json.ObjectAddStr(CSTR("details"), details.ToCString());
		csys.Delete();
		return SetData(output, json);
	}

	if (convert)
	{
		UInt32 sourceSrid;
		UInt32 targetSrid;
		Double x;
		Double y;
		if (!GetUInt32(input, U8STR("sourceSrid"), sourceSrid) || !GetUInt32(input, U8STR("targetSrid"), targetSrid) ||
			!GetNumber(input, U8STR("x"), x) || !GetNumber(input, U8STR("y"), y))
		{
			printf("AICSysTool: invalid conversion parameters\n");
			return false;
		}
		NN<Math::CSys::CoordinateSystem> source;
		NN<Math::CSys::CoordinateSystem> target;
		if (!Math::CSys::CoordinateSystemManager::SRCreateCSys(sourceSrid).SetTo(source))
		{
			printf("AICSysTool: unsupported source SRID\n");
			return false;
		}
		if (!Math::CSys::CoordinateSystemManager::SRCreateCSys(targetSrid).SetTo(target))
		{
			source.Delete();
			printf("AICSysTool: unsupported target SRID\n");
			return false;
		}
		if (!source->IsProjected() && (y < -90 || y > 90))
		{
			source.Delete();
			target.Delete();
			printf("AICSysTool: latitude is outside -90 to 90 degrees\n");
			return false;
		}
		if (source->GetCoordSysType() == Math::CSys::CoordinateSystem::CoordinateSystemType::CassiniSoldner ||
			target->GetCoordSysType() == Math::CSys::CoordinateSystem::CoordinateSystemType::CassiniSoldner)
		{
			source.Delete();
			target.Delete();
			printf("AICSysTool: Cassini-Soldner conversion is not supported\n");
			return false;
		}
		Math::Coord2DDbl result(x, y);
		if (sourceSrid != targetSrid)
		{
			NN<Math::CSys::GeographicCoordinateSystem> sourceGeo = source->IsProjected() ?
				NN<Math::CSys::ProjectedCoordinateSystem>::ConvertFrom(source)->GetGeographicCoordinateSystem() :
				NN<Math::CSys::GeographicCoordinateSystem>::ConvertFrom(source);
			// CoordinateSystem::Equals compares geographic ellipsoids, not datums.
			if (!target->IsProjected() && sourceGeo->Equals(target) &&
				sourceGeo->GetDatum()->srid != NN<Math::CSys::GeographicCoordinateSystem>::ConvertFrom(target)->GetDatum()->srid)
			{
				if (source->IsProjected())
					result = NN<Math::CSys::ProjectedCoordinateSystem>::ConvertFrom(source)->ToGeographicCoordinateDeg(result);
				Math::Vector3D cartesian = sourceGeo->ToCartesianCoordDeg(Math::Vector3D(result, 0));
				result = NN<Math::CSys::GeographicCoordinateSystem>::ConvertFrom(target)->FromCartesianCoordDeg(cartesian).GetXY();
			}
			else
			{
				result = Math::CSys::CoordinateSystem::Convert(source, target, result);
			}
		}
		source.Delete();
		target.Delete();
		if (Math::IsNAN(result.x) || Math::IsInfinity(result.x) || Math::IsNAN(result.y) || Math::IsInfinity(result.y))
		{
			printf("AICSysTool: conversion produced invalid coordinates\n");
			return false;
		}
		Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
		json.ObjectAddUInt64(CSTR("sourceSrid"), sourceSrid);
		json.ObjectAddUInt64(CSTR("targetSrid"), targetSrid);
		json.ObjectAddFloat64(CSTR("x"), result.x);
		json.ObjectAddFloat64(CSTR("y"), result.y);
		return SetData(output, json);
	}

	printf("AICSysTool: action is invalid\n");
	return false;
}
