import * as data from "./data.js";
import * as kml from "./kml.js";
import { Coord2D } from "./math.js";
import * as map from "./map.js";

export function fromLatLon(latLon)
{
	return new Coord2D(latLon.lng, latLon.lat);
}

export function createLayer(layer, options)
{
	var lyrOpts;
	if (options == null)
		options = {};
	switch (layer.type)
	{
	case map.WebMapType.OSMTile:
		lyrOpts = data.mergeOptions({}, options);
		if (layer.minZoom)
			lyrOpts.minZoom = layer.minZoom;
		if (layer.maxZoom)
			lyrOpts.maxZoom = layer.maxZoom;
		lyrOpts.maxNativeZoom = lyrOpts.maxZoom;
		lyrOpts.maxZoom = 25;
		return L.tileLayer(layer.url, lyrOpts);
	case map.WebMapType.WMS:
		lyrOpts = data.mergeOptions({layers: layer.layers || layer.name, format: layer.format || 'image/png'}, options);
		if (layer.minZoom)
			lyrOpts.minZoom = layer.minZoom;
		if (layer.maxZoom)
			lyrOpts.maxZoom = layer.maxZoom;
		if (lyrOpts.format.startsWith("image/png"))
			lyrOpts.transparent = true;
		lyrOpts.maxNativeZoom = lyrOpts.maxZoom;
		lyrOpts.maxZoom = 25;
		return L.tileLayer.wms(layer.url, lyrOpts);
	}
	return null;
}

export function createKMLLookAt(map)
{
	var center = map.getCenter();
	var zoom = map.getZoom();
	var range = 100000 * Math.pow(2, 11 - zoom);
	return new kml.LookAt(center.lng, center.lat, 0, range);
}

export function toKMLFeature(layer)
{
	if (layer instanceof L.FeatureGroup)
	{
		var i;
		for (i in layer._layers)
		{
			toKMLFeature(layer._layers[i]);
		}
	}
	else if (layer instanceof L.Marker)
	{
		console.log("Marker found", layer);
	}
	else if (layer instanceof L.Polyline)
	{
		console.log("Polyline found", layer);
	}
	else
	{
		console.log("Unknown type", layer);
	}
}
