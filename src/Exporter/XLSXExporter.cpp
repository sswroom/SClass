#include "Stdafx.h"
#include "MyMemory.h"
#include "Exporter/XLSXExporter.h"
#include "IO/BuildTime.h"
#include "IO/ZIPBuilder.h"
#include "Text/Locale.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
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
	if (pobj->GetParserType() != IO::ParsedObject::PT_WORKBOOK)
	{
		return IO::FileExporter::ST_NOT_SUPPORTED;
	}
	return IO::FileExporter::ST_NORMAL_STREAM;
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
	if (pobj->GetParserType() != IO::ParsedObject::PT_WORKBOOK)
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
	dt.SetCurrTimeUTC();
	NEW_CLASS(zip, IO::ZIPBuilder(stm));

	sb.ClearStr();
	sb.Append((const UTF8Char*)"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	sb.Append((const UTF8Char*)"<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">");
	sb.Append((const UTF8Char*)"<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles\" Target=\"styles.xml\"/>");
	i = 0;
	while (i < j)
	{
		sb.Append((const UTF8Char*)"<Relationship Id=\"rId");
		sb.AppendUOSInt(i + 2);
		sb.Append((const UTF8Char*)"\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/worksheet\" Target=\"worksheets/sheet");
		sb.AppendUOSInt(i + 1);
		sb.Append((const UTF8Char*)".xml\"/>");
		i++;
	}
	sb.Append((const UTF8Char*)"\n</Relationships>");
	zip->AddFile((const UTF8Char*)"xl/_rels/workbook.xml.rels", sb.ToString(), sb.GetLength(), dt.ToTicks(), false);

	i = 0;
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
		sb.Append((const UTF8Char*)"<sheetData/>");
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
		sb.Append((const UTF8Char*)"</worksheet>");
		Text::StrConcat(Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"xl/worksheets/sheet"), i + 1), (const UTF8Char*)".xml");
		zip->AddFile(sbuff, sb.ToString(), sb.GetLength(), dt.ToTicks(), false);
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

	//xl/styles.xml

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

	DEL_CLASS(zip);
	return true;
}
