import * as data from "./data";

export enum AltitudeMode
{
	ClampToGround,
	RelativeToGround,
	Absolute
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

export class Style extends Element
{
	id: string;

	constructor(id: string);
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

	appendOuterXML(strs: string[], level: number): void;
}

export class Placemark extends Feature
{
	constructor();

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
