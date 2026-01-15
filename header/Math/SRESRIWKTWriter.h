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

		virtual UnsafeArray<UTF8Char> WriteSRID(UnsafeArray<UTF8Char> buff, UInt32 srid, UIntOS lev, Text::LineBreakType lbt);
		virtual UnsafeArray<UTF8Char> WritePrimem(UnsafeArray<UTF8Char> buff, Math::CoordinateSystem::PrimemType primem, UIntOS lev, Text::LineBreakType lbt);
		virtual UnsafeArray<UTF8Char> WriteUnit(UnsafeArray<UTF8Char> buff, Math::CoordinateSystem::UnitType unit, UIntOS lev, Text::LineBreakType lbt);
		virtual UnsafeArray<UTF8Char> WriteProjExtra(UnsafeArray<UTF8Char> buff, NN<Math::CoordinateSystem> pcsys, UIntOS lev, Text::LineBreakType lbt);
	};
}
#endif
