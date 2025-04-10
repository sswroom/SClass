#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListArr.h"
#include "Data/FastStringMap.h"
#include "Data/StringUTF8Map.h"
#include "Exporter/XLSXExporter.h"
#include "IO/BuildTime.h"
#include "IO/ZIPBuilder.h"
#include "Math/Math.h"
#include "Math/RectArea.h"
#include "Text/CharUtil.h"
#include "Text/Locale.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XLSUtil.h"
#include "Text/XML.h"
#include "Text/SpreadSheet/Workbook.h"

using namespace Text::SpreadSheet;

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

IO::FileExporter::SupportType Exporter::XLSXExporter::IsObjectSupported(NN<IO::ParsedObject> pobj)
{
	if (pobj->GetParserType() != IO::ParserType::Workbook)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	return IO::FileExporter::SupportType::NormalStream;
}

Bool Exporter::XLSXExporter::GetOutputName(UOSInt index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("Excel XLSX File"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.xlsx"));
		return true;
	}
	return false;
}

Bool Exporter::XLSXExporter::ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
{
	if (pobj->GetParserType() != IO::ParserType::Workbook)
	{
		return false;
	}
	NN<Text::SpreadSheet::Workbook> workbook = NN<Text::SpreadSheet::Workbook>::ConvertFrom(pobj);
	NN<Text::SpreadSheet::Worksheet> sheet;
	NN<Text::SpreadSheet::CellStyle> tmpStyle;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sbContTypes;
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	Data::Timestamp ts;
	Data::DateTime dt2;
	Data::DateTime *t;
	NN<Text::String> s;
	NN<Text::String> s2;
	UnsafeArrayOpt<const UTF8Char> csptr;
	UnsafeArray<const UTF8Char> nncsptr;
	IO::ZIPBuilder *zip;
	UOSInt i;
	UOSInt k;
	UOSInt l;
	UOSInt m;
	UOSInt n;
	UOSInt drawingCnt = 0;
	UOSInt chartCnt = 0;
	Data::ArrayListNN<Text::String> sharedStrings;
	Data::FastStringMap<UOSInt> stringMap;
	ts = Data::Timestamp::UtcNow();
	NEW_CLASS(zip, IO::ZIPBuilder(stm, IO::ZIPOS::MSDOS));

	Bool dirXl = false;
	Bool dirXlWs = false;
	Bool dirXlWsRel = false;
	Bool dirXlDraw = false;
	Bool dirXlDrawRel = false;
	Bool dirXlChart = false;
	Bool dirRel = false;
	Bool dirXlRel = false;

	sbContTypes.AppendC(UTF8STRC("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"));
	sbContTypes.AppendC(UTF8STRC("<Types xmlns=\"http://schemas.openxmlformats.org/package/2006/content-types\">"));
	sbContTypes.AppendC(UTF8STRC("<Default Extension=\"xml\" ContentType=\"application/xml\"/>"));
	sbContTypes.AppendC(UTF8STRC("<Default Extension=\"rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>"));
	sbContTypes.AppendC(UTF8STRC("<Default Extension=\"png\" ContentType=\"image/png\"/>"));
	sbContTypes.AppendC(UTF8STRC("<Default Extension=\"jpeg\" ContentType=\"image/jpeg\"/>"));
	sbContTypes.AppendC(UTF8STRC("<Override PartName=\"/xl/_rels/workbook.xml.rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>"));

	NN<Text::SpreadSheet::Worksheet::RowData> row;
	Data::ArrayIterator<NN<Text::SpreadSheet::Worksheet>> itSheet;

	sb.ClearStr();
	sb.AppendC(UTF8STRC("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"));
	sb.AppendC(UTF8STRC("<workbook xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\">"));
	sb.AppendC(UTF8STRC("<fileVersion appName=\"AVIRead\"/>"));
	sb.AppendC(UTF8STRC("<workbookPr backupFile=\"false\" showObjects=\"all\" date1904=\"false\"/>"));
	sb.AppendC(UTF8STRC("<workbookProtection/>"));
	sb.AppendC(UTF8STRC("<bookViews>"));
	sb.AppendC(UTF8STRC("<workbookView showHorizontalScroll=\"true\" showVerticalScroll=\"true\" showSheetTabs=\"true\" xWindow=\"0\" yWindow=\"0\" windowWidth=\"16384\" windowHeight=\"8192\" tabRatio=\"500\" firstSheet=\"0\" activeTab=\"0\"/>"));
	sb.AppendC(UTF8STRC("</bookViews>"));
	sb.AppendC(UTF8STRC("<sheets>"));
	itSheet = workbook->Iterator();
	i = 0;
	while (itSheet.HasNext())
	{
		sheet = itSheet.Next();
		sb.AppendC(UTF8STRC("<sheet name="));
		s = sheet->GetName();
		s2 = Text::XML::ToNewAttrText(s->v);
		sb.Append(s2);
		s2->Release();
		sb.AppendC(UTF8STRC(" sheetId=\""));
		sb.AppendUOSInt(i + 1);
		sb.AppendC(UTF8STRC("\" state=\"visible\" r:id=\"rId"));
		sb.AppendUOSInt(i + 3);
		sb.AppendC(UTF8STRC("\"/>"));
		i++;
	}
	sb.AppendC(UTF8STRC("</sheets>"));
	sb.AppendC(UTF8STRC("<calcPr iterateCount=\"100\" refMode=\"A1\" iterate=\"false\" iterateDelta=\"0.001\"/>"));
	sb.AppendC(UTF8STRC("</workbook>"));
	if (!dirXl)
	{
		zip->AddDir(CSTR("xl/"), ts, ts, ts, 0);
		dirXl = true;
	}
	zip->AddFile(CSTR("xl/workbook.xml"), sb.ToString(), sb.GetLength(), ts, ts, ts, Data::Compress::Inflate::CompressionLevel::BestCompression, 0);
	sbContTypes.AppendC(UTF8STRC("<Override PartName=\"/xl/workbook.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet.main+xml\"/>"));

	sb.ClearStr();
	sb.Append(CSTR("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"));
	sb.Append(CSTR("<a:theme xmlns:a=\"http://schemas.openxmlformats.org/drawingml/2006/main\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" name=\"Office\">"));
	sb.Append(CSTR("<a:themeElements><a:clrScheme name=\"LibreOffice\"><a:dk1><a:srgbClr val=\"000000\"/></a:dk1><a:lt1><a:srgbClr val=\"ffffff\"/></a:lt1><a:dk2><a:srgbClr val=\"000000\"/></a:dk2>"));
	sb.Append(CSTR("<a:lt2><a:srgbClr val=\"ffffff\"/></a:lt2><a:accent1><a:srgbClr val=\"18a303\"/></a:accent1><a:accent2><a:srgbClr val=\"0369a3\"/></a:accent2>"));
	sb.Append(CSTR("<a:accent3><a:srgbClr val=\"a33e03\"/></a:accent3><a:accent4><a:srgbClr val=\"8e03a3\"/></a:accent4><a:accent5><a:srgbClr val=\"c99c00\"/></a:accent5>"));
	sb.Append(CSTR("<a:accent6><a:srgbClr val=\"c9211e\"/></a:accent6><a:hlink><a:srgbClr val=\"0000ee\"/></a:hlink><a:folHlink><a:srgbClr val=\"551a8b\"/></a:folHlink></a:clrScheme>"));
	sb.Append(CSTR("<a:fontScheme name=\"Office\"><a:majorFont><a:latin typeface=\"Arial\" pitchFamily=\"0\" charset=\"1\"/><a:ea typeface=\"DejaVu Sans\" pitchFamily=\"0\" charset=\"1\"/>"));
	sb.Append(CSTR("<a:cs typeface=\"DejaVu Sans\" pitchFamily=\"0\" charset=\"1\"/></a:majorFont><a:minorFont><a:latin typeface=\"Arial\" pitchFamily=\"0\" charset=\"1\"/><a:ea typeface=\"DejaVu Sans\" pitchFamily=\"0\" charset=\"1\"/>"));
	sb.Append(CSTR("<a:cs typeface=\"DejaVu Sans\" pitchFamily=\"0\" charset=\"1\"/></a:minorFont></a:fontScheme><a:fmtScheme><a:fillStyleLst><a:solidFill><a:schemeClr val=\"phClr\"></a:schemeClr></a:solidFill>"));
	sb.Append(CSTR("<a:solidFill><a:schemeClr val=\"phClr\"></a:schemeClr></a:solidFill><a:solidFill><a:schemeClr val=\"phClr\"></a:schemeClr></a:solidFill></a:fillStyleLst>"));
	sb.Append(CSTR("<a:lnStyleLst><a:ln w=\"6350\" cap=\"flat\" cmpd=\"sng\" algn=\"ctr\"><a:prstDash val=\"solid\"/><a:miter/></a:ln><a:ln w=\"6350\" cap=\"flat\" cmpd=\"sng\" algn=\"ctr\">"));
	sb.Append(CSTR("<a:prstDash val=\"solid\"/><a:miter/></a:ln><a:ln w=\"6350\" cap=\"flat\" cmpd=\"sng\" algn=\"ctr\"><a:prstDash val=\"solid\"/><a:miter/></a:ln></a:lnStyleLst>"));
	sb.Append(CSTR("<a:effectStyleLst><a:effectStyle><a:effectLst/></a:effectStyle><a:effectStyle><a:effectLst/></a:effectStyle><a:effectStyle><a:effectLst/></a:effectStyle></a:effectStyleLst><a:bgFillStyleLst>"));
	sb.Append(CSTR("<a:solidFill><a:schemeClr val=\"phClr\"></a:schemeClr></a:solidFill><a:solidFill><a:schemeClr val=\"phClr\"></a:schemeClr></a:solidFill><a:solidFill><a:schemeClr val=\"phClr\"></a:schemeClr></a:solidFill></a:bgFillStyleLst></a:fmtScheme></a:themeElements></a:theme>"));
	zip->AddDir(CSTR("xl/theme/"), ts, ts, ts, 0);
	zip->AddFile(CSTR("xl/theme/theme1.xml"), sb.ToString(), sb.GetLength(), ts, ts, ts, Data::Compress::Inflate::CompressionLevel::BestCompression, 0);
	//sbContTypes.AppendC(UTF8STRC("<Override PartName=\"/xl/theme/theme1.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.theme+xml\"/>"));

	sb.ClearStr();
	sb.AppendC(UTF8STRC("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"));
	sb.AppendC(UTF8STRC("<styleSheet xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\">"));
	{
		Data::StringUTF8Map<UOSInt> numFmtMap;
		Data::ArrayListArr<const UTF8Char> numFmts;
		Data::ArrayList<BorderInfo*> borders;
		Text::SpreadSheet::CellStyle::BorderStyle borderNone;
		borderNone.borderType = BorderType::None;
		borderNone.borderColor = 0;
		BorderInfo *border = MemAlloc(BorderInfo, 1);
		border->left = borderNone;
		border->top = borderNone;
		border->right = borderNone;
		border->bottom = borderNone;
		borders.Add(border);

		nncsptr = (const UTF8Char*)"General";
		numFmtMap.Put(nncsptr, numFmts.GetCount());
		numFmts.Add(nncsptr);

		Data::ArrayIterator<NN<Text::SpreadSheet::CellStyle>> itStyle = workbook->StyleIterator();
		i = 0;
		while (itStyle.HasNext())
		{
			NN<Text::SpreadSheet::CellStyle> style = itStyle.Next();
			NN<Text::String> optS;
			if (!style->GetDataFormat().SetTo(optS))
			{
				nncsptr = U8STR("General");
			}
			else
			{
				nncsptr = UnsafeArray<const UTF8Char>(optS->v);
			}
			if (!numFmtMap.ContainsKey(nncsptr))
			{
				numFmtMap.Put(nncsptr, numFmts.GetCount());
				numFmts.Add(nncsptr);
			}
			Bool borderFound = false;
			k = borders.GetCount();
			while (k-- > 0)
			{
				border = borders.GetItem(k);
				if (border->left == style->GetBorderLeft() &&
					border->top == style->GetBorderTop() &&
					border->right == style->GetBorderRight() &&
					border->bottom == style->GetBorderBottom())
				{
					borderFound = true;
					break;
				}
			}
			if (!borderFound)
			{
				border = MemAlloc(BorderInfo, 1);
				border->left = style->GetBorderLeft();
				border->top = style->GetBorderTop();
				border->right = style->GetBorderRight();
				border->bottom = style->GetBorderBottom();
				borders.Add(border);
			}
		}
		if (numFmts.GetCount() > 0)
		{
			sb.AppendC(UTF8STRC("<numFmts count=\""));
			sb.AppendUOSInt(numFmts.GetCount());
			sb.AppendC(UTF8STRC("\">"));
			i = 0;
			k = numFmts.GetCount();
			while (i < k)
			{
				sb.AppendC(UTF8STRC("<numFmt numFmtId=\""));
				sb.AppendUOSInt(i + 164);
				sb.AppendC(UTF8STRC("\" formatCode="));
				ToFormatCode(sbuff, numFmts.GetItemNoCheck(i));
				s = Text::XML::ToNewAttrText(UARR(sbuff));
				sb.Append(s);
				s->Release();
				sb.AppendC(UTF8STRC("/>"));
				i++;
			}
			sb.AppendC(UTF8STRC("</numFmts>"));
		}
		if (workbook->GetFontCount() > 0)
		{
			sb.AppendC(UTF8STRC("<fonts count=\""));
			sb.AppendUOSInt(workbook->GetFontCount());
			sb.AppendC(UTF8STRC("\">"));
			i = 0;
			k = workbook->GetFontCount();
			while (i < k)
			{
				NN<Text::SpreadSheet::WorkbookFont> font = workbook->GetFontNoCheckc(i);
				sb.AppendC(UTF8STRC("<font>"));
				if (font->GetSize() != 0)
				{
					sb.AppendC(UTF8STRC("<sz val=\""));
					sb.AppendDouble(font->GetSize());
					sb.AppendC(UTF8STRC("\"/>"));
				}
				if (font->GetName().SetTo(s))
				{
					sb.AppendC(UTF8STRC("<name val="));
					s = Text::XML::ToNewAttrText(s->v);
					sb.Append(s);
					s->Release();
					sb.AppendC(UTF8STRC("/>"));
				}
				switch (font->GetFamily())
				{
				case FontFamily::NA:
					sb.AppendC(UTF8STRC("<family val=\"0\"/>"));
					break;
				case FontFamily::Roman:
					sb.AppendC(UTF8STRC("<family val=\"1\"/>"));
					break;
				case FontFamily::Swiss:
					sb.AppendC(UTF8STRC("<family val=\"2\"/>"));
					break;
				case FontFamily::Modern:
					sb.AppendC(UTF8STRC("<family val=\"3\"/>"));
					break;
				case FontFamily::Script:
					sb.AppendC(UTF8STRC("<family val=\"4\"/>"));
					break;
				case FontFamily::Decorative:
					sb.AppendC(UTF8STRC("<family val=\"5\"/>"));
					break;
				}
				sb.AppendC(UTF8STRC("</font>"));
				i++;
			}
			sb.AppendC(UTF8STRC("</fonts>"));
		}

		sb.AppendC(UTF8STRC("<fills count=\"2\">"));
		sb.AppendC(UTF8STRC("<fill>"));
		sb.AppendC(UTF8STRC("<patternFill patternType=\"none\"/>"));
		sb.AppendC(UTF8STRC("</fill>"));
		sb.AppendC(UTF8STRC("<fill>"));
		sb.AppendC(UTF8STRC("<patternFill patternType=\"gray125\"/>"));
		sb.AppendC(UTF8STRC("</fill>"));
		sb.AppendC(UTF8STRC("</fills>"));

		i = 0;
		k = borders.GetCount();
		sb.AppendC(UTF8STRC("<borders count=\""));
		sb.AppendUOSInt(k);
		sb.AppendC(UTF8STRC("\">"));
		while (i < k)
		{
			border = borders.GetItem(i);
			sb.AppendC(UTF8STRC("<border diagonalUp=\"false\" diagonalDown=\"false\">"));
			AppendBorder(sb, border->left, CSTR("left"));
			AppendBorder(sb, border->right, CSTR("right"));
			AppendBorder(sb, border->top, CSTR("top"));
			AppendBorder(sb, border->bottom, CSTR("bottom"));
			sb.AppendC(UTF8STRC("<diagonal/>"));
			sb.AppendC(UTF8STRC("</border>"));
			i++;
		}
		sb.AppendC(UTF8STRC("</borders>"));

		sb.AppendC(UTF8STRC("<cellStyleXfs count=\"20\">"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"164\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"true\" applyAlignment=\"true\" applyProtection=\"true\">"));
		sb.AppendC(UTF8STRC("<alignment horizontal=\"general\" vertical=\"bottom\" textRotation=\"0\" wrapText=\"false\" indent=\"0\" shrinkToFit=\"false\"/>"));
		sb.AppendC(UTF8STRC("<protection locked=\"true\" hidden=\"false\"/>"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"1\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"1\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"2\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"2\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"43\" fontId=\"1\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"41\" fontId=\"1\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"44\" fontId=\"1\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"42\" fontId=\"1\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("<xf numFmtId=\"9\" fontId=\"1\" fillId=\"0\" borderId=\"0\" applyFont=\"true\" applyBorder=\"false\" applyAlignment=\"false\" applyProtection=\"false\">"));
		sb.AppendC(UTF8STRC("</xf>"));
		sb.AppendC(UTF8STRC("</cellStyleXfs>"));

		if (workbook->GetStyleCount() > 0)
		{
			sb.AppendC(UTF8STRC("<cellXfs count=\""));
			sb.AppendUOSInt(workbook->GetStyleCount());
			sb.AppendC(UTF8STRC("\">"));
			i = 0;
			k = workbook->GetStyleCount();
			while (i < k)
			{
				NN<Text::SpreadSheet::CellStyle> style;
				if (workbook->GetStyle(i).SetTo(style))
				{
					AppendXF(sb, style, borders, workbook, numFmtMap);
				}
				i++;
			}
			sb.AppendC(UTF8STRC("</cellXfs>"));
		}

		sb.AppendC(UTF8STRC("<cellStyles count=\"6\">"));
		sb.AppendC(UTF8STRC("<cellStyle name=\"Normal\" xfId=\"0\" builtinId=\"0\"/>"));
		sb.AppendC(UTF8STRC("<cellStyle name=\"Comma\" xfId=\"15\" builtinId=\"3\"/>"));
		sb.AppendC(UTF8STRC("<cellStyle name=\"Comma [0]\" xfId=\"16\" builtinId=\"6\"/>"));
		sb.AppendC(UTF8STRC("<cellStyle name=\"Currency\" xfId=\"17\" builtinId=\"4\"/>"));
		sb.AppendC(UTF8STRC("<cellStyle name=\"Currency [0]\" xfId=\"18\" builtinId=\"7\"/>"));
		sb.AppendC(UTF8STRC("<cellStyle name=\"Percent\" xfId=\"19\" builtinId=\"5\"/>"));
		sb.AppendC(UTF8STRC("</cellStyles>"));

		i = 0;
		k = borders.GetCount();
		while (i < k)
		{
			MemFree(borders.GetItem(i));
			i++;
		}
	}
	sb.AppendC(UTF8STRC("</styleSheet>"));
	zip->AddFile(CSTR("xl/styles.xml"), sb.ToString(), sb.GetLength(), ts, ts, ts, Data::Compress::Inflate::CompressionLevel::BestCompression, 0);
	sbContTypes.AppendC(UTF8STRC("<Override PartName=\"/xl/styles.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.styles+xml\"/>"));

	itSheet = workbook->Iterator();
	i = 0;
	while (itSheet.HasNext())
	{
		sheet = itSheet.Next();
		UOSInt maxRow = sheet->GetCount();
		UOSInt maxCol = 1;
		k = 0;
		while (k < maxRow)
		{
			if (sheet->GetItem(k).SetTo(row) && row->cells.GetCount() > maxCol)
			{
				maxCol = row->cells.GetCount();
			}
			k++;
		}
		Data::ArrayList<LinkInfo*> links;
		LinkInfo *link;

		sb.ClearStr();
		sb.AppendC(UTF8STRC("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"));
		sb.AppendC(UTF8STRC("<worksheet xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" xmlns:xdr=\"http://schemas.openxmlformats.org/drawingml/2006/spreadsheetDrawing\" xmlns:x14=\"http://schemas.microsoft.com/office/spreadsheetml/2009/9/main\" xmlns:xr2=\"http://schemas.microsoft.com/office/spreadsheetml/2015/revision2\" xmlns:mc=\"http://schemas.openxmlformats.org/markup-compatibility/2006\">"));
		sb.AppendC(UTF8STRC("<sheetPr filterMode=\"false\">"));
		sb.AppendC(UTF8STRC("<pageSetUpPr fitToPage=\"false\"/>"));
		sb.AppendC(UTF8STRC("</sheetPr>"));
		sb.AppendC(UTF8STRC("<dimension ref=\"A1"));
		if (maxRow > 1 || maxCol > 1)
		{
			sptr = Text::XLSUtil::GetCellID(sbuff, maxCol - 1, maxRow - 1);
			sb.AppendUTF8Char(':');
			sb.AppendP(sbuff, sptr);
		}
		sb.AppendC(UTF8STRC("\"/>"));
		sb.AppendC(UTF8STRC("<sheetViews>"));
		sb.AppendC(UTF8STRC("<sheetView showFormulas=\"false\" showGridLines=\"true\" showRowColHeaders=\"true\" showZeros=\"true\" rightToLeft=\"false\" tabSelected=\"true\" showOutlineSymbols=\"true\" defaultGridColor=\"true\" view=\"normal\" topLeftCell=\"A1\" colorId=\"64\" zoomScale=\""));
		sb.AppendU32(sheet->GetZoom());
		sb.AppendC(UTF8STRC("\" zoomScaleNormal=\""));
		sb.AppendU32(sheet->GetZoom());
		sb.AppendC(UTF8STRC("\" zoomScalePageLayoutView=\""));
		sb.AppendU32(sheet->GetZoom());
		sb.AppendC(UTF8STRC("\" workbookViewId=\"0\">"));
		sb.AppendC(UTF8STRC("<selection pane=\"topLeft\" activeCell=\"A1\" activeCellId=\"0\" sqref=\"A1\"/>"));
		sb.AppendC(UTF8STRC("</sheetView>"));
		sb.AppendC(UTF8STRC("</sheetViews>"));
		sb.AppendC(UTF8STRC("<sheetFormatPr defaultColWidth=\""));
		sb.AppendDouble(sheet->GetDefColWidthPt() / 5.25);
		sb.AppendC(UTF8STRC("\" defaultRowHeight=\""));
		sb.AppendDouble(sheet->GetDefRowHeightPt());
		sb.AppendC(UTF8STRC("\" zeroHeight=\"false\" outlineLevelRow=\"0\" outlineLevelCol=\"0\"></sheetFormatPr>"));
		Bool found = false;
		Double lastColWidth = -1;
		UOSInt lastColIndex = INVALID_INDEX;

		k = 0;
		l = sheet->GetColWidthCount();
		while (k < l)
		{
			if (sheet->GetColWidthPt(k) != lastColWidth)
			{
				if (lastColIndex != INVALID_INDEX)
				{
					if (!found)
					{
						found = true;
						sb.AppendC(UTF8STRC("<cols>"));
					}
					sb.AppendC(UTF8STRC("<col min=\""));
					sb.AppendUOSInt(lastColIndex + 1);
					sb.AppendC(UTF8STRC("\" max=\""));
					sb.AppendUOSInt(k);
					sb.AppendC(UTF8STRC("\" width=\""));
					if (lastColWidth >= 0)
					{
						sb.AppendDouble(lastColWidth / 5.25);
						sb.AppendC(UTF8STRC("\" customWidth=\"1\""));
					}
					else
					{
						sb.AppendDouble(sheet->GetDefColWidthPt() / 5.25);
						sb.AppendC(UTF8STRC("\" customWidth=\"false\""));
					}
					sb.AppendC(UTF8STRC("/>"));
				}
				lastColWidth = sheet->GetColWidthPt(k);
				lastColIndex = k;
			}
			k++;
		}
		if (lastColWidth >= 0)
		{
			if (!found)
			{
				found = true;
				sb.AppendC(UTF8STRC("<cols>"));
			}
			sb.AppendC(UTF8STRC("<col min=\""));
			sb.AppendUOSInt(lastColIndex + 1);
			sb.AppendC(UTF8STRC("\" max=\""));
			sb.AppendUOSInt(l);
			sb.AppendC(UTF8STRC("\" width=\""));
			if (lastColWidth >= 0)
			{
				sb.AppendDouble(lastColWidth / 5.25);
				sb.AppendC(UTF8STRC("\" customWidth=\"1\""));
			}
			else
			{
				sb.AppendDouble(sheet->GetDefColWidthPt() / 5.25);
				sb.AppendC(UTF8STRC("\" customWidth=\"false\""));
			}
			sb.AppendC(UTF8STRC("/>"));
		}
		if (l < sheet->GetMaxCol())
		{
			if (!found)
			{
				found = true;
				sb.AppendC(UTF8STRC("<cols>"));
			}
			sb.AppendC(UTF8STRC("<col min=\""));
			sb.AppendUOSInt(l + 1);
			sb.AppendC(UTF8STRC("\" max=\""));
			sb.AppendUOSInt(sheet->GetMaxCol());
			sb.AppendC(UTF8STRC("\" width=\""));
			sb.AppendDouble(sheet->GetDefColWidthPt() / 5.25);
			sb.AppendC(UTF8STRC("\" customWidth=\"false\" collapsed=\"false\" hidden=\"false\" outlineLevel=\"0\" style=\"0\"/>"));
		}
		if (found)
		{
			sb.AppendC(UTF8STRC("</cols>"));
		}
		Data::ArrayList<Math::RectArea<UOSInt>> mergeList;
		k = 0;
		l = sheet->GetCount();
		if (l > 0)
		{
			sb.AppendC(UTF8STRC("<sheetData>"));
			while (k < l)
			{
				if (sheet->GetItem(k).SetTo(row))
				{
					sb.AppendC(UTF8STRC("<row r=\""));
					sb.AppendUOSInt(k + 1);
					sb.AppendC(UTF8STRC("\" customFormat=\"false\" ht=\"12.8\" hidden=\"false\" customHeight=\"false\" outlineLevel=\"0\" collapsed=\"false\">"));

					m = 0;
					n = row->cells.GetCount();
					while (m < n)
					{
						NN<Text::String> cellValue;
						NN<Text::SpreadSheet::Worksheet::CellData> cell;
						if (row->cells.GetItem(m).SetTo(cell) && cell->cellValue.SetTo(cellValue) && cell->cdt != Text::SpreadSheet::CellDataType::MergedLeft && cell->cdt != Text::SpreadSheet::CellDataType::MergedUp)
						{
							sb.AppendC(UTF8STRC("<c r=\""));
							sptr = Text::StrUOSInt(Text::SpreadSheet::Workbook::ColCode(sbuff, m), k + 1);
							sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
							sb.AppendUTF8Char('"');
							if (cell->style.SetTo(tmpStyle))
							{
								sb.AppendC(UTF8STRC(" s=\""));
								sb.AppendUOSInt(tmpStyle->GetIndex());
								sb.AppendUTF8Char('"');
							}
							switch (cell->cdt)
							{
							case Text::SpreadSheet::CellDataType::String:
								sb.AppendC(UTF8STRC(" t=\"s\""));
								break;
							case Text::SpreadSheet::CellDataType::Number:
							case Text::SpreadSheet::CellDataType::DateTime:
								sb.AppendC(UTF8STRC(" t=\"n\""));
								break;
							case Text::SpreadSheet::CellDataType::MergedLeft:
							case Text::SpreadSheet::CellDataType::MergedUp:
								break;
							}
							sb.AppendC(UTF8STRC("><v>"));
							switch (cell->cdt)
							{
							case Text::SpreadSheet::CellDataType::String:
								{
									UOSInt sIndex = stringMap.GetNN(cellValue);
									if (sIndex == 0 && stringMap.IndexOf(cellValue) < 0)
									{
										sIndex = sharedStrings.Add(cellValue);
										stringMap.PutNN(cellValue, sIndex);
									}
									sb.AppendUOSInt(sIndex);
								}
								break;
							case Text::SpreadSheet::CellDataType::Number:
								sb.Append(cellValue);
								break;
							case Text::SpreadSheet::CellDataType::DateTime:
								{
									Data::DateTime dt;
									dt.ToLocalTime();
									dt.SetValue(cellValue->ToCString());
									sb.AppendDouble(Text::XLSUtil::Date2Number(Data::Timestamp::FromStr(cellValue->ToCString(), Data::DateTimeUtil::GetLocalTzQhr())));
								}
								break;
							case Text::SpreadSheet::CellDataType::MergedLeft:
							case Text::SpreadSheet::CellDataType::MergedUp:
								break;
							}
							
							if (cell->cellURL.NotNull())
							{
								link = MemAlloc(LinkInfo, 1);
								link->row = k;
								link->col = m;
								link->cell = cell;
								links.Add(link);
							}
							if (cell->mergeHori > 1 || cell->mergeVert > 1)
							{
								mergeList.Add(Math::RectArea<UOSInt>(m, k, cell->mergeHori, cell->mergeVert));
							}
							
							sb.AppendC(UTF8STRC("</v></c>"));
						}
						else if (row->cells.GetItem(m).SetTo(cell) && cell->style.SetTo(tmpStyle))
						{
							sb.AppendC(UTF8STRC("<c r=\""));
							sptr = Text::StrUOSInt(Text::SpreadSheet::Workbook::ColCode(sbuff, m), k + 1);
							sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
							sb.AppendUTF8Char('"');
							sb.AppendC(UTF8STRC(" s=\""));
							sb.AppendUOSInt(tmpStyle->GetIndex());
							sb.AppendUTF8Char('"');
							sb.AppendC(UTF8STRC("></c>"));
						}
						m++;
					}
					sb.AppendC(UTF8STRC("</row>"));
				}

				k++;
			}
			sb.AppendC(UTF8STRC("</sheetData>"));

			if (mergeList.GetCount() > 0)
			{
				sb.AppendC(UTF8STRC("<mergeCells count=\""));
				sb.AppendUOSInt(mergeList.GetCount());
				sb.AppendC(UTF8STRC("\">"));
				k = 0;
				l = mergeList.GetCount();
				while (k < l)
				{
					sb.AppendC(UTF8STRC("<mergeCell ref=\""));
					Math::RectArea<UOSInt> rect = mergeList.GetItem(k);
					sptr = Text::XLSUtil::GetCellID(sbuff, rect.min.x, rect.min.y);
					*sptr++ = ':';
					sptr = Text::XLSUtil::GetCellID(sptr, rect.max.x - 1, rect.max.y - 1);
					sb.AppendP(sbuff, sptr);
					sb.AppendC(UTF8STRC("\"/>"));
					k++;
				}
				sb.AppendC(UTF8STRC("</mergeCells>"));
			}

			if (links.GetCount() > 0)
			{
				UOSInt idBase = sheet->GetDrawingCount() + 1;
				sb.AppendC(UTF8STRC("<hyperlinks>"));
				m = 0;
				n = links.GetCount();
				while (m < n)
				{
					link = links.GetItem(m);
					sb.AppendC(UTF8STRC("<hyperlink ref=\""));
					sptr = Text::SpreadSheet::Workbook::ColCode(sbuff, link->col);
					sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
					sb.AppendUOSInt(link->row + 1);
					sb.AppendC(UTF8STRC("\" r:id=\"rId"));
					sb.AppendUOSInt(idBase + m);
					sb.AppendC(UTF8STRC("\" display="));
					s = Text::XML::ToNewAttrText(UnsafeArray<const UTF8Char>(Text::String::OrEmpty(link->cell->cellValue)->v));
					sb.Append(s);
					s->Release();
					sb.AppendC(UTF8STRC("/>"));
					m++;
				}
				sb.AppendC(UTF8STRC("</hyperlinks>"));
			}
		}
		else
		{
			sb.AppendC(UTF8STRC("<sheetData/>"));
		}
		//<sheetProtection sheet="true" password="cc1a" objects="true" scenarios="true"/><printOptions headings="false" gridLines="false" gridLinesSet="true" horizontalCentered="false" verticalCentered="false"/>
		sb.AppendC(UTF8STRC("<pageMargins left=\""));
		sb.AppendDouble(sheet->GetMarginLeft());
		sb.AppendC(UTF8STRC("\" right=\""));
		sb.AppendDouble(sheet->GetMarginRight());
		sb.AppendC(UTF8STRC("\" top=\""));
		sb.AppendDouble(sheet->GetMarginTop());
		sb.AppendC(UTF8STRC("\" bottom=\""));
		sb.AppendDouble(sheet->GetMarginBottom());
		sb.AppendC(UTF8STRC("\" header=\""));
		sb.AppendDouble(sheet->GetMarginHeader());
		sb.AppendC(UTF8STRC("\" footer=\""));
		sb.AppendDouble(sheet->GetMarginFooter());
		sb.AppendC(UTF8STRC("\"/>"));
		sb.AppendC(UTF8STRC("<pageSetup paperSize=\"9\" scale=\"100\" firstPageNumber=\"1\" fitToWidth=\"1\" fitToHeight=\"1\" pageOrder=\"downThenOver\" orientation=\"portrait\" blackAndWhite=\"false\" draft=\"false\" cellComments=\"none\" useFirstPageNumber=\"true\" horizontalDpi=\"300\" verticalDpi=\"300\" copies=\"1\"/>"));
		sb.AppendC(UTF8STRC("<headerFooter differentFirst=\"false\" differentOddEven=\"false\">"));
		sb.AppendC(UTF8STRC("<oddHeader>&amp;C&amp;&quot;Times New Roman,Regular&quot;&amp;12&amp;A</oddHeader>"));
		sb.AppendC(UTF8STRC("<oddFooter>&amp;C&amp;&quot;Times New Roman,Regular&quot;&amp;12Page &amp;P</oddFooter>"));
		sb.AppendC(UTF8STRC("</headerFooter>"));
		k = 0;
		l = sheet->GetDrawingCount();
		while (k < l)
		{
			sb.AppendC(UTF8STRC("<drawing r:id=\"rId"));
			sb.AppendUOSInt(k + 1);
			sb.AppendC(UTF8STRC("\"/>"));
			k++;
		}
		sb.AppendC(UTF8STRC("</worksheet>"));
		if (!dirXl)
		{
			zip->AddDir(CSTR("xl/"), ts, ts, ts, 0);
			dirXl = true;
		}
		if (!dirXlWs)
		{
			zip->AddDir(CSTR("xl/worksheets/"), ts, ts, ts, 0);
			dirXlWs = true;
		}
		sptr = Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("xl/worksheets/sheet")), i + 1), UTF8STRC(".xml"));
		zip->AddFile(CSTRP(sbuff, sptr), sb.ToString(), sb.GetLength(), ts, ts, ts, Data::Compress::Inflate::CompressionLevel::BestCompression, 0);
		sbContTypes.AppendC(UTF8STRC("<Override PartName=\"/"));
		sbContTypes.AppendC(sbuff, (UOSInt)(sptr - sbuff));
		sbContTypes.AppendC(UTF8STRC("\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.worksheet+xml\"/>"));

		if (sheet->GetDrawingCount() > 0 || links.GetCount() > 0)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"));
			sb.AppendC(UTF8STRC("<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"));
			k = 0;
			l = sheet->GetDrawingCount();
			while (k < l)
			{
				sb.AppendC(UTF8STRC("<Relationship Id=\"rId"));
				sb.AppendUOSInt(k + 1);
				sb.AppendC(UTF8STRC("\" Target=\"../drawings/drawing"));
				sb.AppendUOSInt(k + 1 + drawingCnt);
				sb.AppendC(UTF8STRC(".xml\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/drawing\"/>"));
				k++;
			}
			m = 0;
			n = links.GetCount();
			while (m < n)
			{
				link = links.GetItem(m);
				sb.AppendC(UTF8STRC("<Relationship Id=\"rId"));
				sb.AppendUOSInt(l + m + 1);
				sb.AppendC(UTF8STRC("\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/hyperlink\" Target="));
				s = Text::XML::ToNewAttrText(UnsafeArray<const UTF8Char>(Text::String::OrEmpty(link->cell->cellURL)->v));
				sb.Append(s);
				s->Release();
				sb.AppendC(UTF8STRC(" TargetMode=\"External\"/>"));
				MemFree(link);
				m++;
			}
			sb.AppendC(UTF8STRC("</Relationships>"));

			if (!dirXlWsRel)
			{
				dirXlWsRel = true;
				zip->AddDir(CSTR("xl/worksheets/_rels/"), ts, ts, ts, 0);
			}
			sptr = Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("xl/worksheets/_rels/sheet")), i + 1), UTF8STRC(".xml.rels"));
			zip->AddFile(CSTRP(sbuff, sptr), sb.ToString(), sb.GetLength(), ts, ts, ts, Data::Compress::Inflate::CompressionLevel::BestCompression, 0);
			sbContTypes.AppendC(UTF8STRC("<Override PartName=\"/"));
			sbContTypes.AppendC(sbuff, (UOSInt)(sptr - sbuff));
			sbContTypes.AppendC(UTF8STRC("\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>"));

			k = 0;
			l = sheet->GetDrawingCount();
			while (k < l)
			{
				NN<Text::SpreadSheet::OfficeChart> chart;
				NN<Text::SpreadSheet::WorksheetDrawing> drawing = sheet->GetDrawingNoCheck(k);
				sb.ClearStr();
				sb.AppendC(UTF8STRC("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"));
				sb.AppendC(UTF8STRC("<xdr:wsDr xmlns:xdr=\"http://schemas.openxmlformats.org/drawingml/2006/spreadsheetDrawing\" xmlns:a=\"http://schemas.openxmlformats.org/drawingml/2006/main\" xmlns:c=\"http://schemas.openxmlformats.org/drawingml/2006/chart\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\">"));
				switch (drawing->anchorType)
				{
				case Text::SpreadSheet::AnchorType::Absolute:
					sb.AppendC(UTF8STRC("<xdr:absoluteAnchor>"));
					sb.AppendC(UTF8STRC("<xdr:pos x=\""));
					sb.AppendOSInt(Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->posXInch)));
					sb.AppendC(UTF8STRC("\" y=\""));
					sb.AppendOSInt(Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->posYInch)));
					sb.AppendC(UTF8STRC("\"/>"));
					sb.AppendC(UTF8STRC("<xdr:ext cx=\""));
					sb.AppendOSInt(Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->widthInch)));
					sb.AppendC(UTF8STRC("\" cy=\""));
					sb.AppendOSInt(Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->heightInch)));
					sb.AppendC(UTF8STRC("\"/>"));
					break;
				case Text::SpreadSheet::AnchorType::OneCell:
					sb.AppendC(UTF8STRC("<xdr:oneCellAnchor>"));
					sb.AppendC(UTF8STRC("<xdr:from>"));
					sb.AppendC(UTF8STRC("<xdr:col>"));
					sb.AppendUOSInt(drawing->col1 + 1);
					sb.AppendC(UTF8STRC("</xdr:col>"));
					sb.AppendC(UTF8STRC("<xdr:colOff>"));
					sb.AppendOSInt(Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->posXInch)));
					sb.AppendC(UTF8STRC("</xdr:colOff>"));
					sb.AppendC(UTF8STRC("<xdr:row>"));
					sb.AppendUOSInt(drawing->row1 + 1);
					sb.AppendC(UTF8STRC("</xdr:row>"));
					sb.AppendC(UTF8STRC("<xdr:rowOff>"));
					sb.AppendOSInt(Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->posYInch)));
					sb.AppendC(UTF8STRC("</xdr:rowOff>"));
					sb.AppendC(UTF8STRC("</xdr:from>"));
					sb.AppendC(UTF8STRC("<xdr:ext cx=\""));
					sb.AppendOSInt(Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->widthInch)));
					sb.AppendC(UTF8STRC("\" cy=\""));
					sb.AppendOSInt(Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->heightInch)));
					sb.AppendC(UTF8STRC("\"/>"));
					break;
				case Text::SpreadSheet::AnchorType::TwoCell:
					sb.AppendC(UTF8STRC("<xdr:twoCellAnchor editAs=\"twoCell\">"));
					sb.AppendC(UTF8STRC("<xdr:from>"));
					sb.AppendC(UTF8STRC("<xdr:col>"));
					sb.AppendUOSInt(drawing->col1 + 1);
					sb.AppendC(UTF8STRC("</xdr:col>"));
					sb.AppendC(UTF8STRC("<xdr:colOff>"));
					sb.AppendOSInt(Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->posXInch)));
					sb.AppendC(UTF8STRC("</xdr:colOff>"));
					sb.AppendC(UTF8STRC("<xdr:row>"));
					sb.AppendUOSInt(drawing->row1 + 1);
					sb.AppendC(UTF8STRC("</xdr:row>"));
					sb.AppendC(UTF8STRC("<xdr:rowOff>"));
					sb.AppendOSInt(Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->posYInch)));
					sb.AppendC(UTF8STRC("</xdr:rowOff>"));
					sb.AppendC(UTF8STRC("</xdr:from>"));
					sb.AppendC(UTF8STRC("<xdr:to>"));
					sb.AppendC(UTF8STRC("<xdr:col>"));
					sb.AppendUOSInt(drawing->col2 + 1);
					sb.AppendC(UTF8STRC("</xdr:col>"));
					sb.AppendC(UTF8STRC("<xdr:colOff>"));
					sb.AppendOSInt(Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->widthInch)));
					sb.AppendC(UTF8STRC("</xdr:colOff>"));
					sb.AppendC(UTF8STRC("<xdr:row>"));
					sb.AppendUOSInt(drawing->row2 + 1);
					sb.AppendC(UTF8STRC("</xdr:row>"));
					sb.AppendC(UTF8STRC("<xdr:rowOff>"));
					sb.AppendOSInt(Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, drawing->heightInch)));
					sb.AppendC(UTF8STRC("</xdr:rowOff>"));
					sb.AppendC(UTF8STRC("</xdr:to>"));
					break;
				}
				if (drawing->chart.SetTo(chart))
				{
					sb.AppendC(UTF8STRC("<xdr:graphicFrame>"));
					sb.AppendC(UTF8STRC("<xdr:nvGraphicFramePr>"));
					sb.AppendC(UTF8STRC("<xdr:cNvPr id=\""));
					sb.AppendUOSInt(chartCnt);
					sb.AppendC(UTF8STRC("\" name=\"Diagramm"));
					sb.AppendUOSInt(chartCnt);
					sb.AppendC(UTF8STRC("\"/>"));
					sb.AppendC(UTF8STRC("<xdr:cNvGraphicFramePr/>"));
					sb.AppendC(UTF8STRC("</xdr:nvGraphicFramePr>"));
					sb.AppendC(UTF8STRC("<xdr:xfrm>"));
					sb.AppendC(UTF8STRC("<a:off x=\""));
					sb.AppendOSInt(Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, chart->GetXInch())));
					sb.AppendC(UTF8STRC("\" y=\""));
					sb.AppendOSInt(Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, chart->GetYInch())));
					sb.AppendC(UTF8STRC("\"/>"));
					sb.AppendC(UTF8STRC("<a:ext cx=\""));
					sb.AppendOSInt(Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, chart->GetWInch())));
					sb.AppendC(UTF8STRC("\" cy=\""));
					sb.AppendOSInt(Double2OSInt(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_EMU, chart->GetHInch())));
					sb.AppendC(UTF8STRC("\"/>"));
					sb.AppendC(UTF8STRC("</xdr:xfrm>"));
					sb.AppendC(UTF8STRC("<a:graphic>"));
					sb.AppendC(UTF8STRC("<a:graphicData uri=\"http://schemas.openxmlformats.org/drawingml/2006/chart\">"));
					sb.AppendC(UTF8STRC("<c:chart r:id=\"rId"));
					sb.AppendUOSInt(chartCnt + 1);
					sb.AppendC(UTF8STRC("\"/>"));
					sb.AppendC(UTF8STRC("</a:graphicData>"));
					sb.AppendC(UTF8STRC("</a:graphic>"));
					sb.AppendC(UTF8STRC("</xdr:graphicFrame>"));	
				}
				else
				{
					///////////////////////////////////////
				}
				sb.AppendC(UTF8STRC("<xdr:clientData/>"));
				switch (drawing->anchorType)
				{
				case Text::SpreadSheet::AnchorType::Absolute:
					sb.AppendC(UTF8STRC("</xdr:absoluteAnchor>"));
					break;
				case Text::SpreadSheet::AnchorType::OneCell:
					sb.AppendC(UTF8STRC("</xdr:oneCellAnchor>"));
					break;
				case Text::SpreadSheet::AnchorType::TwoCell:
					sb.AppendC(UTF8STRC("</xdr:twoCellAnchor>"));
					break;
				}
				sb.AppendC(UTF8STRC("</xdr:wsDr>"));
				drawingCnt++;
				if (!dirXlDraw)
				{
					dirXlDraw = true;
					zip->AddDir(CSTR("xl/drawings/"), ts, ts, ts, 0);
				}
				sptr = Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("xl/drawings/drawing")), drawingCnt), UTF8STRC(".xml"));
				zip->AddFile(CSTRP(sbuff, sptr), sb.ToString(), sb.GetLength(), ts, ts, ts, Data::Compress::Inflate::CompressionLevel::BestCompression, 0);
				sbContTypes.AppendC(UTF8STRC("<Override PartName=\"/"));
				sbContTypes.AppendC(sbuff, (UOSInt)(sptr - sbuff));
				sbContTypes.AppendC(UTF8STRC("\" ContentType=\"application/vnd.openxmlformats-officedocument.drawing+xml\"/>"));

				if (drawing->chart.SetTo(chart))
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"));
					sb.AppendC(UTF8STRC("<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"));
					sb.AppendC(UTF8STRC("<Relationship Id=\"rId1\" Target=\"../charts/chart"));
					sb.AppendUOSInt(chartCnt + 1);
					sb.AppendC(UTF8STRC(".xml\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/chart\"/>"));
					sb.AppendC(UTF8STRC("</Relationships>"));

					if (!dirXlDrawRel)
					{
						dirXlDrawRel = true;
						zip->AddDir(CSTR("xl/drawings/_rels/"), ts, ts, ts, 0);
					}
					sptr = Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("xl/drawings/_rels/drawing")), drawingCnt), UTF8STRC(".xml.rels"));
					zip->AddFile(CSTRP(sbuff, sptr), sb.ToString(), sb.GetLength(), ts, ts, ts, Data::Compress::Inflate::CompressionLevel::BestCompression, 0);
					sbContTypes.AppendC(UTF8STRC("<Override PartName=\"/"));
					sbContTypes.AppendC(sbuff, (UOSInt)(sptr - sbuff));
					sbContTypes.AppendC(UTF8STRC("\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>"));

					chartCnt++;
					sb.ClearStr();
					sb.AppendC(UTF8STRC("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"));
					sb.AppendC(UTF8STRC("<c:chartSpace xmlns:c=\"http://schemas.openxmlformats.org/drawingml/2006/chart\" xmlns:a=\"http://schemas.openxmlformats.org/drawingml/2006/main\">"));
					sb.AppendC(UTF8STRC("<c:chart>"));
					if (chart->GetTitleText().SetTo(s))
					{
						
						AppendTitle(sb, s->v);
					}
					sb.AppendC(UTF8STRC("<c:plotArea>"));
					sb.AppendC(UTF8STRC("<c:layout/>"));
					if (chart->GetChartType() != ChartType::Unknown)
					{
						NN<Text::SpreadSheet::OfficeChartAxis> nnaxis;
						switch (chart->GetChartType())
						{
						case ChartType::LineChart:
							sb.AppendC(UTF8STRC("<c:lineChart>"));
							break;
						case ChartType::Unknown:
							break;
						}
						m = 0;
						n = chart->GetSeriesCount();
						while (m < n)
						{
							AppendSeries(sb, chart->GetSeriesNoCheck(m), m);
							m++;
						}
						if (chart->GetCategoryAxis().SetTo(nnaxis))
						{
							sb.AppendC(UTF8STRC("<c:axId val=\""));
							sb.AppendUOSInt(chart->GetAxisIndex(nnaxis));
							sb.AppendC(UTF8STRC("\"/>"));
						}
						if (chart->GetValueAxis().SetTo(nnaxis))
						{
							sb.AppendC(UTF8STRC("<c:axId val=\""));
							sb.AppendUOSInt(chart->GetAxisIndex(nnaxis));
							sb.AppendC(UTF8STRC("\"/>"));
						}
						switch (chart->GetChartType())
						{
						case ChartType::LineChart:
							sb.AppendC(UTF8STRC("</c:lineChart>"));
							break;
						case ChartType::Unknown:
							break;
						}
					}
					m = 0;
					n = chart->GetAxisCount();
					while (m < n)
					{
						AppendAxis(sb, chart->GetAxis(m), m);
						m++;
					}
					AppendShapeProp(sb, chart->GetShapeProp());
					sb.AppendC(UTF8STRC("</c:plotArea>"));
					if (chart->HasLegend())
					{
						sb.AppendC(UTF8STRC("<c:legend>"));
						sb.AppendC(UTF8STRC("<c:legendPos val=\""));
						switch (chart->GetLegendPos())
						{
						case LegendPos::Bottom:
							sb.AppendC(UTF8STRC("b"));
							break;
						}
						sb.AppendC(UTF8STRC("\"/>"));
						sb.AppendC(UTF8STRC("<c:overlay val=\""));
						if (chart->IsLegendOverlay())
						{
							sb.AppendC(UTF8STRC("true"));
						}
						else
						{
							sb.AppendC(UTF8STRC("false"));
						}
						sb.AppendC(UTF8STRC("\"/>"));
						sb.AppendC(UTF8STRC("</c:legend>"));
					}
					sb.AppendC(UTF8STRC("<c:plotVisOnly val=\"true\"/>"));
					switch (chart->GetDisplayBlankAs())
					{
					case BlankAs::Default:
						break;
					case BlankAs::Gap:
						sb.AppendC(UTF8STRC("<c:dispBlanksAs val=\"gap\"/>"));
						break;
					case BlankAs::Zero:
						sb.AppendC(UTF8STRC("<c:dispBlanksAs val=\"zero\"/>"));
						break;
					}
					sb.AppendC(UTF8STRC("</c:chart>"));
					//////////////////////////////////////
					sb.AppendC(UTF8STRC("</c:chartSpace>"));

					if (!dirXlChart)
					{
						dirXlChart = true;
						zip->AddDir(CSTR("xl/charts/"), ts, ts, ts, 0);
					}
					sptr = Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("xl/charts/chart")), chartCnt), UTF8STRC(".xml"));
					zip->AddFile(CSTRP(sbuff, sptr), sb.ToString(), sb.GetLength(), ts, ts, ts, Data::Compress::Inflate::CompressionLevel::BestCompression, 0);
					sbContTypes.AppendC(UTF8STRC("<Override PartName=\"/"));
					sbContTypes.AppendC(sbuff, (UOSInt)(sptr - sbuff));
					sbContTypes.AppendC(UTF8STRC("\" ContentType=\"application/vnd.openxmlformats-officedocument.drawingml.chart+xml\"/>"));
				}
				k++;
			}
		}
		i++;
	}

	if (sharedStrings.GetCount() > 0)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"));
		sb.AppendC(UTF8STRC("<sst xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\" count=\""));
		sb.AppendUOSInt(sharedStrings.GetCount());
		sb.AppendC(UTF8STRC("\" uniqueCount=\""));
		sb.AppendUOSInt(sharedStrings.GetCount());
		sb.AppendC(UTF8STRC("\">"));
		i = 0;
		k = sharedStrings.GetCount();
		while (i < k)
		{
			sb.AppendC(UTF8STRC("<si><t xml:space=\"preserve\">"));
			s = Text::XML::ToNewXMLText(sharedStrings.GetItemNoCheck(i)->v);
			sb.Append(s);
			s->Release();
			sb.AppendC(UTF8STRC("</t></si>"));
			i++;
		}
		sb.AppendC(UTF8STRC("</sst>"));
		zip->AddFile(CSTR("xl/sharedStrings.xml"), sb.ToString(), sb.GetLength(), ts, ts, ts, Data::Compress::Inflate::CompressionLevel::BestCompression, 0);
		sbContTypes.AppendC(UTF8STRC("<Override PartName=\"/xl/sharedStrings.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.sharedStrings+xml\"/>"));
	}

	sb.ClearStr();
	sb.AppendC(UTF8STRC("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"));
	sb.AppendC(UTF8STRC("<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"));
	sb.AppendC(UTF8STRC("<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument\" Target=\"xl/workbook.xml\"/>"));
	sb.AppendC(UTF8STRC("<Relationship Id=\"rId2\" Type=\"http://schemas.openxmlformats.org/package/2006/relationships/metadata/core-properties\" Target=\"docProps/core.xml\"/>"));
	sb.AppendC(UTF8STRC("<Relationship Id=\"rId3\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/extended-properties\" Target=\"docProps/app.xml\"/>"));
	sb.AppendC(UTF8STRC("\n</Relationships>"));
	if (!dirRel)
	{
		zip->AddDir(CSTR("_rels/"), ts, ts, ts, 0);
		dirRel = true;
	}
	zip->AddFile(CSTR("_rels/.rels"), sb.ToString(), sb.GetLength(), ts, ts, ts, Data::Compress::Inflate::CompressionLevel::BestCompression, 0);
	sbContTypes.AppendC(UTF8STRC("<Override PartName=\"/_rels/.rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>"));

	sb.ClearStr();
	sb.AppendC(UTF8STRC("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"));
	sb.AppendC(UTF8STRC("<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"));
	sb.AppendC(UTF8STRC("<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/theme\" Target=\"theme/theme1.xml\"/>"));
	sb.AppendC(UTF8STRC("<Relationship Id=\"rId2\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles\" Target=\"styles.xml\"/>"));
	i = 0;
	k = workbook->GetCount();
	while (i < k)
	{
		sb.AppendC(UTF8STRC("<Relationship Id=\"rId"));
		sb.AppendUOSInt(i + 3);
		sb.AppendC(UTF8STRC("\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/worksheet\" Target=\"worksheets/sheet"));
		sb.AppendUOSInt(i + 1);
		sb.AppendC(UTF8STRC(".xml\"/>"));
		i++;
	}
	if (sharedStrings.GetCount() > 0)
	{
		sb.AppendC(UTF8STRC("<Relationship Id=\"rId"));
		sb.AppendUOSInt(workbook->GetCount() + 3);
		sb.AppendC(UTF8STRC("\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/sharedStrings\" Target=\"sharedStrings.xml\"/>"));
	}
	sb.AppendC(UTF8STRC("\n</Relationships>"));
	if (!dirXlRel)
	{
		dirXlRel = true;
		zip->AddDir(CSTR("xl/_rels/"), ts, ts, ts, 0);
	}
	zip->AddFile(CSTR("xl/_rels/workbook.xml.rels"), sb.ToString(), sb.GetLength(), ts, ts, ts, Data::Compress::Inflate::CompressionLevel::BestCompression, 0);

	sb.ClearStr();
	sb.AppendC(UTF8STRC("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"));
	sb.AppendC(UTF8STRC("<cp:coreProperties xmlns:cp=\"http://schemas.openxmlformats.org/package/2006/metadata/core-properties\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:dcterms=\"http://purl.org/dc/terms/\" xmlns:dcmitype=\"http://purl.org/dc/dcmitype/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"));
	t = workbook->GetCreateTime();
	if (t)
	{
		sb.AppendC(UTF8STRC("<dcterms:created xsi:type=\"dcterms:W3CDTF\">"));
		sptr = t->ToString(sbuff, "yyyy-MM-dd");
		sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
		sb.AppendUTF8Char('T');
		sptr = t->ToString(sbuff, "HH:mm:ss");
		sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
		sb.AppendUTF8Char('Z');
		sb.AppendC(UTF8STRC("</dcterms:created>"));
	}
	sb.AppendC(UTF8STRC("<dc:creator>"));
	if (workbook->GetAuthor().SetTo(nncsptr))
	{
		s = Text::XML::ToNewXMLText(nncsptr);
		sb.Append(s);
		s->Release();
	}
	sb.AppendC(UTF8STRC("</dc:creator>"));
	sb.AppendC(UTF8STRC("<dc:description>"));
	csptr = 0;
	if (csptr.SetTo(nncsptr))
	{
		s = Text::XML::ToNewXMLText(nncsptr);
		sb.Append(s);
		s->Release();
	}
	sb.AppendC(UTF8STRC("</dc:description>"));
	sb.AppendC(UTF8STRC("<dc:language>"));
	UInt32 lcid = Text::EncodingFactory::GetSystemLCID();
	NN<Text::Locale::LocaleEntry> loc;
	if (Text::Locale::GetLocaleEntry(lcid).SetTo(loc))
	{
		s = Text::XML::ToNewXMLText(loc->shortName);
		sb.Append(s);
		s->Release();
	}
	sb.AppendC(UTF8STRC("</dc:language>"));
	sb.AppendC(UTF8STRC("<cp:lastModifiedBy>"));
	if (workbook->GetLastAuthor().SetTo(nncsptr))
	{
		s = Text::XML::ToNewXMLText(nncsptr);
		sb.Append(s);
		s->Release();
	}
	sb.AppendC(UTF8STRC("</cp:lastModifiedBy>"));
	t = workbook->GetModifyTime();
	if (t)
	{
		sb.AppendC(UTF8STRC("<dcterms:modified xsi:type=\"dcterms:W3CDTF\">"));
		sptr = t->ToString(sbuff, "yyyy-MM-dd");
		sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
		sb.AppendUTF8Char('T');
		sptr = t->ToString(sbuff, "HH:mm:ss");
		sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
		sb.AppendUTF8Char('Z');
		sb.AppendC(UTF8STRC("</dcterms:modified>"));
	}
	sb.AppendC(UTF8STRC("<cp:revision>"));
	sb.AppendU32(1);
	sb.AppendC(UTF8STRC("</cp:revision>"));
	sb.AppendC(UTF8STRC("<dc:subject>"));
	csptr = 0;
	if (csptr.SetTo(nncsptr))
	{
		s = Text::XML::ToNewXMLText(nncsptr);
		sb.Append(s);
		s->Release();
	}
	sb.AppendC(UTF8STRC("</dc:subject>"));
	sb.AppendC(UTF8STRC("<dc:title>"));
	csptr = 0;
	if (csptr.SetTo(nncsptr))
	{
		s = Text::XML::ToNewXMLText(nncsptr);
		sb.Append(s);
		s->Release();
	}
	sb.AppendC(UTF8STRC("</dc:title>"));
	sb.AppendC(UTF8STRC("</cp:coreProperties>"));
	zip->AddDir(CSTR("docProps/"), ts, ts, ts, 0);
	zip->AddFile(CSTR("docProps/core.xml"), sb.ToString(), sb.GetLength(), ts, ts, ts, Data::Compress::Inflate::CompressionLevel::BestCompression, 0);
	sbContTypes.AppendC(UTF8STRC("<Override PartName=\"/docProps/core.xml\" ContentType=\"application/vnd.openxmlformats-package.core-properties+xml\"/>"));

	sb.ClearStr();
	sb.AppendC(UTF8STRC("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"));
	sb.AppendC(UTF8STRC("<Properties xmlns=\"http://schemas.openxmlformats.org/officeDocument/2006/extended-properties\" xmlns:vt=\"http://schemas.openxmlformats.org/officeDocument/2006/docPropsVTypes\">"));
	sb.AppendC(UTF8STRC("<Template></Template>"));
	sb.AppendC(UTF8STRC("<TotalTime>1</TotalTime>"));
	sb.AppendC(UTF8STRC("<Application>"));
	sb.AppendC(UTF8STRC("AVIRead/"));
	IO::BuildTime::GetBuildTime(dt2);
	sptr = dt2.ToString(sbuff, "yyyyMMdd_HHmmss");
	sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sb.AppendC(UTF8STRC("</Application>"));
	sb.AppendC(UTF8STRC("<AppVersion>15.0000</AppVersion>"));
	sb.AppendC(UTF8STRC("</Properties>"));
	zip->AddFile(CSTR("docProps/app.xml"), sb.ToString(), sb.GetLength(), ts, ts, ts, Data::Compress::Inflate::CompressionLevel::BestCompression, 0);
	sbContTypes.AppendC(UTF8STRC("<Override PartName=\"/docProps/app.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.extended-properties+xml\"/>"));

	sbContTypes.AppendC(UTF8STRC("\n</Types>"));
	zip->AddFile(CSTR("[Content_Types].xml"), sbContTypes.ToString(), sbContTypes.GetLength(), ts, ts, ts, Data::Compress::Inflate::CompressionLevel::BestCompression, 0);

	DEL_CLASS(zip);
	return true;
}

