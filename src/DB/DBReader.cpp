#include "Stdafx.h"
#include "Data/DateTime.h"
#include "DB/DBReader.h"
#include "DB/TableDef.h"
#include "Text/MyString.h"

DB::TableDef *DB::DBReader::GenTableDef(const UTF8Char *tableName)
{
	DB::TableDef *table;
	DB::ColDef *colDef;
	UOSInt i;
	UOSInt j;
	NEW_CLASS(table, DB::TableDef(tableName));
	i = 0;
	j = this->ColCount();
	while (i < j)
	{
		NEW_CLASS(colDef, DB::ColDef((const UTF8Char*)""));
		this->GetColDef(i, colDef);
		table->AddCol(colDef);
		i++;
	}
	return table;
}
