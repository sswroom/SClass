#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Text/Encoding.h"
#include "Text/Locale.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/URLString.h"
#include "Win32/Clipboard.h"
#include <gtk/gtk.h>
#include <wchar.h>

Win32::Clipboard::Clipboard(void *hwnd)
{
	this->succ = true;
}

Win32::Clipboard::~Clipboard()
{
}


UOSInt Win32::Clipboard::GetDataFormats(Data::ArrayList<UInt32> *dataTypes)
{
	if (!this->succ)
		return 0;
	GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	if (clipboard == 0)
	{
		return false;
	}
	UOSInt ret = 0;
	if (gtk_clipboard_wait_is_text_available(clipboard))
	{
		dataTypes->Add(1);
		ret++;
	}
	if (gtk_clipboard_wait_is_uris_available(clipboard))
	{
		dataTypes->Add(2);
		ret++;
	}
	GdkAtom *targets;
	gint nTargets;
	OSInt i;
	if (gtk_clipboard_wait_for_targets(clipboard, &targets, &nTargets))
	{
		i = 0;
		while (i < nTargets)
		{
			dataTypes->Add(128 + (UInt32)i);
			ret++;
			i++;
		}
		g_free(targets);
	}
	return ret;
}

Bool Win32::Clipboard::GetDataText(UInt32 fmtId, Text::StringBuilderUTF *sb)
{
	return GetDataTextH(0, fmtId, sb, 0);
}

Win32::Clipboard::FilePasteType Win32::Clipboard::GetDataFiles(Data::ArrayList<const UTF8Char *> *fileNames)
{
	if (!this->succ)
		return Win32::Clipboard::FPT_NONE;
	GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	if (clipboard == 0)
	{
		return Win32::Clipboard::FPT_NONE;
	}

	Win32::Clipboard::FilePasteType ret = Win32::Clipboard::FPT_NONE;
	GdkAtom *targets;
	UTF8Char sbuff[512];
	gint nTargets;
	UOSInt i;
	UOSInt j;
	if (gtk_clipboard_wait_for_targets(clipboard, &targets, &nTargets))
	{
		const Char *typeName;
		i = 0;
		while (i < (UInt32)nTargets)
		{
			typeName = gdk_atom_name(targets[i]);
			if (Text::StrEquals(typeName, "x-special/gnome-copied-files"))
			{
				GtkSelectionData *data = gtk_clipboard_wait_for_contents(clipboard, targets[i]);
				if (data)
				{
					gint leng;
					const guchar *rawdata = gtk_selection_data_get_data_with_length(data, &leng);
					if (Text::StrStartsWith((const UTF8Char*)rawdata, (const UTF8Char*)"copy"))
					{
						ret = Win32::Clipboard::FPT_COPY;
						i = 5;
					}
					else if (Text::StrStartsWith((const UTF8Char*)rawdata, (const UTF8Char*)"cut"))
					{
						ret = Win32::Clipboard::FPT_MOVE;
						i = 4;
					}
					if (ret != Win32::Clipboard::FPT_NONE)
					{
						Text::StringBuilderUTF8 sb;
						while (true)
						{
							j = Text::StrIndexOf((const UTF8Char*)&rawdata[i], 10);
							if (j != INVALID_INDEX)
							{
								sb.ClearStr();
								sb.AppendC((const UTF8Char*)&rawdata[i], (UOSInt)j);
								i += j + 1;
								if (sb.StartsWith((const UTF8Char*)"file:///"))
								{
									Text::URLString::GetURLFilePath(sbuff, sb.ToString());
									fileNames->Add(Text::StrCopyNew(sbuff));
								}
								else
								{
									fileNames->Add(Text::StrCopyNew(sb.ToString()));
								}
							}
							else
							{
								sb.ClearStr();
								sb.AppendC((const UTF8Char*)&rawdata[i], (UInt32)leng - i);
								if (sb.StartsWith((const UTF8Char*)"file:///"))
								{
									Text::URLString::GetURLFilePath(sbuff, sb.ToString());
									fileNames->Add(Text::StrCopyNew(sbuff));
								}
								else
								{
									fileNames->Add(Text::StrCopyNew(sb.ToString()));
								}
								break;
							}
						}
					}
					gtk_selection_data_free(data);
				}
				break;
			}
			i++;
		}
		g_free(targets);
	}
	return ret;
}

void Win32::Clipboard::FreeDataFiles(Data::ArrayList<const UTF8Char *> *fileNames)
{
	UOSInt i = fileNames->GetCount();;
	while (i-- > 0)
	{
		Text::StrDelNew(fileNames->GetItem(i));
	}
}