void Exporter::XLSXExporter::AppendFill(NN<Text::StringBuilderUTF8> sb, Optional<OfficeFill> fill)
{
	NN<OfficeFill> nnfill;
	NN<OfficeColor> color;
	if (!fill.SetTo(nnfill))
		return;
	switch (nnfill->GetFillType())
	{
	case FillType::SolidFill:
		if (!nnfill->GetColor().SetTo(color))
		{
			sb->AppendC(UTF8STRC("<a:solidFill/>"));
		}
		else
		{
			sb->AppendC(UTF8STRC("<a:solidFill>"));
			if (color->GetColorType() == ColorType::Preset)
			{
				sb->AppendC(UTF8STRC("<a:prstClr val=\""));
				Text::CStringNN col = PresetColorCode(color->GetPresetColor());
				sb->AppendC(col.v, col.leng);
				sb->AppendC(UTF8STRC("\"/>"));
			}
			sb->AppendC(UTF8STRC("</a:solidFill>"));
		}
		break;
	}
}

void Exporter::XLSXExporter::AppendLineStyle(NN<Text::StringBuilderUTF8> sb, Optional<Text::SpreadSheet::OfficeLineStyle> lineStyle)
{
	NN<Text::SpreadSheet::OfficeLineStyle> nnlineStyle;
	if (!lineStyle.SetTo(nnlineStyle))
		return;
	sb->AppendC(UTF8STRC("<a:ln>"));
	AppendFill(sb, nnlineStyle->GetFillStyle());
	sb->AppendC(UTF8STRC("</a:ln>"));
}

