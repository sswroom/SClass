#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/SRESRIWKTWriter.h"

Math::SRESRIWKTWriter::SRESRIWKTWriter()
{

}

Math::SRESRIWKTWriter::~SRESRIWKTWriter()
{

}

Char *Math::SRESRIWKTWriter::WriteSRID(Char *buff, UInt32 srid, UOSInt lev, Text::LineBreakType lbt)
{
	return buff;
}

Char *Math::SRESRIWKTWriter::WritePrimem(Char *buff, Math::CoordinateSystem::PrimemType primem, UOSInt lev, Text::LineBreakType lbt)
{
	switch (primem)
	{
	case Math::CoordinateSystem::PT_GREENWICH:
		*buff++ = ',';
		buff = WriteNextLine(buff, lev, lbt);
		buff = Text::StrConcat(buff, "PRIMEM[\"Greenwich\",0]");
		return buff;
	}
	return buff;
}
Char *Math::SRESRIWKTWriter::WriteUnit(Char *buff, Math::CoordinateSystem::UnitType unit, UOSInt lev, Text::LineBreakType lbt)
{
	switch (unit)
	{
	case Math::CoordinateSystem::UT_DEGREE:
		*buff++ = ',';
		buff = WriteNextLine(buff, lev, lbt);
		buff = Text::StrConcat(buff, "UNIT[\"Degree\",0.017453292519943295]");
		return buff;
	case Math::CoordinateSystem::UT_METRE:
		*buff++ = ',';
		buff = WriteNextLine(buff, lev, lbt);
		buff = Text::StrConcat(buff, "UNIT[\"Meter\",1]");
		return buff;
	}
	return buff;
}

Char *Math::SRESRIWKTWriter::WriteProjExtra(Char *buff, Math::CoordinateSystem *pcsys, UOSInt lev, Text::LineBreakType lbt)
{
	return buff;
}
