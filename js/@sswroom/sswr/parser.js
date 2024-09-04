import * as cert from "./cert.js";
import { ASN1Util } from "./certutil.js";
import * as data from "./data.js";
import * as geometry from "./geometry.js";
import * as kml from "./kml.js";
import * as media from "./media.js";
import * as text from "./text.js";
import * as web from "./web.js";

/**
 * @param {ChildNode} node
 */
function parseKMLStyle(node)
{
	let subNode;
	let subNode2;
	let subNode3;
	let innerStyle;
	let id = node.id;
	let style = new kml.Style(id);
	for (subNode of node.childNodes)
	{
		switch (subNode.nodeName)
		{
		case "#text":
			break;
		case "IconStyle":
			innerStyle = new kml.IconStyle();
			for (subNode2 of subNode.childNodes)
			{
				switch (subNode2.nodeName)
				{
				case "#text":
					break;
				case "color":
					innerStyle.setColor(subNode2.textContent);
					break;
				case "scale":
					innerStyle.setScale(Number.parseFloat(subNode2.textContent));
					break;
				case "hotSpot":
					{
						let x;
						let y;
						let xunits;
						let yunits;
						for (subNode3 of subNode2.attributes)
						{
							switch (subNode3.name)
							{
							case "x":
								x = Number.parseFloat(subNode3.value);
								break;
							case "y":
								y = Number.parseFloat(subNode3.value);
								break;
							case "xunits":
								xunits = subNode3.value;
								break;
							case "yunits":
								yunits = subNode3.value;
								break;
							default:
								console.log("Unknown attribute in hotSpot", subNode3);
								break;
							}
						}
						if (x != null && y != null && xunits != null && yunits != null)
						{
							innerStyle.setHotSpotX(x, xunits);
							innerStyle.setHotSpotY(y, yunits);
						}
						else
						{
							console.log("hotSopt attributes has missing", x, y, xunits, yunits);
						}
					}
					break;
				case "Icon":
					for (subNode3 of subNode2.childNodes)
					{
						switch (subNode3.nodeName)
						{
						case "#text":
							break;
						case "href":
							{
								let url = subNode3.textContent;
								if (url.startsWith("http://") || url.startsWith("https://"))
								{
									innerStyle.setIconUrl(url);
								}
								else
								{
									console.log("Icon Style.Icon maybe harmful and has to be blocked", url);
								}
							}
							break;
						default:
							console.log("Unknown node in kml IconStyle.Icon", subNode3);
							break;
						}
					}
					break;
				default:
					console.log("Unknown node in kml IconStyle", subNode2);
					break;
				}
			}
			style.setIconStyle(innerStyle);
			break;
		case "LineStyle":
			innerStyle = new kml.LineStyle();
			for (subNode2 of subNode.childNodes)
			{
				switch (subNode2.nodeName)
				{
				case "#text":
					break;
				case "color":
					innerStyle.setColor(subNode2.textContent);
					break;
				case "width":
					innerStyle.setWidth(Number.parseFloat(subNode2.textContent));
					break;
				case "gx:labelVisibility":
					innerStyle.setLabelVisibility(subNode2.textContent == "1");
					break;
				default:
					console.log("Unknown node in kml LineStyle", subNode2);
					break;
				}
			}
			style.setLineStyle(innerStyle);
			break;
		case "LabelStyle":
			innerStyle = new kml.LabelStyle();
			for (subNode2 of subNode.childNodes)
			{
				switch (subNode2.nodeName)
				{
				case "#text":
					break;
				case "color":
					innerStyle.setColor(subNode2.textContent);
					break;
				case "scale":
					innerStyle.setScale(Number.parseFloat(subNode2.textContent));
					break;
				default:
					console.log("Unknown node in kml LabelStyle", subNode2);
					break;
				}
			}
			style.setLabelStyle(innerStyle);
			break;
		case "PolyStyle":
			innerStyle = new kml.PolyStyle();
			for (subNode2 of subNode.childNodes)
			{
				switch (subNode2.nodeName)
				{
				case "#text":
					break;
				case "color":
					innerStyle.setColor(subNode2.textContent);
					break;
				case "outline":
					innerStyle.setOutline(subNode2.textContent == "1");
					break;
				default:
					console.log("Unknown node in kml PolyStyle", subNode2);
					break;
				}
			}
			style.setPolyStyle(innerStyle);
			break;
		default:
			console.log("Unknown node in kml Style", subNode);
			break;
		}
	}
	return style;
}

