import * as data from "./data.js";
import { Coord2D } from "./math.js";
import * as map from "./map.js";

export function fromLatLon(latLon)
{
	return new Coord2D(latLon.lng, latLon.lat);
}

export function createLayer(layer, options)
{
	if (options == null)
		options = {};
	switch (layer.type)
	{
	case map.WebMapType.OSMTile:
		return L.tileLayer(layer.url, options);
	case map.WebMapType.WMS:
		return L.tileLayer.wms(layer.url, data.mergeOptions({layers: layer.layers || layer.name, format: layer.format || 'image/png'}, options));
	}
	return null;
}