import * as math from "./math";

declare class BoundaryPointResult
{
	x: number;
	y: number;
	dist: number;
}

export enum VectorType
{
	Unknown,
	Point,
	LineString,
	Polygon,
	MultiPoint,
	Polyline, //MultiLineString
	MultiPolygon,
	GeometryCollection,
	CircularString,
	CompoundCurve,
	CurvePolygon,
	MultiCurve,
	MultiSurface,
	Curve,
	Surface,
	PolyhedralSurface,
	Tin,
	Triangle,
	LinearRing,

	Image,
	String,
	Ellipse,
	PieArea
}

export abstract class Vector2D {
	srid: number;
	type: VectorType;
	constructor(srid: number);
	abstract calBoundaryPoint(coord: math.Coord2D): BoundaryPointResult;
	insideOrTouch(coord: math.Coord2D): boolean;
	getBounds(): math.RectArea;
	abstract hasArea(): boolean;
	abstract calcHIntersacts(y: double): number[];
	abstract getDisplayCenter(): math.Coord2D;
}

export class Point extends Vector2D
{
	coordinates: number[];
	constructor(srid: number, coordinates: number[] | math.Coord2D | math.Vector3);
	calBoundaryPoint(coord: math.Coord2D): BoundaryPointResult;
	insideOrTouch(coord: math.Coord2D): boolean;
	getBounds(): math.RectArea;
	hasArea(): boolean;
	calcHIntersacts(y: double): number[];
	getDisplayCenter(): math.Coord2D;
}


export class LineString extends Vector2D
{
	coordinates: number[][];
	constructor(srid: number, coordinates: number[][]);
	calBoundaryPoint(coord: math.Coord2D): BoundaryPointResult;
	insideOrTouch(coord: math.Coord2D): boolean;
	getBounds(): math.RectArea;
	hasArea(): boolean;
	calcHIntersacts(y: double): number[];
	getDisplayCenter(): math.Coord2D;
}

export class LinearRing extends LineString
{
	constructor(srid: number, coordinates: number[][]);
	insideOrTouch(coord: math.Coord2D): boolean;
	isOpen(): boolean;
	isClose(): boolean;
	toPolygon(): Polygon;
	hasArea(): boolean;
	getDisplayCenter(): math.Coord2D;

	static getIntersactsCenter(vals: number[]): number;
	static createFromCircle(srid: number, center: math.Coord2D, radiusX: number, radiusY: number, nPoints: number): LinearRing;
}

export class MultiGeometry<VecType> extends Vector2D
{
	geometries: VecType[];

	constructor(srid: number);
	calBoundaryPoint(coord: math.Coord2D): BoundaryPointResult;
	insideOrTouch(coord: math.Coord2D): boolean;
	getBounds(): math.RectArea;
	hasArea(): boolean;
	getDisplayCenter(): math.Coord2D;
}

export class Polygon extends MultiGeometry<LinearRing>
{
	constructor(srid: number, coordinates?: number[][][]);
	insideOrTouch(coord: math.Coord2D): boolean;
}

export class MultiPolygon extends MultiGeometry<Polygon>
{
	constructor(srid: number, coordinates?: number[][][][]);
}

export class Polyline extends MultiGeometry<LineString>
{
	constructor(srid: number, coordinates?: number[][][]);
}

export class GeometryCollection extends MultiGeometry<Vector2D>
{
	constructor(srid: number);
}