/**
 * @param {kml.Document | kml.Folder} container
 * @param {ChildNode} kmlNode
 * @param {kml.Container|undefined} doc
 */
function parseKMLContainer(container, kmlNode, doc)
{
	doc = doc || container;
	let node;
	let feature;
	let subNode;
	let subNode2;
	for (node of kmlNode.childNodes)
	{
		switch (node.nodeName)
		{
		case "Style":
			doc.addStyle(parseKMLStyle(node));
			break;
		case "StyleMap":
			{
				let id;
				let normalStyle;
				let highlightStyle;
				for (subNode of node.attributes)
				{
					if (subNode.name == "id")
						id = subNode.value;
					else
						console.log("Unknown attribute in StyleMap", subNode);
				}
				for (subNode of node.childNodes)
				{
					let pairType;
					switch (subNode.nodeName)
					{
					case "#text":
						break;
					case "Pair":
						pairType = null;
						for (subNode2 of subNode.childNodes)
						{
							switch (subNode2.nodeName)
							{
							case "#text":
								break;
							case "key":
								pairType = subNode2.textContent;
								break;
							case "Style":
								if (pairType == "normal")
									normalStyle = parseKMLStyle(subNode2);
								else if (pairType == "highlight")
									highlightStyle = parseKMLStyle(subNode2);
								else
									console.log("Unknown pair type", pairType);
								break;
							case "styleUrl":
								if (pairType == "normal")
									normalStyle = doc.getStyleById(subNode2.textContent.substring(1));
								else if (pairType == "highlight")
									highlightStyle = doc.getStyleById(subNode2.textContent.substring(1));
								else
									console.log("Unknown pair type", pairType);
								break;
							default:
								console.log("Unknown node in StyleMap.Pair", subNode2);
								break;
							}
						}
						break;
					default:
						console.log("Unknown node in StyleMap", subNode);
						break;
					}
				}
				if (id && normalStyle && highlightStyle)
				{
					doc.addStyleMap(new kml.StyleMap(id, normalStyle, highlightStyle));
				}
				else
				{
					console.log("Missing items in StyleMap", id, normalStyle, highlightStyle);
				}
			}
			break;
		}
	}
	for (node of kmlNode.childNodes)
	{
		switch (node.nodeName)
		{
		case "description":
			container.setDescription(node.textContent);
			break;
		case "name":
			container.setName(node.textContent);
			break;
		case "open":
			container.setOpen(node.textContent == "1");
			break;
		case "visibility":
			container.setVisibility(node.textContent == "1");
			break;
		case "atom:author":
			for (subNode of node.childNodes)
			{
				switch (subNode.nodeName)
				{
				case "#text":
					{
						let txt = subNode.textContent.trim();
						if (txt.length > 0)
						{
							if (container.author)
								container.setAuthor(container.author + " " + txt);
							else
								container.setAuthor(txt);
						}						
					}
					break;
				case "atom:name":
					container.setAuthorName(subNode.textContent);
					break;
				default:
					console.log("Unknown node in kml atom:author: "+subNode.nodeName, subNode);
					break;
				}
			}
			break;
		case "atom:link":
			for (subNode of node.attributes)
			{
				switch (subNode.name)
				{
				case "href":
					container.setLink(subNode.value);
					break;
				default:
					console.log("Unknown attribute in kml atom:link: "+subNode.nodeName, subNode);
					break;
				}
			}
			break;
		case "Style":
			break;
		case "StyleMap":
			break;
		case "Folder":
		case "Placemark":
			feature = parseKMLNode(node, doc || container);
			if (feature)
				container.addFeature(feature);
			break;
		case "NetworkLink":
			feature = parseKMLNetworkLink(node, doc || container);
			if (feature)
				container.addFeature(feature);
			break;
		case "LookAt":
			container.setLookAt(parseKMLLookAt(node));
			break;
		case "Snippet":
			container.setSnippet(node.textContent);
			break;
		case "#text":
			break;
		case "Schema":
			break;
		default:
			console.log("Unknown node in kml container: "+node.nodeName, node);
			break;
		}
	}
	return container;
}

