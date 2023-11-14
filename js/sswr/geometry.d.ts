declare class BoundaryPointResult
{
	x: number;
	y: number;
	dist: number;
};

declare class Vector2D {
	type: string;
	constructor(srid: number);
	insideVector(x: number, y: number): boolean;
};

declare class LineString extends Vector2D
{
	coordinates: number[][];
	constructor(srid: number, coordinates: number[][]);
	calBoundaryPoint(x: number, y: number): BoundaryPointResult;
}

declare class MultiGeometry extends Vector2D
{
	coordinates: Vector2D[];

	constructor(srid: number);
	calBoundaryPoint(x: number, y: number): BoundaryPointResult;
	insideVector(x: number, y: number): boolean;
}

declare class MultiPolygon extends MultiGeometry
{
	constructor(srid: number, coordinates: number[][][][]);
}

declare class Polygon extends Vector2D
{
	coordinates: number[][][];
	constructor(srid: number, coordinates: number[][][]);
	calBoundaryPoint(x: number, y: number): BoundaryPointResult;
	insideVector(x: number, y: number): boolean;
}
