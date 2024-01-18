import * as data from "./data";
import * as geometry from "./geometry";

export enum AltitudeMode
{
	ClampToGround,
	RelativeToGround,
	Absolute
};

export enum DisplayMode
{
	Default,
	Hide
};

export enum ListItemType
{
	Check,
	RadioFolder,
	CheckOffOnly,
	CheckHideChildren
};

export enum ItemIconMode
{
	Open,
	Closed,
	Error,
	Fetching0,
	Fetching1,
	Fetching2
};

export class Element
{
	appendOuterXML(strs: string[], level: number): void;
}

export class NetworkLinkControl extends Element
{
	minRefreshPeriod?: number;
	maxSessionLength?: number;
	cookie?: string;
	message?: string;
	linkName?: string;
	linkDescription?: string;
	linkSnippet?: string;
	expires?: data.Timestamp;

	getUsedNS(ns: object): void;
	appendOuterXML(strs: string[], level: number): void;
};

export class LookAt extends Element
{
	longitude: number;
	latitude: number;
	altitude: number;
	heading: number;
	tilt: number;
	range: number;
	altitudeMode: AltitudeMode;

	constructor(longitude: number, latitude: number, altitude: number, range: number);
	setHeading(heading: number): void;
	setTilt(tilt: number): void;
	setAltitudeMode(altitudeMode: AltitudeMode): void;

	appendOuterXML(strs: string[], level: number): void;
};

export class ColorStyle extends Element
{
	color: string;
	randomColor: boolean;

	constructor();
	setRandomColor(randomColor: boolean): void;
	fromARGB(a: number, r: number, g: number, b: number): void;
	fromCSSColor(color: string): void;

	sameColor(c: ColorStyle): boolean;
	appendInnerXML(strs: string[], level: number): void;
};

export class IconStyle extends ColorStyle
{
	scale: number;
	heading: number;
	iconUrl: string;
	hotSpotX: number;
	hotSpotY: number;

	constructor();
	setScale(scale: number);
	setHeading(heading: number);
	setIconUrl(iconUrl: string);
	setHotSpotX(hotSpotX: number);
	setHotSpotY(hotSpotY: number);
	appendOuterXML(strs: string[], level: number): void;
	equals(o: IconStyle): boolean;
};

export class LabelStyle extends ColorStyle
{
	
	constructor();
	appendOuterXML(strs: string[], level: number): void;
	equals(o: LabelStyle): boolean;
};

export class LineStyle extends ColorStyle
{
	width: number;
	constructor();
	setWidth(width: number);
	appendOuterXML(strs: string[], level: number): void;
	equals(o: LineStyle): boolean;
};

export class PolyStyle extends ColorStyle
{

	constructor();
	appendOuterXML(strs: string[], level: number): void;
	equals(o: PolyStyle): boolean;
};

export class BalloonStyle extends Element
{
	bgColor: string;
	textColor: string;
	text: string;
	displayMode: DisplayMode;

	constructor();
	appendOuterXML(strs: string[], level: number): void;
	equals(o: BalloonStyle): boolean;
};

export class ListStyle extends Element
{
	listItemType: ListItemType;
	bgColor: string;

	constructor();
	appendOuterXML(strs: string[], level: number): void;
	equals(o: ListStyle): boolean;
};

export class Style extends Element
{
	id: string;
	iconStyle: IconStyle;
	labelStyle: LabelStyle;
	lineStyle: LineStyle;
	polyStyle: PolyStyle;
	balloonStyle: BalloonStyle;
	listStyle: ListStyle;

	constructor(id: string);
	setIconStyle(style: IconStyle): void;
	setLabelStyle(style: LabelStyle): void;
	setLineStyle(style: LineStyle): void;
	setPolyStyle(style: PolyStyle): void;
	setBalloonStyle(style: BalloonStyle): void;
	setListStyle(style: ListStyle): void;
	isStyle(iconStyle: IconStyle, labelStyle: LabelStyle, lineStyle: LineStyle, polyStyle: PolyStyle, balloonStyle: BalloonStyle, listStyle: ListStyle): boolean;
	appendOuterXML(strs: string[], level: number): void;
};

export class Feature extends Element
{
	name: string;
	visibility: boolean;
	open: boolean;
	author: string;
	link: string;
	address: string;
	//AddressDetails: ?;
	phoneNumber: string;
	snippet: string;
	description: string;
	lookAt: LookAt;
	style: Style;

	constructor();
	setName(name: string): void;
	setVisibility(visibility: boolean): void;
	setOpen(open: boolean): void;
	setAuthor(author: string): void;
	setLink(link: string): void;
	setAddress(address: string): void;
	setPhoneNumber(phoneNumber: string): void;
	setSnippet(snippet: string): void;
	setDescription(description: string): void;
	setLookAt(lookAt: LookAt): void;
	setStyle(style: Style): void;

	appendInnerXML(strs: string[], level: number): void;
}

export class Container extends Feature
{
	eleName: string;
	features: Feature[];

	constructor(eleName: string);
	addFeature(feature: Feature): void;
}

export class Document extends Container
{
	styleList: Style[];

	constructor();
	getOrNewStyle(iconStyle: IconStyle, labelStyle: LabelStyle, lineStyle: LineStyle, polyStyle: PolyStyle, balloonStyle: BalloonStyle, listStyle: ListStyle): Style;
	appendOuterXML(strs: string[], level: number): void;
}

export class Folder extends Container
{
	constructor();
	appendOuterXML(strs: string[], level: number): void;
}

export class NetworkLink extends Feature
{
	constructor();

	appendOuterXML(strs: string[], level: number): void;
}

export class Placemark extends Feature
{
	vec: geometry.Vector2D;
	constructor(vec: geometry.Vector2D);

	appendGeometry(strs: string[], level: number, vec: geometry.Vector2D, subGeom?: boolean): void;
	appendOuterXML(strs: string[], level: number): void;
}

export class GroundOverlay extends Feature
{
	constructor();

	appendOuterXML(strs: string[], level: number): void;
}

export class PhotoOverlay extends Feature
{
	constructor();

	appendOuterXML(strs: string[], level: number): void;
}

export class ScreenOverlay extends Feature
{
	constructor();

	appendOuterXML(strs: string[], level: number): void;
}

export function toString(item: Feature | NetworkLinkControl): string;
