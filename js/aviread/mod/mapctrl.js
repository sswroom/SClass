import * as cesium from "/js/@sswroom/sswr/cesium.js";
import * as leaflet from "/js/@sswroom/sswr/leaflet.js";
import * as map from "/js/@sswroom/sswr/map.js";
import * as math from "/js/@sswroom/sswr/math.js";

var mapCtrl;
if (window.L)
{
	mapCtrl = new leaflet.LeafletMap("map");
//	var mapObj = L.map('map').setView([22.4, 114.2], 13);
//	L.tileLayer('https://tile.openstreetmap.org/{z}/{x}/{y}.png', {maxZoom: 19}).addTo(mapObj);
}
else if (window.Cesium)
{
	mapCtrl = new cesium.CesiumMap("map");
}

if (mapCtrl instanceof map.MapControl)
{
	mapCtrl.panZoomScale(new math.Coord2D(114.2, 22.4), 20000);
	var lyr = mapCtrl.createLayer({
		name: "OSM",
		url: "https://tile.openstreetmap.org/{z}/{x}/{y}.png",
		type: map.WebMapType.OSMTile,
		maxZoom: 19
	});
	mapCtrl.addLayer(lyr);
}