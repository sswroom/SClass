function appLoaded()
{
	if (navigator.serviceWorker) {
		navigator.serviceWorker.register('./service-worker.js',{scope : '/pwa/'}).then(function(registration) {
			console.log('ServiceWorker registration successful with scope: ', registration.scope);
		}).catch(function(err) {
			console.log('ServiceWorker registration failed: ', err);
		});
	}
}