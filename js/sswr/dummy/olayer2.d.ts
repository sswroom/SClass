export class Bounds
{
	constructor(left: number, bottom: number, right: number, top: number);
	constructor(bounds: number[]);
	toString(): string;
	toArray(reverseAxisOrder?: boolean): number[];
	toBBOX(decimal?: number, reverseAxisOrder?: boolean): string;
	toGeometry(): Geometry.Polygon;
	getWidth(): number;
	getHeight(): number;
	getSize(): Size;
	getCenterLonLat(): LonLat;
	scale(ratio: number, origin?: Pixel|LonLat): Bounds;
	add(x: number, y: number): Bounds;
	extend(object: LonLat|Geometry.Point, Bounds): void;
	extendXY(x: number, y: number): void;
	containsLonLat(ll: LonLat|{lon:number,lat:number}, options?: {inclusive?: boolean, worldBounds?: Bounds}): boolean;
	containsPixel(px: Pixel, inclusive?: boolean): boolean;
	contains(x: number, y: number, inclusive?: boolean): boolean;
	intersectsBounds(bounds: Bounds, options?: {inclusive?: boolean, worldBounds?: Bounds}): boolean;
	containsBounds(bounds: Bounds, partial?: boolean, inclusive?: boolean): boolean;
	determineQuadrant(lonlat: LonLat): string;
	transform(source: Projection, dest: Projection): Bounds;
	static fromString(str: string, reverseAxisOrder?: boolean): Bounds;
	static fromArray(bbox: number[], reverseAxisOrder?: boolean): Bounds;
	static fromSize(size: Size|{w: number, h: number}): Bounds;
}

export abstract class Feature
{
	popupClass: Class;
	constructor(layer: Layer, lonlat: LonLat, data: object);
}

export class Style
{
	fill?: boolean;
	fillColor?: string;
	fillOpacity?: number;
	stroke?: boolean;
	strokeColor?: string;
	strokeOpacity?: number;
	strokeWidth?: number;
	strokeLinecap?: string;
	strokeDashstyle?: string;
	graphic?: boolean;
	pointRadius?: number;
	pointerEvents?: string;
	cursor?: string;
	externalGraphic?: string;
	graphicWidth?: number;
	graphicHeight?: number;
	graphicOpacity?: number;
	graphicXOffset?: number;
	graphicYOffset?: number;
	rotation?: number;
	graphicZIndex?: number;
	graphicName?: string;
	graphicTitle?: string;
	title?: string;
	backgroundGraphic?: string;
	backgroundGraphicZIndex?: number;
	backgroundXOffset?: number;
	backgroundYOffset?: number;
	backgroundHeight?: number;
	backgroundWidth?: number;
	label?: string;
	labelAlign?: string;
	labelXOffset?: number;
	labelYOffset?: number;
	labelSelect?: boolean;
	labelOutlineColor?: string;
	labelOutlineWidth?: number;
	labelOutlineOpacity?: number;
	fontColor?: string;
	fontOpacity?: number;
	fontFamily?: string;
	fontSize?: string;
	fontStyle?: string;
	fontWeight?: string;
	display?: string;
}

namespace Feature
{
	export class Vector extends Feature
	{
		geometry: Geometry;
		attributes?: any;
		style?: Style;
		url: string;
		modified: object;
		constructor(geometry: Geometry, attributes?: any, style?: Style);
	}
}

export abstract class Geometry
{
	clone(): Geometry;
	getBounds(): Bounds;
	calculateBounds(): void;
	distanceTo(geometry: Geometry, options?: any): number|{distance: number, x0: number, y0: number, x1: number, x2: number};
	getVertices(nodes: boolean): Geometry.Point[];
	getCentroid(): Geometry.Point;
}

namespace Geometry
{
	export class Point extends Geometry
	{
		x: number;
		y: number;
		constructor(x: number, y: number);
		clone(): Point;
		distanceTo(geometry: Geometry, options?: {details?: boolean, edge?: boolean}): number|{distance: number, x0: number, y0: number, x1: number, x2: number};
		equals(geom: Point): boolean;
		move(x: number, y: number): void;
		rotate(angle: number, origin: Point): void;
		getCentroid(): Point;
		resize(scale: number, origin: Point, ratio?: number): Geometry;
		intersects(geometry: Geometry): boolean;
		transform(source: Projection, dest: Projection): Geometry;
		getVertices(nodes: boolean): Point[];
	}
}

export class Icon
{
	url: string;
	size: Size;
	offset: Pixel;
	imageDiv: HTMLImageElement;

	constructor(url: string, size: Size|{w: number, h: number}, offset: Pixel|{x: number, y: number}, calculateOffset?: ()=>void);
	isDrawn(): boolean;
}

export class LonLat
{
	lon: number;
	lat: number;

	constructor(lon: number, lat: number);
	toShortString(): string;
	clone(): LonLat;
	add(lon: number, lat: number): LonLat;
	equals(ll: LonLat|null):boolean;
	transform(source: Projection, dest: Projection): LonLat;
	wrapDateLine(maxExtent: Bounds): LonLat;
}

export class Marker
{
	lonlat: LonLat;
	icon: Icon;

	constructor(lonlat: LonLat, icon: Icon);
	destroy(): void;
	isDrawn(): boolean;
}

export class Pixel
{
	x: number;
	y: number;

	constructor(x: number, y: number);
	clone(): Pixel;
	equals(px: Pixel|null): boolean;
	distanceTo(px: Pixel): number;
	add(x: number, y: number): Pixel;
	offset(px: Pixel|{x: number, y: number}): Pixel;
}

export class Projection
{
	constructor(projCode: string, options?: {});
	getCode(): string;
	getUnits(): string;

	static addTransform(from: string, to: string, method: (point: Point) => void): void;
	static transform(point: Point|{x: number, y: number}, source: Projection, dest: Projection): Point;
	static nullTransform(point: Point): void;
}

export class Size
{
	w: number;
	h: number;
	constructor(w: number, h: number);
	clone(): Size;
	equals(sz: Size|null): boolean;
}