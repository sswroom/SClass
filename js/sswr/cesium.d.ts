import { Cartesian3, Ellipsoid, Viewer } from "cesium";
import { Coord2D } from "./math";
import { Polygon } from "./geometry";

export function screenToLatLon(viewer: Viewer, x: number, y: number, ellipsoid: Ellipsoid) : Coord2D;
export function fromCXYZArray(arr: any[]): Cartesian3[];
export function toCartesian3Arr(coords: number[][]) : Cartesian3[];	
export function newObjFromGeoJSON(geoJSON: object) : object;
export function addGeoJSON(viewer: Viewer, geoJSON: object, color: any, extSize: number): void;
export function fromCartesian3Array(viewer: Viewer, arr: Cartesian3[]): object[];
export function fromPolygonGraphics(viewer: Viewer, pg: any): Polygon;