/**
 * @param {ChildNode} kmlNode
 * @param {kml.Container|undefined} doc
 */
function parseKMLPlacemark(kmlNode, doc)
{
	let node;
	let name;
	let description;
	let snippet;
	let geom;
	let visibility;
	let style;
	for (node of kmlNode.childNodes)
	{
		switch (node.nodeName)
		{
		case "#text":
			break;
		case "name":
			name = node.textContent;
			break;
		case "description":
			description = node.textContent;
			break;
		case "snippet":
			snippet = node.textContent;
			break;
		case "Snippet":
			snippet = node.textContent;
			break;
		case "styleUrl":
			if (node.textContent.startsWith("#") && doc)
			{
				style = doc.getStyleById(node.textContent.substring(1));
				if (style == null)
				{
					console.log("styleUrl not found in kml placemark", node.textContent);
				}
			}
			else
			{
				console.log("Unknown styleUrl in kml placemark", node.textContent);
			}
			break;
		case "visibility":
			visibility = (node.textContent == "1");
			break;
		case "Point":
		case "LineString":
		case "MultiGeometry":
			geom = parseKMLGeometry(node);
			break;
		case "ExtendedData":
			break;
		default:
			console.log("Unknown node in kml placemark: "+node.nodeName, node);
			break;
		}
	}
	if (geom)
	{
		let feature = new kml.Placemark(geom);
		if (name)
			feature.setName(name);
		if (description)
			feature.setDescription(description);
		if (snippet)
			feature.setSnippet(snippet);
		if (style)
			feature.setStyle(style);
		if (visibility != null)
			feature.setVisibility(visibility);
		return feature;
	}
	return null;
}

/**
 * @param {{ nodeName: any; childNodes: any; }} kmlNode
 */
