#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/SROGCWKTWriter.h"

Math::SROGCWKTWriter::SROGCWKTWriter()
{

}

Math::SROGCWKTWriter::~SROGCWKTWriter()
{

}

UnsafeArray<UTF8Char> Math::SROGCWKTWriter::WriteSRID(UnsafeArray<UTF8Char> buff, UInt32 srid, UOSInt lev, Text::LineBreakType lbt)
{
	if (srid != 0)
	{
		*buff++ = ',';
		buff = WriteNextLine(buff, lev, lbt);
		buff = Text::StrConcatC(buff, UTF8STRC("AUTHORITY[\"EPSG\",\""));
		buff = Text::StrUInt32(buff, srid);
		buff = Text::StrConcatC(buff, UTF8STRC("\"]"));
	}
	return buff;
}

UnsafeArray<UTF8Char> Math::SROGCWKTWriter::WritePrimem(UnsafeArray<UTF8Char> buff, Math::CoordinateSystem::PrimemType primem, UOSInt lev, Text::LineBreakType lbt)
{
	switch (primem)
	{
	case Math::CoordinateSystem::PT_GREENWICH:
		*buff++ = ',';
		buff = WriteNextLine(buff, lev, lbt);
		buff = Text::StrConcatC(buff, UTF8STRC("PRIMEM[\"Greenwich\",0"));
		buff = this->WriteSRID(buff, primem, lev + 1, lbt);
		*buff++ = ']';
		*buff = 0;
		return buff;
	}
	return buff;
}
UnsafeArray<UTF8Char> Math::SROGCWKTWriter::WriteUnit(UnsafeArray<UTF8Char> buff, Math::CoordinateSystem::UnitType unit, UOSInt lev, Text::LineBreakType lbt)
{
	switch (unit)
	{
	case Math::CoordinateSystem::UT_DEGREE:
		*buff++ = ',';
		buff = WriteNextLine(buff, lev, lbt);
		buff = Text::StrConcatC(buff, UTF8STRC("UNIT[\"degree\",0.01745329251994328"));
		buff = this->WriteSRID(buff, unit, lev + 1, lbt);
		*buff++ = ']';
		*buff = 0;
		return buff;
	case Math::CoordinateSystem::UT_CLARKE_FOOT:
		*buff++ = ',';
		buff = WriteNextLine(buff, lev, lbt);
		buff = Text::StrConcatC(buff, UTF8STRC("UNIT[\"Clarke's foot\",0.3047972654"));
		buff = this->WriteSRID(buff, unit, lev + 1, lbt);
		*buff++ = ']';
		*buff = 0;
		return buff;
	case Math::CoordinateSystem::UT_METRE:
		*buff++ = ',';
		buff = WriteNextLine(buff, lev, lbt);
		buff = Text::StrConcatC(buff, UTF8STRC("UNIT[\"metre\",1"));
		buff = this->WriteSRID(buff, unit, lev + 1, lbt);
		*buff++ = ']';
		*buff = 0;
		return buff;
	}
	return buff;
}

UnsafeArray<UTF8Char> Math::SROGCWKTWriter::WriteProjExtra(UnsafeArray<UTF8Char> buff, NN<Math::CoordinateSystem> pcsys, UOSInt lev, Text::LineBreakType lbt)
{
	*buff++ = ',';
	buff = WriteNextLine(buff, lev, lbt);
	buff = Text::StrConcatC(buff, UTF8STRC("AXIS[\"Easting\",EAST]"));
	*buff++ = ',';
	buff = WriteNextLine(buff, lev, lbt);
	buff = Text::StrConcatC(buff, UTF8STRC("AXIS[\"Northing\",NORTH]"));
	return buff;
}
