export class UnitInfo
{
	constructor(unit, symbol, name, ratio, scale)
	{
		this.unit = unit;
		this.symbol = symbol;
		this.name = name;
		this.ratio = ratio;
		this.scale = scale || 0.0;
	}
}

export class Acceleration
{
	static Unit = {
		METER_PER_SECOND_SQUARED: 1,
		STANDARD_GRAVITY: 2
	};

	static getUnitInfo(u)
	{
		switch (u)
		{
		case Acceleration.Unit.METER_PER_SECOND_SQUARED:
			return new UnitInfo(u, "m/s^2", "Meter Per Second Squared", 1);
		case Acceleration.Unit.STANDARD_GRAVITY:
			return new UnitInfo(u, "g", "Gravity", 9.80665);
		}
		return null;
	}

	static getUnitRatio(u)
	{
		switch (u)
		{
		case Acceleration.Unit.METER_PER_SECOND_SQUARED:
			return 1.0;
		case Acceleration.Unit.STANDARD_GRAVITY:
			return 9.80665;
		}
		return 1.0;
	}
	
	static convert(fromUnit, toUnit, fromValue)
	{
		return fromValue * Acceleration.getUnitRatio(fromUnit) / Acceleration.getUnitRatio(toUnit);
	}
}

export class Angle
{
	static Unit = {
		RADIAN: 1,
		GRADIAN: 2,
		TURN: 3,
		DEGREE: 4,
		ARCMINUTE: 5,
		ARCSECOND: 6,
		MILLIARCSECOND: 7,
		MICROARCSECOND: 8
	};

	static getUnitInfo(u)
	{
		switch (u)
		{
		case Angle.Unit.RADIAN:
			return new UnitInfo(u, "rad", "Radian", 1);
		case Angle.Unit.GRADIAN:
			return new UnitInfo(u, "grad", "Gradian", Math.PI / 200.0);
		case Angle.Unit.TURN:
			return new UnitInfo(u, "", "Turns", Math.PI * 2.0);
		case Angle.Unit.DEGREE:
			return new UnitInfo(u, "°", "Degree", Math.PI / 180.0);
		case Angle.Unit.ARCMINUTE:
			return new UnitInfo(u, "′", "Arcminute", Math.PI / 10800.0);
		case Angle.Unit.ARCSECOND:
			return new UnitInfo(u, "″", "Arcsecond", Math.PI / 648000.0);
		case Angle.Unit.MILLIARCSECOND:
			return new UnitInfo(u, "mas", "Milliarcsecond", Math.PI / 648000000.0);
		case Angle.Unit.MICROARCSECOND:
			return new UnitInfo(u, "μas", "Microarcsecond", Math.PI / 648000000000.0);
		}
		return null;
	}

	static getUnitRatio(u)
	{
		switch (u)
		{
		case Angle.Unit.RADIAN:
			return 1;
		case Angle.Unit.GRADIAN:
			return Math.PI / 200.0;
		case Angle.Unit.TURN:
			return Math.PI * 2.0;
		case Angle.Unit.DEGREE:
			return Math.PI / 180.0;
		case Angle.Unit.ARCMINUTE:
			return Math.PI / 10800.0;
		case Angle.Unit.ARCSECOND:
			return Math.PI / 648000.0;
		case Angle.Unit.MILLIARCSECOND:
			return Math.PI / 648000000.0;
		case Angle.Unit.MICROARCSECOND:
			return Math.PI / 648000000000.0;
		}
		return 1;
	}

	static convert(fromUnit, toUnit, fromValue)
	{
		return fromValue * Angle.getUnitRatio(fromUnit) / Angle.getUnitRatio(toUnit);
	}
}

export class ApparentPower
{
	static Unit = {
		VOLT_AMPERE: 1
	};

	static getUnitInfo(u)
	{
		switch (u)
		{
		case ApparentPower.Unit.VOLT_AMPERE:
			return new UnitInfo(u, "VA", "Volt-Ampere", 1);
		}
		return null;
	}

	static getUnitRatio(u)
	{
		switch (u)
		{
		case ApparentPower.Unit.VOLT_AMPERE:
			return 1;
		}
		return 1;
	}

