#include "Stdafx.h"
#include "DB/DBReader.h"
#include "DB/JavaDBUtil.h"
#include "DB/MSSQLConn.h"
#include "Text/JavaText.h"
#include "Text/JSText.h"

#define LOGPREFIX ((const UTF8Char*)"DB:")

Text::String *DB::JavaDBUtil::AppendFieldAnno(Text::StringBuilderUTF *sb, DB::ColDef *colDef)
{
	if (colDef->IsPK())
	{
		sb->Append((const UTF8Char*)"\t@Id\r\n");
		if (colDef->IsAutoInc())
		{
			sb->Append((const UTF8Char*)"\t@GeneratedValue(strategy = GenerationType.IDENTITY)\r\n");
		}
	}
	if (colDef->GetColName()->HasUpperCase())
	{
		sb->Append((const UTF8Char*)"\t@Column(name=");
		Text::String *s = Text::JSText::ToNewJSTextDQuote(colDef->GetColName()->v);
		sb->Append(s);
		s->Release();
		sb->Append((const UTF8Char*)")\r\n");
		return colDef->GetColName()->ToLower();
	}
	else
	{
		return colDef->GetColName()->Clone();
	}
}

void DB::JavaDBUtil::AppendFieldDef(Text::StringBuilderUTF *sb, DB::ColDef *col, Text::String *colName)
{
	sb->Append((const UTF8Char*)"\tprivate ");
	sb->Append(Text::JavaText::GetJavaTypeName(col->GetColType(), col->IsNotNull()));
	sb->AppendChar(' ', 1);
	Text::JavaText::ToJavaName(sb, colName->v, false);
	sb->Append((const UTF8Char*)";\r\n");
}

void DB::JavaDBUtil::AppendConstrHdr(Text::StringBuilderUTF *sb, DB::ColDef *col, Text::String *colName, Bool isLast)
{
	sb->Append(Text::JavaText::GetJavaTypeName(col->GetColType(), col->IsNotNull()));
	sb->AppendChar(' ', 1);
	Text::JavaText::ToJavaName(sb, colName->v, false);
	if (!isLast)
	{
		sb->Append((const UTF8Char*)", ");
	}
}

void DB::JavaDBUtil::AppendConstrItem(Text::StringBuilderUTF *sb, Text::String *colName)
{
	sb->Append((const UTF8Char*)"\t\tthis.");
	Text::JavaText::ToJavaName(sb, colName->v, false);
	sb->Append((const UTF8Char*)" = ");
	Text::JavaText::ToJavaName(sb, colName->v, false);
	sb->Append((const UTF8Char*)";\r\n");
}

void DB::JavaDBUtil::AppendGetterSetter(Text::StringBuilderUTF *sb, DB::ColDef *col, Text::String *colName)
{
	sb->Append((const UTF8Char*)"\r\n");
	sb->Append((const UTF8Char*)"\tpublic ");
	sb->Append(Text::JavaText::GetJavaTypeName(col->GetColType(), col->IsNotNull()));
	if (col->GetColType() == DB::DBUtil::CT_Bool)
	{
		sb->Append((const UTF8Char*)" is");
	}
	else
	{
		sb->Append((const UTF8Char*)" get");
	}
	Text::JavaText::ToJavaName(sb, colName->v, true);
	sb->Append((const UTF8Char*)"() {\r\n");
	sb->Append((const UTF8Char*)"\t\treturn this.");
	Text::JavaText::ToJavaName(sb, colName->v, false);
	sb->Append((const UTF8Char*)";\r\n");
	sb->Append((const UTF8Char*)"\t}\r\n");
	sb->Append((const UTF8Char*)"\r\n");
	sb->Append((const UTF8Char*)"\tpublic void set");
	Text::JavaText::ToJavaName(sb, colName->v, true);
	sb->AppendChar('(', 1);
	sb->Append(Text::JavaText::GetJavaTypeName(col->GetColType(), col->IsNotNull()));
	sb->AppendChar(' ', 1);
	Text::JavaText::ToJavaName(sb, colName->v, false);
	sb->Append((const UTF8Char*)") {\r\n");
	sb->Append((const UTF8Char*)"\t\tthis.");
	Text::JavaText::ToJavaName(sb, colName->v, false);
	sb->Append((const UTF8Char*)" = ");
	Text::JavaText::ToJavaName(sb, colName->v, false);
	sb->Append((const UTF8Char*)";\r\n");
	sb->Append((const UTF8Char*)"\t}\r\n");
}

