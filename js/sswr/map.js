import * as geometry from "./geometry.js";
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

export class GPSTrack
{
	constructor(recs)
	{
		this.recs = recs;
	}

	createLineString()
	{
		var coordinates = new Array();
		var i = 0;
		var j = this.recs.length;
		while (i < j)
		{
			coordinates.push([this.recs[i].lon, this.recs[i].lat, this.recs[i].a]);
			i++;
		}
		return new geometry.LineString(4326, coordinates);
	}

	getPosByTime(ts)
	{
		return this.getPosByTicks(ts.toTicks());
	}

	getPosByTicks(ticks)
	{
		if (ticks >= this.recs[0].t && ticks <= this.recs[this.recs.length - 1].t)
		{
			var i = 0;
			var j = this.recs.length - 1;
			var k;
			var l;
			while (i <= j)
			{
				k = (i + j) >> 1;
				l = this.recs[k].t;
				if (ticks > l)
				{
					i = k + 1;
				}
				else if (ticks < l)
				{
					j = k - 1;
				}
				else
				{
					return new math.Vector3(this.recs[k].lon, this.recs[k].lat, this.recs[k].a);
				}
			}
			var tDiff;
			var rec1 = this.recs[i - 1];
			var rec2 = this.recs[i];
			tDiff = rec2.t - rec1.t;
			return new math.Vector3(
				(rec1.lon * (rec2.t - ticks) + rec2.lon * (ticks - rec1.t)) / tDiff,
				(rec1.lat * (rec2.t - ticks) + rec2.lat * (ticks - rec1.t)) / tDiff,
				(rec1.a * (rec2.t - ticks) + rec2.a * (ticks - rec1.t)) / tDiff);
		}
		return new math.Vector3(0, 0, 0);
	}
}