	static convert(fromUnit, toUnit, fromValue)
	{
		return fromValue * ApparentPower.getUnitRatio(fromUnit) / ApparentPower.getUnitRatio(toUnit);
	}
}

export class Count
{
	static Unit = {
		UNIT: 1,
		K_UNIT: 2,
		KI_UNIT: 3,
		M_UNIT: 4,
		MI_UNIT: 5,
		G_UNIT: 6,
		GI_UNIT: 7,
		T_UNIT: 8,
		TI_UNIT: 9
	};

	static getUnitInfo(u)
	{
		switch (u)
		{
		case Count.Unit.UNIT:
			return new UnitInfo(u, "", "Unit", 1.0);
		case Count.Unit.K_UNIT:
			return new UnitInfo(u, "K", "Kilo", 1000.0);
		case Count.Unit.KI_UNIT:
			return new UnitInfo(u, "Ki", "Binary Kilo", 1024.0);
		case Count.Unit.M_UNIT:
			return new UnitInfo(u, "M", "Mega", 1000000.0);
		case Count.Unit.MI_UNIT:
			return new UnitInfo(u, "Mi", "Binary Mega", 1048576.0);
		case Count.Unit.G_UNIT:
			return new UnitInfo(u, "G", "Giga", 1000000000.0);
		case Count.Unit.GI_UNIT:
			return new UnitInfo(u, "Gi", "Binary Giga", 1073741824.0);
		case Count.Unit.T_UNIT:
			return new UnitInfo(u, "T", "Tera", 1000000000000.0);
		case Count.Unit.TI_UNIT:
			return new UnitInfo(u, "Ti", "Binary Tera", 1099511627776.0);
		}
		return null;
	}

	static getUnitRatio(u)
	{
		switch (u)
		{
		case Count.Unit.UNIT:
			return 1.0;
		case Count.Unit.K_UNIT:
			return 1000.0;
		case Count.Unit.KI_UNIT:
			return 1024.0;
		case Count.Unit.M_UNIT:
			return 1000000.0;
		case Count.Unit.MI_UNIT:
			return 1048576.0;
		case Count.Unit.G_UNIT:
			return 1000000000.0;
		case Count.Unit.GI_UNIT:
			return 1073741824.0;
		case Count.Unit.T_UNIT:
			return 1000000000000.0;
		case Count.Unit.TI_UNIT:
			return 1099511627776.0;
		}
		return 1;
	}

	static convert(fromUnit, toUnit, fromValue)
	{
		return fromValue * ApparentPower.getUnitRatio(fromUnit) / ApparentPower.getUnitRatio(toUnit);
	}
}

export class Distance
{
	static Unit = {
		METER: 0,
		CENTIMETER: 1,
		MILLIMETER: 2,
		MICROMETER: 3,
		NANOMETER: 4,
		PICOMETER: 5,
		KILOMETER: 6,
		INCH: 7,
		FOOT: 8,
		YARD: 9,
		MILE: 10,
		NAUTICAL_MILE: 11,
		AU: 12,
		LIGHTSECOND: 13,
		LIGHTMINUTE: 14,
		LIGHTHOUR: 15,
		LIGHTDAY: 16,
		LIGHTWEEK: 17,
		LIGHTYEAR: 18,
		EMU: 19,
		POINT: 20,
		PIXEL: 21,
		TWIP: 22
	}
	
