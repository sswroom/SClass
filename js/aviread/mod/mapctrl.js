import * as cesium from "/js/@sswroom/sswr/cesium.js";
import * as kml from "/js/@sswroom/sswr/kml.js";
import * as leaflet from "/js/@sswroom/sswr/leaflet.js";
import * as map from "/js/@sswroom/sswr/map.js";
import * as math from "/js/@sswroom/sswr/math.js";
import * as olayer2 from "/js/@sswroom/sswr/olayer2.js";
import * as parser from "/js/@sswroom/sswr/parser.js";
import * as web from "/js/@sswroom/sswr/web.js";
import * as domtoimage from "/js/@sswroom/sswr/domtoimage/index.js";
import { EasyPrint } from "/js/@sswroom/sswr/leaflet/EasyPrint.js";

async function onFileDrop(file)
{
	if (file instanceof File)
	{
		let obj = await parser.parseFile(file);
		if (obj == null)
		{

		}
		else if (obj instanceof kml.KMLFile)
		{
			let bounds = obj.root.getBounds();
			mapCtrl.addKML(obj);
			if (bounds)
				mapCtrl.zoomToExtent(bounds);
		}
		else
		{
			console.log("Unknown parsed file", obj);
		}
	}
}

function onBrowseClick()
{
	let inputElement = document.createElement("input");
	inputElement.type = "file";
	inputElement.addEventListener("change", (e)=>{
		onFileDrop(e.target.files[0]);
	});
	inputElement.dispatchEvent(new MouseEvent("click")); 
}

let tileLayer;
async function onCaptureClick()
{
//	let print = new EasyPrint(mapCtrl.mapObj, {tileLayer: tileLayer});
//	print.printMap("A4Landscape", null);

	let map = document.getElementById("map");
	let svg = await domtoimage.toSvg(map, null);//{imagePlaceholder: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAABmJLR0QA/wD/AP+gvaeTAAAACXBIWXMAAC4jAAAuIwF4pT92AAAAB3RJTUUH6AMECQMVtyBSbwAAABl0RVh0Q29tbWVudABDcmVhdGVkIHdpdGggR0lNUFeBDhcAAAALSURBVAjXY2AAAgAABQAB4iYFmwAAAABJRU5ErkJggg=="});
	web.openUrl(svg, "test.svg");

//	let map = document.getElementById("map");
//	let png = await domtoimage.toPng(map, null);//{imagePlaceholder: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAABmJLR0QA/wD/AP+gvaeTAAAACXBIWXMAAC4jAAAuIwF4pT92AAAAB3RJTUUH6AMECQMVtyBSbwAAABl0RVh0Q29tbWVudABDcmVhdGVkIHdpdGggR0lNUFeBDhcAAAALSURBVAjXY2AAAgAABQAB4iYFmwAAAABJRU5ErkJggg=="});
//	web.openUrl(png);
}

let mapCtrl;
if (window.L)
{
	mapCtrl = new leaflet.LeafletMap("map");
//	let mapObj = L.map('map').setView([22.4, 114.2], 13);
//	L.tileLayer('https://tile.openstreetmap.org/{z}/{x}/{y}.png', {maxZoom: 19}).addTo(mapObj);
}
else if (window.Cesium)
{
	mapCtrl = new cesium.CesiumMap("map");
}
else if (window.OpenLayers)
{
	mapCtrl = new olayer2.Olayer2Map("map");
}
else
{
	console.log("Unknown map type");
}

if (mapCtrl instanceof map.MapControl)
{
	mapCtrl.panZoomScale(new math.Coord2D(114.2, 22.4), 20000);
	let lyr = mapCtrl.createLayer({
		name: "OSM",
		url: "https://tile.openstreetmap.org/{z}/{x}/{y}.png",
		type: map.WebMapType.OSMTile,
		maxZoom: 19
	});
	mapCtrl.addLayer(lyr);
	tileLayer = lyr;
	web.handleFileDrop(document.getElementById("map"), onFileDrop);
}

document.getElementById("btnBrowse").addEventListener("click", onBrowseClick);
let btn = document.getElementById("btnCapture");
if (btn)
{
	btn.addEventListener("click", onCaptureClick);
}