import * as data from "./data.js";
import * as media from "./media.js";
import * as text from "./text.js";

export const OSType = {
	Unknown: "Unknown",
	WindowsNT: "WindowsNT",
	WindowsCE: "WindowsCE",
	WindowsSvr: "WindowsSvr",
	Linux_X86_64: "Linux_X86_64",
	Android: "Android",
	iPad: "iPad",
	iPhone: "iPhone",
	Darwin: "Darwin",
	MacOS: "MacOS",
	WindowsNT64: "WindowsNT64",
	ChromeOS: "ChromeOS",
	Linux_i686: "Linux_i686",
	Netcast: "Netcast"
}

export const BrowserType = {
	Unknown: "Unknown",
	IE: "IE",
	Firefox: "Firefox",
	Chrome: "Chrome",
	Safari: "Safari",
	UCBrowser: "UCBrowser",
	CFNetwork: "CFNetwork",
	SogouWeb: "SogouWeb",
	Baidu: "Baidu",
	Semrush: "Semrush",
	Dalvik: "Dalvik",
	Indy: "Indy",
	GoogleBots: "GoogleBots",
	AndroidWV: "AndroidWV",
	Samsung: "Samsung",
	WestWind: "WestWind",
	Yandex: "Yandex",
	Bing: "Bing",
	Masscan: "Masscan",
	PyRequests: "PyRequests",
	Zgrab: "Zgrab",
	Edge: "Edge",
	PyURLLib: "PyURLLib",
	GoogleBotD: "GoogleBotD",
	DotNet: "DotNet",
	WinDiag: "WinDiag",
	SSWR: "SSWR",
	SmartTV: "SmartTV",
	BlexBot: "BlexBot",
	SogouPic: "SogouPic",
	Nutch: "Nutch",
	Yisou: "Yisou",
	Wget: "Wget",
	Scrapy: "Scrapy",
	GoHTTP: "GoHTTP",
	WinHTTP: "WinHTTP",
	NLPProject: "NLPProject",
	ApacheHTTP: "ApacheHTTP",
	BannerDet: "BannerDet",
	NetcraftWeb: "NetcraftWeb",
	NetcraftAG: "NetcraftAG",
	AhrefsBot: "AhrefsBot",
	Mj12Bot: "Mj12Bot",
	NetSysRes: "NetSysRes",
	Whatsapp: "Whatsapp",
	Curl: "Curl",
	GSA: "GSA",
	Facebook: "Facebook",
	Netseen: "Netseen",
	MSNBot: "MSNBot",
	LibwwwPerl: "LibwwwPerl",
	HuaweiBrowser: "HuaweiBrowser",
	Opera: "Opera",
	MiBrowser: "MiBrowser"
}

export function getRequestURLBase()
{
	let url = document.location.href;
	let i = url.indexOf("://");
	let j = url.indexOf("/", i + 3);
	if (j >= 0)
	{
		return url.substring(0, j);
	}
	else
	{
		return url;
	}
}
	
/**
 * @param {string} name
 */
export function getParameterByName(name)
{
	let url = window.location.href;
	name = name.replace(/[[\]]/g, '\\$&');
	let regex = new RegExp('[?&]' + name + '(=([^&#]*)|&|#|$)'),
		results = regex.exec(url);
	if (!results) return null;
	if (!results[2]) return '';
	return decodeURIComponent(results[2].replace(/\+/g, ' '));
}
	
/**
 * @param {string | URL} url
 * @param {(arg0: object) => void} onResultFunc
 */
export function loadJSON(url, onResultFunc)
{
	let xmlhttp = new XMLHttpRequest();
	xmlhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200)
		{
			onResultFunc(JSON.parse(this.responseText));
		}
	};
	xmlhttp.open("GET", url, true);
	xmlhttp.send();
}

/**
 * @param {object | any[]} o
 */
export function buildTable(o)
{
	let name;
	let ret = new Array();
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
		let i = 0;
		let j = o.length;
		let obj;
		while (i < j)
		{
			obj = o[i];
			ret.push("<tr>");
			for (name in o[0])
			{
				let thisObj = obj[name];
				if (thisObj instanceof media.StaticImage)
				{
					ret.push("<td><img src="+text.toAttrText(thisObj.img.src)+"/></td>");
				}
				else if (typeof thisObj == "object")
				{
					ret.push("<td>"+text.toHTMLText(JSON.stringify(thisObj))+"</td>");
				}
				else
				{
					ret.push("<td>"+text.toHTMLText(""+thisObj)+"</td>");
				}
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
			if (typeof o[name] == "object")
			{
				ret.push("<td>"+text.toHTMLText(JSON.stringify(o[name]))+"</td>");
			}
			else
			{
				ret.push("<td>"+text.toHTMLText(""+o[name])+"</td>");
			}
			ret.push("</tr>");
		}
	}
	ret.push("</table>");
	return ret.join("");
}

/**
 * @param {string | Blob} data
 * @param {string} contentType
 * @param {string} fileName
 */
export function openData(data, contentType, fileName)
{
	if (typeof data == "string")
	{
		let ele = document.createElement("a");
		ele.setAttribute('href', 'data:'+encodeURIComponent(contentType)+";charset=utf-8," + encodeURIComponent(data));
		if (fileName)
			ele.setAttribute('download', fileName);
		ele.style.display = 'none';

		document.body.appendChild(ele);
		ele.click();
		document.body.removeChild(ele);
	}
	else if (data instanceof Blob)
	{
		let ele = document.createElement("a");
		ele.setAttribute('href', URL.createObjectURL(data));
		if (fileName)
			ele.setAttribute('download', fileName);
		ele.style.display = 'none';

		document.body.appendChild(ele);
		ele.click();
		document.body.removeChild(ele);
	}
}

