#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/GTK/GTKDragDrop.h"
#include <gtk/gtk.h>
#include <stdio.h>

void UI::GTK::GTKDropData::AppendWC(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UTF16Char> s, UIntOS slen)
{
	NN<Text::String> str = Text::String::NewW(s, slen);
	sb->Append(str);
	str->Release();
}

UI::GTK::GTKDropData::GTKDropData(void *widget, void *context, UInt32 time, Bool readData)
{
	this->widget = widget;
	this->context = context;
	this->time = time;

	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	GList *list = gdk_drag_context_list_targets((GdkDragContext*)context);
	if (list)
	{
		guint i;
		guint j;
		GdkAtom target;
		i = 0;
		j = g_list_length(list);
		while (i < j)
		{
			target = GDK_POINTER_TO_ATOM(g_list_nth_data(list, i));
			const char *csptr = gdk_atom_name(target);
			if (csptr)
			{
				sptr = Text::StrConcat(sbuff, (const UTF8Char*)csptr);
			}
			else
			{
				sptr = Text::StrIntOS(Text::StrConcatC(sbuff, UTF8STRC("Format ")), (IntOS)target);
			}
//			printf("%s\r\n", sbuff);
			this->targetMap.Put(CSTRP(sbuff, sptr), (IntOS)target);

			if (readData)
			{
				gtk_drag_get_data((GtkWidget*)this->widget, (GdkDragContext*)this->context, target, this->time);
			}

			i++;
		}
	}
//	printf("GUIDragDataGTK Init End\r\n");
}

UI::GTK::GTKDropData::~GTKDropData()
{
	UIntOS i = this->targetText.GetCount();
	while (i-- > 0)
	{
		this->targetText.GetItemNoCheck(i)->Release();
	}
}

/*void UI::GUIDragDataGTK::ReadData()
{
	Data::ArrayList<IntOS> *targetList = this->targetMap->GetValues();
	IntOS i = 0;
	IntOS j = targetList->GetCount();
	while (i < j)
	{
		i++;
	}
}*/

UIntOS UI::GTK::GTKDropData::GetCount()
{
	return this->targetMap.GetCount();
}

UnsafeArrayOpt<const UTF8Char> UI::GTK::GTKDropData::GetName(UIntOS index)
{
	NN<Text::String> key;
	if (this->targetMap.GetKey(index).SetTo(key))
	{
		return key->v.Ptr();
	}
	return nullptr;
}

Bool UI::GTK::GTKDropData::GetDataText(UnsafeArray<const UTF8Char> name, NN<Text::StringBuilderUTF8> sb)
{
	IntOS fmt = this->targetMap.Get(Text::CStringNN::FromPtr(name));
	if (fmt == 0)
		return false;
	NN<Text::String> txt;
	if (this->targetText.Get((Int32)fmt).SetTo(txt))
	{
		sb->Append(txt);
		return true;
	}
	else
	{
		return false;
	}
}

IO::Stream *UI::GTK::GTKDropData::GetDataStream(UnsafeArray<const UTF8Char> name)
{
	return 0;
}

void UI::GTK::GTKDropData::OnDataReceived(void *selData)
{
	GdkAtom target = gtk_selection_data_get_target((GtkSelectionData*)selData);
	Int32 itarget = (Int32)(IntOS)target;
	UIntOS dataSize = (UInt32)gtk_selection_data_get_length((GtkSelectionData*)selData);
	GdkAtom dataType = gtk_selection_data_get_data_type((GtkSelectionData*)selData);
	const char *csptr = gdk_atom_name(target);
	if (csptr == 0)
	{
		csptr = "Unknown";
	}

	const char *csptr2 = gdk_atom_name(dataType);
	if (csptr2 == 0)
	{
		csptr2 = "Unknown";
	}
	GdkAtom sel = gtk_selection_data_get_selection((GtkSelectionData*)selData);
//	printf("Selection = %d, Target = %d (%s), Data Type = %d (%s), size = %d\r\n", sel, itarget, csptr, (Int32)(IntOS)dataType, csptr2, (Int32)dataSize);
	if (this->targetText.ContainsKey(itarget))
	{
		return;
	}

	if (dataSize >= 0 && sel != 0)
	{
		Text::StringBuilderUTF8 sb;
		const UInt8 *data = gtk_selection_data_get_data((GtkSelectionData*)selData);
		if (Text::StrEqualsCh(csptr, "application/x-moz-nativeimage"))
		{
			GdkPixbuf *pixbuf = gtk_selection_data_get_pixbuf((GtkSelectionData*)selData);
			if (pixbuf)
			{
				sb.AppendC(UTF8STRC("PixBuf Image"));
				g_object_unref(pixbuf);
			}
		}
		else if (Text::StrEqualsCh(csptr, "text/html"))
		{
			AppendWC(sb, (const UTF16Char*)data, dataSize / 2);
		}
		else if (Text::StrEqualsCh(csptr, "text/unicode"))
		{
			AppendWC(sb, (const UTF16Char*)data, dataSize / 2);
		}
		else if (Text::StrEqualsCh(csptr, "text/x-moz-url"))
		{
			AppendWC(sb, (const UTF16Char*)data, dataSize / 2);
		}
		else if (Text::StrEqualsCh(csptr, "text/x-moz-url-data"))
		{
			AppendWC(sb, (const UTF16Char*)data, dataSize / 2);
		}
		else if (Text::StrEqualsCh(csptr, "text/x-moz-url-desc"))
		{
			AppendWC(sb, (const UTF16Char*)data, dataSize / 2);
		}
		else if (Text::StrEqualsCh(csptr, "text/_moz_htmlcontext"))
		{
			AppendWC(sb, (const UTF16Char*)data, dataSize / 2);
		}
		else if (Text::StrEqualsCh(csptr, "text/_moz_htmlinfo"))
		{
			AppendWC(sb, (const UTF16Char*)data, dataSize / 2);
		}
		else if (Text::StrEqualsCh(csptr, "application/x-moz-file-promise-url"))
		{
			AppendWC(sb, (const UTF16Char*)data, dataSize / 2);
		}
		else if (Text::StrEqualsCh(csptr, "application/x-moz-file-promise-dest-filename"))
		{
			AppendWC(sb, (const UTF16Char*)data, dataSize / 2);
		}
		else if (Text::StrEqualsCh(csptr, "application/x-moz-custom-clipdata"))
		{
			sb.AppendHexBuff(data, dataSize, ' ', Text::LineBreakType::CRLF);
		}
		else
		{
			sb.AppendC((const UTF8Char*)data, dataSize);
		}
		this->targetText.Put(itarget, Text::String::New(sb.ToString(), sb.GetLength()));
	}
	else
	{
		this->targetText.Put(itarget, Text::String::NewEmpty());
	}
	
}

