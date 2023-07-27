let web = new Object();
var sswr = new Object();
sswr.web = web;


sswr.web = new Object();
sswr.web.getRequestURLBase = function()
{
	var url = document.location.href;
	var i = url.indexOf("://");
	var j = url.indexOf("/", i + 3);
	if (j >= 0)
	{
		return url.substring(0, j);
	}
	else
	{
		return url;
	}
}

sswr.web.getParameterByName = function(name)
{
	var url = window.location.href;
	name = name.replace(/[[\]]/g, '\\$&');
    var regex = new RegExp('[?&]' + name + '(=([^&#]*)|&|#|$)'),
        results = regex.exec(url);
    if (!results) return null;
    if (!results[2]) return '';
    return decodeURIComponent(results[2].replace(/\+/g, ' '));
}

sswr.web.loadJSON = function(url, onResultFunc)
{
	var xmlhttp = new XMLHttpRequest();
	xmlhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200)
		{
			onResultFunc(JSON.parse(this.responseText));
		}
	};
	xmlhttp.open("GET", url, true);
	xmlhttp.send();
}

export default web;