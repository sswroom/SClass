#ifndef _SM_MATH_SRESRIWKTWRITER
#define _SM_MATH_SRESRIWKTWRITER
#include "Math/SRWKTWriter.h"

namespace Math
{
	class SRESRIWKTWriter : public SRWKTWriter
	{
	public:
		SRESRIWKTWriter();
		virtual ~SRESRIWKTWriter();

		virtual UTF8Char *WriteSRID(UTF8Char *buff, UInt32 srid, UOSInt lev, Text::LineBreakType lbt);
		virtual UTF8Char *WritePrimem(UTF8Char *buff, Math::CoordinateSystem::PrimemType primem, UOSInt lev, Text::LineBreakType lbt);
		virtual UTF8Char *WriteUnit(UTF8Char *buff, Math::CoordinateSystem::UnitType unit, UOSInt lev, Text::LineBreakType lbt);
		virtual UTF8Char *WriteProjExtra(UTF8Char *buff, NotNullPtr<Math::CoordinateSystem> pcsys, UOSInt lev, Text::LineBreakType lbt);
	};
}
#endif
