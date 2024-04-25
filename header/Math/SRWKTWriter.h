#ifndef _SM_MATH_SRWKTWRITER
#define _SM_MATH_SRWKTWRITER
#include "Math/CoordinateSystem.h"
#include "Text/MyString.h"

namespace Math
{
	class SRWKTWriter
	{
	public:
		SRWKTWriter();
		virtual ~SRWKTWriter();

		UTF8Char *WriteCSys(NN<Math::CoordinateSystem> csys, UTF8Char *buff, UOSInt lev, Text::LineBreakType lbt);
		UTF8Char *WriteDatum(NN<const Math::CoordinateSystem::DatumData1> datum, UTF8Char *buff, UOSInt lev, Text::LineBreakType lbt);
		UTF8Char *WriteSpheroid(const Math::CoordinateSystem::SpheroidData *spheroid, UTF8Char *buff, UOSInt lev, Text::LineBreakType lbt);
		UTF8Char *WriteNextLine(UTF8Char *buff, UOSInt lev, Text::LineBreakType lbt);
		virtual UTF8Char *WriteSRID(UTF8Char *buff, UInt32 srid, UOSInt lev, Text::LineBreakType lbt) = 0;
		virtual UTF8Char *WritePrimem(UTF8Char *buff, Math::CoordinateSystem::PrimemType primem, UOSInt lev, Text::LineBreakType lbt) = 0;
		virtual UTF8Char *WriteUnit(UTF8Char *buff, Math::CoordinateSystem::UnitType unit, UOSInt lev, Text::LineBreakType lbt) = 0;
		virtual UTF8Char *WriteProjExtra(UTF8Char *buff, NN<Math::CoordinateSystem> pcsys, UOSInt lev, Text::LineBreakType lbt) = 0;
	};
}
#endif
