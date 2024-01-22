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
	var i;
	if (c.startsWith("#"))
	{
		if (c.length == 7)
		{
			return hexColor(parseInt(c.substring(1), 16));
		}
	}
	else if ((i = c.indexOf("(")) >= 0 && c.endsWith(")"))
	{
		var funcName = c.substring(0, i).trim();
		if (funcName == 'rgb')
		{
			var funcCont = c.substring(i + 1, c.length - 1).trim();
			var color = {a: 1.0};
			i = funcCont.indexOf("/");
			if (i >= 0)
			{
				var a = funcCont.substring(i + 1).trim();
				funcCont = funcCont.substring(0, i).trim();
				if (a.endsWith("%"))
				{
					color.a = a.substring(0, a.length - 1) / 100;
				}
				else
				{
					color.a = a - 0;
				}
			}
			var rgb = funcCont.split(" ");
			if (rgb.length == 3)
			{
				if (rgb[0].endsWith("%"))
					color.r = rgb[0].substring(0, rgb[0].length - 1) / 100;
				else
					color.r = rgb[0] / 255;
				if (rgb[1].endsWith("%"))
					color.g = rgb[1].substring(0, rgb[1].length - 1) / 100;
				else
					color.g = rgb[1] / 255;
				if (rgb[2].endsWith("%"))
					color.b = rgb[2].substring(0, rgb[2].length - 1) / 100;
				else
					color.b = rgb[2] / 255;
			}
			else
			{
				color.r = 0;
				color.g = 0;
				color.b = 0;
			}
			return color;
		}
	}
	else
	{
		switch (c)
		{
		case "aliceblue":
			return hexColor(0xf0f8ff);
		case "antiquewhite":
			return hexColor(0xfaebd7);
		case "aqua":
			return hexColor(0x00ffff);
		case "aquamarine":
			return hexColor(0x7fffd4);
		case "azure":
			return hexColor(0xf0ffff);
		case "beige":
			return hexColor(0xf5f5dc);
		case "bisque":
			return hexColor(0xffe4c4);
		case "black":
			return hexColor(0x000000);
		case "blanchedalmond":
			return hexColor(0xffebcd);
		case "blue":
			return hexColor(0x0000ff);
		case "blueviolet":
			return hexColor(0x8a2be2);
		case "brown":
			return hexColor(0xa52a2a);
		case "burlywood":
			return hexColor(0xdeb887);
		case "cadetblue":
			return hexColor(0x5f9ea0);
		case "chartreuse":
			return hexColor(0x7fff00);
		case "chocolate":
			return hexColor(0xd2691e);
		case "coral":
			return hexColor(0xff7f50);
		case "cornflowerblue":
			return hexColor(0x6495ed);
		case "cornsilk":
			return hexColor(0xfff8dc);
		case "crimson":
			return hexColor(0xdc143c);
		case "cyan":
			return hexColor(0x00ffff);
		case "fucdarkbluehsia":
			return hexColor(0x00008b);
		case "darkcyan":
			return hexColor(0x008b8b);
		case "darkgoldenrod":
			return hexColor(0xb8860b);
		case "darkgray":
			return hexColor(0xa9a9a9);
		case "darkgreen":
			return hexColor(0x006400);
		case "darkgrey":
			return hexColor(0xa9a9a9);
		case "darkkhaki":
			return hexColor(0xbdb76b);
		case "darkmagenta":
			return hexColor(0x8b008b);
		case "darkolivegreen":
			return hexColor(0x556b2f);
		case "darkorange":
			return hexColor(0xff8c00);
		case "darkorchid":
			return hexColor(0x9932cc);
		case "darkred":
			return hexColor(0x8b0000);
		case "darksalmon":
			return hexColor(0xe9967a);
		case "darkseagreen":
			return hexColor(0x8fbc8f);
		case "darkslateblue":
			return hexColor(0x483d8b);
		case "darkslategray":
			return hexColor(0x2f4f4f);
		case "darkslategrey":
			return hexColor(0x2f4f4f);
		case "darkturquoise":
			return hexColor(0x00ced1);
		case "darkviolet":
			return hexColor(0x9400d3);
		case "deeppink":
			return hexColor(0xff1493);
		case "deepskyblue":
			return hexColor(0x00bfff);
		case "dimgray":
			return hexColor(0x696969);
		case "dodgerblue":
			return hexColor(0x1e90ff);
		case "firebrick":
			return hexColor(0xb22222);
		case "floralwhite":
			return hexColor(0xfffaf0);
		case "forestgreen":
			return hexColor(0x228b22);
		case "fuchsia":
			return hexColor(0xff00ff);
		case "gainsboro":
			return hexColor(0xdcdcdc);
		case "ghostwhite":
			return hexColor(0xf8f8ff);
		case "gold":
			return hexColor(0xffd700);
		case "goldenrod":
			return hexColor(0xdaa520);
		case "gray":
			return hexColor(0x808080);
		case "green":
			return hexColor(0x008000);
		case "greenyellow":
			return hexColor(0xadff2f);
		case "grey":
			return hexColor(0x808080);
		case "honeydew":
			return hexColor(0xf0fff0);
		case "hotpink":
			return hexColor(0xff69b4);
		case "indianred":
			return hexColor(0xcd5c5c);
		case "indigo":
			return hexColor(0x4b0082);
		case "ivory":
			return hexColor(0xfffff0);
		case "khaki":
			return hexColor(0xf0e68c);
		case "lavender":
			return hexColor(0xe6e6fa);
		case "lavenderblush":
			return hexColor(0xfff0f5);
		case "lawngreen":
			return hexColor(0x7cfc00);
		case "lemonchiffon":
			return hexColor(0xfffacd);
		case "lightblue":
			return hexColor(0xadd8e6);
		case "lightcoral":
			return hexColor(0xf08080);
		case "lightcyan":
			return hexColor(0xe0ffff);
		case "lightgoldenrodyellow":
			return hexColor(0xfafad2);
		case "lightgray":
			return hexColor(0xd3d3d3);
		case "lightgreen":
			return hexColor(0x90ee90);
		case "lightgrey":
			return hexColor(0xd3d3d3);
		case "lightpink":
			return hexColor(0xffb6c1);
		case "lightsalmon":
			return hexColor(0xffa07a);
		case "lightseagreen":
			return hexColor(0x20b2aa);
		case "lightskyblue":
			return hexColor(0x87cefa);
		case "lightslategray":
			return hexColor(0x778899);
		case "lightslategrey":
			return hexColor(0x778899);
		case "lightsteelblue":
			return hexColor(0xb0c4de);
		case "lightyellow":
			return hexColor(0xffffe0);
		case "lime":
			return hexColor(0x00ff00);
		case "limegreen":
			return hexColor(0x32cd32);
		case "linen":
			return hexColor(0xfaf0e6);
		case "magenta":
			return hexColor(0xff00ff);
		case "maroon":
			return hexColor(0x800000);
		case "mediumaquamarine":
			return hexColor(0x66cdaa);
		case "mediumblue":
			return hexColor(0x0000cd);
		case "mediumorchid":
			return hexColor(0xba55d3);
		case "mediumpurple":
			return hexColor(0x9370db);
		case "mediumseagreen":
			return hexColor(0x3cb371);
		case "mediumslateblue":
			return hexColor(0x7b68ee);
		case "mediumspringgreen":
			return hexColor(0x00fa9a);
		case "mediumturquoise":
			return hexColor(0x48d1cc);
		case "mediumvioletred":
			return hexColor(0xc71585);
		case "midnightblue":
			return hexColor(0x191970);
		case "mintcream":
			return hexColor(0xf5fffa);
		case "mistyrose":
			return hexColor(0xffe4e1);
		case "moccasin":
			return hexColor(0xffe4b5);
		case "navajowhite":
			return hexColor(0xffdead);
		case "navy":
			return hexColor(0x000080);
		case "oldlace":
			return hexColor(0xfdf5e6);
		case "olive":
			return hexColor(0x808000);
		case "olivedrab":
			return hexColor(0x6b8e23);
		case "orange":
			return hexColor(0xffa500);
		case "orangered":
			return hexColor(0xff4500);
		case "orchid":
			return hexColor(0xda70d6);
		case "palegoldenrod":
			return hexColor(0xeee8aa);
		case "palegreen":
			return hexColor(0x98fb98);
		case "paleturquoise":
			return hexColor(0xafeeee);
		case "palevioletred":
			return hexColor(0xdb7093);
		case "papayawhip":
			return hexColor(0xffefd5);
		case "peachpuff":
			return hexColor(0xffdab9);
		case "peru":
			return hexColor(0xcd853f);
		case "pink":
			return hexColor(0xffc0cb);
		case "plum":
			return hexColor(0xdda0dd);
		case "powderblue":
			return hexColor(0xb0e0e6);
		case "purple":
			return hexColor(0x800080);
		case "rebeccapurple":
			return hexColor(0x663399);
		case "red":
			return hexColor(0xff0000);
		case "rosybrown":
			return hexColor(0xbc8f8f);
		case "royalblue":
			return hexColor(0x4169e1);
		case "saddlebrown":
			return hexColor(0x8b4513);
		case "salmon":
			return hexColor(0xfa8072);
		case "sandybrown":
			return hexColor(0xf4a460);
		case "seagreen":
			return hexColor(0x2e8b57);
		case "seashell":
			return hexColor(0xfff5ee);
		case "sienna":
			return hexColor(0xa0522d);
		case "silver":
			return hexColor(0xc0c0c0);
		case "skyblue":
			return hexColor(0x87ceeb);
		case "slateblue":
			return hexColor(0x6a5acd);
		case "slategray":
			return hexColor(0x708090);
		case "slategrey":
			return hexColor(0x708090);
		case "snow":
			return hexColor(0xfffafa);
		case "springgreen":
			return hexColor(0x00ff7f);
		case "steelblue":
			return hexColor(0x4682b4);
		case "tan":
			return hexColor(0xd2b48c);
		case "teal":
			return hexColor(0x008080);
		case "thistle":
			return hexColor(0xd8bfd8);
		case "tomato":
			return hexColor(0xff6347);
		case "transparent":
			return {a: 0, r: 0, g: 0, b: 0};
		case "turquoise":
			return hexColor(0x40e0d0);
		case "violet":
			return hexColor(0xee82ee);
		case "wheat":
			return hexColor(0xf5deb3);
		case "white":
			return hexColor(0xffffff);
		case "whitesmoke":
			return hexColor(0xf5f5f5);
		case "yellow":
			return hexColor(0xffff00);
		case "yellowgreen":
			return hexColor(0x9acd32);
		}
	}
	return {a: 0.0, r: 0.0, g: 0.0, b: 0.0};
}

