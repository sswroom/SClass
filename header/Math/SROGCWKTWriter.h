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

		virtual UnsafeArray<UTF8Char> WriteSRID(UnsafeArray<UTF8Char> buff, UInt32 srid, UOSInt lev, Text::LineBreakType lbt);
		virtual UnsafeArray<UTF8Char> WritePrimem(UnsafeArray<UTF8Char> buff, Math::CoordinateSystem::PrimemType primem, UOSInt lev, Text::LineBreakType lbt);
		virtual UnsafeArray<UTF8Char> WriteUnit(UnsafeArray<UTF8Char> buff, Math::CoordinateSystem::UnitType unit, UOSInt lev, Text::LineBreakType lbt);
		virtual UnsafeArray<UTF8Char> WriteProjExtra(UnsafeArray<UTF8Char> buff, NN<Math::CoordinateSystem> pcsys, UOSInt lev, Text::LineBreakType lbt);
	};
}
#endif
