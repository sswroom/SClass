import * as geometry from "./geometry.js";
import * as text from "./text.js";
import * as web from "./web.js";

export const AltitudeMode = {
	ClampToGround: "clampToGround",
	RelativeToGround: "relativeToGround",
	Absolute: "absolute"
};

export const DisplayMode = {
	Default: "default",
	Hide: "hide"
};

export const ListItemType = {
	Check: "check",
	RadioFolder: "radioFolder",
	CheckOffOnly: "checkOffOnly",
	CheckHideChildren: "checkHideChildren"
};

export const ItemIconMode = {
	Open: "open",
	Closed: "closed",
	Error: "error",
	Fetching0: "fetching0",
	Fetching1: "fetching1",
	Fetching2: "fetching2"
};

export class Element
{
}

export class LookAt extends Element
{
	constructor(longitude, latitude, altitude, range)
	{
		super();
		this.longitude = longitude;
		this.latitude = latitude;
		this.altitude = altitude;
		this.range = range;
		this.tilt = 0;
		this.heading = 0;
		this.altitudeMode = null;
	}

	setHeading(heading)
	{
		this.heading = heading;
	}

	setTilt(tilt)
	{
		this.tilt = tilt;
	}

	setAltitudeMode(altitudeMode)
	{
		this.altitudeMode = altitudeMode;
	}

	appendOuterXML(strs, level)
	{
		strs.push("\t".repeat(level)+"<LookAt>");
		strs.push("\t".repeat(level + 1)+"<longitude>"+(this.longitude-0)+"</longitude>");
		strs.push("\t".repeat(level + 1)+"<latitude>"+(this.latitude-0)+"</latitude>");
		strs.push("\t".repeat(level + 1)+"<altitude>"+(this.altitude-0)+"</altitude>");
		strs.push("\t".repeat(level + 1)+"<heading>"+(this.heading-0)+"</heading>");
		strs.push("\t".repeat(level + 1)+"<tilt>"+(this.tilt-0)+"</tilt>");
		strs.push("\t".repeat(level + 1)+"<range>"+(this.range-0)+"</range>");
		if (this.altitudeMode)
			strs.push("\t".repeat(level + 1)+"<altitudeMode>"+text.toXMLText(this.altitudeMode)+"</altitudeMode>");
		strs.push("\t".repeat(level)+"</LookAt>");
	}
};


export class ColorStyle extends Element
{
	constructor()
	{
		super();
	}

	setRandomColor(randomColor)
	{
		this.randomColor = randomColor;
	}

	fromARGB(a, r, g, b)
	{
		if (a < 0) a = 0;
		else if (a > 1.0) a = 1.0;
		if (r < 0) r = 0;
		else if (r > 1.0) r = 1.0;
		if (g < 0) g = 0;
		else if (g > 1.0) g = 1.0;
		if (b < 0) b = 0;
		else if (b > 1.0) b = 1.0;
		a = Math.round(a * 255).toString(16);
		r = Math.round(r * 255).toString(16);
		g = Math.round(g * 255).toString(16);
		b = Math.round(b * 255).toString(16);
		if (a.length < 2) a = "0"+a;
		if (r.length < 2) r = "0"+r;
		if (g.length < 2) g = "0"+g;
		if (b.length < 2) b = "0"+b;
		this.color = a+b+g+r;
	}

	fromCSSColor(color)
	{
		var c = web.parseCSSColor(color);
		this.fromARGB(c.a, c.r, c.g, c.b);
	}

	sameColor(c)
	{
		if (this.randomColor != c.randomColor)
			return false;
		if (this.color != c.color)
			return false;
		return true;
	}

	appendInnerXML(strs, level)
	{
		if (this.color)
			strs.push("\t".repeat(level)+"<color>"+this.color+"</color>");
		if (this.randomColor != null)
			strs.push("\t".repeat(level)+"<colorMode>"+((this.randomColor)?"random":"normal")+"</colorMode>");
	}
};

export class IconStyle extends ColorStyle
{
	constructor()
	{
		super();
	}

	setScale(scale)
	{
		this.scale = scale;
	}

	setHeading(heading)
	{
		this.heading = heading;
	}

	setIconUrl(iconUrl)
	{
		this.iconUrl = iconUrl;
	}

	setHotSpotX(hotSpotX)
	{
		this.hotSpotX = hotSpotX;
	}

	setHotSpotY(hotSpotY)
	{
		this.hotSpotY = hotSpotY;
	}

