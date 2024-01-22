import * as math from "./math";

export function lon2PixelX(lon: number, level: number, tileSize: number): number;
export function lat2PixelY(lat: number, level: number, tileSize: number): number;
export function pixelX2Lon(x: number, level: number, tileSize: number): number;
export function pixelY2Lat(y: number, level: number, tileSize: number): number;
export function tileUrls(osmUrl: string, minCoord: math.Coord2D, maxCoord: math.Coord2D, minLev: number, maxLev: number): string[];
export function removeTileUrls(urls: string[], osmUrl: string, minLev: number, maxLev: number, areaList: math.RectArea[]): void;
export function scale2Level(scale: number): number;