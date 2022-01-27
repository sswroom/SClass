#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Unit/Angle.h"
#include "Math/Unit/ApparentPower.h"
#include "Math/Unit/Count.h"
#include "Math/Unit/ElectricCurrent.h"
#include "Math/Unit/ElectricPotential.h"
#include "Math/Unit/Energy.h"
#include "Math/Unit/Frequency.h"
#include "Math/Unit/Power.h"
#include "Math/Unit/Ratio.h"
#include "Math/Unit/ReactiveEnergy.h"
#include "Math/Unit/ReactivePower.h"
#include "Math/Unit/Temperature.h"
#include "Math/Unit/UnitBase.h"

Text::CString Math::Unit::UnitBase::GetUnitShortName(ValueType vt, Int32 unit)
{
	switch (vt)
	{
	case VT_MAPPING:
		return CSTR("");
	case VT_ELECTRIC_POTENTIAL:
		return Math::Unit::ElectricPotential::GetUnitShortName((Math::Unit::ElectricPotential::ElectricPotentialUnit)unit);
	case VT_ELECTRIC_CURRENT:
		return Math::Unit::ElectricCurrent::GetUnitShortName((Math::Unit::ElectricCurrent::ElectricCurrentUnit)unit);
	case VT_POWER:
		return Math::Unit::Power::GetUnitShortName((Math::Unit::Power::PowerUnit)unit);
	case VT_ANGLE:
		return Math::Unit::Angle::GetUnitShortName((Math::Unit::Angle::AngleUnit)unit);
	case VT_FREQUENCY:
		return Math::Unit::Frequency::GetUnitShortName((Math::Unit::Frequency::FrequencyUnit)unit);
	case VT_RATIO:
		return Math::Unit::Ratio::GetUnitShortName((Math::Unit::Ratio::RatioUnit)unit);
	case VT_APPARENT_POWER:
		return Math::Unit::ApparentPower::GetUnitShortName((Math::Unit::ApparentPower::ApparentPowerUnit)unit);
	case VT_REACTIVE_POWER:
		return Math::Unit::ReactivePower::GetUnitShortName((Math::Unit::ReactivePower::ReactivePowerUnit)unit);
	case VT_ENERGY:
		return Math::Unit::Energy::GetUnitShortName((Math::Unit::Energy::EnergyUnit)unit);
	case VT_REACTIVE_ENERGY:
		return Math::Unit::ReactiveEnergy::GetUnitShortName((Math::Unit::ReactiveEnergy::ReactiveEnergyUnit)unit);
	case VT_COUNT:
		return Math::Unit::Count::GetUnitShortName((Math::Unit::Count::CountUnit)unit);
	case VT_TEMPERATURE:
		return Math::Unit::Temperature::GetUnitShortName((Math::Unit::Temperature::TemperatureUnit)unit);
	default:
		return CSTR("");
	}
}
