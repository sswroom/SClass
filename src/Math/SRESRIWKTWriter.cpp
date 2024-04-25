#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/SRESRIWKTWriter.h"

Math::SRESRIWKTWriter::SRESRIWKTWriter()
{

}

Math::SRESRIWKTWriter::~SRESRIWKTWriter()
{

}

UTF8Char *Math::SRESRIWKTWriter::WriteSRID(UTF8Char *buff, UInt32 srid, UOSInt lev, Text::LineBreakType lbt)
{
	return buff;
}

UTF8Char *Math::SRESRIWKTWriter::WritePrimem(UTF8Char *buff, Math::CoordinateSystem::PrimemType primem, UOSInt lev, Text::LineBreakType lbt)
{
	switch (primem)
	{
	case Math::CoordinateSystem::PT_GREENWICH:
		*buff++ = ',';
		buff = WriteNextLine(buff, lev, lbt);
		buff = Text::StrConcatC(buff, UTF8STRC("PRIMEM[\"Greenwich\",0]"));
		return buff;
	}
	return buff;
}

UTF8Char *Math::SRESRIWKTWriter::WriteUnit(UTF8Char *buff, Math::CoordinateSystem::UnitType unit, UOSInt lev, Text::LineBreakType lbt)
{
	switch (unit)
	{
	case Math::CoordinateSystem::UT_DEGREE:
		*buff++ = ',';
		buff = WriteNextLine(buff, lev, lbt);
		buff = Text::StrConcatC(buff, UTF8STRC("UNIT[\"Degree\",0.017453292519943295]"));
		return buff;
	case Math::CoordinateSystem::UT_METRE:
		*buff++ = ',';
		buff = WriteNextLine(buff, lev, lbt);
		buff = Text::StrConcatC(buff, UTF8STRC("UNIT[\"Meter\",1]"));
		return buff;
	}
	return buff;
}

UTF8Char *Math::SRESRIWKTWriter::WriteProjExtra(UTF8Char *buff, NN<Math::CoordinateSystem> pcsys, UOSInt lev, Text::LineBreakType lbt)
{
	return buff;
}