	appendOuterXML(strs, level)
	{
		strs.push("\t".repeat(level)+"<IconStyle>");
		this.appendInnerXML(strs, level + 1);
		if (this.scale)
			strs.push("\t".repeat(level + 1)+"<scale>"+this.scale+"</scale>");
		if (this.heading != null)
			strs.push("\t".repeat(level + 1)+"<heading>"+this.heading+"</heading>");
		if (this.iconUrl)
		{
			strs.push("\t".repeat(level + 1)+"<Icon>");
			strs.push("\t".repeat(level + 2)+"<href>"+text.toXMLText(this.iconUrl)+"</href>");
			strs.push("\t".repeat(level + 1)+"</Icon>");
		}
		if (this.hotSpotX != null && this.hotSpotY != null)
			strs.push("\t".repeat(level + 1)+"<hotSpot x="+text.toAttrText(""+this.hotSpotX)+" y="+text.toAttrText(""+this.hotSpotY)+" xunits=\"fraction\" yunits=\"fraction\"/>");
		strs.push("\t".repeat(level)+"</IconStyle>");
	}

	equals(o)
	{
		if (!o instanceof IconStyle)
			return false;
		if (!this.sameColor(o))
			return false;
		if (this.scale != o.scale)
			return false;
		if (this.heading != o.heading)
			return false;
		if (this.iconUrl != o.iconUrl)
			return false;
		if (this.hotSpotX != o.hotSpotX)
			return false;
		if (this.hotSpotY != o.hotSpotY)
			return false;
		return true;
	}
}

export class LabelStyle extends ColorStyle
{
	constructor()
	{
		super();
	}

	appendOuterXML(strs, level)
	{
		strs.push("\t".repeat(level)+"<LabelStyle>");
		this.appendInnerXML(strs, level + 1);
		strs.push("\t".repeat(level)+"</LabelStyle>");
	}

	equals(o)
	{
		if (!o instanceof LabelStyle)
			return false;
		if (!this.sameColor(o))
			return false;
		return true;
	}
};

export class LineStyle extends ColorStyle
{
	constructor()
	{
		super();
	}

	setWidth(width)
	{
		this.width = width;
	}

	appendOuterXML(strs, level)
	{
		strs.push("\t".repeat(level)+"<LineStyle>");
		this.appendInnerXML(strs, level + 1);
		if (this.width)
			strs.push("\t".repeat(level + 1)+"<width>"+this.width+"</width>");
		strs.push("\t".repeat(level)+"</LineStyle>");
	}

	equals(o)
	{
		if (!o instanceof LineStyle)
			return false;
		if (!this.sameColor(o))
			return false;
		if (this.width != o.width)
			return false;
		return true;
	}
};

export class PolyStyle extends ColorStyle
{
	constructor()
	{
		super();
	}

	appendOuterXML(strs, level)
	{
		strs.push("\t".repeat(level)+"<PolyStyle>");
		this.appendInnerXML(strs, level + 1);
		strs.push("\t".repeat(level)+"</PolyStyle>");
	}

	equals(o)
	{
		if (!o instanceof PolyStyle)
			return false;
		if (!this.sameColor(o))
			return false;
		return true;
	}
};

export class BalloonStyle extends Element
{
	constructor()
	{
		super();
	}

	appendOuterXML(strs, level)
	{
		strs.push("\t".repeat(level)+"<BalloonStyle>");
		this.appendInnerXML(strs, level + 1);
		if (this.bgColor)
			strs.push("\t".repeat(level + 1)+"<bgColor>"+this.bgColor+"</bgColor>");
		if (this.textColor)
			strs.push("\t".repeat(level + 1)+"<textColor>"+this.textColor+"</textColor>");
		if (this.text)
			strs.push("\t".repeat(level + 1)+"<text>"+text.toXMLText(this.text)+"</text>");
		if (this.displayMode)
			strs.push("\t".repeat(level + 1)+"<displayMode>"+text.toXMLText(this.displayMode)+"</displayMode>");
		strs.push("\t".repeat(level)+"</BalloonStyle>");
	}

	equals(o)
	{
		if (!o instanceof BalloonStyle)
			return false;
		if (this.bgColor != o.bgColor)
			return false;
		if (this.textColor != o.textColor)
			return false;
		if (this.text != o.text)
			return false;
		if (this.displayMode != o.displayMode)
			return false;
		return true;
	}
};

export class ListStyle extends Element
{
	constructor()
	{
		super();
	}

