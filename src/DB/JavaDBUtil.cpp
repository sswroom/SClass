#include "Stdafx.h"
#include "DB/DBReader.h"
#include "DB/JavaDBUtil.h"
#include "DB/MSSQLConn.h"
#include "Text/JavaText.h"
#include "Text/JSText.h"

#define LOGPREFIX CSTR("DB:")

Text::String *DB::JavaDBUtil::AppendFieldAnno(Text::StringBuilderUTF8 *sb, DB::ColDef *colDef)
{
	if (colDef->IsPK())
	{
		sb->AppendC(UTF8STRC("\t@Id\r\n"));
		if (colDef->IsAutoInc())
		{
			sb->AppendC(UTF8STRC("\t@GeneratedValue(strategy = GenerationType.IDENTITY)\r\n"));
		}
	}
	if (colDef->GetColName()->HasUpperCase())
	{
		sb->AppendC(UTF8STRC("\t@Column(name="));
		Text::String *s = Text::JSText::ToNewJSTextDQuote(colDef->GetColName()->v);
		sb->Append(s);
		s->Release();
		sb->AppendC(UTF8STRC(")\r\n"));
		return colDef->GetColName()->ToLower();
	}
	else
	{
		return colDef->GetColName()->Clone();
	}
}

void DB::JavaDBUtil::AppendFieldDef(Text::StringBuilderUTF8 *sb, DB::ColDef *col, Text::String *colName)
{
	sb->AppendC(UTF8STRC("\tprivate "));
	sb->Append(Text::JavaText::GetJavaTypeName(col->GetColType(), col->IsNotNull()));
	sb->AppendUTF8Char(' ');
	Text::JavaText::ToJavaName(sb, colName->v, false);
	sb->AppendC(UTF8STRC(";\r\n"));
}

void DB::JavaDBUtil::AppendConstrHdr(Text::StringBuilderUTF8 *sb, DB::ColDef *col, Text::String *colName, Bool isLast)
{
	sb->Append(Text::JavaText::GetJavaTypeName(col->GetColType(), col->IsNotNull()));
	sb->AppendUTF8Char(' ');
	Text::JavaText::ToJavaName(sb, colName->v, false);
	if (!isLast)
	{
		sb->AppendC(UTF8STRC(", "));
	}
}

void DB::JavaDBUtil::AppendConstrItem(Text::StringBuilderUTF8 *sb, Text::String *colName)
{
	sb->AppendC(UTF8STRC("\t\tthis."));
	Text::JavaText::ToJavaName(sb, colName->v, false);
	sb->AppendC(UTF8STRC(" = "));
	Text::JavaText::ToJavaName(sb, colName->v, false);
	sb->AppendC(UTF8STRC(";\r\n"));
}

void DB::JavaDBUtil::AppendGetterSetter(Text::StringBuilderUTF8 *sb, DB::ColDef *col, Text::String *colName)
{
	sb->AppendC(UTF8STRC("\r\n"));
	sb->AppendC(UTF8STRC("\tpublic "));
	sb->Append(Text::JavaText::GetJavaTypeName(col->GetColType(), col->IsNotNull()));
	if (col->GetColType() == DB::DBUtil::CT_Bool)
	{
		sb->AppendC(UTF8STRC(" is"));
	}
	else
	{
		sb->AppendC(UTF8STRC(" get"));
	}
	Text::JavaText::ToJavaName(sb, colName->v, true);
	sb->AppendC(UTF8STRC("() {\r\n"));
	sb->AppendC(UTF8STRC("\t\treturn this."));
	Text::JavaText::ToJavaName(sb, colName->v, false);
	sb->AppendC(UTF8STRC(";\r\n"));
	sb->AppendC(UTF8STRC("\t}\r\n"));
	sb->AppendC(UTF8STRC("\r\n"));
	sb->AppendC(UTF8STRC("\tpublic void set"));
	Text::JavaText::ToJavaName(sb, colName->v, true);
	sb->AppendUTF8Char('(');
	sb->Append(Text::JavaText::GetJavaTypeName(col->GetColType(), col->IsNotNull()));
	sb->AppendUTF8Char(' ');
	Text::JavaText::ToJavaName(sb, colName->v, false);
	sb->AppendC(UTF8STRC(") {\r\n"));
	sb->AppendC(UTF8STRC("\t\tthis."));
	Text::JavaText::ToJavaName(sb, colName->v, false);
	sb->AppendC(UTF8STRC(" = "));
	Text::JavaText::ToJavaName(sb, colName->v, false);
	sb->AppendC(UTF8STRC(";\r\n"));
	sb->AppendC(UTF8STRC("\t}\r\n"));
}

