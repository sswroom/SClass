#include "Stdafx.h"
#include "Text/MyString.h"
#include "Text/Cpp/CppParseStatus.h"

void Text::Cpp::CppParseStatus::FreeDefineInfo(NN<Text::Cpp::CppParseStatus::DefineInfo> defInfo)
{
	defInfo->defineName->Release();
	OPTSTR_DEL(defInfo->defineVal);
	OPTSTR_DEL(defInfo->defineParam);
	MemFreeNN(defInfo);
}

void Text::Cpp::CppParseStatus::FreeFileStatus(NN<FileParseStatus> fileStatus)
{
	fileStatus.Delete();
}

Text::Cpp::CppParseStatus::CppParseStatus(NN<Text::String> rootFile)
{
	this->fileName = rootFile->Clone();
}

Text::Cpp::CppParseStatus::CppParseStatus(Text::CStringNN rootFile)
{
	this->fileName = Text::String::New(rootFile);
}

Text::Cpp::CppParseStatus::~CppParseStatus()
{
	UOSInt i;
	this->fileName->Release();
	this->statuses.FreeAll(FreeFileStatus);
	i = this->fileNames.GetCount();
	while (i-- > 0)
	{
		this->fileNames.GetItem(i)->Release();
	}
	this->defines.FreeAll(FreeDefineInfo);
}

Optional<Text::Cpp::CppParseStatus::FileParseStatus> Text::Cpp::CppParseStatus::GetFileStatus()
{
	return this->statuses.GetItem(this->statuses.GetCount() - 1);
}

Bool Text::Cpp::CppParseStatus::BeginParseFile(Text::CStringNN fileName)
{
	NN<Text::String> fname;
	OSInt i = this->fileNames.SortedIndexOfPtr(fileName.v, fileName.leng);
	if (i >= 0)
	{
		fname = Text::String::OrEmpty(this->fileNames.GetItem((UOSInt)i));
	}
	else
	{
		fname = Text::String::New(fileName.v, fileName.leng);
		this->fileNames.Insert((UOSInt)~i, fname.Ptr());
	}

	NN<FileParseStatus> status;
	NEW_CLASSNN(status, FileParseStatus());
	this->statuses.Add(status);
	status->currMode = PM_NORMAL;
	status->fileName = fname;
	status->lineNum = 0;
	status->lineStart = true;
	status->modeStatus = 0;
	return true;
}

Bool Text::Cpp::CppParseStatus::EndParseFile(Text::CStringNN fileName)
{
	NN<FileParseStatus> status;
	UOSInt i = this->statuses.GetCount() - 1;
	if (!this->statuses.GetItem(i).SetTo(status))
		return false;
	if (!status->fileName->Equals(fileName))
	{
		return false;
	}
	Bool valid = true;
	if (status->ifValid.GetCount() > 0)
	{
		valid = false;
	}
	if (status->currMode != PM_NORMAL)
	{
		valid = false;
	}
	this->statuses.RemoveAt(i);
	FreeFileStatus(status);
	return valid;
}

Bool Text::Cpp::CppParseStatus::IsDefined(Text::CStringNN defName)
{
	NN<DefineInfo> defInfo;
	if (!this->defines.GetC(defName).SetTo(defInfo))
		return false;
	if (defInfo->undefined)
		return false;
	return true;
}

