import * as data from "./data.js";
import * as text from "./text.js";

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

export function buildTable(o)
{
	var name;
	var ret = new Array();
	ret.push("<table>");
	if (data.isArray(o))
	{
		if (o.length > 0)
		{
			ret.push("<tr>");
			for (name in o[0])
			{
				ret.push("<th>"+text.toHTMLText(name)+"</th>");
			}
			ret.push("</tr>");
		}
		var i = 0;
		var j = o.length;
		var obj;
		while (i < j)
		{
			obj = o[i];
			ret.push("<tr>");
			for (name in o[0])
			{
				ret.push("<td>"+text.toHTMLText(""+obj[name])+"</td>");
			}
			ret.push("</tr>");
			i++;
		}
	}
	else if (data.isObject(o))
	{
		for (name in o)
		{
			ret.push("<tr>");
			ret.push("<td>"+text.toHTMLText(name)+"</td>");
			ret.push("<td>"+text.toHTMLText(""+o[name])+"</td>");
			ret.push("</tr>");
		}
	}
	ret.push("</table>");
	return ret.join("");
}

export function openData(data, contentType, fileName)
{
	if (typeof data == "string")
	{
		var ele = document.createElement("a");
		ele.setAttribute('href', 'data:'+encodeURIComponent(contentType)+";charset=utf-8," + encodeURIComponent(data));
		if (fileName)
			ele.setAttribute('download', fileName);
		ele.style.display = 'none';

		document.body.appendChild(ele);
		ele.click();
		document.body.removeChild(ele);
	}
	return;
}

function hexColor(c)
{
	var b = c % 256;
	var g = (c >> 8) % 256;
	var r = (c >> 16) % 256;
	return {a: 1.0, r: r / 255, g: g / 255, b: b / 255};
}

export function parseCSSColor(c)
{
	if (c.startsWith("#"))
	{
		if (c.length == 7)
		{
			return hexColor(parseInt(c.substring(1), 16));
		}
	}
	else
	{
		switch (c)
		{
		case "black":
			return hexColor(0x000000);
		case "silver":
			return hexColor(0xc0c0c0);
		case "gray":
			return hexColor(0x808080);
		case "white":
			return hexColor(0xffffff);
		case "maroon":
			return hexColor(0x800000);
		case "red":
			return hexColor(0xff0000);
		case "purple":
			return hexColor(0x800080);
		case "fuchsia":
			return hexColor(0xff00ff);
		case "green":
			return hexColor(0x008000);
		case "lime":
			return hexColor(0x00ff00);
		case "olive":
			return hexColor(0x808000);
		case "yellow":
			return hexColor(0xffff00);
		case "navy":
			return hexColor(0x000080);
		case "blue":
			return hexColor(0x0000ff);
		case "teal":
			return hexColor(0x008080);
		case "aqua":
			return hexColor(0x00ffff);
		}
	}
	return {a: 0.0, r: 0.0, g: 0.0, b: 0.0};
}

export class Dialog
{
	constructor(content, options)
	{
		this.content = content;
		this.options = data.mergeOptions(options, {
			width: 200,
			height: 200,
			zIndex: 100,
			buttonClass: "dialog-button",
			contentClass: "dialog-content",
			buttons: [this.closeButton()],
			margin: 0
			});
		this.darkColor = null;
	}

	show()
	{
		if (this.darkColor)
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
		var width;
		var height;
		if (typeof this.options.width == "number")
			width = this.options.width + "px";
		else
			width = this.options.width;
		if (typeof this.options.height == "number")
			height = this.options.height + "px";
		else
			height = this.options.height;
		dialog.style.width = width;
		dialog.style.height = height;
		this.darkColor.appendChild(dialog);

		var content = document.createElement("table");
		content.setAttribute("border", "0");
		content.style.width = width;
		content.style.height = height;
		var row = document.createElement("tr");
		var contentCell = document.createElement("td");
		contentCell.setAttribute("colspan", this.options.buttons.length);
		contentCell.className = this.options.contentClass;
		contentCell.style.overflowY = "auto";
		if (this.options.margin)
		{
			contentCell.style.marginLeft = this.options.margin+"px";
			contentCell.style.marginTop = this.options.margin+"px";
			contentCell.style.marginRight = this.options.margin+"px";
		}
		if (this.content instanceof HTMLElement)
			contentCell.appendChild(this.content);
		else
			contentCell.innerHTML = this.content;
		row.appendChild(contentCell);
		content.appendChild(row);
		row = document.createElement("tr");
		row.setAttribute("height", "20");
		var i;
		var col;
		for (i in this.options.buttons)
		{
			col = document.createElement("td");
			col.style.textAlign = "center";
			col.style.verticalAlign = "middle";
			if (i == 0 && this.options.margin)
			{
				col.style.marginLeft = this.options.margin+"px";
			}
			col.className = this.options.buttonClass;
			col.addEventListener("click", this.options.buttons[i].onclick, this);
			col.innerText = this.options.buttons[i].name;
			row.append(col);
		}
		content.appendChild(row);
		dialog.appendChild(content);
		contentCell.style.display = "inline-block";
		if (this.options.margin)
		{
			var minHeight = row.offsetHeight;
			row.setAttribute("height", this.options.margin + minHeight);
			contentCell.style.height = (dialog.offsetHeight - minHeight - this.options.margin - this.options.margin)+"px";
		}
		else
		{
			contentCell.style.height = (dialog.offsetHeight - row.offsetHeight)+"px";
		}
	}

	close()
	{
		if (this.darkColor)
		{
			document.body.removeChild(this.darkColor);
			this.darkColor = null;
		}
	}

	setContent(content)
	{
		this.content = content;
	}

	updateOption(name, value)
	{
		this.options[name] = value;
	}

	closeButton(name)
	{
		if (name == null) name = "Close";
		return {name: name, onclick: ()=>{this.close();}};
	}
};
