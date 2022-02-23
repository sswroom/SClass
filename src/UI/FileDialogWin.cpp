#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "IO/Path.h"
#include "Text/StringBuilderUTF16.h"
#include "UI/FileDialog.h"
#include <windows.h>
#include <commdlg.h>

#define MAXFILENAMESIZE 512

/*		IO::Registry *reg;
		WChar *dialogName;
		WChar *lastName;
		WChar *fileName;
		Data::ArrayList *patterns;
		Data::ArrayList *names;*/

void UI::FileDialog::ClearFileNames()
{
	UOSInt i;
	i = this->fileNames->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->fileNames->RemoveAt(i));
	}
}

UI::FileDialog::FileDialog(const WChar *compName, const WChar *appName, const WChar *dialogName, Bool isSave)
{
	UOSInt i;
	WChar buff[256];
	WChar *sptr;
	this->reg = IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, compName, appName);
	this->isSave = isSave;
	this->filterIndex = (UOSInt)-1;
	this->allowMulti = false;
	i = Text::StrCharCnt(dialogName);
	this->dialogName = MemAlloc(WChar, i + 7);
	sptr = Text::StrConcat(this->dialogName, dialogName);
	sptr = Text::StrConcat(sptr, L"Dialog");

	this->fileName = 0;
	this->lastName = 0;
	sptr = this->reg->GetValueStr(this->dialogName, buff);
	if (sptr)
	{
		this->lastName = Text::StrCopyNew(buff);
	}
	NEW_CLASS(this->fileNames, Data::ArrayListStrUTF8());
	NEW_CLASS(this->names, Data::ArrayListString());
	NEW_CLASS(this->patterns, Data::ArrayListString());
}

UI::FileDialog::~FileDialog()
{
	UOSInt i;
	IO::Registry::CloseRegistry(this->reg);
	MemFree(this->dialogName);
	if (this->lastName)
	{
		Text::StrDelNew(this->lastName);
		this->lastName = 0;
	}
	if (this->fileName)
	{
		this->fileName->Release();
		this->fileName = 0;
	}
	i = this->patterns->GetCount();
	while (i-- > 0)
	{
		this->patterns->RemoveAt(i)->Release();
		this->names->RemoveAt(i)->Release();
	}
	this->ClearFileNames();
	DEL_CLASS(this->fileNames);
	DEL_CLASS(this->patterns);
	DEL_CLASS(this->names);
}

void UI::FileDialog::AddFilter(Text::CString pattern, Text::CString name)
{
	this->patterns->Add(Text::String::New(pattern));
	this->names->Add(Text::String::New(name));
}

UOSInt UI::FileDialog::GetFilterIndex()
{
	return this->filterIndex;
}

void UI::FileDialog::SetFileName(Text::CString fileName)
{
	if (this->fileName)
	{
		this->fileName->Release();
		this->fileName = 0;
	}
	this->fileName = Text::String::New(fileName);
}

Text::String *UI::FileDialog::GetFileName()
{
	return this->fileName;
}

UOSInt UI::FileDialog::GetFileNameCount()
{
	UOSInt cnt = this->fileNames->GetCount();
	if (cnt)
		return cnt;
	if (this->fileName)
		return 1;
	return 0;
}

const UTF8Char *UI::FileDialog::GetFileNames(UOSInt index)
{
	if (index == 0 && this->fileNames->GetCount() == 0)
		return STR_PTR(this->fileName);
	return this->fileNames->GetItem(index);
}

void UI::FileDialog::SetAllowMultiSel(Bool allowMulti)
{
	this->allowMulti = allowMulti;
}

