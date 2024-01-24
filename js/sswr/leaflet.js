import * as data from "./data.js";
import * as geometry from "./geometry.js";
import * as kml from "./kml.js";
import * as math from "./math.js";
import * as map from "./map.js";
import * as osm from "./osm.js";
import * as text from "./text.js";
import * as web from "./web.js";

export function fromLatLon(latLon)
{
	return new math.Coord2D(latLon.lng, latLon.lat);
}

export function fromLatLng(latLng)
{
	return new math.Coord2D(latLng.lng, latLng.lat);
}

export function fromLatLngBounds(b)
{
	return new math.RectArea(b.getWest(), b.getSouth(), b.getEast(), b.getNorth());
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

export function createFromKMLFeature(feature, options)
{
	options = data.mergeOptions(options, {noPopup: false});
	if (feature instanceof kml.Container)
	{
		var i;
		var layers = L.featureGroup();
		for (i in feature.features)
		{
			createFromKMLFeature(feature.features[i], options).addTo(layers);
		}
		return layers;
	}
	else if (feature instanceof kml.Placemark)
	{
		var opt = {};
		if (feature.name)
			opt.name = feature.name;
		if (feature.style)
		{
			var style = feature.style;
			if (style instanceof kml.StyleMap)
			{
				style = style.normalStyle;
			}
			if (style instanceof kml.Style)
			{
				if (style.iconStyle)
				{
					var s = style.iconStyle;
					if (s.leafletIcon == null)
					{
						var icon = {};
						if (s.iconUrl)
						{
							icon.iconUrl = s.iconUrl;
						}
						if (s.hotSpotX && s.hotSpotY)
						{
							icon.iconAnchor = [s.hotSpotX, s.hotSpotY];
						}
						s.leafletIcon = L.icon(icon);
					}
					opt.icon = s.leafletIcon;
				}
				if (style.lineStyle)
				{
					var ls = style.lineStyle;
					if (ls.color)
						opt.lineColor = kml.toCSSColor(ls.color);
					if (ls.width)
						opt.lineWidth = ls.width;
				}
			}
		}
		var layer = createFromGeometry(feature.vec, opt);
		if (layer && !options.noPopup)
		{
			if (feature.name && feature.description)
			{
				layer.bindPopup("<b>"+text.toHTMLText(feature.name)+"</b><br/>"+feature.description);
			}
			else if (feature.name)
			{
				layer.bindPopup("<b>"+text.toHTMLText(feature.name)+"</b>");
			}
			else if (feature.description)
			{
				layer.bindPopup(feature.description);
			}
		}
		return layer;
	}
	else
	{
		console.log("Unknown KML feature type", feature);
		return null;
	}
}

export function createFromGeometry(geom, options)
{
	if (geom instanceof geometry.Point)
	{
		var opt = {};
		if (options)
		{
			if (options.name)
				opt.title = options.name;
			if (options.icon)
				opt.icon = options.icon;
		}
		return L.marker(L.latLng(geom.coordinates[1], geom.coordinates[0]), opt);
	}
	else if (geom instanceof geometry.LineString)
	{
		var opt = {};
		var i;
		var pts = [];
		if (options.lineColor)
			opt.color = options.lineColor;
		if (options.lineWidth)
			opt.weight = options.lineWidth;
		for (i in geom.coordinates)
		{
			var latLng = L.latLng(geom.coordinates[i][1], geom.coordinates[i][0]);
			if (latLng)
			{
				pts.push(latLng);
			}
			else
			{
				console.log("Error in LineString", geom.coordinates[i]);
			}
		}
		return L.polyline(pts, opt);
	}
	else
	{
		console.log("Unknown geometry type", geom);
		return null;
	}
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
		if (layer.options && layer.options.icon)
		{
			var opt = layer.options.icon.options;
			if (opt)
			{
				var iconStyle = new kml.IconStyle();
				if (opt.iconSize && layer._icon)
				{
					var imgW = layer._icon.naturalWidth || layer._icon.offsetWidth;
					iconStyle.setScale(opt.iconSize[0] / imgW);
				}
				if (opt.iconAnchor)
				{
					iconStyle.setHotSpotX(opt.iconAnchor[0], kml.HotSpotUnit.Pixels);
					iconStyle.setHotSpotY(opt.iconAnchor[1], kml.HotSpotUnit.InsetPixels);
				}
				if (opt.iconUrl)
				{
					if (opt.iconUrl == "marker-icon.png")
					{
						iconStyle.setIconUrl(web.appendUrl("../../leaflet/dist/images/marker-icon.png", import.meta.url));
					}
					else
					{
						iconStyle.setIconUrl(web.appendUrl(opt.iconUrl, document.location.href));
					}
				}
				feature.setStyle(doc.getOrNewStyle(iconStyle, null, null, null, null, null));
			}
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
			{
				var c = web.parseCSSColor(layer.options.color);
				if (layer.options.opacity)
				{
					c.a = c.a * layer.options.opacity;
				}
				lineStyle.fromARGB(c.a, c.r, c.g, c.b);
			}
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

export class LeafletMap extends map.MapControl
{
	constructor(divId)
	{
		super();
		this.mapObj = L.map(divId);
	}

	createLayer(layer, options)
	{
		return createLayer(layer, options);
	}

//	createMarkerLayer(name: string, options?: LayerOptions): any;
//	createGeometryLayer(name: string, options?: LayerOptions): any;

	addLayer(layer)
	{
		layer.addTo(this.mapObj);
	}

	addKMLFeature(feature)
	{
		createFromKMLFeature(feature).addTo(this.mapObj);
	}

//	uninit(): void;
	zoomIn()
	{
		this.mapObj.zoomIn();
	}

	zoomOut()
	{
		this.mapObj.zoomOut();
	}

	zoomScale(scale)
	{
		this.mapObj.setZoom(osm.scale2Level(scale));
	}

	panTo(pos)
	{
		this.mapObj.setView(L.latLng(pos.y, pos.x));
	}

	panZoomScale(pos, scale)
	{
		this.mapObj.setView(L.latLng(pos.y, pos.x), osm.scale2Level(scale));
	}

	//zoomToExtent(extent: math.RectArea): void;

/*	handleMouseLClick(clickFunc: (mapPos: math.Coord2D, scnPos: math.Coord2D)=>void): void;
	handleMouseMove(moveFunc: (mapPos: math.Coord2D)=>void): void;
	handlePosChange(posFunc: (mapPos: math.Coord2D)=>void): void;
	map2ScnPos(mapPos: math.Coord2D): math.Coord2D;
	scn2MapPos(scnPos: math.Coord2D): math.Coord2D;

	createMarker(mapPos: math.Coord2D, imgURL: string, imgWidth: number, imgHeight: number, options?: MarkerOptions): any;
	layerAddMarker(markerLayer: any, marker: any): void;
	layerRemoveMarker(markerLayer: any, marker: any): void;
	layerClearMarkers(markerLayer: any): void;
	markerIsOver(marker: any, scnPos: math.Coord2D): boolean;

	createGeometry(geom: geometry.Vector2D, options: GeometryOptions): any;
	layerAddGeometry(geometryLayer: any, geom: any): void;
	layerRemoveGeometry(geometryLayer: any, geom: any): void;
	layerClearGeometries(geometryLayer: any): void;*/
}