/**
 * @param {number} c
 */
function hexColor(c)
{
	let b = c % 256;
	let g = (c >> 8) % 256;
	let r = (c >> 16) % 256;
	return {a: 1.0, r: r / 255, g: g / 255, b: b / 255};
}
/**
 * @param {string} s
 * @param {number} maxVal
 */
function toRatio(s, maxVal)
{
	if (s.endsWith("%"))
		return Number(s.substring(0, s.length - 1)) / 100;
	else
		return Number(s) / maxVal;
}
/**
 * @param {string} c
 */
export function parseCSSColor(c)
{
	let i;
	if (c.startsWith("#"))
	{
		if (c.length == 7)
		{
			return hexColor(parseInt(c.substring(1), 16));
		}
	}
	else if ((i = c.indexOf("(")) >= 0 && c.endsWith(")"))
	{
		let funcName = c.substring(0, i).trim();
		if (funcName == 'rgb')
		{
			let funcCont = c.substring(i + 1, c.length - 1).trim();
			let color = {a: 1.0};
			i = funcCont.indexOf("/");
			if (i >= 0)
			{
				let a = funcCont.substring(i + 1).trim();
				funcCont = funcCont.substring(0, i).trim();
				color.a = toRatio(a, 1);
			}
			let rgb = funcCont.split(" ");
			if (rgb.length == 3)
			{
				color.r = toRatio(rgb[0], 255);
				color.g = toRatio(rgb[1], 255);
				color.b = toRatio(rgb[2], 255);
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
	console.log("Unsupported CSS Color");
	return {a: 0.0, r: 0.0, g: 0.0, b: 0.0};
}

/**
 * @param {{ addEventListener: (arg0: string, arg1: { (ev: any): void; (ev: any): void; }) => void; }} ele
 * @param {(arg0: any) => void} hdlr
 */
export function handleFileDrop(ele, hdlr)
{
	ele.addEventListener("dragover",(/** @type {{ preventDefault: () => void; dataTransfer: { dropEffect: string; }; }} */ ev)=>{
		ev.preventDefault();
		ev.dataTransfer.dropEffect = "copy";
	});

	ele.addEventListener("drop", (/** @type {{ preventDefault: () => void; dataTransfer: { items: { [x: string]: any; }; files: { [x: string]: any; }; }; }} */ ev)=>{
		ev.preventDefault();
		if (ev.dataTransfer.items)
		{
			let i;
			for (i in ev.dataTransfer.items)
			{
				let item = ev.dataTransfer.items[i];
				if (item.kind === "file")
				{
					hdlr(item.getAsFile());
				}
			}
		}
		else if (ev.dataTransfer.files)
		{
			let i;
			for (i in ev.dataTransfer.files)
			{
				hdlr(ev.dataTransfer.files[i]);
			}
		}
	});
}

/**
 * @param {string} targetUrl
 * @param {string} docUrl
 */
export function appendUrl(targetUrl, docUrl)
{
	if (targetUrl.indexOf(":") >= 0)
		return targetUrl;
	let i = docUrl.indexOf("://");
	if (i < 0)
		return targetUrl;
	let j = docUrl.indexOf("/", i + 3);
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

/**
 * @param {string} fileName
 */
export function mimeFromFileName(fileName)
{
	let i;
	i = fileName.lastIndexOf("/");
	if (i >= 0)
		fileName = fileName.substring(i + 1);
	i = fileName.lastIndexOf(".");
	if (i >= 0)
		return mimeFromExt(fileName.substring(i + 1));
	return "application/octet-stream";
}

/**
 * @param {any} ext
 */
export function mimeFromExt(ext)
{
	switch (ext)
	{
	case "3gp": return "video/3gpp";
	case "3g2": return "video/3gpp2";
	case "7z": return "application/x-7z-compressed";
	case "aac": return "audio/aac";
	case "abw": return "application/x-abiword";
	case "aif": return "audio/aiff";
	case "arc": return "application/x-freearc";
	case "asf": return "video/x-ms-asf";
	case "avi": return "video/x-msvideo";
	case "avif": return "image/avif";
	case "azw": return "application/vnd.amazon.ebook";
	case "bin": return "application/octet-stream";
	case "bmp": return "image/bmp";
	case "bz": return "application/x-bzip";
	case "bz2": return "application/x-bzip2";
	case "cda": return "application/x-cdf";
	case "cjs": return "application/node";
	case "csh": return "application/x-csh";
	case "css": return "text/css";
	case "csv": return "text/csv";
	case "dbf": return "application/dbf";
	case "doc": return "application/msword";
	case "docm": return "application/vnd.ms-word.document.macroEnabled.12";
	case "docx": return "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	case "dot": return "application/msword";
	case "dotm": return "application/vnd.ms-word.template.macroEnabled.12";
	case "dotx": return "application/vnd.openxmlformats-officedocument.wordprocessingml.template";
	case "eml": return "message/rfc822";
	case "eot": return "application/vnd.ms-fontobject";
	case "epub": return "application/epub+zip";
	case "exe": return "application/x-exe";
	case "flv": return "video/x-flv";
	case "gif": return "image/gif";
	case "glb": return "model/gltf-binary";
	case "gpx": return "application/gpx+xml";
	case "gz": return "application/gzip";
	case "heic": return "image/heic";
	case "heif": return "image/heif";
	case "htm": return "text/html";
	case "html": return "text/html";
	case "ico": return "image/vnd.microsoft.icon";
	case "ics": return "text/calendar";
	case "igs": return "model/iges";
	case "ipa": return "application/x-ios-app";
	case "iso": return "application/x-iso9660-image";
	case "jar": return "application/java-archive";
	case "jp2": return "image/jpeg2000";
	case "jpeg": return "image/jpeg";
	case "jpg": return "image/jpeg";
	case "js": return "text/javascript"; //RFC 9239
	case "json": return "application/json";
	case "jsonld": return "application/ld+json";
	case "kml": return "application/vnd.google-earth.kml+xml";
	case "kmz": return "application/vnd.google-earth.kmz";
	case "lnk": return "application/x-ms-shortcut";
	case "m1v": return "video/MPV";
	case "m2v": return "video/MPV";
	case "m2p": return "video/MP2P";
	case "m2ts": return "video/MP2T";
	case "m2t": return "video/MP2T";
	case "m3u8": return "application/vnd.apple.mpegurl";
	case "m4a": return "audio/x-m4a";
	case "md": return "text/markdown";
	case "mdb": return "application/vnd.ms-access";
	case "mid": return "audio/midi";
	case "midi": return "audio/midi";
	case "mjs": return "text/javascript"; //RFC 9239
	case "mkv": return "video/x-matroska";
	case "mov": return "video/quicktime";
	case "mp2": return "audio/mpeg";
	case "mp3": return "audio/mpeg";
	case "mp4": return "video/mp4";
	case "mpeg": return "video/mpeg";
	case "mpg": return "video/mpeg";
	case "mpkg": return "application/vnd.apple.installer+xml";
	case "odp": return "application/vnd.oasis.opendocument.presentation";
	case "ods": return "application/vnd.oasis.opendocument.spreadsheet";
	case "odt": return "application/vnd.oasis.opendocument.text";
	case "oga": return "audio/ogg";
	case "ogg": return "application/ogg";
	case "ogv": return "video/ogg";
	case "ogx": return "application/ogg";
	case "opus": return "audio/opus";
	case "otf": return "font/otf";
	case "p10": return "application/pkcs10";
	case "p7c": return "application/pkcs7-mime";
	case "p7m": return "application/pkcs7-mime";
	case "p7s": return "application/pkcs7-signature";
	case "pac": return "application/x-ns-proxy-autoconfig";
	case "pdf": return "application/pdf";
	case "php": return "application/x-httpd-php";
	case "pic": return "image/x-pict";
	case "png": return "image/png";
	case "pnt": return "image/x-maxpaint";
	case "ppt": return "application/vnd.ms-powerpoint";
	case "pptx": return "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	case "rar": return "application/vnd.rar"; //application/x-rar-compressed
	case "rtf": return "application/rtf";
	case "sh": return "application/x-sh";
	case "svg": return "image/svg+xml";
	case "swf": return "application/x-shockwave-flash";
	case "tar": return "application/x-tar";
	case "tga": return "image/x-targa";
	case "tif": return "image/tiff";
	case "tiff": return "image/tiff";
	case "ts": return "video/mp2t";
	case "ttf": return "font/ttf";
	case "txt": return "text/plain";
	case "vsd": return "application/vnd.visio";
	case "wasm": return "application/wasm";
	case "wav": return "audio/wav";
	case "weba": return "audio/webm";
	case "webm": return "video/webm";
	case "webp": return "image/webp";
	case "wma": return "audio/x-ms-wma";
	case "wmv": return "video/x-ms-wmv";
	case "woff": return "font/woff";
	case "woff2": return "font/woff2";
	case "wrl": return "model/vrml";
	case "x3d": return "model/x3d+xml";
	case "x3dv": return "model/x3d+vrml";
	case "x3db": return "model/x3d+binary";
	case "xhtml": return "application/xhtml+xml";
	case "xla": return "application/vnd.ms-excel";
	case "xlam": return "application/vnd.ms-excel.addin.macroEnabled.12";
	case "xls": return "application/vnd.ms-excel";
	case "xlsb": return "application/vnd.ms-excel.sheet.binary.macroEnabled.12";
	case "xlsm": return "application/vnd.ms-excel.sheet.macroEnabled.12";
	case "xlsx": return "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	case "xlt": return "application/vnd.ms-excel";
	case "xltm": return "application/vnd.ms-excel.template.macroEnabled.12";
	case "xltx": return "application/vnd.openxmlformats-officedocument.spreadsheetml.template";
	case "xml": return "application/xml"; //RFC 7303
	case "xul": return "application/vnd.mozilla.xul+xml";
	case "zip": return "application/zip";
	default: return "application/octet-stream";
	}
}

/**
 * @param {string} url
 */
export function getImageInfo(url)
{
	return new Promise(function (resolve, reject) {
        const image = document.createElement('img');
        image.addEventListener('load', function (e) {
			if (e.target)
			{
				resolve({
					// @ts-ignore
					width: e.target.width,
					// @ts-ignore
					height: e.target.height,
				});
			}
			else
			{
				reject();
			}
        });

        image.addEventListener('error', function () {
            reject();
        });

        image.src = url;
    });
}

/**
 * @param {{ [x: string]: any; }} prop
 * @param {{ [x: string]: string; } | null} nameMap
 * @param {any} timeFormat
 */
export function propertiesToHTML(prop, nameMap, timeFormat)
{
	let ret = ["<ul>"];
	let i;
	if (nameMap)
	{
		for (i in prop)
		{
			if (nameMap[i])
			{
				ret.push("<li><b>"+nameMap[i]+": </b>");
				if (prop[i] instanceof data.Timestamp)
				{
					ret.push(text.toHTMLText(prop[i].toString(timeFormat)));
				}
				else if (typeof prop[i] == "object")
				{
					ret.push(propertiesToHTML(prop[i], null, timeFormat));
				}
				else
				{
					ret.push(text.toHTMLText(""+prop[i]));
				}
				ret.push("</li>");
			}
		}
	}
	else
	{
		for (i in prop)
		{
			ret.push("<li><b>"+text.toHTMLText(i)+": </b>");
			if (prop[i] instanceof data.Timestamp)
			{
				ret.push(text.toHTMLText(prop[i].toString(timeFormat)));
			}
			else if (typeof prop[i] == "object")
			{
				ret.push(propertiesToHTML(prop[i], null, timeFormat));
			}
			else
			{
				ret.push(text.toHTMLText(""+prop[i]));
			}
			ret.push("</li>");
		}
	}
	ret.push("</ul>");
	return ret.join("");
}

/**
 * @param {string} name
 */
export async function getCacheSize(name)
{
	const cache = await caches.open(name);
	const keys = await cache.keys();
	let i;
	let size = 0;
	for (i in keys)
	{
		let resp = await cache.match(keys[i]);
		if (resp)
		{
			size += (await resp.arrayBuffer()).byteLength;
		}
	}
	return size;
}

/**
 * @param {string} id
 * @returns {HTMLInputElement}
 */
export function getInputElement(id)
{
	let ele = document.getElementById(id);
	if (ele == null)
		throw new Error("Element with id \""+id+"\" not found");
	if (ele instanceof HTMLInputElement)
		return ele;
	throw new Error("Element with id \""+id+"\" is not an input");
}

/**
 * @param {string} id
 * @returns {HTMLSelectElement}
 */
export function getSelectElement(id)
{
	let ele = document.getElementById(id);
	if (ele == null)
		throw new Error("Element with id \""+id+"\" not found");
	if (ele instanceof HTMLSelectElement)
		return ele;
	throw new Error("Element with id \""+id+"\" is not a select");
}

/**
 * @param {string} id
 * @returns {HTMLButtonElement}
 */
export function getButtonElement(id)
{
	let ele = document.getElementById(id);
	if (ele == null)
		throw new Error("Element with id \""+id+"\" not found");
	if (ele instanceof HTMLButtonElement)
		return ele;
	throw new Error("Element with id \""+id+"\" is not a button");
}

/**
 * @param {string} id
 * @returns {HTMLDivElement}
 */
export function getDivElement(id)
{
	let ele = document.getElementById(id);
	if (ele == null)
		throw new Error("Element with id \""+id+"\" not found");
	if (ele instanceof HTMLDivElement)
		return ele;
	throw new Error("Element with id \""+id+"\" is not a div");
}

/**
 * @param {string} id
 * @returns {HTMLSpanElement}
 */
export function getSpanElement(id)
{
	let ele = document.getElementById(id);
	if (ele == null)
		throw new Error("Element with id \""+id+"\" not found");
	if (ele instanceof HTMLSpanElement)
		return ele;
	throw new Error("Element with id \""+id+"\" is not a span");
}

/**
 * @param {string} id
 * @returns {HTMLCanvasElement}
 */
export function getCanvasElement(id)
{
	let ele = document.getElementById(id);
	if (ele == null)
		throw new Error("Element with id \""+id+"\" not found");
	if (ele instanceof HTMLCanvasElement)
		return ele;
	throw new Error("Element with id \""+id+"\" is not a canvas");
}

/**
 * @param {string} id
 * @returns {HTMLImageElement}
 */
export function getImgElement(id)
{
	let ele = document.getElementById(id);
	if (ele == null)
		throw new Error("Element with id \""+id+"\" not found");
	if (ele instanceof HTMLImageElement)
		return ele;
	throw new Error("Element with id \""+id+"\" is not an img");
}

/**
 * @returns {Promise<{os: String,osVer?:string,browser: String,browserVer?: string,devName?:string}>}
 */
export async function getBrowserInfo()
{
	let ret = parseUserAgent(navigator.userAgent);
	// @ts-ignore
	if (navigator.userAgentData)
	{
		// @ts-ignore
		let data = await navigator.userAgentData.getHighEntropyValues(['fullVersionList', 'platformVersion', 'model']);
		if (data.platformVersion)
			ret.osVer = data.platformVersion;
		if (data.model)
			ret.devName = data.model;
		if (data.fullVersionList)
		{
			console.log(data.fullVersionList);
			let i = 0;
			while (i < data.fullVersionList.length)
			{
				if (ret.browser == BrowserType.Edge)
				{
					if (data.fullVersionList[i].brand == "Microsoft Edge")
						ret.browserVer = data.fullVersionList[i].version;
				}
				else if (ret.browser == BrowserType.Chrome)
				{
					if (data.fullVersionList[i].brand == "Chrome" || data.fullVersionList[i].brand == "Chromium")
						ret.browserVer = data.fullVersionList[i].version;
				}
				else if (ret.browser == BrowserType.Opera)
				{
					if (data.fullVersionList[i].brand == "Opera")
						ret.browserVer = data.fullVersionList[i].version;
				}
				i++;
			}
		}
	}
	if (ret.os == OSType.MacOS)
	{
		if (navigator.platform === "MacIntel" && navigator.maxTouchPoints && navigator.maxTouchPoints > 2)
		{
			ret.os = OSType.iPad;
			ret.osVer = ret.browserVer;
		}
	}
	return ret;
}

/**
 * @param {string} userAgent
 * @returns {{os: String;osVer?: string;browser: String;browserVer?: string;devName?: string;}}
 */
export function parseUserAgent(userAgent)
{
	let ent = {os: OSType.Unknown, browser: BrowserType.Unknown};
	let i;
	if (userAgent == "Microsoft Windows Network Diagnostics")
	{
		ent.browser = BrowserType.WinDiag;
		return ent;
	}
	else if (userAgent.startsWith("West Wind Internet Protocols "))
	{
		ent.browser = BrowserType.WestWind;
		ent.browserVer = userAgent.substring(29);
		return ent;
	}
	else if (userAgent.startsWith("Sogou web spider/"))
	{
		ent.browser = BrowserType.SogouWeb;
		i = userAgent.indexOf('(');
		if (i >= 0)
		{
			ent.browserVer = userAgent.substring(17, i);
		}
		else
		{
			ent.browserVer = userAgent.substring(17);
		}
		return ent;
	}
	else if (userAgent.startsWith("Sogou Pic Spider/"))
	{
		ent.browser = BrowserType.SogouPic;
		i = userAgent.indexOf('(');
		if (i >= 0)
		{
			ent.browserVer = userAgent.substring(17, i);
		}
		else
		{
			ent.browserVer = userAgent.substring(17);
		}
		return ent;
	}
	else if (userAgent == "Nutch Master Test/Dolphin-0.1-Beta")
	{
		ent.browser = BrowserType.Nutch;
		ent.browserVer = "0.1";
		return ent;
	}
	else if (userAgent == "YisouSpider")
	{
		ent.browser = BrowserType.Yisou;
		return ent;
	}
	else if (userAgent.startsWith("HTTP Banner Detection"))
	{
		ent.browser = BrowserType.BannerDet;
		return ent;
	}
	else if (userAgent.startsWith("NetSystemsResearch "))
	{
		ent.browser = BrowserType.NetSysRes;
		return ent;
	}
	if (userAgent.startsWith("\"") && userAgent.endsWith("\""))
	{
		userAgent.substring(1, userAgent.length - 1);
	}
	userAgent = userAgent.trim();
	if (userAgent == "nlpproject.info research")
	{
		ent.browser = BrowserType.NLPProject;
		return ent;
	}

	let strs = [];
	let bst = false;
	let j;
	let c;
	j = 0;
	i = 0;
	while (i < userAgent.length)
	{
		c = userAgent.charAt(i);
		if (c == ' ' && !bst)
		{
			strs.push(userAgent.substring(j, i));
			j = i + 1;
		}
		else if (c == ')' && bst)
		{
			bst = false;
		}
		else if (c == '(')
		{
			bst = true;
		}
		i++;
	}
	if (j < userAgent.length)
	{
		strs.push(userAgent.substring(j));
	}
	i = 0;
	while (i < strs.length)
	{
		if (strs[i].charAt(0) == '(')
		{
			let charCnt = strs[i].length;
			if (strs[i].endsWith(')'))
			{
				strs[i] = strs[i].substring(0, strs[i].length - 1);
			}
			let strs2 = strs[i].substring(1).split(";");
			j = 0;
			let lastIsAndroid = false;
			while (j < strs2.length)
			{
				strs2[j] = strs2[j].trim();
				if (strs2[j].startsWith("MSIE "))
				{
					ent.browser = BrowserType.IE;
					ent.browserVer = strs2[j].substring(5);
				}
				else if (strs2[j].startsWith("Windows NT "))
				{
					ent.os = OSType.WindowsNT;
					ent.osVer = strs2[j].substring(11);
				}
				else if (strs2[j] == "WOW64")
				{
					if (ent.os == OSType.WindowsNT)
					{
						ent.os = OSType.WindowsNT64;
					}
				}
				else if (strs2[j] == "Win64")
				{
					if (ent.os == OSType.WindowsNT)
					{
						ent.os = OSType.WindowsNT64;
					}
				}
				else if (strs2[j] == "iPad")
				{
					ent.os = OSType.iPad;
				}
				else if (ent.os == OSType.Unknown && strs2[j] == "Linux i686")
				{
					ent.os = OSType.Linux_i686;
				}
				else if (ent.os == OSType.Unknown && strs2[j] == "Linux x86_64")
				{
					ent.os = OSType.Linux_X86_64;
				}
				else if (strs2[j] == "Android")
				{
					ent.os = OSType.Android;
				}
				else if (strs2[j] == "wv")
				{
					if (ent.os == OSType.Android)
					{
						ent.browser = BrowserType.AndroidWV;
					}
				}
				else if (strs2[j].startsWith("Android "))
				{
					ent.os = OSType.Android;
					ent.osVer = strs2[j].substring(8);
					lastIsAndroid = true;
				}
				else if (strs2[j].startsWith("CrOS "))
				{
					ent.os = OSType.ChromeOS;
					let k = strs2[j].indexOf(' ', 5);
					ent.osVer = strs2[j].substring(k + 1);
				}
				else if (ent.os == OSType.iPad && strs2[j] == "U")
				{
					ent.os = OSType.Android;
				}
				else if (strs2[j] == "iPhone")
				{
					ent.os = OSType.iPhone;
				}
				else if (strs2[j] == "Macintosh")
				{
					ent.os = OSType.MacOS;
				}
				else if ((ent.os == OSType.iPad || ent.os == OSType.iPhone) && strs2[j].startsWith("CPU OS "))
				{
					let k = strs2[j].indexOf(' ', 7);
					if (k >= 0)
					{
						strs2[j] = strs2[j].substring(0, k);
					}
					strs2[j] = text.replaceAll(strs2[j], '_', '.');
					ent.osVer = strs2[j].substring(7);
				}
				else if (ent.os == OSType.iPhone && strs2[j].startsWith("CPU iPhone OS "))
				{
					let k = strs2[j].indexOf(' ', 14);
					if (k >= 0)
					{
						strs2[j].substring(0, k);
					}
					k = strs2[j].indexOf(" like Mac OS");
					if (k >= 0)
					{
						strs2[j] = strs2[j].substring(0, k);
					}
					strs2[j] = text.replaceAll(strs2[j], '_', '.');
					ent.osVer = strs2[j].substring(14);
				}
				else if (ent.os == OSType.MacOS && strs2[j].startsWith("Intel Mac OS X "))
				{
					let k = strs2[j].indexOf(' ', 15);
					if (k >= 0)
					{
						strs2[j].substring(0, k);
					}
					strs2[j] = text.replaceAll(strs2[j], '_', '.');
					ent.osVer = strs2[j].substring(15);
				}
				else if (strs2[j] == "Trident/7.0")
				{
					ent.browser = BrowserType.IE;
					ent.browserVer = "11.0";
				}
				else if (strs2[j] == "JuziBrowser") //JuziBrowser
				{
				}
				else if (strs2[j] == "SE 2.X MetaSr 1.0") //Sugou Browser
				{
				}
				else if (ent.browser == BrowserType.Unknown && strs2[j] == "Indy Library")
				{
					ent.browser = BrowserType.Indy;
				}
				else if (strs2[j].startsWith("Googlebot/"))
				{
					if (ent.os == OSType.Android)
					{
						ent.browser = BrowserType.GoogleBots;
						ent.browserVer = strs2[j].substring(10);
					}
					else if (ent.os == OSType.Unknown)
					{
						ent.browser = BrowserType.GoogleBotD;
						ent.browserVer = strs2[j].substring(10);
					}
				}
				else if (ent.browser == BrowserType.Unknown && strs2[j].startsWith("Baiduspider-render/"))
				{
					ent.browser = BrowserType.Baidu;
					ent.browserVer = strs2[j].substring(19);
				}
				else if (ent.browser == BrowserType.Unknown && strs2[j].startsWith("SemrushBot/"))
				{
					ent.browser = BrowserType.Semrush;
					ent.browserVer = strs2[j].substring(11);
				}
				else if (ent.browser == BrowserType.Unknown && strs2[j].startsWith("YandexBot/"))
				{
					ent.browser = BrowserType.Yandex;
					ent.browserVer = strs2[j].substring(10);
				}
				else if (ent.browser == BrowserType.Unknown && strs2[j].startsWith("BLEXBot/"))
				{
					ent.browser = BrowserType.BlexBot;
					ent.browserVer = strs2[j].substring(10);
				}
				else if (ent.browser == BrowserType.Unknown && strs2[j].startsWith("bingbot/"))
				{
					ent.browser = BrowserType.Bing;
					ent.browserVer = strs2[j].substring(8);
				}
				else if ((ent.browser == BrowserType.Unknown || ent.browser == BrowserType.IE) && strs2[j].startsWith("MS Web Services Client Protocol "))
				{
					ent.browser = BrowserType.DotNet;
					ent.browserVer = strs2[j].substring(32);
				}
				else if (ent.browser == BrowserType.Unknown && strs2[j].startsWith("WinHttp.WinHttpRequest."))
				{
					ent.browser = BrowserType.WinHTTP;
					ent.browserVer = strs2[j].substring(23);
				}
				else if (ent.browser == BrowserType.Unknown && strs2[j].startsWith("NetcraftSurveyAgent/"))
				{
					ent.browser = BrowserType.NetcraftAG;
					ent.browserVer = strs2[j].substring(20);
				}
				else if (ent.browser == BrowserType.Unknown && strs2[j].startsWith("AhrefsBot/"))
				{
					ent.browser = BrowserType.AhrefsBot;
					ent.browserVer = strs2[j].substring(10);
				}
				else if (ent.browser == BrowserType.Unknown && strs2[j].startsWith("MJ12bot/"))
				{
					ent.browser = BrowserType.Mj12Bot;
					if (strs2[j].charAt(8) == 'v')
					{
						ent.browserVer = strs2[j].substring(9);
					}
					else
					{
						ent.browserVer = strs2[j].substring(8);
					}
				}
				else if (ent.browser == BrowserType.Unknown && strs2[j] == "Netcraft Web Server Survey")
				{
					ent.browser = BrowserType.NetcraftWeb;
				}
				else if (strs2[j] == "Mobile")
				{
				}
				else if (strs2[j] == "rv:")
				{
				}
				else if (lastIsAndroid)
				{
					let k = strs2[j].indexOf(" Build/");
					if (k >= 0)
					{
						strs2[j] = strs2[j].substring(0, k);
					}

					k = strs2[j].indexOf(" MIUI/");
					if (k >= 0)
					{
						strs2[j] = strs2[j].substring(0, k);
					}
					if (strs2[j].startsWith("SAMSUNG SM-"))
					{
						ent.devName = strs2[j].substring(8);
					}
					else if (strs2[j].startsWith("HUAWEI "))
					{
						ent.devName = strs2[j].substring(7);
					}
					else if (strs2[j] == "K")
					{
					}
					else
					{
						ent.devName = strs2[j];
					}
					lastIsAndroid = false;
				}
				j++;
			}
		}
		else if (ent.browser == BrowserType.Unknown && strs[i].startsWith("Firefox/"))
		{
			ent.browser = BrowserType.Firefox;
			ent.browserVer = strs[i].substring(8);
		}
		else if ((ent.browser == BrowserType.Unknown || ent.browser == BrowserType.Safari) && strs[i].startsWith("SamsungBrowser/"))
		{
			ent.browser = BrowserType.Samsung;
			ent.browserVer = strs[i].substring(15);
		}
		else if ((ent.browser == BrowserType.Unknown || ent.browser == BrowserType.Safari) && strs[i].startsWith("GSA/"))
		{
			ent.browser = BrowserType.GSA;
			ent.browserVer = strs[i].substring(4);
		}
		else if ((ent.browser == BrowserType.Unknown || ent.browser == BrowserType.Safari) && strs[i].startsWith("CriOS/"))
		{
			ent.browser = BrowserType.Chrome;
			ent.browserVer = strs[i].substring(6);
		}
		else if ((ent.browser == BrowserType.Unknown || ent.browser == BrowserType.Safari) && strs[i].startsWith("FxiOS/"))
		{
			ent.browser = BrowserType.Firefox;
			ent.browserVer = strs[i].substring(6);
		}
		else if (strs[i].startsWith("Chrome/"))
		{
			if (ent.browser == BrowserType.Unknown || ent.browser == BrowserType.Safari)
			{
				ent.browser = BrowserType.Chrome;
				ent.browserVer = strs[i].substring(7);
			}
			else if (ent.browser == BrowserType.AndroidWV)
			{
				ent.browserVer = strs[i].substring(7);
			}
		}
		else if ((ent.browser == BrowserType.Unknown || ent.browser == BrowserType.Chrome) && strs[i].startsWith("Edge/"))
		{
			ent.browser = BrowserType.Edge;
			ent.browserVer = strs[i].substring(5);
		}
		else if ((ent.browser == BrowserType.Unknown || ent.browser == BrowserType.Chrome) && strs[i].startsWith("Edg/"))
		{
			ent.browser = BrowserType.Edge;
			ent.browserVer = strs[i].substring(4);
		}
		else if ((ent.browser == BrowserType.Unknown || ent.browser == BrowserType.Chrome) && strs[i].startsWith("EdgiOS/"))
		{
			ent.browser = BrowserType.Edge;
			ent.browserVer = strs[i].substring(7);
		}
		else if ((ent.browser == BrowserType.Unknown || ent.browser == BrowserType.Chrome) && strs[i].startsWith("OPT/"))
		{
			ent.browser = BrowserType.Opera;
			ent.browserVer = strs[i].substring(4);
		}
		else if ((ent.browser == BrowserType.Unknown || ent.browser == BrowserType.Chrome) && strs[i].startsWith("OPR/"))
		{
			ent.browser = BrowserType.Opera;
			ent.browserVer = strs[i].substring(4);
		}
		else if ((ent.browser == BrowserType.Unknown || ent.browser == BrowserType.Chrome) && strs[i].startsWith("HuaweiBrowser/"))
		{
			ent.browser = BrowserType.HuaweiBrowser;
			ent.browserVer = strs[i].substring(14);
		}
		else if ((ent.browser == BrowserType.Unknown || ent.browser == BrowserType.Chrome) && strs[i].startsWith("XiaoMi/MiuiBrowser/"))
		{
			ent.browser = BrowserType.MiBrowser;
			ent.browserVer = strs[i].substring(19);
		}
		else if (ent.browser == BrowserType.Unknown && strs[i].startsWith("Safari/"))
		{
			ent.browser = BrowserType.Safari;
			ent.browserVer = strs[i].substring(7);
		}
		else if (ent.browser == BrowserType.Unknown && strs[i].startsWith("UCBrowser/"))
		{
			ent.browser = BrowserType.UCBrowser;
			ent.browserVer = strs[i].substring(10);
		}
		else if ((ent.browser == BrowserType.Unknown || ent.browser == BrowserType.Chrome) && strs[i].startsWith("baidu.sogo.uc.UCBrowser/"))
		{
			ent.browser = BrowserType.UCBrowser;
			ent.browserVer = strs[i].substring(24);
		}
		else if (strs[i].startsWith("UBrowser/"))
		{
		}
		else if (strs[i].startsWith("baiduboxapp/"))
		{
		}
		else if (ent.browser == BrowserType.Unknown && strs[i].startsWith("Dalvik/"))
		{
			ent.browser = BrowserType.Dalvik;
			ent.browserVer = strs[i].substring(7);
		}
		else if (ent.os == OSType.Unknown && strs[i].startsWith("Darwin/"))
		{
			ent.os = OSType.Darwin;
			ent.osVer = strs[i].substring(7);
		}
		else if (ent.os == OSType.Unknown && strs[i].startsWith("SmartTV/"))
		{
			ent.os = OSType.Netcast;
			ent.osVer = strs[i].substring(8);
		}
		else if (ent.browser == BrowserType.Unknown && strs[i].startsWith("CFNetwork/"))
		{
			ent.browser = BrowserType.CFNetwork;
			ent.browserVer = strs[i].substring(10);
		}
		else if (ent.browser == BrowserType.Unknown && strs[i].startsWith("Version/"))
		{
			ent.browser = BrowserType.Safari;
			ent.browserVer = strs[i].substring(8);
		}
		else if (ent.browser == BrowserType.Unknown && strs[i].startsWith("masscan/"))
		{
			ent.browser = BrowserType.Masscan;
			ent.browserVer = strs[i].substring(8);
		}
		else if (ent.browser == BrowserType.Unknown && strs[i].startsWith("zgrab/"))
		{
			ent.browser = BrowserType.Zgrab;
			ent.browserVer = strs[i].substring(6);
		}
		else if (ent.browser == BrowserType.Unknown && strs[i].startsWith("python-requests/"))
		{
			ent.browser = BrowserType.PyRequests;
			ent.browserVer = strs[i].substring(16);
		}
		else if (ent.browser == BrowserType.Unknown && strs[i].startsWith("Python-urllib/"))
		{
			ent.browser = BrowserType.PyURLLib;
			ent.browserVer = strs[i].substring(14);
		}
		else if (ent.browser == BrowserType.Unknown && strs[i].startsWith("Wget/"))
		{
			ent.browser = BrowserType.Wget;
			ent.browserVer = strs[i].substring(5);
		}
		else if (ent.browser == BrowserType.Unknown && strs[i].startsWith("Scrapy/"))
		{
			ent.browser = BrowserType.Scrapy;
			ent.browserVer = strs[i].substring(7);
		}
		else if (ent.browser == BrowserType.Unknown && strs[i].startsWith("Go-http-client/"))
		{
			ent.browser = BrowserType.GoHTTP;
			ent.browserVer = strs[i].substring(15);
		}
		else if (ent.browser == BrowserType.Unknown && strs[i].startsWith("Apache-HttpClient/"))
		{
			ent.browser = BrowserType.ApacheHTTP;
			ent.browserVer = strs[i].substring(18);
		}
		else if (ent.browser == BrowserType.Unknown && strs[i].startsWith("WhatsApp/"))
		{
			ent.browser = BrowserType.Whatsapp;
			ent.browserVer = strs[i].substring(9);
		}
		else if (ent.browser == BrowserType.Unknown && strs[i].startsWith("curl/"))
		{
			ent.browser = BrowserType.Curl;
			ent.browserVer = strs[i].substring(5);
		}
		else if (ent.browser == BrowserType.Unknown && strs[i].startsWith("sswr/"))
		{
			ent.browser = BrowserType.SSWR;
			ent.browserVer = strs[i].substring(5);
		}
		else if (ent.browser == BrowserType.Unknown && strs[i].startsWith("facebookexternalhit/"))
		{
			ent.browser = BrowserType.Facebook;
			ent.browserVer = strs[i].substring(20);
		}
		else if (ent.browser == BrowserType.Unknown && strs[i].startsWith("NetSeen/"))
		{
			ent.browser = BrowserType.Netseen;
			ent.browserVer = strs[i].substring(8);
		}
		else if (ent.browser == BrowserType.Unknown && strs[i].startsWith("msnbot/"))
		{
			ent.browser = BrowserType.MSNBot;
			ent.browserVer = strs[i].substring(7);
		}
		else if (ent.browser == BrowserType.Unknown && strs[i].startsWith("libwww-perl/"))
		{
			ent.browser = BrowserType.LibwwwPerl;
			ent.browserVer = strs[i].substring(12);
		}
		else if (strs[i] == "LBBROWSER")
		{
		}
		i++;
	}
	return ent;
}


export class Dialog
{
	/**
	 * @param {string | HTMLElement} content
	 * @param {{width?:number,height?:number,zIndex?:number,buttonClass?:string,contentClass?:string,buttons?:{name:string,onclick:()=>void}[],margin?:number} | null} options
	 */
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

	getDocBody()
	{
		let d = (top)?top.document:window.document;
		if (d.body.nodeName.toLowerCase() == "frameset")
		{
			/**
			 * @type {Window}
			 */
			let w = window;
			while (w.parent && w.parent.document.body.nodeName.toLowerCase() != "frameset")
			{
				w = w.parent;
			}
			return w.document.body;
		}
		else
		{
			return d.body;
		}
	}

	show()
	{
		if (this.darkColor)
			return;
		let darkColor = document.createElement("div");
		darkColor.style.position = "absolute";
		darkColor.style.width = "100%";
		darkColor.style.height = "100%";
		darkColor.style.backgroundColor = "rgba(0, 0, 0, 0.7)";
		darkColor.style.zIndex = ""+this.options.zIndex;
		darkColor.style.display = "flex";
		darkColor.style.alignItems = "center";
		darkColor.style.justifyContent = "center";
		let docBody = this.getDocBody();
		if (docBody.children.length > 0)
			docBody.insertBefore(darkColor, docBody.children[0]);
		else
			docBody.appendChild(darkColor);
		this.darkColor = darkColor;
		let dialog = document.createElement("div");
		dialog.style.backgroundColor = "#ffffff";
		let width;
		let height;
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

		let content = document.createElement("table");
		content.setAttribute("border", "0");
		content.style.width = width;
		content.style.height = height;
		let row = document.createElement("tr");
		let contentCell = document.createElement("td");
		contentCell.setAttribute("colspan", ""+this.options.buttons.length);
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
		let i;
		let col;
		i = 0;
		while (i < this.options.buttons.length)
		{
			col = document.createElement("td");
			col.style.textAlign = "center";
			col.style.verticalAlign = "middle";
			if (i == 0 && this.options.margin)
			{
				col.style.marginLeft = this.options.margin+"px";
			}
			col.className = this.options.buttonClass;
			col.addEventListener("click", this.options.buttons[i].onclick);
			col.innerText = this.options.buttons[i].name;
			row.append(col);
			i++;
		}
		content.appendChild(row);
		dialog.appendChild(content);
		contentCell.style.display = "inline-block";
		if (this.options.margin)
		{
			let minHeight = row.offsetHeight;
			row.setAttribute("height", ""+(this.options.margin + minHeight));
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
			this.getDocBody().removeChild(this.darkColor);
			this.darkColor = null;
		}
	}

	/**
	 * @param {string} content
	 */
	setContent(content)
	{
		this.content = content;
	}

	/**
	 * @param {string} name
	 * @param {any} value
	 */
	updateOption(name, value)
	{
		this.options[name] = value;
	}

	/**
	 * @param {string | null | undefined} [name]
	 */
	closeButton(name)
	{
		if (name == null) name = "Close";
		return {name: name, onclick: ()=>{this.close();}};
	}
};
