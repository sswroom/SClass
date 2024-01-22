export class UnitInfo<UnitType>
{
	unit: UnitType;
	symbol: string;
	name: string;
	ratio: number;
	scale: number;

	constructor(unit: any, symbol: string, name: string, ratio: number, scale?: number);
}

module Acceleration
{
	enum Unit
	{
		METER_PER_SECOND_SQUARED,
		STANDARD_GRAVITY
	}
}

export class Acceleration
{
	static getUnitInfo(u: Acceleration.Unit): UnitInfo<Acceleration.Unit> | null;
	static getUnitRatio(u: Acceleration.Unit) : number;
	static convert(fromUnit: Acceleration.Unit, toUnit: Acceleration.Unit, fromValue: number): number;
}

module Angle
{
	enum Unit
	{
		RADIAN,
		GRADIAN,
		TURN,
		DEGREE,
		ARCMINUTE,
		ARCSECOND,
		MILLIARCSECOND,
		MICROARCSECOND
	}
}

export class Angle
{
	static getUnitInfo(u: Angle.Unit): UnitInfo<Angle.Unit> | null;
	static getUnitRatio(u: Angle.Unit) : number;
	static convert(fromUnit: Angle.Unit, toUnit: Angle.Unit, fromValue: number): number;
}

module ApparentPower
{
	enum Unit
	{
		VOLT_AMPERE
	}
}

export class ApparentPower
{
	static getUnitInfo(u: ApparentPower.Unit): UnitInfo<ApparentPower.Unit> | null;
	static getUnitRatio(u: ApparentPower.Unit) : number;
	static convert(fromUnit: ApparentPower.Unit, toUnit: ApparentPower.Unit, fromValue: number): number;
}

module Count
{
	enum Unit
	{
		UNIT,
		K_UNIT,
		KI_UNIT,
		M_UNIT,
		MI_UNIT,
		G_UNIT,
		GI_UNIT,
		T_UNIT,
		TI_UNIT
	}
}

export class Count
{
	static getUnitInfo(u: Count.Unit): UnitInfo<Count.Unit> | null;
	static getUnitRatio(u: Count.Unit) : number;
	static convert(fromUnit: Count.Unit, toUnit: Count.Unit, fromValue: number): number;
}

module Distance
{
	enum Unit
	{
		METER,
		CENTIMETER,
		MILLIMETER,
		MICROMETER,
		NANOMETER,
		PICOMETER,
		KILOMETER,
		INCH,
		FOOT,
		YARD,
		MILE,
		NAUTICAL_MILE,
		AU,
		LIGHTSECOND,
		LIGHTMINUTE,
		LIGHTHOUR,
		LIGHTDAY,
		LIGHTWEEK,
		LIGHTYEAR,
		EMU,
		POINT,
		PIXEL,
		TWIP
	}
}

export class Distance
{
	static getUnitInfo(u: Distance.Unit): UnitInfo<Distance.Unit> | null;
	static getUnitRatio(u: Distance.Unit): number;
	static convert(fromUnit: Distance.Unit, toUnit: Distance.Unit, fromValue: number): number;
}

module ElectricCurrent
{
	enum Unit
	{
		AMPERE,
		MILLIAMPERE,
		MICROAMPERE
	}
}

export class ElectricCurrent
{
	static getUnitInfo(u: ElectricCurrent.Unit): UnitInfo<ElectricCurrent.Unit> | null;
	static getUnitRatio(u: ElectricCurrent.Unit): number;
	static convert(fromUnit: ElectricCurrent.Unit, toUnit: ElectricCurrent.Unit, fromValue: number): number;
}

module ElectricPotential
{
	enum Unit
	{
		VOLT,
		MILLIVOLT
	}
}

export class ElectricPotential
{
	static getUnitInfo(u: ElectricPotential.Unit): UnitInfo<ElectricPotential.Unit> | null;
	static getUnitRatio(u: ElectricPotential.Unit): number;
	static convert(fromUnit: ElectricPotential.Unit, toUnit: ElectricPotential.Unit, fromValue: number): number;
}

module Energy
{
	enum Unit
	{
		JOULE,
		WATTHOUR,
		KILOWATTHOUR,
		CALORIE,
		KILOCALORIE
	}
}

export class Energy
{
	static getUnitInfo(u: Energy.Unit): UnitInfo<Energy.Unit> | null;
	static getUnitRatio(u: Energy.Unit): number;
	static convert(fromUnit: Energy.Unit, toUnit: Energy.Unit, fromValue: number): number;
}

module Force
{
	enum Unit
	{
		NEWTON
	}
}