gboolean UI::GTK::GTKDragDrop::OnDragMotion(GtkWidget *widget, GdkDragContext *context, gint x, gint y, guint time, gpointer user_data)
{
	UI::GTK::GTKDragDrop *me = (UI::GTK::GTKDragDrop *)user_data;
//	printf("DragMotion\r\n");
	gdk_drag_status(context, (GdkDragAction)me->EventDragMotion(context, x, y, time), time);
	return true;
}

void UI::GTK::GTKDragDrop::OnDragLeave(GtkWidget *widget, GdkDragContext *context, guint time, gpointer user_data)
{
	UI::GTK::GTKDragDrop *me = (UI::GTK::GTKDragDrop *)user_data;
//	printf("DragLeave\r\n");
	me->EventDragLeave();
}

gboolean UI::GTK::GTKDragDrop::OnDragDrop(GtkWidget *widget, GdkDragContext *context, gint x, gint y, guint time, gpointer user_data)
{
	UI::GTK::GTKDragDrop *me = (UI::GTK::GTKDragDrop *)user_data;
//	printf("DragDrop\r\n");

	me->EventDragDrop(context, x, y, time);
	gtk_drag_finish(context, true, true, time);
	return true;
}

void UI::GTK::GTKDragDrop::OnDropData(GtkWidget *widget, GdkDragContext *context, gint x, gint y, GtkSelectionData *data, guint info, guint time, gpointer user_data)
{
	UI::GTK::GTKDragDrop *me = (UI::GTK::GTKDragDrop*)user_data;
//	printf("DropData\r\n");
	if (me->dragData)
	{
//		printf("DropData->OnDataReceived\r\n");
		me->dragData->OnDataReceived(data);
	}
}

UI::GTK::GTKDragDrop::GTKDragDrop(Optional<ControlHandle> hWnd, NN<UI::GUIDropHandler> hdlr)
{
	this->hWnd = hWnd;
	this->hdlr = hdlr;
	this->currEff = UI::GUIDropHandler::DE_NONE;
	this->dragData = 0;

	gtk_drag_dest_set((GtkWidget*)this->hWnd.OrNull(), (GtkDestDefaults)0, 0, 0, GDK_ACTION_COPY);
	g_signal_connect((GtkWindow*)this->hWnd.OrNull(), "drag-motion", G_CALLBACK(OnDragMotion), this);
	g_signal_connect((GtkWindow*)this->hWnd.OrNull(), "drag-leave", G_CALLBACK(OnDragLeave), this);
	g_signal_connect((GtkWindow*)this->hWnd.OrNull(), "drag-drop", G_CALLBACK(OnDragDrop), this);
	g_signal_connect((GtkWindow*)this->hWnd.OrNull(), "drag-data-received", G_CALLBACK(OnDropData), this);
}

UI::GTK::GTKDragDrop::~GTKDragDrop()
{
	SDEL_CLASS(this->dragData);
}

void UI::GTK::GTKDragDrop::SetHandler(NN<UI::GUIDropHandler> hdlr)
{
	this->hdlr = hdlr;
}

Int32 UI::GTK::GTKDragDrop::EventDragMotion(void *context, IntOS x, IntOS y, UInt32 time)
{
	NN<UI::GTK::GTKDropData> dragData;
	if (dragData.Set(this->dragData))
	{
	}
	else
	{
		NEW_CLASSNN(dragData, UI::GTK::GTKDropData(this->hWnd.OrNull(), context, time, false));
		this->dragData = dragData.Ptr();
		this->currEff = this->hdlr->DragEnter(dragData);
	}
	if (this->currEff == UI::GUIDropHandler::DE_COPY)
		return GDK_ACTION_COPY;
	else if (this->currEff == UI::GUIDropHandler::DE_MOVE)
		return GDK_ACTION_MOVE;
	else if (this->currEff == UI::GUIDropHandler::DE_LINK)
		return GDK_ACTION_LINK;
	else
		return GDK_ACTION_DEFAULT;
}

void UI::GTK::GTKDragDrop::EventDragLeave()
{
	SDEL_CLASS(this->dragData);
}

Bool UI::GTK::GTKDragDrop::EventDragDrop(void *context, IntOS x, IntOS y, UInt32 time)
{
	NN<UI::GTK::GTKDropData> dragData;
	if (dragData.Set(this->dragData))
	{
	}
	else
	{
		NEW_CLASSNN(dragData, UI::GTK::GTKDropData(this->hWnd.OrNull(), context, time, false));
		this->dragData = dragData.Ptr();
	}
	while (gtk_events_pending())
	{
		gtk_main_iteration();
	}
//	printf("OnDragDrop\r\n");
	this->hdlr->DropData(dragData, x, y);
	return true;
}