import { Vector2D } from "./geometry";

export let DataFormat: object;
export function calcDistance(srid: number, geom: Vector2D, x: number, y: number): number;
export function getLayers(svcUrl: string, onResultFunc: Function): void;
export function getLayerData(svcUrl: string, onResultFunc: Function, layerName: string, dataFormat: string): void;
