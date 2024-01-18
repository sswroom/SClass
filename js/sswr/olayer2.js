import * as geometry from "./geometry.js";
import * as kml from "./kml.js";
import * as map from "./map.js";
import * as math from "./math.js";
import * as web from "./web.js";

export class Olayer2Map extends map.MapControl
{
	constructor(mapId)
	{
		super();
		this.inited = false;
		this.mapId = mapId;
		var dom = document.getElementById(mapId);
		dom.style.minWidth = '1px';
		dom.style.minHeight = '1px';
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

	createLayer(layer, options)
	{
		if (layer.type == map.WebMapType.OSMTile)
		{
			return new OpenLayers.Layer.OSM(layer.name, [layer.url]);
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
		var layer = new OpenLayers.Layer.Markers(name);
		return layer;
	}

	createGeometryLayer(name, options)
	{
		var layer = new OpenLayers.Layer.Vector(name);
		return layer;
	}

	addLayer(layer)
	{
		this.map.addLayer(layer);
		if (!this.inited)
		{
			this.inited = true;
			this.map.setCenter(
				new OpenLayers.LonLat(114.2, 22.4).transform(
					this.mapProjection,
					this.map.getProjectionObject()
				), 12);
		}
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
		this.map.zoomToScale(scale, false);
		this.map.setCenter(new OpenLayers.LonLat(pos.x, pos.y).transform(this.mapProjection, this.map.getProjectionObject()), this.map.getZoom(), false, false);
	}

	zoomToExtent(extent)
	{
		var pos = extent.getCenter();
		var currZoom = this.map.getZoom();
		var tl = this.map2ScnPos(new math.Coord2D(extent.min.x, extent.max.y));
		var br = this.map2ScnPos(new math.Coord2D(extent.max.x, extent.min.y));
		var scnSize = this.map.getSize();
		var ratioX = (br.x - tl.x) / scnSize.w;
		var ratioY = (br.y - tl.y) / scnSize.h;
		if (ratioY > ratioX)
		{
			ratioX = ratioY;
		}
		var minZoom = 0;
		var maxZoom = this.map.getNumZoomLevels() - 1;
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
		var scnPx = this.map.getViewPortPxFromLonLat(new OpenLayers.LonLat(mapPos.x, mapPos.y).transform(this.mapProjection, this.map.getProjectionObject()));
		return new math.Coord2D(scnPx.x, scnPx.y);
	}

	scn2MapPos(scnPos)
	{
		var lonLat = this.map.getLonLatFromViewPortPx(new OpenLayers.Pixel(scnPos.x, scnPos.y)).transform(this.map.getProjectionObject(), this.mapProjection);
		return new math.Coord2D(lonLat.lon, lonLat.lat);
	}

	createMarker(mapPos, imgURL, imgWidth, imgHeight, options)
	{
		var size = new OpenLayers.Size(imgWidth, imgHeight);
		var offset = new OpenLayers.Pixel(-(size.w / 2), -(size.h / 2));
		var icon = new OpenLayers.Icon(imgURL, size, offset);
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
		var icon = marker.icon;
		//alert("x="+x+", y="+y+", px.x="+icon.px.x+", px.y="+icon.px.y+", offset.x="+icon.offset.x+", offset.y="+icon.offset.y+", size.w="+icon.size.w+", size.h="+icon.size.h+", debug="+(icon.px.x + icon.px.offset.x + icon.size.w));
		if ((scnPos.x < icon.px.x + icon.offset.x) || (scnPos.y < icon.px.y + icon.offset.y))
			return false;
		if ((icon.px.x + icon.offset.x + icon.size.w <= scnPos.x) || (icon.px.y + icon.offset.y + icon.size.h <= scnPos.y))
			return false;
		return true;
	}

	createGeometry(geom, options)
	{
		var opt = {};
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
			var i;
			var points = [];
			for (i in geom.coordinates)
			{
				var pos = new OpenLayers.LonLat(geom.coordinates[i][0], geom.coordinates[i][1]).transform(this.mapProjection, this.map.getProjectionObject());
				points.push(new OpenLayers.Geometry.Point(pos.lon, pos.lat));
			}
			return new OpenLayers.Feature.Vector(new OpenLayers.Geometry.LinearRing(points), null, opt);
		}
		else if (geom instanceof geometry.LineString)
		{
			var i;
			var points = [];
			for (i in geom.coordinates)
			{
				var pos = new OpenLayers.LonLat(geom.coordinates[i][0], geom.coordinates[i][1]).transform(this.mapProjection, this.map.getProjectionObject());
				points.push(new OpenLayers.Geometry.Point(pos.lon, pos.lat));
			}
			return new OpenLayers.Feature.Vector(new OpenLayers.Geometry.LineString(points), null, opt);
		}
		else if (geom instanceof geometry.Polygon)
		{
			var i;
			var j;
			var lrArr = [];
			for (i in geom.geometries)
			{
				var points = [];
				var lr = geom.geometries[i];
				for (j in lr.coordinates)
				{
					var pos = new OpenLayers.LonLat(lr.coordinates[j][0], lr.coordinates[j][1]).transform(this.mapProjection, this.map.getProjectionObject());
					points.push(new OpenLayers.Geometry.Point(pos.lon, pos.lat));
				}
				lrArr.push(new OpenLayers.Geometry.LineString(points));
			}
			return new OpenLayers.Feature.Vector(new OpenLayers.Geometry.Polygon(lrArr), null, opt);
		}
		else if (geom instanceof geometry.MultiPolygon)
		{
			var i;
			var j;
			var k;
			var pgArr = [];
			for (i in geom.geometries)
			{
				var lrArr = [];
				var pg = geom.geometries[i];
				for (j in pg.geometries)
				{
					var lr = pg.geometries[j];
					var points = [];
					for (k in lr.coordinates)
					{
						var pos = new OpenLayers.LonLat(lr.coordinates[k][0], lr.coordinates[k][1]).transform(this.mapProjection, this.map.getProjectionObject());
						points.push(new OpenLayers.Geometry.Point(pos.lon, pos.lat));
					}
					lrArr.push(new OpenLayers.Geometry.LineString(points));
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
		var feature;
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
			var featureGroup = new kml.Folder();
			featureGroup.setName(layer.name || "Markers");
			var i;
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
			var pos = layer.lonlat.transform(this.map.getProjectionObject(), this.mapProjection);
			feature = new kml.Placemark(new geometry.Point(4326, [pos.lon, pos.lat]));
			feature.setName("Marker");
			var icon = layer.icon;
			if (icon)
			{
				var iconStyle = new kml.IconStyle();
				if (icon.url.startsWith("/"))
				{
					var durl = document.location.href;
					var i = durl.indexOf("://");
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
			var featureGroup = new kml.Folder();
			featureGroup.setName(layer.name || "Vector");
			var i;
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
			var lineStyle = null;
			var polyStyle = null;
			if (layer.style)
			{
				if (layer.style.stroke)
				{
					var c = web.parseCSSColor(layer.style.strokeColor);
					lineStyle = new kml.LineStyle();
					lineStyle.fromARGB(c.a, c.r, c.g, c.b);
					if (layer.style.strokeWidth)
						lineStyle.setWidth(layer.style.strokeWidth)
				}
				if (layer.style.fill)
				{
					var c = web.parseCSSColor(layer.style.fillColor);
					if (layer.style.fillOpacity)
						c.a = c.a * layer.style.fillOpacity;
					polyStyle = new kml.PolyStyle();
					polyStyle.fromARGB(c.a, c.r, c.g, c.b);
				}
			}
			var vec;
			var geom = layer.geometry;
			if (geom instanceof OpenLayers.Geometry.LinearRing)
			{
				var pts = [];
				var i;
				for (i in geom.components)
				{
					var pt = new OpenLayers.LonLat(geom.components[i].x, geom.components[i].y).transform(this.map.getProjectionObject(), this.mapProjection);
					pts.push([pt.lon, pt.lat]);
				}
				vec = new geometry.LinearRing(4326, pts);
			}
			else
			{
				throw new Error("Unsupported type of geometry");
			}
			var placemark = new kml.Placemark(vec);
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
			var lonlat = this.map.getLonLatFromViewPortPx(e.xy).transform(this.map.getProjectionObject(), this.mapProjection);
			this.lclickFunc(lonlat.lat, lonlat.lon, e.xy.x, e.xy.y);
		}
	}

	eventMoved()
	{
		if (this.map == null)
			return;
		if (this.posFunc)
		{
			var cent = this.map.getCenter().transform(this.map.getProjectionObject(), this.mapProjection);
			this.posFunc(new math.Coord2D(cent.lon, cent.lat));
		}
	}

	eventMouseMoved(event)
	{
		if (this.map == null)
			return;
		if (this.moveFunc)
		{
			var pos = this.map.events.getMousePosition(event);
			var cent = this.map.getLonLatFromViewPortPx(pos).transform(this.map.getProjectionObject(), this.mapProjection);
			this.moveFunc(new math.Coord2D(cent.lon, cent.lat));
		}
	}
}
