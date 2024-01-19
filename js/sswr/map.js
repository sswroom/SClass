import * as geometry from "./geometry.js";
import * as math from "./math.js";
import * as web from "./web.js";
import { DistanceUnit } from "./unit.js";

export const DataFormat = {
	Cesium: "cesium",
	WKT: "wkt",
	GeoJSON: "geojson"
};

export const WebMapType = {
	WMS: 0,
	WFS: 1,
	ArcGIS: 2,
	OSMTile: 3
};

export const GeoJSONType = {
	Feature: "Feature",
	FeatureCollection: "FeatureCollection"
};

export const GeometryType = {
	Point: "Point",
	MultiPoint: "MultiPoint",
	LineString: "LineString",
	MultiLineString: "MultiLineString",
	Polygon: "Polygon",
	MultiPolygon: "MultiPolygon",
	GeometryCollection: "GeometryCollection"
};

export function calcDistance(srid, geom, x, y)
{
	var pt = geom.calBoundaryPoint(new math.Coord2D(x, y));
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

export class GeolocationFilter
{
	constructor(minSecs, minDistMeter)
	{
		this.csys = math.CoordinateSystemManager.srCreateCsys(4326);
		this.minSecs = minSecs;
		this.minDistMeter = minDistMeter;
	}

	isValid(pos)
	{
		if (this.lastPos == null)
		{
			this.lastPos = pos;
			return true;
		}
		if (this.minSecs)
		{
			if ((pos.timestamp - this.lastPos.timestamp) / 1000.0 < this.minSecs)
				return false;
		}
		if (this.minDistMeter)
		{
			if (this.csys.calcSurfaceDistance(pos.coords.longitude, pos.coords.latitude, this.lastPos.longitude, this.lastPos.latitude, DistanceUnit.METER) < this.minDistMeter)
				return false;
		}

		this.lastPos = pos;
		return true;
	}
}

export class MapControl
{
	constructor()
	{
	}
}

export class WMS
{
	constructor(url, layer, version)
	{
		this.url = url;
		this.layer = layer;
		this.version = version;
	}

	async queryInfos(mapPos, bounds, width, height)
	{
		if (this.version == null)
		{
			var resp = await fetch(this.url + "?SERVICE=WMS&REQUEST=GetCapabilities");
			var parser = new DOMParser();
			var contentType = resp.headers.get("Content-Type") || "text/xml";
			var doc = parser.parseFromString(await resp.text(), contentType);
			if (doc.activeElement.nodeName == "WMS_Capabilities" || doc.activeElement.nodeName == "WMT_MS_Capabilities")
			{
				var attr = doc.activeElement.attributes.getNamedItem("version");
				if (attr)
				{
					this.version = attr.value;
				}
			}
			if (this.version == null)
			{
				return null;
			}
		}
		var x = (mapPos.x - bounds.min.x) * width / bounds.getWidth();
		var y = (bounds.max.y - mapPos.y) * height / bounds.getHeight();
		var url;
	
		if (this.version == "1.1.1")
		{
			url = this.url + "?service=WMS&version=1.1.1&request=GetFeatureInfo&layers="+encodeURIComponent(this.layer);
			url = url + "&bbox="+bounds.min.y+"%2C"+bounds.min.x+"%2C"+bounds.max.y+"%2C"+bounds.max.x+"&width="+width+"&height="+height;
			url = url + "&srs=EPSG%3A4326&styles=&format=image%2Fpng&QUERY_LAYERS="+encodeURIComponent(this.layer)+"&INFO_FORMAT=application%2Fjson&FEATURE_COUNT=5";
			url = url + "&X="+Math.round(x)+"&Y="+Math.round(y);
		}
		else if (this.version == "1.3.0")
		{
			url = this.url + "?service=WMS&version=1.3.0&request=GetFeatureInfo&layers="+encodeURIComponent(this.layer);
			url = url + "&bbox="+bounds.min.y+"%2C"+bounds.min.x+"%2C"+bounds.max.y+"%2C"+bounds.max.x+"&width="+width+"&height="+height;
			url = url + "&CRS=EPSG%3A4326&styles=&format=image%2Fpng&QUERY_LAYERS="+encodeURIComponent(this.layer)+"&INFO_FORMAT=application%2Fjson&FEATURE_COUNT=5";
			url = url + "&I="+Math.round(x)+"&J="+Math.round(y);
		}
		else
		{
			console.log("WMS: Unsupported version", this.version);
			return null;
		}
		console.log(url);
		var resp = await fetch(url);
		if (resp.ok)
		{
			return await resp.json();
		}
		return null;
	}
}
