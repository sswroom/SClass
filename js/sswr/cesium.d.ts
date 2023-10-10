import { Cartesian3, Ellipsoid, Viewer } from "cesium";
import { Coord2D } from "./math";
import { Polygon } from "./geometry";

declare function screenToLatLon(viewer: Viewer, x: number, y: number, ellipsoid: Ellipsoid) : Coord2D;
declare function fromCXYZArray(arr: any[]): Cartesian3[];
declare function toCartesian3Arr(coords: number[][]) : Cartesian3[];	
declare function newObjFromGeoJSON(geoJSON: object) : object;
declare function addGeoJSON(viewer: Viewer, geoJSON: object, color: any, extSize: number): void;
declare function fromCartesian3Array(viewer: Viewer, arr: Cartesian3[]): object[];
declare function fromPolygonGraphics(viewer: Viewer, pg: any): Polygon;
