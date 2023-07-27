let map = new Object();
var sswr = new Object();
sswr.map = map;

sswr.map = new Object();
sswr.map.DataFormat = new Object();
sswr.map.DataFormat.Cesium = "cesium";
sswr.map.DataFormat.WKT = "wkt";
sswr.map.DataFormat.GeoJSON = "geojson";

sswr.map.calcDistance = function(srid, geom, x, y)
{
	var pt = geom.calBoundaryPoint(x, y);
	var csys = sswr.math.CoordinateSystemManager.srCreateCsys(srid);
	return csys.calcSurfaceDistance(x, y, pt.x, pt.y, sswr.math.unit.Distance.DistanceUnit.METER);
}

sswr.map.getLayers = function(svcUrl, onResultFunc)
{
	sswr.web.loadJSON(svcUrl + '/getlayers', onResultFunc);
}

sswr.map.getLayerData = function(svcUrl, onResultFunc, layerName, dataFormat)
{
	sswr.web.loadJSON(svcUrl + '/getlayerdata?name='+encodeURIComponent(layerName)+"&fmt="+encodeURIComponent(dataFormat), onResultFunc);
}

export default map;