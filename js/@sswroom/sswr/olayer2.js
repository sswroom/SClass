import * as data from "./data.js";
import * as geometry from "./geometry.js";
import * as kml from "./kml.js";
import * as map from "./map.js";
import * as math from "./math.js";
import * as osm from "./osm.js";
import * as text from "./text.js";
import * as web from "./web.js";
import OpenLayers from "./dummy/olayer2.js";

export function toPointArray(numArr, options)
{
	let ret = [];
	let i;
	for (i in numArr)
	{
		let pos = new OpenLayers.LonLat(numArr[i][0], numArr[i][1]).transform(options.objProjection, options.mapProjection);
		ret.push(new OpenLayers.Geometry.Point(pos.lon, pos.lat));
	}
	return ret;
}

export async function createFromKML(feature, options)
{
	if (feature instanceof kml.KMLFile)
	{
		return await createFromKML(feature.root, options);
	}
	options = data.mergeOptions(options, {noPopup: false});
	if (feature instanceof kml.Container)
	{
		let i;
		let layers = [];
		let layer;
		for (i in feature.features)
		{
			layer = await createFromKML(feature.features[i], options);
			if (layer instanceof OpenLayers.Marker)
			{
				layers.push(layer);
			}
			else if (layer instanceof OpenLayers.Feature.Vector)
			{
				layers.push(layer);
			}
			else if (layer != null)
			{
				let j;
				for (j in layer)
				{
					layers.push(layer[j]);
				}
			}
		}
		return layers;
	}
	else if (feature instanceof kml.Placemark)
	{
		/** @type {{objProjection?: OpenLayers.Projection, mapProjection?: OpenLayers.Projection, name?: string, iconUrl?: string, iconOffset?: OpenLayers.Pixel, strokeColor?: string, strokeWidth?: number, stroke?: boolean, fillColor?: string, fill?: boolean}} */
		let opt = {objProjection: options.objProjection, mapProjection: options.mapProjection};
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
					if (s.iconUrl)
					{
						opt.iconUrl = s.iconUrl;
					}
					if (s.hotSpotX && s.hotSpotY)
					{
						opt.iconOffset = new OpenLayers.Pixel(-s.hotSpotX, -s.hotSpotY);
					}
				}
				if (style.lineStyle)
				{
					let ls = style.lineStyle;
					if (ls.color)
						opt.strokeColor = kml.toCSSColor(ls.color);
					if (ls.width)
						opt.strokeWidth = ls.width;
					opt.stroke = true;
				}
				else
				{
					opt.stroke = false;
				}
				if (style.polyStyle)
				{
					let ps = style.polyStyle;
					if (ps.color)
						opt.fillColor = kml.toCSSColor(ps.color);
					opt.fill = true;
				}
				else
				{
					opt.fill = false;
				}		
			}
		}
		let layer = await createFromGeometry(feature.vec, opt);
		if (layer instanceof OpenLayers.Geometry)
		{
			return new OpenLayers.Feature.Vector(layer, {name: feature.name, description: feature.description}, opt);
		}
		else if (layer instanceof OpenLayers.Marker)
		{
			if (!options.noPopup && (feature.name || feature.description))
			{
				let content;
				if (feature.name && feature.description)
				{
					content = "<b>"+text.toHTMLText(feature.name)+"</b><br/>"+feature.description;
				}
				else if (feature.name)
				{
					content = "<b>"+text.toHTMLText(feature.name)+"</b>";
				}
				else
				{
					content = feature.description;
				}
				layer.events.register('click', layer, function(evt) {
					let popup = new OpenLayers.Popup.FramedCloud("Popup",
						layer.lonlat,
						null,
						content,
						null,
						null);
					options.map.addPopup(popup, true);
				});
			}
			return layer;
		}
		return layer;
	}
	else
	{
		console.log("Unknown KML feature type", feature);
		return null;
	}
}

/**
 * @param {geometry.Vector2D} geom
 * @param {{ objProjection?: OpenLayers.Projection; mapProjection?: OpenLayers.Projection; iconUrl?: string; iconOffset?: OpenLayers.Pixel; } | null} options
 * @returns {Promise<OpenLayers.Geometry|OpenLayers.Marker|null>}
 */
