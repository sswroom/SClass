export enum AngleUnit
{
	RADIAN = 1,
	GRADIAN,
	TURN,
	DEGREE,
	ARCMINUTE,
	ARCSECOND,
	MILLIARCSECOND,
	MICROARCSECOND
}

export class Angle
{
	static getUnitRatio(u: AngleUnit) : number;
}

export enum DistanceUnit
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

export class Distance
{
	static getUnitRatio(u: DistanceUnit): number;
	static convert(fromUnit: DistanceUnit, toUnit: DistanceUnit, fromValue: number): number;
}
