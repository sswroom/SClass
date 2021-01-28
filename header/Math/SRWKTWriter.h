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

		Char *WriteCSys(Math::CoordinateSystem *csys, Char *buff, UOSInt lev, Text::LineBreakType lbt);
		Char *WriteDatum(const Math::CoordinateSystem::DatumData1 *datum, Char *buff, UOSInt lev, Text::LineBreakType lbt);
		Char *WriteNextLine(Char *buff, UOSInt lev, Text::LineBreakType lbt);
		virtual Char *WriteSRID(Char *buff, Int32 srid, UOSInt lev, Text::LineBreakType lbt) = 0;
		virtual Char *WritePrimem(Char *buff, Math::CoordinateSystem::PrimemType primem, UOSInt lev, Text::LineBreakType lbt) = 0;
		virtual Char *WriteUnit(Char *buff, Math::CoordinateSystem::UnitType unit, UOSInt lev, Text::LineBreakType lbt) = 0;
		virtual Char *WriteProjExtra(Char *buff, Math::CoordinateSystem *pcsys, UOSInt lev, Text::LineBreakType lbt) = 0;
	};
}
#endif