	static getUnitInfo(u)
	{
		switch (u)
		{
		case Distance.Unit.METER:
			return new UnitInfo(u, "m", "Meter", 1.0);
		case Distance.Unit.CENTIMETER:
			return new UnitInfo(u, "cm", "Centimeter", 0.01);
		case Distance.Unit.MILLIMETER:
			return new UnitInfo(u, "mm", "Millimeter", 0.001);
		case Distance.Unit.MICROMETER:
			return new UnitInfo(u, "μm", "Micrometer", 0.000001);
		case Distance.Unit.NANOMETER:
			return new UnitInfo(u, "nm", "Nanometer", 0.000000001);
		case Distance.Unit.PICOMETER:
			return new UnitInfo(u, "pm", "Picometer", 0.000000000001);
		case Distance.Unit.KILOMETER:
			return new UnitInfo(u, "km", "Kilometer", 1000.0);
		case Distance.Unit.INCH:
			return new UnitInfo(u, "\"", "Inch", 0.0254);
		case Distance.Unit.FOOT:
			return new UnitInfo(u, "ft", "Foot", 0.0254 * 12.0);
		case Distance.Unit.YARD:
			return new UnitInfo(u, "yd", "Yard", 0.0254 * 36.0);
		case Distance.Unit.MILE:
			return new UnitInfo(u, "mile", "Mile", 0.0254 * 12.0 * 5280);
		case Distance.Unit.NAUTICAL_MILE:
			return new UnitInfo(u, "NM", "Nautical Mile", 1852.0);
		case Distance.Unit.AU:
			return new UnitInfo(u, "AU", "Astronomical unit", 149597870700.0);
		case Distance.Unit.LIGHTSECOND:
			return new UnitInfo(u, "ls", "Light-second", 299792458.0);
		case Distance.Unit.LIGHTMINUTE:
			return new UnitInfo(u, "lm", "Light-minute", 17987547480.0);
		case Distance.Unit.LIGHTHOUR:
			return new UnitInfo(u, "lh", "Light-hour", 299792458.0 * 3600.0);
		case Distance.Unit.LIGHTDAY:
			return new UnitInfo(u, "ld", "Light-day", 299792458.0 * 86400.0);
		case Distance.Unit.LIGHTWEEK:
			return new UnitInfo(u, "lw", "Light-week", 299792458.0 * 604800.0);
		case Distance.Unit.LIGHTYEAR:
			return new UnitInfo(u, "ly", "Light-year", 299792458.0 * 31557600.0);
		case Distance.Unit.EMU:
			return new UnitInfo(u, "emu", "English Metric Unit", 1 / 36000000.0);
		case Distance.Unit.POINT:
			return new UnitInfo(u, "pt", "Point", 0.0254 / 72.0);
		case Distance.Unit.PIXEL:
			return new UnitInfo(u, "px", "Pixel", 0.0254 / 96.0);
		case Distance.Unit.TWIP:
			return new UnitInfo(u, "twip", "Twentieth of an inch point", 0.0254 / 1440.0);
		}
		return null;
	}

	static getUnitRatio(u)
	{
		switch (u)
		{
		case Distance.Unit.METER:
			return 1.0;
		case Distance.Unit.CENTIMETER:
			return 0.01;
		case Distance.Unit.MILLIMETER:
			return 0.001;
		case Distance.Unit.MICROMETER:
			return 0.000001;
		case Distance.Unit.NANOMETER:
			return 0.000000001;
		case Distance.Unit.PICOMETER:
			return 0.000000000001;
		case Distance.Unit.KILOMETER:
			return 1000.0;
		case Distance.Unit.INCH:
			return 0.0254;
		case Distance.Unit.FOOT:
			return 0.0254 * 12.0;
		case Distance.Unit.YARD:
			return 0.0254 * 36.0;
		case Distance.Unit.MILE:
			return 0.0254 * 12.0 * 5280;
		case Distance.Unit.NAUTICAL_MILE:
			return 1852.0;
		case Distance.Unit.AU:
			return 149597870700.0;
		case Distance.Unit.LIGHTSECOND:
			return 299792458.0;
		case Distance.Unit.LIGHTMINUTE:
			return 17987547480.0;
		case Distance.Unit.LIGHTHOUR:
			return 299792458.0 * 3600.0;
		case Distance.Unit.LIGHTDAY:
			return 299792458.0 * 86400.0;
		case Distance.Unit.LIGHTWEEK:
			return 299792458.0 * 604800.0;
		case Distance.Unit.LIGHTYEAR:
			return 299792458.0 * 31557600.0;
		case Distance.Unit.EMU:
			return 1 / 36000000.0;
		case Distance.Unit.POINT:
			return 0.0254 / 72.0;
		case Distance.Unit.PIXEL:
			return 0.0254 / 96.0;
		case Distance.Unit.TWIP:
			return 0.0254 / 1440.0;
		}
		return 1;
	}

	static convert(fromUnit, toUnit, fromValue)
	{
		return fromValue * Distance.getUnitRatio(fromUnit) / Distance.getUnitRatio(toUnit);
	}
}

