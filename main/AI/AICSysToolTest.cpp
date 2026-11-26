#include "Stdafx.h"
#include "AI/AICSysTool.h"
#include "Core/Core.h"
#include "Math/Math_C.h"
#include "Math/CSys/CoordinateSystemManager.h"
#include "Text/JSON.h"
#include <cmath>

#define CHECK(expr) if (!(expr)) { printf("AICSysToolTest: failed at line %d\n", __LINE__); return 1; }

static Optional<Text::JSONBase> GetResult(NN<Data::VariObject> output)
{
	NN<Data::VariItem> item;
	NN<Text::String> data;
	if (!output->GetItem(U8STR("data")).SetTo(item) || !item->GetAsNewString().SetTo(data))
		return nullptr;
	Optional<Text::JSONBase> result = Text::JSONBase::ParseJSONStr(data->ToCString());
	data->Release();
	return result;
}

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	AI::AICSysTool tool;
	Data::VariObject input(Data::VariObject::NameType::Field);
	Data::VariObject output(Data::VariObject::NameType::Field);
	input.SetItemStr(U8STR("action"), U8STR("list_srids"));
	input.SetItemU32(U8STR("count"), 3);
	tool.SetMaxToolSize(180);
	UInt32 offset = 0;
	UIntOS registryIndex = 0;
	UIntOS total = 0;
	Bool has4326 = false;
	Bool has3857 = false;
	while (true)
	{
		input.SetItemU32(U8STR("offset"), offset);
		CHECK(tool.Invoke(CSTR("test"), input, output));
		NN<Text::JSONBase> json;
		NN<Text::JSONArray> systems;
		CHECK(GetResult(output).SetTo(json));
		CHECK(json->GetValueArray(CSTR("systems")).SetTo(systems));
		CHECK(systems->GetArrayLength() > 0 && systems->GetArrayLength() <= 3);
		UIntOS i = 0;
		while (i < systems->GetArrayLength())
		{
			NN<Text::JSONObject> item;
			NN<const Math::CSys::CoordinateSystemManager::SpatialRefInfo> ref;
			CHECK(systems->GetArrayObject(i).SetTo(item));
			while (true)
			{
				CHECK(Math::CSys::CoordinateSystemManager::SRGetSpatialRefByIndex(registryIndex++).SetTo(ref));
				NN<Math::CSys::CoordinateSystem> csys;
				if ((ref->srType == Math::CSys::CoordinateSystemManager::SRT_GEOGCS || ref->srType == Math::CSys::CoordinateSystemManager::SRT_PROJCS) &&
					Math::CSys::CoordinateSystemManager::SRCreateCSys(ref->srid).SetTo(csys))
				{
					csys.Delete();
					break;
				}
			}
			CHECK(item->GetObjectInt64(CSTR("srid")) == ref->srid);
			has4326 = has4326 || ref->srid == 4326;
			has3857 = has3857 || ref->srid == 3857;
			total++;
			i++;
		}
		if (json->GetValueType(CSTR("nextOffset")) == Text::JSONType::Null)
		{
			json->EndUse();
			break;
		}
		CHECK(json->GetValueAsInt64(CSTR("nextOffset")) == (Int64)total);
		offset = (UInt32)total;
		json->EndUse();
	}
	NN<const Math::CSys::CoordinateSystemManager::SpatialRefInfo> ref;
	while (Math::CSys::CoordinateSystemManager::SRGetSpatialRefByIndex(registryIndex++).SetTo(ref))
	{
		NN<Math::CSys::CoordinateSystem> csys;
		CHECK(!Math::CSys::CoordinateSystemManager::SRCreateCSys(ref->srid).SetTo(csys));
	}
	CHECK(total > 3 && has4326 && has3857);

	tool.SetMaxToolSize(8192);
	input.SetItemStr(U8STR("action"), U8STR("get_info"));
	input.SetItemU32(U8STR("srid"), 4326);
	CHECK(tool.Invoke(CSTR("test"), input, output));
	NN<Text::JSONBase> info;
	NN<Text::String> name;
	CHECK(GetResult(output).SetTo(info));
	CHECK(info->GetValueAsInt64(CSTR("srid")) == 4326);
	CHECK(info->GetValueString(CSTR("name")).SetTo(name) && name->Equals(UTF8STRC("WGS 84")));
	CHECK(info->GetValueString(CSTR("unit")).SetTo(name) && name->Equals(UTF8STRC("degree")));
	CHECK(info->GetValueString(CSTR("details")).NotNull());
	info->EndUse();
	input.SetItemU32(U8STR("srid"), 3407);
	CHECK(tool.Invoke(CSTR("test"), input, output));
	CHECK(GetResult(output).SetTo(info));
	CHECK(info->GetValueString(CSTR("unit")).SetTo(name) && name->Equals(UTF8STRC("Clarke's foot")));
	CHECK(!info->GetValueAsBool(CSTR("convertible")));
	info->EndUse();
	input.SetItemU32(U8STR("srid"), 6326);
	CHECK(!tool.Invoke(CSTR("test"), input, output));
	input.SetItemF64(U8STR("srid"), 4326.5);
	CHECK(!tool.Invoke(CSTR("test"), input, output));
	input.SetItemU64(U8STR("srid"), 4294967296ULL);
	CHECK(!tool.Invoke(CSTR("test"), input, output));
	input.SetItemStr(U8STR("srid"), U8STR("4326"));
	CHECK(!tool.Invoke(CSTR("test"), input, output));

	input.SetItemStr(U8STR("action"), U8STR("convert_point"));
	input.SetItemU32(U8STR("sourceSrid"), 4326);
	input.SetItemU32(U8STR("targetSrid"), 3857);
	input.SetItemF64(U8STR("x"), 10);
	input.SetItemF64(U8STR("y"), 10);
	CHECK(tool.Invoke(CSTR("test"), input, output));
	NN<Text::JSONBase> point;
	CHECK(GetResult(output).SetTo(point));
	Double x = point->GetValueAsDoubleOrNAN(CSTR("x"));
	Double y = point->GetValueAsDoubleOrNAN(CSTR("y"));
	CHECK(std::fabs(x - 1113194.90793274) < 0.1);
	CHECK(std::fabs(y - 1118889.97485796) < 0.1);
	point->EndUse();
	input.SetItemU32(U8STR("sourceSrid"), 3857);
	input.SetItemU32(U8STR("targetSrid"), 4326);
	input.SetItemF64(U8STR("x"), x);
	input.SetItemF64(U8STR("y"), y);
	CHECK(tool.Invoke(CSTR("test"), input, output));
	CHECK(GetResult(output).SetTo(point));
	CHECK(std::fabs(point->GetValueAsDoubleOrNAN(CSTR("x")) - 10) < 0.000001);
	CHECK(std::fabs(point->GetValueAsDoubleOrNAN(CSTR("y")) - 10) < 0.000001);
	point->EndUse();
	input.SetItemU32(U8STR("targetSrid"), 3857);
	CHECK(tool.Invoke(CSTR("test"), input, output));
	CHECK(GetResult(output).SetTo(point));
	CHECK(point->GetValueAsDoubleOrNAN(CSTR("x")) == x && point->GetValueAsDoubleOrNAN(CSTR("y")) == y);
	point->EndUse();
	input.SetItemU32(U8STR("targetSrid"), 123456);
	CHECK(!tool.Invoke(CSTR("test"), input, output));
	input.SetItemU32(U8STR("targetSrid"), 3407);
	CHECK(!tool.Invoke(CSTR("test"), input, output));
	input.SetItemU32(U8STR("targetSrid"), 4326);
	input.SetItemF64(U8STR("x"), Math::GetInfinity());
	CHECK(!tool.Invoke(CSTR("test"), input, output));
	input.SetItemF64(U8STR("x"), 10);
	input.SetItemU32(U8STR("sourceSrid"), 4326);
	input.SetItemF64(U8STR("y"), 91);
	CHECK(!tool.Invoke(CSTR("test"), input, output));
	input.SetItemF64(U8STR("y"), 10);
	input.SetItemU32(U8STR("sourceSrid"), 4611);
	input.SetItemU32(U8STR("targetSrid"), 4236);
	input.SetItemF64(U8STR("x"), 114);
	input.SetItemF64(U8STR("y"), 22);
	CHECK(tool.Invoke(CSTR("test"), input, output));
	CHECK(GetResult(output).SetTo(point));
	NN<Math::CSys::GeographicCoordinateSystem> sourceGeo;
	NN<Math::CSys::GeographicCoordinateSystem> targetGeo;
	CHECK(Math::CSys::CoordinateSystemManager::SRCreateGeogCSys(4611).SetTo(sourceGeo));
	CHECK(Math::CSys::CoordinateSystemManager::SRCreateGeogCSys(4236).SetTo(targetGeo));
	Math::Vector3D expected = targetGeo->FromCartesianCoordDeg(sourceGeo->ToCartesianCoordDeg(Math::Vector3D(114, 22, 0)));
	CHECK(std::fabs(point->GetValueAsDoubleOrNAN(CSTR("x")) - expected.GetX()) < 0.0000001);
	CHECK(std::fabs(point->GetValueAsDoubleOrNAN(CSTR("y")) - expected.GetY()) < 0.0000001);
	CHECK(std::fabs(point->GetValueAsDoubleOrNAN(CSTR("x")) - 114) > 0.00001);
	point->EndUse();
	sourceGeo.Delete();
	targetGeo.Delete();
	NN<Math::CSys::ProjectedCoordinateSystem> sourceProj;
	CHECK(Math::CSys::CoordinateSystemManager::SRCreateProjCSys(2326).SetTo(sourceProj));
	CHECK(Math::CSys::CoordinateSystemManager::SRCreateGeogCSys(4236).SetTo(targetGeo));
	Math::Coord2DDbl geographic = sourceProj->ToGeographicCoordinateDeg(Math::Coord2DDbl(836694.05, 819069.8));
	expected = targetGeo->FromCartesianCoordDeg(sourceProj->GetGeographicCoordinateSystem()->ToCartesianCoordDeg(Math::Vector3D(geographic, 0)));
	sourceProj.Delete();
	targetGeo.Delete();
	input.SetItemU32(U8STR("sourceSrid"), 2326);
	input.SetItemF64(U8STR("x"), 836694.05);
	input.SetItemF64(U8STR("y"), 819069.8);
	CHECK(tool.Invoke(CSTR("test"), input, output));
	CHECK(GetResult(output).SetTo(point));
	CHECK(std::fabs(point->GetValueAsDoubleOrNAN(CSTR("x")) - expected.GetX()) < 0.0000001);
	CHECK(std::fabs(point->GetValueAsDoubleOrNAN(CSTR("y")) - expected.GetY()) < 0.0000001);
	point->EndUse();
	tool.SetMaxToolSize(10);
	CHECK(tool.Invoke(CSTR("test"), input, output));
	CHECK(GetResult(output).SetTo(point));
	CHECK(std::fabs(point->GetValueAsDoubleOrNAN(CSTR("x")) - expected.GetX()) < 0.0000001);
	point->EndUse();
	input.SetItemStr(U8STR("action"), U8STR("get_info"));
	input.SetItemU32(U8STR("srid"), 4326);
	CHECK(tool.Invoke(CSTR("test"), input, output));
	CHECK(GetResult(output).SetTo(info));
	CHECK(info->GetValueAsInt64(CSTR("srid")) == 4326 && info->GetValueString(CSTR("details")).NotNull());
	info->EndUse();
	input.SetItemStr(U8STR("action"), U8STR("list_srids"));
	input.SetItemU32(U8STR("offset"), 0);
	CHECK(tool.Invoke(CSTR("test"), input, output));
	NN<Text::JSONBase> smallList;
	NN<Text::JSONArray> systems;
	CHECK(GetResult(output).SetTo(smallList));
	CHECK(smallList->GetValueArray(CSTR("systems")).SetTo(systems) && systems->GetArrayLength() == 1);
	CHECK(smallList->GetValueAsInt64(CSTR("nextOffset")) == 1);
	smallList->EndUse();
	input.SetItemU32(U8STR("offset"), 4294967295U);
	CHECK(tool.Invoke(CSTR("test"), input, output));
	CHECK(GetResult(output).SetTo(smallList));
	CHECK(smallList->GetValueArray(CSTR("systems")).SetTo(systems) && systems->GetArrayLength() == 0);
	CHECK(smallList->GetValueType(CSTR("nextOffset")) == Text::JSONType::Null);
	smallList->EndUse();
	input.SetItemU32(U8STR("count"), 0);
	CHECK(!tool.Invoke(CSTR("test"), input, output));
	return 0;
}
