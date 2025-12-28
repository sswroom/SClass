#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "Text/Encoding.h"
#include "Text/Locale.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/URLString.h"
#include "UI/Clipboard.h"
#include <gtk/gtk.h>
#include <wchar.h>

UI::Clipboard::Clipboard(Optional<ControlHandle> hwnd)
{
	this->succ = true;
}

UI::Clipboard::~Clipboard()
{
}


UOSInt UI::Clipboard::GetDataFormats(NN<Data::ArrayList<UInt32>> dataTypes)
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

Bool UI::Clipboard::GetDataText(UInt32 fmtId, NN<Text::StringBuilderUTF8> sb)
{
	return GetDataTextH(0, fmtId, sb, 0);
}

Optional<Data::ByteBuffer> UI::Clipboard::GetDataRAW(UInt32 fmtId)
{
	GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	if (clipboard == 0)
	{
		return 0;
	}

	Optional<Data::ByteBuffer> ret = 0;
	if (fmtId >= 128)
	{
		GdkAtom *targets;
		gint nTargets;
		if (gtk_clipboard_wait_for_targets(clipboard, &targets, &nTargets))
		{
			if (fmtId < (UInt32)nTargets + 128)
			{
				GtkSelectionData *data = gtk_clipboard_wait_for_contents(clipboard, targets[fmtId - 128]);
				if (data)
				{
					gint leng;
					const guchar *rawdata = gtk_selection_data_get_data_with_length(data, &leng);
					NEW_CLASSOPT(ret, Data::ByteBuffer(Data::ByteArrayR(rawdata, (UOSInt)leng)));
					gtk_selection_data_free(data);
				}
			}
			g_free(targets);
			return ret;
		}
		else
		{
			return 0;
		}
	}
	else if (fmtId == 1)
	{
		gchar *s = gtk_clipboard_wait_for_text(clipboard);
		if (s)
		{
			NN<Data::ByteBuffer> ret;
			UOSInt leng = Text::StrCharCnt((const UTF8Char*)s);
			NEW_CLASSNN(ret, Data::ByteBuffer(Data::ByteArrayR((const UTF8Char*)s, leng)));
			g_free(s);
			return ret;
		}
		else
		{
			return 0;
		}
	}
	else if (fmtId == 2)
	{
		gchar **s = gtk_clipboard_wait_for_uris(clipboard);
		if (s)
		{
			Text::StringBuilderUTF8 sb;
			OSInt i = 0;
			while (true)
			{
				if (s[i] == 0)
					break;
				if (i > 0)
				{
					sb.AppendC(UTF8STRC("\r\n"));
				}
				sb.AppendSlow((const UTF8Char*)s[i]);
				i++;
			}
			g_strfreev(s);
			NN<Data::ByteBuffer> ret;
			NEW_CLASSNN(ret, Data::ByteBuffer(sb.ToByteArray()));
			return ret;
		}
		else
		{
			return 0;
		}
	}
	return 0;
}

UI::Clipboard::FilePasteType UI::Clipboard::GetDataFiles(NN<Data::ArrayListStringNN> fileNames)
{
	if (!this->succ)
		return UI::Clipboard::FPT_NONE;
	GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	if (clipboard == 0)
	{
		return UI::Clipboard::FPT_NONE;
	}

	UI::Clipboard::FilePasteType ret = UI::Clipboard::FPT_NONE;
	GdkAtom *targets;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
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
			if (Text::StrEqualsCh(typeName, "x-special/gnome-copied-files"))
			{
				GtkSelectionData *data = gtk_clipboard_wait_for_contents(clipboard, targets[i]);
				if (data)
				{
					gint leng;
					const guchar *rawdata = gtk_selection_data_get_data_with_length(data, &leng);
					if (Text::StrStartsWithC((const UTF8Char*)rawdata, (UOSInt)leng, UTF8STRC("copy")))
					{
						ret = UI::Clipboard::FPT_COPY;
						i = 5;
					}
					else if (Text::StrStartsWithC((const UTF8Char*)rawdata, (UOSInt)leng, UTF8STRC("cut")))
					{
						ret = UI::Clipboard::FPT_MOVE;
						i = 4;
					}
					if (ret != UI::Clipboard::FPT_NONE)
					{
						Text::StringBuilderUTF8 sb;
						while (true)
						{
							j = Text::StrIndexOfChar((const UTF8Char*)&rawdata[i], 10);
							if (j != INVALID_INDEX)
							{
								sb.ClearStr();
								sb.AppendC((const UTF8Char*)&rawdata[i], (UOSInt)j);
								i += j + 1;
								if (sb.StartsWith(UTF8STRC("file:///")))
								{
									sptr = Text::URLString::GetURLFilePath(sbuff, sb.ToString(), sb.GetLength()).Or(sbuff);
									fileNames->Add(Text::String::NewP(sbuff, sptr));
								}
								else
								{
									fileNames->Add(Text::String::New(sb.ToCString()));
								}
							}
							else
							{
								sb.ClearStr();
								sb.AppendC((const UTF8Char*)&rawdata[i], (UInt32)leng - i);
								if (sb.StartsWith(UTF8STRC("file:///")))
								{
									sptr = Text::URLString::GetURLFilePath(sbuff, sb.ToString(), sb.GetLength()).Or(sbuff);
									fileNames->Add(Text::String::NewP(sbuff, sptr));
								}
								else
								{
									fileNames->Add(Text::String::New(sb.ToCString()));
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

void UI::Clipboard::FreeDataFiles(NN<Data::ArrayListStringNN> fileNames)
{
	UOSInt i = fileNames->GetCount();;
	while (i-- > 0)
	{
		OPTSTR_DEL(fileNames->GetItem(i));
	}
}

Bool UI::Clipboard::GetDataTextH(void *hand, UInt32 fmtId, NN<Text::StringBuilderUTF8> sb, UInt32 tymed)
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
						sb->AppendSlow(csptr);
					}
					else
					{
						const Char *typeName = gdk_atom_name(targets[fmtId - 128]);
						gint leng;
						const guchar *rawdata = gtk_selection_data_get_data_with_length(data, &leng);
						if (Text::StrEqualsCh(typeName, "TIMESTAMP") && leng == 8)
						{
							sb->AppendHexBuff(rawdata, (UOSInt)leng, ' ', Text::LineBreakType::CRLF);
/*							Data::DateTime dt;
							dt.SetUnixTimestamp(ReadInt64(rawdata));
							sb->Append(&dt);*/
						}
						else if (Text::StrEqualsCh(typeName, "x-special/gnome-copied-files"))
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
			sb->AppendSlow((const UTF8Char*)s);
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
				sb->AppendSlow((const UTF8Char*)s[i]);
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

Bool UI::Clipboard::SetString(Optional<ControlHandle> hWndOwner, Text::CStringNN s)
{
	GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	if (clipboard == 0)
		return false;

	gtk_clipboard_set_text(clipboard, (const Char*)s.v.Ptr(), (gint)s.leng);
	return true;
}

Bool UI::Clipboard::GetString(Optional<ControlHandle> hWndOwner, NN<Text::StringBuilderUTF8> sb)
{
	GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	if (clipboard == 0)
		return false;

	gchar *s = gtk_clipboard_wait_for_text(clipboard);
	if (s)
	{
		sb->AppendSlow((const UTF8Char*)s);
		return true;
	}
	else
	{
		return false;
	}
}

UnsafeArray<UTF8Char> UI::Clipboard::GetFormatName(UInt32 fmtId, UnsafeArray<UTF8Char> sbuff, UOSInt buffSize)
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

