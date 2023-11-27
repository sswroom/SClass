import Coord2D from "./math/Coord2D";

export function lon2PixelX(lon: number, level: number, tileSize: number): number;
export function lat2PixelY(lat: number, level: number, tileSize: number): number;
export function pixelX2Lon(x: number, level: number, tileSize: number): number;
export function pixelY2Lat(y: number, level: number, tileSize: number): number;
export function tileUrls(osmUrl: string, minCoord: Coord2D, maxCoord: Coord2D, minLev: number, maxLev: number): string[];