	appendOuterXML(strs, level)
	{
		strs.push("\t".repeat(level)+"<ListStyle>");
		this.appendInnerXML(strs, level + 1);
		if (this.listItemType)
			strs.push("\t".repeat(level + 1)+"<listItemType>"+text.toXMLText(this.listItemType)+"</listItemType>");
		if (this.bgColor)
			strs.push("\t".repeat(level + 1)+"<bgColor>"+this.bgColor+"</bgColor>");
		strs.push("\t".repeat(level)+"</ListStyle>");
	}

	equals(o)
	{
		if (!o instanceof ListStyle)
			return false;
		if (this.listItemType != o.listItemType)
			return false;
		if (this.bgColor != o.bgColor)
			return false;
		return true;
	}
};

export class Style extends Element
{
	constructor(id)
	{
		super();
		this.id = id;
	}

	setIconStyle(style)
	{
		if (style instanceof IconStyle)
		{
			this.iconStyle = style;
		}
	}

	setLabelStyle(style)
	{
		if (style instanceof LabelStyle)
		{
			this.labelStyle = style;
		}
	}

	setLineStyle(style)
	{
		if (style instanceof LineStyle)
		{
			this.lineStyle = style;
		}
	}

	setPolyStyle(style)
	{
		if (style instanceof PolyStyle)
		{
			this.polyStyle = style;
		}
	}

	setBalloonStyle(style)
	{
		if (style instanceof BalloonStyle)
		{
			this.balloonStyle = style;
		}
	}

	setListStyle(style)
	{
		if (style instanceof ListStyle)
		{
			this.listStyle = style;
		}
	}

	isStyle(iconStyle, labelStyle, lineStyle, polyStyle, balloonStyle, listStyle)
	{
		if (iconStyle == null)
		{
			if (this.iconStyle != null)
				return false;
		}
		else if (this.iconStyle == null)
		{
			return false;
		}
		else if (!this.iconStyle.equals(iconStyle))
		{
			return false;
		}
		if (labelStyle == null)
		{
			if (this.labelStyle != null)
				return false;
		}
		else if (this.labelStyle == null)
		{
			return false;
		}
		else if (!this.labelStyle.equals(labelStyle))
		{
			return false;
		}
		if (lineStyle == null)
		{
			if (this.lineStyle != null)
				return false;
		}
		else if (this.lineStyle == null)
		{
			return false;
		}
		else if (!this.lineStyle.equals(lineStyle))
		{
			return false;
		}
		if (polyStyle == null)
		{
			if (this.polyStyle != null)
				return false;
		}
		else if (this.polyStyle == null)
		{
			return false;
		}
		else if (!this.polyStyle.equals(polyStyle))
		{
			return false;
		}
		if (balloonStyle == null)
		{
			if (this.balloonStyle != null)
				return false;
		}
		else if (this.balloonStyle == null)
		{
			return false;
		}
		else if (!this.balloonStyle.equals(balloonStyle))
		{
			return false;
		}
		if (listStyle == null)
		{
			if (this.listStyle != null)
				return false;
		}
		else if (this.listStyle == null)
		{
			return false;
		}
		else if (!this.listStyle.equals(listStyle))
		{
			return false;
		}
		return true;
	}

	appendOuterXML(strs, level)
	{
		strs.push("\t".repeat(level)+"<Style id="+text.toAttrText(this.id)+">");
		if (this.iconStyle) this.iconStyle.appendOuterXML(strs, level + 1);
		if (this.labelStyle) this.labelStyle.appendOuterXML(strs, level + 1);
		if (this.lineStyle) this.lineStyle.appendOuterXML(strs, level + 1);
		if (this.polyStyle) this.polyStyle.appendOuterXML(strs, level + 1);
		if (this.balloonStyle) this.balloonStyle.appendOuterXML(strs, level + 1);
		if (this.listStyle) this.listStyle.appendOuterXML(strs, level + 1);
		strs.push("\t".repeat(level)+"</Style>");
	}
};

export class Feature extends Element
{
	constructor()
	{
		super();
	}

	setName(name)
	{
		this.name = name;
	}

	setVisibility(visibility)
	{
		this.visibility = visibility;
	}

	setOpen(open)
	{
		this.open = open;
	}

	setAuthor(author)
	{
		this.author = author;
	}

	setLink(link)
	{
		this.link = link;
	}

	setAddress(address)
	{
		this.address = address;
	}

	setPhoneNumber(phoneNumber)
	{
		this.phoneNumber = phoneNumber;
	}

	setSnippet(snippet)
	{
		this.snippet = snippet;
	}

	setDescription(description)
	{
		this.description = description;
	}

	setLookAt(lookAt)
	{
		this.lookAt = lookAt;
	}

