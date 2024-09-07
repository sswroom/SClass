import * as data from "../data";

export class GPXExporter
{
	constructor();

	getName(): string;
	isObjectSupported(pobj: data.ParsedObject): boolean;
	getOutputExt(): string;
	getOutputMIME(): string;
	exportFile(fileName: string, pobj: data.ParsedObject, param?: object): Uint8Array|null;
};
