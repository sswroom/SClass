import * as math from "./sswr/math.js";
import * as map from "./sswr/map.js";
import * as web from "./sswr/web.js";
import { Timestamp } from "./sswr/data.js";

function burialSiteReceived(result)
{
	var pg = math.GeoJSON.parseGeometry(4326, result.features[0].geometry);
	var x = 114.22122894202;
	var y = 22.362089455590;
	console.log("Distance = "+map.calcDistance(4326, pg, x, y) + ", "+pg.insideOrTouch(new math.Coord2D(x, y)));
}

function geomDistTest()
{
	web.loadJSON('http://192.168.1.113:8765/geoserver/main/ows?service=WFS&version=1.0.0&request=GetFeature&typeName=main%3Ahad_burial_site&outputFormat=application%2Fjson&CQL_FILTER=site_code=%27ND-FL-14C%27', burialSiteReceived);
}

function roundTest()
{
	var temp = [22.450865, 114.220012];
	console.log(math.roundToStr(22.450865, 5))
	console.log(math.roundToFloat(temp[0], 5))
	console.log(math.roundToFloat(89.687835, 5));
	console.log(math.roundToStr(89.687835, 5));
	console.log(math.roundToStr(0.000005, 5));
	console.log(math.roundToStr(0.000004, 5));
	console.log(Timestamp.utcNow().toStringISO8601());
	console.log(performance.now());
}

async function wmsTest()
{
	var wms = new map.WMS("http://192.168.1.25:8080/geoserver/pbg/wms", "pbg:had_burial_poly");
	await wms.queryInfos();
	console.log(wms);
}

wmsTest();