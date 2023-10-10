import * as math from "./math.js";
import * as web from "./web.js";
import { DistanceUnit } from "./unit.js";

export const DataFormat = {
	Cesium: "cesium",
	WKT: "wkt",
	GeoJSON: "geojson"
};
	
export function calcDistance(srid, geom, x, y)
{
	var pt = geom.calBoundaryPoint(x, y);
	var csys = math.CoordinateSystemManager.srCreateCsys(srid);
	return csys.calcSurfaceDistance(x, y, pt.x, pt.y, DistanceUnit.METER);
}

export function getLayers(svcUrl, onResultFunc)
{
	web.loadJSON(svcUrl + '/getlayers', onResultFunc);
}

export function getLayerData(svcUrl, onResultFunc, layerName, dataFormat)
{
	web.loadJSON(svcUrl + '/getlayerdata?name='+encodeURIComponent(layerName)+"&fmt="+encodeURIComponent(dataFormat), onResultFunc);
}