export class ElectricCurrent
{
	static Unit = {
		AMPERE: 1,
		MILLIAMPERE: 2,
		MICROAMPERE: 3
	};

	static getUnitInfo(u)
	{
		switch (u)
		{
		case ElectricCurrent.Unit.AMPERE:
			return new UnitInfo(u, "A", "Ampere", 1);
		case ElectricCurrent.Unit.MILLIAMPERE:
			return new UnitInfo(u, "mA", "Milliampere", 0.001);
		case ElectricCurrent.Unit.MICROAMPERE:
			return new UnitInfo(u, "μA", "Microampere", 0.000001);
		}
		return null;
	}

	static getUnitRatio(u)
	{
		switch (u)
		{
		case ElectricCurrent.Unit.AMPERE:
			return 1;
		case ElectricCurrent.Unit.MILLIAMPERE:
			return 0.001;
		case ElectricCurrent.Unit.MICROAMPERE:
			return 0.000001;
		}
		return 1;
	}

	static convert(fromUnit, toUnit, fromValue)
	{
		return fromValue * ElectricCurrent.getUnitRatio(fromUnit) / ElectricCurrent.getUnitRatio(toUnit);
	}
}

export class ElectricPotential
{
	static Unit = {
		VOLT: 1,
		MILLIVOLT: 2
	};

	static getUnitInfo(u)
	{
		switch (u)
		{
		case ElectricPotential.Unit.VOLT:
			return new UnitInfo(u, "V", "Volt", 1);
		case ElectricPotential.Unit.MILLIVOLT:
			return new UnitInfo(u, "mV", "Millivolt", 0.001);
		}
		return null;
	}

	static getUnitRatio(u)
	{
		switch (u)
		{
		case ElectricPotential.Unit.VOLT:
			return 1;
		case ElectricPotential.Unit.MILLIVOLT:
			return 0.001;
		}
		return 1;
	}

	static convert(fromUnit, toUnit, fromValue)
	{
		return fromValue * ElectricPotential.getUnitRatio(fromUnit) / ElectricPotential.getUnitRatio(toUnit);
	}
}

export class Energy
{
	static Unit = {
		JOULE: 1,
		WATTHOUR: 2,
		KILOWATTHOUR: 3,
		CALORIE: 4,
		KILOCALORIE: 5
	};

	static getUnitInfo(u)
	{
		switch (u)
		{
		case Energy.Unit.JOULE:
			return new UnitInfo(u, "J", "Joule", 1);
		case Energy.Unit.WATTHOUR:
			return new UnitInfo(u, "Wh", "Watt-hour", 3600.0);
		case Energy.Unit.KILOWATTHOUR:
			return new UnitInfo(u, "kWh", "Kilowatt-hour", 3600000.0);
		case Energy.Unit.CALORIE:
			return new UnitInfo(u, "cal", "Gram calorie", 4.184);
		case Energy.Unit.KILOCALORIE:
			return new UnitInfo(u, "kcal", "Kilocalorie", 4184);
		}
		return null;
	}

	static getUnitRatio(u)
	{
		switch (u)
		{
		case Energy.Unit.JOULE:
			return 1;
		case Energy.Unit.WATTHOUR:
			return 3600.0;
		case Energy.Unit.KILOWATTHOUR:
			return 3600000.0;
		case Energy.Unit.CALORIE:
			return 4.184;
		case Energy.Unit.KILOCALORIE:
			return 4184;
		}
		return 1;
	}

	static convert(fromUnit, toUnit, fromValue)
	{
		return fromValue * Energy.getUnitRatio(fromUnit) / Energy.getUnitRatio(toUnit);
	}
}

export class Force
{
	static Unit = {
		NEWTON: 1
	};

	static getUnitInfo(u)
	{
		switch (u)
		{
		case Force.Unit.NEWTON:
			return new UnitInfo(u, "N", "Newton", 1);
		}
		return null;
	}

	static getUnitRatio(u)
	{
		switch (u)
		{
		case Force.Unit.NEWTON:
			return 1;
		}
		return 1;
	}

	static convert(fromUnit, toUnit, fromValue)
	{
		return fromValue * Force.getUnitRatio(fromUnit) / Force.getUnitRatio(toUnit);
	}
}

