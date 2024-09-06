import * as data from "./data";
import * as text from "./text";
import * as unit from "./unit";

export enum FontFamily
{
	NA,
	Roman,
	Swiss,
	Modern,
	Script,
	Decorative
}

export enum BorderType
{
	None,
	Thin,
	Medium,
	Dashed,
	Dotted,
	Thick,
	DOUBLE,
	Hair,
	MediumDashed,
	DashDot,
	MediumDashDot,
	DashDotDot,
	MediumDashDotDot,
	SlantedDashDot
}

export enum FillPattern
{
	NoFill,
	SolidForeground,
	FineDot,
	AltBars,
	SparseDots,
	ThickHorzBands,
	ThickVertBands,
	ThickBackwardDiag,
	ThickForwardDiag,
	BigSpots,
	Bricks,
	ThinHorzBands,
	ThinVertBands,
	ThinBackwardDiag,
	ThinForwardDiag,
	Squares,
	Diamonds,
	LessDots,
	LeastDots
}

export enum CellDataType
{
	String,
	Number,
	DateTime,
	MergedLeft,
	MergedUp
}

export enum AnchorType
{
	Absolute,
	OneCell,
	TwoCell
}

export enum LegendPos
{
	Bottom
}

export enum BlankAs
{
	Default,
	Gap,
	Zero
}

export enum ChartType
{
	Unknown,
	LineChart
}

export enum AxisType
{
	Date,
	Category,
	Numeric,
	Series
}

export enum AxisPosition
{
	Left,
	Top,
	Right,
	Bottom
}

export enum TickLabelPosition
{
	High,
	Low,
	NextTo,
	None
}

export enum AxisCrosses
{
	AutoZero,
	Max,
	Min
}

export enum ColorType
{
	Preset,
	Argb
}

export enum PresetColor
{
	AliceBlue,
	AntiqueWhite,
	Aqua,
	Aquamarine,
	Azure,
	Beige,
	Bisque,
	Black,
	BlanchedAlmond,
	Blue,
	BlueViolet,
	Brown,
	BurlyWood,
	CadetBlue,
	Chartreuse,
	Chocolate,
	Coral,
	CornflowerBlue,
	Cornsilk,
	Crimson,
	Cyan,
	DeepPink,
	DeepSkyBlue,
	DimGray,
	DarkBlue,
	DarkCyan,
	DarkGoldenrod,
	DarkGray,
	DarkGreen,
	DarkKhaki,
	DarkMagenta,
	DarkOliveGreen,
	DarkOrange,
	DarkOrchid,
	DarkRed,
	DarkSalmon,
	DarkSeaGreen,
	DarkSlateBlue,
	DarkSlateGray,
	DarkTurquoise,
	DarkViolet,
	DodgerBlue,
	Firebrick,
	FloralWhite,
	ForestGreen,
	Fuchsia,
	Gainsboro,
	GhostWhite,
	Gold,
	Goldenrod,
	Gray,
	Green,
	GreenYellow,
	Honeydew,
	HotPink,
	IndianRed,
	Indigo,
	Ivory,
	Khaki,
	Lavender,
	LavenderBlush,
	LawnGreen,
	LemonChiffon,
	Lime,
	LimeGreen,
	Linen,
	LightBlue,
	LightCoral,
	LightCyan,
	LightGoldenrodYellow,
	LightGray,
	LightGreen,
	LightPink,
	LightSalmon,
	LightSeaGreen,
	LightSkyBlue,
	LightSlateGray,
	LightSteelBlue,
	LightYellow,
	Magenta,
	Maroon,
	MediumAquamarine,
	MediumBlue,
	MediumOrchid,
	MediumPurple,
	MediumSeaGreen,
	MediumSlateBlue,
	MediumSpringGreen,
	MediumTurquoise,
	MediumVioletRed,
	MidnightBlue,
	MintCream,
	MistyRose,
	Moccasin,
	NavajoWhite,
	Navy,
	OldLace,
	Olive,
	OliveDrab,
	Orange,
	OrangeRed,
	Orchid,
	PaleGoldenrod,
	PaleGreen,
	PaleTurquoise,
	PaleVioletRed,
	PapayaWhip,
	PeachPuff,
	Peru,
	Pink,
	Plum,
	PowderBlue,
	Purple,
	Red,
	RosyBrown,
	RoyalBlue,
	SaddleBrown,
	Salmon,
	SandyBrown,
	SeaGreen,
	SeaShell,
	Sienna,
	Silver,
	SkyBlue,
	SlateBlue,
	SlateGray,
	Snow,
	SpringGreen,
	SteelBlue,
	Tan,
	Teal,
	Thistle,
	Tomato,
	Turquoise,
	Violet,
	Wheat,
	White,
	WhiteSmoke,
	Yellow,
	YellowGreen
}

