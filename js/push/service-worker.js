self.addEventListener("push", event => {
	if (!(self.Notification && self.Notification.permission === "granted")) {
		return;
	}
	const notification = new Notification("This is a notification", {
        body: event.data.text(),
      });
	notification.addEventListener("click", () => {
//		clients.openWindow(
//			"https://example.blog.com/2015/03/04/something-new.html",
//		);
	});
});
