import * as crypto from "/js/@sswroom/sswr/crypto.js";
import * as data from "/js/@sswroom/sswr/data.js";
import {XLSXExporter} from "/js/@sswroom/sswr/exporter/XLSXExporter.js";
import * as spreadsheet from "/js/@sswroom/sswr/spreadsheet.js";
import * as text from "/js/@sswroom/sswr/text.js";
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


let wb = new spreadsheet.Workbook();
let sheet = wb.addWorksheet("Test Sheet");
sheet.setCellString(0, 0, "ABC");
let exporter = new XLSXExporter();
let bytes = exporter.exportFile("test.xlsx", wb);
if (bytes)
{
	web.openData(new Blob([bytes]), exporter.getOutputMIME(), "test.xlsx");
}
