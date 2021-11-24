#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/FileDialog.h"
#include <gtk/gtk.h>

#define MAXFILENAMESIZE 512

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
	NEW_CLASS(this->names, Data::ArrayListStrUTF8());
	NEW_CLASS(this->patterns, Data::ArrayListStrUTF8());
}

UI::FileDialog::~FileDialog()
{
	UOSInt i;
	IO::Registry::CloseRegistry(this->reg);
	MemFree(this->dialogName);
	SDEL_TEXT(this->lastName);
	SDEL_TEXT(this->fileName);
	i = this->patterns->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->patterns->RemoveAt(i));
		Text::StrDelNew(this->names->RemoveAt(i));
	}
	this->ClearFileNames();
	DEL_CLASS(this->fileNames);
	DEL_CLASS(this->patterns);
	DEL_CLASS(this->names);
}

void UI::FileDialog::AddFilter(const UTF8Char *pattern, const UTF8Char *name)
{
	this->patterns->Add(Text::StrCopyNew(pattern));
	this->names->Add(Text::StrCopyNew(name));
}

UOSInt UI::FileDialog::GetFilterIndex()
{
	return this->filterIndex;
}

void UI::FileDialog::SetFileName(const UTF8Char *fileName)
{
	SDEL_TEXT(this->fileName);
	this->fileName = Text::StrCopyNew(fileName);
}

const UTF8Char *UI::FileDialog::GetFileName()
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
		return this->fileName;
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
	WChar fname3[512];
	WChar fname[MAXFILENAMESIZE];
	WChar *sptr;
//	WChar *dptr;
	const WChar *initDir;
	const WChar *initFileName;
	WChar *multiBuff = 0;
	WChar *fnameBuff;
