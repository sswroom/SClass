import * as geometry from "./geometry";
import * as unit from "./unit";

export function roundToFloat(n: number, decimalPoints: number): number;
export function roundToStr(n: number, decimalPoints: number): string;
export class GeoJSON
{
	static parseGeometry(srid: null, geom: object): geometry.Vector2D | null;
};

export class Coord2D
{
	x: number;
	y: number;
	constructor(x: number, y: number);
	get lat(): number;
	get lon(): number;

	mul(val: number): Coord2D;
};

export class RectArea
{
	min: Coord2D;
	max: Coord2D;
	constructor(x1: number, y1: number, x2: number, y2: number);
	get minX(): number;
	get minY(): number;
	get maxX(): number;
	get maxY(): number;

	containPt(x: number, y: number): boolean;
	getCenter(): Coord2D;
	getWidth(): number;
	getHeight(): number;
	getArea(): number;
	unionInPlace(rect: RectArea): RectArea;
	unionPointInPlace(x: number, y: number): RectArea;
};

export class Vector3 extends Coord2D
{
	z: number;
	constructor(x: number, y: number, z: number);
	get height(): number;
	mulXY(val: number): Vector3;
	mul(val: number): Vector3;

	static fromCoord2D(coord: Coord2D, z: number): Vector3;
}

export enum EarthEllipsoidType
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
	calSurfaceDistance(dLat1: number, dLon1: number, dLat2: number, dLon2: number, distUnit: unit.DistanceUnit): number;
	getSemiMajorAxis(): number;
	getSemiMinorAxis(): number;
	getInverseFlattening(): number;
	getEccentricity(): number;
	equals(ellipsoid: EarthEllipsoid): boolean;
	initEarthInfo(eet: EarthEllipsoidType): void;

	toCartesianCoordRad(lonLatH: Vector3): Vector3;
	fromCartesianCoordRad(coord: Vector3): Vector3;
	toCartesianCoordDeg(lonLatH: Vector3): Vector3;
	fromCartesianCoordDeg(coord: Vector3): Vector3;
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
	aunit: unit.AngleUnit;

	constructor(srid: number, spheroid: Spheroid, name: string, x0: number, y0: number, z0: number, cX: number, cY: number, cZ: number, xAngle: number, yAngle: number, zAngle: number, scale: number, aunit: unit.AngleUnit);
}

export enum CoordinateSystemType
{
	Geographic,
	MercatorProjected,
	Mercator1SPProjected,
	PointMapping,
	GausskrugerProjected
};

export abstract class CoordinateSystem
{
	srid: number;
	csysName: string;

	constructor(srid: number, csysName: string);
	abstract calcSurfaceDistance(x1: number, y1: number, x2: number, y2: number, distUnit: unit.DistanceUnit) : number;
	abstract getCoordSysType(): CoordinateSystemType;
	abstract isProjected(): boolean;

	equals(csys: CoordinateSystem): boolean;

	static convert(srcCoord: CoordinateSystem, destCoord: CoordinateSystem, coord: Coord2D) : Coord2D;
	static convert3D(srcCoord: CoordinateSystem, destCoord: CoordinateSystem, srcPos: Vector3) : Vector3;
	static convertArray(srcCoord: CoordinateSystem, destCoord: CoordinateSystem, srcArr: Coord2D[]): Coord2D[];
	static convertToCartesianCoord(srcCoord: CoordinateSystem, srcPos: Vector3): Vector3;
}

export class GeographicCoordinateSystem extends CoordinateSystem
{
	datum: DatumData;
	
	constructor(srid: number, csysName: string, datumData: DatumData);
	calcSurfaceDistance(x1: number, y1: number, x2: number, y2: number, unit: unit.DistanceUnit): number;
	getCoordSysType(): CoordinateSystemType;
	isProjected(): boolean;

	getEllipsoid() : EarthEllipsoid;

	toCartesianCoordRad(lonLatH: Vector3): Vector3;
	fromCartesianCoordRad(coord: Vector3): Vector3;
	toCartesianCoordDeg(lonLatH: Vector3): Vector3;
	fromCartesianCoordDeg(coord: Vector3): Vector3;
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
	calcSurfaceDistance(x1: number, y1: number, x2: number, y2: number, distUnit: unit.DistanceUnit): number;
	isProjected(): boolean;

	getGeographicCoordinateSystem(): GeographicCoordinateSystem;
	abstract toGeographicCoordinateRad(projPos: Coord2D): Coord2D;
	abstract fromGeographicCoordinateRad(geoPos: Coord2D): Coord2D;
	toGeographicCoordinateDeg(projPos: Coord2D): Coord2D;
	fromGeographicCoordinateDeg(geoPos: Coord2D): Coord2D;
	sameProjection(csys: ProjectedCoordinateSystem): boolean;
}

export class MercatorProjectedCoordinateSystem extends ProjectedCoordinateSystem
{
	constructor(srid: number, csysName: string, falseEasting: number, falseNorthing: number, dcentralMeridian: number, dlatitudeOfOrigin: number, scaleFactor: number, gcs: GeographicCoordinateSystem);
	getCoordSysType(): CoordinateSystemType;

	toGeographicCoordinateRad(projPos: Coord2D): Coord2D;
	fromGeographicCoordinateRad(geoPos: Coord2D): Coord2D;
	calcM(rLat: number): number;
}

export class Mercator1SPProjectedCoordinateSystem extends ProjectedCoordinateSystem
{
	constructor(srid: number, csysName: string, falseEasting: number, falseNorthing: number, dcentralMeridian: number, dlatitudeOfOrigin: number, scaleFactor: number, gcs: GeographicCoordinateSystem);
	getCoordSysType(): CoordinateSystemType;

	toGeographicCoordinateRad(projPos: Coord2D): Coord2D;
	fromGeographicCoordinateRad(geoPos: Coord2D): Coord2D;
}

export class CoordinateSystemManager
{
	static srCreateGeogCSysData(srid: number, datumSrid: number, name: string): GeographicCoordinateSystem | null;
	static srCreateProjCSysData(srid: number, geogcsSrid: number, csysType: CoordinateSystemType, projName: string, falseEasting: number, falseNorthing: number, centralMeridian: number, latitudeOfOrigin: number, scaleFactor: number): ProjectedCoordinateSystem | null;
	static srCreateGeogCSys(srid: number): GeographicCoordinateSystem | null;
	static srCreateProjCSys(srid: number): ProjectedCoordinateSystem | null;
	static srCreateCsys(srid: number) : CoordinateSystem | null;
	static srGetDatumData(srid: number) : DatumData | null;
	static srGetSpheroid(srid: number) : Spheroid | null;
};