void Exporter::XLSXExporter::AppendTitle(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UTF8Char> title)
{
	sb->AppendC(UTF8STRC("<c:title>"));
	sb->AppendC(UTF8STRC("<c:tx>"));
	sb->AppendC(UTF8STRC("<c:rich>"));
	sb->AppendC(UTF8STRC("<a:bodyPr anchor=\"t\" rtlCol=\"false\"/>"));
	sb->AppendC(UTF8STRC("<a:lstStyle/>"));
	sb->AppendC(UTF8STRC("<a:p>"));
	sb->AppendC(UTF8STRC("<a:pPr algn=\"l\">"));
	sb->AppendC(UTF8STRC("<a:defRPr/>"));
	sb->AppendC(UTF8STRC("</a:pPr>"));
	sb->AppendC(UTF8STRC("<a:r>"));
	sb->AppendC(UTF8STRC("<a:rPr lang=\"en-HK\"/>"));
	sb->AppendC(UTF8STRC("<a:t>"));
	NN<Text::String> s = Text::XML::ToNewXMLText(title);
	sb->Append(s);
	s->Release();
	sb->AppendC(UTF8STRC("</a:t>"));
	sb->AppendC(UTF8STRC("</a:r>"));
	sb->AppendC(UTF8STRC("<a:endParaRPr lang=\"en-US\" sz=\"1100\"/>"));
	sb->AppendC(UTF8STRC("</a:p>"));
	sb->AppendC(UTF8STRC("</c:rich>"));
	sb->AppendC(UTF8STRC("</c:tx>"));
	sb->AppendC(UTF8STRC("<c:layout/>"));
	sb->AppendC(UTF8STRC("</c:title>"));
}