	setStyle(style)
	{
		this.style = style;
	}

	appendInnerXML(strs, level)
	{
		if (this.name)
			strs.push("\t".repeat(level)+"<name>"+text.toXMLText(this.name)+"</name>");
		if (this.visibility != null)
			strs.push("\t".repeat(level)+"<visibility>"+(this.visibility?"1":"0")+"</visibility>");
		if (this.open != null)
			strs.push("\t".repeat(level)+"<open>"+(this.open?"1":"0")+"</open>");
		if (this.author)
			strs.push("\t".repeat(level)+"<atom:author>"+text.toXMLText(this.author)+"</atom:author>");
		if (this.link)
			strs.push("\t".repeat(level)+"<atom:link href="+text.toAttrText(this.link)+"/>");
		if (this.address)
			strs.push("\t".repeat(level)+"<address>"+text.toXMLText(this.address)+"</address>");
		//AddressDetails: ?;
		if (this.phoneNumber)
			strs.push("\t".repeat(level)+"<phoneNumber>"+text.toXMLText(this.phoneNumber)+"</phoneNumber>");
		if (this.snippet)
			strs.push("\t".repeat(level)+"<snippet>"+text.toXMLText(this.snippet)+"</snippet>");
		if (this.description)
			strs.push("\t".repeat(level)+"<description>"+text.toXMLText(this.description)+"</description>");
		if (this.lookAt)
			this.lookAt.appendOuterXML(strs, level);
		if (this.style)
			strs.push("\t".repeat(level)+"<styleUrl>#"+text.toXMLText(this.style.id)+"</styleUrl>");
	}
}

export class Container extends Feature
{
	constructor(eleName)
	{
		super();
		this.eleName = eleName;
		this.features = [];
	}

	addFeature(feature)
	{
		this.features.push(feature);
	}
}

export class Document extends Container
{
	constructor()
	{
		super("Document");
		this.styleList = [];
	}

	getOrNewStyle(iconStyle, labelStyle, lineStyle, polyStyle, balloonStyle, listStyle)
	{
		var i;
		for (i in this.styleList)
		{
			if (this.styleList[i].isStyle(iconStyle, labelStyle, lineStyle, polyStyle, balloonStyle, listStyle))
				return this.styleList[i];
		}
		var style = new Style("style"+(this.styleList.length + 1));
		style.iconStyle = iconStyle;
		style.labelStyle = labelStyle;
		style.lineStyle = lineStyle;
		style.polyStyle = polyStyle;
		style.balloonStyle = balloonStyle;
		style.listStyle = listStyle;
		this.styleList.push(style);
		return style;
	}

	appendOuterXML(strs, level)
	{
		var i;
		strs.push("\t".repeat(level)+"<"+this.eleName+">");
		this.appendInnerXML(strs, level + 1);
		for (i in this.styleList)
		{
			this.styleList[i].appendOuterXML(strs, level + 1);
		}
		for (i in this.features)
		{
			this.features[i].appendOuterXML(strs, level + 1);
		}
		strs.push("\t".repeat(level)+"</"+this.eleName+">");
	}
}

export class Folder extends Container
{
	constructor()
	{
		super("Folder");
	}

	appendOuterXML(strs, level)
	{
		strs.push("\t".repeat(level)+"<"+this.eleName+">");
		this.appendInnerXML(strs, level + 1);
		var i;
		for (i in this.features)
		{
			this.features[i].appendOuterXML(strs, level + 1);
		}
		strs.push("\t".repeat(level)+"</"+this.eleName+">");
	}
}

export class Placemark extends Feature
{
	constructor(vec)
	{
		super();
		this.vec = vec;
	}

