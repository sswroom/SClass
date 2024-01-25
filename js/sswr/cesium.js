import * as data from "./data.js";
import * as kml from "./kml.js";
import * as map from "./map.js";
import * as math from "./math.js";
import * as geometry from "./geometry.js";

export function screenToLatLon(viewer, x, y, ellipsoid)
{
	var pos = new Cesium.Cartesian2(x, y);
	if (ellipsoid == null)
		ellipsoid = viewer.scene.globe.ellipsoid;
	var cartesian = viewer.camera.pickEllipsoid(pos, ellipsoid);
	if (cartesian)
	{
		var cartographic = ellipsoid.cartesianToCartographic(cartesian);
		return new math.Coord2D(cartographic.longitude * 180 / Math.PI, cartographic.latitude * 180 / Math.PI);
	}
	else
	{
		return new math.Coord2D(0, 0);
	}
}
	
export function fromCXYZArray(arr)
{
	var ret = new Array();
	var i = 0;
	var j = arr.length;
	while (i < j)
	{
		ret.push(Cesium.Cartesian3.fromArray(arr[i]));
		i++;
	}
	return ret;
}
	
export function toCartesian3Arr(coords)
{
	var arr = new Array();
	var i = 0;
	var j = coords.length;
	if (coords[0].length == 3)
	{
		while (i < j)
		{
			arr.push(Cesium.Cartesian3.fromDegrees(coords[i][0], coords[i][1], coords[i][2]));
			i++;
		}
	}
	else
	{
		while (i < j)
		{
			arr.push(Cesium.Cartesian3.fromDegrees(coords[i][0], coords[i][1]));
			i++;
		}
	}
	return arr;
}
	
export function newObjFromGeoJSON(geoJSON)
{
	var o = new Object();
	o.id = geoJSON.id;
	o.name = geoJSON.id;
	var n;
	var props = new Array();
	for (n in geoJSON.properties)
	{
		props.push(text.toHTMLText(n)+": "+text.toHTMLText(geoJSON.properties[n]));
	}
	o.description = props.join("<br/>");
	return o;
}

export function addGeoJSON(viewer, geoJSON, color, extSize)
{
	var oColor = color.darken(0.5, new Cesium.Color());
	if (geoJSON.type == "FeatureCollection")
	{
		var i = 0;
		var j = geoJSON.features.length;
		while (i < j)
		{
			addGeoJSON(viewer, geoJSON.features[i], color, extSize);			
			i++;
		}
	}
	else if (geoJSON.type == "Feature")
	{
		var o;
		if (geoJSON.geometry != null)
		{
			if (geoJSON.geometry.type == "Polygon")
			{
				var coordinates = geoJSON.geometry.coordinates;
				var i = 0;
				var j = coordinates.length;
				while (i < j)
				{
					o = newObjFromGeoJSON(geoJSON);
					o.id = o.id + "_" + i;
					o.polygon = new Object();
					o.polygon.hierarchy = toCartesian3Arr(coordinates[i]);
					o.polygon.height = -extSize;
					o.polygon.heightReference = Cesium.HeightReference.RELATIVE_TO_GROUND;
					o.polygon.extrudedHeight = 10 + extSize;
					o.polygon.extrudedHeightReference = Cesium.HeightReference.RELATIVE_TO_GROUND;
					o.polygon.material = color;
					o.polygon.outline = true;
					o.polygon.outlineColor = oColor;
					o.polygon.closeTop = false;
					o.polygon.closeBottom = false;
					viewer.entities.add(o);
					i++;
				}
			}
		}
	}
}

export function fromCartesian3Array(viewer, arr)
{
	var coordinates = new Array();
	var points;
	var ellipsoid = viewer.scene.globe.ellipsoid;
	var cartoArr = ellipsoid.cartesianArrayToCartographicArray(arr);
	var i = 0;
	var j = cartoArr.length;
	while (i < j)
	{
		points = new Array();
		points.push(cartoArr[i].longitude * 180 / Math.PI);
		points.push(cartoArr[i].latitude * 180 / Math.PI);
		points.push(cartoArr[i].height);
		coordinates.push(points);
		i++;
	}
	return coordinates;
}
	
