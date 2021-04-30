#ifndef _SM_MATH_SROGCWKTWRITER
#define _SM_MATH_SROGCWKTWRITER
#include "Math/SRWKTWriter.h"

namespace Math
{
	class SROGCWKTWriter : public SRWKTWriter
	{
	public:
		SROGCWKTWriter();
		virtual ~SROGCWKTWriter();

		virtual Char *WriteSRID(Char *buff, UInt32 srid, UOSInt lev, Text::LineBreakType lbt);
		virtual Char *WritePrimem(Char *buff, Math::CoordinateSystem::PrimemType primem, UOSInt lev, Text::LineBreakType lbt);
		virtual Char *WriteUnit(Char *buff, Math::CoordinateSystem::UnitType unit, UOSInt lev, Text::LineBreakType lbt);
		virtual Char *WriteProjExtra(Char *buff, Math::CoordinateSystem *pcsys, UOSInt lev, Text::LineBreakType lbt);
	};
}
#endif
