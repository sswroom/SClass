import * as data from "./data";
import * as geometry from "./geometry";
import * as kml from "./kml";
import * as map from "./map";
import * as math from "./math";

export enum DataFormat
{
	Cesium,
	WKT,
	GeoJSON
}

export enum WebMapType
{
	WMS,
	WFS,
	ArcGIS,
	OSMTile
}

export enum GeoJSONType
{
	Feature,
	FeatureCollection
}

export enum GeometryType
{
	Point,
	MultiPoint,
	LineString,
	MultiLineString,
	Polygon,
	MultiPolygon,
	GeometryCollection
}

declare class LayerInfo
{
	name: string;
	url: string;
	type: WebMapType;
	layers?: string;
	format?: string;
	minZoom?: number;
	maxZoom?: number;
	minX?: number;
	minY?: number;
	maxX?: number;
	maxY?: number;
}

declare class GeoJSON
{
	type: GeoJSONType;
	bbox?: number[];
}

declare class Geometry
{
	type: GeometryType;
	coordinates?: number[]|number[][]|number[][][]|number[][][][];
	geometries?: Geometry[];
}

declare class GeoJSONFeature<PropType> extends GeoJSON
{
	id?: string | number;
	geometry: Geometry;
	properties?: PropType;
}

declare class GeoJSONFeatureCollection<PropType> extends GeoJSON
{
	features: GeoJSONFeature<PropType>[];
}

export function calcDistance(srid: number, geom: geometry.Vector2D, x: number, y: number): number;
export function getLayers(svcUrl: string, onResultFunc: Function): void;
export function getLayerData(svcUrl: string, onResultFunc: Function, layerName: string, dataFormat: DataFormat): void;

declare class GPSRecord
{
	recTime: number;
	lat: number;
	lon: number;
	altitude: number;
	speed: number;
	heading: number;
	valid: boolean;
	sateUsed: number;
}

export class GPSTrack extends data.ParsedObject
{
	recs: GPSRecord[];

	constructor(recs: GPSRecord[], sourceName?: string);
	addPosition(pos: GeolocationPosition);
	getTrackCnt(): number;
	getTrack(index: number): GPSRecord[]|null;
	createLineString(): geometry.LineString;
	getPosByTime(ts: data.Timestamp): math.Vector3;
	getPosByTicks(ticks: number): math.Vector3;
}

export class GeolocationFilter
{
	lastPos?: GeolocationPosition;
	csys: math.CoordinateSystem;
	minSecs: number;
	minDistMeter: number;

	constructor(minSecs: number, minDistMeter: number);
	isValid(pos: GeolocationPosition): boolean;
}


declare class LayerOptions
{
}

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

declare class MarkerInfo
{
}

export abstract class MapControl
{
	constructor();
	abstract getDiv(): HTMLDivElement;
	abstract sizeUpdated(): void;
	abstract createLayer(layer: map.LayerInfo, options?: LayerOptions): any;
	abstract createMarkerLayer(name: string, options?: LayerOptions): any;
	abstract createGeometryLayer(name: string, options?: LayerOptions): any;
	abstract addLayer(layer: any): void;
	abstract addKML(feature: kml.Feature | kml.KMLFile): void;
	abstract uninit(): void;
	abstract zoomIn(): void;
	abstract zoomOut(): void;
	abstract zoomScale(scale: number): void;
	abstract panTo(pos: math.Coord2D): void;
	abstract panZoomScale(pos: math.Coord2D, scale: number): void;
	abstract zoomToExtent(extent: math.RectArea): void;
	abstract handleMouseLClick(clickFunc: (mapPos: math.Coord2D, scnPos: math.Coord2D)=>void): void;
	abstract handleMouseMove(moveFunc: (mapPos: math.Coord2D)=>void): void;
	abstract handlePosChange(posFunc: (mapPos: math.Coord2D)=>void): void;
	abstract map2ScnPos(mapPos: math.Coord2D): math.Coord2D;
	abstract scn2MapPos(scnPos: math.Coord2D): math.Coord2D;

	abstract createMarker(mapPos: math.Coord2D, imgURL: string, imgWidth: number, imgHeight: number, options?: MarkerOptions): MarkerInfo;
	abstract layerAddMarker(markerLayer: any, marker: MarkerInfo): void;
	abstract layerRemoveMarker(markerLayer: any, marker: MarkerInfo): void;
	abstract layerClearMarkers(markerLayer: any): void;
	abstract layerMoveMarker(markerLayer: any, marker: MarkerInfo, mapPos: math.Coord2D): MarkerInfo;
	abstract markerUpdateIcon(markerLayer: any, marker: MarkerInfo, url: string): MarkerInfo;
	abstract markerIsOver(marker: MarkerInfo, scnPos: math.Coord2D): boolean;
	abstract markerShowPopup(marker: MarkerInfo, content: string, w?: number, h?: number): void;
	abstract hidePopup();

	abstract createGeometry(geom: geometry.Vector2D, options: GeometryOptions): any;
	abstract layerAddGeometry(geometryLayer: any, geom: any): void;
	abstract layerRemoveGeometry(geometryLayer: any, geom: any): void;
	abstract layerClearGeometries(geometryLayer: any): void;
}

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
	queryInfos(mapPos: math.Coord2D, bounds: math.RectArea, width: number, height: number): Promise<OWSFeatureCollection | null>;
}