	appendGeometry(strs, level, vec, subGeom)
	{
		if (vec instanceof geometry.Point)
		{
			strs.push("\t".repeat(level)+"<Point>");
			if (vec.coordinates.length >= 3)
			{
				if (subGeom != true)
					strs.push("\t".repeat(level + 1)+"<altitudeMode>absolute</altitudeMode>");
				strs.push("\t".repeat(level + 1)+"<coordinates>"+vec.coordinates[0]+","+vec.coordinates[1]+","+vec.coordinates[2]+"</coordinates>");
			}
			else
			{
				strs.push("\t".repeat(level + 1)+"<coordinates>"+vec.coordinates[0]+","+vec.coordinates[1]+"</coordinates>");
			}
			strs.push("\t".repeat(level)+"</Point>");
		}
		else if (vec instanceof geometry.LinearRing)
		{
			if (subGeom)
			{
				var i;
				strs.push("\t".repeat(level)+"<LinearRing>");
				if (vec.coordinates[0].length >= 3)
				{
					if (subGeom != true)
						strs.push("\t".repeat(level + 1)+"<altitudeMode>absolute</altitudeMode>");
					strs.push("\t".repeat(level + 1)+"<coordinates>");
					for (i in vec.coordinates)
					{
						strs.push("\t".repeat(level + 2)+vec.coordinates[i][0]+","+vec.coordinates[i][1]+","+vec.coordinates[i][2]+" ");
					}
					strs.push("\t".repeat(level + 1)+"</coordinates>");
				}
				else
				{
					if (subGeom != true)
						strs.push("\t".repeat(level + 1)+"<tessellate>1</tessellate>");
					strs.push("\t".repeat(level + 1)+"<coordinates>");
					for (i in vec.coordinates)
					{
						strs.push("\t".repeat(level + 2)+vec.coordinates[i][0]+","+vec.coordinates[i][1]+" ");
					}
					strs.push("\t".repeat(level + 1)+"</coordinates>");
				}
				strs.push("\t".repeat(level)+"</LinearRing>");
			}
			else
			{
				var pg = vec.toPolygon();
				this.appendGeometry(strs, level, pg, subGeom);
			}
		}
		else if (vec instanceof geometry.LineString)
		{
			var i;
			strs.push("\t".repeat(level)+"<LineString>");
			if (vec.coordinates[0].length >= 3)
			{
				if (subGeom != true)
					strs.push("\t".repeat(level + 1)+"<altitudeMode>absolute</altitudeMode>");
				strs.push("\t".repeat(level + 1)+"<coordinates>");
				for (i in vec.coordinates)
				{
					strs.push("\t".repeat(level + 2)+vec.coordinates[i][0]+","+vec.coordinates[i][1]+","+vec.coordinates[i][2]+" ");
				}
				strs.push("\t".repeat(level + 1)+"</coordinates>");
			}
			else
			{
				if (subGeom != true)
					strs.push("\t".repeat(level + 1)+"<tessellate>1</tessellate>");
				strs.push("\t".repeat(level + 1)+"<coordinates>");
				for (i in vec.coordinates)
				{
					strs.push("\t".repeat(level + 2)+vec.coordinates[i][0]+","+vec.coordinates[i][1]+" ");
				}
				strs.push("\t".repeat(level + 1)+"</coordinates>");
			}
			strs.push("\t".repeat(level)+"</LineString>");
		}
		else if (vec instanceof geometry.Polygon)
		{
			var i;
			strs.push("\t".repeat(level)+"<Polygon>");
			if (vec.geometries[0].coordinates[0].length >= 3)
			{
				if (subGeom != true)
					strs.push("\t".repeat(level + 1)+"<altitudeMode>absolute</altitudeMode>");
			}
			else
			{
				if (subGeom != true)
					strs.push("\t".repeat(level + 1)+"<tessellate>1</tessellate>");
			}
			for (i in vec.geometries)
			{
				if (i == 0)
				{
					strs.push("\t".repeat(level + 1)+"<outerBoundaryIs>");
					this.appendGeometry(strs, level + 2, vec.geometries[i], true);
					strs.push("\t".repeat(level + 1)+"</outerBoundaryIs>");
				}
				else
				{
					strs.push("\t".repeat(level + 1)+"<innerBoundaryIs>");
					this.appendGeometry(strs, level + 2, vec.geometries[i], true);
					strs.push("\t".repeat(level + 1)+"</innerBoundaryIs>");
				}
			}
			strs.push("\t".repeat(level)+"</Polygon>");
		}
		else if (vec instanceof geometry.MultiPolygon)
		{
			var i;
			strs.push("\t".repeat(level)+"<MultiGeometry>");
			for (i in vec.geometries)
			{
				this.appendGeometry(strs, level + 1, vec.geometries[i], false);
			}
			strs.push("\t".repeat(level)+"</MultiGeometry>");
		}
	}

	appendOuterXML(strs, level)
	{
		strs.push("\t".repeat(level)+"<Placemark>");
		this.appendInnerXML(strs, level + 1);
		if (this.vec)
		{
			this.appendGeometry(strs, level + 1, this.vec);
		}
		strs.push("\t".repeat(level)+"</Placemark>");
	}
}

export function toString(item)
{
	var strs = [];
	strs.push("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>");
	strs.push("<kml xmlns=\"http://www.opengis.net/kml/2.2\">");
	item.appendOuterXML(strs, 1);
	strs.push("</kml>");
	return strs.join("\r\n");
}
