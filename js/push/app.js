import * as data from '../sswr/data.js';
import * as text from '../sswr/text.js';

window.appLoaded = function()
{
}

window.notifyClicked = function()
{
	if (window.Notification)
	{
		window.Notification.requestPermission().then(function(result) {
			if (result !== "granted") {
				console.log("No notification permission granted!");
				return;
			} else {
				console.log("Button clicked");
				new window.Notification("This is a notification", {
					body: "This is the body of the notification",
				});
			}
		});
	}
	else if (window.reg)
	{
		window.reg.showNotification("This is a notification", {
			body: "This is the body of the notification",
		});
	}
	else
	{
		console.log("Notification not exist");
	}
}

window.registerClicked = function()
{
	if (window.reg)
		return;
	if (navigator.serviceWorker) {
		navigator.serviceWorker.register('./service-worker.js',{scope : '/push/'}).then(function(registration) {
			window.reg = registration;
			console.log('ServiceWorker registration successful with scope: ', registration.scope);
			var options = null;
			var appKey = document.getElementById("appKey").value;
			if (appKey && appKey.length > 0)
			{
				options = {applicationServerKey: appKey};
			}
			registration.pushManager.subscribe(options).then(
				(pushSubscription) => {
					var d = document.getElementById("message");
					d.innerHTML = "<br/>Endpoint: "+text.toHTMLText(pushSubscription.endpoint)+
					"<br/>Key: "+data.arrayBuffer2Base64(pushSubscription.getKey("p256dh"))+
					"<br/>Auth: "+data.arrayBuffer2Base64(pushSubscription.getKey("auth"))+
					"<br/>Exp Time: "+pushSubscription.expirationTime;
				},
				(error) => {
					console.error(error);
				},
			).catch(function(err) {
				console.log('Subscribe failed: ', err);
			});
		}).catch(function(err) {
			console.log('ServiceWorker registration failed: ', err);
		});
	}
	else
	{
		console.log('ServiceWorker not found');
	}
}