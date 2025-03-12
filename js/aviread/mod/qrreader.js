import * as web from "/js/@sswroom/sswr/web.js";

function onScanSuccess(decodedText, decodedResult)
{
	web.getInputElement("txtQRCode").value = decodedText;
	console.log(decodedResult);
}

function onScanFailed(error)
{
	console.log(error);
}

const config = { fps: 10, qrbox: {width: 250, height: 250} };

//const html5QrCode = new Html5Qrcode("reader", true);
//html5QrCode.start({facingMode: "environment"}, config, onScanSuccess);

const html5QrCode = new Html5QrcodeScanner("reader", config, true);
html5QrCode.render(onScanSuccess, onScanFailed);