Bool Win32::Clipboard::GetDataTextH(void *hand, UInt32 fmtId, Text::StringBuilderUTF *sb, UInt32 tymed)
{
	GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	if (clipboard == 0)
	{
		return false;
	}

	if (fmtId >= 128)
	{
		GdkAtom *targets;
		gint nTargets;
		if (gtk_clipboard_wait_for_targets(clipboard, &targets, &nTargets))
		{
			Bool ret = false;
			if (fmtId < (UInt32)nTargets + 128)
			{
				GtkSelectionData *data = gtk_clipboard_wait_for_contents(clipboard, targets[fmtId - 128]);
				if (data)
				{
					ret = true;
					const UTF8Char *csptr = (const UTF8Char*)gtk_selection_data_get_text(data);
					if (csptr)
					{
						sb->Append(csptr);
					}
					else
					{
						const Char *typeName = gdk_atom_name(targets[fmtId - 128]);
						gint leng;
						const guchar *rawdata = gtk_selection_data_get_data_with_length(data, &leng);
						if (Text::StrEquals(typeName, "TIMESTAMP") && leng == 8)
						{
							sb->AppendHexBuff(rawdata, (UOSInt)leng, ' ', Text::LineBreakType::CRLF);
/*							Data::DateTime dt;
							dt.SetUnixTimestamp(ReadInt64(rawdata));
							sb->Append(&dt);*/
						}
						else if (Text::StrEquals(typeName, "x-special/gnome-copied-files"))
						{
							sb->AppendHexBuff(rawdata, (UOSInt)leng, ' ', Text::LineBreakType::CRLF);
//							sb->Append((const UTF8Char*)rawdata, leng));
						}
						else
						{
							sb->AppendHexBuff(rawdata, (UOSInt)leng, ' ', Text::LineBreakType::CRLF);
						}
					}
					gtk_selection_data_free(data);
				}
			}
			else
			{
			}
			g_free(targets);
			return ret;
		}
		else
		{
			return false;
		}
	}
	else if (fmtId == 1)
	{
		gchar *s = gtk_clipboard_wait_for_text(clipboard);
		if (s)
		{
			sb->Append((const UTF8Char*)s);
			g_free(s);
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (fmtId == 2)
	{
		gchar **s = gtk_clipboard_wait_for_uris(clipboard);
		if (s)
		{
			OSInt i = 0;
			while (true)
			{
				if (s[i] == 0)
					break;
				if (i > 0)
				{
					sb->AppendC(UTF8STRC("\r\n"));
				}
				sb->Append((const UTF8Char*)s[i]);
				i++;
			}
			g_strfreev(s);
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

Bool Win32::Clipboard::SetString(ControlHandle *hWndOwner, const UTF8Char *s)
{
	GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	if (clipboard == 0)
		return false;

	UOSInt len = Text::StrCharCnt(s);
	gtk_clipboard_set_text(clipboard, (const Char*)s, (gint)len);
	return true;
}

Bool Win32::Clipboard::GetString(ControlHandle *hWndOwner, Text::StringBuilderUTF *sb)
{
	GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	if (clipboard == 0)
		return false;

	gchar *s = gtk_clipboard_wait_for_text(clipboard);
	if (s)
	{
		sb->Append((const UTF8Char*)s);
		return true;
	}
	else
	{
		return false;
	}
}

UTF8Char *Win32::Clipboard::GetFormatName(UInt32 fmtId, UTF8Char *sbuff, UOSInt buffSize)
{
	if (fmtId >= 128)
	{
		GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
		if (clipboard == 0)
		{
			return Text::StrConcatC(sbuff, UTF8STRC("Unknown"));
		}
		GdkAtom *targets;
		gint nTargets;
		if (gtk_clipboard_wait_for_targets(clipboard, &targets, &nTargets))
		{
			if (fmtId < (UInt32)nTargets + 128)
			{
				sbuff = Text::StrConcat(sbuff, (const UTF8Char*)gdk_atom_name(targets[fmtId - 128]));
				return sbuff;
			}
			else
			{
				sbuff = Text::StrConcatC(sbuff, UTF8STRC("Unknown"));
			}
			g_free(targets);
			return sbuff;
		}
		else
		{
			return Text::StrConcatC(sbuff, UTF8STRC("Unknown"));
		}
	}
	else if (fmtId == 1)
	{
		return Text::StrConcatC(sbuff, UTF8STRC("TEXT"));
	}
	else if (fmtId == 2)
	{
		return Text::StrConcatC(sbuff, UTF8STRC("URIs"));
	}
	else
	{
		return Text::StrConcatC(sbuff, UTF8STRC("Unknown"));
	}
}

