import * as geometry from "./geometry.js";
import * as kml from "./kml.js";
import * as text from "./text.js";
import * as web from "./web.js";

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
			style.setLineStyle(innerStyle);
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
		case "LookAt":
			container.setLookAt(parseKMLLookAt(node));
			break;
		case "Snippet":
			container.setSnippet(node.textContent);
			break;
		case "#text":
			break;
		default:
			console.log("Unknown node in kml container: "+node.nodeName, node);
			break;
		}
	}
	return container;
}

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
						if (geom)
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
					geom.geometries.push(geomList[i]);
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

export function parseXML(txt)
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
						return parseKMLNode(kmlNode);
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

export async function parseFile(file)
{
	if (file.type == "")
	{
		file.type = web.mimeFromFileName(file.name);
	}

	if (file.type == "application/vnd.google-earth.kml+xml")
	{
		return parseXML(await file.text());
	}
	else
	{
		console.log("Unsupported file type", file.type);
		return null;
	}
}