export enum FillType
{
	SolidFill
}

export enum MarkerStyle
{
	Circle,
	Dash,
	Diamond,
	Dot,
	None,
	Picture,
	Plus,
	Square,
	Star,
	Triangle,
	X
}

export class WorkbookFont
{
	private name: string|null;
	private size: number; //point
	private bold: boolean;
	private italic: boolean;
	private underline: boolean;
	private color: number;
	private family: FontFamily;

	constructor();

	setName(name: string|null): void;
	setSize(size: number): void;
	setBold(bold: boolean): void;
	setItalic(italic: boolean): void;
	setUnderline(underline: boolean): void;
	setColor(color: number): void;
	setFamily(family: FontFamily): void;

	getName(): string|null;
	getSize(): number;
	isBold(): boolean;
	isItalic(): boolean;
	isUnderline(): boolean;
	getColor(): number;
	getFamily(): FontFamily;

	clone(): WorkbookFont;
	equals(font: WorkbookFont): boolean;
}

export class BorderStyle
{
	borderType: BorderType;
	borderColor: number;

	constructor(borderColor: number, borderType: BorderType);

	set(style: BorderStyle): void;
	clone(): BorderStyle;
	equals(style: BorderStyle): boolean;
}

export class CellStyle
{
	private index: number;
	private id: string|null;

	private halign: text.HAlignment;
	private valign: text.VAlignment;
	private wordWrap: boolean;

	private borderBottom: BorderStyle;
	private borderLeft: BorderStyle;
	private borderRight: BorderStyle;
	private borderTop: BorderStyle;

	private font: WorkbookFont|null;
	private fillColor: number;
	private fillPattern: FillPattern;
	private dataFormat: string|null;
	private protection: boolean;

	constructor(index: number);

	clone(): CellStyle;
	copyFrom(style: CellStyle): void;
	equals(style: CellStyle): boolean;

	setIndex(index: number): void;
	setID(id: string|null): void;
	setHAlign(halign: text.HAlignment): void;
	setVAlign(valign: text.VAlignment): void;
	setWordWrap(wordWrap: boolean): void;
	setFillColor(color: number, pattern: FillPattern): void;
	setFont(font: WorkbookFont|null): void;
	setBorderLeft(border: BorderStyle): void;
	setBorderRight(border: BorderStyle): void;
	setBorderTop(border: BorderStyle): void;
	setBorderBottom(border: BorderStyle): void;
	setDataFormat(dataFormat: string): void;

	getIndex(): number;
	getID(): string|null;
	getHAlign(): text.HAlignment;
	getVAlign(): text.VAlignment;
	getWordWrap(): boolean;
	getFillColor(): number;
	getFillPattern(): FillPattern;
	getFont(): WorkbookFont|null;
	getBorderLeft(): BorderStyle;
	getBorderRight(): BorderStyle;
	getBorderTop(): BorderStyle;
	getBorderBottom(): BorderStyle;
	getDataFormat(): string|null;
}

declare class CellData
{
	cdt: CellDataType;
	cellValue: string|null;
	style: CellStyle|null;
	mergeHori: number;
	mergeVert: number;
	hidden: boolean;
	cellURL: string|null;
}

declare class RowData
{
	style: CellStyle|null;
	cells: CellData[];
	height: number;
}

export interface IStyleCtrl
{
	getStyleIndex(style: CellStyle): number;
	getStyle(index: number): CellStyle|null;
	findOrCreateStyle(tmpStyle: CellStyle): CellStyle;
}

export class OfficeColor
{
	private colorType: ColorType;
	private color: number;

	constructor(colorType: ColorType, color: number);

	getColorType(): ColorType;
	getPresetColor(): PresetColor;
	getColorArgb(): number;

	static newPreset(color: PresetColor): OfficeColor;
	static newArgb(argb: number): OfficeColor;

	static presetColorGetArgb(color: PresetColor): number;
}

export class OfficeFill
{
	private fillType: FillType;
	private color: OfficeColor|null;

