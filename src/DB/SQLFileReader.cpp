#include "Stdafx.h"
#include "DB/SQLFileReader.h"

DB::SQLFileReader::SQLFileReader(NN<IO::Stream> stm, DB::SQLType sqlType, Bool mergeInsert) : reader(stm)
{
	this->sqlType = sqlType;
	this->mergeInsert = mergeInsert;
}

DB::SQLFileReader::~SQLFileReader()
{
}

Bool DB::SQLFileReader::NextSQL(NN<Text::StringBuilderUTF8> sb)
{
	if (this->mergeInsert)
	{
		if (this->sbLastLine.leng > 0)
		{
			sb->Append(this->sbLastLine);
			this->sbLastLine.ClearStr();
		}
		else
		{
			if (!this->reader.ReadLine(sb, 4096))
			{
				return false;
			}
			if (sb->EndsWith(';'))
			{
				sb->RemoveChars(1);
			}
			else
			{
				this->reader.GetLastLineBreak(sb);
				while (this->reader.ReadLine(sb, 4096))
				{
					if (sb->EndsWith(';'))
					{
						sb->RemoveChars(1);
						break;
					}
					this->reader.GetLastLineBreak(sb);
				}
			}
		}
		if (!sb->StartsWithICase(UTF8STRC("INSERT INTO ")))
		{
			return true;
		}
		UIntOS i = sb->IndexOfICase(UTF8STRC(") VALUES ("));
		if (i == INVALID_INDEX)
		{
			return true;
		}
		UIntOS insertCnt = 1;
		while (true)
		{
			if (!this->reader.ReadLine(this->sbLastLine, 4096))
			{
				return true;
			}
			while(!this->sbLastLine.EndsWith(';'))
			{
				this->reader.GetLastLineBreak(this->sbLastLine);
				if (!this->reader.ReadLine(this->sbLastLine, 4096))
					break;
			}
			if (this->sbLastLine.EndsWith(';'))
			{
				this->sbLastLine.RemoveChars(1);
			}
			if (this->sbLastLine.leng > i + 10 && this->sbLastLine.StartsWith(sb->v, i + 10))
			{
				sb->AppendUTF8Char(',');
				sb->Append(this->sbLastLine.Substring(i + 9));
				this->sbLastLine.ClearStr();
				insertCnt++;
				if (insertCnt >= 250)
				{
					return true;
				}
			}
			else
			{
				return true;
			}
		}
	}
	else
	{
		if (!this->reader.ReadLine(sb, 4096))
		{
			return false;
		}
		if (sb->EndsWith(';'))
		{
			sb->RemoveChars(1);
			return true;
		}
		this->reader.GetLastLineBreak(sb);
		while (this->reader.ReadLine(sb, 4096))
		{
			if (sb->EndsWith(';'))
			{
				sb->RemoveChars(1);
				return true;
			}
			this->reader.GetLastLineBreak(sb);
		}
		return true;
	}
}