export class Frequency
{
	static Unit = {
		HERTZ: 1,
		KILOHERTZ: 2
	};

	static getUnitInfo(u)
	{
		switch (u)
		{
		case Frequency.Unit.HERTZ:
			return new UnitInfo(u, "Hz", "Hertz", 1);
		case Frequency.Unit.KILOHERTZ:
			return new UnitInfo(u, "kHz", "Kilohertz", 1000);
		}
		return null;
	}

	static getUnitRatio(u)
	{
		switch (u)
		{
		case Frequency.Unit.HERTZ:
			return 1;
		case Frequency.Unit.KILOHERTZ:
			return 1000;
		}
		return 1;
	}

	static convert(fromUnit, toUnit, fromValue)
	{
		return fromValue * Frequency.getUnitRatio(fromUnit) / Frequency.getUnitRatio(toUnit);
	}
}

export class MagneticField
{
	static Unit = {
		TESLA: 1,
		GAUSS: 2,
		MICROTESLA: 3,
		MILLITESLA: 4
	};

	static getUnitInfo(u)
	{
		switch (u)
		{
		case MagneticField.Unit.TESLA:
			return new UnitInfo(u, "T", "Tesla", 1);
		case MagneticField.Unit.GAUSS:
			return new UnitInfo(u, "G", "Gauss", 0.0001);
		case MagneticField.Unit.MICROTESLA:
			return new UnitInfo(u, "uT", "Micro Tesla", 0.000001);
		case MagneticField.Unit.MILLITESLA:
			return new UnitInfo(u, "mT", "Milli Tesla", 0.001);
		}
		return null;
	}

	static getUnitRatio(u)
	{
		switch (u)
		{
		case MagneticField.Unit.TESLA:
			return 1;
		case MagneticField.Unit.GAUSS:
			return 0.0001;
		case MagneticField.Unit.MICROTESLA:
			return 0.000001;
		case MagneticField.Unit.MILLITESLA:
			return 0.001;
		}
		return 1;
	}

	static convert(fromUnit, toUnit, fromValue)
	{
		return fromValue * MagneticField.getUnitRatio(fromUnit) / MagneticField.getUnitRatio(toUnit);
	}
}

export class Mass
{
	static Unit = {
		KILOGRAM: 1,
		GRAM: 2,
		TONNE: 3,
		POUND: 4,
		OZ: 5
	};

	static getUnitInfo(u)
	{
		switch (u)
		{
		case Mass.Unit.KILOGRAM:
			return new UnitInfo(u, "kg", "Kilogram", 1.0);
		case Mass.Unit.GRAM:
			return new UnitInfo(u, "g", "Gram", 0.001);
		case Mass.Unit.TONNE:
			return new UnitInfo(u, "t", "Tonne", 1000.0);
		case Mass.Unit.POUND:
			return new UnitInfo(u, "lb", "Pounds", 0.45359237);
		case Mass.Unit.OZ:
			return new UnitInfo(u, "oz", "Ounce", 0.45359237 / 16);
		}
		return null;
	}

	static getUnitRatio(u)
	{
		switch (u)
		{
		case Mass.Unit.KILOGRAM:
			return 1.0;
		case Mass.Unit.GRAM:
			return 0.001;
		case Mass.Unit.TONNE:
			return 1000.0;
		case Mass.Unit.POUND:
			return 0.45359237;
		case Mass.Unit.OZ:
			return 0.45359237 / 16;
		}
		return 1;
	}

	static convert(fromUnit, toUnit, fromValue)
	{
		return fromValue * Mass.getUnitRatio(fromUnit) / Mass.getUnitRatio(toUnit);
	}
}

export class Power
{
	static Unit = {
		WATT: 1,
		MILLIWATT: 2,
		KILOWATT: 3
	};

	static getUnitInfo(u)
	{
		switch (u)
		{
		case Power.Unit.WATT:
			return new UnitInfo(u, "W", "Watt", 1.0);
		case Power.Unit.MILLIWATT:
			return new UnitInfo(u, "mW", "Milliwatt", 0.001);
		case Power.Unit.KILOWATT:
			return new UnitInfo(u, "kW", "Kilowatt", 1000.0);
		}
		return null;
	}

