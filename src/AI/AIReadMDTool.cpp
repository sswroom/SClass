#include "Stdafx.h"
#include "AI/AIReadMDTool.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Text/JSONBuilder.h"
#include "Text/MyString.h"

void AI::AIReadMDTool::AppendDirectoryPath(NN<Text::StringBuilderUTF8> sb, NN<Text::String> directory)
{
	sb->Append(directory);
	if (directory->leng > 0 && directory->v[directory->leng - 1] != '/' && directory->v[directory->leng - 1] != '\\')
	{
		sb->AppendUTF8Char(IO::Path::PATH_SEPERATOR);
	}
}

Bool AI::AIReadMDTool::IsMarkdownFileName(UnsafeArray<const UTF8Char> fileName, UIntOS fileNameLen)
{
	return fileNameLen > 3 &&
		Text::StrEndsWithC(fileName, fileNameLen, UTF8STRC(".md")) &&
		Text::StrIndexOfCharC(fileName, fileNameLen, '/') == INVALID_INDEX &&
		Text::StrIndexOfCharC(fileName, fileNameLen, '\\') == INVALID_INDEX &&
		Text::StrIndexOfCharC(fileName, fileNameLen, ':') == INVALID_INDEX;
}

AI::AIReadMDTool::AIReadMDTool(Text::CStringNN directory) : directory(Text::String::New(directory))
{
}

AI::AIReadMDTool::~AIReadMDTool()
{
	this->directory->Release();
}

Text::CStringNN AI::AIReadMDTool::GetName() const
{
	return CSTR("read_markdown");
}

void AI::AIReadMDTool::GetDescription(NN<Text::StringBuilderUTF8> sb) const
{
	sb->AppendC(UTF8STRC("List, search, and read Markdown files. Use action list_files to list available *.md files, search with a keyword to find matching lines, or read_file with fileName to read a file. Search is case-insensitive. Optional lineNumber (1-based) and count limit the read to a range of lines."));
}

void AI::AIReadMDTool::GetParameters(NN<Net::OpenAIToolParameters> parameters) const
{
	parameters->AddParameter(CSTR("action"), CSTR("list_files, search, or read_file"), Net::OpenAIToolParameterType::String, true);
	parameters->AddParameter(CSTR("fileName"), CSTR("Markdown filename in the configured directory; required for read_file"), Net::OpenAIToolParameterType::String, false);
	parameters->AddParameter(CSTR("keyword"), CSTR("Keyword to search for in Markdown files; required for search"), Net::OpenAIToolParameterType::String, false);
	parameters->AddParameter(CSTR("lineNumber"), CSTR("1-based first line to read; defaults to 1"), Net::OpenAIToolParameterType::Integer, false);
	parameters->AddParameter(CSTR("count"), CSTR("Maximum number of lines to read; defaults to all remaining lines"), Net::OpenAIToolParameterType::Integer, false);
}

