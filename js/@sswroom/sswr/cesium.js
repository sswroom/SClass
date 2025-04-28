import * as data from "./data.js";
import * as geometry from "./geometry.js";
import * as kml from "./kml.js";
import * as map from "./map.js";
import * as math from "./math.js";
import * as text from "./text.js";
import * as web from "./web.js";

/**
 * @param {{ scene: { globe: { ellipsoid: any; }; }; camera: { pickEllipsoid: (arg0: any, arg1: any) => any; }; }} viewer
 * @param {number} x
 * @param {number} y
 * @param {{ cartesianToCartographic: (arg0: any) => any; } | null} ellipsoid
 */
export function screenToLatLon(viewer, x, y, ellipsoid)
{
	let pos = new Cesium.Cartesian2(x, y);
	if (ellipsoid == null)
		ellipsoid = viewer.scene.globe.ellipsoid;
	let cartesian = viewer.camera.pickEllipsoid(pos, ellipsoid);
	if (cartesian)
	{
		let cartographic = ellipsoid.cartesianToCartographic(cartesian);
		return new math.Coord2D(cartographic.longitude * 180 / Math.PI, cartographic.latitude * 180 / Math.PI);
	}
	else
	{
		return new math.Coord2D(0, 0);
	}
}
	
export function fromCXYZArray(arr)
{
	let ret = new Array();
	let i = 0;
	let j = arr.length;
	while (i < j)
	{
		ret.push(Cesium.Cartesian3.fromArray(arr[i]));
		i++;
	}
	return ret;
}
	
export function toCartesian3Arr(coords)
{
	let arr = new Array();
	let i = 0;
	let j = coords.length;
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
	let o = new Object();
	o.id = geoJSON.id;
	o.name = geoJSON.id;
	let n;
	let props = new Array();
	for (n in geoJSON.properties)
	{
		props.push(text.toHTMLText(n)+": "+text.toHTMLText(geoJSON.properties[n]));
	}
	o.description = props.join("<br/>");
	return o;
}