export class Force
{
	static getUnitInfo(u: Force.Unit): UnitInfo<Force.Unit> | null;
	static getUnitRatio(u: Force.Unit): number;
	static convert(fromUnit: Force.Unit, toUnit: Force.Unit, fromValue: number): number;
}

module Frequency
{
	enum Unit
	{
		HERTZ,
		KILOHERTZ
	}
}

export class Frequency
{
	static getUnitInfo(u: Frequency.Unit): UnitInfo<Frequency.Unit> | null;
	static getUnitRatio(u: Frequency.Unit): number;
	static convert(fromUnit: Frequency.Unit, toUnit: Frequency.Unit, fromValue: number): number;
}

module MagneticField
{
	enum Unit
	{
		TESLA,
		GAUSS,
		MICROTESLA,
		MILLITESLA
	}
}

export class MagneticField
{
	static getUnitInfo(u: MagneticField.Unit): UnitInfo<MagneticField.Unit> | null;
	static getUnitRatio(u: MagneticField.Unit): number;
	static convert(fromUnit: MagneticField.Unit, toUnit: MagneticField.Unit, fromValue: number): number;
}

module Mass
{
	enum Unit
	{
		KILOGRAM,
		GRAM,
		TONNE,
		POUND,
		OZ
	}
}

export class Mass
{
	static getUnitInfo(u: Mass.Unit): UnitInfo<Mass.Unit> | null;
	static getUnitRatio(u: Mass.Unit): number;
	static convert(fromUnit: Mass.Unit, toUnit: Mass.Unit, fromValue: number): number;
}

module Power
{
	enum Unit
	{
		WATT,
		MILLIWATT,
		KILOWATT
	}
}

export class Power
{
	static getUnitInfo(u: Power.Unit): UnitInfo<Power.Unit> | null;
	static getUnitRatio(u: Power.Unit): number;
	static convert(fromUnit: Power.Unit, toUnit: Power.Unit, fromValue: number): number;
}

module Pressure
{
	enum Unit
	{
		PASCAL,
		BAR,
		ATM,
		TORR,
		PSI,
		KPASCAL,
		HPASCAL
	}
}

export class Pressure
{
	static getUnitInfo(u: Pressure.Unit): UnitInfo<Pressure.Unit> | null;
	static getUnitRatio(u: Pressure.Unit): number;
	static convert(fromUnit: Pressure.Unit, toUnit: Pressure.Unit, fromValue: number): number;
}

module Ratio
{
	enum Unit
	{
		RATIO,
		PERCENT
	}
}

export class Ratio
{
	static getUnitInfo(u: Ratio.Unit): UnitInfo<Ratio.Unit> | null;
	static getUnitRatio(u: Ratio.Unit): number;
	static convert(fromUnit: Ratio.Unit, toUnit: Ratio.Unit, fromValue: number): number;
}

module ReactiveEnergy
{
	enum Unit
	{
		KVARH
	}
}

export class ReactiveEnergy
{
	static getUnitInfo(u: ReactiveEnergy.Unit): UnitInfo<ReactiveEnergy.Unit> | null;
	static getUnitRatio(u: ReactiveEnergy.Unit): number;
	static convert(fromUnit: ReactiveEnergy.Unit, toUnit: ReactiveEnergy.Unit, fromValue: number): number;
}

module ReactivePower
{
	enum Unit
	{
		VAR
	}
}

export class ReactivePower
{
	static getUnitInfo(u: ReactivePower.Unit): UnitInfo<ReactivePower.Unit> | null;
	static getUnitRatio(u: ReactivePower.Unit): number;
	static convert(fromUnit: ReactivePower.Unit, toUnit: ReactivePower.Unit, fromValue: number): number;
}

module Speed
{
	enum Unit
	{
		METER_PER_SECOND,
		KM_PER_HOUR,
		MILE_PER_HOUR,
		KNOT
	}
}

export class Speed
{
	static getUnitInfo(u: Speed.Unit): UnitInfo<Speed.Unit> | null;
	static getUnitRatio(u: Speed.Unit): number;
	static convert(fromUnit: Speed.Unit, toUnit: Speed.Unit, fromValue: number): number;
}

module Temperature
{
	enum Unit
	{
		CELSIUS,
		KELVIN,
		FAHRENHEIT
	}
}

export class Temperature
{
	static getUnitInfo(u: Temperature.Unit): UnitInfo<Temperature.Unit> | null;
	static getUnitRatio(u: Temperature.Unit): number;
	static getUnitScale(u: Temperature.Unit): number;
	static convert(fromUnit: Temperature.Unit, toUnit: Temperature.Unit, fromValue: number): number;
}

export function getList(): any[];
