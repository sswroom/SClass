import * as data from "./data.js";
import * as geometry from "./geometry.js";
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

export function toPoint(layer)
{
	if (layer instanceof L.Marker)
	{
		return new geometry.Point(4326, fromLatLon(layer._latlng));
	}
	return null;
}

export function toLineString(layer)
{
	if (layer instanceof L.Polyline)
	{
		var coords = [];
		var i;
		for (i in layer._latlngs)
		{
			coords.push([layer._latlngs[i].lng,layer._latlngs[i].lat]);
		}
		return new geometry.LineString(4326, coords);
	}
	return null;
}

export function toKMLFeature(layer, doc)
{
	var feature;
	if (doc == null)
	{
		doc = new kml.Document();
		feature = toKMLFeature(layer, doc);
		if (feature)
		{
			doc.addFeature(feature);
			return doc;
		}
		return null;
	}
	if (layer instanceof L.FeatureGroup)
	{
		var featureGroup = new kml.Folder();
		featureGroup.setName("FeatureGroup");
		var i;
		for (i in layer._layers)
		{
			feature = toKMLFeature(layer._layers[i], doc);
			if (feature)
				featureGroup.addFeature(feature);
		}
		return featureGroup;
	}
	else if (layer instanceof L.Marker)
	{
		feature = new kml.Placemark(toPoint(layer));
		feature.setName("Marker");
//		console.log("Marker found", layer);
		if (layer._popup && layer._popup._content)
		{
			feature.setDescription(layer._popup._content);
		}
		return feature;
	}
	else if (layer instanceof L.Polyline)
	{
		feature = new kml.Placemark(toLineString(layer));
		feature.setName("Polyline");
		if (layer.options.color || layer.options.weight)
		{
			var lineStyle = new kml.LineStyle();
			if (layer.options.color)
				lineStyle.fromCSSColor(layer.options.color);
			if (layer.options.weight)
				lineStyle.setWidth(layer.options.weight);
			feature.setStyle(doc.getOrNewStyle(null, null, lineStyle, null, null, null));
		}
		return feature;
	}
	else
	{
		console.log("Unknown type", layer);
	}
	return null;
}

export function toKMLString(layer)
{
	var feature = toKMLFeature(layer);
	if (feature)
	{
		return kml.toString(feature);
	}
	return null;
}