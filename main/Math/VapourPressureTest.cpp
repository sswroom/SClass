#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Math/Unit/Pressure.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	Text::StringBuilderUTF8 sb;
	sb.ClearStr();
	sb.Append(CSTR("1 Pa = "));
	sb.AppendDouble(Math::Unit::Pressure::Convert(Math::Unit::Pressure::PU_PASCAL, Math::Unit::Pressure::PU_BAR, 1));
	sb.AppendUTF8Char(' ');
	sb.Append(Math::Unit::Pressure::GetUnitShortName(Math::Unit::Pressure::PU_BAR));
	console.WriteLine(sb.ToCString());

	sb.ClearStr();
	sb.Append(CSTR("1 Pa = "));
	sb.AppendDouble(Math::Unit::Pressure::Convert(Math::Unit::Pressure::PU_PASCAL, Math::Unit::Pressure::PU_ATM, 1));
	sb.AppendUTF8Char(' ');
	sb.Append(Math::Unit::Pressure::GetUnitShortName(Math::Unit::Pressure::PU_ATM));
	console.WriteLine(sb.ToCString());

	sb.ClearStr();
	sb.Append(CSTR("1 Pa = "));
	sb.AppendDouble(Math::Unit::Pressure::Convert(Math::Unit::Pressure::PU_PASCAL, Math::Unit::Pressure::PU_TORR, 1));
	sb.AppendUTF8Char(' ');
	sb.Append(Math::Unit::Pressure::GetUnitShortName(Math::Unit::Pressure::PU_TORR));
	console.WriteLine(sb.ToCString());

	sb.ClearStr();
	sb.Append(CSTR("1 Pa = "));
	sb.AppendDouble(Math::Unit::Pressure::Convert(Math::Unit::Pressure::PU_PASCAL, Math::Unit::Pressure::PU_PSI, 1));
	sb.AppendUTF8Char(' ');
	sb.Append(Math::Unit::Pressure::GetUnitShortName(Math::Unit::Pressure::PU_PSI));
	console.WriteLine(sb.ToCString());

	sb.ClearStr();
	sb.Append(CSTR("PH2O (70C) (Antoine) = "));
	sb.AppendDouble(Math::Unit::Pressure::VapourPressureAntoine(Math::Unit::Pressure::PU_PASCAL, Math::Unit::Temperature::TU_CELSIUS, 70, Math::Unit::Substance::ST_WATER));
	console.WriteLine(sb.ToCString());

	sb.ClearStr();
	sb.Append(CSTR("PH2O (70C) (Tetens) = "));
	sb.AppendDouble(Math::Unit::Pressure::WaterVapourPressureTetens(Math::Unit::Pressure::PU_PASCAL, Math::Unit::Temperature::TU_CELSIUS, 70));
	console.WriteLine(sb.ToCString());

	sb.ClearStr();
	sb.Append(CSTR("PH2O (70C) (Buck) = "));
	sb.AppendDouble(Math::Unit::Pressure::WaterVapourPressureBuck(Math::Unit::Pressure::PU_PASCAL, Math::Unit::Temperature::TU_CELSIUS, 70));
	console.WriteLine(sb.ToCString());

	sb.ClearStr();
	sb.Append(CSTR("PH2O (80C) (Antoine) = "));
	sb.AppendDouble(Math::Unit::Pressure::VapourPressureAntoine(Math::Unit::Pressure::PU_PASCAL, Math::Unit::Temperature::TU_CELSIUS, 80, Math::Unit::Substance::ST_WATER));
	console.WriteLine(sb.ToCString());

	sb.ClearStr();
	sb.Append(CSTR("PH2O (80C) (Tetens) = "));
	sb.AppendDouble(Math::Unit::Pressure::WaterVapourPressureTetens(Math::Unit::Pressure::PU_PASCAL, Math::Unit::Temperature::TU_CELSIUS, 80));
	console.WriteLine(sb.ToCString());

	sb.ClearStr();
	sb.Append(CSTR("PH2O (80C) (Buck) = "));
	sb.AppendDouble(Math::Unit::Pressure::WaterVapourPressureBuck(Math::Unit::Pressure::PU_PASCAL, Math::Unit::Temperature::TU_CELSIUS, 80));
	console.WriteLine(sb.ToCString());
	return 0;
}