export async function createFromGeometry(geom, options)
{
	if (options == null)
	{
		console.log("createFromGeometry does not have options");
		return null;
	}
	if (geom instanceof geometry.Point)
	{
		let icon;
		let lonLat = new OpenLayers.LonLat(geom.coordinates[0], geom.coordinates[1]);
		if (options.objProjection && options.mapProjection)
		{
			lonLat = lonLat.transform(options.objProjection, options.mapProjection);
		}
		if (options.iconUrl && options.iconOffset)
		{
			let size = await web.getImageInfo(options.iconUrl);
			if (size == null)
			{
				console.log("Error in getting image info from url", options.iconUrl);
				return null;
			}
			icon = new OpenLayers.Icon(options.iconUrl, new OpenLayers.Size(size.width, size.height), options.iconOffset);
			return new OpenLayers.Marker(lonLat, icon);
		}
		else
		{
			return new OpenLayers.Geometry.Point(lonLat.lon, lonLat.lat);
		}
	}
	else if (geom instanceof geometry.LinearRing)
	{
		return new OpenLayers.Geometry.LinearRing(toPointArray(geom.coordinates, options));
	}
	else if (geom instanceof geometry.LineString)
	{
		return new OpenLayers.Geometry.LineString(toPointArray(geom.coordinates, options));
	}
	else if (geom instanceof geometry.Polygon)
	{
		let lrList = [];
		let i;
		for (i in geom.geometries)
		{
			let lr = await createFromGeometry(geom.geometries[i], options);
			if (lr instanceof OpenLayers.Geometry.LinearRing)
				lrList.push(lr);
		}
		return new OpenLayers.Geometry.Polygon(lrList);
	}
	else if (geom instanceof geometry.MultiPolygon)
	{
		let pgList = [];
		let i;
		for (i in geom.geometries)
		{
			let pg = await createFromGeometry(geom.geometries[i], options);
			if (pg instanceof OpenLayers.Geometry.Polygon)
				pgList.push(pg);
		}
		return new OpenLayers.Geometry.MultiPolygon(pgList);
	}
	else
	{
		console.log("Unknown geometry type", geom);
		return null;
	}
}

export class Olayer2Map extends map.MapControl
{
	constructor(mapId)
	{
		super();
		this.inited = false;
		this.initX = 114.2;
		this.initY = 22.4;
		this.initLev = 12;
		this.mapId = mapId;
		let dom = document.getElementById(mapId);
		if (dom)
		{
			dom.style.minWidth = '1px';
			dom.style.minHeight = '1px';
		}
		this.mapProjection = new OpenLayers.Projection("EPSG:4326");
		this.map = new OpenLayers.Map(this.mapId, {
			eventListeners: {
				"moveend": ()=>{this.eventMoved();},
				"zoomend": ()=>{this.eventMoved();},
				"mousemove": (e)=>{this.eventMouseMoved(e);}
			},
			controls: [
				new OpenLayers.Control.PanZoomBar(),
				new OpenLayers.Control.Navigation()
			]
		});

		this.Click = OpenLayers.Class(OpenLayers.Control, {
			defaultHandlerOptions: {
				'single': true,
				'double': false,
				'pixelTolerance': 0,
				'stopSingle': false,
				'stopDouble': false
			},

			initialize: function(options) {
				this.handlerOptions = OpenLayers.Util.extend(
					{}, this.defaultHandlerOptions
				);
				OpenLayers.Control.prototype.initialize.apply(
					this, arguments
				);
				this.handler = new OpenLayers.Handler.Click(
					this, {
						'click': this.trigger
					}, this.handlerOptions
				);
			},

			trigger: (e)=>{this.eventLClicked(e);}

		});

		this.mouseCtrl = new this.Click();
		this.map.addControl(this.mouseCtrl);
		this.mouseCtrl.activate();
		this.currMarkerPopup = null;
		this.currMarkerPopupObj = null;
	}

	/**
	 * @param {map.LayerInfo} layer
	 * @param {object} options
	 */
	createLayer(layer, options)
	{
		if (layer.type == map.WebMapType.OSMTile)
		{
			return new OpenLayers.Layer.OSM(layer.name, [layer.url.replace("{x}", "${x}").replace("{y}", "${y}").replace("{z}", "${z}")]);
		}
		else if (layer.type == map.WebMapType.ArcGIS)
		{
			return new OpenLayers.Layer.ArcGIS93Rest(layer.name, [layer.url]);
		}
		else if (layer.type == map.WebMapType.WMS)
		{
			return new OpenLayers.Layer.WMS(layer.name, layer.url, {layers: layer.layers});
		}
		return null;
	}

	createMarkerLayer(name, options)
	{
		let layer = new OpenLayers.Layer.Markers(name);
		return layer;
	}

	createGeometryLayer(name, options)
	{
		let layer = new OpenLayers.Layer.Vector(name);
		return layer;
	}

