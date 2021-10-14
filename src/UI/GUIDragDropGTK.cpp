#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/GUIDragDropGTK.h"
#include <gtk/gtk.h>
#include <stdio.h>

UI::GUIDragDataGTK *GUIDragDataGTK_currData = 0;

void GUIDragDataGTK_AppendWC(Text::StringBuilderUTF *sb, const UTF16Char *s, UOSInt slen)
{
	const UTF16Char *csptr = Text::StrCopyNewC(s, slen);
	const UTF8Char *csptr2 = Text::StrToUTF8New(csptr);
	sb->Append(csptr2);
	Text::StrDelNew(csptr2);
	Text::StrDelNew(csptr);
}

UI::GUIDragDataGTK::GUIDragDataGTK(void *widget, void *context, UInt32 time, Bool readData)
{
	this->widget = widget;
	this->context = context;
	this->time = time;
	NEW_CLASS(this->targetMap, Data::StringUTF8Map<OSInt>());
	NEW_CLASS(this->targetText, Data::Int32Map<const UTF8Char *>());
	GUIDragDataGTK_currData = this;

	UTF8Char sbuff[128];
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
				Text::StrConcat(sbuff, (const UTF8Char*)csptr);
			}
			else
			{
				Text::StrOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"Format "), (OSInt)target);
			}
//			printf("%s\r\n", sbuff);
			this->targetMap->Put(sbuff, (OSInt)target);

			if (readData)
			{
				gtk_drag_get_data((GtkWidget*)this->widget, (GdkDragContext*)this->context, target, this->time);
			}

			i++;
		}
	}
//	printf("GUIDragDataGTK Init End\r\n");
}

UI::GUIDragDataGTK::~GUIDragDataGTK()
{
	GUIDragDataGTK_currData = 0;
	DEL_CLASS(this->targetMap);
	Data::ArrayList<const UTF8Char*> *targetList = this->targetText->GetValues();
	UOSInt i = targetList->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(targetList->GetItem(i));
	}
	DEL_CLASS(this->targetText);
}

/*void UI::GUIDragDataGTK::ReadData()
{
	Data::ArrayList<OSInt> *targetList = this->targetMap->GetValues();
	OSInt i = 0;
	OSInt j = targetList->GetCount();
	while (i < j)
	{
		i++;
	}
}*/

UOSInt UI::GUIDragDataGTK::GetCount()
{
	return this->targetMap->GetCount();
}

const UTF8Char *UI::GUIDragDataGTK::GetName(UOSInt index)
{
	return this->targetMap->GetKey(index);
}

Bool UI::GUIDragDataGTK::GetDataText(const UTF8Char *name, Text::StringBuilderUTF *sb)
{
	OSInt fmt = this->targetMap->Get(name);
	if (fmt == 0)
		return false;
	const UTF8Char *txt = this->targetText->Get((Int32)fmt);
	if (txt)
	{
		sb->Append(txt);
		return true;
	}
	else
	{
		return false;
	}
}

IO::Stream *UI::GUIDragDataGTK::GetDataStream(const UTF8Char *name)
{
	return 0;
}

void UI::GUIDragDataGTK::OnDataReceived(void *selData)
{
	GdkAtom target = gtk_selection_data_get_target((GtkSelectionData*)selData);
	Int32 itarget = (Int32)(OSInt)target;
	UOSInt dataSize = (UInt32)gtk_selection_data_get_length((GtkSelectionData*)selData);
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
//	printf("Selection = %d, Target = %d (%s), Data Type = %d (%s), size = %d\r\n", sel, itarget, csptr, (Int32)(OSInt)dataType, csptr2, (Int32)dataSize);
	if (this->targetText->Get(itarget))
	{
		return;
	}

	if (dataSize >= 0 && sel != 0)
	{
		Text::StringBuilderUTF8 sb;
		const UInt8 *data = gtk_selection_data_get_data((GtkSelectionData*)selData);
		if (Text::StrEquals(csptr, "application/x-moz-nativeimage"))
		{
			GdkPixbuf *pixbuf = gtk_selection_data_get_pixbuf((GtkSelectionData*)selData);
			if (pixbuf)
			{
				sb.Append((const UTF8Char*)"PixBuf Image");
				g_object_unref(pixbuf);
			}
		}
		else if (Text::StrEquals(csptr, "text/html"))
		{
			GUIDragDataGTK_AppendWC(&sb, (const UTF16Char*)data, dataSize / 2);
		}
		else if (Text::StrEquals(csptr, "text/unicode"))
		{
			GUIDragDataGTK_AppendWC(&sb, (const UTF16Char*)data, dataSize / 2);
		}
		else if (Text::StrEquals(csptr, "text/x-moz-url"))
		{
			GUIDragDataGTK_AppendWC(&sb, (const UTF16Char*)data, dataSize / 2);
		}
		else if (Text::StrEquals(csptr, "text/x-moz-url-data"))
		{
			GUIDragDataGTK_AppendWC(&sb, (const UTF16Char*)data, dataSize / 2);
		}
		else if (Text::StrEquals(csptr, "text/x-moz-url-desc"))
		{
			GUIDragDataGTK_AppendWC(&sb, (const UTF16Char*)data, dataSize / 2);
		}
		else if (Text::StrEquals(csptr, "text/_moz_htmlcontext"))
		{
			GUIDragDataGTK_AppendWC(&sb, (const UTF16Char*)data, dataSize / 2);
		}
		else if (Text::StrEquals(csptr, "text/_moz_htmlinfo"))
		{
			GUIDragDataGTK_AppendWC(&sb, (const UTF16Char*)data, dataSize / 2);
		}
		else if (Text::StrEquals(csptr, "application/x-moz-file-promise-url"))
		{
			GUIDragDataGTK_AppendWC(&sb, (const UTF16Char*)data, dataSize / 2);
		}
		else if (Text::StrEquals(csptr, "application/x-moz-file-promise-dest-filename"))
		{
			GUIDragDataGTK_AppendWC(&sb, (const UTF16Char*)data, dataSize / 2);
		}
		else if (Text::StrEquals(csptr, "application/x-moz-custom-clipdata"))
		{
			sb.AppendHexBuff(data, dataSize, ' ', Text::LineBreakType::CRLF);
		}
		else
		{
			sb.AppendC((const UTF8Char*)data, dataSize);
		}
		this->targetText->Put(itarget, Text::StrCopyNew(sb.ToString()));
	}
	else
	{
		this->targetText->Put(itarget, Text::StrCopyNew((const UTF8Char*)""));
	}
	
}