	constructor(fillType: FillType, color: OfficeColor|null);

	getFillType(): FillType;
	getColor(): OfficeColor|null;

	static newSolidFill(color: OfficeColor|null): OfficeFill;
}

export class OfficeLineStyle
{
	private fill: OfficeFill|null;

	constructor(fill: OfficeFill|null);

	getFillStyle(): OfficeFill|null;
}

export class OfficeShapeProp
{
	private fill: OfficeFill|null;
	private lineStyle: OfficeLineStyle|null;

	constructor(fill: OfficeFill|null, lineStyle: OfficeLineStyle|null);

	getFill(): OfficeFill|null;
	setFill(fill: OfficeFill|null): void;
	getLineStyle(): OfficeLineStyle|null;
	setLineStyle(lineStyle: OfficeLineStyle|null): void;
}

export class OfficeChartAxis
{
	private axisType: AxisType;
	private axisPos: AxisPosition;
	private title: string|null;
	private shapeProp: OfficeShapeProp|null;
	private majorGridProp: OfficeShapeProp|null;
	private tickLblPos: TickLabelPosition;
	private crosses: AxisCrosses;

	constructor(axisType: AxisType, axisPos: AxisPosition);

	getAxisType(): AxisType;
	getAxisPos(): AxisPosition;
	getTitle(): string|null;
	setTitle(title: string|null): void;
	getShapeProp(): OfficeShapeProp|null;
	setShapeProp(shapeProp: OfficeShapeProp|null): void;
	getMajorGridProp(): OfficeShapeProp|null;
	setMajorGridProp(majorGridProp: OfficeShapeProp|null): void;
	getTickLblPos(): TickLabelPosition;
	setTickLblPos(tickLblPos: TickLabelPosition): void;
	getCrosses(): AxisCrosses;
	setCrosses(axisCrosses: AxisCrosses): void;
}

export class WorkbookDataSource
{
	private sheet: Worksheet;
	private firstRow: number;
	private lastRow: number;
	private firstCol: number;
	private lastCol: number;

	constructor(sheet: Worksheet, firstRow: number, lastRow: number, firstCol: number, lastCol: number);

	toCodeRange(): string;
	getSheet(): Worksheet;
	getFirstRow(): number;
	getLastRow(): number;
	getFirstCol(): number;
	getLastCol(): number;
}

export class OfficeChartSeries
{
	private categoryData: WorkbookDataSource;
	private valueData: WorkbookDataSource;
	private title: string|null;
	private smooth: boolean;
	private shapeProp: OfficeShapeProp|null;
	private markerSize: number;
	private markerStyle: MarkerStyle;

	constructor(categoryData: WorkbookDataSource, valueData: WorkbookDataSource);

	getCategoryData(): WorkbookDataSource;
	getValueData(): WorkbookDataSource;
	getTitle(): string|null;
	setTitle(title: string|null): void;
	isSmooth(): boolean;
	setSmooth(smooth: boolean): void;
	getShapeProp(): OfficeShapeProp|null;
	setShapeProp(shapeProp: OfficeShapeProp|null): void;
	setLineStyle(lineStyle: OfficeLineStyle|null): void;
	getMarkerSize(): number;
	setMarkerSize(markerSize: number): void;
	getMarkerStyle(): MarkerStyle;
	setMarkerStyle(markerStyle: MarkerStyle): void;
}

class OfficeChart
{
	private static seriesColor: PresetColor[];

	private xInch: number;
	private yInch: number;
	private wInch: number;
	private hInch: number;
	private titleText: string|null;
	private shapeProp: OfficeShapeProp|null;
	private legend: boolean;
	private legendPos: LegendPos;
	private legendOverlay: boolean;
	private displayBlankAs: BlankAs;
	private chartType: ChartType;
	private categoryAxis: OfficeChartAxis|null;
	private valueAxis: OfficeChartAxis|null;
	private axes: OfficeChartAxis[];
	private series: OfficeChartSeries[];

	constructor(du: unit.Distance.Unit, x: number, y: number, w: number, h: number);
	
	getXInch(): number;
	getYInch(): number;
	getWInch(): number;
	getHInch(): number;

	setTitleText(titleText: string|null): void;
	getTitleText(): string|null;

