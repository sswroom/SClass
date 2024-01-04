export function getRequestURLBase(): string;
export function getParameterByName(name: string): string | null;
export function loadJSON(url: string, onResultFunc: Function): void;

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
}

export class Dialog
{
	showing: boolean;
	content: string | HTMLElement;
	options: DialogOptions;
	darkColor?: HTMLElement;

	constructor(content: string | HTMLElement, options?: DialogOptions);
	show(): void;
	close(): void;
	
	static closeButton(name?: string): void;
}
