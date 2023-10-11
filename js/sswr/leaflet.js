import { Coord2D } from "./math.js";

export function fromLatLon(latLon)
{
	return new Coord2D(latLon.lng, latLon.lat);
}
