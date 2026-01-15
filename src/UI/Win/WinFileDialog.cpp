#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "IO/Path.h"
#include "Text/StringBuilderUTF16.h"
#include "UI/Win/WinFileDialog.h"
#include <windows.h>
#include <commdlg.h>

#define MAXFILENAMESIZE 512

UI::Win::WinFileDialog::WinFileDialog(UnsafeArray<const WChar> compName, UnsafeArray<const WChar> appName, UnsafeArray<const WChar> dialogName, Bool isSave) : UI::GUIFileDialog(compName, appName, dialogName, isSave)
{
}

UI::Win::WinFileDialog::~WinFileDialog()
{
}

Bool UI::Win::WinFileDialog::ShowDialog(Optional<ControlHandle> ownerHandle)
{
	WChar fname1[512];
	WChar fname2[512];
	WChar fname[MAXFILENAMESIZE];
	UnsafeArray<WChar> wptr;
	UnsafeArray<WChar> dptr;
	const WChar *initDir;
	WChar *initFileName;
	WChar *multiBuff = 0;
	WChar *fnameBuff;
	IntOS fnameBuffSize;
	NN<Text::String> s;
	UnsafeArray<const WChar> ws;
	Text::StringBuilderUTF16 sb;
	UIntOS i = 0;
	UIntOS filterCnt = this->names.GetCount();

	OPENFILENAMEW ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAMEW);
	ofn.hwndOwner = 0;//hwnd;
	ofn.hInstance = 0;

	if (!isSave && filterCnt > 0)
	{
		sb.AppendC(UTF8STRC("Supported Files"));
		sb.AppendChar('\0', 1);
		while (i < filterCnt)
		{
			if (i > 0)
			{
				sb.AppendC(UTF8STRC(";"));
			}
			sb.Append(Text::String::OrEmpty(this->patterns.GetItem(i++)));
		}
		sb.AppendChar('\0', 1);
	}

	i = 0;
	while (i < filterCnt)
	{
		sb.Append(this->names.GetItemNoCheck(i));
		sb.AppendC(UTF8STRC(" ("));
		sb.Append(this->patterns.GetItemNoCheck(i));
		sb.AppendChar(')', 1);
		sb.AppendChar('\0', 1);
		sb.Append(this->patterns.GetItemNoCheck(i++));
		sb.AppendChar('\0', 1);
	}
	if (!this->isSave)
	{
		sb.AppendC(UTF8STRC("All Files (*.*)"));
		sb.AppendChar('\0', 1);
		sb.AppendC(UTF8STRC("*.*"));
		sb.AppendChar('\0', 1);
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
	if (this->fileName.SetTo(s))
	{
		Text::StrUTF8_WChar(fname2, s->v, 0);
		Text::StrReplaceW(fname2, '/', '_');
		Text::StrReplaceW(&fname2[2], ':', '_');
		Text::StrConcat(fnameBuff, fname2);

		i = Text::StrLastIndexOfCharW(fname2, '\\');
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
		i = Text::StrLastIndexOfCharW(initFileName, '.');
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

	if (this->lastName.SetTo(ws))
	{
		Text::StrConcat(fname1, ws);
		UnsafeArray<WChar> wptr = fnameBuff;
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
					wptr = Text::StrConcat(wptr, ptrStart);
					ptrStart = 0;
				}
				c = *++currPtr;
				if (c == 'd')
				{
					wptr = Text::StrConcat(wptr, initDir);
				}
				else if (c == 'n')
				{
					wptr = Text::StrConcat(wptr, initFileName);
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
			Text::StrConcat(wptr, ptrStart);
		}
	}


	ofn.lpstrFilter = sb.ToPtr();
	ofn.lpstrCustomFilter = 0; 
	ofn.nMaxCustFilter = 0;
	if (this->filterIndex == (UIntOS)-1)
	{
		if (this->isSave && fnameBuff && fnameBuff[0] != 0)
		{
			Bool found = false;
			UIntOS foundIndexLeng = 0;
			NN<Text::String> u8fname = Text::String::NewNotNull(fnameBuff);
			NN<Text::String> pattern;
			i = 0;
			while (i < filterCnt)
			{
				if (this->patterns.GetItem(i).SetTo(pattern) && IO::Path::FilePathMatch(u8fname->v, u8fname->leng, pattern->v, pattern->leng))
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
		OPTSTR_DEL(this->fileName);
		this->fileName = nullptr;
		this->filterIndex = ofn.nFilterIndex - 1;
		if (isSave && ofn.nFileExtension == 0)
		{
			NN<Text::String> pattern;
			if (this->patterns.GetItem(this->filterIndex).SetTo(pattern) && Text::StrStartsWithC(pattern->v, pattern->leng, UTF8STRC("*.")))
			{
				Text::StrUTF8_WChar(&fnameBuff[Text::StrCharCnt(fnameBuff)], &pattern->v[1], 0);
			}
			this->fileName = Text::String::NewNotNull(fnameBuff).Ptr();
		}
		else if (!isSave && this->allowMulti)
		{
			NN<Text::String> s = Text::String::NewNotNull(fnameBuff);
			IO::Path::PathType pt = IO::Path::GetPathType(s->ToCString());
			s->Release();
			if (pt == IO::Path::PathType::Directory)
			{
				dptr = Text::StrConcat(fname, fnameBuff);
				wptr = &fnameBuff[Text::StrCharCnt(fnameBuff) + 1];
				*dptr++ = IO::Path::PATH_SEPERATOR;
				
				i = 0;
				while (*wptr)
				{
					Text::StrConcat(dptr, wptr);
					this->fileNames.Add(Text::String::NewNotNull(fname));
					wptr = &wptr[Text::StrCharCnt(UnsafeArray<const WChar>(wptr)) + 1];
					i++;
				}
				if (i == 1)
				{
					this->fileName = Text::String::OrEmpty(this->fileNames.GetItem(0))->Clone();
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
				wptr = Text::StrConcat(fname1, L"|d");
				currPtr = &fnameBuff[Text::StrCharCnt(initDir)];
			}
			else
			{
				wptr = fname1;
				currPtr = fnameBuff;
			}
			if (initFileName[0])
			{
				i = Text::StrIndexOfW(currPtr, initFileName);
				if (i >= 0)
				{
					currPtr[i] = 0;
					wptr = Text::StrConcat(wptr, currPtr);
					wptr = Text::StrConcat(wptr, L"|n");
					Text::StrConcat(wptr, &currPtr[i + Text::StrCharCnt(initFileName)]);
				}
				else
				{
					Text::StrConcat(wptr, currPtr);
				}
			}
			else
			{
				Text::StrConcat(wptr, currPtr);
			}
			NN<IO::Registry> reg;
			if (this->reg.SetTo(reg))
				reg->SetValue(this->dialogName, fname1);
		}
	}
	if (multiBuff)
	{
		MemFree(multiBuff);
	}
	return ret;
}
