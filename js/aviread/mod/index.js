import * as web from "/js/@sswroom/sswr/web.js";

if (navigator.serviceWorker) {
	navigator.serviceWorker.register('./service-worker.js',{scope : '/aviread/'}).then(function(registration) {
		console.log('ServiceWorker registration successful with scope: ', registration.scope);
	}).catch(function(err) {
		console.log('ServiceWorker registration failed: ', err);
	});
}

console.log(web.appendUrl("../img/circle.png", "http://127.0.0.1:2080/simontest/maptest.html"));