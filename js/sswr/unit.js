export const AngleUnit = {
	RADIAN: 1,
	GRADIAN: 2,
	TURN: 3,
	DEGREE: 4,
	ARCMINUTE: 5,
	ARCSECOND: 6,
	MILLIARCSECOND: 7,
	MICROARCSECOND: 8
};

export class Angle
{
	static getUnitRatio(u)
	{
		switch (u)
		{
		case AngleUnit.RADIAN:
			return 1;
		case AngleUnit.GRADIAN:
			return Math.PI / 200.0;
		case AngleUnit.TURN:
			return Math.PI * 2.0;
		case AngleUnit.DEGREE:
			return Math.PI / 180.0;
		case AngleUnit.ARCMINUTE:
			return Math.PI / 10800.0;
		case AngleUnit.ARCSECOND:
			return Math.PI / 648000.0;
		case AngleUnit.MILLIARCSECOND:
			return Math.PI / 648000000.0;
		case AngleUnit.MICROARCSECOND:
			return Math.PI / 648000000000.0;
		}
		return 1;
	}
}

export const DistanceUnit = {
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

export class Distance
{
	static getUnitRatio(u)
	{
		switch (u)
		{
		case DistanceUnit.METER:
			return 1.0;
		case DistanceUnit.CENTIMETER:
			return 0.01;
		case DistanceUnit.MILLIMETER:
			return 0.001;
		case DistanceUnit.MICROMETER:
			return 0.000001;
		case DistanceUnit.NANOMETER:
			return 0.000000001;
		case DistanceUnit.PICOMETER:
			return 0.000000000001;
		case DistanceUnit.KILOMETER:
			return 1000.0;
		case DistanceUnit.INCH:
			return 0.0254;
		case DistanceUnit.FOOT:
			return 0.0254 * 12.0;
		case DistanceUnit.YARD:
			return 0.0254 * 36.0;
		case DistanceUnit.MILE:
			return 0.0254 * 12.0 * 5280;
		case DistanceUnit.NAUTICAL_MILE:
			return 1852.0;
		case DistanceUnit.AU:
			return 149597870700.0;
		case DistanceUnit.LIGHTSECOND:
			return 299792458.0;
		case DistanceUnit.LIGHTMINUTE:
			return 17987547480.0;
		case DistanceUnit.LIGHTHOUR:
			return 299792458.0 * 3600.0;
		case DistanceUnit.LIGHTDAY:
			return 299792458.0 * 86400.0;
		case DistanceUnit.LIGHTWEEK:
			return 299792458.0 * 604800.0;
		case DistanceUnit.LIGHTYEAR:
			return 299792458.0 * 31557600.0;
		case DistanceUnit.EMU:
			return 1 / 36000000.0;
		case DistanceUnit.POINT:
			return 0.0254 / 72.0;
		case DistanceUnit.PIXEL:
			return 0.0254 / 96.0;
		case DistanceUnit.TWIP:
			return 0.0254 / 1440.0;
		}
		return 1;
	}

	static convert(fromUnit, toUnit, fromValue)
	{
		return fromValue * Distance.getUnitRatio(fromUnit) / Distance.getUnitRatio(toUnit);
	}
}
