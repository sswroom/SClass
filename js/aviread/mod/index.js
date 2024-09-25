import * as crypto from "/js/@sswroom/sswr/crypto.js";
import * as data from "/js/@sswroom/sswr/data.js";
import * as map from "/js/@sswroom/sswr/map.js";
import {XLSXExporter} from "/js/@sswroom/sswr/exporter/XLSXExporter.js";
import {GPXExporter} from "/js/@sswroom/sswr/exporter/GPXExporter.js";
import * as spreadsheet from "/js/@sswroom/sswr/spreadsheet.js";
import * as text from "/js/@sswroom/sswr/text.js";
import * as unit from "/js/@sswroom/sswr/unit.js";
import * as web from "/js/@sswroom/sswr/web.js";
import * as zip from "/js/@sswroom/sswr/zip.js";

if (navigator.serviceWorker) {
	navigator.serviceWorker.register('./service-worker.js',{scope : '/aviread/'}).then(function(registration) {
		console.log('ServiceWorker registration successful with scope: ', registration.scope);
	}).catch(function(err) {
		console.log('ServiceWorker registration failed: ', err);
	});
}

console.log(web.appendUrl("../img/circle.png", "http://127.0.0.1:2080/simontest/maptest.html"));

let iv = new Uint8Array([0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc]);
let enc = new crypto.AES256GCM(crypto.ICrypto.keyToBuffer("Testing", 32), iv);
let encText = await enc.encrypt(new TextEncoder().encode("Hello World"));
let b64 = new text.Base64Enc();
console.log(b64.encodeBin(encText));
console.log(new TextDecoder().decode(await enc.decrypt(encText)));

let builder = new data.ByteBuilder();
builder.writeInt32(0, 1000, true);
builder.writeInt16(4, 10000, true);
console.log(text.u8Arr2Hex(builder.build(), " "));

/*let zbuilder = new zip.ZIPBuilder(zip.ZIPOS.UNIX);
let t = data.Timestamp.now();
zbuilder.addFile("Testing.txt", new TextEncoder().encode("Testing Testing"), t, t, t, 0);
let zipFile = zbuilder.finalize();
web.openData(new Blob([zipFile]), "application/zip", "Testing.zip");*/

/*let wb = new spreadsheet.Workbook();
let sheet = wb.addWorksheet("Test Sheet");
sheet.setCellString(0, 0, "2024-Jan");
sheet.setCellInt32(1, 0, 24);
sheet.setCellString(0, 1, "2024-Feb");
sheet.setCellInt32(1, 1, 12);
sheet.setCellString(0, 2, "2024-Mar");
sheet.setCellInt32(1, 2, 36);
let chart = sheet.createChart(unit.Distance.Unit.CENTIMETER, 0, 3, 15, 10, "Chart Test");
chart.initLineChart("Count", "Month", spreadsheet.AxisType.Category);
chart.addLegend(spreadsheet.LegendPos.Bottom);
chart.addSeries(new spreadsheet.WorkbookDataSource(sheet, 0, 0, 0, 2), new spreadsheet.WorkbookDataSource(sheet, 1, 1, 0, 2), "Test", false);
let exporter = new XLSXExporter();
let bytes = exporter.exportFile("test.xlsx", wb);
if (bytes)
{
	web.openData(new Blob([bytes]), exporter.getOutputMIME(), "test.xlsx");
}*/

/*let recs = [];
let t = new Date().getTime();
recs.push({recTime: t, lat: 22.4, lon: 114.2, altitude: 1, speed: 0, heading: 0, sateUsed: 3, valid: true});
recs.push({recTime: t + 1000, lat: 22.5, lon: 114.2, altitude: 2, speed: 0, heading: 0, sateUsed: 3, valid: true});
let track = new map.GPSTrack(recs);
let exporter = new GPXExporter();
let bytes = exporter.exportFile("test.gpx", track);
if (bytes)
{
	web.openData(new Blob([bytes]), exporter.getOutputMIME(), "test.gpx");
}*/
