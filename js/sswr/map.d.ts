import { Vector3 } from "./math";
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

declare class LayerInfo
{
	name: string;
	url: string;
	type: WebMapType;
	layers?: string;
	format?: string;
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
}

export class GPSTrack
{
	recs: GPSRecord[];
	constructor(recs: GPSRecord[]);
	createLineString(): LineString;
	getPosByTime(ts: Timestamp): Vector3;
	getPosByTicks(ticks: number): Vector3;
}