Bool Text::Cpp::CppParseStatus::AddGlobalDef(Text::CStringNN defName, Text::CString defVal)
{
	NN<DefineInfo> defInfo;
	if (this->defines.GetC(defName).SetTo(defInfo))
	{
		if (defInfo->undefined)
		{
			defInfo->fileName = 0;
			defInfo->lineNum = 0;
			OPTSTR_DEL(defInfo->defineVal);
			OPTSTR_DEL(defInfo->defineParam);
			defInfo->defineVal = Text::String::NewOrNull(defVal);
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
		defInfo = MemAllocNN(DefineInfo);
		defInfo->defineName = Text::String::New(defName);
		defInfo->fileName = 0;
		defInfo->lineNum = 0;
		defInfo->defineVal = Text::String::NewOrNull(defVal);
		defInfo->defineParam = 0;
		defInfo->undefined = false;
		this->defines.PutC(defName, defInfo);
		return true;
	}
}

Bool Text::Cpp::CppParseStatus::AddDef(Text::CStringNN defName, Text::CString defParam, Text::CString defVal, Int32 lineNum)
{
	NN<FileParseStatus> fStatus;
	NN<DefineInfo> defInfo;
	NN<Text::String> nns;
	if (!GetFileStatus().SetTo(fStatus))
		return false;
	if (this->defines.GetC(defName).SetTo(defInfo))
	{
		if (defInfo->undefined)
		{
			defInfo->fileName = fStatus->fileName.Ptr();
			defInfo->lineNum = fStatus->lineNum;
			OPTSTR_DEL(defInfo->defineVal);
			OPTSTR_DEL(defInfo->defineParam);
			defInfo->defineVal = Text::String::NewOrNull(defVal);
			if (defInfo->defineVal.SetTo(nns))
			{
				nns->Trim();
			}
			defInfo->defineParam = Text::String::NewOrNull(defParam);
			defInfo->undefined = false;
			return true;
		}
		else
		{
			if (defVal.leng == 0 && defInfo->defineVal.IsNull())
			{
				return true;
			}
			else if (defVal.leng == 0 || !defInfo->defineVal.SetTo(nns))
			{
				return false;
			}
			Text::StringBuilderUTF8 sb;
			sb.AppendOpt(defVal);
			sb.Trim();
			return nns->Equals(sb);
		}
	}
	else
	{
		defInfo = MemAllocNN(DefineInfo);
		defInfo->defineName = Text::String::New(defName);
		defInfo->fileName = fStatus->fileName.Ptr();
		defInfo->lineNum = fStatus->lineNum;
		defInfo->defineVal = Text::String::NewOrNull(defVal);
		if (defInfo->defineVal.SetTo(nns))
		{
			nns->Trim();
		}
		defInfo->defineParam = Text::String::NewOrNull(defParam);
		defInfo->undefined = false;
		this->defines.PutC(defName, defInfo);
		return true;
	}
}

Bool Text::Cpp::CppParseStatus::Undefine(Text::CStringNN defName)
{
	NN<DefineInfo> defInfo;
	if (this->defines.GetC(defName).SetTo(defInfo))
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

Bool Text::Cpp::CppParseStatus::GetDefineVal(Text::CStringNN defName, Text::CString defParam, NN<Text::StringBuilderUTF8> sbOut)
{
	NN<DefineInfo> defInfo;
	if (this->defines.GetC(defName).SetTo(defInfo))
	{
		if (!defInfo->undefined)
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendOpt(defInfo->defineVal);
			if (defInfo->defineParam != 0)
			{
				Text::StringBuilderUTF8 sb1;
				Text::StringBuilderUTF8 sb2;
				Text::StringBuilderUTF8 sb3;
				if (defParam.leng > 0)
				{
					Text::CStringNN defParamNN = defParam.OrEmpty();
					if (this->IsDefined(defParamNN))
					{
						this->GetDefineVal(defParamNN, CSTR_NULL, sb3);
					}
					else
					{
						sb3.Append(defParamNN);
					}
				}

				sb1.ClearStr();
				sb1.AppendC(UTF8STRC("##"));
				sb1.AppendOpt(defInfo->defineParam);
				sb2.ClearStr();
				sb2.AppendC(sb3.ToString(), sb3.GetLength());
				sb2.AppendC(UTF8STRC(" "));
				sb.ReplaceStr(sb1.ToString(), sb1.GetLength(), sb2.ToString(), sb2.GetLength());

				sb1.ClearStr();
				sb1.AppendOpt(defInfo->defineParam);
				sb1.AppendC(UTF8STRC("##"));
				sb2.ClearStr();
				sb2.AppendC(UTF8STRC(" "));
				sb2.AppendC(sb3.ToString(), sb3.GetLength());
				sb.ReplaceStr(sb1.ToString(), sb1.GetLength(), sb2.ToString(), sb2.GetLength());

				sb1.ClearStr();
				sb1.AppendOpt(defInfo->defineParam);
				sb2.ClearStr();
				sb2.AppendC(UTF8STRC(" "));
				sb2.AppendC(sb3.ToString(), sb3.GetLength());
				sb2.AppendC(UTF8STRC(" "));
				sb.ReplaceStr(sb1.ToString(), sb1.GetLength(), sb2.ToString(), sb2.GetLength());
			}
			sbOut->AppendC(sb.ToString(), sb.GetLength());
			return true;
		}
	}
	return false;

}

UOSInt Text::Cpp::CppParseStatus::GetDefineCount()
{
	return this->defines.GetCount();
}

Bool Text::Cpp::CppParseStatus::GetDefineInfo(UOSInt index, NN<DefineInfo> defInfo)
{
	NN<DefineInfo> def;
	if (!this->defines.GetItem(index).SetTo(def))
		return false;
	defInfo.CopyFrom(def);
	return true;
}

UOSInt Text::Cpp::CppParseStatus::GetFileCount()
{
	return this->fileNames.GetCount();
}

Text::String *Text::Cpp::CppParseStatus::GetFileName(UOSInt index)
{
	return this->fileNames.GetItem(index);
}

NN<Text::String> Text::Cpp::CppParseStatus::GetCurrCodeFile() const
{
	UOSInt i = this->statuses.GetCount();
	if (i > 0)
	{
		return this->statuses.GetItemNoCheck(i - 1)->fileName;
	}
	return this->fileName;
}