void Exporter::XLSXExporter::AppendShapeProp(NN<Text::StringBuilderUTF8> sb, Optional<Text::SpreadSheet::OfficeShapeProp> shapeProp)
{
	NN<Text::SpreadSheet::OfficeShapeProp> nnshapeProp;
	if (!shapeProp.SetTo(nnshapeProp))
		return;
	sb->AppendC(UTF8STRC("<c:spPr>"));
	AppendFill(sb, nnshapeProp->GetFill());
	AppendLineStyle(sb, nnshapeProp->GetLineStyle());
	sb->AppendC(UTF8STRC("</c:spPr>"));
}

void Exporter::XLSXExporter::AppendAxis(NN<Text::StringBuilderUTF8> sb, Optional<Text::SpreadSheet::OfficeChartAxis> axis, UOSInt index)
{
	NN<Text::SpreadSheet::OfficeShapeProp> shapeProp;
	NN<Text::SpreadSheet::OfficeChartAxis> nnaxis;
	if (!axis.SetTo(nnaxis))
		return;
	NN<Text::String> s;	
	switch (nnaxis->GetAxisType())
	{
	case AxisType::Category:
		sb->AppendC(UTF8STRC("<c:catAx>"));
		break;
	case AxisType::Date:
		sb->AppendC(UTF8STRC("<c:dateAx>"));
		break;
	case AxisType::Numeric:
		sb->AppendC(UTF8STRC("<c:valAx>"));
		break;
	case AxisType::Series:
		sb->AppendC(UTF8STRC("<c:serAx>"));
		break;
	}
	sb->AppendC(UTF8STRC("<c:axId val=\""));
	sb->AppendUOSInt(index);
	sb->AppendC(UTF8STRC("\"/>"));
	sb->AppendC(UTF8STRC("<c:scaling>"));
	sb->AppendC(UTF8STRC("<c:orientation val=\"minMax\"/>"));
	sb->AppendC(UTF8STRC("</c:scaling>"));
	sb->AppendC(UTF8STRC("<c:delete val=\"false\"/>"));
	switch (nnaxis->GetAxisPos())
	{
	case AxisPosition::Left:
		sb->AppendC(UTF8STRC("<c:axPos val=\"l\"/>"));
		break;
	case AxisPosition::Top:
		sb->AppendC(UTF8STRC("<c:axPos val=\"t\"/>"));
		break;
	case AxisPosition::Right:
		sb->AppendC(UTF8STRC("<c:axPos val=\"r\"/>"));
		break;
	case AxisPosition::Bottom:
		sb->AppendC(UTF8STRC("<c:axPos val=\"b\"/>"));
		break;
	}
	if (nnaxis->GetMajorGridProp().SetTo(shapeProp))
	{
		sb->AppendC(UTF8STRC("<c:majorGridlines>"));
		AppendShapeProp(sb, shapeProp);
		sb->AppendC(UTF8STRC("</c:majorGridlines>"));
	}
	if (nnaxis->GetTitle().SetTo(s))
	{
		AppendTitle(sb, s->v);
	}
	sb->AppendC(UTF8STRC("<c:majorTickMark val=\"cross\"/>"));
	sb->AppendC(UTF8STRC("<c:minorTickMark val=\"none\"/>"));
	switch (nnaxis->GetTickLblPos())
	{
	case TickLabelPosition::High:
		sb->AppendC(UTF8STRC("<c:tickLblPos val=\"high\"/>"));
		break;
	case TickLabelPosition::Low:
		sb->AppendC(UTF8STRC("<c:tickLblPos val=\"low\"/>"));
		break;
	case TickLabelPosition::NextTo:
		sb->AppendC(UTF8STRC("<c:tickLblPos val=\"nextTo\"/>"));
		break;
	case TickLabelPosition::None:
		sb->AppendC(UTF8STRC("<c:tickLblPos val=\"none\"/>"));
		break;
	}
	AppendShapeProp(sb, nnaxis->GetShapeProp());
// 	sb->AppendC(UTF8STRC("<c:crossAx val=\"1\"/>");
	sb->AppendC(UTF8STRC("<c:crosses val=\"autoZero\"/>"));
	sb->AppendC(UTF8STRC("<c:crossBetween val=\"midCat\"/>"));
	switch (nnaxis->GetAxisType())
	{
	case AxisType::Category:
		sb->AppendC(UTF8STRC("</c:catAx>"));
		break;
	case AxisType::Date:
		sb->AppendC(UTF8STRC("</c:dateAx>"));
		break;
	case AxisType::Numeric:
		sb->AppendC(UTF8STRC("</c:valAx>"));
		break;
	case AxisType::Series:
		sb->AppendC(UTF8STRC("</c:serAx>"));
		break;
	}
}

