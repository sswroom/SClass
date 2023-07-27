import math from "./math.h";
import text from "./text.h";

let cesium = new Object();
var sswr = new Object();
sswr.cesium = cesium;
sswr.math = math;
sswr.text = text;

sswr.cesium.screenToLatLon = function(viewer, x, y, ellipsoid)
{
	var pos = new Cesium.Cartesian2(x, y);
	if (ellipsoid == null)
		ellipsoid = viewer.scene.globe.ellipsoid;
	var cartesian = viewer.camera.pickEllipsoid(pos, ellipsoid);
	if (cartesian)
	{
		var cartographic = ellipsoid.cartesianToCartographic(cartesian);
		return {"lat":cartographic.latitude * 180 / Math.PI,"lon":cartographic.longitude * 180 / Math.PI};
	}
	else
	{
		return {"lat":null,"lon":null};
	}
}

sswr.cesium.fromCXYZArray = function(arr)
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

sswr.cesium.toCartesian3Arr = function(coords)
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

sswr.cesium.newObjFromGeoJSON = function(geoJSON)
{
	var o = new Object();
	o.id = geoJSON.id;
	o.name = geoJSON.id;
	var n;
	var props = new Array();
	for (n in geoJSON.properties)
	{
		props.push(sswr.text.toHTMLText(n)+": "+sswr.text.toHTMLText(geoJSON.properties[n]));
	}
	o.description = props.join("<br/>");
	return o;
}

sswr.cesium.addGeoJSON = function(viewer, geoJSON, color, extSize)
{
	var oColor = color.darken(0.5, new Cesium.Color());
	if (geoJSON.type == "FeatureCollection")
	{
		var i = 0;
		var j = geoJSON.features.length;
		while (i < j)
		{
			sswr.cesium.addGeoJSON(viewer, geoJSON.features[i], color, extSize);			
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
					o = sswr.cesium.newObjFromGeoJSON(geoJSON);
					o.id = o.id + "_" + i;
					o.polygon = new Object();
					o.polygon.hierarchy = sswr.cesium.toCartesian3Arr(coordinates[i]);
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

sswr.cesium.fromCartesian3Array = function(viewer, arr)
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

sswr.cesium.fromPolygonGraphics = function(viewer, pg)
{
	var coordinates = new Array();
	var hierarchy = pg.hierarchy.getValue();
	coordinates.push(sswr.cesium.fromCartesian3Array(viewer, hierarchy.positions));
	var i = 0;
	var j =hierarchy.holes.length;
	while (i < j)
	{
		coordinates.push(sswr.cesium.fromCartesian3Array(viewer, hierarchy.holes[i].positions));
		i++;
	}
	return new sswr.math.geometry.Polygon(4326, coordinates);
}

/*sswr.cesium.createPolygon = function(viewer, lats, lons, height)
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

export default cesium;