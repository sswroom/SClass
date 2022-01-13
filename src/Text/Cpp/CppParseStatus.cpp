#include "Stdafx.h"
#include "Text/MyString.h"
#include "Text/Cpp/CppParseStatus.h"

void Text::Cpp::CppParseStatus::FreeDefineInfo(Text::Cpp::CppParseStatus::DefineInfo *defInfo)
{
	Text::StrDelNew(defInfo->defineName);
	SDEL_TEXT(defInfo->defineVal);
	SDEL_TEXT(defInfo->defineParam);
	MemFree(defInfo);
}

void Text::Cpp::CppParseStatus::FreeFileStatus(FileParseStatus *fileStatus)
{
	DEL_CLASS(fileStatus->lineBuffSB);
	DEL_CLASS(fileStatus->pastModes);
	DEL_CLASS(fileStatus->ifValid);
	MemFree(fileStatus);
}

Text::Cpp::CppParseStatus::CppParseStatus(Text::String *rootFile)
{
	this->fileName = rootFile->Clone();
	NEW_CLASS(this->defines, Data::StringUTF8Map<DefineInfo*>());
	NEW_CLASS(this->statuses, Data::ArrayList<FileParseStatus*>());
	NEW_CLASS(this->fileNames, Data::ArrayListICaseString());
}

Text::Cpp::CppParseStatus::CppParseStatus(const UTF8Char *rootFile)
{
	this->fileName = Text::String::NewNotNull(rootFile);
	NEW_CLASS(this->defines, Data::StringUTF8Map<DefineInfo*>());
	NEW_CLASS(this->statuses, Data::ArrayList<FileParseStatus*>());
	NEW_CLASS(this->fileNames, Data::ArrayListICaseString());
}

Text::Cpp::CppParseStatus::~CppParseStatus()
{
	UOSInt i;
	this->fileName->Release();
	i = this->statuses->GetCount();
	while (i-- > 0)
	{
		FreeFileStatus(this->statuses->GetItem(i));
	}
	DEL_CLASS(this->statuses);
	i = this->fileNames->GetCount();
	while (i-- > 0)
	{
		this->fileNames->GetItem(i)->Release();
	}
	DEL_CLASS(this->fileNames);
	Data::ArrayList<DefineInfo*> *defs = this->defines->GetValues();
	i = defs->GetCount();
	while (i-- > 0)
	{
		FreeDefineInfo(defs->GetItem(i));
	}
	DEL_CLASS(this->defines);
}

Text::Cpp::CppParseStatus::FileParseStatus *Text::Cpp::CppParseStatus::GetFileStatus()
{
	return this->statuses->GetItem(this->statuses->GetCount() - 1);
}

Bool Text::Cpp::CppParseStatus::BeginParseFile(const UTF8Char *fileName)
{
	Text::String *fname;
	OSInt i = this->fileNames->SortedIndexOfPtr(fileName);
	if (i >= 0)
	{
		fname = this->fileNames->GetItem((UOSInt)i);
	}
	else
	{
		fname = Text::String::NewNotNull(fileName);
		this->fileNames->Insert((UOSInt)~i, fname);
	}

	FileParseStatus *status = MemAlloc(FileParseStatus, 1);
	this->statuses->Add(status);
	status->currMode = PM_NORMAL;
	status->fileName = fname;
	status->lineNum = 0;
	status->lineStart = true;
	status->modeStatus = 0;
	NEW_CLASS(status->lineBuffSB, Text::StringBuilderUTF8());
	NEW_CLASS(status->pastModes, Data::ArrayList<ParserMode>());
	NEW_CLASS(status->ifValid, Data::ArrayList<Int32>());
	return true;
}

Bool Text::Cpp::CppParseStatus::EndParseFile(const UTF8Char *fileName, UOSInt fileNameLen)
{
	FileParseStatus *status;
	UOSInt i = this->statuses->GetCount() - 1;
	status = this->statuses->GetItem(i);
	if (status == 0)
		return false;
	if (!status->fileName->Equals(fileName, fileNameLen))
	{
		return false;
	}
	Bool valid = true;
	if (status->ifValid->GetCount() > 0)
	{
		valid = false;
	}
	if (status->currMode != PM_NORMAL)
	{
		valid = false;
	}
	this->statuses->RemoveAt(i);
	FreeFileStatus(status);
	return valid;
}

Bool Text::Cpp::CppParseStatus::IsDefined(const UTF8Char *defName)
{
	DefineInfo *defInfo = this->defines->Get(defName);
	if (defInfo == 0)
		return false;
	if (defInfo->undefined)
		return false;
	return true;
}