export function fromPolygonGraphics(viewer, pg)
{
	var coordinates = new Array();
	var hierarchy = pg.hierarchy.getValue();
	coordinates.push(fromCartesian3Array(viewer, hierarchy.positions));
	var i = 0;
	var j =hierarchy.holes.length;
	while (i < j)
	{
		coordinates.push(fromCartesian3Array(viewer, hierarchy.holes[i].positions));
		i++;
	}
	return new geometry.Polygon(4326, coordinates);
}

export function createFromKMLFeature(feature, options)
{
	options = data.mergeOptions(options, {noPopup: false});
	if (feature instanceof kml.Container)
	{
		var i;
		var layers = [];
		var layer;
		for (i in feature.features)
		{
			layer = createFromKMLFeature(feature.features[i], options);
			if (layer instanceof Cesium.Entity)
			{
				layers.push(layer);
			}
			else if (layer != null)
			{
				var j;
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
					if (s.iconUrl)
					{
						opt.iconUrl = s.iconUrl;
					}
					if (s.hotSpotX && s.hotSpotY)
					{
						if (s.hotSpotX == 0)
							opt.horizontalOrigin = Cesium.HorizontalOrigin.LEFT;
						else if (s.hotSpotUnitX == kml.HotSpotUnit.Fraction && s.hotSpotX == 1)
							opt.horizontalOrigin = Cesium.HorizontalOrigin.RIGHT;
						if (s.hotSpotY == 0)
							opt.verticalOrigin = Cesium.VerticalOrigin.TOP;
						else if (s.hotSpotUnitY == kml.HotSpotUnit.Fraction && s.hotSpotY == 1)
							opt.verticalOrigin = Cesium.VerticalOrigin.BOTTOM;
						else
						{
							if (s.hotSpotX * 2 == s.hotSpotY)
							{
								opt.verticalOrigin = Cesium.VerticalOrigin.CENTER;
							}
						}
					}
				}
				if (style.lineStyle)
				{
					var ls = style.lineStyle;
					if (ls.color)
						opt.lineColor = ls.color;
					if (ls.width)
						opt.lineWidth = ls.width;
				}
				if (style.polyStyle)
				{
					var ps = style.polyStyle;
					if (ps.color)
						opt.fillColor = ps.color;
				}
			}
		}
		var layer = createFromGeometry(feature.vec, opt);
		if (layer)
		{
			if (feature.name)
				layer.name = feature.name;
			if (feature.description)
				layer.description = feature.description;
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
			if (options.iconUrl)
				opt.image = options.iconUrl;
			if (options.horizontalOrigin)
				opt.horizontalOrigin = options.horizontalOrigin;
			if (options.verticalOrigin)
				opt.verticalOrigin = options.verticalOrigin;
		}
		return new Cesium.Entity({
			position: Cesium.Cartesian3.fromDegrees(geom.coordinates[0], geom.coordinates[1]),
			billboard: new Cesium.BillboardGraphics(opt)});
	}
	else if (geom instanceof geometry.LineString)
	{
		var opt = {};
		if (options.lineColor)
		{
			var c = kml.toColor(options.lineColor);
			opt.material = new Cesium.Color(c.r, c.g, c.b, c.a);
		}
		if (options.lineWidth)
			opt.width = options.lineWidth;
		opt.positions = toCartesian3Arr(geom.coordinates);
		return new Cesium.Entity({polyline: new Cesium.PolylineGraphics(opt)});
	}
	else if (geom instanceof geometry.Polygon)
	{
		var opt = {};
		opt.heightReference = Cesium.HeightReference.CLAMP_TO_GROUND;
		opt.height = 0;
		if (options.lineColor)
		{
			var c = kml.toColor(options.lineColor);
			opt.outlineColor = new Cesium.Color(c.r, c.g, c.b, c.a);
			opt.outline = true;
		}
		if (options.lineWidth)
		{
			opt.outlineWidth = options.lineWidth;
			opt.outline = true;
		}
		if (options.fillColor)
		{
			var c = kml.toColor(options.fillColor);
			opt.material = new Cesium.Color(c.r, c.g, c.b, c.a);
		}
		opt.hierarchy = {positions: toCartesian3Arr(geom.geometries[0].coordinates), holes: []};
		var i = 1;
		var j = geom.geometries.length;
		while (i < j)
		{
			opt.hierarchy.holes.push({positions: toCartesian3Arr(geom.geometries[i].coordinates)});
			i++;
		}
		return new Cesium.Entity({polygon: new Cesium.PolygonGraphics(opt)});
	}
	else if (geom instanceof geometry.MultiPolygon)
	{
		if (geom.geometries.length == 1)
		{
			return createFromGeometry(geom.geometries[0], options);
		}
		var opt = {};
		if (options.lineColor)
		{
			var c = kml.toColor(options.lineColor);
			opt.outlineColor = new Cesium.Color(c.r, c.g, c.b, c.a);
			opt.outline = true;
		}
		if (options.lineWidth)
		{
			opt.outlineWidth = options.lineWidth;
			opt.outline = true;
		}
		if (options.fillColor)
		{
			var c = kml.toColor(options.fillColor);
			opt.material = new Cesium.Color(c.r, c.g, c.b, c.a);
		}
		console.log("MultiPolygon not supported", geom);
		return null;
	}
	else
	{
		console.log("Unknown geometry type", geom);
		return null;
	}
}

