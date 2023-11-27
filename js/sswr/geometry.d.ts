declare class BoundaryPointResult
{
	x: number;
	y: number;
	dist: number;
};

export class Vector2D {
	type: string;
	constructor(srid: number);
	insideVector(x: number, y: number): boolean;
};

export class LineString extends Vector2D
{
	coordinates: number[][];
	constructor(srid: number, coordinates: number[][]);
	calBoundaryPoint(x: number, y: number): BoundaryPointResult;
}

export class MultiGeometry extends Vector2D
{
	coordinates: Vector2D[];

	constructor(srid: number);
	calBoundaryPoint(x: number, y: number): BoundaryPointResult;
	insideVector(x: number, y: number): boolean;
}

export class MultiPolygon extends MultiGeometry
{
	constructor(srid: number, coordinates: number[][][][]);
}

export class Polygon extends Vector2D
{
	coordinates: number[][][];
	constructor(srid: number, coordinates: number[][][]);
	calBoundaryPoint(x: number, y: number): BoundaryPointResult;
	insideVector(x: number, y: number): boolean;
}