void Exporter::XLSXExporter::AppendSeries(NN<Text::StringBuilderUTF8> sb, NN<Text::SpreadSheet::OfficeChartSeries> series, UOSInt index)
{
	NN<Text::String> s;
	sb->AppendC(UTF8STRC("<c:ser>"));
	sb->AppendC(UTF8STRC("<c:idx val=\""));
	sb->AppendUOSInt(index);
	sb->AppendC(UTF8STRC("\"/>"));
	sb->AppendC(UTF8STRC("<c:order val=\""));
	sb->AppendUOSInt(index);
	sb->AppendC(UTF8STRC("\"/>"));
	if (series->GetTitle().SetTo(s))
	{
		sb->AppendC(UTF8STRC("<c:tx>"));
		sb->AppendC(UTF8STRC("<c:v>"));
		s = Text::XML::ToNewXMLText(s->v);
		sb->Append(s);
		s->Release();
		sb->AppendC(UTF8STRC("</c:v>"));
		sb->AppendC(UTF8STRC("</c:tx>"));
	}
	AppendShapeProp(sb, series->GetShapeProp());
	sb->AppendC(UTF8STRC("<c:marker>"));
	switch (series->GetMarkerStyle())
	{
	case MarkerStyle::Circle:
		sb->AppendC(UTF8STRC("<c:symbol val=\"circle\"/>"));
		break;
	case MarkerStyle::Dash:
		sb->AppendC(UTF8STRC("<c:symbol val=\"dash\"/>"));
		break;
	case MarkerStyle::Diamond:
		sb->AppendC(UTF8STRC("<c:symbol val=\"diamond\"/>"));
		break;
	case MarkerStyle::Dot:
		sb->AppendC(UTF8STRC("<c:symbol val=\"dot\"/>"));
		break;
	case MarkerStyle::None:
		sb->AppendC(UTF8STRC("<c:symbol val=\"none\"/>"));
		break;
	case MarkerStyle::Picture:
		sb->AppendC(UTF8STRC("<c:symbol val=\"picture\"/>"));
		break;
	case MarkerStyle::Plus:
		sb->AppendC(UTF8STRC("<c:symbol val=\"plus\"/>"));
		break;
	case MarkerStyle::Square:
		sb->AppendC(UTF8STRC("<c:symbol val=\"square\"/>"));
		break;
	case MarkerStyle::Star:
		sb->AppendC(UTF8STRC("<c:symbol val=\"star\"/>"));
		break;
	case MarkerStyle::Triangle:
		sb->AppendC(UTF8STRC("<c:symbol val=\"triangle\"/>"));
		break;
	case MarkerStyle::X:
		sb->AppendC(UTF8STRC("<c:symbol val=\"x\"/>"));
		break;
	}
	if (series->GetMarkerSize() != 0)
	{
		sb->AppendC(UTF8STRC("<c:size val=\""));
		sb->AppendU32(series->GetMarkerSize());
		sb->AppendC(UTF8STRC("\"/>"));
	}
	sb->AppendC(UTF8STRC("</c:marker>"));

	UTF8Char sbuff[128];
	NN<WorkbookDataSource> catData = series->GetCategoryData();
	sb->AppendC(UTF8STRC("<c:cat>"));
	sb->AppendC(UTF8STRC("<c:strRef>"));
	sb->AppendC(UTF8STRC("<c:f>"));
	catData->ToCodeRange(sbuff);
	s = Text::XML::ToNewXMLText(sbuff);
	sb->Append(s);
	s->Release();
	sb->AppendC(UTF8STRC("</c:f>"));
	sb->AppendC(UTF8STRC("<c:strCache/>"));
	sb->AppendC(UTF8STRC("</c:strRef>"));
	sb->AppendC(UTF8STRC("</c:cat>"));

	NN<WorkbookDataSource> valData = series->GetValueData();
	sb->AppendC(UTF8STRC("<c:val>"));
	sb->AppendC(UTF8STRC("<c:numRef>"));
	sb->AppendC(UTF8STRC("<c:f>"));
	valData->ToCodeRange(sbuff);
	s = Text::XML::ToNewXMLText(sbuff);
	sb->Append(s);
	s->Release();
	sb->AppendC(UTF8STRC("</c:f>"));
	UOSInt firstRow = valData->GetFirstRow();
	UOSInt lastRow = valData->GetLastRow();
	UOSInt firstCol = valData->GetFirstCol();
	UOSInt lastCol = valData->GetLastCol();
	if (firstRow == lastRow)
	{
		NN<Worksheet> sheet = valData->GetSheet();
		sb->AppendC(UTF8STRC("<c:numCache>"));
		sb->AppendC(UTF8STRC("<c:ptCount val=\""));
		sb->AppendUOSInt(lastCol - firstCol + 1);
		sb->AppendC(UTF8STRC("\"/>"));
		NN<Worksheet::RowData> row;
		NN<Worksheet::CellData> cell;
		NN<Text::String> cellValue;
		UOSInt i;
		if (sheet->GetItem(firstRow).SetTo(row))
		{
			i = firstCol;
			while (i <= lastCol)
			{
				if (row->cells.GetItem(i).SetTo(cell) && cell->cellValue.SetTo(cellValue) && (cell->cdt == CellDataType::DateTime || cell->cdt == CellDataType::Number))
				{
					sb->AppendC(UTF8STRC("<c:pt idx=\""));
					sb->AppendUOSInt(i - firstCol);
					sb->AppendC(UTF8STRC("\"><c:v>"));
					sb->Append(cellValue);
					sb->AppendC(UTF8STRC("</c:v></c:pt>"));
				}
				i++;
			}
		}
		sb->AppendC(UTF8STRC("</c:numCache>"));
	}
	else if (firstCol == lastCol)
	{
		NN<Worksheet> sheet = valData->GetSheet();
		sb->AppendC(UTF8STRC("<c:numCache>"));
		sb->AppendC(UTF8STRC("<c:ptCount val=\""));
		sb->AppendUOSInt(lastRow - firstRow + 1);
		sb->AppendC(UTF8STRC("\"/>"));
		NN<Worksheet::RowData> row;
		NN<Worksheet::CellData> cell;
		NN<Text::String> cellValue;
		UOSInt i;
		i = firstRow;
		while (i <= lastRow)
		{
			if (sheet->GetItem(i).SetTo(row))
			{
				if (row->cells.GetItem(firstCol).SetTo(cell) && cell->cellValue.SetTo(cellValue) && (cell->cdt == CellDataType::DateTime || cell->cdt == CellDataType::Number))
				{
					sb->AppendC(UTF8STRC("<c:pt idx=\""));
					sb->AppendUOSInt(i - firstRow);
					sb->AppendC(UTF8STRC("\"><c:v>"));
					sb->Append(cellValue);
					sb->AppendC(UTF8STRC("</c:v></c:pt>"));
				}
				i++;
			}
			i++;	
		}
		sb->AppendC(UTF8STRC("</c:numCache>"));
	}
	else
	{
		sb->AppendC(UTF8STRC("<c:numCache/>"));
	}
	sb->AppendC(UTF8STRC("</c:numRef>"));
	sb->AppendC(UTF8STRC("</c:val>"));

	sb->AppendC(UTF8STRC("<c:smooth val=\""));
	if (series->IsSmooth())
		sb->AppendC(UTF8STRC("true"));
	else
		sb->AppendC(UTF8STRC("false"));
	sb->AppendC(UTF8STRC("\"/>"));
	sb->AppendC(UTF8STRC("</c:ser>"));
}