	getShapeProp(): OfficeShapeProp|null;
	setShapeProp(shapeProp: OfficeShapeProp|null): void;
	addLegend(pos: LegendPos): void;
	hasLegend(): boolean;
	getLegendPos(): LegendPos;
	isLegendOverlay(): boolean;
	setDisplayBlankAs(displayBlankAs: BlankAs): void;
	getDisplayBlankAs(): BlankAs;

	initChart(chartType: ChartType, categoryAxis: OfficeChartAxis, valueAxis: OfficeChartAxis): void;
	initLineChart(leftAxisName: string|null, bottomAxisName: string|null, bottomAxisType: AxisType): void;
	getChartType(): ChartType;
	createAxis(axisType: AxisType, axisPos: AxisPosition): OfficeChartAxis;
	getAxisCount(): number;
	getAxis(index: number): OfficeChartAxis|null;
	getAxisIndex(axis: OfficeChartAxis): number;
	getCategoryAxis(): OfficeChartAxis|null;
	getValueAxis(): OfficeChartAxis|null;

	addSeries(categoryData: WorkbookDataSource, valueData: WorkbookDataSource, name: string|null, showMarker: boolean): void;
	getSeriesCount(): number;
	getSeriesNoCheck(index: number): OfficeChartSeries;
	getSeries(index: number): OfficeChartSeries|null;
}

declare class WorksheetDrawing
{
	anchorType: AnchorType;
	posXInch: number;
	posYInch: number;
	widthInch: number;
	heightInch: number;
	row1: number;
	col1: number;
	row2: number;
	col2: number;
	chart: OfficeChart|null;
}

export class Worksheet
{
	private name: string;
	private rows: RowData[];
	private colWidthsPt: number[];
	private drawings: any[]; //WorksheetDrawing[];
	private defColWidthPt: number;
	private defRowHeightPt: number;
	private freezeHori: number;
	private freezeVert: number;
	private marginLeft: number;
	private marginRight: number;
	private marginTop: number;
	private marginBottom: number;
	private marginHeader: number;
	private marginFooter: number;
	private options: number;
	private zoom: number;
	private maxCol: number;

	createRow(row: number): RowData|null;
	private getCellData(row: number, col: number, keepMerge: boolean): CellData|null;
	private cloneRow(row: rowData, srcCtrl: IStyleCtrl, newCtrl: IStyleCtrl): RowData;
	private cloneCell(cell: CellData, srcCtrl: IStyleCtrl, newCtrl: IStyleCtrl): CellData;

	constructor(name: string);

	clone(srcCtrl: IStyleCtrl, newCtrl: IStyleCtrl): Worksheet;

	setOptions(options: number): void;
	getOptions(): number;
	setFreezeHori(freezeHori: number): void;
	getFreezeHori(): number;
	setFreezeVert(freezeVert: number): void;
	getFreezeVert(): number;
	setMarginLeft(marginLeft: number): void; //Inch
	getMarginLeft(): number;
	setMarginRight(marginRight: number): void; //Inch
	getMarginRight(): number;
	setMarginTop(marginTop: number): void; //Inch
	getMarginTop(): number;
	setMarginBottom(marginBottom: number): void; //Inch
	getMarginBottom(): number;
	setMarginHeader(marginHeader: number): void; //Inch
	getMarginHeader(): number;
	setMarginFooter(marginFooter: number); //Inch
	getMarginFooter(): number;
	setZoom(zoom: number): void;
	getZoom(): number;
	isDefaultPageSetup(): boolean;
	setDefColWidthPt(defColWidthPt: number): void;
	getDefColWidthPt(): number;
	setDefRowHeightPt(defRowHeightPt: number): void;
	getDefRowHeightPt(): number;

	getName(): string;
	setCellStyle(row: number, col: number, style: CellStyle|null): boolean;
	setCellStyleHAlign(row: number, col: number, wb: IStyleCtrl, hAlign: text.HAlignment): boolean;
	setCellStyleBorderBottom(row: number, col: number, wb: IStyleCtrl, color: number, borderType: BorderType): boolean;
	setCellURL(row: number, col: number, url: string|null): boolean;
	setCellString(row: number, col: number, val: string, style?: CellStyle|null): boolean;
	setCellTS(row: number, col: number, val: data.Timestamp, style?: CellStyle|null): boolean;
	setCellDouble(row: number, col: number, val: number, style?: CellStyle|null): boolean;
	setCellInt32(row: number, col: number, val: number, style?: CellStyle|null): boolean;
	setCellInt64(row: number, col: number, val: number, style?: CellStyle|null): boolean;
	setCellEmpty(row: number, col: number, style?: CellStyle|null): boolean;
	mergeCells(row: number, col: number, height: number, width: number): boolean;
	setCellMergeLeft(row: number, col: number): boolean;
	setCellMergeUp(row: number, col: number): boolean;
	setRowHidden(row: number, hidden: boolean): boolean;
	setRowHeight(row: number, height: number): boolean;

