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

		virtual UTF8Char *WriteSRID(UTF8Char *buff, UInt32 srid, UOSInt lev, Text::LineBreakType lbt);
		virtual UTF8Char *WritePrimem(UTF8Char *buff, Math::CoordinateSystem::PrimemType primem, UOSInt lev, Text::LineBreakType lbt);
		virtual UTF8Char *WriteUnit(UTF8Char *buff, Math::CoordinateSystem::UnitType unit, UOSInt lev, Text::LineBreakType lbt);
		virtual UTF8Char *WriteProjExtra(UTF8Char *buff, NN<Math::CoordinateSystem> pcsys, UOSInt lev, Text::LineBreakType lbt);
	};
}
#endif