function parseKMLGeometry(kmlNode)
{
	let subNode;
	let subNode2;
	let geomList;
	let geom;
	let coords;
	let i;
	switch (kmlNode.nodeName)
	{
	case "Point":
		for (subNode of kmlNode.childNodes)
		{
			switch (subNode.nodeName)
			{
			case "#text":
				break;
			case "extrude":
				break;
			case "altitudeMode":
				break;
			case "tessellate":
				break;
			case "coordinates":
				coords = subNode.textContent.split(",");
				return new geometry.Point(4326, text.arrayToNumbers(coords));
			default:
				console.log("Unknown node in kml Point: "+subNode.nodeName, subNode);
				break;
			}
		}
		break;
	case "LineString":
		for (subNode of kmlNode.childNodes)
		{
			switch (subNode.nodeName)
			{
			case "#text":
				break;
			case "extrude":
				break;
			case "altitudeMode":
				break;
			case "tessellate":
				break;
			case "coordinates":
				{
					let pts = subNode.textContent.split(/\s+/);
					let ls = [];
					let i;
					let arr;
					for (i in pts)
					{
						arr = pts[i].split(",");
						if (arr.length >= 2)
						{
							ls.push(text.arrayToNumbers(arr));
						}
					}
					return new geometry.LineString(4326, ls);
				}
			default:
				console.log("Unknown node in kml LineString: "+subNode.nodeName, subNode);
				break;
			}
		}
		break;
	case "LinearRing":
		for (subNode of kmlNode.childNodes)
		{
			switch (subNode.nodeName)
			{
			case "#text":
				break;
			case "extrude":
				break;
			case "altitudeMode":
				break;
			case "tessellate":
				break;
			case "coordinates":
				{
					let pts = subNode.textContent.split(/\s+/);
					let ls = [];
					let i;
					let arr;
					for (i in pts)
					{
						arr = pts[i].split(",");
						if (arr.length >= 2)
						{
							ls.push(text.arrayToNumbers(arr));
						}
					}
					return new geometry.LinearRing(4326, ls);
				}
			default:
				console.log("Unknown node in kml LinearRing: "+subNode.nodeName, subNode);
				break;
			}
		}
		break;
	case "Polygon":
		geomList = [];
		for (subNode of kmlNode.childNodes)
		{
			switch (subNode.nodeName)
			{
			case "#text":
				break;
			case "extrude":
				break;
			case "altitudeMode":
				break;
			case "tessellate":
				break;
			case "outerBoundaryIs":
				for (subNode2 of subNode.childNodes)
				{
					switch (subNode2.nodeName)
					{
					case "#text":
						break;
					case "LinearRing":
						geom = parseKMLGeometry(subNode2);
						if (geom && (geom instanceof geometry.LinearRing))
							geomList.push(geom);
						break;
					default:
						console.log("Unknown node in kml Polygon.outerBoundaryIs", subNode2);
						break;
					}
				}
				break;
			default:
				console.log("Unknown node in kml Polygon: "+subNode.nodeName, subNode);
				break;
			}
		}
		if (geomList.length > 0)
		{
			geom = new geometry.Polygon(4326);
			for (i in geomList)
			{
				geom.geometries.push(geomList[i]);
			}
			return geom; 
		}
		break;
	case "MultiGeometry":
		geomList = [];
		for (subNode of kmlNode.childNodes)
		{
			switch (subNode.nodeName)
			{
			case "#text":
				break;
			default:
				geom = parseKMLGeometry(subNode);
				if (geom)
				{
					geomList.push(geom);
				}
				else
				{
					console.log("Unknown node in kml MultiGeometry", subNode);
				}
				break;
			}
		}
		if (geomList.length > 0)
		{
			if (geomList[0] instanceof geometry.Polygon && (geomList.length == 1 || geomList[1] instanceof geometry.Polygon))
			{
				geom = new geometry.MultiPolygon(4326);
				for (i in geomList)
				{
					let g = geomList[i];
					if (g instanceof geometry.Polygon)
					{
						geom.geometries.push(g);
					}
				}
				return geom; 
			}
			else if (geomList[0] instanceof geometry.LineString && (geomList.length == 1 || geomList[1] instanceof geometry.LineString))
			{
				geom = new geometry.Polyline(4326);
				for (i in geomList)
				{
					let g = geomList[i];
					if (g instanceof geometry.LineString)
						geom.geometries.push(g);
				}
				return geom; 
			}
			else
			{
				geom = new geometry.GeometryCollection(4326);
				for (i in geomList)
				{
					geom.geometries.push(geomList[i]);
				}
				return geom; 
			}
		}
		break;
	default:
		console.log("Unknown node in kml geometry", kmlNode);
		break;
	}
	return null;
}

/**
 * @param {{ childNodes: any; }} kmlNode
 * @param {any} doc
 */
