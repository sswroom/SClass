import math from "./math.js";

var map = {};
map.DataFormat = {};
map.DataFormat.Cesium = "cesium";
map.DataFormat.WKT = "wkt";
map.DataFormat.GeoJSON = "geojson";

map.calcDistance = function(srid, geom, x, y)
{
	var pt = geom.calBoundaryPoint(x, y);
	var csys = math.CoordinateSystemManager.srCreateCsys(srid);
	return csys.calcSurfaceDistance(x, y, pt.x, pt.y, math.unit.Distance.DistanceUnit.METER);
}

map.getLayers = function(svcUrl, onResultFunc)
{
	web.loadJSON(svcUrl + '/getlayers', onResultFunc);
}

map.getLayerData = function(svcUrl, onResultFunc, layerName, dataFormat)
{
	web.loadJSON(svcUrl + '/getlayerdata?name='+encodeURIComponent(layerName)+"&fmt="+encodeURIComponent(dataFormat), onResultFunc);
}

export default map;