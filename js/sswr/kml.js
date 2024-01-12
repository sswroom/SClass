import * as text from "./text.js";

export const AltitudeMode = {
	ClampToGround: "clampToGround",
	RelativeToGround: "relativeToGround",
	Absolute: "absolute"
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

	appendOuterXML(strs, level)
	{
		strs.push("\t".repeat(level)+"<"+this.eleName+">");
		this.appendInnerXML(strs, level + 1);
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

export function toString(item)
{
	var strs = [];
	strs.push("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>");
	strs.push("<kml xmlns=\"http://www.opengis.net/kml/2.2\">");
	item.appendOuterXML(strs, 1);
	strs.push("</kml>");
	return strs.join("\r\n");
}
