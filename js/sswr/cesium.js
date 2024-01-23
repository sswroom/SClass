import * as map from "./map.js";
import { Coord2D } from "./math.js";
import { Polygon } from "./geometry.js";

export function screenToLatLon(viewer, x, y, ellipsoid)
{
	var pos = new Cesium.Cartesian2(x, y);
	if (ellipsoid == null)
		ellipsoid = viewer.scene.globe.ellipsoid;
	var cartesian = viewer.camera.pickEllipsoid(pos, ellipsoid);
	if (cartesian)
	{
		var cartographic = ellipsoid.cartesianToCartographic(cartesian);
		return new Coord2D(cartographic.longitude * 180 / Math.PI, cartographic.latitude * 180 / Math.PI);
	}
	else
	{
		return new Coord2D(0, 0);
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
	return new Polygon(4326, coordinates);
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
			if (options && options.maxZoom)
			{
				opt.maximumLevel = options.maxZoom;
			}
			return new Cesium.ImageryLayer(new Cesium.UrlTemplateImageryProvider({
				url: layer.url}, opt));
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