export function handleFileDrop(ele, hdlr)
{
	ele.addEventListener("dragover",(ev)=>{
		ev.preventDefault();
		ev.dataTransfer.dropEffect = "copy";
	});

	ele.addEventListener("drop", (ev)=>{
		ev.preventDefault();
		if (ev.dataTransfer.items)
		{
			var i;
			for (i in ev.dataTransfer.items)
			{
				var item = ev.dataTransfer.items[i];
				if (item.kind === "file")
				{
					hdlr(item.getAsFile());
				}
			}
		}
		else if (ev.dataTransfer.files)
		{
			var i;
			for (i in ev.dataTransfer.files)
			{
				hdlr(ev.dataTransfer.files[i]);
			}
		}
	});
}

export function appendUrl(targetUrl, docUrl)
{
	if (targetUrl.indexOf(":") >= 0)
		return targetUrl;
	var i = docUrl.indexOf("://");
	if (i < 0)
		return targetUrl;
	var j = docUrl.indexOf("/", i + 3);
	if (j < 0)
	{
		j = docUrl.length;
	}

	if (targetUrl.startsWith("/"))
	{
		return docUrl.substring(0, j) + targetUrl;
	}
	i = docUrl.lastIndexOf("/");
	while (true)
	{
		if (targetUrl.startsWith("./"))
		{
			targetUrl = targetUrl.substring(2);
		}
		else if (targetUrl.startsWith("../"))
		{
			targetUrl = targetUrl.substring(3);
			if (i > j)
			{
				docUrl = docUrl.substring(0, i);
				i = docUrl.lastIndexOf("/");
			}
		}
		else
		{
			if (i >= j)
				docUrl = docUrl.substring(0, i);
			return docUrl + "/" + targetUrl;
		}
	}
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
