#include "Stdafx.h"
#include "MyMemory.h"
#include "Exporter/PDFExporter.h"
#include "IO/StreamWriter.h"
#include "Math/Math.h"
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

IO::FileExporter::SupportType Exporter::PDFExporter::IsObjectSupported(IO::ParsedObject *pobj)
{
	if (pobj->GetParserType() != IO::ParsedObject::PT_VECTOR_DOCUMENT)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	return IO::FileExporter::SupportType::NormalStream;
}

Bool Exporter::PDFExporter::GetOutputName(OSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcat(nameBuff, (const UTF8Char*)"PDF File");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.pdf");
		return true;
	}
	return false;
}

Bool Exporter::PDFExporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
{
	if (pobj->GetParserType() != IO::ParsedObject::PT_VECTOR_DOCUMENT)
	{
		return false;
	}
	Media::VectorDocument *vdoc = (Media::VectorDocument*)pobj;
	Media::VectorGraph *g;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	OSInt i;
	OSInt j;
	OSInt pageContentId;
	OSInt infoId;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	Data::ArrayList<Int64> objPos;
	Data::ArrayList<Int32> pageContent;
	Data::ArrayList<Int32> pageList;
	Int64 refPos;
	Int64 currPos;
	Data::DateTime dt;
	stm->Write((UInt8*)"%PDF-1.4\r", 9);
	currPos = 9;
	objPos.Add(0);
	objPos.Add(currPos);
	sb.ClearStr();
	sb.Append((const UTF8Char*)"1 0 obj\r");
	sb.Append((const UTF8Char*)"<< /Type /Catalog /Pages 2 0 R\r>>\rendobj\r");
	stm->Write((UInt8*)sb.ToString(), sb.GetLength());
	currPos += sb.GetLength();
	objPos.Add(0);

	i = 0;
	j = vdoc->GetCount();
	while (i < j)
	{
		g = vdoc->GetItem(i);
		pageContentId = objPos.GetCount();
		objPos.Add(0);
		sb.ClearStr();

		//////////////////

		sb2.ClearStr();
		sb2.AppendOSInt(pageContentId);
		sb2.Append((const UTF8Char*)" 0 obj\r");
		sb2.Append((const UTF8Char*)"<</Length ");
		sb2.AppendOSInt(sb.GetLength());
		sb2.Append((const UTF8Char*)">>\r");
		sb2.Append((const UTF8Char*)"stream\r");
		sb2.AppendSB(&sb);
		sb2.Append((const UTF8Char*)"endstream\r");
		sb2.Append((const UTF8Char*)"endobj\r");

		objPos.SetItem(pageContentId, currPos);
		stm->Write((UInt8*)sb2.ToString(), sb2.GetLength());
		currPos += sb2.GetLength();

		pageList.Add((Int32)objPos.GetCount());
		sb.ClearStr();
		sb.AppendOSInt(objPos.GetCount());
		sb.Append((const UTF8Char*)" 0 obj\r");
		sb.Append((const UTF8Char*)"<</Type/Page/MediaBox [0 0 ");
		sb.AppendI32(Math::Double2Int32(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_MILLIMETER, Math::Unit::Distance::DU_INCH, g->GetVisibleWidthMM()) * 72.0));
		sb.Append((const UTF8Char*)" ");
		sb.AppendI32(Math::Double2Int32(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_MILLIMETER, Math::Unit::Distance::DU_INCH, g->GetVisibleHeightMM()) * 72.0));
		sb.Append((const UTF8Char*)"]\r");
		sb.Append((const UTF8Char*)"/Parent 2 0 R\r");
		sb.Append((const UTF8Char*)"/Contents ");
		sb.AppendOSInt(pageContentId);
		sb.Append((const UTF8Char*)" 0 R\r");
		sb.Append((const UTF8Char*)">>\r");
		sb.Append((const UTF8Char*)"endobj\r");

		objPos.Add(currPos);
		stm->Write((UInt8*)sb.ToString(), sb.GetLength());
		currPos += sb.GetLength();

		i++;
	}

	objPos.SetItem(2, currPos);
	sb.ClearStr();
	sb.Append((const UTF8Char*)"2 0 obj\r");
	sb.Append((const UTF8Char*)"<< /Type /Pages /Kids [\r");
	i = 0;
	j = pageList.GetCount();
	while (i < j)
	{
		sb.AppendI32(pageList.GetItem(i));
		sb.Append((const UTF8Char*)" 0 R\r");
		i++;
	}
	sb.Append((const UTF8Char*)"] /Count ");
	sb.AppendOSInt(pageList.GetCount());
	sb.Append((const UTF8Char*)"\r");
	sb.Append((const UTF8Char*)">>\r");
	sb.Append((const UTF8Char*)"endobj\r");
	stm->Write((UInt8*)sb.ToString(), sb.GetLength());
	currPos += sb.GetLength();

	infoId = objPos.GetCount();
	objPos.Add(currPos);
	sb.ClearStr();
	sb.AppendOSInt(infoId);
	sb.Append((const UTF8Char*)" 0 obj\r");
	sb.Append((const UTF8Char*)"<<\r");
	if (vdoc->GetDocName())
	{
		sb.Append((const UTF8Char*)"/Title (");
		sb.Append(vdoc->GetDocName());
		sb.Append((const UTF8Char*)")\r");
	}
	if (vdoc->GetAuthor())
	{
		sb.Append((const UTF8Char*)"/Author (");
		sb.Append(vdoc->GetAuthor());
		sb.Append((const UTF8Char*)")\r");
	}
	if (vdoc->GetSubject())
	{
		sb.Append((const UTF8Char*)"/Subject (");
		sb.Append(vdoc->GetSubject());
		sb.Append((const UTF8Char*)")\r");
	}
	if (vdoc->GetKeywords())
	{
		sb.Append((const UTF8Char*)"/Keywords (");
		sb.Append(vdoc->GetKeywords());
		sb.Append((const UTF8Char*)")\r");
	}
	if (vdoc->GetCreator())
	{
		sb.Append((const UTF8Char*)"/Creator (");
		sb.Append(vdoc->GetCreator());
		sb.Append((const UTF8Char*)")\r");
	}
	if (vdoc->GetProducer())
	{
		sb.Append((const UTF8Char*)"/Producer (");
		sb.Append(vdoc->GetProducer());
		sb.Append((const UTF8Char*)")\r");
	}
	if (vdoc->GetCreateTime())
	{
		sb.Append((const UTF8Char*)"/CreationDate (D:");
		dt.SetTicks(vdoc->GetCreateTime());
		dt.ToString(sbuff, "yyyyMMddHHmmss");
		sb.Append(sbuff);
		sb.Append((const UTF8Char*)"Z)\r");
	}
	if (vdoc->GetModifyTime())
	{
		sb.Append((const UTF8Char*)"/ModDate (D:");
		dt.SetTicks(vdoc->GetModifyTime());
		dt.ToString(sbuff, "yyyyMMddHHmmss");
		sb.Append(sbuff);
		sb.Append((const UTF8Char*)"Z)\r");
	}
	sb.Append((const UTF8Char*)">>\r");
	sb.Append((const UTF8Char*)"endobj\r");
	stm->Write((UInt8*)sb.ToString(), sb.GetLength());
	currPos += sb.GetLength();

	refPos = currPos;
	j = objPos.GetCount();
	sb.ClearStr();
	sb.Append((const UTF8Char*)"xref\r");
	sb.Append((const UTF8Char*)"0 ");
	sb.AppendOSInt(objPos.GetCount());
	sb.Append((const UTF8Char*)"\r");
	sb.Append((const UTF8Char*)"0000000000 65535 f \r");
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
			sb.Append(sbuff);
			sb.Append((const UTF8Char*)" 00000 n \r");
		}
		i++;
	}
	sb.Append((const UTF8Char*)"trailer\r");
	sb.Append((const UTF8Char*)"<< /Size ");
	sb.AppendOSInt(j);
	sb.Append((const UTF8Char*)"\r");
	sb.Append((const UTF8Char*)"/Info ");
	sb.AppendOSInt(infoId);
	sb.Append((const UTF8Char*)" 0 R\r");
	sb.Append((const UTF8Char*)"/Root 1 0 R\r");
	sb.Append((const UTF8Char*)">>\r");
	sb.Append((const UTF8Char*)"startxref\r");
	sb.AppendI64(refPos);
	sb.Append((const UTF8Char*)"\r");
	sb.Append((const UTF8Char*)"%%EOF\r");
	stm->Write((UInt8*)sb.ToString(), sb.GetLength());
	currPos += sb.GetLength();

	return true;
}