void DB::JavaDBUtil::AppendEqualsItem(Text::StringBuilderUTF *sb, DB::ColDef *col, Text::String *colName, Text::String *clsName, Bool isLast)
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
			isObj = false;
			break;
		case DB::DBUtil::CT_VarChar:
		case DB::DBUtil::CT_Char:
		case DB::DBUtil::CT_NVarChar:
		case DB::DBUtil::CT_NChar:
		case DB::DBUtil::CT_UUID:
		case DB::DBUtil::CT_DateTime:
		case DB::DBUtil::CT_DateTime2:
		case DB::DBUtil::CT_Binary:
		case DB::DBUtil::CT_Vector:
		case DB::DBUtil::CT_Unknown:
		default:
			break;
		}
	}
	if (isObj)
	{
		sb->Append((const UTF8Char*)"Objects.equals(");
		Text::JavaText::ToJavaName(sb, colName->v, false);
		sb->Append((const UTF8Char*)", ");
		Text::JavaText::ToJavaName(sb, clsName->v, false);
		sb->AppendChar('.', 1);
		Text::JavaText::ToJavaName(sb, colName->v, false);
		sb->AppendChar(')', 1);
	}
	else
	{
		Text::JavaText::ToJavaName(sb, colName->v, false);
		sb->Append((const UTF8Char*)" == ");
		Text::JavaText::ToJavaName(sb, clsName->v, false);
		sb->AppendChar('.', 1);
		Text::JavaText::ToJavaName(sb, colName->v, false);
	}
	if (!isLast)
	{
		sb->Append((const UTF8Char*)" && ");
	}
}

void DB::JavaDBUtil::AppendHashCodeItem(Text::StringBuilderUTF *sb, Text::String *colName, Bool isLast)
{
	Text::JavaText::ToJavaName(sb, colName->v, false);
	if (!isLast)
	{
		sb->Append((const UTF8Char*)", ");
	}
}

void DB::JavaDBUtil::AppendFieldOrderItem(Text::StringBuilderUTF *sb, Text::String *colName, Bool isLast)
{
	sb->Append((const UTF8Char*)"\t\t\"");
	Text::JavaText::ToJavaName(sb, colName->v, false);
	if (isLast)
	{
		sb->Append((const UTF8Char*)"\"\r\n");
	}
	else
	{
		sb->Append((const UTF8Char*)"\",\r\n");
	}
}

DB::DBTool *DB::JavaDBUtil::OpenJDBC(Text::String *url, Text::String *username, Text::String *password, IO::LogTool *log, Net::SocketFactory *sockf)
{
	if (url == 0 || !url->StartsWith((const UTF8Char*)"jdbc:"))
	{
		return 0;
	}
	if (url->StartsWith(5, (const UTF8Char*)"sqlserver://"))
	{
		Text::StringBuilderUTF8 sb;
		UTF8Char *sarr[2];
		UTF8Char *sarr2[2];
		UOSInt scnt;
		UOSInt scnt2;
		UInt16 port = 1433;
		const UTF8Char *dbName = 0;
		sb.AppendC(&url->v[17], url->leng - 17);
		sarr[1] = sb.ToString();
		scnt = Text::StrSplit(sarr, 2, sarr[1], ';');
		scnt2 = Text::StrSplit(sarr2, 2, sarr[0], ':');
		if (scnt2 == 2)
		{
			if (!Text::StrToUInt16(sarr2[1], &port))
			{
				return 0;
			}
		}
		while (scnt == 2)
		{
			scnt = Text::StrSplit(sarr, 2, sarr[1], ';');
			if (Text::StrStartsWithICase(sarr[0], (const UTF8Char*)"databaseName="))
			{
				dbName = &sarr[0][13];
			}
		}
		return MSSQLConn::CreateDBToolTCP(sarr2[0], port, dbName, STR_PTR(username), STR_PTR(password), log, LOGPREFIX);
	}
	return 0;
}