void DB::JavaDBUtil::AppendEqualsItem(Text::StringBuilderUTF8 *sb, DB::ColDef *col, Text::String *colName, Text::String *clsName, Bool isLast)
{
	Bool isObj = true;
	if (col->IsNotNull())
	{
		switch (col->GetColType())
		{
		case DB::DBUtil::CT_Bool:
		case DB::DBUtil::CT_Byte:
		case DB::DBUtil::CT_Int16:
		case DB::DBUtil::CT_UInt16:
		case DB::DBUtil::CT_UInt32:
		case DB::DBUtil::CT_Int32:
		case DB::DBUtil::CT_Int64:
		case DB::DBUtil::CT_UInt64:
		case DB::DBUtil::CT_Double:
		case DB::DBUtil::CT_Float:
		case DB::DBUtil::CT_Decimal:
			isObj = false;
			break;
		case DB::DBUtil::CT_UTF8Char:
		case DB::DBUtil::CT_UTF16Char:
		case DB::DBUtil::CT_UTF32Char:
		case DB::DBUtil::CT_VarUTF8Char:
		case DB::DBUtil::CT_VarUTF16Char:
		case DB::DBUtil::CT_VarUTF32Char:
		case DB::DBUtil::CT_UUID:
		case DB::DBUtil::CT_Date:
		case DB::DBUtil::CT_DateTime:
		case DB::DBUtil::CT_DateTimeTZ:
		case DB::DBUtil::CT_Binary:
		case DB::DBUtil::CT_Vector:
		case DB::DBUtil::CT_Unknown:
		default:
			break;
		}
	}
	if (isObj)
	{
		sb->AppendC(UTF8STRC("Objects.equals("));
		Text::JavaText::ToJavaName(sb, colName->v, false);
		sb->AppendC(UTF8STRC(", "));
		Text::JavaText::ToJavaName(sb, clsName->v, false);
		sb->AppendUTF8Char('.');
		Text::JavaText::ToJavaName(sb, colName->v, false);
		sb->AppendUTF8Char(')');
	}
	else
	{
		Text::JavaText::ToJavaName(sb, colName->v, false);
		sb->AppendC(UTF8STRC(" == "));
		Text::JavaText::ToJavaName(sb, clsName->v, false);
		sb->AppendUTF8Char('.');
		Text::JavaText::ToJavaName(sb, colName->v, false);
	}
	if (!isLast)
	{
		sb->AppendC(UTF8STRC(" && "));
	}
}

void DB::JavaDBUtil::AppendHashCodeItem(Text::StringBuilderUTF8 *sb, Text::String *colName, Bool isLast)
{
	Text::JavaText::ToJavaName(sb, colName->v, false);
	if (!isLast)
	{
		sb->AppendC(UTF8STRC(", "));
	}
}

void DB::JavaDBUtil::AppendFieldOrderItem(Text::StringBuilderUTF8 *sb, Text::String *colName, Bool isLast)
{
	sb->AppendC(UTF8STRC("\t\t\""));
	Text::JavaText::ToJavaName(sb, colName->v, false);
	if (isLast)
	{
		sb->AppendC(UTF8STRC("\"\r\n"));
	}
	else
	{
		sb->AppendC(UTF8STRC("\",\r\n"));
	}
}

DB::DBTool *DB::JavaDBUtil::OpenJDBC(Text::String *url, Text::String *username, Text::String *password, IO::LogTool *log, Net::SocketFactory *sockf)
{
	if (url == 0 || !url->StartsWith(UTF8STRC("jdbc:")))
	{
		return 0;
	}
	if (url->StartsWith(5, UTF8STRC("sqlserver://")))
	{
		Text::StringBuilderUTF8 sb;
		Text::PString sarr[2];
		Text::PString sarr2[2];
		UOSInt scnt;
		UOSInt scnt2;
		UInt16 port = 1433;
		const UTF8Char *dbName = 0;
		sb.AppendC(&url->v[17], url->leng - 17);
		sarr[1] = sb;
		scnt = Text::StrSplitP(sarr, 2, sarr[1], ';');
		scnt2 = Text::StrSplitP(sarr2, 2, sarr[0], ':');
		if (scnt2 == 2)
		{
			if (!Text::StrToUInt16(sarr2[1].v, &port))
			{
				return 0;
			}
		}
		while (scnt == 2)
		{
			scnt = Text::StrSplitP(sarr, 2, sarr[1], ';');
			if (Text::StrStartsWithICaseC(sarr[0].v, sarr[0].leng, UTF8STRC("databaseName=")))
			{
				dbName = &sarr[0].v[13];
			}
		}
		return MSSQLConn::CreateDBToolTCP(sarr2[0].v, port, dbName, STR_PTR(username), STR_PTR(password), log, LOGPREFIX);
	}
	return 0;
}

