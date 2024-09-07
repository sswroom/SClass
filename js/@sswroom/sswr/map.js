import * as data from "./data.js";
import * as geometry from "./geometry.js";
import * as math from "./math.js";
import * as web from "./web.js";
import * as unit from "./unit.js";

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

/**
 * @param {number} srid
 * @param {geometry.Vector2D} geom
 * @param {number} x
 * @param {number} y
 */
export function calcDistance(srid, geom, x, y)
{
	let pt = geom.calBoundaryPoint(new math.Coord2D(x, y));
	let csys = math.CoordinateSystemManager.srCreateCsys(srid);
	if (csys == null)
		throw new Error("srid "+srid+" is not supported");
	return csys.calcSurfaceDistance(x, y, pt.x, pt.y, unit.Distance.Unit.METER);
}

/**
 * @param {string} svcUrl
 */
export function getLayers(svcUrl, onResultFunc)
{
	web.loadJSON(svcUrl + '/getlayers', onResultFunc);
}

export function getLayerData(svcUrl, onResultFunc, layerName, dataFormat)
{
	web.loadJSON(svcUrl + '/getlayerdata?name='+encodeURIComponent(layerName)+"&fmt="+encodeURIComponent(dataFormat), onResultFunc);
}

export class GPSTrack extends data.ParsedObject
{
	/**
	 * @param {{recTime: number;lat: number;lon: number;altitude: number;speed: number;heading: number;valid: boolean;sateUsed: number;}[]} recs
	 * @param {string|undefined} sourceName
	 */
	constructor(recs, sourceName)
	{
		super(sourceName||"Untitled.gpx", "GPSTrack");
		this.recs = recs;
	}

	/**
	 * @param {GeolocationPosition} pos
	 */
	addPosition(pos)
	{
		this.recs.push({
			recTime: pos.timestamp,
			lat: pos.coords.latitude,
			lon: pos.coords.longitude,
			altitude: pos.coords.altitude || 0,
			heading: pos.coords.heading||0,
			speed: (pos.coords.speed || 0) * 3.6 / 1.852,
			valid: true,
			sateUsed: -1});
	}

	getTrackCnt()
	{
		return 1;
	}

	/**
	 * @param {number} index
	 */
	getTrack(index)
	{
		if (index == 0)
			return this.recs;
		return null;
	}

	createLineString()
	{
		let coordinates = new Array();
		let i = 0;
		let j = this.recs.length;
		while (i < j)
		{
			coordinates.push([this.recs[i].lon, this.recs[i].lat, this.recs[i].altitude]);
			i++;
		}
		return new geometry.LineString(4326, coordinates);
	}

	/**
	 * @param {data.Timestamp} ts
	 */
	getPosByTime(ts)
	{
		return this.getPosByTicks(ts.toTicks());
	}

	/**
	 * @param {number} ticks
	 */
	getPosByTicks(ticks)
	{
		if (ticks >= this.recs[0].recTime && ticks <= this.recs[this.recs.length - 1].recTime)
		{
			let i = 0;
			let j = this.recs.length - 1;
			let k;
			let l;
			while (i <= j)
			{
				k = (i + j) >> 1;
				l = this.recs[k].recTime;
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
					return new math.Vector3(this.recs[k].lon, this.recs[k].lat, this.recs[k].altitude);
				}
			}
			let tDiff;
			let rec1 = this.recs[i - 1];
			let rec2 = this.recs[i];
			tDiff = rec2.recTime - rec1.recTime;
			return new math.Vector3(
				(rec1.lon * (rec2.recTime - ticks) + rec2.lon * (ticks - rec1.recTime)) / tDiff,
				(rec1.lat * (rec2.recTime - ticks) + rec2.lat * (ticks - rec1.recTime)) / tDiff,
				(rec1.altitude * (rec2.recTime - ticks) + rec2.altitude * (ticks - rec1.recTime)) / tDiff);
		}
		return new math.Vector3(0, 0, 0);
	}
}

export class GeolocationFilter
{
	constructor(minSecs, minDistMeter)
	{
		this.csys = math.CoordinateSystemManager.srCreateCsys(4326);
		if (this.csys == null)
			throw new Error();
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
			if (this.csys.calcSurfaceDistance(pos.coords.longitude, pos.coords.latitude, this.lastPos.coords.longitude, this.lastPos.coords.latitude, unit.Distance.Unit.METER) < this.minDistMeter)
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
			let resp = await fetch(this.url + "?SERVICE=WMS&REQUEST=GetCapabilities");
			let parser = new DOMParser();
			let contentType = resp.headers.get("Content-Type") || "text/xml";
			// @ts-ignore
			let doc = parser.parseFromString(await resp.text(), contentType);
			let node = doc.childNodes[0];
			if (node.nodeName == "WMS_Capabilities" || node.nodeName == "WMT_MS_Capabilities")
			{
				// @ts-ignore
				let attr = node.attributes.getNamedItem("version");
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
		let x = (mapPos.x - bounds.min.x) * width / bounds.getWidth();
		let y = (bounds.max.y - mapPos.y) * height / bounds.getHeight();
		let url;
	
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
		let resp = await fetch(url);
		if (resp.ok)
		{
			return await resp.json();
		}
		return null;
	}
}
