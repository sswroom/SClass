import * as data from "../data.js";
import * as math from "../math.js";
import * as spreadsheet from "../spreadsheet.js";
import * as text from "../text.js";
import * as unit from "../unit.js";
import * as zip from "../zip.js";

export class XLSXExporter
{
	constructor()
	{
	}

	getName()
	{
		return "XLSXExporter";
	}

	/**
	 * @param {data.ParsedObject} pobj
	 */
	isObjectSupported(pobj)
	{
		return pobj instanceof spreadsheet.Workbook;
	}

	getOutputExt()
	{
		return "xlsx";
	}

	getOutputMIME()
	{
		return "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	}

	/**
	 * @param {string} fileName
	 * @param {data.ParsedObject} pobj
	 * @param {any} param
	 */
	exportFile(fileName, pobj, param)
	{
		if (!(pobj instanceof spreadsheet.Workbook))
			return null;
		let workbook = pobj;
		let sheet;
		let tmpStyle;
		/** @type {string[]} */
		let sb = [];
		/** @type {string[]} */
		let sbContTypes = [];
		let ts;
		let dt2;
		let t;
		let s;
		let s2;
		let zipFile = new zip.ZIPBuilder(zip.ZIPOS.MSDOS);;
		let i;
		let j;
		let k;
		let l;
		let m;
		let n;
		let drawingCnt = 0;
		let chartCnt = 0;
		let sharedStrings = [];
		let stringMap = {};
		ts = data.Timestamp.now();
	
		let dirXl = false;
		let dirXlWs = false;
		let dirXlWsRel = false;
		let dirXlDraw = false;
		let dirXlDrawRel = false;
		let dirXlChart = false;
		let dirRel = false;
		let dirXlRel = false;
	
		sbContTypes.push("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
		sbContTypes.push("<Types xmlns=\"http://schemas.openxmlformats.org/package/2006/content-types\">");
		sbContTypes.push("<Default Extension=\"xml\" ContentType=\"application/xml\"/>");
		sbContTypes.push("<Default Extension=\"rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>");
		sbContTypes.push("<Default Extension=\"png\" ContentType=\"image/png\"/>");
		sbContTypes.push("<Default Extension=\"jpeg\" ContentType=\"image/jpeg\"/>");
	
		let row;
		i = 0;
		j = workbook.getCount();
		while (i < j)
		{
			sheet = workbook.getItem(i);
			let maxRow = sheet.getCount();
			let maxCol = 1;
			k = 0;
			while (k < maxRow)
			{
				if ((row = sheet.getItem(k)) != null && row.cells.length > maxCol)
				{
					maxCol = row.cells.length;
				}
				k++;
			}
			/** @type {{row: number,col: number,cell: spreadsheet.CellData}[]} */
			let links = [];
			let link;
	
			sb = [];
			sb.push("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n");
			sb.push("<worksheet xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" xmlns:xdr=\"http://schemas.openxmlformats.org/drawingml/2006/spreadsheetDrawing\" xmlns:x14=\"http://schemas.microsoft.com/office/spreadsheetml/2009/9/main\" xmlns:xr2=\"http://schemas.microsoft.com/office/spreadsheetml/2015/revision2\" xmlns:mc=\"http://schemas.openxmlformats.org/markup-compatibility/2006\">");
			sb.push("<sheetPr filterMode=\"false\">");
			sb.push("<pageSetUpPr fitToPage=\"false\"/>");
			sb.push("</sheetPr>");
			sb.push("<dimension ref=\"A1");
			if (maxRow > 1 || maxCol > 1)
			{
				sb.push(':'+spreadsheet.XLSUtil.getCellID(maxCol - 1, maxRow - 1));
			}
			sb.push("\"/>");
			sb.push("<sheetViews>");
			sb.push("<sheetView showFormulas=\"false\" showGridLines=\"true\" showRowColHeaders=\"true\" showZeros=\"true\" rightToLeft=\"false\" tabSelected=\"true\" showOutlineSymbols=\"true\" defaultGridColor=\"true\" view=\"normal\" topLeftCell=\"A1\" colorId=\"64\" zoomScale=\"");
			sb.push(""+sheet.getZoom());
			sb.push("\" zoomScaleNormal=\"");
			sb.push(""+sheet.getZoom());
			sb.push("\" zoomScalePageLayoutView=\"");
			sb.push(""+sheet.getZoom());
			sb.push("\" workbookViewId=\"0\">");
			sb.push("<selection pane=\"topLeft\" activeCell=\"A1\" activeCellId=\"0\" sqref=\"A1\"/>");
			sb.push("</sheetView>");
			sb.push("</sheetViews>");
			sb.push("<sheetFormatPr defaultColWidth=\"");
			sb.push(""+(sheet.getDefColWidthPt() / 5.25));
			sb.push("\" defaultRowHeight=\"");
			sb.push(""+(sheet.getDefRowHeightPt()));
			sb.push("\" zeroHeight=\"false\" outlineLevelRow=\"0\" outlineLevelCol=\"0\"></sheetFormatPr>");
			let found = false;
	
			let lastColWidth = -1;
			let lastColIndex = -1;
	
			k = 0;
			l = sheet.getColWidthCount();
			while (k < l)
			{
				if (sheet.getColWidthPt(k) != lastColWidth)
				{
					if (lastColIndex != -1)
					{
						if (!found)
						{
							found = true;
							sb.push("<cols>");
						}
						sb.push("<col min=\"");
						sb.push(""+(lastColIndex + 10));
						sb.push("\" max=\"");
						sb.push(""+k);
						sb.push("\" width=\"");
						if (lastColWidth >= 0)
						{
							sb.push(""+(lastColWidth / 5.25));
							sb.push("\" customWidth=\"1\"");
						}
						else
						{
							sb.push(""+(sheet.getDefColWidthPt() / 5.25));
							sb.push("\" customWidth=\"false\"");
						}
						sb.push("/>");
					}
					lastColWidth = sheet.getColWidthPt(k);
					lastColIndex = k;
				}
				k++;
			}
			if (lastColWidth >= 0)
			{
				if (!found)
				{
					found = true;
					sb.push("<cols>");
				}
				sb.push("<col min=\"");
				sb.push(""+(lastColIndex + 1));
				sb.push("\" max=\"");
				sb.push(""+l);
				sb.push("\" width=\"");
				if (lastColWidth >= 0)
				{
					sb.push(""+(lastColWidth / 5.25));
					sb.push("\" customWidth=\"1\"");
				}
				else
				{
					sb.push(""+(sheet.getDefColWidthPt() / 5.25));
					sb.push("\" customWidth=\"false\"");
				}
				sb.push("/>");
			}
			if (l < sheet.getMaxCol())
			{
				if (!found)
				{
					found = true;
					sb.push("<cols>");
				}
				sb.push("<col min=\"");
				sb.push(""+(l + 1));
				sb.push("\" max=\"");
				sb.push(""+(sheet.getMaxCol()));
				sb.push("\" width=\"");
				sb.push(""+(sheet.getDefColWidthPt() / 5.25));
				sb.push("\" customWidth=\"false\" collapsed=\"false\" hidden=\"false\" outlineLevel=\"0\" style=\"0\"/>");
			}
			if (found)
			{
				sb.push("</cols>");
			}

			/** @type {math.RectArea[]} */
			let mergeList = [];
			k = 0;
			l = sheet.getCount();
			if (l > 0)
			{
				sb.push("<sheetData>");
				while (k < l)
				{
					if ((row = sheet.getItem(k)) != null)
					{
						sb.push("<row r=\"");
						sb.push(""+(k + 1));
						sb.push("\" customFormat=\"false\" ht=\"12.8\" hidden=\"false\" customHeight=\"false\" outlineLevel=\"0\" collapsed=\"false\">");
	
						m = 0;
						n = row.cells.length;
						while (m < n)
						{
							let cellValue;
							let cell;
							if ((cell = row.cells[m]) != null && (cellValue = cell.cellValue) != null && cell.cdt != spreadsheet.CellDataType.MergedLeft && cell.cdt != spreadsheet.CellDataType.MergedUp)
							{
								sb.push("<c r=\"");
								sb.push(spreadsheet.Workbook.colCode(m)+(k + 1));
								sb.push('"');
								if ((tmpStyle = cell.style) != null)
								{
									sb.push(" s=\"");
									sb.push(""+tmpStyle.getIndex());
									sb.push('"');
								}
								switch (cell.cdt)
								{
								case spreadsheet.CellDataType.String:
									sb.push(" t=\"s\"");
									break;
								case spreadsheet.CellDataType.Number:
								case spreadsheet.CellDataType.DateTime:
									sb.push(" t=\"n\"");
									break;
//								case spreadsheet.CellDataType.MergedLeft:
//								case spreadsheet.CellDataType.MergedUp:
//									break;
								}
								sb.push("><v>");
								switch (cell.cdt)
								{
								case spreadsheet.CellDataType.String:
									{
										let sIndex = stringMap[cellValue];
										if (sIndex == null)
										{
											sIndex = sharedStrings.length;
											sharedStrings.push(cellValue);
											stringMap[cellValue] = sIndex;
										}
										sb.push(""+sIndex);
									}
									break;
								case spreadsheet.CellDataType.Number:
									sb.push(cellValue);
									break;
								case spreadsheet.CellDataType.DateTime:
									{
										let dt = data.Timestamp.fromStr(cellValue);
										if (dt)
										{
											sb.push(""+spreadsheet.XLSUtil.date2Number(dt));
										}
									}
									break;
//								case spreadsheet.CellDataType.MergedLeft:
//								case spreadsheet.CellDataType.MergedUp:
//									break;
								}
								
								if (cell.cellURL != null)
								{
									link = {row: k, col: m, cell: cell};
									links.push(link);
								}
								if (cell.mergeHori > 1 || cell.mergeVert > 1)
								{
									mergeList.push(new math.RectArea(m, k, cell.mergeHori, cell.mergeVert));
								}
								
								sb.push("</v></c>");
							}
							else if ((cell = row.cells[m]) != null && (tmpStyle = cell.style) != null)
							{
								sb.push("<c r=\"")
								sb.push(spreadsheet.Workbook.colCode(m) + (k + 1));
								sb.push('"');
								sb.push(" s=\"");
								sb.push(""+tmpStyle.getIndex());
								sb.push('"');
								sb.push("></c>");
							}
							m++;
						}
						sb.push("</row>");
					}
	
					k++;
				}
				sb.push("</sheetData>");
	
				if (mergeList.length > 0)
				{
					sb.push("<mergeCells count=\"");
					sb.push(""+mergeList.length);
					sb.push("\">");
					k = 0;
					l = mergeList.length;
					while (k < l)
					{
						sb.push("<mergeCell ref=\"");
						let rect = mergeList[k];
						sb.push(spreadsheet.XLSUtil.getCellID(rect.min.x, rect.min.y) + ':' + spreadsheet.XLSUtil.getCellID(rect.max.x - 1, rect.max.y - 1));
						sb.push("\"/>");
						k++;
					}
					sb.push("</mergeCells>");
				}
	
				if (links.length > 0)
				{
					let idBase = sheet.getDrawingCount() + 1;
					sb.push("<hyperlinks>");
					m = 0;
					n = links.length;
					while (m < n)
					{
						link = links[m];
						sb.push("<hyperlink ref=\"");
						sb.push(spreadsheet.Workbook.colCode(link.col));
						sb.push(""+(link.row + 1));
						sb.push("\" r:id=\"rId");
						sb.push(""+(idBase + m));
						sb.push("\" display=");
						s = text.toAttrText(link.cell.cellValue||"");
						sb.push(s);
						sb.push("/>");
						m++;
					}
					sb.push("</hyperlinks>");
				}
			}
			else
			{
				sb.push("<sheetData/>");
			}
			//<sheetProtection sheet="true" password="cc1a" objects="true" scenarios="true"/><printOptions headings="false" gridLines="false" gridLinesSet="true" horizontalCentered="false" verticalCentered="false"/>
			sb.push("<pageMargins left=\"");
			sb.push(""+sheet.getMarginLeft());
			sb.push("\" right=\"");
			sb.push(""+sheet.getMarginRight());
			sb.push("\" top=\"");
			sb.push(""+sheet.getMarginTop());
			sb.push("\" bottom=\"");
			sb.push(""+sheet.getMarginBottom());
			sb.push("\" header=\"");
			sb.push(""+sheet.getMarginHeader());
			sb.push("\" footer=\"");
			sb.push(""+sheet.getMarginFooter());
			sb.push("\"/>");
			sb.push("<pageSetup paperSize=\"9\" scale=\"100\" firstPageNumber=\"1\" fitToWidth=\"1\" fitToHeight=\"1\" pageOrder=\"downThenOver\" orientation=\"portrait\" blackAndWhite=\"false\" draft=\"false\" cellComments=\"none\" useFirstPageNumber=\"true\" horizontalDpi=\"300\" verticalDpi=\"300\" copies=\"1\"/>");
			sb.push("<headerFooter differentFirst=\"false\" differentOddEven=\"false\">");
			sb.push("<oddHeader>&amp;C&amp;&quot;Times New Roman,Regular&quot;&amp;12&amp;A</oddHeader>");
			sb.push("<oddFooter>&amp;C&amp;&quot;Times New Roman,Regular&quot;&amp;12Page &amp;P</oddFooter>");
			sb.push("</headerFooter>");
			k = 0;
			l = sheet.getDrawingCount();
			while (k < l)
			{
				sb.push("<drawing r:id=\"rId");
				sb.push(""+(k + 1));
				sb.push("\"/>");
				k++;
			}
			sb.push("</worksheet>");
			if (!dirXl)
			{
				zipFile.addDir("xl/", ts, ts, ts, 0);
				dirXl = true;
			}
			if (!dirXlWs)
			{
				zipFile.addDir("xl/worksheets/", ts, ts, ts, 0);
				dirXlWs = true;
			}
			s = "xl/worksheets/sheet" + (i + 1) + ".xml";
			zipFile.addFile(s, new TextEncoder().encode(sb.join("")), ts, ts, ts, 0);
			sbContTypes.push("<Override PartName=\"/");
			sbContTypes.push(s);
			sbContTypes.push("\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.worksheet+xml\"/>");
	
			if (sheet.getDrawingCount() > 0 || links.length > 0)
			{
				sb = [];
				sb.push("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>");
				sb.push("<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">");
				k = 0;
				l = sheet.getDrawingCount();
				while (k < l)
				{
					sb.push("<Relationship Id=\"rId");
					sb.push(""+(k + 1));
					sb.push("\" Target=\"../drawings/drawing");
					sb.push(""+(k + 1 + drawingCnt));
					sb.push(".xml\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/drawing\"/>");
					k++;
				}
				m = 0;
				n = links.length;
				while (m < n)
				{
					link = links[m];
					sb.push("<Relationship Id=\"rId");
					sb.push(""+(l + m + 1));
					sb.push("\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/hyperlink\" Target=");
					s = text.toAttrText(link.cell.cellURL || "");
					sb.push(s);
					sb.push(" TargetMode=\"External\"/>");
					m++;
				}
				sb.push("</Relationships>");
	
				if (!dirXlWsRel)
				{
					dirXlWsRel = true;
					zipFile.addDir("xl/worksheets/_rels/", ts, ts, ts, 0);
				}
				s = "xl/worksheets/_rels/sheet" + (i + 1) + ".xml.rels";
				zipFile.addFile(s, new TextEncoder().encode(sb.join("")), ts, ts, ts, 0);
				sbContTypes.push("<Override PartName=\"/");
				sbContTypes.push(s);
				sbContTypes.push("\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>");
	
				k = 0;
				l = sheet.getDrawingCount();
				while (k < l)
				{
					let chart;
					let drawing = sheet.getDrawingNoCheck(k);
					sb = [];
					sb.push("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
					sb.push("<xdr:wsDr xmlns:xdr=\"http://schemas.openxmlformats.org/drawingml/2006/spreadsheetDrawing\" xmlns:a=\"http://schemas.openxmlformats.org/drawingml/2006/main\" xmlns:c=\"http://schemas.openxmlformats.org/drawingml/2006/chart\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\">");
					switch (drawing.anchorType)
					{
					case spreadsheet.AnchorType.Absolute:
						sb.push("<xdr:absoluteAnchor>");
						sb.push("<xdr:pos x=\"");
						sb.push(""+ Math.round(unit.Distance.convert(unit.Distance.Unit.INCH, unit.Distance.Unit.EMU, drawing.posXInch)));
						sb.push("\" y=\"");
						sb.push(""+ Math.round(unit.Distance.convert(unit.Distance.Unit.INCH, unit.Distance.Unit.EMU, drawing.posYInch)));
						sb.push("\"/>");
						sb.push("<xdr:ext cx=\"");
						sb.push(""+ Math.round(unit.Distance.convert(unit.Distance.Unit.INCH, unit.Distance.Unit.EMU, drawing.widthInch)));
						sb.push("\" cy=\"");
						sb.push(""+ Math.round(unit.Distance.convert(unit.Distance.Unit.INCH, unit.Distance.Unit.EMU, drawing.heightInch)));
						sb.push("\"/>");
						break;
					case spreadsheet.AnchorType.OneCell:
						sb.push("<xdr:oneCellAnchor>");
						sb.push("<xdr:from>");
						sb.push("<xdr:col>");
						sb.push(""+(drawing.col1 + 1));
						sb.push("</xdr:col>");
						sb.push("<xdr:colOff>");
						sb.push(""+ Math.round(unit.Distance.convert(unit.Distance.Unit.INCH, unit.Distance.Unit.EMU, drawing.posXInch)));
						sb.push("</xdr:colOff>");
						sb.push("<xdr:row>");
						sb.push(""+(drawing.row1 + 1));
						sb.push("</xdr:row>");
						sb.push("<xdr:rowOff>");
						sb.push(""+ Math.round(unit.Distance.convert(unit.Distance.Unit.INCH, unit.Distance.Unit.EMU, drawing.posYInch)));
						sb.push("</xdr:rowOff>");
						sb.push("</xdr:from>");
						sb.push("<xdr:ext cx=\"");
						sb.push(""+ Math.round(unit.Distance.convert(unit.Distance.Unit.INCH, unit.Distance.Unit.EMU, drawing.widthInch)));
						sb.push("\" cy=\"");
						sb.push(""+ Math.round(unit.Distance.convert(unit.Distance.Unit.INCH, unit.Distance.Unit.EMU, drawing.heightInch)));
						sb.push("\"/>");
						break;
					case spreadsheet.AnchorType.TwoCell:
						sb.push("<xdr:twoCellAnchor editAs=\"twoCell\">");
						sb.push("<xdr:from>");
						sb.push("<xdr:col>");
						sb.push(""+(drawing.col1 + 1));
						sb.push("</xdr:col>");
						sb.push("<xdr:colOff>");
						sb.push(""+ Math.round(unit.Distance.convert(unit.Distance.Unit.INCH, unit.Distance.Unit.EMU, drawing.posXInch)));
						sb.push("</xdr:colOff>");
						sb.push("<xdr:row>");
						sb.push(""+(drawing.row1 + 1));
						sb.push("</xdr:row>");
						sb.push("<xdr:rowOff>");
						sb.push(""+ Math.round(unit.Distance.convert(unit.Distance.Unit.INCH, unit.Distance.Unit.EMU, drawing.posYInch)));
						sb.push("</xdr:rowOff>");
						sb.push("</xdr:from>");
						sb.push("<xdr:to>");
						sb.push("<xdr:col>");
						sb.push(""+(drawing.col2 + 1));
						sb.push("</xdr:col>");
						sb.push("<xdr:colOff>");
						sb.push(""+ Math.round(unit.Distance.convert(unit.Distance.Unit.INCH, unit.Distance.Unit.EMU, drawing.widthInch)));
						sb.push("</xdr:colOff>");
						sb.push("<xdr:row>");
						sb.push(""+(drawing.row2 + 1));
						sb.push("</xdr:row>");
						sb.push("<xdr:rowOff>");
						sb.push(""+ Math.round(unit.Distance.convert(unit.Distance.Unit.INCH, unit.Distance.Unit.EMU, drawing.heightInch)));
						sb.push("</xdr:rowOff>");
						sb.push("</xdr:to>");
						break;
					}
					if ((chart = drawing.chart) != null)
					{
						sb.push("<xdr:graphicFrame>");
						sb.push("<xdr:nvGraphicFramePr>");
						sb.push("<xdr:cNvPr id=\"");
						sb.push(""+chartCnt);
						sb.push("\" name=\"Diagramm");
						sb.push(""+chartCnt);
						sb.push("\"/>");
						sb.push("<xdr:cNvGraphicFramePr/>");
						sb.push("</xdr:nvGraphicFramePr>");
						sb.push("<xdr:xfrm>");
						sb.push("<a:off x=\"");
						sb.push(""+ Math.round(unit.Distance.convert(unit.Distance.Unit.INCH, unit.Distance.Unit.EMU, chart.getXInch())));
						sb.push("\" y=\"");
						sb.push(""+ Math.round(unit.Distance.convert(unit.Distance.Unit.INCH, unit.Distance.Unit.EMU, chart.getYInch())));
						sb.push("\"/>");
						sb.push("<a:ext cx=\"");
						sb.push(""+ Math.round(unit.Distance.convert(unit.Distance.Unit.INCH, unit.Distance.Unit.EMU, chart.getWInch())));
						sb.push("\" cy=\"");
						sb.push(""+ Math.round(unit.Distance.convert(unit.Distance.Unit.INCH, unit.Distance.Unit.EMU, chart.getHInch())));
						sb.push("\"/>");
						sb.push("</xdr:xfrm>");
						sb.push("<a:graphic>");
						sb.push("<a:graphicData uri=\"http://schemas.openxmlformats.org/drawingml/2006/chart\">");
						sb.push("<c:chart r:id=\"rId");
						sb.push(""+(chartCnt + 1));
						sb.push("\"/>");
						sb.push("</a:graphicData>");
						sb.push("</a:graphic>");
						sb.push("</xdr:graphicFrame>");
					}
					else
					{
						///////////////////////////////////////
					}
					sb.push("<xdr:clientData/>");
					switch (drawing.anchorType)
					{
					case spreadsheet.AnchorType.Absolute:
						sb.push("</xdr:absoluteAnchor>");
						break;
					case spreadsheet.AnchorType.OneCell:
						sb.push("</xdr:oneCellAnchor>");
						break;
					case spreadsheet.AnchorType.TwoCell:
						sb.push("</xdr:twoCellAnchor>");
						break;
					}
					sb.push("</xdr:wsDr>");
					drawingCnt++;
					if (!dirXlDraw)
					{
						dirXlDraw = true;
						zipFile.addDir("xl/drawings/", ts, ts, ts, 0);
					}
					s = "xl/drawings/drawing" + drawingCnt +".xml";
					zipFile.addFile(s, new TextEncoder().encode(sb.join("")), ts, ts, ts, 0);
					sbContTypes.push("<Override PartName=\"/");
					sbContTypes.push(s);
					sbContTypes.push("\" ContentType=\"application/vnd.openxmlformats-officedocument.drawing+xml\"/>");
	
					if ((chart = drawing.chart) != null)
					{
						sb = [];
						sb.push("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>");
						sb.push("<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">");
						sb.push("<Relationship Id=\"rId1\" Target=\"../charts/chart");
						sb.push(""+(chartCnt + 1));
						sb.push(".xml\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/chart\"/>");
						sb.push("</Relationships>");
	
						if (!dirXlDrawRel)
						{
							dirXlDrawRel = true;
							zipFile.addDir("xl/drawings/_rels/", ts, ts, ts, 0);
						}
						s = "xl/drawings/_rels/drawing"+ drawingCnt+".xml.rels";
						zipFile.addFile(s, new TextEncoder().encode(sb.join("")), ts, ts, ts, 0);
						sbContTypes.push("<Override PartName=\"/");
						sbContTypes.push(s);
						sbContTypes.push("\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>");
	
						chartCnt++;
						sb = [];
						sb.push("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
						sb.push("<c:chartSpace xmlns:c=\"http://schemas.openxmlformats.org/drawingml/2006/chart\" xmlns:a=\"http://schemas.openxmlformats.org/drawingml/2006/main\">");
						sb.push("<c:chart>");
						if ((s = chart.getTitleText()) != null)
						{
							XLSXExporter.appendTitle(sb, s);
						}
						sb.push("<c:plotArea>");
						sb.push("<c:layout/>");
						if (chart.getChartType() != spreadsheet.ChartType.Unknown)
						{
							let nnaxis;
							switch (chart.getChartType())
							{
							case spreadsheet.ChartType.LineChart:
								sb.push("<c:lineChart>");
								break;
							case spreadsheet.ChartType.Unknown:
								break;
							}
							m = 0;
							n = chart.getSeriesCount();
							while (m < n)
							{
								XLSXExporter.appendSeries(sb, chart.getSeriesNoCheck(m), m);
								m++;
							}
							if ((nnaxis = chart.getCategoryAxis()) != null)
							{
								sb.push("<c:axId val=\"");
								sb.push(""+chart.getAxisIndex(nnaxis));
								sb.push("\"/>");
							}
							if ((nnaxis = chart.getValueAxis()) != null)
							{
								sb.push("<c:axId val=\"");
								sb.push(""+ chart.getAxisIndex(nnaxis));
								sb.push("\"/>");
							}
							switch (chart.getChartType())
							{
							case spreadsheet.ChartType.LineChart:
								sb.push("</c:lineChart>");
								break;
							case spreadsheet.ChartType.Unknown:
								break;
							}
						}
						m = 0;
						n = chart.getAxisCount();
						while (m < n)
						{
							XLSXExporter.appendAxis(sb, chart.getAxis(m), m);
							m++;
						}
						XLSXExporter.appendShapeProp(sb, chart.getShapeProp());
						sb.push("</c:plotArea>");
						if (chart.hasLegend())
						{
							sb.push("<c:legend>");
							sb.push("<c:legendPos val=\"");
							switch (chart.getLegendPos())
							{
							case spreadsheet.LegendPos.Bottom:
								sb.push("b");
								break;
							}
							sb.push("\"/>");
							sb.push("<c:overlay val=\"");
							if (chart.isLegendOverlay())
							{
								sb.push("true");
							}
							else
							{
								sb.push("false");
							}
							sb.push("\"/>");
							sb.push("</c:legend>");
						}
						sb.push("<c:plotVisOnly val=\"true\"/>");
						switch (chart.getDisplayBlankAs())
						{
						case spreadsheet.BlankAs.Default:
							break;
						case spreadsheet.BlankAs.Gap:
							sb.push("<c:dispBlanksAs val=\"gap\"/>");
							break;
						case spreadsheet.BlankAs.Zero:
							sb.push("<c:dispBlanksAs val=\"zero\"/>");
							break;
						}
						sb.push("</c:chart>");
						//////////////////////////////////////
						sb.push("</c:chartSpace>");
	
						if (!dirXlChart)
						{
							dirXlChart = true;
							zipFile.addDir("xl/charts/", ts, ts, ts, 0);
						}
						s = "xl/charts/chart" + chartCnt + ".xml";
						zipFile.addFile(s, new TextEncoder().encode(sb.join("")), ts, ts, ts, 0);
						sbContTypes.push("<Override PartName=\"/");
						sbContTypes.push(s);
						sbContTypes.push("\" ContentType=\"application/vnd.openxmlformats-officedocument.drawingml.chart+xml\"/>");
					}
					k++;
				}
			}
			i++;
		}
	
		sb = [];
		sb.push("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n");
		sb.push("<workbook xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\">");
		sb.push("<fileVersion appName=\"AVIRead\"/>");
		sb.push("<workbookPr backupFile=\"false\" showObjects=\"all\" date1904=\"false\"/>");
		sb.push("<workbookProtection/>");
		sb.push("<bookViews>");
		sb.push("<workbookView showHorizontalScroll=\"true\" showVerticalScroll=\"true\" showSheetTabs=\"true\" xWindow=\"0\" yWindow=\"0\" windowWidth=\"16384\" windowHeight=\"8192\" tabRatio=\"500\" firstSheet=\"0\" activeTab=\"0\"/>");
		sb.push("</bookViews>");
		sb.push("<sheets>");
		i = 0;
		j = workbook.getCount();
		while (i < j)
		{
			sheet = workbook.getItem(i);
			sb.push("<sheet name=");
			s = sheet.getName();
			s2 = text.toAttrText(s);
			sb.push(s2);
			sb.push(" sheetId=\"");
			sb.push(""+(i + 1));
			sb.push("\" state=\"visible\" r:id=\"rId");
			sb.push(""+(i + 2));
			sb.push("\"/>");
			i++;
		}
		sb.push("</sheets>");
		sb.push("<calcPr iterateCount=\"100\" refMode=\"A1\" iterate=\"false\" iterateDelta=\"0.001\"/>");
		sb.push("</workbook>");
		if (!dirXl)
		{
			zipFile.addDir("xl/", ts, ts, ts, 0);
			dirXl = true;
		}
		zipFile.addFile("xl/workbook.xml", new TextEncoder().encode(sb.join("")), ts, ts, ts, 0);
		sbContTypes.push("<Override PartName=\"/xl/workbook.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet.main+xml\"/>");
	
		sb = [];
		sb.push("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
		sb.push("<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">");
		sb.push("<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument\" Target=\"xl/workbook.xml\"/>");
		sb.push("<Relationship Id=\"rId2\" Type=\"http://schemas.openxmlformats.org/package/2006/relationships/metadata/core-properties\" Target=\"docProps/core.xml\"/>");
		sb.push("<Relationship Id=\"rId3\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/extended-properties\" Target=\"docProps/app.xml\"/>");
		sb.push("\n</Relationships>");
		if (!dirRel)
		{
			zipFile.addDir("_rels/", ts, ts, ts, 0);
			dirRel = true;
		}
		zipFile.addFile("_rels/.rels", new TextEncoder().encode(sb.join("")), ts, ts, ts, 0);
		sbContTypes.push("<Override PartName=\"/_rels/.rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>");
	
		sb = [];
		sb.push("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n");
		sb.push("<styleSheet xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\">");
		{
			/** @type {{ [x: string]: number; }} */
			let numFmtMap = {};
			/** @type {string[]} */
			let numFmts = [];
			let borders = [];
			let borderNone = new spreadsheet.BorderStyle(0, spreadsheet.BorderType.None);
			let border = {left: borderNone, top: borderNone, right: borderNone, bottom: borderNone};
			borders.push(border);
	
			s = "general";
			numFmtMap[s] = numFmts.length;;
			numFmts.push(s);
	
			i = 0;
			j = workbook.getStyleCount();
			while (i < j)
			{
				let style = workbook.getStyle(i);
				if (style == null)
					throw new Error("Style is null");
				if ((s = style.getDataFormat()) == null)
				{
					s = "general";
				}
				if (numFmtMap[s] === undefined)
				{
					numFmtMap[s] = numFmts.length;
					numFmts.push(s);
				}
				let borderFound = false;
				k = borders.length;
				while (k-- > 0)
				{
					border = borders[k];
					if (border.left.equals(style.getBorderLeft()) &&
						border.top.equals(style.getBorderTop()) &&
						border.right.equals(style.getBorderRight()) &&
						border.bottom.equals(style.getBorderBottom()))
					{
						borderFound = true;
						break;
					}
				}
				if (!borderFound)
				{
					border = {left: style.getBorderLeft(), top: style.getBorderTop(), right: style.getBorderRight(), bottom: style.getBorderBottom()};
					borders.push(border);
				}
				i++;
			}
			if (numFmts.length > 0)
			{
				sb.push("<numFmts count=\"");
				sb.push(""+numFmts.length);
				sb.push("\">");
				i = 0;
				k = numFmts.length;
				while (i < k)
				{
					sb.push("<numFmt numFmtId=\"");
					sb.push(""+(i + 164));
					sb.push("\" formatCode=");
					s = text.toAttrText(XLSXExporter.toFormatCode(numFmts[i]));
					sb.push(s);
					sb.push("/>");
					i++;
				}
				sb.push("</numFmts>");
			}
			if (workbook.getFontCount() > 0)
			{
				sb.push("<fonts count=\"");
				sb.push(""+workbook.getFontCount());
				sb.push("\">");
				i = 0;
				k = workbook.getFontCount();
				while (i < k)
				{
					let font = workbook.getFontNoCheck(i);
					sb.push("<font>");
					if (font.getSize() != 0)
					{
						sb.push("<sz val=\"");
						sb.push(""+font.getSize());
						sb.push("\"/>");
					}
					if ((s = font.getName()) != null)
					{
						sb.push("<name val=");
						s = text.toAttrText(s);
						sb.push(s);
						sb.push("/>");
					}
					switch (font.getFamily())
					{
					case spreadsheet.FontFamily.NA:
						sb.push("<family val=\"0\"/>");
						break;
					case spreadsheet.FontFamily.Roman:
						sb.push("<family val=\"1\"/>");
						break;
					case spreadsheet.FontFamily.Swiss:
						sb.push("<family val=\"2\"/>");
						break;
					case spreadsheet.FontFamily.Modern:
						sb.push("<family val=\"3\"/>");
						break;
					case spreadsheet.FontFamily.Script:
						sb.push("<family val=\"4\"/>");
						break;
					case spreadsheet.FontFamily.Decorative:
						sb.push("<family val=\"5\"/>");
						break;
					}
					sb.push("</font>");
					i++;
				}
				sb.push("</fonts>");
			}
	
			sb.push("<fills count=\"2\">");
			sb.push("<fill>");
			sb.push("<patternFill patternType=\"none\"/>");
			sb.push("</fill>");
			sb.push("<fill>");
			sb.push("<patternFill patternType=\"gray125\"/>");
			sb.push("</fill>");
			sb.push("</fills>");
	
			i = 0;
			k = borders.length;
			sb.push("<borders count=\"");
			sb.push(""+k);
			sb.push("\">");
			while (i < k)
			{
				border = borders[i];
				sb.push("<border diagonalUp=\"false\" diagonalDown=\"false\">");
				XLSXExporter.appendBorder(sb, border.left, "left");
				XLSXExporter.appendBorder(sb, border.right, "right");
				XLSXExporter.appendBorder(sb, border.top, "top");
				XLSXExporter.appendBorder(sb, border.bottom, "bottom");
				sb.push("<diagonal/>");
				sb.push("</border>");
				i++;
			}
			sb.push("</borders>");
	
			sb.push("<cellStyleXfs count=\"20\">");
			sb.push("<xf numFmtId=\"164\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"true\" applyAlignment=\"true\" applyProtection=\"true\">");
			sb.push("<alignment horizontal=\"general\" vertical=\"bottom\" textRotation=\"0\" wrapText=\"false\" indent=\"0\" shrinkToFit=\"false\"/>");
			sb.push("<protection locked=\"true\" hidden=\"false\"/>");
			sb.push("</xf>");
			sb.push("<xf numFmtId=\"1\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
			sb.push("</xf>");
			sb.push("<xf numFmtId=\"1\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
			sb.push("</xf>");
			sb.push("<xf numFmtId=\"2\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
			sb.push("</xf>");
			sb.push("<xf numFmtId=\"2\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
			sb.push("</xf>");
			sb.push("<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
			sb.push("</xf>");
			sb.push("<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
			sb.push("</xf>");
			sb.push("<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
			sb.push("</xf>");
			sb.push("<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
			sb.push("</xf>");
			sb.push("<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
			sb.push("</xf>");
			sb.push("<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
			sb.push("</xf>");
			sb.push("<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
			sb.push("</xf>");
			sb.push("<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
			sb.push("</xf>");
			sb.push("<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
			sb.push("</xf>");
			sb.push("<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
			sb.push("</xf>");
			sb.push("<xf numFmtId=\"43\" fontId=\"1\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
			sb.push("</xf>");
			sb.push("<xf numFmtId=\"41\" fontId=\"1\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
			sb.push("</xf>");
			sb.push("<xf numFmtId=\"44\" fontId=\"1\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
			sb.push("</xf>");
			sb.push("<xf numFmtId=\"42\" fontId=\"1\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
			sb.push("</xf>");
			sb.push("<xf numFmtId=\"9\" fontId=\"1\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">");
			sb.push("</xf>");
			sb.push("</cellStyleXfs>");
	
			if (workbook.getStyleCount() > 0)
			{
				sb.push("<cellXfs count=\"");
				sb.push(""+workbook.getStyleCount());
				sb.push("\">");
				i = 0;
				k = workbook.getStyleCount();
				while (i < k)
				{
					let style;
					if ((style = workbook.getStyle(i)) != null)
					{
						XLSXExporter.appendXF(sb, style, borders, workbook, numFmtMap);
					}
					i++;
				}
				sb.push("</cellXfs>");
			}
	
			sb.push("<cellStyles count=\"6\">");
			sb.push("<cellStyle name=\"Normal\" xfId=\"0\" builtinId=\"0\"/>");
			sb.push("<cellStyle name=\"Comma\" xfId=\"15\" builtinId=\"3\"/>");
			sb.push("<cellStyle name=\"Comma [0]\" xfId=\"16\" builtinId=\"6\"/>");
			sb.push("<cellStyle name=\"Currency\" xfId=\"17\" builtinId=\"4\"/>");
			sb.push("<cellStyle name=\"Currency [0]\" xfId=\"18\" builtinId=\"7\"/>");
			sb.push("<cellStyle name=\"Percent\" xfId=\"19\" builtinId=\"5\"/>");
			sb.push("</cellStyles>");
		}
		sb.push("</styleSheet>");
		zipFile.addFile("xl/styles.xml", new TextEncoder().encode(sb.join("")), ts, ts, ts, 0);
		sbContTypes.push("<Override PartName=\"/xl/styles.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.styles+xml\"/>");
	
		if (sharedStrings.length > 0)
		{
			sb = [];
			sb.push("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n");
			sb.push("<sst xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\" count=\"");
			sb.push(""+sharedStrings.length);
			sb.push("\" uniqueCount=\"");
			sb.push(""+sharedStrings.length);
			sb.push("\">");
			i = 0;
			k = sharedStrings.length;
			while (i < k)
			{
				sb.push("<si><t xml:space=\"preserve\">");
				s = text.toXMLText(sharedStrings[i]);
				sb.push(s);
				sb.push("</t></si>");
				i++;
			}
			sb.push("</sst>");
			zipFile.addFile("xl/sharedStrings.xml", new TextEncoder().encode(sb.join("")), ts, ts, ts,0);
			sbContTypes.push("<Override PartName=\"/xl/sharedStrings.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.sharedStrings+xml\"/>");
		}
	
		sb = [];
		sb.push("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n");
		sb.push("<a:theme xmlns:a=\"http://schemas.openxmlformats.org/drawingml/2006/main\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" name=\"Office\">");
		sb.push("<a:themeElements><a:clrScheme name=\"LibreOffice\"><a:dk1><a:srgbClr val=\"000000\"/></a:dk1><a:lt1><a:srgbClr val=\"ffffff\"/></a:lt1><a:dk2><a:srgbClr val=\"000000\"/></a:dk2>");
		sb.push("<a:lt2><a:srgbClr val=\"ffffff\"/></a:lt2><a:accent1><a:srgbClr val=\"18a303\"/></a:accent1><a:accent2><a:srgbClr val=\"0369a3\"/></a:accent2>");
		sb.push("<a:accent3><a:srgbClr val=\"a33e03\"/></a:accent3><a:accent4><a:srgbClr val=\"8e03a3\"/></a:accent4><a:accent5><a:srgbClr val=\"c99c00\"/></a:accent5>");
		sb.push("<a:accent6><a:srgbClr val=\"c9211e\"/></a:accent6><a:hlink><a:srgbClr val=\"0000ee\"/></a:hlink><a:folHlink><a:srgbClr val=\"551a8b\"/></a:folHlink></a:clrScheme>");
		sb.push("<a:fontScheme name=\"Office\"><a:majorFont><a:latin typeface=\"Arial\" pitchFamily=\"0\" charset=\"1\"/><a:ea typeface=\"DejaVu Sans\" pitchFamily=\"0\" charset=\"1\"/>");
		sb.push("<a:cs typeface=\"DejaVu Sans\" pitchFamily=\"0\" charset=\"1\"/></a:majorFont><a:minorFont><a:latin typeface=\"Arial\" pitchFamily=\"0\" charset=\"1\"/><a:ea typeface=\"DejaVu Sans\" pitchFamily=\"0\" charset=\"1\"/>");
		sb.push("<a:cs typeface=\"DejaVu Sans\" pitchFamily=\"0\" charset=\"1\"/></a:minorFont></a:fontScheme><a:fmtScheme><a:fillStyleLst><a:solidFill><a:schemeClr val=\"phClr\"></a:schemeClr></a:solidFill>");
		sb.push("<a:solidFill><a:schemeClr val=\"phClr\"></a:schemeClr></a:solidFill><a:solidFill><a:schemeClr val=\"phClr\"></a:schemeClr></a:solidFill></a:fillStyleLst>");
		sb.push("<a:lnStyleLst><a:ln w=\"6350\" cap=\"flat\" cmpd=\"sng\" algn=\"ctr\"><a:prstDash val=\"solid\"/><a:miter/></a:ln><a:ln w=\"6350\" cap=\"flat\" cmpd=\"sng\" algn=\"ctr\">");
		sb.push("<a:prstDash val=\"solid\"/><a:miter/></a:ln><a:ln w=\"6350\" cap=\"flat\" cmpd=\"sng\" algn=\"ctr\"><a:prstDash val=\"solid\"/><a:miter/></a:ln></a:lnStyleLst>");
		sb.push("<a:effectStyleLst><a:effectStyle><a:effectLst/></a:effectStyle><a:effectStyle><a:effectLst/></a:effectStyle><a:effectStyle><a:effectLst/></a:effectStyle></a:effectStyleLst><a:bgFillStyleLst>");
		sb.push("<a:solidFill><a:schemeClr val=\"phClr\"></a:schemeClr></a:solidFill><a:solidFill><a:schemeClr val=\"phClr\"></a:schemeClr></a:solidFill><a:solidFill><a:schemeClr val=\"phClr\"></a:schemeClr></a:solidFill></a:bgFillStyleLst></a:fmtScheme></a:themeElements></a:theme>");
		zipFile.addDir("xl/theme/", ts, ts, ts, 0);
		zipFile.addFile("xl/theme/theme1.xml", new TextEncoder().encode(sb.join("")), ts, ts, ts, 0);
	
		sb = [];
		sb.push("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
		sb.push("<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">");
//		sb.push("<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/theme\" Target=\"theme/theme1.xml\"/>");
		sb.push("<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles\" Target=\"styles.xml\"/>");
		i = 0;
		k = workbook.getCount();
		while (i < k)
		{
			sb.push("<Relationship Id=\"rId");
			sb.push(""+(i + 2));
			sb.push("\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/worksheet\" Target=\"worksheets/sheet");
			sb.push(""+(i + 1));
			sb.push(".xml\"/>");
			i++;
		}
		if (sharedStrings.length > 0)
		{
			sb.push("<Relationship Id=\"rId");
			sb.push(""+(workbook.getCount() + 2));
			sb.push("\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/sharedStrings\" Target=\"sharedStrings.xml\"/>");
		}
		sb.push("\n</Relationships>");
		if (!dirXlRel)
		{
			dirXlRel = true;
			zipFile.addDir("xl/_rels/", ts, ts, ts, 0);
		}
		zipFile.addFile("xl/_rels/workbook.xml.rels", new TextEncoder().encode(sb.join("")), ts, ts, ts, 0);
		sbContTypes.push("<Override PartName=\"/xl/_rels/workbook.xml.rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>");
	
		sb = [];
		sb.push("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n");
		sb.push("<cp:coreProperties xmlns:cp=\"http://schemas.openxmlformats.org/package/2006/metadata/core-properties\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:dcterms=\"http://purl.org/dc/terms/\" xmlns:dcmitype=\"http://purl.org/dc/dcmitype/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">");
		sb.push("<dcterms:created xsi:type=\"dcterms:W3CDTF\">");
		t = workbook.getCreateTime();
		if (t)
		{
			sb.push(t.toString("yyyy-MM-dd"));
			sb.push('T');
			sb.push(t.toString("HH:mm:ss"));
			sb.push('Z');
		}
		sb.push("</dcterms:created>");
		sb.push("<dc:creator>");
		if ((s = workbook.getAuthor()) != null)
		{
			s = text.toXMLText(s);
			sb.push(s);
		}
		sb.push("</dc:creator>");
		sb.push("<dc:description>");
		if (false)
		{
			s = text.toXMLText(s);
			sb.push(s);
		}
		sb.push("</dc:description>");
		sb.push("<dc:language>");
/*		UInt32 lcid = text.EncodingFactory.GetSystemLCID();
		NN<Text.Locale.LocaleEntry> loc;
		if (Text.Locale.GetLocaleEntry(lcid).SetTo(loc))
		{
			s = Text.XML.ToNewXMLText(loc.shortName);
			sb.Append(s);
			s.Release();
		}*/
		sb.push("</dc:language>");
		sb.push("<cp:lastModifiedBy>");
		if ((s = workbook.getLastAuthor()) != null)
		{
			s = text.toXMLText(s);
			sb.push(s);
		}
		sb.push("</cp:lastModifiedBy>");
		sb.push("<dcterms:modified xsi:type=\"dcterms:W3CDTF\">");
		t = workbook.getModifyTime();
		if (t)
		{
			sb.push(t.toString("yyyy-MM-dd"));
			sb.push('T');
			sb.push(t.toString("HH:mm:ss"));
			sb.push('Z');
		}
		sb.push("</dcterms:modified>");
		sb.push("<cp:revision>");
		sb.push(""+1);
		sb.push("</cp:revision>");
		sb.push("<dc:subject>");
		if (false)
		{
			s = text.toXMLText(s);
			sb.push(s);
		}
		sb.push("</dc:subject>");
		sb.push("<dc:title>");
		if (false)
		{
			s = text.toXMLText(s);
			sb.push(s);
		}
		sb.push("</dc:title>");
		sb.push("</cp:coreProperties>");
		zipFile.addDir("docProps/", ts, ts, ts, 0);
		zipFile.addFile("docProps/core.xml", new TextEncoder().encode(sb.join("")), ts, ts, ts, 0);
		sbContTypes.push("<Override PartName=\"/docProps/core.xml\" ContentType=\"application/vnd.openxmlformats-package.core-properties+xml\"/>");
	
		sb = [];
		sb.push("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n");
		sb.push("<Properties xmlns=\"http://schemas.openxmlformats.org/officeDocument/2006/extended-properties\" xmlns:vt=\"http://schemas.openxmlformats.org/officeDocument/2006/docPropsVTypes\">");
		sb.push("<Template></Template>");
		sb.push("<TotalTime>1</TotalTime>");
		sb.push("<Application>");
		sb.push("AVIRead/1.0");
		sb.push("</Application>");
		sb.push("</Properties>");
		zipFile.addFile("docProps/app.xml", new TextEncoder().encode(sb.join("")), ts, ts, ts, 0);
		sbContTypes.push("<Override PartName=\"/docProps/app.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.extended-properties+xml\"/>");
	
		sbContTypes.push("\n</Types>");
		zipFile.addFile("[Content_Types].xml", new TextEncoder().encode(sbContTypes.join("")), ts, ts, ts, 0);
		return zipFile.finalize();
	}

	/**
	 * @param {string[]} sb
	 * @param {spreadsheet.OfficeFill|null} fill
	 */
	static appendFill(sb, fill)
	{
		let color;
		if (fill == null)
			return;
		switch (fill.getFillType())
		{
		case spreadsheet.FillType.SolidFill:
			if ((color = fill.getColor()) == null)
			{
				sb.push("<a:solidFill/>");
			}
			else
			{
				sb.push("<a:solidFill>");
				if (color.getColorType() == spreadsheet.ColorType.Preset)
				{
					sb.push("<a:prstClr val=\"");
					sb.push(XLSXExporter.presetColorCode(color.getPresetColor()));
					sb.push("\"/>");
				}
				sb.push("</a:solidFill>");
			}
			break;
		}
	}

	/**
	 * @param {string[]} sb
	 * @param {spreadsheet.OfficeLineStyle|null} lineStyle
	 */
	static appendLineStyle(sb, lineStyle)
	{
		if (lineStyle == null)
			return;
		sb.push("<a:ln>");
		XLSXExporter.appendFill(sb, lineStyle.getFillStyle());
		sb.push("</a:ln>");
	}

	/**
	 * @param {string[]} sb
	 * @param {string} title
	 */
	static appendTitle(sb, title)
	{
		sb.push("<c:title>");
		sb.push("<c:tx>");
		sb.push("<c:rich>");
		sb.push("<a:bodyPr anchor=\"t\" rtlCol=\"false\"/>");
		sb.push("<a:lstStyle/>");
		sb.push("<a:p>");
		sb.push("<a:pPr algn=\"l\">");
		sb.push("<a:defRPr/>");
		sb.push("</a:pPr>");
		sb.push("<a:r>");
		sb.push("<a:rPr lang=\"en-HK\"/>");
		sb.push("<a:t>");
		let s = text.toXMLText(title);
		sb.push(s)
		sb.push("</a:t>");
		sb.push("</a:r>");
		sb.push("<a:endParaRPr lang=\"en-US\" sz=\"1100\"/>");
		sb.push("</a:p>");
		sb.push("</c:rich>");
		sb.push("</c:tx>");
		sb.push("<c:layout/>");
		sb.push("</c:title>");
	}

	/**
	 * @param {string[]} sb
	 * @param {spreadsheet.OfficeShapeProp | null} shapeProp
	 */
	static appendShapeProp(sb, shapeProp)
	{
		if (shapeProp == null)
			return;
		sb.push("<c:spPr>");
		XLSXExporter.appendFill(sb, shapeProp.getFill());
		XLSXExporter.appendLineStyle(sb, shapeProp.getLineStyle());
		sb.push("</c:spPr>");
	}

	/**
	 * @param {string[]} sb
	 * @param {spreadsheet.OfficeChartAxis | null} axis
	 * @param {number} index
	 */
	static appendAxis(sb, axis, index)
	{
		if (axis == null)
			return;
		let s;	
		switch (axis.getAxisType())
		{
		case spreadsheet.AxisType.Category:
			sb.push("<c:catAx>");
			break;
		case spreadsheet.AxisType.Date:
			sb.push("<c:dateAx>");
			break;
		case spreadsheet.AxisType.Numeric:
			sb.push("<c:valAx>");
			break;
		case spreadsheet.AxisType.Series:
			sb.push("<c:serAx>");
			break;
		}
		sb.push("<c:axId val=\"");
		sb.push(""+index);
		sb.push("\"/>");
		sb.push("<c:scaling>");
		sb.push("<c:orientation val=\"minMax\"/>");
		sb.push("</c:scaling>");
		sb.push("<c:delete val=\"false\"/>");
		switch (axis.getAxisPos())
		{
		case spreadsheet.AxisPosition.Left:
			sb.push("<c:axPos val=\"l\"/>");
			break;
		case spreadsheet.AxisPosition.Top:
			sb.push("<c:axPos val=\"t\"/>");
			break;
		case spreadsheet.AxisPosition.Right:
			sb.push("<c:axPos val=\"r\"/>");
			break;
		case spreadsheet.AxisPosition.Bottom:
			sb.push("<c:axPos val=\"b\"/>");
			break;
		}
		if (axis.getMajorGridProp())
		{
			sb.push("<c:majorGridlines>");
			XLSXExporter.appendShapeProp(sb, axis.getMajorGridProp());
			sb.push("</c:majorGridlines>");
		}
		if ((s = axis.getTitle()) != null)
		{
			XLSXExporter.appendTitle(sb, s);
		}
		sb.push("<c:majorTickMark val=\"cross\"/>");
		sb.push("<c:minorTickMark val=\"none\"/>");
		switch (axis.getTickLblPos())
		{
		case spreadsheet.TickLabelPosition.High:
			sb.push("<c:tickLblPos val=\"high\"/>");
			break;
		case spreadsheet.TickLabelPosition.Low:
			sb.push("<c:tickLblPos val=\"low\"/>");
			break;
		case spreadsheet.TickLabelPosition.NextTo:
			sb.push("<c:tickLblPos val=\"nextTo\"/>");
			break;
		case spreadsheet.TickLabelPosition.None:
			sb.push("<c:tickLblPos val=\"none\"/>");
			break;
		}
		XLSXExporter.appendShapeProp(sb, axis.getShapeProp());
	// 	sb.push("<c:crossAx val=\"1\"/>");
		sb.push("<c:crosses val=\"autoZero\"/>");
		sb.push("<c:crossBetween val=\"midCat\"/>");
		switch (axis.getAxisType())
		{
		case spreadsheet.AxisType.Category:
			sb.push("</c:catAx>");
			break;
		case spreadsheet.AxisType.Date:
			sb.push("</c:dateAx>");
			break;
		case spreadsheet.AxisType.Numeric:
			sb.push("</c:valAx>");
			break;
		case spreadsheet.AxisType.Series:
			sb.push("</c:serAx>");
			break;
		}
	}

	/**
	 * @param {string[]} sb
	 * @param {spreadsheet.OfficeChartSeries} series
	 * @param {number} index
	 */
	static appendSeries(sb, series, index)
	{
		let s;
		sb.push("<c:ser>");
		sb.push("<c:idx val=\"");
		sb.push(""+index);
		sb.push("\"/>");
		sb.push("<c:order val=\"");
		sb.push(""+index);
		sb.push("\"/>");
		if ((s = series.getTitle()) != null)
		{
			sb.push("<c:tx>");
			sb.push("<c:v>");
			s = text.toXMLText(s);
			sb.push(s);
			sb.push("</c:v>");
			sb.push("</c:tx>");
		}
		XLSXExporter.appendShapeProp(sb, series.getShapeProp());
		sb.push("<c:marker>");
		switch (series.getMarkerStyle())
		{
		case spreadsheet.MarkerStyle.Circle:
			sb.push("<c:symbol val=\"circle\"/>");
			break;
		case spreadsheet.MarkerStyle.Dash:
			sb.push("<c:symbol val=\"dash\"/>");
			break;
		case spreadsheet.MarkerStyle.Diamond:
			sb.push("<c:symbol val=\"diamond\"/>");
			break;
		case spreadsheet.MarkerStyle.Dot:
			sb.push("<c:symbol val=\"dot\"/>");
			break;
		case spreadsheet.MarkerStyle.None:
			sb.push("<c:symbol val=\"none\"/>");
			break;
		case spreadsheet.MarkerStyle.Picture:
			sb.push("<c:symbol val=\"picture\"/>");
			break;
		case spreadsheet.MarkerStyle.Plus:
			sb.push("<c:symbol val=\"plus\"/>");
			break;
		case spreadsheet.MarkerStyle.Square:
			sb.push("<c:symbol val=\"square\"/>");
			break;
		case spreadsheet.MarkerStyle.Star:
			sb.push("<c:symbol val=\"star\"/>");
			break;
		case spreadsheet.MarkerStyle.Triangle:
			sb.push("<c:symbol val=\"triangle\"/>");
			break;
		case spreadsheet.MarkerStyle.X:
			sb.push("<c:symbol val=\"x\"/>");
			break;
		}
		if (series.getMarkerSize() != 0)
		{
			sb.push("<c:size val=\"");
			sb.push(""+series.getMarkerSize());
			sb.push("\"/>");
		}
		sb.push("</c:marker>");
	
		let catData = series.getCategoryData();
		sb.push("<c:cat>");
		sb.push("<c:strRef>");
		sb.push("<c:f>");
		s = text.toXMLText(catData.toCodeRange());
		sb.push(s);
		sb.push("</c:f>");
		sb.push("<c:strCache/>");
		sb.push("</c:strRef>");
		sb.push("</c:cat>");
	
		let valData = series.getValueData();
		sb.push("<c:val>");
		sb.push("<c:numRef>");
		sb.push("<c:f>");
		s = text.toXMLText(valData.toCodeRange());
		sb.push(s);
		sb.push("</c:f>");
		let firstRow = valData.getFirstRow();
		let lastRow = valData.getLastRow();
		let firstCol = valData.getFirstCol();
		let lastCol = valData.getLastCol();
		if (firstRow == lastRow)
		{
			let sheet = valData.getSheet();
			sb.push("<c:numCache>");
			sb.push("<c:ptCount val=\"");
			sb.push(""+(lastCol - firstCol + 1));
			sb.push("\"/>");
			let row;
			let cell;
			let cellValue;
			let i;
			if ((row = sheet.getItem(firstRow)) != null)
			{
				i = firstCol;
				while (i <= lastCol)
				{
					if ((cell = row.cells[i]) != null && (cellValue = cell.cellValue) != null && (cell.cdt == spreadsheet.CellDataType.DateTime || cell.cdt == spreadsheet.CellDataType.Number))
					{
						sb.push("<c:pt idx=\"");
						sb.push(""+(i - firstCol));
						sb.push("\"><c:v>");
						sb.push(cellValue);
						sb.push("</c:v></c:pt>");
					}
					i++;
				}
			}
			sb.push("</c:numCache>");
		}
		else if (firstCol == lastCol)
		{
			let sheet = valData.getSheet();
			sb.push("<c:numCache>");
			sb.push("<c:ptCount val=\"");
			sb.push(""+(lastRow - firstRow + 1));
			sb.push("\"/>");
			let row;
			let cell;
			let cellValue;
			let i;
			i = firstRow;
			while (i <= lastRow)
			{
				if ((row = sheet.getItem(i)) != null)
				{
					if ((cell = row.cells[firstCol]) != null && (cellValue = cell.cellValue) != null && (cell.cdt == spreadsheet.CellDataType.DateTime || cell.cdt == spreadsheet.CellDataType.Number))
					{
						sb.push("<c:pt idx=\"");
						sb.push(""+(i - firstRow));
						sb.push("\"><c:v>");
						sb.push(cellValue);
						sb.push("</c:v></c:pt>");
					}
					i++;
				}
				i++;	
			}
			sb.push("</c:numCache>");
		}
		else
		{
			sb.push("<c:numCache/>");
		}
		sb.push("</c:numRef>");
		sb.push("</c:val>");
	
		sb.push("<c:smooth val=\"");
		if (series.isSmooth())
			sb.push("true");
		else
			sb.push("false");
		sb.push("\"/>");
		sb.push("</c:ser>");
	}

	/**
	 * @param {string[]} sb
	 * @param {spreadsheet.BorderStyle} border
	 * @param {string} name
	 */
	static appendBorder(sb, border, name)
	{
		sb.push('<');
		sb.push(name);
		if (border.borderType == spreadsheet.BorderType.None)
		{
			sb.push("/>");
		}
		else
		{
			switch (border.borderType)
			{
				case spreadsheet.BorderType.Thin:
					sb.push(" style=\"thin\">");
					break;
				case spreadsheet.BorderType.Medium:
					sb.push(" style=\"medium\">");
					break;
				case spreadsheet.BorderType.Dashed:
					sb.push(" style=\"dashed\">");
					break;
				case spreadsheet.BorderType.Dotted:
					sb.push(" style=\"dotted\">");
					break;
				case spreadsheet.BorderType.Thick:
					sb.push(" style=\"thick\">");
					break;
				case spreadsheet.BorderType.DOUBLE:
					sb.push(" style=\"double\">");
					break;
				case spreadsheet.BorderType.Hair:
					sb.push(" style=\"hair\">");
					break;
				case spreadsheet.BorderType.MediumDashed:
					sb.push(" style=\"mediumDashed\">");
					break;
				case spreadsheet.BorderType.DashDot:
					sb.push(" style=\"dashDot\">");
					break;
				case spreadsheet.BorderType.MediumDashDot:
					sb.push(" style=\"mediumDashDot\">");
					break;
				case spreadsheet.BorderType.DashDotDot:
					sb.push(" style=\"dashDotDot\">");
					break;
				case spreadsheet.BorderType.MediumDashDotDot:
					sb.push(" style=\"mediumDashDotDot\">");
					break;
				case spreadsheet.BorderType.SlantedDashDot:
					sb.push(" style=\"slantDashDot\">");
					break;
//				case spreadsheet.BorderType.None:
//					break;
			}
			sb.push("<color rgb=\"");
			sb.push(text.toHex32(border.borderColor));
			sb.push("\"/>");
			sb.push("</");
			sb.push(name);
			sb.push('>');
		}
	}

	/**
	 * @param {string[]} sb
	 * @param {spreadsheet.CellStyle} style
	 * @param {{ left: spreadsheet.BorderStyle; top: spreadsheet.BorderStyle; right: spreadsheet.BorderStyle; bottom: spreadsheet.BorderStyle; }[]} borders
	 * @param {spreadsheet.Workbook} workbook
	 * @param {{ [x: string]: number; }} numFmtMap
	 */
	static appendXF(sb, style, borders, workbook, numFmtMap)
	{
		let k;
		let s;
		let border;
		let font = style.getFont();
		if ((s = style.getDataFormat()) == null)
		{
			s = "general";
		}
		sb.push("<xf numFmtId=\"");
		sb.push(""+(Number(numFmtMap[s]) + 164));
		sb.push("\" fontId=\"");
		if (font == null)
		{
			sb.push('0');
		}
		else
		{
			sb.push(""+workbook.getFontIndex(font));
		}
		sb.push("\" fillId=\"0\" borderId=\"");
		k = borders.length;
		while (k-- > 0)
		{
			border = borders[k];
			if (border.left.equals(style.getBorderLeft()) &&
				border.top.equals(style.getBorderTop()) &&
				border.right.equals(style.getBorderRight()) &&
				border.bottom.equals(style.getBorderBottom()))
			{
				break;
			}
		}
		if (k == -1)
		{
			k = 0;
		}
		sb.push(""+k);
		sb.push("\" xfId=\"0\" applyFont=\"");
		if (font != null)
		{
			sb.push("true");
		}
		else
		{
			sb.push("false");
		}
		sb.push("\" applyBorder=\"false\" applyAlignment=\"true\" applyProtection=\"false\">");
		sb.push("<alignment horizontal=\"");
		switch (style.getHAlign())
		{
		case text.HAlignment.Left:
			sb.push("left");
			break;
		case text.HAlignment.Center:
			sb.push("center");
			break;
		case text.HAlignment.Right:
			sb.push("right");
			break;
		case text.HAlignment.Fill:
			sb.push("fill");
			break;
		case text.HAlignment.Justify:
			sb.push("justify");
			break;
		case text.HAlignment.Unknown:
		default:
			sb.push("general");
			break;
		}
		sb.push("\" vertical=\"");
		switch (style.getVAlign())
		{
		case text.VAlignment.Top:
			sb.push("top");
			break;
		case text.VAlignment.Center:
			sb.push("center");
			break;
		case text.VAlignment.Bottom:
			sb.push("bottom");
			break;
		case text.VAlignment.Justify:
			sb.push("justify");
			break;
		case text.VAlignment.Unknown:
		default:
			sb.push("general");
			break;
		}
		sb.push("\" textRotation=\"0\" wrapText=\"");
		if (style.getWordWrap())
			sb.push("true");
		else
			sb.push("false");
		sb.push("\" indent=\"0\" shrinkToFit=\"false\"/>");
		sb.push("<protection locked=\"true\" hidden=\"false\"/>");
		sb.push("</xf>");
	}

	/**
	 * @param {spreadsheet.PresetColor} color
	 */
	static presetColorCode(color)
	{
		switch (color)
		{
		case spreadsheet.PresetColor.AliceBlue:
			return "aliceBlue";
		case spreadsheet.PresetColor.AntiqueWhite:
			return "antiqueWhite";	
		case spreadsheet.PresetColor.Aqua:
			return "aqua";
		case spreadsheet.PresetColor.Aquamarine:
			return "aquamarine";
		case spreadsheet.PresetColor.Azure:
			return "azure";
		case spreadsheet.PresetColor.Beige:
			return "beige";
		case spreadsheet.PresetColor.Bisque:
			return "bisque";
		case spreadsheet.PresetColor.Black:
			return "black";
		case spreadsheet.PresetColor.BlanchedAlmond:
			return "blanchedAlmond";
		case spreadsheet.PresetColor.Blue:
			return "blue";
		case spreadsheet.PresetColor.BlueViolet:
			return "blueViolet";
		case spreadsheet.PresetColor.Brown:
			return "brown";
		case spreadsheet.PresetColor.BurlyWood:
			return "burlyWood";
		case spreadsheet.PresetColor.CadetBlue:
			return "cadetBlue";
		case spreadsheet.PresetColor.Chartreuse:
			return "chartreuse";
		case spreadsheet.PresetColor.Chocolate:
			return "chocolate";
		case spreadsheet.PresetColor.Coral:
			return "coral";
		case spreadsheet.PresetColor.CornflowerBlue:
			return "cornflowerBlue";
		case spreadsheet.PresetColor.Cornsilk:
			return "cornsilk";
		case spreadsheet.PresetColor.Crimson:
			return "crimson";
		case spreadsheet.PresetColor.Cyan:
			return "cyan";
		case spreadsheet.PresetColor.DeepPink:
			return "deepPink";
		case spreadsheet.PresetColor.DeepSkyBlue:
			return "deepSkyBlue";
		case spreadsheet.PresetColor.DimGray:
			return "dimGray";
		case spreadsheet.PresetColor.DarkBlue:
			return "dkBlue";
		case spreadsheet.PresetColor.DarkCyan:
			return "dkCyan";
		case spreadsheet.PresetColor.DarkGoldenrod:
			return "dkGoldenrod";
		case spreadsheet.PresetColor.DarkGray:
			return "dkGray";
		case spreadsheet.PresetColor.DarkGreen:
			return "dkGreen";
		case spreadsheet.PresetColor.DarkKhaki:
			return "dkKhaki";
		case spreadsheet.PresetColor.DarkMagenta:
			return "dkMagenta";
		case spreadsheet.PresetColor.DarkOliveGreen:
			return "dkOliveGreen";
		case spreadsheet.PresetColor.DarkOrange:
			return "dkOrange";
		case spreadsheet.PresetColor.DarkOrchid:
			return "dkOrchid";
		case spreadsheet.PresetColor.DarkRed:
			return "dkRed";
		case spreadsheet.PresetColor.DarkSalmon:
			return "dkSalmon";
		case spreadsheet.PresetColor.DarkSeaGreen:
			return "dkSeaGreen";
		case spreadsheet.PresetColor.DarkSlateBlue:
			return "dkSlateBlue";
		case spreadsheet.PresetColor.DarkSlateGray:
			return "dkSlateGray";
		case spreadsheet.PresetColor.DarkTurquoise:
			return "dkTurquoise";
		case spreadsheet.PresetColor.DarkViolet:
			return "dkViolet";
		case spreadsheet.PresetColor.DodgerBlue:
			return "dodgerBlue";
		case spreadsheet.PresetColor.Firebrick:
			return "firebrick";
		case spreadsheet.PresetColor.FloralWhite:
			return "floralWhite";
		case spreadsheet.PresetColor.ForestGreen:
			return "forestGreen";
		case spreadsheet.PresetColor.Fuchsia:
			return "fuchsia";
		case spreadsheet.PresetColor.Gainsboro:
			return "gainsboro";
		case spreadsheet.PresetColor.GhostWhite:
			return "ghostWhite";
		case spreadsheet.PresetColor.Gold:
			return "gold";
		case spreadsheet.PresetColor.Goldenrod:
			return "goldenrod";
		case spreadsheet.PresetColor.Gray:
			return "gray";
		case spreadsheet.PresetColor.Green:
			return "green";
		case spreadsheet.PresetColor.GreenYellow:
			return "greenYellow";
		case spreadsheet.PresetColor.Honeydew:
			return "honeydew";
		case spreadsheet.PresetColor.HotPink:
			return "hotPink";
		case spreadsheet.PresetColor.IndianRed:
			return "indianRed";
		case spreadsheet.PresetColor.Indigo:
			return "indigo";
		case spreadsheet.PresetColor.Ivory:
			return "ivory";
		case spreadsheet.PresetColor.Khaki:
			return "khaki";
		case spreadsheet.PresetColor.Lavender:
			return "lavender";
		case spreadsheet.PresetColor.LavenderBlush:
			return "lavenderBlush";
		case spreadsheet.PresetColor.LawnGreen:
			return "lawnGreen";
		case spreadsheet.PresetColor.LemonChiffon:
			return "lemonChiffon";
		case spreadsheet.PresetColor.Lime:
			return "lime";
		case spreadsheet.PresetColor.LimeGreen:
			return "limeGreen";
		case spreadsheet.PresetColor.Linen:
			return "linen";
		case spreadsheet.PresetColor.LightBlue:
			return "ltBlue";
		case spreadsheet.PresetColor.LightCoral:
			return "ltCoral";
		case spreadsheet.PresetColor.LightCyan:
			return "ltCyan";
		case spreadsheet.PresetColor.LightGoldenrodYellow:
			return "ltGoldenrodYellow";
		case spreadsheet.PresetColor.LightGray:
			return "ltGray";
		case spreadsheet.PresetColor.LightGreen:
			return "ltGreen";
		case spreadsheet.PresetColor.LightPink:
			return "ltPink";
		case spreadsheet.PresetColor.LightSalmon:
			return "ltSalmon";
		case spreadsheet.PresetColor.LightSeaGreen:
			return "ltSeaGreen";
		case spreadsheet.PresetColor.LightSkyBlue:
			return "ltSkyBlue";
		case spreadsheet.PresetColor.LightSlateGray:
			return "ltSlateGray";
		case spreadsheet.PresetColor.LightSteelBlue:
			return "ltSteelBlue";
		case spreadsheet.PresetColor.LightYellow:
			return "ltYellow";
		case spreadsheet.PresetColor.Magenta:
			return "magenta";
		case spreadsheet.PresetColor.Maroon:
			return "maroon";
		case spreadsheet.PresetColor.MediumAquamarine:
			return "medAquamarine";
		case spreadsheet.PresetColor.MediumBlue:
			return "medBlue";
		case spreadsheet.PresetColor.MediumOrchid:
			return "medOrchid";
		case spreadsheet.PresetColor.MediumPurple:
			return "medPurple";
		case spreadsheet.PresetColor.MediumSeaGreen:
			return "medSeaGreen";
		case spreadsheet.PresetColor.MediumSlateBlue:
			return "medSlateBlue";
		case spreadsheet.PresetColor.MediumSpringGreen:
			return "medSpringGreen";
		case spreadsheet.PresetColor.MediumTurquoise:
			return "medTurquoise";
		case spreadsheet.PresetColor.MediumVioletRed:
			return "medVioletRed";
		case spreadsheet.PresetColor.MidnightBlue:
			return "midnightBlue";
		case spreadsheet.PresetColor.MintCream:
			return "mintCream";
		case spreadsheet.PresetColor.MistyRose:
			return "mistyRose";
		case spreadsheet.PresetColor.Moccasin:
			return "moccasin";
		case spreadsheet.PresetColor.NavajoWhite:
			return "navajoWhite";
		case spreadsheet.PresetColor.Navy:
			return "navy";
		case spreadsheet.PresetColor.OldLace:
			return "oldLace";
		case spreadsheet.PresetColor.Olive:
			return "olive";
		case spreadsheet.PresetColor.OliveDrab:
			return "oliveDrab";
		case spreadsheet.PresetColor.Orange:
			return "orange";
		case spreadsheet.PresetColor.OrangeRed:
			return "orangeRed";
		case spreadsheet.PresetColor.Orchid:
			return "orchid";
		case spreadsheet.PresetColor.PaleGoldenrod:
			return "paleGoldenrod";
		case spreadsheet.PresetColor.PaleGreen:
			return "paleGreen";
		case spreadsheet.PresetColor.PaleTurquoise:
			return "paleTurquoise";
		case spreadsheet.PresetColor.PaleVioletRed:
			return "paleVioletRed";
		case spreadsheet.PresetColor.PapayaWhip:
			return "papayaWhip";
		case spreadsheet.PresetColor.PeachPuff:
			return "peachPuff";
		case spreadsheet.PresetColor.Peru:
			return "peru";
		case spreadsheet.PresetColor.Pink:
			return "pink";
		case spreadsheet.PresetColor.Plum:
			return "plum";
		case spreadsheet.PresetColor.PowderBlue:
			return "powderBlue";
		case spreadsheet.PresetColor.Purple:
			return "purple";
		case spreadsheet.PresetColor.Red:
			return "red";
		case spreadsheet.PresetColor.RosyBrown:
			return "rosyBrown";
		case spreadsheet.PresetColor.RoyalBlue:
			return "royalBlue";
		case spreadsheet.PresetColor.SaddleBrown:
			return "saddleBrown";
		case spreadsheet.PresetColor.Salmon:
			return "salmon";
		case spreadsheet.PresetColor.SandyBrown:
			return "sandyBrown";
		case spreadsheet.PresetColor.SeaGreen:
			return "seaGreen";
		case spreadsheet.PresetColor.SeaShell:
			return "seaShell";
		case spreadsheet.PresetColor.Sienna:
			return "sienna";
		case spreadsheet.PresetColor.Silver:
			return "silver";
		case spreadsheet.PresetColor.SkyBlue:
			return "skyBlue";
		case spreadsheet.PresetColor.SlateBlue:
			return "slateBlue";
		case spreadsheet.PresetColor.SlateGray:
			return "slateGray";
		case spreadsheet.PresetColor.Snow:
			return "snow";
		case spreadsheet.PresetColor.SpringGreen:
			return "springGreen";
		case spreadsheet.PresetColor.SteelBlue:
			return "steelBlue";
		case spreadsheet.PresetColor.Tan:
			return "tan";
		case spreadsheet.PresetColor.Teal:
			return "teal";
		case spreadsheet.PresetColor.Thistle:
			return "thistle";
		case spreadsheet.PresetColor.Tomato:
			return "tomato";
		case spreadsheet.PresetColor.Turquoise:
			return "turquoise";
		case spreadsheet.PresetColor.Violet:
			return "violet";
		case spreadsheet.PresetColor.Wheat:
			return "wheat";
		case spreadsheet.PresetColor.White:
			return "white";
		case spreadsheet.PresetColor.WhiteSmoke:
			return "whiteSmoke";
		case spreadsheet.PresetColor.Yellow:
			return "yellow";
		case spreadsheet.PresetColor.YellowGreen:
			return "yellowGreen";
		default:
			return "Unknown";
		}
	}

	/**
	 * @param {string} dataFormat
	 */
	static toFormatCode(dataFormat)
	{
		let s = [];
		let i = 0;
		let j = dataFormat.length;
		let c;
		while (i < j)
		{
			c = dataFormat.charAt(i);
			if (c == '-')
			{
				s.push('\\');
				s.push(c)
			}
			else
			{
				s.push(c.toLowerCase());
			}
			i++;
		}
		return s.join("");
	}
};
