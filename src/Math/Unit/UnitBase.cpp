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
	case ValueType::Mapping:
		return CSTR("");
	case ValueType::ElectricPotential:
		return Math::Unit::ElectricPotential::GetUnitShortName((Math::Unit::ElectricPotential::ElectricPotentialUnit)unit);
	case ValueType::ElectricCurrent:
		return Math::Unit::ElectricCurrent::GetUnitShortName((Math::Unit::ElectricCurrent::ElectricCurrentUnit)unit);
	case ValueType::Power:
		return Math::Unit::Power::GetUnitShortName((Math::Unit::Power::PowerUnit)unit);
	case ValueType::Angle:
		return Math::Unit::Angle::GetUnitShortName((Math::Unit::Angle::AngleUnit)unit);
	case ValueType::Frequency:
		return Math::Unit::Frequency::GetUnitShortName((Math::Unit::Frequency::FrequencyUnit)unit);
	case ValueType::Ratio:
		return Math::Unit::Ratio::GetUnitShortName((Math::Unit::Ratio::RatioUnit)unit);
	case ValueType::ApparentPower:
		return Math::Unit::ApparentPower::GetUnitShortName((Math::Unit::ApparentPower::ApparentPowerUnit)unit);
	case ValueType::ReactivePower:
		return Math::Unit::ReactivePower::GetUnitShortName((Math::Unit::ReactivePower::ReactivePowerUnit)unit);
	case ValueType::Energy:
		return Math::Unit::Energy::GetUnitShortName((Math::Unit::Energy::EnergyUnit)unit);
	case ValueType::ReactiveEnergy:
		return Math::Unit::ReactiveEnergy::GetUnitShortName((Math::Unit::ReactiveEnergy::ReactiveEnergyUnit)unit);
	case ValueType::Count:
		return Math::Unit::Count::GetUnitShortName((Math::Unit::Count::CountUnit)unit);
	case ValueType::Temperature:
		return Math::Unit::Temperature::GetUnitShortName((Math::Unit::Temperature::TemperatureUnit)unit);
	default:
		return CSTR("");
	}
}
