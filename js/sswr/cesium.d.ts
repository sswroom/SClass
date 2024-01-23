import { Cartesian3, Ellipsoid, Viewer } from "cesium";
import { Polygon } from "./geometry";
import * as kml from "./kml";
import * as map from "./map";
import { Coord2D } from "./math";

export function screenToLatLon(viewer: Viewer, x: number, y: number, ellipsoid: Ellipsoid) : Coord2D;
export function fromCXYZArray(arr: any[]): Cartesian3[];
export function toCartesian3Arr(coords: number[][]) : Cartesian3[];	
export function newObjFromGeoJSON(geoJSON: object) : object;
export function addGeoJSON(viewer: Viewer, geoJSON: object, color: any, extSize: number): void;
export function fromCartesian3Array(viewer: Viewer, arr: Cartesian3[]): object[];
export function fromPolygonGraphics(viewer: Viewer, pg: any): Polygon;

export class CesiumMap extends map.MapControl
{
	constructor(divId: string);
	createLayer(layer: map.LayerInfo, options?: LayerOptions): any;
	createMarkerLayer(name: string, options?: LayerOptions): any;
	createGeometryLayer(name: string, options?: LayerOptions): any;
	addLayer(layer: any): void;
	addKMLFeature(feature: kml.Feature): void;
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
}