Bool Text::Cpp::CppParseStatus::AddGlobalDef(const UTF8Char *defName, const UTF8Char *defVal)
{
	DefineInfo *defInfo;
	defInfo = this->defines->Get(defName);
	if (defInfo)
	{
		if (defInfo->undefined)
		{
			defInfo->fileName = 0;
			defInfo->lineNum = 0;
			SDEL_TEXT(defInfo->defineVal);
			SDEL_TEXT(defInfo->defineParam);
			if (defVal)
			{
				defInfo->defineVal = Text::StrCopyNew(defVal);
			}
			else
			{
				defInfo->defineVal = 0;
			}
			defInfo->undefined = false;
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		defInfo = MemAlloc(DefineInfo, 1);
		defInfo->defineName = Text::StrCopyNew(defName);
		defInfo->fileName = 0;
		defInfo->lineNum = 0;
		if (defVal)
		{
			defInfo->defineVal = Text::StrCopyNew(defVal);
		}
		else
		{
			defInfo->defineVal = 0;
		}
		defInfo->defineParam = 0;
		defInfo->undefined = false;
		this->defines->Put(defName, defInfo);
		return true;
	}
}

Bool Text::Cpp::CppParseStatus::AddDef(const UTF8Char *defName, const UTF8Char *defParam, const UTF8Char *defVal, Int32 lineNum)
{
	FileParseStatus *fStatus = GetFileStatus();
	DefineInfo *defInfo;
	if (Text::StrEquals(defName, (const UTF8Char*)"MF_END"))
	{
		defName = (const UTF8Char*)"MF_END";
	}
	defInfo = this->defines->Get(defName);
	if (defInfo)
	{
		if (defInfo->undefined)
		{
			defInfo->fileName = fStatus->fileName;
			defInfo->lineNum = fStatus->lineNum;
			SDEL_TEXT(defInfo->defineVal);
			SDEL_TEXT(defInfo->defineParam);
			if (defVal)
			{
				defInfo->defineVal = Text::StrCopyNew(defVal);
				Text::StrTrim((UTF8Char*)defInfo->defineVal);
			}
			else
			{
				defInfo->defineVal = 0;
			}
			if (defParam)
			{
				defInfo->defineParam = Text::StrCopyNew(defParam);
			}
			else
			{
				defInfo->defineParam = 0;
			}
			defInfo->undefined = false;
			return true;
		}
		else
		{
			if (defVal == 0 && defInfo->defineVal == 0)
			{
				return true;
			}
			else if (defVal == 0 || defInfo->defineVal == 0)
			{
				return false;
			}
			Text::StringBuilderUTF8 sb;
			sb.Append(defVal);
			sb.Trim();
			return sb.Equals(defInfo->defineVal);
		}
	}
	else
	{
		defInfo = MemAlloc(DefineInfo, 1);
		defInfo->defineName = Text::StrCopyNew(defName);
		defInfo->fileName = fStatus->fileName;
		defInfo->lineNum = fStatus->lineNum;
		if (defVal)
		{
			defInfo->defineVal = Text::StrCopyNew(defVal);
			Text::StrTrim((UTF8Char*)defInfo->defineVal);
		}
		else
		{
			defInfo->defineVal = 0;
		}
		if (defParam)
		{
			defInfo->defineParam = Text::StrCopyNew(defParam);
		}
		else
		{
			defInfo->defineParam = 0;
		}
		defInfo->undefined = false;
		this->defines->Put(defName, defInfo);
		return true;
	}
}

Bool Text::Cpp::CppParseStatus::Undefine(const UTF8Char *defName)
{
	DefineInfo *defInfo = this->defines->Get(defName);
	if (defInfo)
	{
		if (!defInfo->undefined)
		{
			defInfo->undefined = true;
			return true;
		}
		return false;
	}
	return false;
}

Bool Text::Cpp::CppParseStatus::GetDefineVal(const UTF8Char *defName, const UTF8Char *defParam, Text::StringBuilderUTF *sbOut)
{
	DefineInfo *defInfo = this->defines->Get(defName);
	if (defInfo)
	{
		if (!defInfo->undefined)
		{
			Text::StringBuilderUTF8 sb;
			if (defInfo->defineVal)
			{
				sb.Append(defInfo->defineVal);
			}
			if (defInfo->defineParam != 0)
			{
				Text::StringBuilderUTF8 sb1;
				Text::StringBuilderUTF8 sb2;
				Text::StringBuilderUTF8 sb3;
				if (defParam)
				{
					if (this->IsDefined(defParam))
					{
						this->GetDefineVal(defParam, 0, &sb3);
					}
					else
					{
						sb3.Append(defParam);
					}
				}

				sb1.ClearStr();
				sb1.AppendC(UTF8STRC("##"));
				sb1.Append(defInfo->defineParam);
				sb2.ClearStr();
				sb2.AppendC(sb3.ToString(), sb3.GetLength());
				sb2.AppendC(UTF8STRC(" "));
				sb.Replace(sb1.ToString(), sb2.ToString());

				sb1.ClearStr();
				sb1.Append(defInfo->defineParam);
				sb1.AppendC(UTF8STRC("##"));
				sb2.ClearStr();
				sb2.AppendC(UTF8STRC(" "));
				sb2.AppendC(sb3.ToString(), sb3.GetLength());
				sb.Replace(sb1.ToString(), sb2.ToString());

				sb1.ClearStr();
				sb1.Append(defInfo->defineParam);
				sb2.ClearStr();
				sb2.AppendC(UTF8STRC(" "));
				sb2.AppendC(sb3.ToString(), sb3.GetLength());
				sb2.AppendC(UTF8STRC(" "));
				sb.Replace(sb1.ToString(), sb2.ToString());
			}
			sbOut->AppendC(sb.ToString(), sb.GetLength());
			return true;
		}
	}
	return false;

}

UOSInt Text::Cpp::CppParseStatus::GetDefineCount()
{
	return this->defines->GetCount();
}

Bool Text::Cpp::CppParseStatus::GetDefineInfo(UOSInt index, DefineInfo *defInfo)
{
	DefineInfo *def = this->defines->GetValues()->GetItem(index);
	if (def == 0)
		return false;
	MemCopyNO(defInfo, def, sizeof(DefineInfo));
	return true;
}

UOSInt Text::Cpp::CppParseStatus::GetFileCount()
{
	return this->fileNames->GetCount();
}

Text::String *Text::Cpp::CppParseStatus::GetFileName(UOSInt index)
{
	return this->fileNames->GetItem(index);
}

Text::String *Text::Cpp::CppParseStatus::GetCurrCodeFile()
{
	UOSInt i = this->statuses->GetCount();
	if (i > 0)
	{
		return this->statuses->GetItem(i - 1)->fileName;
	}
	return this->fileName;
}