//	OSInt fnameBuffSize;
	Text::StringBuilderUTF8 sb;
	UOSInt i = 0;
	UOSInt filterCnt = this->names->GetCount();

	GtkWidget *dialog;
	GtkFileChooser *chooser;
	if (isSave)
	{
		dialog = gtk_file_chooser_dialog_new("Save File", 0, GTK_FILE_CHOOSER_ACTION_SAVE,  "Cancel", GTK_RESPONSE_CANCEL, "Save", GTK_RESPONSE_ACCEPT, (void*)0);
		chooser = GTK_FILE_CHOOSER(dialog);
		gtk_file_chooser_set_do_overwrite_confirmation(chooser, TRUE);
	}
	else
	{
		dialog = gtk_file_chooser_dialog_new("Open File", 0, GTK_FILE_CHOOSER_ACTION_OPEN,  "Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, (void*)0);
		chooser = GTK_FILE_CHOOSER(dialog);
	}

	GtkFileFilter *filter;
	if (!isSave && filterCnt > 0)
	{
		filter = gtk_file_filter_new();
		gtk_file_filter_set_name(filter, "Supported Files");
		while (i < filterCnt)
		{
			gtk_file_filter_add_pattern(filter, (const Char*)this->patterns->GetItem(i++));
		}
		gtk_file_chooser_add_filter(chooser, filter);
	}

	i = 0;
	while (i < filterCnt)
	{
		filter = gtk_file_filter_new();
		sb.ClearStr();
		sb.Append(this->names->GetItem(i));
		sb.Append((const UTF8Char*)" (");
		sb.Append(this->patterns->GetItem(i));
		sb.AppendChar(')', 1);
		gtk_file_filter_set_name(filter, (const Char*)sb.ToString());
		gtk_file_filter_add_pattern(filter, (const Char*)this->patterns->GetItem(i++));
		gtk_file_chooser_add_filter(chooser, filter);
	}
	if (!this->isSave)
	{
		filter = gtk_file_filter_new();
		gtk_file_filter_set_name(filter, "All Files (*.*)");
		gtk_file_filter_add_pattern(filter, (const Char*)IO::Path::ALL_FILES);
		gtk_file_chooser_add_filter(chooser, filter);
	}
 	
	if (this->allowMulti && !this->isSave)
	{
		gtk_file_chooser_set_select_multiple(chooser, true);
//		multiBuff = fnameBuff = MemAlloc(WChar, 65536);
//		fnameBuffSize = 65536;
	}

	fnameBuff = fname;
//	fnameBuffSize = MAXFILENAMESIZE;
	if (this->fileName)
	{
		Text::StrUTF8_WChar(fname2, this->fileName, 0);
		if (IO::Path::PATH_SEPERATOR == '\\')
		{
			Text::StrReplace(fname2, '/', '_');
		}
		Text::StrReplace(&fname2[2], ':', '_');
		Text::StrConcat(fnameBuff, fname2);

		UOSInt i = Text::StrLastIndexOf(fname2, IO::Path::PATH_SEPERATOR);
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

	}
	else
	{
		fname2[0] = 0;
		fnameBuff[0] = 0;
		initDir = L".";
		initFileName = initDir + 1;
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
				else if (c == 'N')
				{
					i = Text::StrLastIndexOf(initFileName, '.');
					if (i == INVALID_INDEX)
					{
						sptr = Text::StrConcat(sptr, initFileName);
					}
					else
					{
						sptr = Text::StrConcatC(sptr, initFileName, i);
					}
				}
				else if (c == 'x')
				{
					i = Text::StrLastIndexOf(initFileName, '.');
					if (i != INVALID_INDEX)
					{
						sptr = Text::StrConcat(sptr, &initFileName[i]);
					}
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
	

/*	ofn.lpstrFilter = sb.ToString();
	ofn.lpstrCustomFilter = 0; 
	ofn.nMaxCustFilter = 0;*/
	UOSInt nFilterIndex = 0;
	if (this->filterIndex == INVALID_INDEX)
	{
		if (this->isSave && fnameBuff && fnameBuff[0] != 0)
		{
			Bool found = false;
			UOSInt foundIndexLeng = 0;
			const UTF8Char *u8fname = Text::StrToUTF8New(fnameBuff);
			i = 0;
			while (i < filterCnt)
			{
				if (IO::Path::FileNameMatch(u8fname, this->patterns->GetItem(i)))
				{
					if (!found)
					{
						found = true;
						nFilterIndex = i;
						foundIndexLeng = Text::StrCharCnt(this->patterns->GetItem(i));
					}
					else if (Text::StrCharCnt(this->patterns->GetItem(i)) > foundIndexLeng)
					{
						nFilterIndex = i;
						foundIndexLeng = Text::StrCharCnt(this->patterns->GetItem(i));
					}
				}
				i++;
			}
			Text::StrDelNew(u8fname);
			if (!found)
			{
				nFilterIndex = 0;
			}
		}
		else
		{
			nFilterIndex = 0;
		}
	}
	else
	{
		nFilterIndex = this->filterIndex;
	}
	UOSInt si = nFilterIndex;
	GSList *list = gtk_file_chooser_list_filters(chooser);
	while (list)
	{
		if (si == 0)
		{
			gtk_file_chooser_set_filter(chooser, (GtkFileFilter*)list->data);
			break;
		}
		si--;
		if (list->next == 0)
		{
			break;
		}
		list = list->next;
	}
	g_slist_free(list);

/*
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
		ret = GetOpenFileNameW(&ofn) != 0;*/

	const UTF8Char *csptr;
	if (fnameBuff)
	{
		si = Text::StrLastIndexOf(fnameBuff, IO::Path::PATH_SEPERATOR);
		if (si == INVALID_INDEX)
		{
			csptr = Text::StrToUTF8New(initDir);
			gtk_file_chooser_set_current_folder(chooser, (const Char*)csptr);
			Text::StrDelNew(csptr);
		}
		else
		{
			fnameBuff[si] = 0;
			csptr = Text::StrToUTF8New(fnameBuff);
			gtk_file_chooser_set_current_folder(chooser, (const Char*)csptr);
			Text::StrDelNew(csptr);
			fnameBuff[si] = IO::Path::PATH_SEPERATOR;
		}
		csptr = Text::StrToUTF8New(&fnameBuff[si + 1]);
		if (isSave)
		{
			gtk_file_chooser_set_current_name(chooser, (const Char*)csptr);
		}
		else
		{
			gtk_file_chooser_set_filename(chooser, (const Char*)csptr);
		}
		Text::StrDelNew(csptr);
	}
	else
	{
		csptr = Text::StrToUTF8New(initDir);
		gtk_file_chooser_set_current_folder(chooser, (const Char*)csptr);
		Text::StrDelNew(csptr);
	}

	Bool ret = false;
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
	{
		ret = true;
		Bool toSave = true;
		this->ClearFileNames();
		if (this->fileName)
		{
			Text::StrDelNew(this->fileName);
			this->fileName = 0;
		}
		this->filterIndex = (UOSInt)-1;
		GtkFileFilter *filter = gtk_file_chooser_get_filter(chooser);
		if (filter)
		{
			GSList *list = gtk_file_chooser_list_filters(chooser);
			this->filterIndex = 0;
			while (true)
			{
				if (list->data == filter)
				{
					break;
				}
				this->filterIndex++;
				if (list->next == 0)
				{
					this->filterIndex = (UOSInt)-1;
					break;
				}
				list = list->next;
			}
			g_slist_free(list);
		}
		if (this->allowMulti && !this->isSave)
		{
			GSList *fileList = gtk_file_chooser_get_filenames(chooser);
			GSList *it = fileList;
			if (it)
			{
				Text::StrUTF8_WChar(fnameBuff, (const UTF8Char*)it->data, 0);
			}
			while (it)
			{
				this->fileNames->Add(Text::StrCopyNew((const UTF8Char*)it->data));
				g_free(it->data);
				it = g_slist_next(it);
			}
			g_slist_free(fileList);
		}
		else
		{
			char *csptr = gtk_file_chooser_get_filename(chooser);
			Text::StrUTF8_WChar(fnameBuff, (const UTF8Char*)csptr, 0);
			this->fileName = Text::StrCopyNew((const UTF8Char*)csptr);
			g_free(csptr);
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
				if (i != INVALID_INDEX)
				{
					currPtr[i] = 0;
					sptr = Text::StrConcat(sptr, currPtr);
					sptr = Text::StrConcat(sptr, L"|n");
					Text::StrConcat(sptr, &currPtr[i + Text::StrCharCnt(initFileName)]);
				}
				else
				{
					i = Text::StrLastIndexOf(initFileName, '.');
					if (i == INVALID_INDEX)
					{
						Text::StrConcat(sptr, currPtr);
					}
					else
					{
						Text::StrConcatC(fname3, initFileName, i);
						UOSInt j = Text::StrIndexOf(currPtr, fname3);
						if (j != INVALID_INDEX)
						{
							currPtr[j] = 0;
							sptr = Text::StrConcat(sptr, currPtr);
							sptr = Text::StrConcat(sptr, L"|N");
							currPtr += i + j;
						}
						j = Text::StrIndexOf(currPtr, &initFileName[i]);
						if (j != INVALID_INDEX)
						{
							sptr = Text::StrConcatC(sptr, currPtr, j);
							sptr = Text::StrConcat(sptr, L"|x");
							currPtr += j + Text::StrCharCnt(&initFileName[i]);
							if (currPtr[0])
							{
								sptr = Text::StrConcat(sptr, currPtr);
							}
						}
						else
						{
							sptr = Text::StrConcat(sptr, currPtr);
						}
					}
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
	gtk_widget_destroy(dialog);
	return ret;
}
