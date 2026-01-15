#include "Stdafx.h"
#include "MyMemory.h"
#include "Exporter/PDFExporter.h"
#include "IO/StreamWriter.h"
#include "Math/Math_C.h"
#include "Math/Unit/Distance.h"
#include "Media/VectorDocument.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

Exporter::PDFExporter::PDFExporter()
{
}

Exporter::PDFExporter::~PDFExporter()
{
}

Int32 Exporter::PDFExporter::GetName()
{
	return *(Int32*)"PDFE";
}

IO::FileExporter::SupportType Exporter::PDFExporter::IsObjectSupported(NN<IO::ParsedObject> pobj)
{
	if (pobj->GetParserType() != IO::ParserType::VectorDocument)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	return IO::FileExporter::SupportType::NormalStream;
}

Bool Exporter::PDFExporter::GetOutputName(IntOS index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("PDF File"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.pdf"));
		return true;
	}
	return false;
}

Bool Exporter::PDFExporter::ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
{
	if (pobj->GetParserType() != IO::ParserType::VectorDocument)
	{
		return false;
	}
	NN<Media::VectorDocument> vdoc = NN<Media::VectorDocument>::ConvertFrom(pobj);
	NN<Media::VectorGraph> g;
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	IntOS i;
	IntOS j;
	IntOS pageContentId;
	IntOS infoId;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	Data::ArrayList<Int64> objPos;
	Data::ArrayList<Int32> pageContent;
	Data::ArrayList<Int32> pageList;
	Int64 refPos;
	Int64 currPos;
	Data::DateTime dt;
	NN<Text::String> s;
	stm->Write(CSTR("%PDF-1.4\r").ToByteArray());
	currPos = 9;
	objPos.Add(0);
	objPos.Add(currPos);
	sb.ClearStr();
	sb.AppendC(UTF8STRC("1 0 obj\r"));
	sb.AppendC(UTF8STRC("<< /Type /Catalog /Pages 2 0 R\r>>\rendobj\r"));
	stm->Write(sb.ToByteArray());
	currPos += sb.GetLength();
	objPos.Add(0);

	i = 0;
	j = vdoc->GetCount();
	while (i < j)
	{
		if (vdoc->GetItem(i).SetTo(g))
		{
			pageContentId = objPos.GetCount();
			objPos.Add(0);
			sb.ClearStr();

			//////////////////

			sb2.ClearStr();
			sb2.AppendIntOS(pageContentId);
			sb2.AppendC(UTF8STRC(" 0 obj\r"));
			sb2.AppendC(UTF8STRC("<</Length "));
			sb2.AppendIntOS(sb.GetLength());
			sb2.AppendC(UTF8STRC(">>\r"));
			sb2.AppendC(UTF8STRC("stream\r"));
			sb2.AppendSB(sb);
			sb2.AppendC(UTF8STRC("endstream\r"));
			sb2.AppendC(UTF8STRC("endobj\r"));

			objPos.SetItem(pageContentId, currPos);
			stm->Write(sb2.ToByteArray());
			currPos += sb2.GetLength();

			pageList.Add((Int32)objPos.GetCount());
			sb.ClearStr();
			sb.AppendIntOS(objPos.GetCount());
			sb.AppendC(UTF8STRC(" 0 obj\r"));
			sb.AppendC(UTF8STRC("<</Type/Page/MediaBox [0 0 "));
			sb.AppendI32(Double2Int32(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_MILLIMETER, Math::Unit::Distance::DU_INCH, g->GetVisibleWidthMM()) * 72.0));
			sb.AppendC(UTF8STRC(" "));
			sb.AppendI32(Double2Int32(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_MILLIMETER, Math::Unit::Distance::DU_INCH, g->GetVisibleHeightMM()) * 72.0));
			sb.AppendC(UTF8STRC("]\r"));
			sb.AppendC(UTF8STRC("/Parent 2 0 R\r"));
			sb.AppendC(UTF8STRC("/Contents "));
			sb.AppendIntOS(pageContentId);
			sb.AppendC(UTF8STRC(" 0 R\r"));
			sb.AppendC(UTF8STRC(">>\r"));
			sb.AppendC(UTF8STRC("endobj\r"));

			objPos.Add(currPos);
			stm->Write(sb.ToByteArray());
			currPos += sb.GetLength();
		}
		i++;
	}

	objPos.SetItem(2, currPos);
	sb.ClearStr();
	sb.AppendC(UTF8STRC("2 0 obj\r"));
	sb.AppendC(UTF8STRC("<< /Type /Pages /Kids [\r"));
	i = 0;
	j = pageList.GetCount();
	while (i < j)
	{
		sb.AppendI32(pageList.GetItem(i));
		sb.AppendC(UTF8STRC(" 0 R\r"));
		i++;
	}
	sb.AppendC(UTF8STRC("] /Count "));
	sb.AppendIntOS(pageList.GetCount());
	sb.AppendC(UTF8STRC("\r"));
	sb.AppendC(UTF8STRC(">>\r"));
	sb.AppendC(UTF8STRC("endobj\r"));
	stm->Write(sb.ToByteArray());
	currPos += sb.GetLength();

	infoId = objPos.GetCount();
	objPos.Add(currPos);
	sb.ClearStr();
	sb.AppendIntOS(infoId);
	sb.AppendC(UTF8STRC(" 0 obj\r"));
	sb.AppendC(UTF8STRC("<<\r"));
	if (vdoc->GetDocName().SetTo(s))
	{
		sb.AppendC(UTF8STRC("/Title ("));
		sb.Append(s);
		sb.AppendC(UTF8STRC(")\r"));
	}
	if (vdoc->GetAuthor().NotNull())
	{
		sb.AppendC(UTF8STRC("/Author ("));
		sb.AppendSlow(vdoc->GetAuthor());
		sb.AppendC(UTF8STRC(")\r"));
	}
	if (vdoc->GetSubject().NotNull())
	{
		sb.AppendC(UTF8STRC("/Subject ("));
		sb.AppendSlow(vdoc->GetSubject());
		sb.AppendC(UTF8STRC(")\r"));
	}
	if (vdoc->GetKeywords().NotNull())
	{
		sb.AppendC(UTF8STRC("/Keywords ("));
		sb.AppendSlow(vdoc->GetKeywords());
		sb.AppendC(UTF8STRC(")\r"));
	}
	if (vdoc->GetCreator().NotNull())
	{
		sb.AppendC(UTF8STRC("/Creator ("));
		sb.AppendSlow(vdoc->GetCreator());
		sb.AppendC(UTF8STRC(")\r"));
	}
	if (vdoc->GetProducer().NotNull())
	{
		sb.AppendC(UTF8STRC("/Producer ("));
		sb.AppendSlow(vdoc->GetProducer());
		sb.AppendC(UTF8STRC(")\r"));
	}
	if (vdoc->GetCreateTime())
	{
		sb.AppendC(UTF8STRC("/CreationDate (D:"));
		dt.SetTicks(vdoc->GetCreateTime());
		sptr = dt.ToString(sbuff, "yyyyMMddHHmmss");
		sb.AppendC(sbuff, (UIntOS)(sptr - sbuff));
		sb.AppendC(UTF8STRC("Z)\r"));
	}
	if (vdoc->GetModifyTime())
	{
		sb.AppendC(UTF8STRC("/ModDate (D:"));
		dt.SetTicks(vdoc->GetModifyTime());
		sptr = dt.ToString(sbuff, "yyyyMMddHHmmss");
		sb.AppendC(sbuff, (UIntOS)(sptr - sbuff));
		sb.AppendC(UTF8STRC("Z)\r"));
	}
	sb.AppendC(UTF8STRC(">>\r"));
	sb.AppendC(UTF8STRC("endobj\r"));
	stm->Write(sb.ToByteArray());
	currPos += sb.GetLength();

	refPos = currPos;
	j = objPos.GetCount();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("xref\r"));
	sb.AppendC(UTF8STRC("0 "));
	sb.AppendIntOS(objPos.GetCount());
	sb.AppendC(UTF8STRC("\r"));
	sb.AppendC(UTF8STRC("0000000000 65535 f \r"));
	i = 1;
	while (i < j)
	{
		sptr = Text::StrInt64(sbuff, objPos.GetItem(i));
		if ((sptr - sbuff) >= 10)
		{
		}
		else
		{
			sb.AppendChar('0', 10 - (sptr - sbuff));
			sb.AppendC(sbuff, (UIntOS)(sptr - sbuff));
			sb.AppendC(UTF8STRC(" 00000 n \r"));
		}
		i++;
	}
	sb.AppendC(UTF8STRC("trailer\r"));
	sb.AppendC(UTF8STRC("<< /Size "));
	sb.AppendIntOS(j);
	sb.AppendC(UTF8STRC("\r"));
	sb.AppendC(UTF8STRC("/Info "));
	sb.AppendIntOS(infoId);
	sb.AppendC(UTF8STRC(" 0 R\r"));
	sb.AppendC(UTF8STRC("/Root 1 0 R\r"));
	sb.AppendC(UTF8STRC(">>\r"));
	sb.AppendC(UTF8STRC("startxref\r"));
	sb.AppendI64(refPos);
	sb.AppendC(UTF8STRC("\r"));
	sb.AppendC(UTF8STRC("%%EOF\r"));
	stm->Write(sb.ToByteArray());
	currPos += sb.GetLength();

	return true;
}
