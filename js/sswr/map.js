import math from "./math.js";

var map = {
	DataFormat: {
		Cesium: "cesium",
		WKT: "wkt",
		GeoJSON: "geojson"
	},
	
	calcDistance: function(srid, geom, x, y)
	{
		var pt = geom.calBoundaryPoint(x, y);
		var csys = math.CoordinateSystemManager.srCreateCsys(srid);
		return csys.calcSurfaceDistance(x, y, pt.x, pt.y, math.unit.Distance.DistanceUnit.METER);
	},
	
	getLayers: function(svcUrl, onResultFunc)
	{
		web.loadJSON(svcUrl + '/getlayers', onResultFunc);
	},
	
	getLayerData: function(svcUrl, onResultFunc, layerName, dataFormat)
	{
		web.loadJSON(svcUrl + '/getlayerdata?name='+encodeURIComponent(layerName)+"&fmt="+encodeURIComponent(dataFormat), onResultFunc);
	}
};

export default map;