/*export function createPolygon(viewer, lats, lons, height)
{
	if (lats.length != lons.length)
	{
		return null;
	}
	if (height == null)
	{
		height = 0;
	}
	var ellipsoid = viewer.scene.globe.ellipsoid;
	var o = new Object();
	o.positions = new Array();
	var i = 0;
	var j = lats.length;
	while (i < j)
	{
		o.positions.push(ellipsoid.cartographicToCartesian(new Cesium.Cartographic(lons[i] * Math.PI / 180, lats[i] * Math.PI / 180, height)));
		i++;
	}
	o.positions.push(ellipsoid.cartographicToCartesian(new Cesium.Cartographic(lons[0] * Math.PI / 180, lats[0] * Math.PI / 180, height)));
	
	var pg = new Cesium.PolygonGraphics(o);
	return pg;
}*/

export class CesiumMap extends map.MapControl
{
	constructor(divId)
	{
		super();
		this.viewer = new Cesium.Viewer(divId, {
			timeline:false,
			animation:false,
			scene3DOnly:true,
			baseLayerPicker: false,
//			baseLayer: new Cesium.ImageryLayer(new Cesium.OpenStreetMapImageryProvider({
//				url: "https://tile.openstreetmap.org/"
//			  }))}
		});
	};

	createLayer(layer, options)
	{
		if (layer.type == map.WebMapType.OSMTile)
		{
			var opt = {};
			if (layer.maxZoom)
			{
				opt.maximumLevel = layer.maxZoom;
			}
			opt.url = layer.url;
			return new Cesium.ImageryLayer(new Cesium.UrlTemplateImageryProvider(opt));
		}
	}

	/*createMarkerLayer(name: string, options?: LayerOptions): any;
	createGeometryLayer(name: string, options?: LayerOptions): any;*/
	addLayer(layer)
	{
		if (layer instanceof Cesium.ImageryLayer)
		{
			this.viewer.imageryLayers.add(layer);
		}
		else if (layer instanceof Cesium.Entity)
		{
			this.viewer.entities.add(layer);
		}
		else if (data.isArray(layer))
		{
			var i;
			for (i in layer)
			{
				this.addLayer(layer[i]);
			}
		}
		else
		{
			console.log("Unknown type to add", layer);
		}
	}

	addKMLFeature(feature)
	{
		this.addLayer(createFromKMLFeature(feature));
	}

//	uninit(): void;
	zoomIn()
	{
		this.viewer.camera.moveForward();
	}

	zoomOut()
	{
		this.viewer.camera.moveBackward();
	}

	zoomScale(scale)
	{
		this.viewer.camera.flyTo({destination: Cesium.Cartesian3.fromDegrees(pos.x, pos.y, scale), duration:0});
	}

	panTo(pos)
	{
		this.viewer.camera.flyTo({destination: Cesium.Cartesian3.fromDegrees(pos.x, pos.y, 20000), duration:0});
	}

	panZoomScale(pos, scale)
	{
		this.viewer.camera.flyTo({destination: Cesium.Cartesian3.fromDegrees(pos.x, pos.y, scale), duration:0});
	}
/*	zoomToExtent(extent: math.RectArea): void;
	handleMouseLClick(clickFunc: (mapPos: math.Coord2D, scnPos: math.Coord2D)=>void): void;
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
