declare class Color
{
	a: number; //0.0-1.0
	r: number; //0.0-1.0
	g: number; //0.0-1.0
	b: number; //0.0-1.0
}

declare class ImageInfo
{
	width: number;
	height: number;
}

declare class PrintOptions
{
	pageBorderTopHTML?: string;
	pageBorderBottomHTML?: string;
	overlayHTML?: string;
	pageTitle?: string;
}

export enum OSType
{
	Unknown,
	WindowsNT,
	WindowsCE,
	WindowsSvr,
	Linux_X86_64,
	Android,
	iPad,
	iPhone,
	Darwin,
	MacOS,
	WindowsNT64,
	ChromeOS,
	Linux_i686,
	Netcast
}

export enum BrowserType
{
	Unknown,
	IE,
	Firefox,
	Chrome,
	Safari,
	UCBrowser,
	CFNetwork,
	SogouWeb,
	Baidu,
	Semrush,
	Dalvik,
	Indy,
	GoogleBots,
	AndroidWV,
	Samsung,
	WestWind,
	Yandex,
	Bing,
	Masscan,
	PyRequests,
	Zgrab,
	Edge,
	PyURLLib,
	GoogleBotD,
	DotNet,
	WinDiag,
	SSWR,
	SmartTV,
	BlexBot,
	SogouPic,
	Nutch,
	Yisou,
	Wget,
	Scrapy,
	GoHTTP,
	WinHTTP,
	NLPProject,
	ApacheHTTP,
	BannerDet,
	NetcraftWeb,
	NetcraftAG,
	AhrefsBot,
	Mj12Bot,
	NetSysRes,
	Whatsapp,
	Curl,
	GSA,
	Facebook,
	Netseen,
	MSNBot,
	LibwwwPerl,
	HuaweiBrowser,
	Opera,
	MiBrowser
}

export enum PaperOrientation
{
	Landscape,
	Portrait
}

declare class BrowserInfo
{
	os: OSType;
	osVer?: string;
	browser: BrowserType;
	browserVer?: string;
	devName?: string;
}

export function getRequestURLBase(): string;
export function getParameterByName(name: string): string | null;
export function loadJSON(url: string, onResultFunc: Function): void;
export function buildTable(o: object | object[]): string;
export function openData(data: string | Blob, contentType: string, fileName?: string): void;
export function openUrl(url: string, fileName?: string): void;
export function parseCSSColor(c: string): Color;
export function handleFileDrop(ele: HTMLElement, hdlr: (file: File)=>void): void;
export function appendUrl(targetUrl: string, docUrl: string): string;
export function mimeFromFileName(fileName: string): string;
export function mimeFromExt(ext: string): string;
export function getImageInfo(url: string): Promise<ImageInfo|null>;
export function propertiesToHTML(prop: object, nameMap?: object, timeFormat?: string): string;
export function getCacheSize(name: string): Promise<number>;
export function getInputElement(id: string): HTMLInputElement;
export function getSelectElement(id: string): HTMLSelectElement;
export function getButtonElement(id: string): HTMLButtonElement;
export function getDivElement(id: string): HTMLDivElement;
export function getSpanElement(id: string): HTMLSpanElement;
export function getCanvasElement(id: string): HTMLCanvasElement;
export function getImgElement(id: string): HTMLImageElement;
export function canvasToBlob(canvas: HTMLCanvasElement): Promise<Blob|null>;
export function elementToSVGString(node: Element, width: string | number, height: string | number): string;
export function genPrintWindowHTML(imgDataURL: string, orientation: PaperOrientation, paperSize?: string, options: PrintOptions): string;
export function printImageData(imgDataURL: string, orientation: PaperOrientation, paperSize?: string, options: PrintOptions): void;
export function getBrowserInfo(): Promise<BrowserInfo>;
export function parseUserAgent(userAgent: string): BrowserInfo;

declare class DialogButton
{
	name: string;
	onclick: ()=>void;
}

declare class DialogOptions
{
	width?: number | string;
	height?: number | string;
	zIndex?: number;
	buttonClass?: string;
	contentClass?: string;
	buttons?: DialogButton[];
	margin?: number;
}


export class Dialog
{
	content: string | HTMLElement;
	options: DialogOptions;
	darkColor?: HTMLElement;

	constructor(content: string | HTMLElement, options?: DialogOptions);
	show(): void;
	close(): void;
	setContent(content: string): void;
	updateOption(name: string, value: any): void;
	closeButton(name?: string): void;
}
