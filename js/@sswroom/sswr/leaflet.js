import * as data from "./data.js";
import * as geometry from "./geometry.js";
import * as kml from "./kml.js";
import * as math from "./math.js";
import * as map from "./map.js";
import * as osm from "./osm.js";
import * as parser from "./parser.js";
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

export function toLatLngBounds(rect)
{
	return L.latLngBounds(L.latLng(rect.min.y, rect.min.x), L.latLng(rect.max.y, rect.max.x));
}

export function createLayer(layer, options)
{
	let lyrOpts;
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
		if (layer.minX != null && layer.minY != null && layer.maxX != null && layer.maxY != null)
			lyrOpts.bounds = L.latLngBounds(L.latLng(layer.minY, layer.minX), L.latLng(layer.maxY, layer.maxX));
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

export function createFromKML(feature, options)
{
	if (feature instanceof kml.KMLFile)
	{
		return createFromKML(feature.root, options);
	}
	options = data.mergeOptions(options, {noPopup: false});
	if (feature instanceof kml.Container)
	{
		let i;
		let layers = [];
		let lyr;
		let hasGroup = false;
		let hasFeature = false;
		for (i in feature.features)
		{
			lyr = createFromKML(feature.features[i], options);
			layers.push(lyr);
			if ((lyr instanceof L.FeatureGroup) || (lyr instanceof L.LayerGroup))
				hasGroup = true;
			else
				hasFeature = true;
		}
		if (hasGroup)
		{
			return L.layerGroup(layers);
		}
		else
		{
			return L.featureGroup(layers);
		}
	}
	else if (feature instanceof kml.Placemark)
	{
		let opt = {};
		if (feature.name)
			opt.name = feature.name;
		if (feature.style)
		{
			let style = feature.style;
			if (style instanceof kml.StyleMap)
			{
				style = style.normalStyle;
			}
			if (style instanceof kml.Style)
			{
				if (style.iconStyle)
				{
					let s = style.iconStyle;
					if (s.leafletIcon == null)
					{
						let icon = {};
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
					let ls = style.lineStyle;
					if (ls.color)
						opt.lineColor = kml.toCSSColor(ls.color);
					if (ls.width)
						opt.lineWidth = ls.width;
				}
				if (style.polyStyle)
				{
					let ps = style.polyStyle;
					if (ps.color)
						opt.fillColor = kml.toCSSColor(ps.color);
				}
			}
		}
		let layer = createFromGeometry(feature.vec, opt);
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
	else if (feature instanceof kml.NetworkLink)
	{
		return new KMLNetworkLink(feature);
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
		let opt = {};
		if (options)
		{
			if (options.name)
				opt.title = options.name;
			if (options.icon && options.icon.options.iconUrl)
				opt.icon = options.icon;
		}
		return L.marker(L.latLng(geom.coordinates[1], geom.coordinates[0]), opt);
	}
	else if (geom instanceof geometry.LineString)
	{
		let opt = {};
		let i;
		let pts = [];
		if (options.lineColor)
			opt.color = options.lineColor;
		if (options.lineWidth)
			opt.weight = options.lineWidth;
		for (i in geom.coordinates)
		{
			let latLng = L.latLng(geom.coordinates[i][1], geom.coordinates[i][0]);
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
	else if (geom instanceof geometry.Polyline)
	{
		let opt = {};
		let i;
		let j;
		let ptsArr = [];
		if (options.lineColor)
			opt.color = options.lineColor;
		if (options.lineWidth)
			opt.weight = options.lineWidth;
		for (i in geom.geometries)
		{
			let pts = [];
			for (j in geom.geometries[i].coordinates)
			{
				let latLng = L.latLng(geom.geometries[i].coordinates[j][1], geom.geometries[i].coordinates[j][0]);
				if (latLng)
				{
					pts.push(latLng);
				}
				else
				{
					console.log("Error in Polyline", geom.geometries[i].coordinates[j]);
				}
			}
			ptsArr.push(pts);
		}
		return L.polyline(ptsArr, opt);
	}
	else if (geom instanceof geometry.LinearRing)
	{
		let opt = {};
		let k;
		let pts = [];
		if (options.lineColor)
			opt.color = options.lineColor;
		if (options.lineWidth)
			opt.weight = options.lineWidth;
		if (options.fillColor)
		{
			opt.fillColor = options.fillColor;
			opt.fillOpacity = 1.0;
		}
		
		for (k in geom.coordinates)
		{
			let latLng = L.latLng(geom.coordinates[k][1], geom.coordinates[k][0]);
			if (latLng)
			{
				pts.push(latLng);
			}
			else
			{
				console.log("Error in LinearRing", geom.coordinates[k]);
			}
		}
		return L.polygon(pts, opt);		
	}
	else if (geom instanceof geometry.Polygon)
	{
		let opt = {};
		let i;
		let j;
		let pts = [];
		let pts2;
		let lr;
		if (options.lineColor)
			opt.color = options.lineColor;
		if (options.lineWidth)
			opt.weight = options.lineWidth;
		if (options.fillColor)
		{
			opt.fillColor = options.fillColor;
			opt.fillOpacity = 1.0;
		}
		
		for (i in geom.geometries)
		{
			pts2 = [];
			lr = geom.geometries[i];
			for (j in lr.coordinates)
			{
				let latLng = L.latLng(lr.coordinates[j][1], lr.coordinates[j][0]);
				if (latLng)
				{
					pts2.push(latLng);
				}
				else
				{
					console.log("Error in Polygon", lr.coordinates[j]);
				}
			}
			pts.push(pts2);
		}
		return L.polygon(pts, opt);
	}
	else if (geom instanceof geometry.MultiPolygon)
	{
		let opt = {};
		let i;
		let j;
		let k;
		let pts = [];
		let pts2;
		let pts3;
		let pg;
		let lr;
		if (options.lineColor)
			opt.color = options.lineColor;
		if (options.lineWidth)
			opt.weight = options.lineWidth;
		if (options.fillColor)
		{
			opt.fillColor = options.fillColor;
			opt.fillOpacity = 1.0;
		}
		
		for (i in geom.geometries)
		{
			pts2 = [];
			pg = geom.geometries[i];
			for (j in pg.geometries)
			{
				pts3 = [];
				lr = pg.geometries[j];
				for (k in lr.coordinates)
				{
					let latLng = L.latLng(lr.coordinates[k][1], lr.coordinates[k][0]);
					if (latLng)
					{
						pts3.push(latLng);
					}
					else
					{
						console.log("Error in MultiPolygon", lr.coordinates[k]);
					}
				}
				pts2.push(pts3);
			}
			pts.push(pts2);
		}
		return L.polygon(pts, opt);
	}
	else
	{
		console.log("Unknown geometry type", geom);
		return null;
	}
}

export function createKMLLookAt(map)
{
	let center = map.getCenter();
	let zoom = map.getZoom();
	let range = 100000 * Math.pow(2, 11 - zoom);
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
		let coords = [];
		let i;
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
	let feature;
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
		let featureGroup = new kml.Folder();
		featureGroup.setName("");
		let i;
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
		feature.setName("");
//		console.log("Marker found", layer);
		if (layer._popup && layer._popup._content)
		{
			feature.setDescription(layer._popup._content);
		}
		if (layer.options && layer.options.icon)
		{
			let opt = layer.options.icon.options;
			if (opt)
			{
				let iconStyle = new kml.IconStyle();
				if (opt.iconSize && layer._icon)
				{
					let imgW = layer._icon.naturalWidth || layer._icon.offsetWidth;
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
		feature.setName("");
		if (layer.options.color || layer.options.weight)
		{
			let lineStyle = new kml.LineStyle();
			if (layer.options.color)
			{
				let c = web.parseCSSColor(layer.options.color);
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
	let feature = toKMLFeature(layer);
	if (feature)
	{
		return kml.toString(feature);
	}
	return null;
}

export class KMLNetworkLink
{
	constructor(feature)
	{
		if (feature instanceof kml.NetworkLink)
		{
			this.feature = feature;
			this.container = L.featureGroup();
			this.reload();
		}
		else
		{
			throw new Error("Feature is not NetworkLink");
		}
	}

	reload()
	{
		fetch(this.feature.networkLink).then((resp)=>{
			console.log(resp);
			if (resp.ok)
			{
				parser.parseFile(resp).then((val)=>{
					console.log(val);
					if (val instanceof kml.Feature)
					{
						this.container.clearLayers();
						let layer = createFromKML(val);
						if (layer)
							layer.addTo(this.container);
					}
				})
			}
		})
	}

	addTo(container)
	{
		this.container.addTo(container);
	}
}

export class LeafletMap extends map.MapControl
{
	/**
	 * @param {L.LeafletMouseEvent} evt
	 */
	onMouseMove(evt)
	{
		if (this.moveFunc)
		{
			this.moveFunc(fromLatLng(evt.latlng));
		}
	}

	/**
	 * @param {string | HTMLElement} divId
	 */
	constructor(divId)
	{
		super();
		this.mapObj = L.map(divId);
		this.moveFunc = null;
		this.popupMarker = null;
		let self = this;
		this.mapObj.on("mousemove", (evt)=> {self.onMouseMove(evt);});
	}

	// @ts-ignore
	getDiv()
	{
		return this.mapObj.getContainer();
	}

	sizeUpdated()
	{
		this.mapObj.invalidateSize();
	}

	createLayer(layer, options)
	{
		return createLayer(layer, options);
	}

	createMarkerLayer(name, options)
	{
		return L.layerGroup();
	}

	createGeometryLayer(name, options)
	{
		return L.layerGroup();
	}

	addLayer(layer)
	{
		layer.addTo(this.mapObj);
	}

	addKML(feature)
	{
		createFromKML(feature).addTo(this.mapObj);
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

	/**
	 * @param {math.Coord2D} pos
	 */
	panTo(pos)
	{
		this.mapObj.setView(L.latLng(pos.y, pos.x));
	}

	/**
	 * @param {math.Coord2D} pos
	 * @param {number} scale
	 */
	panZoomScale(pos, scale)
	{
		this.mapObj.setView(L.latLng(pos.y, pos.x), osm.scale2Level(scale));
	}

	zoomToExtent(extent)
	{
		this.mapObj.fitBounds(toLatLngBounds(extent));
	}

/*	handleMouseLClick(clickFunc: (mapPos: math.Coord2D, scnPos: math.Coord2D)=>void): void;*/
	/**
	 * @param {(mapPos: math.Coord2D)=>void} moveFunc
	 */
	handleMouseMove(moveFunc)
	{
		this.moveFunc = moveFunc;
	}
/*	handlePosChange(posFunc: (mapPos: math.Coord2D)=>void): void;*/
	/**
	 * @param {math.Coord2D} mapPos
	 */
	map2ScnPos(mapPos)
	{
		let pt = this.mapObj.latLngToContainerPoint(L.latLng(mapPos.lat, mapPos.lon));
		return new math.Coord2D(pt.x, pt.y);
	}

	scn2MapPos(scnPos)
	{
		let latlng = this.mapObj.containerPointToLatLng(L.point(scnPos.x, scnPos.y));
		return new math.Coord2D(latlng.lng, latlng.lat);
	}

	/**
	 * @param {math.Coord2D} mapPos
	 * @param {string} imgURL
	 * @param {number} imgWidth
	 * @param {number} imgHeight
	 * @param {map.MarkerOptions} options
	 */
	createMarker(mapPos, imgURL, imgWidth, imgHeight, options)
	{
		let icon = L.icon({iconUrl: imgURL, iconSize: [imgWidth, imgHeight]});
		let opt = {icon: icon};
		if (options.zIndex)
		{
			opt.zIndexOffset = options.zIndex;
		}
		return L.marker([mapPos.lat, mapPos.lon], opt);
	}

	layerAddMarker(markerLayer, marker)
	{
		marker.addTo(markerLayer);
	}

	layerRemoveMarker(markerLayer, marker)
	{
		markerLayer.removeLayer(marker);
	}

	layerClearMarkers(markerLayer)
	{
		markerLayer.clearLayers();
	}

	layerMoveMarker(markerLayer, marker, mapPos)
	{
		marker.setLatLng(L.latLng(mapPos.lat, mapPos.lon));
		return marker;
	}

	markerUpdateIcon(markerLayer, marker, url)
	{
		let oldIcon = marker.getIcon();
		marker.setIcon(L.icon({iconUrl: url, iconSize: oldIcon.options.iconSize}));
		return marker;
	}

	markerIsOver(marker, scnPos)
	{
		let icon = marker.getIcon();
		let latlng = marker.getLatLng();
		let sz = icon.options.iconSize;
		let iconPos = this.mapObj.latLngToContainerPoint(latlng);
		if (sz == null)
			return false;
		if ((scnPos.x < iconPos.x - sz[0] * 0.5) || (scnPos.y < iconPos.y - sz[1] * 0.5))
			return false;
		if ((iconPos.x + sz[0] * 0.5 <= scnPos.x) || (iconPos.y + sz[1] * 0.5 <= scnPos.y))
			return false;
		return true;
	}

	markerShowPopup(marker, content, w, h)
	{
		let opt = {closeButton: false, closeOnEscapeKey: false};
		if (w && h)
		{
			opt.minWidth = w;
			opt.maxHeight = h;
		}
		marker.bindPopup(content, opt).openPopup();
		this.popupMarker = marker;
	}
	
	hidePopup()
	{
		if (this.popupMarker)
		{
			this.popupMarker.closePopup();
			this.popupMarker = null;
		}
	}

	/**
	 * @param {geometry.Vector2D} geom
	 * @param {map.GeometryOptions} options
	 */
	createGeometry(geom, options)
	{
		let opt = {};
		if (options.lineColor)
		{
			opt.color = options.lineColor;
			opt.stroke = true;
			opt.weight = options.lineWidth || 1;
		}
		else
		{
			opt.stroke = false;
		}
		if (options.fillColor)
		{
			opt.fill = true;
			opt.fillColor = options.fillColor;
			opt.fillOpacity = options.fillOpacity;
		}
		else
		{
			opt.fill = false;
		}
		if (geom instanceof geometry.Point)
		{
//			return new OpenLayers.Feature.Vector(new OpenLayers.Geometry.Point(geom.coordinates[0], geom.coordinates[1]), null, opt);
		}
		else if (geom instanceof geometry.LinearRing)
		{
			let i;
			let points = [];
			for (i in geom.coordinates)
			{
				points.push([geom.coordinates[i][1], geom.coordinates[i][0]]);
			}
			return L.polygon(points, opt);
		}
		else if (geom instanceof geometry.LineString)
		{
			let i;
			let points = [];
			for (i in geom.coordinates)
			{
				points.push([geom.coordinates[i][1], geom.coordinates[i][0]]);
			}
			return L.polyline(points, opt);
		}
		else if (geom instanceof geometry.Polygon)
		{
			let i;
			let j;
			let lrArr = [];
			for (i in geom.geometries)
			{
				let points = [];
				let lr = geom.geometries[i];
				for (j in lr.coordinates)
				{
					points.push([lr.coordinates[j][1], lr.coordinates[j][0]]);
				}
				lrArr.push(points);
			}
			return L.polygon(lrArr, opt);
		}
		else if (geom instanceof geometry.MultiPolygon)
		{
			let i;
			let j;
			let k;
			let pgArr = [];
			for (i in geom.geometries)
			{
				let lrArr = [];
				let pg = geom.geometries[i];
				for (j in pg.geometries)
				{
					let lr = pg.geometries[j];
					let points = [];
					for (k in lr.coordinates)
					{
						points.push([lr.coordinates[k][1], lr.coordinates[k][0]]);
					}
					lrArr.push(points);
				}
				pgArr.push(lrArr);
			}
			return L.polygon(pgArr, opt);
		}
		throw new Error("Unknown geometry type");
	}

	layerAddGeometry(geometryLayer, geom)
	{
		geom.addTo(geometryLayer);
	}

	layerRemoveGeometry(geometryLayer, geom)
	{
		geometryLayer.removeLayer(geom);
	}

	layerClearGeometries(geometryLayer)
	{
		geometryLayer.clearLayers();
	}
}