	addLayer(layer)
	{
		if (layer instanceof OpenLayers.Layer)
		{
			this.map.addLayer(layer);
			if (!this.inited)
			{
				this.inited = true;
				this.map.setCenter(
					new OpenLayers.LonLat(this.initX, this.initY).transform(
						this.mapProjection,
						this.map.getProjectionObject()
					), this.initLev);
			}
		}
		else if (data.isArray(layer))
		{
			let vectorLayer;
			let markerLayer;
			let vectors = [];
			let i;
			for (i in layer)
			{
				if (layer[i] instanceof OpenLayers.Marker)
				{
					if (markerLayer == null)
						markerLayer = new OpenLayers.Layer.Markers("Markers");
					markerLayer.addMarker(layer[i]);
				}
				else if (layer[i] instanceof OpenLayers.Feature.Vector)
				{
					vectors.push(layer[i]);
				}
				else
				{
					console.log("Unknown type on layer array");
				}
			}
			if (vectors.length > 0)
			{
				vectorLayer = new OpenLayers.Layer.Vector("Vectors");
				vectorLayer.addFeatures(vectors);
				this.map.addLayer(vectorLayer);
			}
			if (markerLayer)
				this.map.addLayer(markerLayer);
		}
		else
		{
			console.log("Unknown layer type", layer);
		}
	}

	addKML(feature)
	{
		createFromKML(feature, {map: this.map, objProjection: this.mapProjection, mapProjection: this.map.getProjectionObject()}).then((layer)=>{this.addLayer(layer);});
	}

	uninit()
	{
		this.map.destroy();
	}

	zoomIn()
	{
		this.map.zoomIn();
	}

	zoomOut()
	{
		this.map.zoomOut();
	}

	zoomScale(scale)
	{
		this.map.zoomToScale(scale, false);
	}

	panTo(pos)
	{
		this.map.panTo(new OpenLayers.LonLat(pos.x, pos.y).transform(this.mapProjection, this.map.getProjectionObject()));
	}

	panZoomScale(pos, scale)
	{
		if (this.inited)
		{
			this.map.zoomToScale(scale, false);
			this.map.setCenter(new OpenLayers.LonLat(pos.x, pos.y).transform(this.mapProjection, this.map.getProjectionObject()), this.map.getZoom(), false, false);
		}
		else
		{
			this.initX = pos.x;
			this.initY = pos.y;
			this.initLev = osm.scale2Level(scale);
		}
	}

	zoomToExtent(extent)
	{
		let pos = extent.getCenter();
		let currZoom = this.map.getZoom();
		let tl = this.map2ScnPos(new math.Coord2D(extent.min.x, extent.max.y));
		let br = this.map2ScnPos(new math.Coord2D(extent.max.x, extent.min.y));
		let scnSize = this.map.getSize();
		let ratioX = (br.x - tl.x) / scnSize.w;
		let ratioY = (br.y - tl.y) / scnSize.h;
		if (ratioY > ratioX)
		{
			ratioX = ratioY;
		}
		let maxZoom = this.map.getNumZoomLevels() - 1;
		currZoom += Math.floor(-Math.log(ratioX) / Math.log(2));
		if (currZoom < 0)
			currZoom = 0;
		if (currZoom > maxZoom)
			currZoom = maxZoom;
		this.map.setCenter(new OpenLayers.LonLat(pos.x, pos.y).transform(this.mapProjection, this.map.getProjectionObject()), currZoom, false, false);
	}

	handleMouseLClick(clickFunc)
	{
		this.lclickFunc = clickFunc;
	}

	handleMouseMove(moveFunc)
	{
		this.moveFunc = moveFunc;
	}

	handlePosChange(posFunc)
	{
		this.posFunc = posFunc;
	}

	map2ScnPos(mapPos)
	{
		let scnPx = this.map.getViewPortPxFromLonLat(new OpenLayers.LonLat(mapPos.x, mapPos.y).transform(this.mapProjection, this.map.getProjectionObject()));
		return new math.Coord2D(scnPx.x, scnPx.y);
	}

	scn2MapPos(scnPos)
	{
		let lonLat = this.map.getLonLatFromViewPortPx(new OpenLayers.Pixel(scnPos.x, scnPos.y)).transform(this.map.getProjectionObject(), this.mapProjection);
		return new math.Coord2D(lonLat.lon, lonLat.lat);
	}