Bool AI::AIReadMDTool::Invoke(NN<Data::VariObject> input, NN<Data::VariObject> output)
{
	if (this->directory->leng == 0 || IO::Path::GetPathType(this->directory->ToCString()) != IO::Path::PathType::Directory)
	{
		printf("AIReadMDTool: configured directory is not available\n");
		return false;
	}

	NN<Data::VariItem> actionItem;
	NN<Text::String> action;
	if (!input->GetItem(U8STR("action")).SetTo(actionItem) || !actionItem->GetAsNewString().SetTo(action))
	{
		printf("AIReadMDTool: action not specified or invalid\n");
		return false;
	}
	Bool listFiles = action->Equals(UTF8STRC("list_files"));
	Bool search = action->Equals(UTF8STRC("search"));
	Bool readFile = action->Equals(UTF8STRC("read_file"));
	action->Release();

	if (listFiles)
	{
		Text::StringBuilderUTF8 searchPath;
		AppendDirectoryPath(searchPath, this->directory);
		searchPath.AppendC(UTF8STRC("*.md"));

		NN<IO::Path::FindFileSession> session;
		if (!IO::Path::FindFile(searchPath.ToCString()).SetTo(session))
		{
			printf("AIReadMDTool: failed to list Markdown files\n");
			return false;
		}

		Text::JSONBuilder json(Text::JSONBuilder::OT_ARRAY);
		UTF8Char fileName[4096];
		UnsafeArray<UTF8Char> fileNameEnd;
		IO::Path::PathType pathType;
		while (IO::Path::FindNextFile(fileName, session, 0, pathType, 0).SetTo(fileNameEnd))
		{
			if (pathType == IO::Path::PathType::File && Text::StrEndsWithC(fileName, (UIntOS)(fileNameEnd - fileName), UTF8STRC(".md")))
			{
				json.ArrayAddStr(CSTRP(fileName, fileNameEnd));
			}
		}
		IO::Path::FindFileClose(session);

		NN<Text::String> data = Text::String::New(json.Build());
		output->SetItemStr(U8STR("data"), data);
		data->Release();
		return true;
	}

	if (search)
	{
		NN<Data::VariItem> keywordItem;
		NN<Text::String> keyword;
		if (!input->GetItem(U8STR("keyword")).SetTo(keywordItem) || !keywordItem->GetAsNewString().SetTo(keyword) || keyword->leng == 0)
		{
			printf("AIReadMDTool: keyword is missing or invalid\n");
			return false;
		}

		Text::StringBuilderUTF8 searchPath;
		AppendDirectoryPath(searchPath, this->directory);
		searchPath.AppendC(UTF8STRC("*.md"));

		NN<IO::Path::FindFileSession> session;
		if (!IO::Path::FindFile(searchPath.ToCString()).SetTo(session))
		{
			keyword->Release();
			printf("AIReadMDTool: failed to search Markdown files\n");
			return false;
		}

		Text::JSONBuilder json(Text::JSONBuilder::OT_ARRAY);
		UTF8Char fileNameBuff[4096];
		UnsafeArray<UTF8Char> fileNameEnd;
		IO::Path::PathType pathType;
		while (IO::Path::FindNextFile(fileNameBuff, session, 0, pathType, 0).SetTo(fileNameEnd))
		{
			if (pathType != IO::Path::PathType::File ||
				!Text::StrEndsWithC(fileNameBuff, (UIntOS)(fileNameEnd - fileNameBuff), UTF8STRC(".md")))
			{
				continue;
			}

			Text::StringBuilderUTF8 filePath;
			AppendDirectoryPath(filePath, this->directory);
			filePath.AppendC(fileNameBuff, (UIntOS)(fileNameEnd - fileNameBuff));
			IO::FileStream fs(filePath.ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			if (fs.IsError())
			{
				IO::Path::FindFileClose(session);
				keyword->Release();
				printf("AIReadMDTool: failed to open Markdown file during search\n");
				return false;
			}

			Text::UTF8Reader reader(fs);
			Text::StringBuilderUTF8 line;
			Text::StringBuilderUTF8 linePart;
			UIntOS lineNumber = 1;
			while (reader.ReadLine(linePart, 8192))
			{
				line.Append(linePart.ToCString());
				linePart.ClearStr();
				if (reader.IsLineBreak())
				{
					if (Text::StrIndexOfICase(line.ToString(), keyword->v) != INVALID_INDEX)
					{
						json.ArrayBeginObject();
						json.ObjectAddStr(CSTR("fileName"), CSTRP(fileNameBuff, fileNameEnd));
						json.ObjectAddUInt64(CSTR("lineNumber"), lineNumber);
						json.ObjectAddStr(CSTR("line"), line.ToCString());
						json.ObjectEnd();
					}
					line.ClearStr();
					lineNumber++;
				}
			}
			if (line.GetLength() > 0 && Text::StrIndexOfICase(line.ToString(), keyword->v) != INVALID_INDEX)
			{
				json.ArrayBeginObject();
				json.ObjectAddStr(CSTR("fileName"), CSTRP(fileNameBuff, fileNameEnd));
				json.ObjectAddUInt64(CSTR("lineNumber"), lineNumber);
				json.ObjectAddStr(CSTR("line"), line.ToCString());
				json.ObjectEnd();
			}
			if (fs.IsError())
			{
				IO::Path::FindFileClose(session);
				keyword->Release();
				printf("AIReadMDTool: failed to read Markdown file during search\n");
				return false;
			}
		}
		IO::Path::FindFileClose(session);
		keyword->Release();

		NN<Text::String> data = Text::String::New(json.Build());
		output->SetItemStr(U8STR("data"), data);
		data->Release();
		return true;
	}

	if (!readFile)
	{
		printf("AIReadMDTool: action is invalid\n");
		return false;
	}

	NN<Data::VariItem> fileNameItem;
	NN<Text::String> fileName;
	if (!input->GetItem(U8STR("fileName")).SetTo(fileNameItem) || !fileNameItem->GetAsNewString().SetTo(fileName))
	{
		printf("AIReadMDTool: fileName is missing or invalid\n");
		return false;
	}
	if (!IsMarkdownFileName(fileName->v, fileName->leng))
	{
		fileName->Release();
		printf("AIReadMDTool: fileName is missing or invalid\n");
		return false;
	}

	Text::StringBuilderUTF8 filePath;
	AppendDirectoryPath(filePath, this->directory);
	filePath.Append(fileName);
	fileName->Release();
	if (IO::Path::GetPathType(filePath.ToCString()) != IO::Path::PathType::File)
	{
		printf("AIReadMDTool: Markdown file not found\n");
		return false;
	}

	UIntOS startLine;
	UIntOS lineCount;
	Optional<Data::VariItem> lineCountItem = input->GetItem(U8STR("count"));
	Bool hasCount = !lineCountItem.IsNull();
	if (!GetUIntOSInput(input, U8STR("lineNumber"), 1, startLine) || startLine == 0 ||
		!GetUIntOSInput(input, U8STR("count"), 0, lineCount) || (hasCount && lineCount == 0))
	{
		printf("AIReadMDTool: invalid lineNumber or count\n");
		return false;
	}

	IO::FileStream fs(filePath.ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
	{
		printf("AIReadMDTool: failed to open Markdown file\n");
		return false;
	}

	Text::UTF8Reader reader(fs);
	Text::StringBuilderUTF8 content;
	UIntOS currentLine = 1;
	while (reader.ReadLine(content, 8192))
	{
		if (currentLine < startLine)
		{
			content.ClearStr();
			if (reader.IsLineBreak())
				currentLine++;
			continue;
		}
		if (reader.IsLineBreak())
		{
			currentLine++;
			reader.GetLastLineBreak(content);
		}
		if (currentLine > startLine + lineCount)
			break;
	}
	if (fs.IsError())
	{
		printf("AIReadMDTool: failed to read Markdown file\n");
		return false;
	}

	NN<Text::String> data = Text::String::New(content.ToCString());
	output->SetItemStr(U8STR("data"), data);
	data->Release();
	return true;
}