Bool UI::FileDialog::ShowDialog(ControlHandle *ownerHandle)
{
	WChar fname1[512];
	WChar fname2[512];
	WChar fname[MAXFILENAMESIZE];
	WChar *sptr;
	WChar *dptr;
	const WChar *initDir;
	WChar *initFileName;
	WChar *multiBuff = 0;
	WChar *fnameBuff;
	OSInt fnameBuffSize;
	Text::StringBuilderUTF16 sb;
	UOSInt i = 0;
	UOSInt filterCnt = this->names->GetCount();

	OPENFILENAMEW ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAMEW);
	ofn.hwndOwner = 0;//hwnd;
	ofn.hInstance = 0;

	if (!isSave && filterCnt > 0)
	{
		sb.AppendC(UTF8STRC("Supported Files"));
		sb.AppendC((const UTF8Char*)"\0", 1);
		while (i < filterCnt)
		{
			if (i > 0)
			{
				sb.AppendC(UTF8STRC(";"));
			}
			sb.Append(this->patterns->GetItem(i++));
		}
		sb.AppendC((const UTF8Char*)"\0", 1);
	}

	i = 0;
	while (i < filterCnt)
	{
		sb.Append(this->names->GetItem(i));
		sb.AppendC(UTF8STRC(" ("));
		sb.Append(this->patterns->GetItem(i));
		sb.AppendC((const UTF8Char*)")\0", 2);
		sb.Append(this->patterns->GetItem(i++));
		sb.AppendC((const UTF8Char*)"\0", 1);
	}
	if (!this->isSave)
	{
		sb.AppendC(UTF8STRC("All Files (*.*)"));
		sb.AppendC((const UTF8Char*)"\0", 1);
		sb.AppendC(UTF8STRC("*.*"));
		sb.AppendC((const UTF8Char*)"\0", 1);
	}
	if (this->allowMulti && !this->isSave)
	{
		multiBuff = fnameBuff = MemAlloc(WChar, 65536);
		fnameBuffSize = 65536;
	}
	else
	{
		fnameBuff = fname;
		fnameBuffSize = MAXFILENAMESIZE;
	}
	if (this->fileName)
	{
		Text::StrUTF8_WChar(fname2, this->fileName->v, 0);
		Text::StrReplace(fname2, '/', '_');
		Text::StrReplace(&fname2[2], ':', '_');
		Text::StrConcat(fnameBuff, fname2);

		i = Text::StrLastIndexOfChar(fname2, '\\');
		if (i != INVALID_INDEX)
		{
			fname2[i] = 0;
			initFileName = &fname2[i + 1];
			initDir = fname2;
			if (initFileName[0] == 0)
			{
				Text::StrConcat(fnameBuff, L"");
			}
		}
		else
		{
			initFileName = fname2;
			initDir = L".";
		}
		i = Text::StrLastIndexOfChar(initFileName, '.');
		if (i != INVALID_INDEX)
		{
			initFileName[i] = 0;
		}
	}
	else
	{
		fname2[0] = 0;
		fnameBuff[0] = 0;
		initDir = L".";
		initFileName = (WChar*)(initDir + 1);
	}

	if (this->lastName)
	{
		Text::StrConcat(fname1, this->lastName);
		WChar *sptr = fnameBuff;
		WChar *currPtr = fname1;
		WChar *ptrStart = 0;
		WChar c;
		while ((c = *currPtr) != 0)
		{
			if (c == '|')
			{
				if (ptrStart)
				{
					*currPtr = 0;
					sptr = Text::StrConcat(sptr, ptrStart);
					ptrStart = 0;
				}
				c = *++currPtr;
				if (c == 'd')
				{
					sptr = Text::StrConcat(sptr, initDir);
				}
				else if (c == 'n')
				{
					sptr = Text::StrConcat(sptr, initFileName);
				}
			}
			else if (ptrStart == 0)
			{
				ptrStart = currPtr;
			}
			currPtr++;
		}
		if (ptrStart)
		{
			Text::StrConcat(sptr, ptrStart);
		}
	}


	ofn.lpstrFilter = sb.ToString();
	ofn.lpstrCustomFilter = 0; 
	ofn.nMaxCustFilter = 0;
	if (this->filterIndex == (UOSInt)-1)
	{
		if (this->isSave && fnameBuff && fnameBuff[0] != 0)
		{
			Bool found = false;
			UOSInt foundIndexLeng = 0;
			Text::String *u8fname = Text::String::NewNotNull(fnameBuff);
			i = 0;
			while (i < filterCnt)
			{
				Text::String *pattern = this->patterns->GetItem(i);
				if (IO::Path::FilePathMatch(u8fname->v, u8fname->leng, pattern->v, pattern->leng))
				{
					if (!found)
					{
						found = true;
						ofn.nFilterIndex = (UInt32)(i + 1);
						foundIndexLeng = pattern->leng;
					}
					else if (pattern->leng > foundIndexLeng)
					{
						ofn.nFilterIndex = (UInt32)(i + 1);
						foundIndexLeng = pattern->leng;
					}
				}
				i++;
			}
			u8fname->Release();
			if (!found)
			{
				ofn.nFilterIndex = 1;
			}
		}
		else
		{
			ofn.nFilterIndex = 1;
		}
	}
	else
	{
		ofn.nFilterIndex = (UInt32)(this->filterIndex + 1);
	}
	ofn.lpstrFile = fnameBuff;
	ofn.nMaxFile = (UInt32)fnameBuffSize;
	ofn.lpstrInitialDir = initDir;
	ofn.lpstrFileTitle = 0;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrTitle = 0;
	ofn.Flags = (isSave? OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST : OFN_FILEMUSTEXIST | OFN_HIDEREADONLY);
	ofn.nFileOffset = 0; 
	ofn.nFileExtension = 0; 
	ofn.lpstrDefExt = 0; 
	ofn.lCustData = 0;
	ofn.lpfnHook = 0; 
	ofn.lpTemplateName = 0;
	ofn.hwndOwner = 0;//(HWND)ownerHandle;
	if (!this->isSave && this->allowMulti)
	{
		ofn.Flags |= OFN_ALLOWMULTISELECT | OFN_EXPLORER;
	}

	Bool ret;
	if (isSave)
		ret = GetSaveFileNameW(&ofn) != 0;
	else
		ret = GetOpenFileNameW(&ofn) != 0;
	if (ret)
	{
		Bool toSave = true;
		this->ClearFileNames();
		if (this->fileName)
		{
			this->fileName->Release();
			this->fileName = 0;
		}
		this->filterIndex = ofn.nFilterIndex - 1;
		if (isSave && ofn.nFileExtension == 0)
		{
			Text::String *pattern = this->patterns->GetItem(this->filterIndex);
			if (pattern && Text::StrStartsWithC(pattern->v, pattern->leng, UTF8STRC("*.")))
			{
				Text::StrUTF8_WChar(&fnameBuff[Text::StrCharCnt(fnameBuff)], &pattern->v[1], 0);
			}
			this->fileName = Text::String::NewNotNull(fnameBuff);
		}
		else if (!isSave && this->allowMulti)
		{
			Text::String *s = Text::String::NewNotNull(fnameBuff);
			IO::Path::PathType pt = IO::Path::GetPathType(s->ToCString());
			s->Release();
			if (pt == IO::Path::PathType::Directory)
			{
				dptr = Text::StrConcat(fname, fnameBuff);
				sptr = &fnameBuff[Text::StrCharCnt(fnameBuff) + 1];
				*dptr++ = IO::Path::PATH_SEPERATOR;
				
				i = 0;
				while (*sptr)
				{
					Text::StrConcat(dptr, sptr);
					this->fileNames->Add(Text::StrToUTF8New(fname));
					sptr = &sptr[Text::StrCharCnt(sptr) + 1];
					i++;
				}
				if (i == 1)
				{
					this->fileName = Text::String::NewNotNullSlow(this->fileNames->GetItem(0));
				}
				else
				{
					this->fileName = Text::String::NewNotNull(fnameBuff);
					toSave = false;
				}
			}
			else
			{
				this->fileName = Text::String::NewNotNull(fnameBuff);
			}
		}
		else
		{
			this->fileName = Text::String::NewNotNull(fnameBuff);
		}

		if (toSave)
		{
			WChar *currPtr;
			if (Text::StrStartsWith(fnameBuff, initDir))
			{
				sptr = Text::StrConcat(fname1, L"|d");
				currPtr = &fnameBuff[Text::StrCharCnt(initDir)];
			}
			else
			{
				sptr = fname1;
				currPtr = fnameBuff;
			}
			if (initFileName[0])
			{
				i = Text::StrIndexOf(currPtr, initFileName);
				if (i >= 0)
				{
					currPtr[i] = 0;
					sptr = Text::StrConcat(sptr, currPtr);
					sptr = Text::StrConcat(sptr, L"|n");
					Text::StrConcat(sptr, &currPtr[i + Text::StrCharCnt(initFileName)]);
				}
				else
				{
					Text::StrConcat(sptr, currPtr);
				}
			}
			else
			{
				Text::StrConcat(sptr, currPtr);
			}
			this->reg->SetValue(this->dialogName, fname1);
		}
	}
	if (multiBuff)
	{
		MemFree(multiBuff);
	}
	return ret;
}
