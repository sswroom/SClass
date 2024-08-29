import * as crypto from "/js/@sswroom/sswr/crypto.js";
import * as text from "/js/@sswroom/sswr/text.js";
import * as web from "/js/@sswroom/sswr/web.js";

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