gboolean GUIDragDropGTK_OnDragMotion(GtkWidget *widget, GdkDragContext *context, gint x, gint y, guint time, gpointer user_data)
{
	UI::GUIDragDropGTK *me = (UI::GUIDragDropGTK *)user_data;
//	printf("DragMotion\r\n");
	gdk_drag_status(context, (GdkDragAction)me->OnDragMotion(context, x, y, time), time);
	return true;
}

void GUIDragDropGTK_OnDragLeave(GtkWidget *widget, GdkDragContext *context, guint time, gpointer user_data)
{
	UI::GUIDragDropGTK *me = (UI::GUIDragDropGTK *)user_data;
//	printf("DragLeave\r\n");
	me->OnDragLeave();
}

gboolean GUIDragDropGTK_OnDragDrop(GtkWidget *widget, GdkDragContext *context, gint x, gint y, guint time, gpointer user_data)
{
	UI::GUIDragDropGTK *me = (UI::GUIDragDropGTK *)user_data;
//	printf("DragDrop\r\n");

	me->OnDragDrop(context, x, y, time);
	gtk_drag_finish(context, true, true, time);
	return true;
}

void GUIDragDropGTK_OnDropData(GtkWidget *widget, GdkDragContext *context, gint x, gint y, GtkSelectionData *data, guint info, guint time, gpointer user_data)
{
//	UI::GUIDragDropGTK *me = (UI::GUIDragDropGTK *)user_data;
//	printf("DropData\r\n");
	if (GUIDragDataGTK_currData)
	{
//		printf("DropData->OnDataReceived\r\n");
		GUIDragDataGTK_currData->OnDataReceived(data);
	}
}

UI::GUIDragDropGTK::GUIDragDropGTK(void *hWnd, UI::GUIDropHandler *hdlr)
{
	this->hWnd = hWnd;
	this->hdlr = hdlr;
	this->currEff = UI::GUIDropHandler::DE_NONE;
	this->dragStarted = false;

	gtk_drag_dest_set((GtkWidget*)this->hWnd, (GtkDestDefaults)0, 0, 0, GDK_ACTION_COPY);
	g_signal_connect((GtkWindow*)this->hWnd, "drag-motion", G_CALLBACK(GUIDragDropGTK_OnDragMotion), this);
	g_signal_connect((GtkWindow*)this->hWnd, "drag-leave", G_CALLBACK(GUIDragDropGTK_OnDragLeave), this);
	g_signal_connect((GtkWindow*)this->hWnd, "drag-drop", G_CALLBACK(GUIDragDropGTK_OnDragDrop), this);
	g_signal_connect((GtkWindow*)this->hWnd, "drag-data-received", G_CALLBACK(GUIDragDropGTK_OnDropData), this);
}

UI::GUIDragDropGTK::~GUIDragDropGTK()
{
}

void UI::GUIDragDropGTK::SetHandler(UI::GUIDropHandler *hdlr)
{
	this->hdlr = hdlr;
}

Int32 UI::GUIDragDropGTK::OnDragMotion(void *context, OSInt x, OSInt y, UInt32 time)
{
	if (this->dragStarted)
	{
	}
	else
	{
		UI::GUIDragDataGTK dragData(this->hWnd, context, time, false);
//		printf("OnDragEnter\r\n");
		this->currEff = this->hdlr->DragEnter(&dragData);
		this->dragStarted = true;
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

void UI::GUIDragDropGTK::OnDragLeave()
{
	this->dragStarted = false;
}

Bool UI::GUIDragDropGTK::OnDragDrop(void *context, OSInt x, OSInt y, UInt32 time)
{

	UI::GUIDragDataGTK dragData(this->hWnd, context, time, true);
	while (gtk_events_pending())
	{
		gtk_main_iteration();
	}
//	printf("OnDragDrop\r\n");
	this->hdlr->DropData(&dragData, x, y);
	return true;
}