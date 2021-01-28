#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/SROGCWKTWriter.h"

Math::SROGCWKTWriter::SROGCWKTWriter()
{

}

Math::SROGCWKTWriter::~SROGCWKTWriter()
{

}

Char *Math::SROGCWKTWriter::WriteSRID(Char *buff, Int32 srid, UOSInt lev, Text::LineBreakType lbt)
{
	if (srid != 0)
	{
		*buff++ = ',';
		buff = WriteNextLine(buff, lev, lbt);
		buff = Text::StrConcat(buff, "AUTHORITY[\"EPSG\",\"");
		buff = Text::StrInt32(buff, srid);
		buff = Text::StrConcat(buff, "\"]");
	}
	return buff;
}

Char *Math::SROGCWKTWriter::WritePrimem(Char *buff, Math::CoordinateSystem::PrimemType primem, UOSInt lev, Text::LineBreakType lbt)
{
	switch (primem)
	{
	case Math::CoordinateSystem::PT_GREENWICH:
		*buff++ = ',';
		buff = WriteNextLine(buff, lev, lbt);
		buff = Text::StrConcat(buff, "PRIMEM[\"Greenwich\",0");
		buff = this->WriteSRID(buff, primem, lev + 1, lbt);
		*buff++ = ']';
		*buff = 0;
		return buff;
	}
	return buff;
}
Char *Math::SROGCWKTWriter::WriteUnit(Char *buff, Math::CoordinateSystem::UnitType unit, UOSInt lev, Text::LineBreakType lbt)
{
	switch (unit)
	{
	case Math::CoordinateSystem::UT_DEGREE:
		*buff++ = ',';
		buff = WriteNextLine(buff, lev, lbt);
		buff = Text::StrConcat(buff, "UNIT[\"degree\",0.01745329251994328");
		buff = this->WriteSRID(buff, unit, lev + 1, lbt);
		*buff++ = ']';
		*buff = 0;
		return buff;
	case Math::CoordinateSystem::UT_METRE:
		*buff++ = ',';
		buff = WriteNextLine(buff, lev, lbt);
		buff = Text::StrConcat(buff, "UNIT[\"metre\",1");
		buff = this->WriteSRID(buff, unit, lev + 1, lbt);
		*buff++ = ']';
		*buff = 0;
		return buff;
	}
	return buff;
}

Char *Math::SROGCWKTWriter::WriteProjExtra(Char *buff, Math::CoordinateSystem *pcsys, UOSInt lev, Text::LineBreakType lbt)
{
	*buff++ = ',';
	buff = WriteNextLine(buff, lev, lbt);
	buff = Text::StrConcat(buff, "AXIS[\"Easting\",EAST]");
	*buff++ = ',';
	buff = WriteNextLine(buff, lev, lbt);
	buff = Text::StrConcat(buff, "AXIS[\"Northing\",NORTH]");
	return buff;
}