function parseKMLNetworkLink(kmlNode, doc)
{
	let name;
	let description;
	let open;
	let refreshVisibility;
	let flyToView;
	let linkHref;
	let refreshMode;
	let refreshInterval;
	let viewRefreshMode;

	let feature;
	let node;
	let subNode;
	for (node of kmlNode.childNodes)
	{
		switch (node.nodeName)
		{
		case "#text":
			break;
		case "name":
			name = node.textContent;
			break;
		case "description":
			description = node.textContent.trim();
			break;
		case "open":
			open = (node.textContent == "1");
			break;
		case "refreshVisibility":
			refreshVisibility = (node.textContent == "1");
			break;
		case "flyToView":
			flyToView = (node.textContent == "1");
			break;
		case "Link":
			for (subNode of node.childNodes)
			{
				switch (subNode.nodeName)
				{
				case "#comment":
					break;
				case "#text":
					break;
				case "href":
					linkHref = subNode.textContent;
					break;
				case "refreshMode":
					refreshMode = subNode.textContent;
					break;
				case "refreshInterval":
					refreshInterval = subNode.textContent;
					break;
				case "viewRefreshMode":
					viewRefreshMode = subNode.textContent;
					break;
				default:
					console.log("Unknown node in kml NetworkLink.Link", subNode);
					break;
				}
			}
			break;
		default:
			console.log("Unknown node in kml NetworkLink", node);
			break;
		}
	}
	if (linkHref)
	{
		feature = new kml.NetworkLink(linkHref);
		if (name)
			feature.setName(name);
		if (description)
			feature.setDescription(description);
		if (open != null)
			feature.setOpen(open);
		if (refreshVisibility != null)
			feature.setRefreshVisibility(refreshVisibility);
		if (flyToView != null)
			feature.setFlyToView(flyToView);
		if (refreshMode)
			feature.setRefreshMode(refreshMode);
		if (refreshInterval)
			feature.setRefreshInterval(Number.parseFloat(refreshInterval));
		if (viewRefreshMode)
			feature.setViewRefreshMode(viewRefreshMode);
		return feature;
	}
	return null;
}

/**
 * @param {{ childNodes: any; }} kmlNode
 */
function parseKMLLookAt(kmlNode)
{
	let longitude;
	let latitude;
	let altitude;
	let range;
	let heading;
	let tilt;
	let altitudeMode;
	let node;
	for (node of kmlNode.childNodes)
	{
		switch (node.nodeName)
		{
		case "#text":
			break;
		case "#comment":
			break;
		case "longitude":
			longitude = Number.parseFloat(node.textContent);
			break;
		case "latitude":
			latitude = Number.parseFloat(node.textContent);
			break;
		case "altitude":
			altitude = Number.parseFloat(node.textContent);
			break;
		case "altitudeMode":
			altitudeMode = node.textContent;
			break;
		case "range":
			range = Number.parseFloat(node.textContent);
			break;
		case "heading":
			heading = Number.parseFloat(node.textContent);
			break;
		case "tilt":
			tilt = Number.parseFloat(node.textContent);
			break;
		default:
			console.log("Unknown node in kml LookAt", node);
			break;
		}
	}
	if (longitude != null && latitude != null && altitude != null && range != null)
	{
		let lookAt = new kml.LookAt(longitude, latitude, altitude, range);
		if (altitudeMode)
		{
			lookAt.setAltitudeMode(altitudeMode);
		}
		if (heading != null)
			lookAt.setHeading(heading);
		if (tilt != null)
			lookAt.setTilt(tilt);
		return lookAt;
	}
	else
	{
		console.log("Some fields in LookAt are missing", kmlNode);
		return null;
	}
}

/**
 * @param {ChildNode} kmlNode
 * @param {kml.Container|undefined} [doc]
 */
function parseKMLNode(kmlNode, doc)
{
	switch (kmlNode.nodeName)
	{
	case "Folder":
		return parseKMLContainer(new kml.Folder(), kmlNode, doc);
	case "Document":
		return parseKMLContainer(new kml.Document(), kmlNode, doc);
	case "Placemark":
		return parseKMLPlacemark(kmlNode, doc);
	default:
		console.log("Unknown kml node: "+kmlNode.nodeName, kmlNode);
		return null;
	}
}

/**
 * @param {data.ByteReader} reader
 * @param {string} sourceName
 */
