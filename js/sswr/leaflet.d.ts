import * as L from "./node_modules/leaflet/src/Leaflet";
import { Coord2D } from "./math";
import { LayerInfo } from "./map";

export function fromLatLon(latLon: L.LatLng): Coord2D;
export function createLayer(layer: LayerInfo, options: object): L.Layer;