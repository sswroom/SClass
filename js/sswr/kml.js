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

export const HotSpotUnit = {
	Fraction: "fraction",
	Pixels: "pixels",
	InsetPixels: "insetPixels"
}

export const RefreshMode = {
	OnChange: "onChange",
	OnInterval: "onInterval",
	OnExpire: "onExpire"
};

export const ViewRefreshMode = {
	Never: "never",
	OnStop: "onStop",
	OnRequest: "onRequest",
	OnRegion: "onRegion"
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

	getUsedNS(ns)
	{
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
		let c = web.parseCSSColor(color);
		this.fromARGB(c.a, c.r, c.g, c.b);
	}

	setColor(color)
	{
		if (typeof color == "string" && color.length == 8)
		{
			this.color = color;
		}
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

	setHotSpotX(hotSpotX, hsUnit)
	{
		this.hotSpotX = hotSpotX;
		this.hotSpotUnitX = hsUnit || HotSpotUnit.Fraction;
	}

	setHotSpotY(hotSpotY, hsUnit)
	{
		this.hotSpotY = hotSpotY;
		this.hotSpotUnitY = hsUnit || HotSpotUnit.Fraction;
	}

	getUsedNS(ns)
	{
	}

	appendOuterXML(strs, level)
	{
		strs.push("\t".repeat(level)+"<IconStyle>");
		this.appendInnerXML(strs, level + 1);
		if (this.scale != null)
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
			strs.push("\t".repeat(level + 1)+"<hotSpot x="+text.toAttrText(""+this.hotSpotX)+" y="+text.toAttrText(""+this.hotSpotY)+" xunits=\""+this.hotSpotUnitX+"\" yunits=\""+this.hotSpotUnitY+"\"/>");
		strs.push("\t".repeat(level)+"</IconStyle>");
	}

	equals(o)
	{
		if (!(o instanceof IconStyle))
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
		if (this.hotSpotUnitX != o.hotSpotUnitX)
			return false;
		if (this.hotSpotUnitY != o.hotSpotUnitY)
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

	setScale(scale)
	{
		this.scale = scale;
	}

	getUsedNS(ns)
	{
	}

	appendOuterXML(strs, level)
	{
		strs.push("\t".repeat(level)+"<LabelStyle>");
		this.appendInnerXML(strs, level + 1);
		if (this.scale != null)
			strs.push("\t".repeat(level + 1)+"<scale>"+this.scale+"</scale>");
		strs.push("\t".repeat(level)+"</LabelStyle>");
	}

	equals(o)
	{
		if (!(o instanceof LabelStyle))
			return false;
		if (!this.sameColor(o))
			return false;
		if (this.scale != o.scale)
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

	setLabelVisibility(labelVisibility)
	{
		this.labelVisibility = labelVisibility;
	}

	getUsedNS(ns)
	{
		if (this.labelVisibility != null)
			ns.gx = "http://www.google.com/kml/ext/2.2";
	}

	appendOuterXML(strs, level)
	{
		strs.push("\t".repeat(level)+"<LineStyle>");
		this.appendInnerXML(strs, level + 1);
		if (this.width)
			strs.push("\t".repeat(level + 1)+"<width>"+this.width+"</width>");
		if (this.labelVisibility != null)
			strs.push("\t".repeat(level + 1)+"<gx:labelVisibility>"+(this.labelVisibility?"1":"0")+"</gx:labelVisibility>");
		strs.push("\t".repeat(level)+"</LineStyle>");
	}

	equals(o)
	{
		if (!(o instanceof LineStyle))
			return false;
		if (!this.sameColor(o))
			return false;
		if (this.width != o.width)
			return false;
		return true;
	}
}

export class PolyStyle extends ColorStyle
{
	constructor()
	{
		super();
	}

	setOutline(outline)
	{
		this.outline = outline;
	}

	getUsedNS(ns)
	{
	}

	appendOuterXML(strs, level)
	{
		strs.push("\t".repeat(level)+"<PolyStyle>");
		this.appendInnerXML(strs, level + 1);
		if (this.outline != null)
			strs.push("\t".repeat(level + 1)+"<outline>"+(this.labelVisibility?"1":"0")+"</outline>");
		strs.push("\t".repeat(level)+"</PolyStyle>");
	}

	equals(o)
	{
		if (!(o instanceof PolyStyle))
			return false;
		if (!this.sameColor(o))
			return false;
		if (this.outline != o.outline)
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

	getUsedNS(ns)
	{
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
		if (!(o instanceof BalloonStyle))
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

	getUsedNS(ns)
	{
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
		if (!(o instanceof ListStyle))
			return false;
		if (this.listItemType != o.listItemType)
			return false;
		if (this.bgColor != o.bgColor)
			return false;
		return true;
	}
};

export class StyleSelector extends Element
{
	constructor(id)
	{
		super();
		this.id = id;
	}
}

export class Style extends StyleSelector
{
	constructor(id)
	{
		super(id);
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

	getUsedNS(ns)
	{
		if (this.iconStyle) this.iconStyle.getUsedNS(ns);
		if (this.labelStyle) this.labelStyle.getUsedNS(ns);
		if (this.lineStyle) this.lineStyle.getUsedNS(ns);
		if (this.polyStyle) this.polyStyle.getUsedNS(ns);
		if (this.balloonStyle) this.balloonStyle.getUsedNS(ns);
		if (this.listStyle) this.listStyle.getUsedNS(ns);
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

export class StyleMap extends StyleSelector
{
	constructor(id, normalStyle, highlightStyle)
	{
		super(id);
		this.normalStyle = normalStyle;
		this.highlightStyle = highlightStyle;
	}

	getUsedNS(ns)
	{
	}

	appendOuterXML(strs, level)
	{
		strs.push("\t".repeat(level)+"<StyleMap id="+text.toAttrText(this.id)+">");
		strs.push("\t".repeat(level + 1)+"<Pair>");
		strs.push("\t".repeat(level + 2)+"<key>normal</key>");
		strs.push("\t".repeat(level + 2)+"<styleUrl>#"+text.toXMLText(this.normalStyle.id)+"</styleUrl>");
		strs.push("\t".repeat(level + 1)+"</Pair>");
		strs.push("\t".repeat(level + 1)+"<Pair>");
		strs.push("\t".repeat(level + 2)+"<key>highlight</key>");
		strs.push("\t".repeat(level + 2)+"<styleUrl>#"+text.toXMLText(this.highlightStyle.id)+"</styleUrl>");
		strs.push("\t".repeat(level + 1)+"</Pair>");
		strs.push("\t".repeat(level)+"</Style>");
	}
}

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

	setAuthorName(authorName)
	{
		this.authorName = authorName;
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
		if (this.author && this.authorName)
			strs.push("\t".repeat(level)+"<atom:author>"+text.toXMLText(this.author)+"<atom:name>"+text.toXMLText(this.authorName)+"</atom:name></atom:author>");
		else if (this.author)
			strs.push("\t".repeat(level)+"<atom:author>"+text.toXMLText(this.author)+"</atom:author>");
		else if (this.authorName)
			strs.push("\t".repeat(level)+"<atom:author><atom:name>"+text.toXMLText(this.authorName)+"</atom:name></atom:author>");
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
		this.styleList = null;
	}

	addFeature(feature)
	{
		this.features.push(feature);
	}

	addStyle(style)
	{
		if (this.styleList == null)
			this.styleList = [];
		this.styleList.push(style);
	}

	addStyleMap(styleMap)
	{
		if (this.styleMapList == null)
			this.styleMapList = [];
		this.styleMapList.push(styleMap);
	}

	getOrNewStyle(iconStyle, labelStyle, lineStyle, polyStyle, balloonStyle, listStyle)
	{
		if (this.styleList == null)
			this.styleList = [];
		let i;
		for (i in this.styleList)
		{
			if (this.styleList[i].isStyle(iconStyle, labelStyle, lineStyle, polyStyle, balloonStyle, listStyle))
				return this.styleList[i];
		}
		let style = new Style("style"+(this.styleList.length + 1));
		style.iconStyle = iconStyle;
		style.labelStyle = labelStyle;
		style.lineStyle = lineStyle;
		style.polyStyle = polyStyle;
		style.balloonStyle = balloonStyle;
		style.listStyle = listStyle;
		this.styleList.push(style);
		return style;
	}

	getStyleById(id)
	{
		let i;
		if (this.styleList)
		{
			for (i in this.styleList)
			{
				if (this.styleList[i].id == id)
					return this.styleList[i];
			}
		}
		if (this.styleMapList)
		{
			for (i in this.styleMapList)
			{
				if (this.styleMapList[i].id == id)
					return this.styleMapList[i];
			}
		}
		return null;
	}

	getBounds()
	{
		let bounds = null;
		let thisBounds;
		let i;
		for (i in this.features)
		{
			thisBounds = this.features[i].getBounds();
			if (thisBounds)
			{
				if (bounds)
				{
					bounds = bounds.unionInPlace(thisBounds);
				}
				else
				{
					bounds = thisBounds;
				}
			}
		}
		return bounds;
	}

	getUsedNS(ns)
	{
		if (this.styleList)
		{
			let i;
			for (i in this.styleList)
			{
				this.styleList[i].getUsedNS(ns);
			}
		}
	}

	appendOuterXML(strs, level)
	{
		let i;
		strs.push("\t".repeat(level)+"<"+this.eleName+">");
		this.appendInnerXML(strs, level + 1);
		if (this.styleList)
		{
			for (i in this.styleList)
			{
				this.styleList[i].appendOuterXML(strs, level + 1);
			}
		}
		if (this.styleMapList)
		{
			for (i in this.styleMapList)
			{
				this.styleMapList[i].appendOuterXML(strs, level + 1);
			}
		}
		for (i in this.features)
		{
			this.features[i].appendOuterXML(strs, level + 1);
		}
		strs.push("\t".repeat(level)+"</"+this.eleName+">");
	}
}

export class Document extends Container
{
	constructor()
	{
		super("Document");
	}
}

export class Folder extends Container
{
	constructor()
	{
		super("Folder");
	}
}

export class NetworkLink extends Feature
{
	constructor(networkLink)
	{
		super();
		this.networkLink = networkLink;
	}

	setRefreshVisibility(refreshVisibility)
	{
		this.refreshVisibility = refreshVisibility;
	}

	setFlyToView(flyToView)
	{
		this.flyToView = flyToView;
	}

	setRefreshMode(refreshMode)
	{
		this.refreshMode = refreshMode;
	}

	setRefreshInterval(refreshInterval)
	{
		this.refreshInterval = refreshInterval;
	}

	setViewRefreshMode(viewRefreshMode)
	{
		this.viewRefreshMode = viewRefreshMode;
	}

	getBounds()
	{
		return null;
	}

	getUsedNS(ns)
	{
	}

	appendOuterXML(strs, level)
	{
		strs.push("\t".repeat(level)+"<NetworkLink>");
		this.appendInnerXML(strs, level + 1);
		if (this.refreshVisibility != null)
			strs.push("\t".repeat(level + 1)+"<refreshVisibility>"+(this.refreshVisibility?"1":"0")+"</refreshVisibility>");
		if (this.flyToView != null)
			strs.push("\t".repeat(level + 1)+"<flyToView>"+(this.flyToView?"1":"0")+"</flyToView>");

		strs.push("\t".repeat(level + 1)+"<Link>");
		strs.push("\t".repeat(level + 2)+"<href>"+text.toXMLText(this.networkLink)+"</href>");
		if (this.refreshMode)
			strs.push("\t".repeat(level + 2)+"<refreshMode>"+text.toXMLText(this.refreshMode)+"</refreshMode>");
		if (this.refreshInterval)
			strs.push("\t".repeat(level + 2)+"<refreshInterval>"+this.refreshInterval+"</refreshInterval>");
		if (this.viewRefreshMode)
			strs.push("\t".repeat(level + 2)+"<viewRefreshMode>"+text.toXMLText(this.viewRefreshMode)+"</viewRefreshMode>");
		strs.push("\t".repeat(level + 1)+"</Link>");
		strs.push("\t".repeat(level)+"</NetworkLink>");
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
				let i;
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
				let pg = vec.toPolygon();
				this.appendGeometry(strs, level, pg, subGeom);
			}
		}
		else if (vec instanceof geometry.LineString)
		{
			let i;
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
			let i;
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
			let i;
			strs.push("\t".repeat(level)+"<MultiGeometry>");
			for (i in vec.geometries)
			{
				this.appendGeometry(strs, level + 1, vec.geometries[i], false);
			}
			strs.push("\t".repeat(level)+"</MultiGeometry>");
		}
	}

	getBounds()
	{
		if (this.vec)
		{
			return this.vec.getBounds();
		}
		return null;
	}

	getUsedNS(ns)
	{
		if (this.author || this.link || this.authorName)
		{
			ns.atom = "http://www.w3.org/2005/Atom";
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
	let strs = [];
	strs.push("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>");
	let namespaces = {};
	item.getUsedNS(namespaces);
	strs.push("<kml xmlns=\"http://www.opengis.net/kml/2.2\">");
	item.appendOuterXML(strs, 1);
	strs.push("</kml>");
	return strs.join("\r\n");
}

export function toColor(color)
{
	if (color.length != 8)
		return null;

	return {
		a: Number.parseInt(color.substring(0, 2), 16) / 255,
		b: Number.parseInt(color.substring(2, 4), 16) / 255,
		g: Number.parseInt(color.substring(4, 6), 16) / 255,
		r: Number.parseInt(color.substring(6, 8), 16) / 255
	};
}

export function toCSSColor(color)
{
	if (color.length != 8)
		return null;

	let a = Number.parseInt(color.substring(0, 2), 16);
	if (a == 255)
	{
		return "#"+color.substring(6, 8)+color.substring(4, 6)+color.substring(2, 4);
	}
	else
	{
		let b = Number.parseInt(color.substring(2, 4), 16);
		let g = Number.parseInt(color.substring(4, 6), 16);
		let r = Number.parseInt(color.substring(6, 8), 16);
		return "rgba("+r+", "+g+", "+b+", "+(a / 255.0)+")";
	}
}
