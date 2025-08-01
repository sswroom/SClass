#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/GTK/GTKFileDialog.h"
#include <gtk/gtk.h>

#define MAXFILENAMESIZE 512

UI::GTK::GTKFileDialog::GTKFileDialog(UnsafeArray<const WChar> compName, UnsafeArray<const WChar> appName, UnsafeArray<const WChar> dialogName, Bool isSave) : UI::GUIFileDialog(compName, appName, dialogName, isSave)
{
}

UI::GTK::GTKFileDialog::~GTKFileDialog()
{
}

Bool UI::GTK::GTKFileDialog::ShowDialog(Optional<ControlHandle> ownerHandle)
{
	WChar fname1[512];
	WChar fname2[512];
	WChar fname3[512];
	WChar fname[MAXFILENAMESIZE];
	UnsafeArray<WChar> wptr;
	const WChar *initDir;
	const WChar *initFileName;
	WChar *multiBuff = 0;
	WChar *fnameBuff;
	Text::StringBuilderUTF8 sb;
	UOSInt i = 0;
	UOSInt filterCnt = this->names.GetCount();
	NN<Text::String> name;
	NN<Text::String> pattern;
	NN<Text::String> s;
	const Char *title;

	GtkWidget *dialog;
	GtkFileChooser *chooser;
	if (isSave)
	{
		title = "Save File";
		if (this->fileName.SetTo(s))
		{
			sb.ClearStr();
			sb.Append(CSTR("Save File for ("));
			sb.Append(s);
			sb.AppendUTF8Char(')');
			title = (const Char*)sb.ToPtr();
		}
		dialog = gtk_file_chooser_dialog_new(title, 0, GTK_FILE_CHOOSER_ACTION_SAVE,  "Cancel", GTK_RESPONSE_CANCEL, "Save", GTK_RESPONSE_ACCEPT, (void*)0);
		chooser = GTK_FILE_CHOOSER(dialog);
		gtk_file_chooser_set_do_overwrite_confirmation(chooser, TRUE);
		gtk_file_chooser_set_create_folders(chooser, TRUE);
	}
	else
	{
		title = "Open File";
		if (this->fileName.SetTo(s))
		{
			sb.ClearStr();
			sb.Append(CSTR("Open File for ("));
			sb.Append(s);
			sb.AppendUTF8Char(')');
			title = (const Char*)sb.ToPtr();
		}
		dialog = gtk_file_chooser_dialog_new(title, 0, GTK_FILE_CHOOSER_ACTION_OPEN,  "Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, (void*)0);
		chooser = GTK_FILE_CHOOSER(dialog);
	}

	GtkFileFilter *filter;
	if (!isSave && filterCnt > 0)
	{
		filter = gtk_file_filter_new();
		gtk_file_filter_set_name(filter, "Supported Files");
		while (i < filterCnt)
		{
			if (this->patterns.GetItem(i++).SetTo(pattern))
				gtk_file_filter_add_pattern(filter, (const Char*)pattern->v.Ptr());
		}
		gtk_file_chooser_add_filter(chooser, filter);
	}

	i = 0;
	while (i < filterCnt)
	{
		if (this->names.GetItem(i).SetTo(name) && this->patterns.GetItem(i).SetTo(pattern))
		{
			filter = gtk_file_filter_new();
			sb.ClearStr();
			sb.Append(name);
			sb.AppendC(UTF8STRC(" ("));
			sb.Append(pattern);
			sb.AppendUTF8Char(')');
			gtk_file_filter_set_name(filter, (const Char*)sb.ToPtr());
			gtk_file_filter_add_pattern(filter, (const Char*)pattern->v.Ptr());
			gtk_file_chooser_add_filter(chooser, filter);
		}
		i++;
	}
	if (!this->isSave)
	{
		filter = gtk_file_filter_new();
		gtk_file_filter_set_name(filter, "All Files (*.*)");
		gtk_file_filter_add_pattern(filter, (const Char*)IO::Path::ALL_FILES.Ptr());
		gtk_file_chooser_add_filter(chooser, filter);
	}
 	
	if (this->allowMulti && !this->isSave)
	{
		gtk_file_chooser_set_select_multiple(chooser, true);
//		multiBuff = fnameBuff = MemAlloc(WChar, 65536);
//		fnameBuffSize = 65536;
	}

	fnameBuff = fname;
	if (this->fileName.SetTo(s))
	{
		Text::StrUTF8_WChar(fname2, s->v, 0);
		if (IO::Path::PATH_SEPERATOR == '\\')
		{
			Text::StrReplaceW(fname2, '/', '_');
		}
		Text::StrReplaceW(&fname2[2], ':', '_');
		Text::StrConcat(fnameBuff, fname2);

		UOSInt i = Text::StrLastIndexOfCharW(fname2, IO::Path::PATH_SEPERATOR);
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

	UnsafeArray<const WChar> lastName;
	if (this->lastName.SetTo(lastName))
	{
		Text::StrConcat(fname1, lastName);
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
				else if (c == 'N')
				{
					i = Text::StrLastIndexOfCharW(initFileName, '.');
					if (i == INVALID_INDEX)
					{
						wptr = Text::StrConcat(wptr, initFileName);
					}
					else
					{
						wptr = Text::StrConcatC(wptr, initFileName, i);
					}
				}
				else if (c == 'x')
				{
					i = Text::StrLastIndexOfCharW(initFileName, '.');
					if (i != INVALID_INDEX)
					{
						wptr = Text::StrConcat(wptr, &initFileName[i]);
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
			Text::StrConcat(wptr, ptrStart);
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
			NN<Text::String> u8fname = Text::String::NewNotNull(fnameBuff);
			i = 0;
			while (i < filterCnt)
			{
				NN<Text::String> pattern;
				if (this->patterns.GetItem(i).SetTo(pattern) && IO::Path::FilePathMatch(u8fname->v, u8fname->leng, pattern->v, pattern->leng))
				{
					if (!found)
					{
						found = true;
						nFilterIndex = i;
						foundIndexLeng = pattern->leng;
					}
					else if (pattern->leng > foundIndexLeng)
					{
						nFilterIndex = i;
						foundIndexLeng = pattern->leng;
					}
				}
				i++;
			}
			u8fname->Release();
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
	GSList *item = list;
	while (item)
	{
		if (si == 0)
		{
			gtk_file_chooser_set_filter(chooser, (GtkFileFilter*)item->data);
			break;
		}
		si--;
		if (item->next == 0)
		{
			break;
		}
		item = item->next;
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

	UnsafeArray<const UTF8Char> csptr;
	if (fnameBuff)
	{
		si = Text::StrLastIndexOfCharW(fnameBuff, IO::Path::PATH_SEPERATOR);
		if (si == INVALID_INDEX)
		{
			csptr = Text::StrToUTF8New(initDir);
			gtk_file_chooser_set_current_folder(chooser, (const Char*)csptr.Ptr());
			Text::StrDelNew(csptr);
		}
		else
		{
			fnameBuff[si] = 0;
			csptr = Text::StrToUTF8New(fnameBuff);
			gtk_file_chooser_set_current_folder(chooser, (const Char*)csptr.Ptr());
			Text::StrDelNew(csptr);
			fnameBuff[si] = IO::Path::PATH_SEPERATOR;
		}
		csptr = Text::StrToUTF8New(&fnameBuff[si + 1]);
		if (isSave)
		{
			gtk_file_chooser_set_current_name(chooser, (const Char*)csptr.Ptr());
		}
		else
		{
			gtk_file_chooser_set_filename(chooser, (const Char*)csptr.Ptr());
		}
		Text::StrDelNew(csptr);
	}
	else
	{
		csptr = Text::StrToUTF8New(initDir);
		gtk_file_chooser_set_current_folder(chooser, (const Char*)csptr.Ptr());
		Text::StrDelNew(csptr);
	}

	Bool ret = false;
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
	{
		ret = true;
		Bool toSave = true;
		this->ClearFileNames();
		OPTSTR_DEL(this->fileName);
		this->fileName = 0;
		this->filterIndex = (UOSInt)-1;
		GtkFileFilter *filter = gtk_file_chooser_get_filter(chooser);
		if (filter)
		{
			GSList *list = gtk_file_chooser_list_filters(chooser);
			GSList *item = list;
			this->filterIndex = 0;
			while (true)
			{
				if (item->data == filter)
				{
					break;
				}
				this->filterIndex++;
				if (item->next == 0)
				{
					this->filterIndex = (UOSInt)-1;
					break;
				}
				item = item->next;
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
				this->fileNames.Add(Text::String::NewNotNullSlow((const UTF8Char*)it->data));
				g_free(it->data);
				it = g_slist_next(it);
			}
			g_slist_free(fileList);
		}
		else
		{
			char *csptr = gtk_file_chooser_get_filename(chooser);
			Text::StrUTF8_WChar(fnameBuff, (const UTF8Char*)csptr, 0);
			this->fileName = Text::String::NewNotNullSlow((const UTF8Char*)csptr).Ptr();
			g_free(csptr);
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
				if (i != INVALID_INDEX)
				{
					currPtr[i] = 0;
					wptr = Text::StrConcat(wptr, currPtr);
					wptr = Text::StrConcat(wptr, L"|n");
					Text::StrConcat(wptr, &currPtr[i + Text::StrCharCnt(initFileName)]);
				}
				else
				{
					i = Text::StrLastIndexOfCharW(initFileName, '.');
					if (i == INVALID_INDEX)
					{
						Text::StrConcat(wptr, currPtr);
					}
					else
					{
						Text::StrConcatC(fname3, initFileName, i);
						UOSInt j = Text::StrIndexOfW(currPtr, fname3);
						if (j != INVALID_INDEX)
						{
							currPtr[j] = 0;
							wptr = Text::StrConcat(wptr, currPtr);
							wptr = Text::StrConcat(wptr, L"|N");
							currPtr += i + j;
						}
						j = Text::StrIndexOfW(currPtr, &initFileName[i]);
						if (j != INVALID_INDEX)
						{
							wptr = Text::StrConcatC(wptr, currPtr, j);
							wptr = Text::StrConcat(wptr, L"|x");
							currPtr += j + Text::StrCharCnt(&initFileName[i]);
							if (currPtr[0])
							{
								wptr = Text::StrConcat(wptr, currPtr);
							}
						}
						else
						{
							wptr = Text::StrConcat(wptr, currPtr);
						}
					}
				}
			}
			else
			{
				Text::StrConcat(wptr, currPtr);
			}
			NN<IO::Registry> nnreg;
			if (this->reg.SetTo(nnreg))
				nnreg->SetValue(this->dialogName, fname1);
		}
	}
	if (multiBuff)
	{
		MemFree(multiBuff);
	}
	gtk_widget_destroy(dialog);
	return ret;
}