async function parseJpg(reader, sourceName)
{
	if (!(reader instanceof data.ByteReader))
		return null;
	if (reader.getLength() < 2)
		return null;
	if (reader.readUInt8(0) != 0xff || reader.readUInt8(1) != 0xd8)
		return null;
	let ofst = 2;
	let ret = false;
	let j;
	let exif;
	while (true)
	{
		if (ofst + 4 > reader.getLength())
		{
			ret = false;
			break;
		}
		if (reader.readUInt8(ofst + 0) != 0xff)
		{
			ret = false;
			break;
		}
		if (reader.readUInt8(ofst + 1) == 0xdb)
		{
			ret = true;
			break;
		}

		j = ((reader.readUInt8(ofst + 2) << 8) | reader.readUInt8(ofst + 3)) - 2;
		if (reader.readUInt8(ofst + 1) == 0xe1)
		{
			let t = reader.readUTF8Z(ofst + 4, 14);
			if (t == "Exif")
			{
				let lsb;
				if (reader.readUTF8(ofst + 10, 2) == "II")
				{
					lsb = true;
				}
				else if (reader.readUTF8(ofst + 10, 2) == "MM")
				{
					lsb = false;
				}
				else
				{
					console.log("JPG Exif unknown byte order", reader.readUTF8(ofst + 10, 2));
					ret = false;
					break;
				}
				if (reader.readUInt16(ofst + 12, lsb) != 42)
				{
					console.log("JPG Exif not 42", reader.readUInt16(ofst + 12, lsb));
					ret = false;
					break;
				}
				if (reader.readUInt32(ofst + 14, lsb) != 8)
				{
					console.log("JPG Exif not 8", reader.readUInt32(ofst + 14, lsb));
					ret = false;
					break;
				}
				exif = media.EXIFData.parseIFD(reader, ofst + 18, lsb, null, ofst + 10);
				if (exif == null)
				{
					console.log("Error in parsing EXIF");
				}
				ofst += j + 4;
			}
			else if (t == "FLIR")
			{
/*				if (buff[4] == 0 && buff[5] == 1)
				{
					if (flirMstm == 0 && buff[6] == 0)
					{
						flirMaxSegm = buff[7];
						NEW_CLASS(flirMstm, IO::MemoryStream());
						flirCurrSegm = buff[6];
						Data::ByteBuffer tagBuff(j);
						fd->GetRealData(ofst + 4, j, tagBuff);
						flirMstm->Write(&tagBuff[8], j - 8);
					}
					else if (flirMstm && buff[6] == (flirCurrSegm + 1))
					{
						flirCurrSegm = (UInt8)(flirCurrSegm + 1);
						Data::ByteBuffer tagBuff(j);
						fd->GetRealData(ofst + 4, j, tagBuff);
						flirMstm->Write(&tagBuff[8], j - 8);
					}
				}*/
				ofst += j + 4;
			}
			else
			{
				console.log("Unknown type for e1", t);
				ofst += j + 4;
			}
		}
		else if (reader.readUInt8(ofst + 1) == 0xe2)
		{
			let t = reader.readUTF8Z(ofst + 4);
			if (t == "ICC_PROFILE")
			{
				let iccBuff = reader.getArrayBuffer(ofst + 4 + 14, j - 14);
				/*
				NotNullPtr<Media::ICCProfile> icc;
				if (Media::ICCProfile::Parse(tagBuff.SubArray(14, j - 14)).SetTo(icc))
				{
					icc->SetToColorProfile(img->info.color);
					icc.Delete();
				}*/
				console.log("ICC Profile found");
			}
			else
			{
				console.log("Unknown type for e2", t);
			}
			ofst += j + 4;
		}
		else
		{
			ofst += j + 4;
		}
	}
	if (ret)
	{
		let buff = reader.getArrayBuffer();
		let b = new Blob([buff], {type: "image/jpeg"});
		let img = await media.loadImageFromBlob(b);
		let simg = new media.StaticImage(img, sourceName, "image/jpeg");
		if (exif)
			simg.setExif(exif);
		return simg;
	}
	return null;
}

/**
 * @param {data.ByteReader} reader
 * @param {string} sourceName
 */
async function parseWebp(reader, sourceName)
{
	if (!(reader instanceof data.ByteReader))
		return null;
	if (reader.getLength() < 20)
		return null;
	if (reader.readUInt32(0, true) != 0x46464952 || reader.readUInt32(8, true) != 0x50424557 || reader.readUInt32(4, true) + 8 != reader.getLength())
		return null;
	let buff = reader.getArrayBuffer();
	let b = new Blob([buff], {type: "image/webp"});
	let img = await media.loadImageFromBlob(b);
	let simg = new media.StaticImage(img, sourceName, "image/webp");
	return simg;
}