	static getUnitRatio(u)
	{
		switch (u)
		{
		case Power.Unit.WATT:
			return 1.0;
		case Power.Unit.MILLIWATT:
			return 0.001;
		case Power.Unit.KILOWATT:
			return 1000.0;
		}
		return 1;
	}

	static convert(fromUnit, toUnit, fromValue)
	{
		return fromValue * Power.getUnitRatio(fromUnit) / Power.getUnitRatio(toUnit);
	}
}

export class Pressure
{
	static Unit = {
		PASCAL: 1,
		BAR: 2,
		ATM: 3,
		TORR: 4,
		PSI: 5,
		KPASCAL: 6,
		HPASCAL: 7
	};

	static getUnitInfo(u)
	{
		switch (u)
		{
		case Pressure.Unit.PASCAL:
			return new UnitInfo(u, "Pa", "Pascal", 1.0);
		case Pressure.Unit.BAR:
			return new UnitInfo(u, "bar", "Bar", 100000.0);
		case Pressure.Unit.ATM:
			return new UnitInfo(u, "atm", "Standard atmosphere", 101325.0);
		case Pressure.Unit.TORR:
			return new UnitInfo(u, "Torr", "Torr", 101325.0 / 760.0);
		case Pressure.Unit.PSI:
			return new UnitInfo(u, "psi", "Pounds per square inch", 4.4482216152605 / 0.0254 / 0.0254);
		case Pressure.Unit.KPASCAL:
			return new UnitInfo(u, "kPa", "Kilo Pascal", 1000.0);
		case Pressure.Unit.HPASCAL:
			return new UnitInfo(u, "hPa", "Hecto Pascal", 100.0);
		}
		return null;
	}

	static getUnitRatio(u)
	{
		switch (u)
		{
		case Pressure.Unit.PASCAL:
			return 1.0;
		case Pressure.Unit.BAR:
			return 100000.0;
		case Pressure.Unit.ATM:
			return 101325.0;
		case Pressure.Unit.TORR:
			return 101325.0 / 760.0;
		case Pressure.Unit.PSI:
			return 4.4482216152605 / 0.0254 / 0.0254;
		case Pressure.Unit.KPASCAL:
			return 1000.0;
		case Pressure.Unit.HPASCAL:
			return 100.0;
		}
		return 1;
	}

	static convert(fromUnit, toUnit, fromValue)
	{
		return fromValue * Pressure.getUnitRatio(fromUnit) / Pressure.getUnitRatio(toUnit);
	}
}

export class Ratio
{
	static Unit = {
		RATIO: 1,
		PERCENT: 2
	};

	static getUnitInfo(u)
	{
		switch (u)
		{
		case Ratio.Unit.RATIO:
			return new UnitInfo(u, "", "Ratio", 1.0);
		case Ratio.Unit.PERCENT:
			return new UnitInfo(u, "%", "Percent", 0.01);
		}
		return null;
	}

	static getUnitRatio(u)
	{
		switch (u)
		{
		case Ratio.Unit.RATIO:
			return 1.0;
		case Ratio.Unit.PERCENT:
			return 0.01;
		}
		return 1;
	}

	static convert(fromUnit, toUnit, fromValue)
	{
		return fromValue * Ratio.getUnitRatio(fromUnit) / Ratio.getUnitRatio(toUnit);
	}
}

export class ReactiveEnergy
{
	static Unit = {
		KVARH: 1
	};

	static getUnitInfo(u)
	{
		switch (u)
		{
		case ReactiveEnergy.Unit.KVARH:
			return new UnitInfo(u, "kvarh", "Kilovolt-amperer hour", 1.0);
		}
		return null;
	}

	static getUnitRatio(u)
	{
		switch (u)
		{
		case ReactiveEnergy.Unit.KVARH:
			return 1.0;
		}
		return 1;
	}

	static convert(fromUnit, toUnit, fromValue)
	{
		return fromValue * ReactiveEnergy.getUnitRatio(fromUnit) / ReactiveEnergy.getUnitRatio(toUnit);
	}
}

export class ReactivePower
{
	static Unit = {
		VAR: 1
	};