export function addGeoJSON(viewer, geoJSON, color, extSize)
{
	let oColor = color.darken(0.5, new Cesium.Color());
	if (geoJSON.type == "FeatureCollection")
	{
		let i = 0;
		let j = geoJSON.features.length;
		while (i < j)
		{
			addGeoJSON(viewer, geoJSON.features[i], color, extSize);			
			i++;
		}
	}
	else if (geoJSON.type == "Feature")
	{
		let o;
		if (geoJSON.geometry != null)
		{
			if (geoJSON.geometry.type == "Polygon")
			{
				let coordinates = geoJSON.geometry.coordinates;
				let i = 0;
				let j = coordinates.length;
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
	let coordinates = new Array();
	let points;
	let ellipsoid = viewer.scene.globe.ellipsoid;
	let cartoArr = ellipsoid.cartesianArrayToCartographicArray(arr);
	let i = 0;
	let j = cartoArr.length;
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
	let coordinates = new Array();
	let hierarchy = pg.hierarchy.getValue();
	coordinates.push(fromCartesian3Array(viewer, hierarchy.positions));
	let i = 0;
	let j =hierarchy.holes.length;
	while (i < j)
	{
		coordinates.push(fromCartesian3Array(viewer, hierarchy.holes[i].positions));
		i++;
	}
	return new geometry.Polygon(4326, coordinates);
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
		let layer;
		for (i in feature.features)
		{
			layer = createFromKML(feature.features[i], options);
			if (layer instanceof Cesium.Entity)
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
					let ls = style.lineStyle;
					if (ls.color)
						opt.lineColor = ls.color;
					if (ls.width)
						opt.lineWidth = ls.width;
				}
				if (style.polyStyle)
				{
					let ps = style.polyStyle;
					if (ps.color)
						opt.fillColor = ps.color;
				}
			}
		}
		let layer = createFromGeometry(feature.vec, opt);
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

/**
 * @param {geometry.Vector2D} geom
 * @param {map.GeometryOptions} options
 */
export function createFromGeometry(geom, options)
{
	if (geom instanceof geometry.Point)
	{
		let opt = {};
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
	else if (geom instanceof geometry.LinearRing)
	{
		let opt = {};
		opt.heightReference = Cesium.HeightReference.CLAMP_TO_GROUND;
		opt.height = 0;
		if (options.lineColor)
		{
			let c = parseColor(options.lineColor);
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
			let c = parseColor(options.fillColor);
			if (options.fillOpacity)
			{
				c.a = options.fillOpacity;
			}
			opt.material = new Cesium.Color(c.r, c.g, c.b, c.a);
		}
		opt.hierarchy = {positions: toCartesian3Arr(geom.coordinates), holes: []};
		return new Cesium.Entity({polygon: new Cesium.PolygonGraphics(opt)});
	}
	else if (geom instanceof geometry.LineString)
	{
		let opt = {};
		if (options.lineColor)
		{
			let c = parseColor(options.lineColor);
			opt.material = new Cesium.Color(c.r, c.g, c.b, c.a);
		}
		if (options.lineWidth)
			opt.width = options.lineWidth;
		opt.positions = toCartesian3Arr(geom.coordinates);
		return new Cesium.Entity({polyline: new Cesium.PolylineGraphics(opt)});
	}
	else if (geom instanceof geometry.Polygon)
	{
		let opt = {};
		opt.heightReference = Cesium.HeightReference.CLAMP_TO_GROUND;
		opt.height = 0;
		if (options.lineColor)
		{
			let c = parseColor(options.lineColor);
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
			let c = parseColor(options.fillColor);
			if (options.fillOpacity)
			{
				c.a = options.fillOpacity;
			}
			opt.material = new Cesium.Color(c.r, c.g, c.b, c.a);
		}
		opt.hierarchy = {positions: toCartesian3Arr(geom.geometries[0].coordinates), holes: []};
		let i = 1;
		let j = geom.geometries.length;
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
		let opt = {};
		if (options.lineColor)
		{
			let c = parseColor(options.lineColor);
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
			let c = parseColor(options.fillColor);
			if (options.fillOpacity)
			{
				c.a = options.fillOpacity;
			}
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

/**
 * @param {string} c
 */
export function parseColor(c)
{
	let col = kml.toColor(c);
	if (col == null)
	{
		col = web.parseCSSColor(c);
	}
	return col;
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
	let ellipsoid = viewer.scene.globe.ellipsoid;
	let o = new Object();
	o.positions = new Array();
	let i = 0;
	let j = lats.length;
	while (i < j)
	{
		o.positions.push(ellipsoid.cartographicToCartesian(new Cesium.Cartographic(lons[i] * Math.PI / 180, lats[i] * Math.PI / 180, height)));
		i++;
	}
	o.positions.push(ellipsoid.cartographicToCartesian(new Cesium.Cartographic(lons[0] * Math.PI / 180, lats[0] * Math.PI / 180, height)));
	
	let pg = new Cesium.PolygonGraphics(o);
	return pg;
}*/

export class CesiumMap extends map.MapControl
{
	constructor(divId)
	{
		super();
		this.mouseMove = null;
		this.viewer = new Cesium.Viewer(divId, {
			timeline:false,
			animation:false,
			scene3DOnly:true,
			baseLayerPicker: false,
//			baseLayer: new Cesium.ImageryLayer(new Cesium.OpenStreetMapImageryProvider({
//				url: "https://tile.openstreetmap.org/"
//			  }))}
		});
		this.viewer.screenSpaceEventHandler.setInputAction((movement) => {
			let pos = movement.startPosition;
			let container = this.viewer.container;
			if (pos.x >= 0 && pos.y >= 0 && pos.x < container.offsetWidth && pos.y < container.offsetHeight)
			{
				if (this.mouseMove)
				{
					this.mouseMove(this.scn2MapPos(new math.Coord2D(pos.x, pos.y)));
				}
			}
		}, Cesium.ScreenSpaceEventType.MOUSE_MOVE);
	};

	getDiv()
	{
		return this.viewer.container;
	}

	sizeUpdated()
	{

	}

	createLayer(layer, options)
	{
		if (layer.type == map.WebMapType.OSMTile)
		{
			let opt = {};
			if (layer.maxZoom)
			{
				opt.maximumLevel = layer.maxZoom;
			}
			opt.url = layer.url;
			return new Cesium.ImageryLayer(new Cesium.UrlTemplateImageryProvider(opt));
		}
	}

	createMarkerLayer(name, options)
	{
		return [];
	}

	createGeometryLayer(name, options)
	{
		return [];
	}

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
			let i;
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

	addKML(feature)
	{
		this.addLayer(createFromKML(feature));
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

	zoomToExtent(extent)
	{
		const zoomRatio = 500000000;
		let center = extent.getCenter();
		let xScale = (extent.max.x - extent.min.x) / this.viewer.canvas.width * zoomRatio;
		let yScale = (extent.max.y - extent.min.y) / this.viewer.canvas.height * zoomRatio;
		if (yScale > xScale)
		{
			xScale = yScale;
		}
		this.viewer.camera.flyTo({destination: Cesium.Cartesian3.fromDegrees(center.x, center.y, xScale), duration:0});
	}

/*	handleMouseLClick(clickFunc: (mapPos: math.Coord2D, scnPos: math.Coord2D)=>void): void;*/
	/**
	 * @param {(mapPos: math.Coord2D)=>void} moveFunc
	 */
	handleMouseMove(moveFunc)
	{
		this.mouseMove = moveFunc;
	}
/*	handlePosChange(posFunc: (mapPos: math.Coord2D)=>void): void;*/
	/**
	 * @param {math.Coord2D} mapPos
	 */
	map2ScnPos(mapPos)
	{
		let pos = Cesium.SceneTransforms.worldToDrawingBufferCoordinates(this.viewer.scene, Cesium.Cartesian3.fromDegrees(mapPos.x, mapPos.y));
		if (pos)
		{
			return new math.Coord2D(pos.x, pos.y);
		}
		else
		{
			return new math.Coord2D(0, 0);
		}
	}

	/**
	 * @param {math.Coord2D} scnPos
	 */
	scn2MapPos(scnPos)
	{
		return screenToLatLon(this.viewer, scnPos.x, scnPos.y, null);
	}

	createMarker(mapPos, imgURL, imgWidth, imgHeight, options)
	{
		return new Cesium.Entity({position: Cesium.Cartesian3.fromDegrees(mapPos.x, mapPos.y), billboard: {image: imgURL, width: imgWidth, height: imgHeight}});
	}

	layerAddMarker(markerLayer, marker)
	{
		markerLayer.push(marker);
		this.viewer.entities.add(marker);
	}

	layerRemoveMarker(markerLayer, marker)
	{
		markerLayer.remove(marker);
		this.viewer.entities.remove(marker);
	}

	layerClearMarkers(markerLayer)
	{
		let geom;
		while (geom = markerLayer.pop())
		{
			this.viewer.entities.remove(geom);
		}
	}

	layerMoveMarker(markerLayer, marker, mapPos)
	{
		marker.position = Cesium.Cartesian3.fromDegrees(mapPos.x, mapPos.y);
		return marker;
	}

	markerUpdateIcon(markerLayer, marker, url)
	{
		marker.billboard.image = url;
		return marker;
	}
	
	markerIsOver(marker, scnPos)
	{
		let latlng = marker.position.getValue();
		let sz = [marker.billboard.width, marker.billboard.height];
		let iconPos = Cesium.SceneTransforms.worldToDrawingBufferCoordinates(this.viewer.scene, latlng);
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
		marker.description = {getValue: ()=>{return content;}};
		this.viewer.selectedEntity = marker;
	}

	hidePopup()
	{
		this.viewer.selectedEntity = null;
	}

	/**
	 * @param {geometry.Vector2D} geom
	 * @param {map.GeometryOptions} options
	 */
	createGeometry(geom, options)
	{
		return createFromGeometry(geom, options);
	}

	layerAddGeometry(geometryLayer, geom)
	{
		geometryLayer.push(geom);
		this.viewer.entities.add(geom);
	}

	layerRemoveGeometry(geometryLayer, geom)
	{
		geometryLayer.remove(geom);
		this.viewer.entities.remove(geom);
	}

	layerClearGeometries(geometryLayer)
	{
		let geom;
		while (geom = geometryLayer.pop())
		{
			this.viewer.entities.remove(geom);
		}
	}

	getViewer()
	{
		return this.viewer;
	}
}
