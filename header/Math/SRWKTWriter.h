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

		UnsafeArray<UTF8Char> WriteCSys(NN<Math::CoordinateSystem> csys, UnsafeArray<UTF8Char> buff, UIntOS lev, Text::LineBreakType lbt);
		UnsafeArray<UTF8Char> WriteDatum(NN<const Math::CoordinateSystem::DatumData1> datum, UnsafeArray<UTF8Char> buff, UIntOS lev, Text::LineBreakType lbt);
		UnsafeArray<UTF8Char> WriteSpheroid(const Math::CoordinateSystem::SpheroidData *spheroid, UnsafeArray<UTF8Char> buff, UIntOS lev, Text::LineBreakType lbt);
		UnsafeArray<UTF8Char> WriteNextLine(UnsafeArray<UTF8Char> buff, UIntOS lev, Text::LineBreakType lbt);
		virtual UnsafeArray<UTF8Char> WriteSRID(UnsafeArray<UTF8Char> buff, UInt32 srid, UIntOS lev, Text::LineBreakType lbt) = 0;
		virtual UnsafeArray<UTF8Char> WritePrimem(UnsafeArray<UTF8Char> buff, Math::CoordinateSystem::PrimemType primem, UIntOS lev, Text::LineBreakType lbt) = 0;
		virtual UnsafeArray<UTF8Char> WriteUnit(UnsafeArray<UTF8Char> buff, Math::CoordinateSystem::UnitType unit, UIntOS lev, Text::LineBreakType lbt) = 0;
		virtual UnsafeArray<UTF8Char> WriteProjExtra(UnsafeArray<UTF8Char> buff, NN<Math::CoordinateSystem> pcsys, UIntOS lev, Text::LineBreakType lbt) = 0;
	};
}
#endif
