#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/DBReader.h"
#include "Exporter/DBPListExporter.h"
#include "IO/StreamWriter.h"
#include "Text/Encoding.h"
#include "Text/MyStringFloat.h"
#include "Text/XML.h"

Exporter::DBPListExporter::DBPListExporter()
{
	this->codePage = 65001;
}

Exporter::DBPListExporter::~DBPListExporter()
{
}

Int32 Exporter::DBPListExporter::GetName()
{
	return *(Int32*)"DPLS";
}

IO::FileExporter::SupportType Exporter::DBPListExporter::IsObjectSupported(IO::ParsedObject *pobj)
{
	if (!DB::ReadingDB::IsDBObj(pobj))
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	return IO::FileExporter::SupportType::NormalStream;
}

Bool Exporter::DBPListExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("DB Parameter List File"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.plist"));
		return true;
	}
	return false;
}

void Exporter::DBPListExporter::SetCodePage(UInt32 codePage)
{
	this->codePage = codePage;
}

Bool Exporter::DBPListExporter::ExportFile(IO::SeekableStream *stm, Text::CString fileName, IO::ParsedObject *pobj, void *param)
{
	if (!DB::ReadingDB::IsDBObj(pobj))
	{
		return false;
	}

	Text::String *name = 0;
	if (param)
	{
		DBParam *dbParam = (DBParam*)param;
		name = dbParam->names->GetItem(dbParam->tableIndex);
	}
	DB::ReadingDB *db = (DB::ReadingDB*)pobj;
	DB::DBReader *r;
	r = db->QueryTableData(CSTR_NULL, STR_CSTR(name), 0, 0, 0, CSTR_NULL, 0);
	if (r == 0)
	{
		return false;
	}
	IO::StreamWriter *writer;
	UTF8Char *lineBuff1;
	UTF8Char *lineBuff2;
	UTF8Char *sptr;
	UTF8Char *sptr2;
	UOSInt colCnt;
	UOSInt i;
	UOSInt colSize;

	NEW_CLASS(writer, IO::StreamWriter(stm, this->codePage));

	lineBuff1 = MemAlloc(UTF8Char, 65536);
	lineBuff2 = MemAlloc(UTF8Char, 65536);

	sptr = Text::StrConcatC(Text::EncodingFactory::GetInternetName(Text::StrConcatC(lineBuff1, UTF8STRC("<?xml version=\"1.0\" encoding=\"")), this->codePage), UTF8STRC("\"?>"));
	writer->WriteLineC(lineBuff1, (UOSInt)(sptr - lineBuff1));
	writer->WriteLineC(UTF8STRC("<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">"));
	writer->WriteLineC(UTF8STRC("<plist version=\"1.0\">"));
	writer->WriteLineC(UTF8STRC("<array>"));

	sptr = lineBuff2;
	colCnt = r->ColCount();
	while (r->ReadNext())
	{
		writer->WriteLineC(UTF8STRC("    <dict>"));
		sptr = lineBuff2;
		colCnt = r->ColCount();
		i = 0;
		while (i < colCnt)
		{
			r->GetName(i, lineBuff1);
			sptr2 = Text::StrConcatC(Text::XML::ToXMLText(Text::StrConcatC(lineBuff2, UTF8STRC("        <key>")), lineBuff1), UTF8STRC("</key>"));
			writer->WriteLineC(lineBuff2, (UOSInt)(sptr2 - lineBuff2));

			DB::DBUtil::ColType ct = r->GetColType(i, &colSize);
			switch (ct)
			{
			case DB::DBUtil::CT_DateTimeTZ:
			case DB::DBUtil::CT_DateTime:
				sptr = Text::StrConcatC(r->GetTimestamp(i).ToString(Text::StrConcatC(lineBuff1, UTF8STRC("        <string>")), "yyyy-MM-dd HH:mm:ss"), UTF8STRC("</string>"));
				writer->WriteLineC(lineBuff1, (UOSInt)(sptr - lineBuff1));
				break;
			case DB::DBUtil::CT_Date:
				sptr = Text::StrConcatC(r->GetTimestamp(i).ToString(Text::StrConcatC(lineBuff1, UTF8STRC("        <string>")), "yyyy-MM-dd"), UTF8STRC("</string>"));
				writer->WriteLineC(lineBuff1, (UOSInt)(sptr - lineBuff1));
				break;
			case DB::DBUtil::CT_Double:
			case DB::DBUtil::CT_Float:
				sptr = Text::StrConcatC(Text::StrDouble(Text::StrConcatC(lineBuff1, UTF8STRC("        <string>")), r->GetDbl(i)), UTF8STRC("</string>"));
				writer->WriteLineC(lineBuff1, (UOSInt)(sptr - lineBuff1));
				break;
			case DB::DBUtil::CT_Int16:
			case DB::DBUtil::CT_Int32:
			case DB::DBUtil::CT_UInt16:
				sptr = Text::StrConcatC(Text::StrInt32(Text::StrConcatC(lineBuff1, UTF8STRC("        <integer>")), r->GetInt32(i)), UTF8STRC("</integer>"));
				writer->WriteLineC(lineBuff1, (UOSInt)(sptr - lineBuff1));
				break;
			case DB::DBUtil::CT_Int64:
			case DB::DBUtil::CT_UInt32:
			case DB::DBUtil::CT_UInt64:
				sptr = Text::StrConcatC(Text::StrInt64(Text::StrConcatC(lineBuff1, UTF8STRC("        <integer>")), r->GetInt64(i)), UTF8STRC("</integer>"));
				writer->WriteLineC(lineBuff1, (UOSInt)(sptr - lineBuff1));
				break;
			case DB::DBUtil::CT_Bool:
			case DB::DBUtil::CT_Byte:
			case DB::DBUtil::CT_Binary:
			case DB::DBUtil::CT_Vector:

			case DB::DBUtil::CT_UTF8Char:
			case DB::DBUtil::CT_UTF16Char:
			case DB::DBUtil::CT_UTF32Char:
			case DB::DBUtil::CT_VarUTF8Char:
			case DB::DBUtil::CT_VarUTF16Char:
			case DB::DBUtil::CT_VarUTF32Char:
			case DB::DBUtil::CT_Unknown:
			case DB::DBUtil::CT_UUID:
			default:
				if (r->GetStr(i, lineBuff2, 65536))
				{
					sptr = Text::StrConcatC(Text::XML::ToXMLText(Text::StrConcatC(lineBuff1, UTF8STRC("        <string>")), lineBuff2), UTF8STRC("</string>"));
				}
				else
				{
					sptr = Text::StrConcatC(lineBuff1, UTF8STRC("        <string>(null)</string>"));
				}
				writer->WriteLineC(lineBuff1, (UOSInt)(sptr - lineBuff1));
				break;
			}

			i++;
		}
		writer->WriteLineC(UTF8STRC("    </dict>"));
	}
	
	MemFree(lineBuff2);
	MemFree(lineBuff1);

	writer->WriteLineC(UTF8STRC("</array>"));
	writer->WriteLineC(UTF8STRC("</plist>"));

	db->CloseReader(r);
	DEL_CLASS(writer);
	return true;
}

UOSInt Exporter::DBPListExporter::GetParamCnt()
{
	return 1;
}

void *Exporter::DBPListExporter::CreateParam(IO::ParsedObject *pobj)
{
	DBParam *param = MemAlloc(DBParam, 1);
	param->db = (DB::ReadingDB *)pobj;
	NEW_CLASS(param->names, Data::ArrayList<Text::String*>());
	param->db->QueryTableNames(CSTR_NULL, param->names);
	param->tableIndex = 0;
	return param;
}

void Exporter::DBPListExporter::DeleteParam(void *param)
{
	DBParam *dbParam = (DBParam*)param;
	LIST_FREE_STRING(dbParam->names);
	DEL_CLASS(dbParam->names);
	MemFree(dbParam);
}

Bool Exporter::DBPListExporter::GetParamInfo(UOSInt index, IO::FileExporter::ParamInfo *info)
{
	if (index == 0)
	{
		info->name = CSTR("Table");
		info->paramType = IO::FileExporter::ParamType::SELECTION;
		info->allowNull = true;
		return true;
	}
	else
	{
		return false;
	}
}

Bool Exporter::DBPListExporter::SetParamStr(void *param, UOSInt index, const UTF8Char *val)
{
	return false;
}

Bool Exporter::DBPListExporter::SetParamInt32(void *param, UOSInt index, Int32 val)
{
	return false;
}

Bool Exporter::DBPListExporter::SetParamSel(void *param, UOSInt index, Int32 selCol)
{
	if (index == 0)
	{
		DBParam *dbParam = (DBParam*)param;
		dbParam->tableIndex = (UOSInt)selCol;
		return true;
	}
	return false;
}

UTF8Char *Exporter::DBPListExporter::GetParamStr(void *param, UOSInt index, UTF8Char *buff)
{
	return 0;
}

Int32 Exporter::DBPListExporter::GetParamInt32(void *param, UOSInt index)
{
	return 0;
}

Int32 Exporter::DBPListExporter::GetParamSel(void *param, UOSInt index)
{
	if (index == 0)
	{
		DBParam *dbParam = (DBParam*)param;
		return (Int32)dbParam->tableIndex;
	}
	return 0;
}

UTF8Char *Exporter::DBPListExporter::GetParamSelItems(void *param, UOSInt index, UOSInt itemIndex, UTF8Char *buff)
{
	if (index == 0)
	{
		DBParam *dbParam = (DBParam*)param;
		Text::String *name = dbParam->names->GetItem(itemIndex);
		if (name)
		{
			return name->ConcatTo(buff);
		}
		return 0;
	}
	return 0;
}