void Exporter::XLSXExporter::AppendBorder(NN<Text::StringBuilderUTF8> sb, Text::SpreadSheet::CellStyle::BorderStyle border, Text::CStringNN name)
{
	sb->AppendUTF8Char('<');
	sb->Append(name);
	if (border.borderType == BorderType::None)
	{
		sb->AppendC(UTF8STRC("/>"));
	}
	else
	{
		switch (border.borderType)
		{
			case BorderType::Thin:
				sb->AppendC(UTF8STRC(" style=\"thin\">"));
				break;
			case BorderType::Medium:
				sb->AppendC(UTF8STRC(" style=\"medium\">"));
				break;
			case BorderType::Dashed:
				sb->AppendC(UTF8STRC(" style=\"dashed\">"));
				break;
			case BorderType::Dotted:
				sb->AppendC(UTF8STRC(" style=\"dotted\">"));
				break;
			case BorderType::Thick:
				sb->AppendC(UTF8STRC(" style=\"thick\">"));
				break;
			case BorderType::DOUBLE:
				sb->AppendC(UTF8STRC(" style=\"double\">"));
				break;
			case BorderType::Hair:
				sb->AppendC(UTF8STRC(" style=\"hair\">"));
				break;
			case BorderType::MediumDashed:
				sb->AppendC(UTF8STRC(" style=\"mediumDashed\">"));
				break;
			case BorderType::DashDot:
				sb->AppendC(UTF8STRC(" style=\"dashDot\">"));
				break;
			case BorderType::MediumDashDot:
				sb->AppendC(UTF8STRC(" style=\"mediumDashDot\">"));
				break;
			case BorderType::DashDotDot:
				sb->AppendC(UTF8STRC(" style=\"dashDotDot\">"));
				break;
			case BorderType::MediumDashDotDot:
				sb->AppendC(UTF8STRC(" style=\"mediumDashDotDot\">"));
				break;
			case BorderType::SlantedDashDot:
				sb->AppendC(UTF8STRC(" style=\"slantDashDot\">"));
				break;
			case BorderType::None:
				break;
		}
		sb->AppendC(UTF8STRC("<color rgb=\""));
		sb->AppendHex32(border.borderColor);
		sb->AppendC(UTF8STRC("\"/>"));
		sb->AppendC(UTF8STRC("</"));
		sb->Append(name);
		sb->AppendUTF8Char('>');
	}
}

