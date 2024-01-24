import * as data from "./data";
import * as geometry from "./geometry";
import * as web from "./web";

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

export enum HotSpotUnit
{
	Fraction,
	Pixels,
	InsetPixels
};

export class Element
{
	getUsedNS(ns: object): void;
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

	getUsedNS(ns: object): void;
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
	setColor(color: string): void;

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
	hotSpotUnitX: HotSpotUnit;
	hotSpotUnitY: HotSpotUnit;

	constructor();
	setScale(scale: number): void;
	setHeading(heading: number): void;
	setIconUrl(iconUrl: string): void;
	setHotSpotX(hotSpotX: number, hsUnit?: HotSpotUnit): void;
	setHotSpotY(hotSpotY: number, hsUnit?: HotSpotUnit): void;
	getUsedNS(ns: object): void;
	appendOuterXML(strs: string[], level: number): void;
	equals(o: IconStyle): boolean;
};

export class LabelStyle extends ColorStyle
{
	scale: number;	
	constructor();
	setScale(scale: number): void;
	getUsedNS(ns: object): void;
	appendOuterXML(strs: string[], level: number): void;
	equals(o: LabelStyle): boolean;
};

export class LineStyle extends ColorStyle
{
	width: number;
	labelVisibility: boolean;
	constructor();
	setWidth(width: number): void;
	setLabelVisibility(labelVisibility: boolean): void;
	getUsedNS(ns: object): void;
	appendOuterXML(strs: string[], level: number): void;
	equals(o: LineStyle): boolean;
};

export class PolyStyle extends ColorStyle
{

	constructor();
	getUsedNS(ns: object): void;
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
	getUsedNS(ns: object): void;
	appendOuterXML(strs: string[], level: number): void;
	equals(o: BalloonStyle): boolean;
};

export class ListStyle extends Element
{
	listItemType: ListItemType;
	bgColor: string;

	constructor();
	getUsedNS(ns: object): void;
	appendOuterXML(strs: string[], level: number): void;
	equals(o: ListStyle): boolean;
};

export class StyleSelector extends Element
{
	id: string;
	constructor(id: string);
}
export class Style extends StyleSelector
{
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
	getUsedNS(ns: object): void;
	appendOuterXML(strs: string[], level: number): void;
};

export class StyleMap extends StyleSelector
{
	normalStyle: Style;
	highlightStyle: Style;

	constructor(id: string, normalStyle: Style, highlightStyle: Style);
	getUsedNS(ns: object): void;
	appendOuterXML(strs: string[], level: number): void;
}

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
	style: StyleSelector;

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
	setStyle(style: StyleSelector): void;

	appendInnerXML(strs: string[], level: number): void;
}

export class Container extends Feature
{
	eleName: string;
	features: Feature[];
	styleList: Style[];
	styleMapList: StyleMap[];

	constructor(eleName: string);
	addFeature(feature: Feature): void;
	addStyle(style: Style): void;
	addStyleMap(styleMap: StyleMap): void;
	getOrNewStyle(iconStyle: IconStyle, labelStyle: LabelStyle, lineStyle: LineStyle, polyStyle: PolyStyle, balloonStyle: BalloonStyle, listStyle: ListStyle): Style;
	getStyleById(id: string): Style | StyleMap | null;
	getUsedNS(ns: object): void;
	appendOuterXML(strs: string[], level: number): void;
}

export class Document extends Container
{
	constructor();
}

export class Folder extends Container
{
	constructor();
}

export class NetworkLink extends Feature
{
	constructor();

	getUsedNS(ns: object): void;
	appendOuterXML(strs: string[], level: number): void;
}

export class Placemark extends Feature
{
	vec: geometry.Vector2D;
	constructor(vec: geometry.Vector2D);

	appendGeometry(strs: string[], level: number, vec: geometry.Vector2D, subGeom?: boolean): void;
	getUsedNS(ns: object): void;
	appendOuterXML(strs: string[], level: number): void;
}

export class GroundOverlay extends Feature
{
	constructor();

	getUsedNS(ns: object): void;
	appendOuterXML(strs: string[], level: number): void;
}

export class PhotoOverlay extends Feature
{
	constructor();

	getUsedNS(ns: object): void;
	appendOuterXML(strs: string[], level: number): void;
}

export class ScreenOverlay extends Feature
{
	constructor();

	getUsedNS(ns: object): void;
	appendOuterXML(strs: string[], level: number): void;
}

export function toString(item: Feature | NetworkLinkControl): string;
export function toColor(color: string): web.Color;
export function toCSSColor(color: string): string;
