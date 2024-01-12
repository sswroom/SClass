import { CoordinateSystem, Vector3 } from "./math";
import { LineString, Vector2D } from "./geometry";
import { Timestamp } from "./data";

export enum DataFormat
{
	Cesium,
	WKT,
	GeoJSON
};

export enum WebMapType
{
	WMS,
	WFS,
	ArcGIS,
	OSMTile
};

export enum GeoJSONType
{
	Feature,
	FeatureCollection
};

export enum GeometryType
{
	Point,
	MultiPoint,
	LineString,
	MultiLineString,
	Polygon,
	MultiPolygon,
	GeometryCollection
};

declare class LayerInfo
{
	name: string;
	url: string;
	type: WebMapType;
	layers?: string;
	format?: string;
	minZoom?: number;
	maxZoom?: number;
};

declare class GeoJSON
{
	type: GeoJSONType;
	bbox?: number[];
};

declare class Geometry
{
	type: GeometryType;
	coordinates?: any[];
	geometries?: Geometry[];
};

declare class GeoJSONFeature<PropType> extends GeoJSON
{
	id?: string | number;
	geometry: Geometry;
	properties?: PropType;
};

declare class GeoJSONFeatureCollection extends GeoJSON
{
	features: GeoJSONFeature[];
};

export function calcDistance(srid: number, geom: Vector2D, x: number, y: number): number;
export function getLayers(svcUrl: string, onResultFunc: Function): void;
export function getLayerData(svcUrl: string, onResultFunc: Function, layerName: string, dataFormat: DataFormat): void;

declare class GPSRecord
{
	t: number;
	lat: number;
	lon: number;
	a: number;
	s: number;
	d: number;
	v: boolean;
	sate: number;
};

export class GPSTrack
{
	recs: GPSRecord[];
	constructor(recs: GPSRecord[]);
	createLineString(): LineString;
	getPosByTime(ts: Timestamp): Vector3;
	getPosByTicks(ticks: number): Vector3;
};

export class GeolocationFilter
{
	lastPos?: GeolocationPosition;
	csys: CoordinateSystem;
	minSecs: number;
	minDistMeter: number;

	constructor(minSecs: number, minDistMeter: number);
	isValid(pos: GeolocationPosition): boolean;
};