/**
 * @param {data.ByteReader} reader
 * @param {string} fileName
 * @param {string} mime
 */
function parseX509(reader, fileName, mime)
{
	if (!(reader instanceof data.ByteReader))
		return null;
	let initTxt;
	let initOfst = 0;
	if (reader.readUInt8(0) == 0xef && reader.readUInt8(1) == 0xbb && reader.readUInt8(2) == 0xbf)
	{
		initOfst = 3;
	}
	initTxt = reader.readUTF8(initOfst, 5);
	if (initTxt == "-----")
	{
		let files = [];
		let lines = text.splitLines(reader.readUTF8(initOfst, reader.getLength() - initOfst));
		let i = 0;
		let j = lines.length;
		while (i < j)
		{
			if (lines[i] == "-----BEGIN CERTIFICATE-----")
			{
				let b64 = [];
				i++;
				while (true)
				{
					if (i >= j)
						return null;
					if (lines[i] == "-----END CERTIFICATE-----")
					{
						break;
					}
					else
					{
						b64.push(lines[i]);
					}
					i++;
				}
				files.push(new cert.X509Cert(fileName, new text.Base64Enc().decodeBin(b64.join("")).buffer));
			}
			else if (lines[i] == "-----BEGIN RSA PRIVATE KEY-----")
			{
				let b64 = [];
				let enc = false;
				i++;
				while (true)
				{
					if (i >= j)
						return null;
					if (lines[i] == "-----END RSA PRIVATE KEY-----")
					{
						break;
					}
					else if (lines[i].startsWith("Proc-Type:"))
					{
						enc = true;
					}
					else
					{
						b64.push(lines[i]);
					}
					i++;
				}
				files.push(new cert.X509Key(fileName, new text.Base64Enc().decodeBin(b64.join("")).buffer, cert.KeyType.RSA));
			}
			else if (lines[i] == "-----BEGIN DSA PRIVATE KEY-----")
			{
				let b64 = [];
				i++;
				while (true)
				{
					if (i >= j)
						return null;
					if (lines[i] == "-----END DSA PRIVATE KEY-----")
					{
						break;
					}
					else
					{
						b64.push(lines[i]);
					}
					i++;
				}
				files.push(new cert.X509Key(fileName, new text.Base64Enc().decodeBin(b64.join("")).buffer, cert.KeyType.DSA));
			}
			else if (lines[i] == "-----BEGIN EC PRIVATE KEY-----")
			{
				let b64 = [];
				i++;
				while (true)
				{
					if (i >= j)
						return null;
					if (lines[i] == "-----END EC PRIVATE KEY-----")
					{
						break;
					}
					else
					{
						b64.push(lines[i]);
					}
					i++;
				}
				files.push(new cert.X509Key(fileName, new text.Base64Enc().decodeBin(b64.join("")).buffer, cert.KeyType.ECDSA));
			}
			else if (lines[i] == "-----BEGIN PRIVATE KEY-----")
			{
				let b64 = [];
				i++;
				while (true)
				{
					if (i >= j)
						return null;
					if (lines[i] == "-----END PRIVATE KEY-----")
					{
						break;
					}
					else
					{
						b64.push(lines[i]);
					}
					i++;
				}
				files.push(new cert.X509PrivKey(fileName, new text.Base64Enc().decodeBin(b64.join("")).buffer));
			}
			else if (lines[i] == "-----BEGIN PUBLIC KEY-----")
			{
				let b64 = [];
				i++;
				while (true)
				{
					if (i >= j)
						return null;
					if (lines[i] == "-----END PUBLIC KEY-----")
					{
						break;
					}
					else
					{
						b64.push(lines[i]);
					}
					i++;
				}
				files.push(new cert.X509PubKey(fileName, new text.Base64Enc().decodeBin(b64.join("")).buffer));
			}
			else if (lines[i] == "-----BEGIN CERTIFICATE REQUEST-----")
			{
				let b64 = [];
				i++;
				while (true)
				{
					if (i >= j)
						return null;
					if (lines[i] == "-----END CERTIFICATE REQUEST-----")
					{
						break;
					}
					else
					{
						b64.push(lines[i]);
					}
					i++;
				}
				files.push(new cert.X509CertReq(fileName, new text.Base64Enc().decodeBin(b64.join("")).buffer));
			}
			else if (lines[i] == "-----BEGIN NEW CERTIFICATE REQUEST-----")
			{
				let b64 = [];
				i++;
				while (true)
				{
					if (i >= j)
						return null;
					if (lines[i] == "-----END NEW CERTIFICATE REQUEST-----")
					{
						break;
					}
					else
					{
						b64.push(lines[i]);
					}
					i++;
				}
				files.push(new cert.X509CertReq(fileName, new text.Base64Enc().decodeBin(b64.join("")).buffer));
			}
			i++;
		}
		if (files.length == 1)
		{
			return files[0];
		}
		else if (files.length > 1)
		{
			let f = files[0];
			if (f instanceof cert.X509Cert)
			{
				let fileList = new cert.X509FileList(fileName, f);
				let i = 1;
				while (i < files.length)
				{
					fileList.addFile(files[i]);
					i++;
				}
				return fileList;
			}
		}
		console.log("Unsupported file", fileName, mime);
	}
	else if (reader.readUInt8(0) == 0x30 && ASN1Util.pduIsValid(reader, 0, reader.getLength()))
	{
		fileName = fileName.toUpperCase();
		console.log(mime);
		if (mime == "application/x-pkcs12" || fileName.endsWith(".P12") || fileName.endsWith(".PFX"))
		{
			return new cert.X509PKCS12(fileName, reader.getArrayBuffer());
		}
		else if (mime == "application/x-x509-ca-cert" || mime == "application/x-x509-user-cert" || mime == "application/pkix-cert" || fileName.endsWith(".DER") || fileName.endsWith(".CER") || fileName.endsWith(".CRT"))
		{
			return new cert.X509Cert(fileName, reader.getArrayBuffer());
		}
		else if (mime == "application/x-pkcs7-certificates" || fileName.endsWith(".P7B") || fileName.endsWith(".P7S"))
		{
			return new cert.X509PKCS7(fileName, reader.getArrayBuffer());
		}
		else if (mime == "application/pkix-crl" || fileName.endsWith(".CRL"))
		{
			return new cert.X509CRL(fileName, reader.getArrayBuffer());
		}
	}
	return null;
}