void Exporter::XLSXExporter::AppendXF(NN<Text::StringBuilderUTF8> sb, NN<Text::SpreadSheet::CellStyle> style, NN<Data::ArrayList<BorderInfo*>> borders, NN<Text::SpreadSheet::Workbook> workbook, NN<Data::StringUTF8Map<UOSInt>> numFmtMap)
{
	UOSInt k;
	UnsafeArray<const UTF8Char> csptr;
	BorderInfo *border;
	Optional<Text::SpreadSheet::WorkbookFont> font = style->GetFont();
	NN<Text::SpreadSheet::WorkbookFont> nnfont;
	NN<Text::String> optS;
	if (!style->GetDataFormat().SetTo(optS))
	{
		csptr = (const UTF8Char*)"General";
	}
	else
	{
		csptr = optS->v;
	}
	sb->AppendC(UTF8STRC("<xf numFmtId=\""));
	sb->AppendUOSInt(numFmtMap->Get(csptr) + 164);
	sb->AppendC(UTF8STRC("\" fontId=\""));
	if (!font.SetTo(nnfont))
	{
		sb->AppendUTF8Char('0');
	}
	else
	{
		sb->AppendUOSInt(workbook->GetFontIndex(nnfont));
	}
	sb->AppendC(UTF8STRC("\" fillId=\"0\" borderId=\""));
	k = borders->GetCount();
	while (k-- > 0)
	{
		border = borders->GetItem(k);
		if (border->left == style->GetBorderLeft() &&
			border->top == style->GetBorderTop() &&
			border->right == style->GetBorderRight() &&
			border->bottom == style->GetBorderBottom())
		{
			break;
		}
	}
	if (k == INVALID_INDEX)
	{
		k = 0;
	}
	sb->AppendUOSInt(k);
	sb->AppendC(UTF8STRC("\" xfId=\"0\" applyFont=\""));
	if (font.NotNull())
	{
		sb->AppendC(UTF8STRC("true"));
	}
	else
	{
		sb->AppendC(UTF8STRC("false"));
	}
	sb->AppendC(UTF8STRC("\" applyBorder=\"false\" applyAlignment=\"true\" applyProtection=\"false\">"));
	sb->AppendC(UTF8STRC("<alignment horizontal=\""));
	switch (style->GetHAlign())
	{
	case Text::HAlignment::Left:
		sb->AppendC(UTF8STRC("left"));
		break;
	case Text::HAlignment::Center:
		sb->AppendC(UTF8STRC("center"));
		break;
	case Text::HAlignment::Right:
		sb->AppendC(UTF8STRC("right"));
		break;
	case Text::HAlignment::Fill:
		sb->AppendC(UTF8STRC("fill"));
		break;
	case Text::HAlignment::Justify:
		sb->AppendC(UTF8STRC("justify"));
		break;
	case Text::HAlignment::Unknown:
	default:
		sb->AppendC(UTF8STRC("general"));
		break;
	}
	sb->AppendC(UTF8STRC("\" vertical=\""));
	switch (style->GetVAlign())
	{
	case Text::VAlignment::Top:
		sb->AppendC(UTF8STRC("top"));
		break;
	case Text::VAlignment::Center:
		sb->AppendC(UTF8STRC("center"));
		break;
	case Text::VAlignment::Bottom:
		sb->AppendC(UTF8STRC("bottom"));
		break;
	case Text::VAlignment::Justify:
		sb->AppendC(UTF8STRC("justify"));
		break;
	case Text::VAlignment::Unknown:
	default:
		sb->AppendC(UTF8STRC("general"));
		break;
	}
	sb->AppendC(UTF8STRC("\" textRotation=\"0\" wrapText=\""));
	if (style->GetWordWrap())
		sb->AppendC(UTF8STRC("true"));
	else
		sb->AppendC(UTF8STRC("false"));
	sb->AppendC(UTF8STRC("\" indent=\"0\" shrinkToFit=\"false\"/>"));
	sb->AppendC(UTF8STRC("<protection locked=\"true\" hidden=\"false\"/>"));
	sb->AppendC(UTF8STRC("</xf>"));
}