Bool DB::JavaDBUtil::ToJavaEntity(Text::StringBuilderUTF8 *sb, Text::String *schemaName, Text::String *tableName, DB::ReadingDBTool *db)
{
	Text::StringBuilderUTF8 sbConstrHdr;
	Text::StringBuilderUTF8 sbConstrItem;
	Text::StringBuilderUTF8 sbGetterSetter;
	Text::StringBuilderUTF8 sbEquals;
	Text::StringBuilderUTF8 sbHashCode;
	Text::StringBuilderUTF8 sbFieldOrder;
	sb->AppendC(UTF8STRC("@Entity\r\n"));
	sb->AppendC(UTF8STRC("@Table(name="));
	Text::JSText::ToJSTextDQuote(sb, tableName->v);
	if (schemaName)
	{
		sb->AppendC(UTF8STRC(", schema="));
		Text::JSText::ToJSTextDQuote(sb, schemaName->v);
	}
	sb->AppendC(UTF8STRC(")\r\n"));
	sb->AppendC(UTF8STRC("public class "));
	Text::String *clsName;
	if (Text::StrHasUpperCase(tableName->v))
	{
		clsName = Text::String::New(tableName->v, tableName->leng);
		Text::StrToLowerC(clsName->v, clsName->v, clsName->leng);
	}
	else
	{
		clsName = tableName->Clone();
	}
	Text::JavaText::ToJavaName(sb, clsName->v, true);
	sb->AppendC(UTF8STRC("\r\n"));

	sbConstrHdr.AppendC(UTF8STRC("\r\n"));
	sbConstrHdr.AppendC(UTF8STRC("\tpublic "));
	Text::JavaText::ToJavaName(&sbConstrHdr, clsName->v, true);
	sbConstrHdr.AppendC(UTF8STRC("() {\r\n"));
	sbConstrHdr.AppendC(UTF8STRC("\t}\r\n"));
	sbConstrHdr.AppendC(UTF8STRC("\r\n"));
	sbConstrHdr.AppendC(UTF8STRC("\tpublic "));
	Text::JavaText::ToJavaName(&sbConstrHdr, clsName->v, true);
	sbConstrHdr.AppendUTF8Char('(');

	sbEquals.AppendC(UTF8STRC("\r\n"));
	sbEquals.AppendC(UTF8STRC("\t@Override\r\n"));
	sbEquals.AppendC(UTF8STRC("\tpublic boolean equals(Object o) {\r\n"));
	sbEquals.AppendC(UTF8STRC("\t\tif (o == this)\r\n"));
	sbEquals.AppendC(UTF8STRC("\t\t\treturn true;\r\n"));
	sbEquals.AppendC(UTF8STRC("\t\tif (!(o instanceof "));
	Text::JavaText::ToJavaName(&sbEquals, clsName->v, true);
	sbEquals.AppendC(UTF8STRC(")) {\r\n"));
	sbEquals.AppendC(UTF8STRC("\t\t\treturn false;\r\n"));
	sbEquals.AppendC(UTF8STRC("\t\t}\r\n"));
	sbEquals.AppendC(UTF8STRC("\t\t"));
	Text::JavaText::ToJavaName(&sbEquals, clsName->v, true);
	sbEquals.AppendUTF8Char(' ');
	Text::JavaText::ToJavaName(&sbEquals, clsName->v, false);
	sbEquals.AppendC(UTF8STRC(" = ("));
	Text::JavaText::ToJavaName(&sbEquals, clsName->v, true);
	sbEquals.AppendC(UTF8STRC(") o;\r\n"));
	sbEquals.AppendC(UTF8STRC("\t\treturn "));

	sbHashCode.AppendC(UTF8STRC("\r\n"));
	sbHashCode.AppendC(UTF8STRC("\t@Override\r\n"));
	sbHashCode.AppendC(UTF8STRC("\tpublic int hashCode() {\r\n"));
	sbHashCode.AppendC(UTF8STRC("\t\treturn Objects.hash("));

	sbFieldOrder.AppendC(UTF8STRC("\r\n"));
	sbFieldOrder.AppendC(UTF8STRC("\t@Override\r\n"));
	sbFieldOrder.AppendC(UTF8STRC("\tpublic String toString() {\r\n"));
	sbFieldOrder.AppendC(UTF8STRC("\t\treturn DataTools.toObjectString(this);\r\n"));
	sbFieldOrder.AppendC(UTF8STRC("\t}\r\n"));
	sbFieldOrder.AppendC(UTF8STRC("\r\n"));
	sbFieldOrder.AppendC(UTF8STRC("\tpublic static String[] getFieldOrder() {\r\n"));
	sbFieldOrder.AppendC(UTF8STRC("\t\treturn new String[] {\r\n"));

	sb->AppendC(UTF8STRC("{\r\n"));
	DB::TableDef *tableDef = db->GetTableDef(STR_CSTR(schemaName), tableName->ToCString());
	Text::String *colName;
	if (tableDef)
	{
		UOSInt j;
		UOSInt k;
		DB::ColDef *colDef;
		j = 0;
		k = tableDef->GetColCnt();
		while (j < k)
		{
			colDef = tableDef->GetCol(j);
			colName = AppendFieldAnno(sb, colDef);
			AppendFieldDef(sb, colDef, colName);
			AppendConstrHdr(&sbConstrHdr, colDef, colName, j + 1 == k);
			AppendConstrItem(&sbConstrItem, colName);
			AppendGetterSetter(&sbGetterSetter, colDef, colName);
			AppendEqualsItem(&sbEquals, colDef, colName, clsName, j + 1 == k);
			AppendHashCodeItem(&sbHashCode, colName, j + 1 == k);
			AppendFieldOrderItem(&sbFieldOrder, colName, j + 1 == k);
			colName->Release();
			j++;
		}
	}
	else
	{
		DB::DBReader *r = db->QueryTableData(STR_CSTR(schemaName), tableName->ToCString(), 0, 0, 0, CSTR_NULL, 0);
		if (r)
		{
			DB::ColDef colDef(CSTR(""));
			UOSInt j = 0;
			UOSInt k = r->ColCount();
			while (j < k)
			{
				if (r->GetColDef(j, &colDef))
				{
					colName = AppendFieldAnno(sb, &colDef);
					AppendFieldDef(sb, &colDef, colName);
					AppendConstrHdr(&sbConstrHdr, &colDef, colName, j + 1 == k);
					AppendConstrItem(&sbConstrItem, colName);
					AppendGetterSetter(&sbGetterSetter, &colDef, colName);
					AppendEqualsItem(&sbEquals, &colDef, colName, clsName, j + 1 == k);
					AppendHashCodeItem(&sbHashCode, colName, j + 1 == k);
					AppendFieldOrderItem(&sbFieldOrder, colName, j + 1 == k);
					colName->Release();
				}
				j++;
			}
		}
	}
	DEL_CLASS(tableDef);

	sbConstrHdr.AppendC(UTF8STRC(") {\r\n"));
	sbConstrItem.AppendC(UTF8STRC("\t}\r\n"));
	sbEquals.AppendC(UTF8STRC(";\r\n"));
	sbEquals.AppendC(UTF8STRC("\t}\r\n"));
	sbHashCode.AppendC(UTF8STRC(");\r\n"));
	sbHashCode.AppendC(UTF8STRC("\t}\r\n"));
	sbFieldOrder.AppendC(UTF8STRC("\t\t};\r\n"));
	sbFieldOrder.AppendC(UTF8STRC("\t}\r\n"));

	sb->AllocLeng(sbConstrHdr.GetLength() + sbConstrItem.GetLength() + sbGetterSetter.GetLength() + sbEquals.GetLength() + sbHashCode.GetLength() + sbFieldOrder.GetLength());
	sb->AppendC(sbConstrHdr.ToString(), sbConstrHdr.GetLength());
	sb->AppendC(sbConstrItem.ToString(), sbConstrItem.GetLength());
	sb->AppendC(sbGetterSetter.ToString(), sbGetterSetter.GetLength());
	sb->AppendC(sbEquals.ToString(), sbEquals.GetLength());
	sb->AppendC(sbHashCode.ToString(), sbHashCode.GetLength());
	sb->AppendC(sbFieldOrder.ToString(), sbFieldOrder.GetLength());
	sb->AppendC(UTF8STRC("}\r\n"));
	clsName->Release();
	return true;
}