Bool DB::JavaDBUtil::ToJavaEntity(Text::StringBuilderUTF *sb, Text::String *tableName, DB::ReadingDBTool *db)
{
	Text::StringBuilderUTF8 sbConstrHdr;
	Text::StringBuilderUTF8 sbConstrItem;
	Text::StringBuilderUTF8 sbGetterSetter;
	Text::StringBuilderUTF8 sbEquals;
	Text::StringBuilderUTF8 sbHashCode;
	Text::StringBuilderUTF8 sbFieldOrder;
	UOSInt i;
	const UTF8Char *csptr;
	sb->Append((const UTF8Char*)"@Entity\r\n");
	sb->Append((const UTF8Char*)"@Table(name=");
	i = tableName->IndexOf('.');
	Text::JSText::ToJSTextDQuote(sb, &tableName->v[i + 1]);
	if (i != INVALID_INDEX)
	{
		csptr = Text::StrCopyNewC(tableName->v, i);
		sb->Append((const UTF8Char*)", schema=");
		Text::JSText::ToJSTextDQuote(sb, csptr);
		Text::StrDelNew(csptr);
	}
	sb->Append((const UTF8Char*)")\r\n");
	sb->Append((const UTF8Char*)"public class ");
	Text::String *clsName;
	if (Text::StrHasUpperCase(&tableName->v[i + 1]))
	{
		clsName = Text::String::New(&tableName->v[i + 1], tableName->leng - i - 1);
		Text::StrToLower(clsName->v, clsName->v);
		Text::JavaText::ToJavaName(sb, clsName->v, true);
		clsName->Release();
	}
	else
	{
		if (i != INVALID_INDEX)
		{
			clsName = Text::String::New(&tableName->v[i + 1], tableName->leng - i - 1);
		}
		else
		{
			clsName = tableName->Clone();
		}
	}
	Text::JavaText::ToJavaName(sb, clsName->v, true);
	sb->Append((const UTF8Char*)"\r\n");

	sbConstrHdr.Append((const UTF8Char*)"\r\n");
	sbConstrHdr.Append((const UTF8Char*)"\tpublic ");
	Text::JavaText::ToJavaName(&sbConstrHdr, clsName->v, true);
	sbConstrHdr.Append((const UTF8Char*)"() {\r\n");
	sbConstrHdr.Append((const UTF8Char*)"\t}\r\n");
	sbConstrHdr.Append((const UTF8Char*)"\r\n");
	sbConstrHdr.Append((const UTF8Char*)"\tpublic ");
	Text::JavaText::ToJavaName(&sbConstrHdr, clsName->v, true);
	sbConstrHdr.AppendChar('(', 1);

	sbEquals.Append((const UTF8Char*)"\r\n");
	sbEquals.Append((const UTF8Char*)"\t@Override\r\n");
	sbEquals.Append((const UTF8Char*)"\tpublic boolean equals(Object o) {\r\n");
	sbEquals.Append((const UTF8Char*)"\t\tif (o == this)\r\n");
	sbEquals.Append((const UTF8Char*)"\t\t\treturn true;\r\n");
	sbEquals.Append((const UTF8Char*)"\t\tif (!(o instanceof ");
	Text::JavaText::ToJavaName(&sbEquals, clsName->v, true);
	sbEquals.Append((const UTF8Char*)")) {\r\n");
	sbEquals.Append((const UTF8Char*)"\t\t\treturn false;\r\n");
	sbEquals.Append((const UTF8Char*)"\t\t}\r\n");
	sbEquals.Append((const UTF8Char*)"\t\t");
	Text::JavaText::ToJavaName(&sbEquals, clsName->v, true);
	sbEquals.AppendChar(' ', 1);
	Text::JavaText::ToJavaName(&sbEquals, clsName->v, false);
	sbEquals.Append((const UTF8Char*)" = (");
	Text::JavaText::ToJavaName(&sbEquals, clsName->v, true);
	sbEquals.Append((const UTF8Char*)") o;\r\n");
	sbEquals.Append((const UTF8Char*)"\t\treturn ");

	sbHashCode.Append((const UTF8Char*)"\r\n");
	sbHashCode.Append((const UTF8Char*)"\t@Override\r\n");
	sbHashCode.Append((const UTF8Char*)"\tpublic int hashCode() {\r\n");
	sbHashCode.Append((const UTF8Char*)"\t\treturn Objects.hash(");

	sbFieldOrder.Append((const UTF8Char*)"\r\n");
	sbFieldOrder.Append((const UTF8Char*)"\t@Override\r\n");
	sbFieldOrder.Append((const UTF8Char*)"\tpublic String toString() {\r\n");
	sbFieldOrder.Append((const UTF8Char*)"\t\treturn DataTools.toObjectString(this);\r\n");
	sbFieldOrder.Append((const UTF8Char*)"\t}\r\n");
	sbFieldOrder.Append((const UTF8Char*)"\r\n");
	sbFieldOrder.Append((const UTF8Char*)"\tpublic static String[] getFieldOrder() {\r\n");
	sbFieldOrder.Append((const UTF8Char*)"\t\treturn new String[] {\r\n");

	sb->Append((const UTF8Char*)"{\r\n");
	DB::TableDef *tableDef = db->GetTableDef(tableName->v);
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
		DB::DBReader *r = db->GetTableData(tableName->v, 0, 0, 0, 0, 0);
		if (r)
		{
			DB::ColDef colDef((const UTF8Char*)"");
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

	sbConstrHdr.Append((const UTF8Char*)") {\r\n");
	sbConstrItem.Append((const UTF8Char*)"\t}\r\n");
	sbEquals.Append((const UTF8Char*)";\r\n");
	sbEquals.Append((const UTF8Char*)"\t}\r\n");
	sbHashCode.Append((const UTF8Char*)");\r\n");
	sbHashCode.Append((const UTF8Char*)"\t}\r\n");
	sbFieldOrder.Append((const UTF8Char*)"\t\t};\r\n");
	sbFieldOrder.Append((const UTF8Char*)"\t}\r\n");

	sb->AllocLeng(sbConstrHdr.GetLength() + sbConstrItem.GetLength() + sbGetterSetter.GetLength() + sbEquals.GetLength() + sbHashCode.GetLength() + sbFieldOrder.GetLength());
	sb->AppendC(sbConstrHdr.ToString(), sbConstrHdr.GetLength());
	sb->AppendC(sbConstrItem.ToString(), sbConstrItem.GetLength());
	sb->AppendC(sbGetterSetter.ToString(), sbGetterSetter.GetLength());
	sb->AppendC(sbEquals.ToString(), sbEquals.GetLength());
	sb->AppendC(sbHashCode.ToString(), sbHashCode.GetLength());
	sb->AppendC(sbFieldOrder.ToString(), sbFieldOrder.GetLength());
	sb->Append((const UTF8Char*)"}\r\n");
	clsName->Release();
	return true;
}
