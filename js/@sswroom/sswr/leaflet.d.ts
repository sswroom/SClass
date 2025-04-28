import * as L from "./node_modules/leaflet/src/Leaflet";
import * as geometry from "./geometry";
import * as kml from "./kml";
import * as math from "./math";
import * as map from "./map";

declare class GeometryOptions
{
	name: string;
	icon: object;
}

declare class KMLFeatureOptions
{
	noPopup: boolean;
}

export function fromLatLon(latLon: L.LatLng): math.Coord2D;
export function fromLatLng(latLng: L.LatLng): math.Coord2D;
export function fromLatLngBounds(b: L.LatLngBounds): math.RectArea;
export function toLatLngBounds(rect: math.RectArea): L.LatLngBounds;

export function createLayer(layer: map.LayerInfo, options?: object): L.Layer;
export function createFromKML(feature: kml.Feature | kml.KMLFile, options?: KMLFeatureOptions): L.Layer;
export function createFromGeometry(geom: geometry.Vector2D, options: GeometryOptions): L.Layer;
export function createKMLLookAt(map: L.Map): kml.LookAt;
export function toKMLFeature(layer: L.Layer, doc?: kml.Document): kml.Feature | null;
export function toKMLString(layer: L.Layer): string | null;

export class KMLNetworkLink
{
	feature: kml.NetworkLink;
	container: L.FeatureGroup;

	constructor(feature: kml.NetworkLink);
	reload(): void;
	addTo(container: L.FeatureGroup): KMLNetworkLink;
}

export class LeafletMap extends map.MapControl
{
	constructor(divId: string);
	getDiv(): HTMLDivElement;
	sizeUpdated(): void;
	createLayer(layer: map.LayerInfo, options?: map.LayerOptions): any;
	createMarkerLayer(name: string, options?: map.LayerOptions): any;
	createGeometryLayer(name: string, options?: map.LayerOptions): any;
	addLayer(layer: any): void;
	addKML(feature: kml.Feature | kml.KMLFile): void;
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

	createMarker(mapPos: math.Coord2D, imgURL: string, imgWidth: number, imgHeight: number, options?: map.MarkerOptions): map.MarkerInfo;
	layerAddMarker(markerLayer: any, marker: map.MarkerInfo): void;
	layerRemoveMarker(markerLayer: any, marker: map.MarkerInfo): void;
	layerClearMarkers(markerLayer: any): void;
	layerMoveMarker(markerLayer: any, marker: MarkerInfo, mapPos: math.Coord2D): MarkerInfo;
	markerUpdateIcon(markerLayer: any, marker: MarkerInfo, url: string): MarkerInfo;
	markerIsOver(marker: map.MarkerInfo, scnPos: math.Coord2D): boolean;
	markerShowPopup(marker: MarkerInfo, content: string, w?: number, h?: number): void;
	hidePopup();

	createGeometry(geom: geometry.Vector2D, options: map.GeometryOptions): any;
	layerAddGeometry(geometryLayer: any, geom: any): void;
	layerRemoveGeometry(geometryLayer: any, geom: any): void;
	layerClearGeometries(geometryLayer: any): void;
}
