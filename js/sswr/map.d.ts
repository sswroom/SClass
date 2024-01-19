import { Timestamp } from "./data";
import * as geometry from "./geometry";
import * as map from "./map";
import * as math from "./math";

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

export function calcDistance(srid: number, geom: geometry.Vector2D, x: number, y: number): number;
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
	createLineString(): geometry.LineString;
	getPosByTime(ts: Timestamp): math.Vector3;
	getPosByTicks(ticks: number): math.Vector3;
};

export class GeolocationFilter
{
	lastPos?: GeolocationPosition;
	csys: math.CoordinateSystem;
	minSecs: number;
	minDistMeter: number;

	constructor(minSecs: number, minDistMeter: number);
	isValid(pos: GeolocationPosition): boolean;
};


declare class LayerOptions
{
};

declare class MarkerOptions
{
	zIndex?: number;
}

declare class GeometryOptions
{
	lineColor?: string;
	lineWidth?: number;
	fillColor?: string;
	fillOpacity?: number;
}

export class MapControl
{
	constructor();
	createLayer(layer: map.LayerInfo, options?: LayerOptions): any;
	createMarkerLayer(name: string, options?: LayerOptions): any;
	createGeometryLayer(name: string, options?: LayerOptions): any;
	addLayer(layer: any);
	uninit(): void;
	zoomIn(): void;
	zoomOut(): void;
	zoomScale(scale: number): void;
	panTo(pos: math.Coord2D): void;
	panZoomScale(pos: math.Coord2D, scale: number): void;
	zoomToExtent(extent: math.RectArea): void;
	handleMouseLClick(clickFunc: (mapPos: math.Coord2D, scnPos: math.Coord2D)=>void): void;
	handleMouseMove(moveFunc: (mapPos: math.Coord2D)=>void): void;
	handlePosChange(posFunc: (mapPos: math.Coord2D)=>void): void;
	map2ScnPos(mapPos: math.Coord2D): math.Coord2D;
	scn2MapPos(scnPos: math.Coord2D): math.Coord2D;

	createMarker(mapPos: math.Coord2D, imgURL: string, imgWidth: number, imgHeight: number, options?: MarkerOptions): any;
	layerAddMarker(markerLayer: any, marker: any): void;
	layerRemoveMarker(markerLayer: any, marker: any): void;
	layerClearMarkers(markerLayer: any): void;
	markerIsOver(marker: any, scnPos: math.Coord2D): boolean;

	createGeometry(geom: geometry.Vector2D, options: GeometryOptions): any;
	layerAddGeometry(geometryLayer: any, geom: any): void;
	layerRemoveGeometry(geometryLayer: any, geom: any): void;
	layerClearGeometries(geometryLayer: any): void;
};

declare class OWSSpatialReference
{
	type: string;
	properties: string;
}

declare class OWSFeature
{
	type: string;
	id: string;
	geometry_name?: string;
	geometry: Geometry;
	properties: object;
}

declare class OWSFeatureCollection
{
	crs: OWSSpatialReference;
	features: OWSFeature[];
	numberReturned: number;
	timeStamp: string;
	totalFeatures: number | string;
	type: string;
}

export class WMS
{
	url: string;
	layer: string;
	version: string;

	constructor(url: string, layer: string, version?: string);
	queryInfos(mapPos: math.Coord2D, bounds: math.RectArea, width: number, height: number): Promise<object | null>;
}
