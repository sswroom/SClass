#include "Stdafx.h"
#include "MyMemory.h"
#include "Exporter/XLSXExporter.h"
#include "IO/BuildTime.h"
#include "IO/ZIPBuilder.h"
#include "Math/Math.h"
#include "Text/CharUtil.h"
#include "Text/Locale.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XLSUtil.h"
#include "Text/XML.h"
#include "Text/SpreadSheet/Workbook.h"

Exporter::XLSXExporter::XLSXExporter()
{
}

Exporter::XLSXExporter::~XLSXExporter()
{
}

Int32 Exporter::XLSXExporter::GetName()
{
	return *(Int32*)"XLSX";
}

IO::FileExporter::SupportType Exporter::XLSXExporter::IsObjectSupported(IO::ParsedObject *pobj)
{
	if (pobj->GetParserType() != IO::ParserType::Workbook)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	return IO::FileExporter::SupportType::NormalStream;
}

Bool Exporter::XLSXExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcat(nameBuff, (const UTF8Char*)"Excel XLSX File");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.xlsx");
		return true;
	}
	return false;
}

Bool Exporter::XLSXExporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
{
	if (pobj->GetParserType() != IO::ParserType::Workbook)
	{
		return false;
	}
	Text::SpreadSheet::Workbook *workbook = (Text::SpreadSheet::Workbook*)pobj;
	Text::SpreadSheet::Worksheet *sheet;
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[256];
	Data::DateTime dt;
	Data::DateTime dt2;
	Data::DateTime *t;
	const UTF8Char *csptr;
	const UTF8Char *csptr2;
	IO::ZIPBuilder *zip;
	UOSInt i;
	UOSInt j = workbook->GetCount();
	UOSInt k;
	UOSInt l;
	UOSInt m;
	UOSInt n;
	UOSInt drawingCnt = 0;
	Data::ArrayList<const UTF8Char*> sharedStrings;
	Data::StringUTF8Map<UOSInt> stringMap;
	dt.SetCurrTimeUTC();
	NEW_CLASS(zip, IO::ZIPBuilder(stm));

	i = 0;
	j = workbook->GetCount();
	while (i < j)
	{
		sheet = workbook->GetItem(i);
		sb.ClearStr();
		sb.Append((const UTF8Char*)"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n");
		sb.Append((const UTF8Char*)"<worksheet xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\">");
		sb.Append((const UTF8Char*)"<sheetPr filterMode=\"false\">");
		sb.Append((const UTF8Char*)"<pageSetUpPr fitToPage=\"false\"/>");
		sb.Append((const UTF8Char*)"</sheetPr>");
		sb.Append((const UTF8Char*)"<dimension ref=\"A1\"/>");
		sb.Append((const UTF8Char*)"<sheetViews>");
		sb.Append((const UTF8Char*)"<sheetView showFormulas=\"false\" showGridLines=\"true\" showRowColHeaders=\"true\" showZeros=\"true\" rightToLeft=\"false\" tabSelected=\"true\" showOutlineSymbols=\"true\" defaultGridColor=\"true\" view=\"normal\" topLeftCell=\"A1\" colorId=\"64\" zoomScale=\"");
		sb.AppendU32(sheet->GetZoom());
		sb.Append((const UTF8Char*)"\" zoomScaleNormal=\"");
		sb.AppendU32(sheet->GetZoom());
		sb.Append((const UTF8Char*)"\" zoomScalePageLayoutView=\"");
		sb.AppendU32(sheet->GetZoom());
		sb.Append((const UTF8Char*)"\" workbookViewId=\"0\">");
		sb.Append((const UTF8Char*)"<selection pane=\"topLeft\" activeCell=\"A1\" activeCellId=\"0\" sqref=\"A1\"/>");
		sb.Append((const UTF8Char*)"</sheetView>");
		sb.Append((const UTF8Char*)"</sheetViews>");
		sb.Append((const UTF8Char*)"<sheetFormatPr defaultRowHeight=\"12.8\" zeroHeight=\"false\" outlineLevelRow=\"0\" outlineLevelCol=\"0\"></sheetFormatPr>");
		sb.Append((const UTF8Char*)"<cols>");
		sb.Append((const UTF8Char*)"<col collapsed=\"false\" customWidth=\"false\" hidden=\"false\" outlineLevel=\"0\" max=\"1025\" min=\"1\" style=\"0\" width=\"11.52\"/>");
		sb.Append((const UTF8Char*)"</cols>");
		k = 0;
		l = sheet->GetCount();
		if (l > 0)
		{
			sb.Append((const UTF8Char*)"<sheetData>");
			while (k < l)
			{
				Text::SpreadSheet::Worksheet::RowData *row = sheet->GetItem(k);
				sb.Append((const UTF8Char*)"<row r=\"");
				sb.AppendUOSInt(k + 1);
				sb.Append((const UTF8Char*)"\" customFormat=\"false\" ht=\"12.8\" hidden=\"false\" customHeight=\"false\" outlineLevel=\"0\" collapsed=\"false\">");

				m = 0;
				n = row->cells->GetCount();
				while (m < n)
				{
					Text::SpreadSheet::Worksheet::CellData *cell = row->cells->GetItem(m);
					if (cell && cell->cdt != Text::SpreadSheet::CellDataType::MergedLeft && cell->cdt != Text::SpreadSheet::CellDataType::MergedTop)
					{
						sb.Append((const UTF8Char*)"<c r=\"");
						Text::StrUOSInt(Text::SpreadSheet::Workbook::ColCode(sbuff, m), k + 1);
						sb.Append(sbuff);
						sb.AppendChar('"', 1);
						if (cell->style)
						{
							sb.Append((const UTF8Char*)" s=\"");
							sb.AppendUOSInt(cell->style->GetIndex());
							sb.AppendChar('"', 1);
						}
						switch (cell->cdt)
						{
						case Text::SpreadSheet::CellDataType::String:
							sb.Append((const UTF8Char*)" t=\"s\"");
							break;
						case Text::SpreadSheet::CellDataType::Number:
						case Text::SpreadSheet::CellDataType::DateTime:
							sb.Append((const UTF8Char*)" t=\"n\"");
							break;
						case Text::SpreadSheet::CellDataType::MergedLeft:
						case Text::SpreadSheet::CellDataType::MergedTop:
							break;
						}
						sb.Append((const UTF8Char*)"><v>");
						switch (cell->cdt)
						{
						case Text::SpreadSheet::CellDataType::String:
							{
								UOSInt sIndex = stringMap.Get(cell->cellValue);
								if (sIndex == 0 && !stringMap.ContainsKey(cell->cellValue))
								{
									sIndex = sharedStrings.Add(cell->cellValue);
									stringMap.Put(cell->cellValue, sIndex);
								}
								sb.AppendUOSInt(sIndex);
							}
							break;
						case Text::SpreadSheet::CellDataType::Number:
							sb.Append(cell->cellValue);
							break;
						case Text::SpreadSheet::CellDataType::DateTime:
							{
								Data::DateTime dt;
								dt.ToLocalTime();
								dt.SetValue(cell->cellValue);
								Text::SBAppendF64(&sb, Text::XLSUtil::Date2Number(&dt));
							}
							break;
						case Text::SpreadSheet::CellDataType::MergedLeft:
						case Text::SpreadSheet::CellDataType::MergedTop:
							break;
						}
						
						sb.Append((const UTF8Char*)"</v></c>");
					}
					m++;
				}
				sb.Append((const UTF8Char*)"</row>");
				k++;
			}
			sb.Append((const UTF8Char*)"</sheetData>");
		}
		else
		{
			sb.Append((const UTF8Char*)"<sheetData/>");
		}
		//<sheetProtection sheet="true" password="cc1a" objects="true" scenarios="true"/><printOptions headings="false" gridLines="false" gridLinesSet="true" horizontalCentered="false" verticalCentered="false"/>
		sb.Append((const UTF8Char*)"<pageMargins left=\"");
		Text::SBAppendF64(&sb, sheet->GetMarginLeft());
		sb.Append((const UTF8Char*)"\" right=\"");
		Text::SBAppendF64(&sb, sheet->GetMarginRight());
		sb.Append((const UTF8Char*)"\" top=\"");
		Text::SBAppendF64(&sb, sheet->GetMarginTop());
		sb.Append((const UTF8Char*)"\" bottom=\"");
		Text::SBAppendF64(&sb, sheet->GetMarginBottom());
		sb.Append((const UTF8Char*)"\" header=\"");
		Text::SBAppendF64(&sb, sheet->GetMarginHeader());
		sb.Append((const UTF8Char*)"\" footer=\"");
		Text::SBAppendF64(&sb, sheet->GetMarginFooter());
		sb.Append((const UTF8Char*)"\"/>");
		sb.Append((const UTF8Char*)"<pageSetup paperSize=\"9\" scale=\"100\" firstPageNumber=\"1\" fitToWidth=\"1\" fitToHeight=\"1\" pageOrder=\"downThenOver\" orientation=\"portrait\" blackAndWhite=\"false\" draft=\"false\" cellComments=\"none\" useFirstPageNumber=\"true\" horizontalDpi=\"300\" verticalDpi=\"300\" copies=\"1\"/>");
		sb.Append((const UTF8Char*)"<headerFooter differentFirst=\"false\" differentOddEven=\"false\">");
		sb.Append((const UTF8Char*)"<oddHeader>&amp;C&amp;&quot;Times New Roman,Regular&quot;&amp;12&amp;A</oddHeader>");
		sb.Append((const UTF8Char*)"<oddFooter>&amp;C&amp;&quot;Times New Roman,Regular&quot;&amp;12Page &amp;P</oddFooter>");
		sb.Append((const UTF8Char*)"</headerFooter>");
		k = 0;
		l = sheet->GetDrawingCount();
		while (k < l)
		{
			sb.Append((const UTF8Char*)"<drawing r:id=\"rId");
			sb.AppendUOSInt(k + 1);
			sb.Append((const UTF8Char*)"\"/>");
			k++;
		}
		sb.Append((const UTF8Char*)"</worksheet>");
		Text::StrConcat(Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"xl/worksheets/sheet"), i + 1), (const UTF8Char*)".xml");
		zip->AddFile(sbuff, sb.ToString(), sb.GetLength(), dt.ToTicks(), false);

		if (sheet->GetDrawingCount() > 0)
		{
			sb.ClearStr();
			sb.Append((const UTF8Char*)"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>");
			sb.Append((const UTF8Char*)"<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">");
			k = 0;
			l = sheet->GetDrawingCount();
			while (k < l)
			{
				sb.Append((const UTF8Char*)"<Relationship Id=\"rId");
				sb.AppendUOSInt(k + 1);
				sb.Append((const UTF8Char*)"\" Target=\"../drawings/drawing");
				sb.AppendUOSInt(k + 1 + drawingCnt);
				sb.Append((const UTF8Char*)".xml\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/drawing\"/>");
				k++;
			}
			sb.Append((const UTF8Char*)"</Relationships>");

			Text::StrConcat(Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"xl/worksheets/_rels/sheet"), i + 1), (const UTF8Char*)".xml.rels");
			zip->AddFile(sbuff, sb.ToString(), sb.GetLength(), dt.ToTicks(), false);

			k = 0;
			l = sheet->GetDrawingCount();
			while (k < l)
			{
				Text::SpreadSheet::WorksheetDrawing *drawing = sheet->GetDrawing(k);
				sb.ClearStr();
				sb.Append((const UTF8Char*)"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");
				sb.Append((const UTF8Char*)"<xdr:wsDr xmlns:xdr=\"http://schemas.openxmlformats.org/drawingml/2006/spreadsheetDrawing\" xmlns:a=\"http://schemas.openxmlformats.org/drawingml/2006/main\" xmlns:c=\"http://schemas.openxmlformats.org/drawingml/2006/chart\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\">");
				switch (drawing->anchorType)
				{
				case Text::SpreadSheet::AnchorType::Absolute:
					sb.Append((const UTF8Char*)"<xdr:absoluteAnchor>");
					sb.Append((const UTF8Char*)"<xdr:pos x=\"");
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->posXInch)));
					sb.Append((const UTF8Char*)"\" y=\"");
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->posYInch)));
					sb.Append((const UTF8Char*)"\"/>");
					sb.Append((const UTF8Char*)"<xdr:ext cx=\"");
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->widthInch)));
					sb.Append((const UTF8Char*)"\" cy=\"");
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->heightInch)));
					sb.Append((const UTF8Char*)"\"/>");
					break;
				case Text::SpreadSheet::AnchorType::OneCell:
					sb.Append((const UTF8Char*)"<xdr:oneCellAnchor>");
					sb.Append((const UTF8Char*)"<xdr:from>");
					sb.Append((const UTF8Char*)"<xdr:col>");
					sb.AppendUOSInt(drawing->col1 + 1);
					sb.Append((const UTF8Char*)"</xdr:col>");
					sb.Append((const UTF8Char*)"<xdr:colOff>");
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->posXInch)));
					sb.Append((const UTF8Char*)"</xdr:colOff>");
					sb.Append((const UTF8Char*)"<xdr:row>");
					sb.AppendUOSInt(drawing->row1 + 1);
					sb.Append((const UTF8Char*)"</xdr:row>");
					sb.Append((const UTF8Char*)"<xdr:rowOff>");
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->posYInch)));
					sb.Append((const UTF8Char*)"</xdr:rowOff>");
					sb.Append((const UTF8Char*)"</xdr:from>");
					sb.Append((const UTF8Char*)"<xdr:ext cx=\"");
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->widthInch)));
					sb.Append((const UTF8Char*)"\" cy=\"");
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->heightInch)));
					sb.Append((const UTF8Char*)"\"/>");
					break;
				case Text::SpreadSheet::AnchorType::TwoCell:
					sb.Append((const UTF8Char*)"<xdr:twoCellAnchor editAs=\"twoCell\">");
					sb.Append((const UTF8Char*)"<xdr:from>");
					sb.Append((const UTF8Char*)"<xdr:col>");
					sb.AppendUOSInt(drawing->col1 + 1);
					sb.Append((const UTF8Char*)"</xdr:col>");
					sb.Append((const UTF8Char*)"<xdr:colOff>");
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->posXInch)));
					sb.Append((const UTF8Char*)"</xdr:colOff>");
					sb.Append((const UTF8Char*)"<xdr:row>");
					sb.AppendUOSInt(drawing->row1 + 1);
					sb.Append((const UTF8Char*)"</xdr:row>");
					sb.Append((const UTF8Char*)"<xdr:rowOff>");
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->posYInch)));
					sb.Append((const UTF8Char*)"</xdr:rowOff>");
					sb.Append((const UTF8Char*)"</xdr:from>");
					sb.Append((const UTF8Char*)"<xdr:to>");
					sb.Append((const UTF8Char*)"<xdr:col>");
					sb.AppendUOSInt(drawing->col2 + 1);
					sb.Append((const UTF8Char*)"</xdr:col>");
					sb.Append((const UTF8Char*)"<xdr:colOff>");
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->widthInch)));
					sb.Append((const UTF8Char*)"</xdr:colOff>");
					sb.Append((const UTF8Char*)"<xdr:row>");
					sb.AppendUOSInt(drawing->row2 + 1);
					sb.Append((const UTF8Char*)"</xdr:row>");
					sb.Append((const UTF8Char*)"<xdr:rowOff>");
					sb.AppendOSInt(Math::Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->heightInch)));
					sb.Append((const UTF8Char*)"</xdr:rowOff>");
					sb.Append((const UTF8Char*)"</xdr:to>");
					break;
				}
				///////////////////////////////////////
				sb.Append((const UTF8Char*)"<xdr:clientData/>");
				switch (drawing->anchorType)
				{
				case Text::SpreadSheet::AnchorType::Absolute:
					sb.Append((const UTF8Char*)"<xdr:absoluteAnchor>");
					break;
				case Text::SpreadSheet::AnchorType::OneCell:
					sb.Append((const UTF8Char*)"<xdr:oneCellAnchor>");
					break;
				case Text::SpreadSheet::AnchorType::TwoCell:
					sb.Append((const UTF8Char*)"</xdr:twoCellAnchor>");
					break;
				}
				sb.Append((const UTF8Char*)"</xdr:wsDr>");
				drawingCnt++;
				Text::StrConcat(Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"xl/drawings/drawing"), drawingCnt), (const UTF8Char*)".xml");
				zip->AddFile(sbuff, sb.ToString(), sb.GetLength(), dt.ToTicks(), false);
				k++;
			}
		}
		i++;
	}

	sb.ClearStr();
	sb.Append((const UTF8Char*)"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n");
	sb.Append((const UTF8Char*)"<workbook xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\">");
	sb.Append((const UTF8Char*)"<fileVersion appName=\"AVIRead\"/>");
	sb.Append((const UTF8Char*)"<workbookPr backupFile=\"false\" showObjects=\"all\" date1904=\"false\"/>");
	sb.Append((const UTF8Char*)"<workbookProtection/>");
	sb.Append((const UTF8Char*)"<bookViews>");
	sb.Append((const UTF8Char*)"<workbookView showHorizontalScroll=\"true\" showVerticalScroll=\"true\" showSheetTabs=\"true\" xWindow=\"0\" yWindow=\"0\" windowWidth=\"16384\" windowHeight=\"8192\" tabRatio=\"500\" firstSheet=\"0\" activeTab=\"0\"/>");
	sb.Append((const UTF8Char*)"</bookViews>");
	sb.Append((const UTF8Char*)"<sheets>");
	i = 0;
	j = workbook->GetCount();
	while (i < j)
	{
		Text::SpreadSheet::Worksheet *sheet = workbook->GetItem(i);
		sb.Append((const UTF8Char*)"<sheet name=");
		csptr = sheet->GetName();
		csptr2 = Text::XML::ToNewAttrText(csptr);
		sb.Append(csptr2);
		Text::XML::FreeNewText(csptr2);
		sb.Append((const UTF8Char*)" sheetId=\"");
		sb.AppendUOSInt(i + 1);
		sb.Append((const UTF8Char*)"\" state=\"visible\" r:id=\"rId");
		sb.AppendUOSInt(i + 2);
		sb.Append((const UTF8Char*)"\"/>");
		i++;
	}
	sb.Append((const UTF8Char*)"</sheets>");
	sb.Append((const UTF8Char*)"<calcPr iterateCount=\"100\" refMode=\"A1\" iterate=\"false\" iterateDelta=\"0.001\"/>");
	sb.Append((const UTF8Char*)"</workbook>");
	zip->AddFile((const UTF8Char*)"xl/workbook.xml", sb.ToString(), sb.GetLength(), dt.ToTicks(), false);

	sb.ClearStr();
	sb.Append((const UTF8Char*)"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	sb.Append((const UTF8Char*)"<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">");
	sb.Append((const UTF8Char*)"<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument\" Target=\"xl/workbook.xml\"/>");
	sb.Append((const UTF8Char*)"<Relationship Id=\"rId2\" Type=\"http://schemas.openxmlformats.org/package/2006/relationships/metadata/core-properties\" Target=\"docProps/core.xml\"/>");
	sb.Append((const UTF8Char*)"<Relationship Id=\"rId3\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/extended-properties\" Target=\"docProps/app.xml\"/>");
	sb.Append((const UTF8Char*)"\n</Relationships>");
	zip->AddFile((const UTF8Char*)"_rels/.rels", sb.ToString(), sb.GetLength(), dt.ToTicks(), false);

	sb.ClearStr();
	sb.Append((const UTF8Char*)"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n");
	sb.Append((const UTF8Char*)"<Properties xmlns=\"http://schemas.openxmlformats.org/officeDocument/2006/extended-properties\" xmlns:vt=\"http://schemas.openxmlformats.org/officeDocument/2006/docPropsVTypes\">");
	sb.Append((const UTF8Char*)"<Template></Template>");
	sb.Append((const UTF8Char*)"<TotalTime>1</TotalTime>");
	sb.Append((const UTF8Char*)"<Application>");
	sb.Append((const UTF8Char*)"AVIRead/");
	IO::BuildTime::GetBuildTime(&dt2);
	dt2.ToString(sbuff, "yyyyMMdd_HHmmss");
	sb.Append(sbuff);
	sb.Append((const UTF8Char*)"</Application>");
	sb.Append((const UTF8Char*)"</Properties>");
	zip->AddFile((const UTF8Char*)"docProps/app.xml", sb.ToString(), sb.GetLength(), dt.ToTicks(), false);

	sb.ClearStr();
	sb.Append((const UTF8Char*)"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n");
	sb.Append((const UTF8Char*)"<cp:coreProperties xmlns:cp=\"http://schemas.openxmlformats.org/package/2006/metadata/core-properties\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:dcterms=\"http://purl.org/dc/terms/\" xmlns:dcmitype=\"http://purl.org/dc/dcmitype/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">");
	sb.Append((const UTF8Char*)"<dcterms:created xsi:type=\"dcterms:W3CDTF\">");
	t = workbook->GetCreateTime();
	if (t)
	{
		t->ToString(sbuff, "yyyy-MM-dd");
		sb.Append(sbuff);
		sb.AppendChar('T', 1);
		t->ToString(sbuff, "HH:mm:ss");
		sb.Append(sbuff);
		sb.AppendChar('Z', 1);
	}
	sb.Append((const UTF8Char*)"</dcterms:created>");
	sb.Append((const UTF8Char*)"<dc:creator>");
	csptr = workbook->GetAuthor();
	if (csptr)
	{
		csptr2 = Text::XML::ToNewXMLText(csptr);
		sb.Append(csptr2);
		Text::XML::FreeNewText(csptr2);
	}
	sb.Append((const UTF8Char*)"</dc:creator>");
	sb.Append((const UTF8Char*)"<dc:description>");
	csptr = 0;
	if (csptr)
	{
		csptr2 = Text::XML::ToNewXMLText(csptr);
		sb.Append(csptr2);
		Text::XML::FreeNewText(csptr2);
	}
	sb.Append((const UTF8Char*)"</dc:description>");
	sb.Append((const UTF8Char*)"<dc:language>");
	UInt32 lcid = Text::EncodingFactory::GetSystemLCID();
	Text::Locale::LocaleEntry *loc = Text::Locale::GetLocaleEntry(lcid);
	if (loc)
	{
		csptr2 = Text::XML::ToNewXMLText(loc->shortName);
		sb.Append(csptr2);
		Text::XML::FreeNewText(csptr2);
	}
	sb.Append((const UTF8Char*)"</dc:language>");
	sb.Append((const UTF8Char*)"<cp:lastModifiedBy>");
	csptr = workbook->GetLastAuthor();
	if (csptr)
	{
		csptr2 = Text::XML::ToNewXMLText(csptr);
		sb.Append(csptr2);
		Text::XML::FreeNewText(csptr2);
	}
	sb.Append((const UTF8Char*)"</cp:lastModifiedBy>");
	sb.Append((const UTF8Char*)"<dcterms:modified xsi:type=\"dcterms:W3CDTF\">");
	t = workbook->GetModifyTime();
	if (t)
	{
		t->ToString(sbuff, "yyyy-MM-dd");
		sb.Append(sbuff);
		sb.AppendChar('T', 1);
		t->ToString(sbuff, "HH:mm:ss");
		sb.Append(sbuff);
		sb.AppendChar('Z', 1);
	}
	sb.Append((const UTF8Char*)"</dcterms:modified>");
	sb.Append((const UTF8Char*)"<cp:revision>");
	sb.AppendU32(1);
	sb.Append((const UTF8Char*)"</cp:revision>");
	sb.Append((const UTF8Char*)"<dc:subject>");
	csptr = 0;
	if (csptr)
	{
		csptr2 = Text::XML::ToNewXMLText(csptr);
		sb.Append(csptr2);
		Text::XML::FreeNewText(csptr2);
	}
	sb.Append((const UTF8Char*)"</dc:subject>");
	sb.Append((const UTF8Char*)"<dc:title>");
	csptr = 0;
	if (csptr)
	{
		csptr2 = Text::XML::ToNewXMLText(csptr);
		sb.Append(csptr2);
		Text::XML::FreeNewText(csptr2);
	}
	sb.Append((const UTF8Char*)"</dc:title>");
	sb.Append((const UTF8Char*)"</cp:coreProperties>");
	zip->AddFile((const UTF8Char*)"docProps/core.xml", sb.ToString(), sb.GetLength(), dt.ToTicks(), false);

	sb.ClearStr();
	sb.Append((const UTF8Char*)"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	sb.Append((const UTF8Char*)"<Types xmlns=\"http://schemas.openxmlformats.org/package/2006/content-types\">");
	sb.Append((const UTF8Char*)"<Default Extension=\"xml\" ContentType=\"application/xml\"/>");
	sb.Append((const UTF8Char*)"<Default Extension=\"rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>");
	sb.Append((const UTF8Char*)"<Default Extension=\"png\" ContentType=\"image/png\"/>");
	sb.Append((const UTF8Char*)"<Default Extension=\"jpeg\" ContentType=\"image/jpeg\"/>");
	sb.Append((const UTF8Char*)"<Override PartName=\"/xl/_rels/workbook.xml.rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>");
	i = 0;
	j = workbook->GetCount();
	while (i < j)
	{
		sb.Append((const UTF8Char*)"<Override PartName=\"/xl/worksheets/sheet");
		sb.AppendUOSInt(i + 1);
		sb.Append((const UTF8Char*)".xml\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.worksheet+xml\"/>");
		i++;
	}
	sb.Append((const UTF8Char*)"<Override PartName=\"/xl/workbook.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet.main+xml\"/>");
	sb.Append((const UTF8Char*)"<Override PartName=\"/xl/styles.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.styles+xml\"/>");
	sb.Append((const UTF8Char*)"<Override PartName=\"/_rels/.rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>");
	sb.Append((const UTF8Char*)"<Override PartName=\"/docProps/app.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.extended-properties+xml\"/>");
	sb.Append((const UTF8Char*)"<Override PartName=\"/docProps/core.xml\" ContentType=\"application/vnd.openxmlformats-package.core-properties+xml\"/>");
	sb.Append((const UTF8Char*)"\n</Types>");
	zip->AddFile((const UTF8Char*)"[Content_Types].xml", sb.ToString(), sb.GetLength(), dt.ToTicks(), false);

	sb.ClearStr();
	sb.Append((const UTF8Char*)"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\r\n");
	sb.Append((const UTF8Char*)"<styleSheet xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\">");
	{
		Data::StringUTF8Map<UOSInt> numFmtMap;
		Data::ArrayList<const UTF8Char*> numFmts;
		csptr = (const UTF8Char*)"General";
		numFmtMap.Put(csptr, numFmts.GetCount());
		numFmts.Add(csptr);

		i = 0;
		j = workbook->GetStyleCount();
		while (i < j)
		{
			Text::SpreadSheet::CellStyle *style = workbook->GetStyle(i);
			csptr = style->GetDataFormat();
			if (csptr == 0)
			{
				csptr = (const UTF8Char*)"General";
			}
			if (!numFmtMap.ContainsKey(csptr))
			{
				numFmtMap.Put(csptr, numFmts.GetCount());
				numFmts.Add(csptr);
			}
			i++;
		}
		if (numFmts.GetCount() > 0)
		{
			sb.Append((const UTF8Char*)"<numFmts count=\"");
			sb.AppendUOSInt(numFmts.GetCount());
			sb.Append((const UTF8Char*)"\">");
			i = 0;
			j = numFmts.GetCount();
			while (i < j)
			{
				sb.Append((const UTF8Char*)"<numFmt numFmtId=\"");
				sb.AppendUOSInt(i + 164);
				sb.Append((const UTF8Char*)"\" formatCode=");
				ToFormatCode(sbuff, numFmts.GetItem(i));
				csptr = Text::XML::ToNewAttrText(sbuff);
				sb.Append(csptr);
				Text::XML::FreeNewText(csptr);
				sb.Append((const UTF8Char*)"/>");
				i++;
			}
			sb.Append((const UTF8Char*)"</numFmts>");
		}
		if (workbook->GetFontCount() > 0)
		{
			sb.Append((const UTF8Char*)"<fonts count=\"");
			sb.AppendUOSInt(workbook->GetFontCount());
			sb.Append((const UTF8Char*)"\">");
			i = 0;
			j = workbook->GetFontCount();
			while (i < j)
			{
				Text::SpreadSheet::WorkbookFont *font = workbook->GetFont(i);
				sb.Append((const UTF8Char*)"<font>");
				if (font->GetSize() != 0)
				{
					sb.Append((const UTF8Char*)"<sz val=\"");
					Text::SBAppendF64(&sb, font->GetSize());
					sb.Append((const UTF8Char*)"\"/>");
				}
				if (font->GetName())
				{
					sb.Append((const UTF8Char*)"<name val=");
					csptr = Text::XML::ToNewAttrText(font->GetName());
					sb.Append(csptr);
					Text::XML::FreeNewText(csptr);
					sb.Append((const UTF8Char*)"/>");
				}
				sb.Append((const UTF8Char*)"<family val=\"0\"/>");
				sb.Append((const UTF8Char*)"</font>");
				i++;
			}
			sb.Append((const UTF8Char*)"</fonts>");
		}

		sb.Append((const UTF8Char*)"<fills count=\"1\">");
		sb.Append((const UTF8Char*)"<fill>");
		sb.Append((const UTF8Char*)"<patternFill patternType=\"none\"/>");
		sb.Append((const UTF8Char*)"</fill>");
		sb.Append((const UTF8Char*)"</fills>");

		sb.Append((const UTF8Char*)"<borders count=\"1\">");
		sb.Append((const UTF8Char*)"<border diagonalUp=\"false\" diagonalDown=\"false\">");
		sb.Append((const UTF8Char*)"<left/>");
		sb.Append((const UTF8Char*)"<right/>");
		sb.Append((const UTF8Char*)"<top/>");
		sb.Append((const UTF8Char*)"<bottom/>");
		sb.Append((const UTF8Char*)"<diagonal/>");
		sb.Append((const UTF8Char*)"</border>");
		sb.Append((const UTF8Char*)"</borders>");

		sb.Append((const UTF8Char*)"<cellStyleXfs count=\"1\">");
		sb.Append((const UTF8Char*)"<xf numFmtId=\"164\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"true\" applyAlignment=\"true\" applyProtection=\"true\">");
		sb.Append((const UTF8Char*)"<alignment horizontal=\"general\" vertical=\"bottom\" textRotation=\"0\" wrapText=\"false\" indent=\"0\" shrinkToFit=\"false\"/>");
		sb.Append((const UTF8Char*)"<protection locked=\"true\" hidden=\"false\"/>");
		sb.Append((const UTF8Char*)"</xf>");
		sb.Append((const UTF8Char*)"</cellStyleXfs>");

		if (workbook->GetStyleCount() > 0)
		{
			sb.Append((const UTF8Char*)"<cellXfs count=\"");
			sb.AppendUOSInt(workbook->GetStyleCount());
			sb.Append((const UTF8Char*)"\">");
			i = 0;
			j = workbook->GetStyleCount();
			while (i < j)
			{
				Text::SpreadSheet::CellStyle *style = workbook->GetStyle(i);
				Text::SpreadSheet::WorkbookFont *font = style->GetFont();
				csptr = style->GetDataFormat();
				if (csptr == 0)
				{
					csptr = (const UTF8Char*)"General";
				}
				sb.Append((const UTF8Char*)"<xf numFmtId=\"");
				sb.AppendUOSInt(numFmtMap.Get(csptr) + 164);
				sb.Append((const UTF8Char*)"\" fontId=\"");
				if (font == 0)
				{
					sb.AppendChar('0', 1);
				}
				else
				{
					sb.AppendUOSInt(workbook->GetFontIndex(font));
				}
				sb.Append((const UTF8Char*)"\" fillId=\"0\" borderId=\"0\" xfId=\"0\" applyFont=\"");
				if (font)
				{
					sb.Append((const UTF8Char*)"true");
				}
				else
				{
					sb.Append((const UTF8Char*)"false");
				}
				sb.Append((const UTF8Char*)"\" applyBorder=\"false\" applyAlignment=\"true\" applyProtection=\"false\">");
				sb.Append((const UTF8Char*)"<alignment horizontal=\"");
				switch (style->GetHAlign())
				{
				case Text::SpreadSheet::HAlignment::Left:
					sb.Append((const UTF8Char*)"left");
					break;
				case Text::SpreadSheet::HAlignment::Center:
					sb.Append((const UTF8Char*)"center");
					break;
				case Text::SpreadSheet::HAlignment::Right:
					sb.Append((const UTF8Char*)"right");
					break;
				case Text::SpreadSheet::HAlignment::Fill:
					sb.Append((const UTF8Char*)"fill");
					break;
				case Text::SpreadSheet::HAlignment::Justify:
					sb.Append((const UTF8Char*)"justify");
					break;
				case Text::SpreadSheet::HAlignment::Unknown:
				default:
					sb.Append((const UTF8Char*)"general");
					break;
				}
				sb.Append((const UTF8Char*)"\" vertical=\"");
				switch (style->GetVAlign())
				{
				case Text::SpreadSheet::VAlignment::Top:
					sb.Append((const UTF8Char*)"top");
					break;
				case Text::SpreadSheet::VAlignment::Center:
					sb.Append((const UTF8Char*)"center");
					break;
				case Text::SpreadSheet::VAlignment::Bottom:
					sb.Append((const UTF8Char*)"bottom");
					break;
				case Text::SpreadSheet::VAlignment::Justify:
					sb.Append((const UTF8Char*)"justify");
					break;
				case Text::SpreadSheet::VAlignment::Unknown:
				default:
					sb.Append((const UTF8Char*)"general");
					break;
				}
				sb.Append((const UTF8Char*)"\" textRotation=\"0\" wrapText=\"");
				sb.Append(style->GetWordWrap()?(const UTF8Char*)"true":(const UTF8Char*)"false");
				sb.Append((const UTF8Char*)"\" indent=\"0\" shrinkToFit=\"false\"/>");
				sb.Append((const UTF8Char*)"<protection locked=\"true\" hidden=\"false\"/>");
				sb.Append((const UTF8Char*)"</xf>");
				i++;
			}
			sb.Append((const UTF8Char*)"</cellXfs>");
		}
	}
	sb.Append((const UTF8Char*)"</styleSheet>");
	zip->AddFile((const UTF8Char*)"xl/styles.xml", sb.ToString(), sb.GetLength(), dt.ToTicks(), false);

	if (sharedStrings.GetCount() > 0)
	{
		sb.ClearStr();
		sb.Append((const UTF8Char*)"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\r\n");
		sb.Append((const UTF8Char*)"<sst xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\" count=\"");
		sb.AppendUOSInt(sharedStrings.GetCount());
		sb.Append((const UTF8Char*)"\" uniqueCount=\"");
		sb.AppendUOSInt(sharedStrings.GetCount());
		sb.Append((const UTF8Char*)"\">");
		i = 0;
		j = sharedStrings.GetCount();
		while (i < j)
		{
			sb.Append((const UTF8Char*)"<si><t xml:space=\"preserve\">");
			csptr = Text::XML::ToNewXMLText(sharedStrings.GetItem(i));
			sb.Append(csptr);
			Text::XML::FreeNewText(csptr);
			sb.Append((const UTF8Char*)"</t></si>");
			i++;
		}
		sb.Append((const UTF8Char*)"</sst>");
		zip->AddFile((const UTF8Char*)"xl/sharedStrings.xml", sb.ToString(), sb.GetLength(), dt.ToTicks(), false);
	}

	sb.ClearStr();
	sb.Append((const UTF8Char*)"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	sb.Append((const UTF8Char*)"<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">");
	sb.Append((const UTF8Char*)"<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles\" Target=\"styles.xml\"/>");
	i = 0;
	j = workbook->GetCount();
	while (i < j)
	{
		sb.Append((const UTF8Char*)"<Relationship Id=\"rId");
		sb.AppendUOSInt(i + 2);
		sb.Append((const UTF8Char*)"\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/worksheet\" Target=\"worksheets/sheet");
		sb.AppendUOSInt(i + 1);
		sb.Append((const UTF8Char*)".xml\"/>");
		i++;
	}
	if (sharedStrings.GetCount() > 0)
	{
		sb.Append((const UTF8Char*)"<Relationship Id=\"rId");
		sb.AppendUOSInt(workbook->GetCount() + 2);
		sb.Append((const UTF8Char*)"\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/sharedStrings\" Target=\"sharedStrings.xml\"/>");
	}
	sb.Append((const UTF8Char*)"\n</Relationships>");
	zip->AddFile((const UTF8Char*)"xl/_rels/workbook.xml.rels", sb.ToString(), sb.GetLength(), dt.ToTicks(), false);

	DEL_CLASS(zip);
	return true;
}

UTF8Char *Exporter::XLSXExporter::ToFormatCode(UTF8Char *sbuff, const UTF8Char *dataFormat)
{
	UTF8Char c;
	while (true)
	{
		c = *dataFormat++;
		if (c == 0)
		{
			*sbuff = 0;
			break;
		}
		else if (c == '-')
		{
			*sbuff++ = '\\';
			*sbuff++ = c;
		}
		else
		{
			*sbuff++ = Text::CharUtil::ToLower(c);
		}
	}
	return sbuff;
}
