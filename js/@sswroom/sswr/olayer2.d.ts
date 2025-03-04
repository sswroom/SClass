import * as geometry from "./geometry";
import * as kml from "./kml";
import * as map from "./map";
import * as math from "./math";

declare class Olayer2Options
{
	objProjection: OpenLayers.Projection;
	mapProjection: OpenLayers.Projection;
	map: OpenLayers.Map;
};

export function toPointArray(numArr: number[][], options: Olayer2Options): OpenLayers.Geometry.Point[];
export function createFromKML(feature: kml.Feature | kml.KMLFile, options: Olayer2Options): Promise<OpenLayers.Feature.Vector | OpenLayers.Marker | any[] | null>;
export function createFromGeometry(geom: geometry, options: Olayer2Options): Promise<OpenLayers.Marker | OpenLayers.Geometry | null>;

export class Olayer2Map extends map.MapControl
{
	inited: boolean;
	mapId: string;
	mapProjection: OpenLayers.Projection;
	map: OpenLayers.Map;
	Click: OpenLayers.Control;
	mouseCtrl: Olayer2Map.Click;
	currMarkerPopup: any;
	currMarkerPopupObj: any;
	lclickFunc: (lat: number, lon: number, scnX: number, scnY: number)=>void;
	moveFunc: (lat: number, lon: number)=>void;
	posFunc: (lat: number, lon: number)=>void;

	constructor(mapId: string);
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

	createMarker(mapPos: math.Coord2D, imgURL: string, imgWidth: number, imgHeight: number, options?: map.MarkerOptions): any;
	layerAddMarker(markerLayer: any, marker: any): void;
	layerRemoveMarker(markerLayer: any, marker: any): void;
	layerClearMarkers(markerLayer: any): void;
	markerIsOver(marker: any, scnPos: math.Coord2D): boolean;

	createGeometry(geom: geometry.Vector2D, options: map.GeometryOptions): any;
	layerAddGeometry(geometryLayer: any, geom: any): void;
	layerRemoveGeometry(geometryLayer: any, geom: any): void;
	layerClearGeometries(goemetryLayer: any): void;

	toKMLFeature(layer: any, doc?: kml.Document): kml.Feature | null;

	private eventLClicked(e: any): void;
	private eventMoved(): void;
	private eventMouseMoved(event: any): void;
}
