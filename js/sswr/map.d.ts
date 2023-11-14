import { LineString, Vector2D } from "./geometry";

export let DataFormat: object;
export function calcDistance(srid: number, geom: Vector2D, x: number, y: number): number;
export function getLayers(svcUrl: string, onResultFunc: Function): void;
export function getLayerData(svcUrl: string, onResultFunc: Function, layerName: string, dataFormat: string): void;

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
}
