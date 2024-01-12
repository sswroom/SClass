export function getRequestURLBase(): string;
export function getParameterByName(name: string): string | null;
export function loadJSON(url: string, onResultFunc: Function): void;
export function buildTable(o: object | object[]): string;
export function openData(data: string, contentType: string, fileName?: string): void;

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