	getCount(): number;
	getItem(row: number): RowData|null;
	removeCol(col: number): void;
	insertCol(col: number): void;
	getMaxCol(): number;

	setColWidth(col: number, width: number, du: unit.Distance.Unit): void;
	getColWidthCount(): number;
	getColWidthPt(col: number): number;
	getColWidth(col: number, du: unit.Distance.Unit): number;

	getCellDataRead(row: number, col: number): CellData|null;
	getCellString(cell: CellData): string|null;

	getDrawingCount(): number;
	getDrawing(index: number): WorksheetDrawing|null;
	getDrawingNoCheck(index: number): WorksheetDrawing;
	createDrawing(du: unit.Distance.Unit, x: number, y: number, w: number, h: number): WorksheetDrawing;
	createChart(du: unit.Distance.Unit, x: number, y: number, w: number, h: number, title: string|null): OfficeChart;
}

export class Workbook extends data.ParsedObject implements IStyleCtrl
{
	private author: string|null;
	private lastAuthor: string|null;
	private company: string|null;
	private createTime: data.Timestamp|null;
	private modifyTime: data.Timestamp|null;
	private version: number;
	private windowTopX: number;
	private windowTopY: number;
	private windowWidth: number;
	private windowHeight: number;
	private activeSheet: number;
	private palette: number[];

	private sheets: Worksheet[];
	private styles: CellStyle[];
	private fonts: WorkbookFont[];

	private static defPalette: number[];

	constructor();

	clone(): Workbook;
	addDefaultStyles(): void;

	setAuthor(author: string|null): void;
	setLastAuthor(lastAuthor: string|null): void;
	setCompany(company: string|null): void;
	setCreateTime(createTime: data.Timestamp|null): void;
	setModifyTime(modifyTime: data.Timestamp|null): void;
	setVersion(version: number): void;
	getAuthor(): string|null;
	getLastAuthor(): string|null;
	getCompany(): string|null;
	getCreateTime(): data.Timestamp|null;
	getModifyTime(): data.Timestamp|null;
	getVersion(): number;
	hasInfo(): boolean;

	setWindowTopX(windowTopX: number): void;
	setWindowTopY(windowTopY: number): void;
	setWindowWidth(windowWidth: number): void;
	setWindowHeight(windowHeight: number): void;
	setActiveSheet(index: number): void;
	getWindowTopX(): number;
	getWindowTopY(): number;
	getWindowWidth(): number;
	getWindowHeight(): number;
	getActiveSheet(): number;
	hasWindowInfo(): boolean;

	hasCellStyle(): boolean;
	newCellStyleDef(): CellStyle;
	newCellStyle(font: WorkbookFont|null, halign: text.HAlignment, valign: text.VAlignment, dataFormat: string|null): CellStyle;
	getStyleCount(): number;
	getStyleIndex(style: CellStyle): number;
	getStyle(index: number): CellStyle|null;
	findOrCreateStyle(tmpStyle: CellStyle): CellStyle;
	getDefaultStyle(): CellStyle|null;
	getPalette(): number[];
	setPalette(palette: number[]): void;

	addWorksheet(name: string|null): Worksheet;
	insertWorksheet(index: number, name: string): Worksheet;
	getCount(): number;
	getItem(index: number): Worksheet;
	removeAt(index: number): void;
	getWorksheetByName(name: string): Worksheet|null;

	getFontCount(): number;
	getFontNoCheck(index: number): WorkbookFont;
	getFont(index: number): WorkbookFont|null;
	getFontIndex(font: WorkbookFont): number;
	newFont(name: string|null, size: number, bold: boolean): WorkbookFont;

	static getDefPalette(): number[];
	static colCode(col: number): string;	
}

export class XLSUtil
{
	static date2Number(ts: data.Timestamp): number;
	static number2Timestamp(v: number): data.Timestamp;
	static getCellID(col: number, row: number): string;
}

export function cellDataTypeGetName(cdt: CellDataType): string;