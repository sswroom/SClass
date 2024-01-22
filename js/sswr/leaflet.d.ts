import * as L from "./node_modules/leaflet/src/Leaflet";
import * as kml from "./kml";
import { Coord2D, RectArea } from "./math";
import * as map from "./map";

export function fromLatLon(latLon: L.LatLng): Coord2D;
export function fromLatLng(latLng: L.LatLng): Coord2D;
export function fromLatLngBounds(b: L.LatLngBounds): RectArea;
export function createLayer(layer: map.LayerInfo, options: object): L.Layer;
export function createKMLLookAt(map: L.Map): kml.LookAt;
export function toKMLFeature(layer: L.Layer, doc?: kml.Document): kml.Feature | null;
export function toKMLString(layer: L.Layer): string | null;

export class LeafletMap extends map.MapControl
{
	constructor(divId: string);
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
}
