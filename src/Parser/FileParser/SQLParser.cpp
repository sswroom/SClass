#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/SQL/SQLComment.h"
#include "DB/SQL/SQLFile.h"
#include "DB/SQL/SQLMetaCommand.h"
#include "IO/StreamDataStream.h"
#include "Parser/FileParser/SQLParser.h"
#include "Text/UTF8Reader.h"

Parser::FileParser::SQLParser::SQLParser()
{
	this->defSQLType = DB::SQLType::Unknown;
}

Parser::FileParser::SQLParser::~SQLParser()
{
}

Int32 Parser::FileParser::SQLParser::GetName()
{
	return *(Int32*)"SQLP";
}

void Parser::FileParser::SQLParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::SQLFile)
	{
		selector->AddFilter(CSTR("*.sql"), CSTR("SQL File"));
	}
}

IO::ParserType Parser::FileParser::SQLParser::GetParserType()
{
	return IO::ParserType::SQLFile;
}

Optional<IO::ParsedObject> Parser::FileParser::SQLParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	if (!fd->GetFullFileName()->EndsWithICase(UTF8STRC(".sql")))
	{
		return nullptr;
	}
	NN<DB::SQL::SQLFile> sqlFile;
	NEW_CLASSNN(sqlFile, DB::SQL::SQLFile(fd->GetFullFileName()));
	IO::StreamDataStream stm(fd);
	Text::UTF8Reader reader(stm);
	NN<DB::SQL::SQLCommand> sql;
	DB::SQLType sqlType = this->defSQLType;
	Text::StringBuilderUTF8 sb;
	while (reader.ReadLine(sb, 4096))
	{
		sb.Trim();
		if (sb.GetLength() == 0)
		{
		}
		else if (sb.StartsWith(CSTR("--")))
		{
			if (sb.Equals(CSTR("-- PostgreSQL database dump")))
			{
				sqlType = DB::SQLType::PostgreSQL;
			}
			else if (sb.Equals(CSTR("-- MySQL dump")))
			{
				sqlType = DB::SQLType::MySQL;
			}
			NEW_CLASSNN(sql, DB::SQL::SQLComment(sb.ToCString().Substring(2), DB::SQL::SQLComment::CommentType::DoubleDash));
			sqlFile->AddSQL(sql);
			sb.ClearStr();
		}
		else if (sb.StartsWith('#') && sqlType == DB::SQLType::MySQL)
		{
			NEW_CLASSNN(sql, DB::SQL::SQLComment(sb.ToCString().Substring(1), DB::SQL::SQLComment::CommentType::Sharp));
			sqlFile->AddSQL(sql);
			sb.ClearStr();
		}
		else if (sb.StartsWith('\\') && sqlType == DB::SQLType::PostgreSQL)
		{
			NEW_CLASSNN(sql, DB::SQL::SQLMetaCommand(sb.ToCString().Substring(1)));
			sqlFile->AddSQL(sql);
			sb.ClearStr();
		}
		else
		{
			if (DB::SQL::SQLCommand::Parse(sb.v, sqlType).SetTo(sql))
			{
				sqlFile->AddSQL(sql);
				sb.ClearStr();
			}
			else
			{
				printf("SQLParser: Unknown SQL command: %s\r\n", sb.ToPtr());
				sqlFile.Delete();
				return nullptr;
			}
		}
	}
	return sqlFile;
}

void Parser::FileParser::SQLParser::SetDefaultSQLType(DB::SQLType sqlType)
{
	this->defSQLType = sqlType;
}