	createMarker(mapPos, imgURL, imgWidth, imgHeight, options)
	{
		let size = new OpenLayers.Size(imgWidth, imgHeight);
		let offset = new OpenLayers.Pixel(-(size.w / 2), -(size.h / 2));
		let icon = new OpenLayers.Icon(imgURL, size, offset);
		if (options && options.zIndex)
		{
			icon.imageDiv.style.zIndex = options.zIndex;
		}
		return new OpenLayers.Marker(new OpenLayers.LonLat(mapPos.x, mapPos.y).transform(this.mapProjection, this.map.getProjectionObject()), icon);
	}

	layerAddMarker(markerLayer, marker)
	{
		markerLayer.addMarker(marker);
	}

	layerRemoveMarker(markerLayer, marker)
	{
		markerLayer.removeMarker(marker);
		marker.destroy();
	}

	layerClearMarkers(markerLayer)
	{
		markerLayer.clearMarkers();
	}

	markerIsOver(marker, scnPos)
	{
		let icon = marker.icon;
		//alert("x="+x+", y="+y+", px.x="+icon.px.x+", px.y="+icon.px.y+", offset.x="+icon.offset.x+", offset.y="+icon.offset.y+", size.w="+icon.size.w+", size.h="+icon.size.h+", debug="+(icon.px.x + icon.px.offset.x + icon.size.w));
		if ((scnPos.x < icon.px.x + icon.offset.x) || (scnPos.y < icon.px.y + icon.offset.y))
			return false;
		if ((icon.px.x + icon.offset.x + icon.size.w <= scnPos.x) || (icon.px.y + icon.offset.y + icon.size.h <= scnPos.y))
			return false;
		return true;
	}