/**
 * @param {string} txt
 * @param {string} sourceName
 */
export function parseXML(txt, sourceName)
{
	let parser = new DOMParser();
	let xmlDoc = parser.parseFromString(txt, "application/xml");
	let xmlRoot;
	for (xmlRoot of xmlDoc.childNodes)
	{
		switch (xmlRoot.nodeName)
		{
		case "#comment":
			break;
		default:
			if (xmlRoot.nodeName == "kml")
			{
				let kmlNode;
				for (kmlNode of xmlRoot.childNodes)
				{
					if (kmlNode.nodeName != "#text")
					{
						let feature = parseKMLNode(kmlNode);
						if (feature)
						{
							return new kml.KMLFile(feature, sourceName);
						}
						else
						{
							return null;
						}
					}
				}
			}
			else
			{
				console.log("Xml file format not supported", xmlRoot.nodeName);
			}
			break;
		}
	}
	return null;

}

/**
 * @param {File | Response} file
 */
export async function parseFile(file)
{
	let t = file.type;
	if (t == "")
	{
		t = web.mimeFromFileName(file.name);
	}

	if (t == "application/vnd.google-earth.kml+xml")
	{
		return parseXML(await file.text(), file.name);
	}
	else
	{
		let view = new data.ByteReader(await file.arrayBuffer());
		let obj;
		if (obj = await parseJpg(view, file.name)) return obj;
		if (obj = await parseWebp(view, file.name)) return obj;
		if (obj = parseX509(view, file.name, t)) return obj;
		console.log("Unsupported file type", t);
		return null;
	}
}

