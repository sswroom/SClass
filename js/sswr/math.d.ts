import { AngleUnit, DistanceUnit } from "./unit";

export function roundToFloat(n: number, decimalPoints: number): number;
export function roundToStr(n: number, decimalPoints: number): string;
export class GeoJSON
{
	static parseGeometry(srid: null, geom: object): Vector2D | null;
};

declare class Coord2D
{
	x: number;
	y: number;
	constructor(x: number, y: number);
	get lat(): number;
	get lon(): number;
}

declare enum EarthEllipsoidType
{
	OTHER,
	PLESSIS,
	EVEREST1830,
	EVEREST1830M,
	EVEREST1830N,
	AIRY1830,
	AIRY1830M,
	BESSEL1841,
	CLARKE1866,
	CLARKE1878,
	CLARKE1880,
	HELMERT1906,
	HAYFORD1910,
	INTL1924,
	KRASSOVSKY1940,
	WGS66,
	AUSTRALIAN1966,
	NEWINTL1967,
	GPS67,
	SAM1969,
	WGS72,
	GRS80,
	WGS84,
	WGS84_OGC,
	IERS1989,
	IERS2003
};

export class EarthEllipsoid
{
	semiMajorAxis : number;
	inverseFlattening: number;
	eet: EarthEllipsoidType;
	eccentricity: number;
	constructor(semiMajorAxis: number, inverseFlattening: number, eet: EarthEllipsoidType);
	calSurfaceDistance(dLat1: number, dLon1: number, dLat2: number, dLon2: number, distUnit: DistanceUnit): number;
	getSemiMinorAxis() : number;
	initEarthInfo(eet: EarthEllipsoidType): void;
}

export class Spheroid
{
	sric: number;
	ellipsoid: EarthEllipsoid;
	name: string;
	constructor(srid: number, ellipsoid: EarthEllipsoid, name: string);
}

export class DatumData
{
	srid : number;
	spheroid: Spheroid;
	name: string;
	x0: number;
	y0: number;
	z0: number;
	cX: number;
	cY: number;
	cZ: number;
	xAngle: number;
	yAngle: number;
	zAngle: number;
	scale: number;
	aunit: AngleUnit;

	constructor(srid: number, spheroid: Spheroid, name: string, x0: number, y0: number, z0: number, cX: number, cY: number, cZ: number, xAngle: number, yAngle: number, zAngle: number, scale: number, aunit: AngleUnit);
}

export enum CoordinateSystemType
{
	Geographic,
	MercatorProjected,
	Mercator1SPProjected,
	PointMapping,
	GausskrugerProjected
};

declare abstract class CoordinateSystem
{
	srid: number;
	csysName: string;

	constructor(srid: number, csysName: string);
	abstract calcSurfaceDistance(x1: number, y1: number, x2: number, y2: number, distUnit: DistanceUnit) : number;
}

export class GeographicCoordinateSystem extends CoordinateSystem
{
	datum: DatumData;
	
	constructor(srid: number, csysName: string, datumData: DatumData);
	calcSurfaceDistance(x1: number, y1: number, x2: number, y2: number, unit: DistanceUnit): number;
}

export class ProjectedCoordinateSystem extends CoordinateSystem
{
	falseEasting: number;
	falseNorthing: number;
	rcentralMeridian: number;
	rlatitudeOfOrigin: number;
	scaleFactor: number;
	gcs: GeographicCoordinateSystem;
	constructor(srid: number, csysName: string, falseEasting: number, falseNorthing: number, dcentralMeridian: number, dlatitudeOfOrigin: number, scaleFactor: number, gcs: GeographicCoordinateSystem);
	calcSurfaceDistance(x1: number, y1: number, x2: number, y2: number, distUnit: DistanceUnit): number;
}

export class MercatorProjectedCoordinateSystem extends ProjectedCoordinateSystem
{
	constructor(srid: number, csysName: string, falseEasting: number, falseNorthing: number, dcentralMeridian: number, dlatitudeOfOrigin: number, scaleFactor: number, gcs: GeographicCoordinateSystem);
}

export class Mercator1SPProjectedCoordinateSystem extends ProjectedCoordinateSystem
{
	constructor(srid: number, csysName: string, falseEasting: number, falseNorthing: number, dcentralMeridian: number, dlatitudeOfOrigin: number, scaleFactor: number, gcs: GeographicCoordinateSystem);
}

export class CoordinateSystemManager
{
	static srCreateGeogCSys(srid: number, datumSrid: number, name: string): GeographicCoordinateSystem | null;
	static srCreateProjCSys(srid: number, geogcsSrid: number, csysType: CoordinateSystemType, projName: string, falseEasting: number, falseNorthing: number, centralMeridian: number, latitudeOfOrigin: number, scaleFactor: number): ProjectedCoordinateSystem | null;
	static srCreateCsys(srid: number) : CoordinateSystem | null;
	static srGetDatumData(srid: number) : DatumData | null;
	static srGetSpheroid(srid: number) : Spheroid | null;
};