	createGeometry(geom, options)
	{
		let opt = {};
		if (options.lineColor)
		{
			opt.strokeColor = options.lineColor;
			opt.stroke = true;
			opt.strokeWidth = options.lineWidth || 1;
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
			return new OpenLayers.Feature.Vector(new OpenLayers.Geometry.Point(geom.coordinates[0], geom.coordinates[1]), null, opt);
		}
		else if (geom instanceof geometry.LinearRing)
		{
			let i;
			let points = [];
			for (i in geom.coordinates)
			{
				let pos = new OpenLayers.LonLat(geom.coordinates[i][0], geom.coordinates[i][1]).transform(this.mapProjection, this.map.getProjectionObject());
				points.push(new OpenLayers.Geometry.Point(pos.lon, pos.lat));
			}
			return new OpenLayers.Feature.Vector(new OpenLayers.Geometry.LinearRing(points), null, opt);
		}
		else if (geom instanceof geometry.LineString)
		{
			let i;
			let points = [];
			for (i in geom.coordinates)
			{
				let pos = new OpenLayers.LonLat(geom.coordinates[i][0], geom.coordinates[i][1]).transform(this.mapProjection, this.map.getProjectionObject());
				points.push(new OpenLayers.Geometry.Point(pos.lon, pos.lat));
			}
			return new OpenLayers.Feature.Vector(new OpenLayers.Geometry.LineString(points), null, opt);
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
					let pos = new OpenLayers.LonLat(lr.coordinates[j][0], lr.coordinates[j][1]).transform(this.mapProjection, this.map.getProjectionObject());
					points.push(new OpenLayers.Geometry.Point(pos.lon, pos.lat));
				}
				lrArr.push(new OpenLayers.Geometry.LinearRing(points));
			}
			return new OpenLayers.Feature.Vector(new OpenLayers.Geometry.Polygon(lrArr), null, opt);
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
						let pos = new OpenLayers.LonLat(lr.coordinates[k][0], lr.coordinates[k][1]).transform(this.mapProjection, this.map.getProjectionObject());
						points.push(new OpenLayers.Geometry.Point(pos.lon, pos.lat));
					}
					lrArr.push(new OpenLayers.Geometry.LinearRing(points));
				}
				pgArr.push(new OpenLayers.Geometry.Polygon(lrArr));
			}
			return new OpenLayers.Feature.Vector(new OpenLayers.Geometry.MultiPolygon(pgArr), null, opt);
		}
		throw new Error("Unknown geometry type");
	}

	layerAddGeometry(geometryLayer, geometry)
	{
		geometryLayer.addFeatures([geometry]);
	}

	layerRemoveGeometry(geometryLayer, geom)
	{
		geometryLayer.removeFeatures([geom]);
	}

	layerClearGeometries(goemetryLayer)
	{
		goemetryLayer.removeAllFeatures();
	}

	toKMLFeature(layer, doc)
	{
		let feature;
		if (doc == null || !(doc instanceof kml.Document))
		{
			doc = new kml.Document();
			feature = this.toKMLFeature(layer, doc);
			if (feature)
			{
				doc.addFeature(feature);
				return doc;
			}
			return null;
		}
		if (layer instanceof OpenLayers.Layer.Markers)
		{
			let featureGroup = new kml.Folder();
			featureGroup.setName(layer.name || "Markers");
			let i;
			for (i in layer.markers)
			{
				feature = this.toKMLFeature(layer.markers[i], doc);
				if (feature)
					featureGroup.addFeature(feature);
			}
			return featureGroup;
		}
		else if (layer instanceof OpenLayers.Marker)
		{
			let pos = layer.lonlat.transform(this.map.getProjectionObject(), this.mapProjection);
			feature = new kml.Placemark(new geometry.Point(4326, [pos.lon, pos.lat]));
			feature.setName("Marker");
			let icon = layer.icon;
			if (icon)
			{
				let iconStyle = new kml.IconStyle();
				if (icon.url.startsWith("/"))
				{
					let durl = document.location.href;
					let i = durl.indexOf("://");
					i = durl.indexOf("/", i + 3);
					if (i >= 0)
					{
						iconStyle.setIconUrl(durl.substring(0, i)+icon.url);
					}
					else
					{
						iconStyle.setIconUrl(durl+icon.url);
					}
				}
				else
				{
					iconStyle.setIconUrl(icon.url);
				}
				iconStyle.setHotSpotX(-icon.offset.x / icon.size.w);
				iconStyle.setHotSpotY(-icon.offset.y / icon.size.h);
				feature.setStyle(doc.getOrNewStyle(iconStyle, null, null, null, null, null));
			}
			return feature;
		}
		else if (layer instanceof OpenLayers.Layer.Vector)
		{
			let featureGroup = new kml.Folder();
			featureGroup.setName(layer.name || "Vector");
			let i;
			for (i in layer.features)
			{
				feature = this.toKMLFeature(layer.features[i], doc);
				if (feature)
					featureGroup.addFeature(feature);
			}
			return featureGroup;
		}
		else if (layer instanceof OpenLayers.Feature.Vector)
		{
			let lineStyle = null;
			let polyStyle = null;
			if (layer.style)
			{
				if (layer.style.stroke && layer.style.strokeColor)
				{
					let c = web.parseCSSColor(layer.style.strokeColor);
					lineStyle = new kml.LineStyle();
					lineStyle.fromARGB(c.a, c.r, c.g, c.b);
					if (layer.style.strokeWidth)
						lineStyle.setWidth(layer.style.strokeWidth)
				}
				if (layer.style.fill && layer.style.fillColor)
				{
					let c = web.parseCSSColor(layer.style.fillColor);
					if (layer.style.fillOpacity)
						c.a = c.a * layer.style.fillOpacity;
					polyStyle = new kml.PolyStyle();
					polyStyle.fromARGB(c.a, c.r, c.g, c.b);
				}
			}
			let vec;
			let geom = layer.geometry;
			if (geom instanceof OpenLayers.Geometry.LinearRing)
			{
				let pts = [];
				let i;
				for (i in geom.components)
				{
					let pt = new OpenLayers.LonLat(geom.components[i].x, geom.components[i].y).transform(this.map.getProjectionObject(), this.mapProjection);
					pts.push([pt.lon, pt.lat]);
				}
				vec = new geometry.LinearRing(4326, pts);
			}
			else
			{
				throw new Error("Unsupported type of geometry");
			}
			let placemark = new kml.Placemark(vec);
			placemark.setStyle(doc.getOrNewStyle(null, null, lineStyle, polyStyle, null, null));
			return placemark;
		}
		else
		{
			console.log("Unknown type", layer);
		}
		return null;
	}

	eventLClicked(e)
	{
		if (this.lclickFunc)
		{
			let lonlat = this.map.getLonLatFromViewPortPx(e.xy).transform(this.map.getProjectionObject(), this.mapProjection);
			this.lclickFunc(lonlat.lat, lonlat.lon, e.xy.x, e.xy.y);
		}
	}

	eventMoved()
	{
		if (this.map == null)
			return;
		if (this.posFunc)
		{
			let cent = this.map.getCenter().transform(this.map.getProjectionObject(), this.mapProjection);
			this.posFunc(new math.Coord2D(cent.lon, cent.lat));
		}
	}

	eventMouseMoved(event)
	{
		if (this.map == null)
			return;
		if (this.moveFunc)
		{
			let pos = this.map.events.getMousePosition(event);
			let cent = this.map.getLonLatFromViewPortPx(pos).transform(this.map.getProjectionObject(), this.mapProjection);
			this.moveFunc(new math.Coord2D(cent.lon, cent.lat));
		}
	}
}
