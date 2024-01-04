import * as data from "./data.js";

export function getRequestURLBase()
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
	
export function getParameterByName(name)
{
	var url = window.location.href;
	name = name.replace(/[[\]]/g, '\\$&');
	var regex = new RegExp('[?&]' + name + '(=([^&#]*)|&|#|$)'),
		results = regex.exec(url);
	if (!results) return null;
	if (!results[2]) return '';
	return decodeURIComponent(results[2].replace(/\+/g, ' '));
}
	
export function loadJSON(url, onResultFunc)
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

export class Dialog
{
	constructor(content, options)
	{
		this.showing = false;
		this.content = content;
		this.options = data.mergeOptions(options, {
			width: 200,
			height: 200,
			zIndex: 100,
			buttonClass: "dialog-button",
			contentClass: "dialog-content",
			buttons: [Dialog.closeButton()]
			});
		this.darkColor = null;
	}

	show()
	{
		if (this.showing)
			return;
		var darkColor = document.createElement("div");
		darkColor.style.position = "absolute";
		darkColor.style.width = "100%";
		darkColor.style.height = "100%";
		darkColor.style.backgroundColor = "rgba(0, 0, 0, 0.7)";
		darkColor.style.zIndex = this.options.zIndex;
		darkColor.style.display = "flex";
		darkColor.style.alignItems = "center";
		darkColor.style.justifyContent = "center";
		if (document.body.children.length > 0)
			document.body.insertBefore(darkColor, document.body.children[0]);
		else
			document.body.appendChild(darkColor);
		this.darkColor = darkColor;
		var dialog = document.createElement("div");
		dialog.style.backgroundColor = "#ffffff";
		dialog.style.width = "200px";
		dialog.style.height = "200px";
		this.darkColor.appendChild(dialog);

		var content = document.createElement("table");
		content.setAttribute("border", "0");
		content.setAttribute("width", "100%");
		content.setAttribute("height", "100%");
		var row = document.createElement("tr");
		var col = document.createElement("td");
		col.setAttribute("colspan", this.options.buttons.length);
		col.className = this.options.contentClass;
		if (this.content instanceof HTMLElement)
			col.appendChild(this.content);
		else
			col.innerHTML = this.content;
		row.appendChild(col);
		content.appendChild(row);
		row = document.createElement("tr");
		row.setAttribute("height", "20");
		var i;
		for (i in this.options.buttons)
		{
			col = document.createElement("td");
			col.style.textAlign = "center";
			col.style.verticalAlign = "middle";
			col.className = this.options.buttonClass;
			col.onclick = this.options.buttons[i].onclick;
			col.innerText = this.options.buttons[i].name;
			row.append(col);
		}
		content.appendChild(row);
		dialog.appendChild(content);
	}

	close()
	{
		if (this.darkColor)
		{
			document.body.removeChild(this.darkColor);
			this.darkColor = null;
		}
	}

	static closeButton(name)
	{
		if (name == null) name = "Close";
		return {name: name, onclick: ()=>{this.close();}};
	}
};
