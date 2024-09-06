import * as data from "../data";
import * as spreadsheet from "../spreadsheet";

declare class LinkInfo
{
	row: number;
	col: number;
	cell: spreadsheet.CellData;
};

declare class BorderInfo
{
	left: spreadsheet.BorderStyle;
	top: spreadsheet.BorderStyle;
	right: spreadsheet.BorderStyle;
	bottom: spreadsheet.BorderStyle;
};

export class XLSXExporter
{
	constructor();

	getName(): string;
	isObjectSupported(pobj: data.ParsedObject): boolean;
	getOutputExt(): string;
	getOutputMIME(): string;
	exportFile(fileName: string, pobj: data.ParsedObject, param?: object): Uint8Array|null;

	//static appendFill(sb: string[], fill: spreadsheet.OfficeFill|null): void;
	//static appendLineStyle(sb: string[], lineStyle: spreadsheet.OfficeLineStyle|null): void;
	static appendTitle(sb: string[], title: string): void;
	//static appendShapeProp(sb: string[], shapeProp: spreadsheet.OfficeShapeProp): void;
	//static appendAxis(sb: string[], axis: spreadsheet.OfficeChartAxis, index: number): void;
	//static appendSeries(sb: string[], series: spreadsheet.OfficeChartSeries, index: number): void;
	static appendBorder(sb: string[], border: spreadsheet.BorderStyle, name: string): void;
	static appendXF(sb: string[], style: spreadsheet.CellStyle, borders: BorderInfo[], workbook: spreadsheet.Workbook, numFmtMap: object): void;

	static presetColorCode(color: spreadsheet.PresetColor): string;
	static toFormatCode(dataFormat: string): string;
};