Text::CStringNN Exporter::XLSXExporter::PresetColorCode(PresetColor color)
{
	switch (color)
	{
	case PresetColor::AliceBlue:
		return CSTR("aliceBlue");
	case PresetColor::AntiqueWhite:
		return CSTR("antiqueWhite");	
	case PresetColor::Aqua:
		return CSTR("aqua");	
	case PresetColor::Aquamarine:
		return CSTR("aquamarine");	
	case PresetColor::Azure:
		return CSTR("azure");	
	case PresetColor::Beige:
		return CSTR("beige");	
	case PresetColor::Bisque:
		return CSTR("bisque");	
	case PresetColor::Black:
		return CSTR("black");	
	case PresetColor::BlanchedAlmond:
		return CSTR("blanchedAlmond");	
	case PresetColor::Blue:
		return CSTR("blue");	
	case PresetColor::BlueViolet:
		return CSTR("blueViolet");	
	case PresetColor::Brown:
		return CSTR("brown");	
	case PresetColor::BurlyWood:
		return CSTR("burlyWood");	
	case PresetColor::CadetBlue:
		return CSTR("cadetBlue");	
	case PresetColor::Chartreuse:
		return CSTR("chartreuse");	
	case PresetColor::Chocolate:
		return CSTR("chocolate");	
	case PresetColor::Coral:
		return CSTR("coral");	
	case PresetColor::CornflowerBlue:
		return CSTR("cornflowerBlue");	
	case PresetColor::Cornsilk:
		return CSTR("cornsilk");	
	case PresetColor::Crimson:
		return CSTR("crimson");	
	case PresetColor::Cyan:
		return CSTR("cyan");	
	case PresetColor::DeepPink:
		return CSTR("deepPink");	
	case PresetColor::DeepSkyBlue:
		return CSTR("deepSkyBlue");	
	case PresetColor::DimGray:
		return CSTR("dimGray");	
	case PresetColor::DarkBlue:
		return CSTR("dkBlue");	
	case PresetColor::DarkCyan:
		return CSTR("dkCyan");	
	case PresetColor::DarkGoldenrod:
		return CSTR("dkGoldenrod");	
	case PresetColor::DarkGray:
		return CSTR("dkGray");	
	case PresetColor::DarkGreen:
		return CSTR("dkGreen");	
	case PresetColor::DarkKhaki:
		return CSTR("dkKhaki");	
	case PresetColor::DarkMagenta:
		return CSTR("dkMagenta");	
	case PresetColor::DarkOliveGreen:
		return CSTR("dkOliveGreen");	
	case PresetColor::DarkOrange:
		return CSTR("dkOrange");	
	case PresetColor::DarkOrchid:
		return CSTR("dkOrchid");	
	case PresetColor::DarkRed:
		return CSTR("dkRed");	
	case PresetColor::DarkSalmon:
		return CSTR("dkSalmon");	
	case PresetColor::DarkSeaGreen:
		return CSTR("dkSeaGreen");	
	case PresetColor::DarkSlateBlue:
		return CSTR("dkSlateBlue");	
	case PresetColor::DarkSlateGray:
		return CSTR("dkSlateGray");	
	case PresetColor::DarkTurquoise:
		return CSTR("dkTurquoise");	
	case PresetColor::DarkViolet:
		return CSTR("dkViolet");	
	case PresetColor::DodgerBlue:
		return CSTR("dodgerBlue");	
	case PresetColor::Firebrick:
		return CSTR("firebrick");	
	case PresetColor::FloralWhite:
		return CSTR("floralWhite");	
	case PresetColor::ForestGreen:
		return CSTR("forestGreen");	
	case PresetColor::Fuchsia:
		return CSTR("fuchsia");	
	case PresetColor::Gainsboro:
		return CSTR("gainsboro");	
	case PresetColor::GhostWhite:
		return CSTR("ghostWhite");	
	case PresetColor::Gold:
		return CSTR("gold");	
	case PresetColor::Goldenrod:
		return CSTR("goldenrod");	
	case PresetColor::Gray:
		return CSTR("gray");	
	case PresetColor::Green:
		return CSTR("green");	
	case PresetColor::GreenYellow:
		return CSTR("greenYellow");	
	case PresetColor::Honeydew:
		return CSTR("honeydew");	
	case PresetColor::HotPink:
		return CSTR("hotPink");	
	case PresetColor::IndianRed:
		return CSTR("indianRed");	
	case PresetColor::Indigo:
		return CSTR("indigo");	
	case PresetColor::Ivory:
		return CSTR("ivory");	
	case PresetColor::Khaki:
		return CSTR("khaki");	
	case PresetColor::Lavender:
		return CSTR("lavender");	
	case PresetColor::LavenderBlush:
		return CSTR("lavenderBlush");	
	case PresetColor::LawnGreen:
		return CSTR("lawnGreen");	
	case PresetColor::LemonChiffon:
		return CSTR("lemonChiffon");	
	case PresetColor::Lime:
		return CSTR("lime");	
	case PresetColor::LimeGreen:
		return CSTR("limeGreen");	
	case PresetColor::Linen:
		return CSTR("linen");	
	case PresetColor::LightBlue:
		return CSTR("ltBlue");	
	case PresetColor::LightCoral:
		return CSTR("ltCoral");	
	case PresetColor::LightCyan:
		return CSTR("ltCyan");	
	case PresetColor::LightGoldenrodYellow:
		return CSTR("ltGoldenrodYellow");	
	case PresetColor::LightGray:
		return CSTR("ltGray");	
	case PresetColor::LightGreen:
		return CSTR("ltGreen");	
	case PresetColor::LightPink:
		return CSTR("ltPink");	
	case PresetColor::LightSalmon:
		return CSTR("ltSalmon");	
	case PresetColor::LightSeaGreen:
		return CSTR("ltSeaGreen");	
	case PresetColor::LightSkyBlue:
		return CSTR("ltSkyBlue");	
	case PresetColor::LightSlateGray:
		return CSTR("ltSlateGray");	
	case PresetColor::LightSteelBlue:
		return CSTR("ltSteelBlue");	
	case PresetColor::LightYellow:
		return CSTR("ltYellow");	
	case PresetColor::Magenta:
		return CSTR("magenta");	
	case PresetColor::Maroon:
		return CSTR("maroon");	
	case PresetColor::MediumAquamarine:
		return CSTR("medAquamarine");	
	case PresetColor::MediumBlue:
		return CSTR("medBlue");	
	case PresetColor::MediumOrchid:
		return CSTR("medOrchid");	
	case PresetColor::MediumPurple:
		return CSTR("medPurple");	
	case PresetColor::MediumSeaGreen:
		return CSTR("medSeaGreen");	
	case PresetColor::MediumSlateBlue:
		return CSTR("medSlateBlue");	
	case PresetColor::MediumSpringGreen:
		return CSTR("medSpringGreen");	
	case PresetColor::MediumTurquoise:
		return CSTR("medTurquoise");	
	case PresetColor::MediumVioletRed:
		return CSTR("medVioletRed");	
	case PresetColor::MidnightBlue:
		return CSTR("midnightBlue");	
	case PresetColor::MintCream:
		return CSTR("mintCream");	
	case PresetColor::MistyRose:
		return CSTR("mistyRose");	
	case PresetColor::Moccasin:
		return CSTR("moccasin");	
	case PresetColor::NavajoWhite:
		return CSTR("navajoWhite");	
	case PresetColor::Navy:
		return CSTR("navy");	
	case PresetColor::OldLace:
		return CSTR("oldLace");	
	case PresetColor::Olive:
		return CSTR("olive");	
	case PresetColor::OliveDrab:
		return CSTR("oliveDrab");	
	case PresetColor::Orange:
		return CSTR("orange");	
	case PresetColor::OrangeRed:
		return CSTR("orangeRed");	
	case PresetColor::Orchid:
		return CSTR("orchid");	
	case PresetColor::PaleGoldenrod:
		return CSTR("paleGoldenrod");	
	case PresetColor::PaleGreen:
		return CSTR("paleGreen");	
	case PresetColor::PaleTurquoise:
		return CSTR("paleTurquoise");	
	case PresetColor::PaleVioletRed:
		return CSTR("paleVioletRed");	
	case PresetColor::PapayaWhip:
		return CSTR("papayaWhip");	
	case PresetColor::PeachPuff:
		return CSTR("peachPuff");	
	case PresetColor::Peru:
		return CSTR("peru");	
	case PresetColor::Pink:
		return CSTR("pink");	
	case PresetColor::Plum:
		return CSTR("plum");	
	case PresetColor::PowderBlue:
		return CSTR("powderBlue");	
	case PresetColor::Purple:
		return CSTR("purple");	
	case PresetColor::Red:
		return CSTR("red");	
	case PresetColor::RosyBrown:
		return CSTR("rosyBrown");	
	case PresetColor::RoyalBlue:
		return CSTR("royalBlue");	
	case PresetColor::SaddleBrown:
		return CSTR("saddleBrown");	
	case PresetColor::Salmon:
		return CSTR("salmon");	
	case PresetColor::SandyBrown:
		return CSTR("sandyBrown");	
	case PresetColor::SeaGreen:
		return CSTR("seaGreen");	
	case PresetColor::SeaShell:
		return CSTR("seaShell");	
	case PresetColor::Sienna:
		return CSTR("sienna");	
	case PresetColor::Silver:
		return CSTR("silver");	
	case PresetColor::SkyBlue:
		return CSTR("skyBlue");	
	case PresetColor::SlateBlue:
		return CSTR("slateBlue");	
	case PresetColor::SlateGray:
		return CSTR("slateGray");	
	case PresetColor::Snow:
		return CSTR("snow");	
	case PresetColor::SpringGreen:
		return CSTR("springGreen");	
	case PresetColor::SteelBlue:
		return CSTR("steelBlue");	
	case PresetColor::Tan:
		return CSTR("tan");	
	case PresetColor::Teal:
		return CSTR("teal");	
	case PresetColor::Thistle:
		return CSTR("thistle");	
	case PresetColor::Tomato:
		return CSTR("tomato");	
	case PresetColor::Turquoise:
		return CSTR("turquoise");	
	case PresetColor::Violet:
		return CSTR("violet");	
	case PresetColor::Wheat:
		return CSTR("wheat");	
	case PresetColor::White:
		return CSTR("white");	
	case PresetColor::WhiteSmoke:
		return CSTR("whiteSmoke");	
	case PresetColor::Yellow:
		return CSTR("yellow");	
	case PresetColor::YellowGreen:
		return CSTR("yellowGreen");	
	default:
		return CSTR("Unknown");
	}
}

UnsafeArray<UTF8Char> Exporter::XLSXExporter::ToFormatCode(UnsafeArray<UTF8Char> sbuff, UnsafeArray<const UTF8Char> dataFormat)
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