	static getUnitInfo(u)
	{
		switch (u)
		{
		case ReactivePower.Unit.VAR:
			return new UnitInfo(u, "", "Ratio", 1.0);
		}
		return null;
	}

	static getUnitRatio(u)
	{
		switch (u)
		{
		case ReactivePower.Unit.VAR:
			return 1.0;
		}
		return 1;
	}

	static convert(fromUnit, toUnit, fromValue)
	{
		return fromValue * ReactivePower.getUnitRatio(fromUnit) / ReactivePower.getUnitRatio(toUnit);
	}
}

export class Speed
{
	static Unit = {
		METER_PER_SECOND: 1,
		KM_PER_HOUR: 2,
		MILE_PER_HOUR: 3,
		KNOT: 4
	};

	static getUnitInfo(u)
	{
		switch (u)
		{
		case Speed.Unit.METER_PER_SECOND:
			return new UnitInfo(u, "m/s", "Meter per second", 1.0);
		case Speed.Unit.KM_PER_HOUR:
			return new UnitInfo(u, "km/h", "Kilometer per hour", 1000 / 3600.0);
		case Speed.Unit.MILE_PER_HOUR:
			return new UnitInfo(u, "mph", "Mile per hour", Distance.getUnitRatio(Distance.Unit.MILE) / 3600.0);
		case Speed.Unit.KNOT:
			return new UnitInfo(u, "knot", "Knot", Distance.getUnitRatio(Distance.Unit.NAUTICAL_MILE) / 3600.0);
		}
		return null;
	}

	static getUnitRatio(u)
	{
		switch (u)
		{
		case Speed.Unit.METER_PER_SECOND:
			return 1.0;
		case Speed.Unit.KM_PER_HOUR:
			return 1000 / 3600.0;
		case Speed.Unit.MILE_PER_HOUR:
			return Distance.getUnitRatio(Distance.Unit.MILE) / 3600.0;
		case Speed.Unit.KNOT:
			return Distance.getUnitRatio(Distance.Unit.NAUTICAL_MILE) / 3600.0;
		}
		return 1;
	}

	static convert(fromUnit, toUnit, fromValue)
	{
		return fromValue * Speed.getUnitRatio(fromUnit) / Speed.getUnitRatio(toUnit);
	}
}

export class Temperature
{
	static Unit = {
		CELSIUS: 1,
		KELVIN: 2,
		FAHRENHEIT: 3
	};

	static getUnitInfo(u)
	{
		switch (u)
		{
		case Temperature.Unit.CELSIUS:
			return new UnitInfo(u, "℃", "Degree Celsius", 1.0, -273.15);
		case Temperature.Unit.KELVIN:
			return new UnitInfo(u, "K", "Kelvin", 1.0, 0.0);
		case Temperature.Unit.FAHRENHEIT:
			return new UnitInfo(u, "℉", "Degree Fahrenheit", 5/9, -459.67);
		}
		return null;
	}

	static getUnitRatio(u)
	{
		switch (u)
		{
		case Temperature.Unit.CELSIUS:
			return 1.0;
		case Temperature.Unit.KELVIN:
			return 1.0;
		case Temperature.Unit.FAHRENHEIT:
			return 5/9;
		}
		return 1;
	}

	static getUnitScale(u)
	{
		switch (u)
		{
		case Temperature.Unit.CELSIUS:
			return -273.15;
		case Temperature.Unit.KELVIN:
			return 0.0;
		case Temperature.Unit.FAHRENHEIT:
			return -459.67;
		}
		return null;
	}

	static convert(fromUnit, toUnit, fromValue)
	{
		return (fromValue * Temperature.getUnitRatio(fromUnit) - Temperature.getUnitScale(fromUnit)) / Temperature.getUnitRatio(toUnit) + Temperature.getUnitScale(toUnit);
	}
}

export function getList()
{
	return [
		Acceleration,
		Angle,
		ApparentPower,
		Count,
		Distance,
		ElectricCurrent,
		ElectricPotential,
		Energy,
		Force,
		Frequency,
		MagneticField,
		Mass,
		Power,
		Pressure,
		Ratio,
		ReactiveEnergy,
		ReactivePower,
		Speed,
		Temperature
	];
}
