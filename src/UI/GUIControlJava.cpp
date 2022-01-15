#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Text/MyString.h"
#include "UI/GUIClientControl.h"
#include "UI/GUIControl.h"
#include <jni.h>

extern "C"
{
	extern void *jniEnv;
}

UI::GUIControl::GUIControl(UI::GUICore *ui, GUIClientControl *parent)
{
	NEW_CLASS(this->resizeHandlers, Data::ArrayList<UIEvent>());
	NEW_CLASS(this->resizeHandlersObjs, Data::ArrayList<void*>());
	this->ui = ui;
	this->parent = parent;
	this->selfResize = false;
	this->dockType = UI::GUIControl::DOCK_NONE;
	this->hdpi = 96.0;
	this->ddpi = 96.0;
	this->lxPos = 0;
	this->lyPos = 0;
	this->lxPos2 = 320;
	this->lyPos2 = 240;
	this->inited = false;
	this->dropHdlr = 0;
}

UI::GUIControl::~GUIControl()
{
	DEL_CLASS(this->resizeHandlers);
	DEL_CLASS(this->resizeHandlersObjs);
/*	if (this->dropHdlr)
	{
		UI::GUIDragDropGTK *dragDrop = (UI::GUIDragDropGTK*)this->dropHdlr;
		DEL_CLASS(dragDrop);
	}*/
}

void *UI::GUIControl::GetFont()
{
	///////////////////////////
	return 0;
}

void UI::GUIControl::Show()
{
	JNIEnv *env = (JNIEnv*)jniEnv;
	jclass cls = env->GetObjectClass((jobject)this->hwnd);
	jmethodID mid = env->GetMethodID(cls, "setVisible", "(Z)V");
	env->CallVoidMethod((jobject)this->hwnd, mid, true);
/*
	if (!this->inited)
	{
		g_signal_connect((GtkWidget*)this->hwnd, "configure-event", G_CALLBACK(GUIControl_SizeChanged), this);
		this->inited = true;
	}*/
}

void UI::GUIControl::Close()
{
//	gtk_widget_destroy((GtkWidget*)this->hwnd);
}

void UI::GUIControl::SetText(const UTF8Char *text)
{
	/////////////////////////////////
}

UTF8Char *UI::GUIControl::GetText(UTF8Char *buff)
{
	////////////////////////////////
	return 0;
}

Bool UI::GUIControl::GetText(Text::StringBuilderUTF *sb)
{
	///////////////////////////////
	return false;
}

void UI::GUIControl::SetSize(Double width, Double height)
{
	this->SetArea(this->lxPos, this->lyPos, this->lxPos + width, this->lyPos + height, true);
}

void UI::GUIControl::SetSizeP(OSInt width, OSInt height)
{
	this->SetArea(this->lxPos, this->lyPos, this->lxPos + width * this->ddpi / this->hdpi, this->lyPos + height * this->ddpi / this->hdpi, true);
}

void UI::GUIControl::GetSize(Double *width, Double *height)
{
	if (width)
		*width = this->lxPos2 - this->lxPos;
	if (height)
		*height = this->lyPos2 - this->lyPos;
//	printf("Control.GetSize %lf, %lf\r\n", *width, *height);
}

void UI::GUIControl::GetSizeP(OSInt *width, OSInt *height)
{
	if (width)
		*width = Double2Int32((this->lxPos2 - this->lxPos) * this->hdpi / this->ddpi);
	if (height)
		*height = Double2Int32((this->lyPos2 - this->lyPos) * this->hdpi / this->ddpi);
//	printf("Control.GetSizeP %ld, %ld\r\n", (Int32)*width, (Int32)*height);
}

void UI::GUIControl::SetPosition(Double x, Double y)
{
	SetArea(x, y, x + this->lxPos2 - this->lxPos, y + this->lyPos2 - this->lyPos, true);
}

void UI::GUIControl::GetPositionP(OSInt *x, OSInt *y)
{
	if (x)
		*x = Double2Int32(this->lxPos * this->hdpi / this->ddpi);
	if (y)
		*y = Double2Int32(this->lyPos * this->hdpi / this->ddpi);
}

void UI::GUIControl::GetScreenPosP(OSInt *x, OSInt *y)
{
	///////////////////////////////
}

void UI::GUIControl::SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn)
{
	if (left == this->lxPos && top == this->lyPos && right == this->lxPos2 && bottom == this->lyPos2)
		return;
	Double xOfst = 0;
	Double yOfst = 0;
	if (this->parent)
	{
		this->parent->GetClientOfst(&xOfst, &yOfst);
	}
	this->lxPos = left;
	this->lyPos = top;
	this->selfResize = true;

/*	if (this->parent)
	{
		void *container = this->parent->GetContainer();
		gtk_fixed_move((GtkFixed*)container, (GtkWidget*)this->hwnd, Double2Int32((left + xOfst) * this->hdpi / this->ddpi), Double2Int32((top + yOfst) * this->hdpi / this->ddpi));
	}
	gtk_widget_set_size_request((GtkWidget*)this->hwnd, Double2Int32((right - left) * this->hdpi / this->ddpi), Double2Int32((bottom - top) * this->hdpi / this->ddpi));

	gint outW;
	gint outH;
	gtk_widget_get_size_request((GtkWidget*)this->hwnd, &outW, &outH);
	if (outW == -1)
	{
		this->lxPos2 = right;
	}
	else
	{
		this->lxPos2 = left + outW * this->ddpi / this->hdpi;
	}
	if (outH == -1)
	{
		this->lyPos2 = bottom;
	}
	else
	{
		this->lyPos2 = top + outH * this->ddpi / this->hdpi;
	}*/
	this->selfResize = false;
	this->OnSizeChanged(updateScn);
}

void UI::GUIControl::SetAreaP(OSInt left, OSInt top, OSInt right, OSInt bottom, Bool updateScn)
{
	if (left == this->lxPos && top == this->lyPos && right == this->lxPos2 && bottom == this->lyPos2)
		return;
	Double xOfst = 0;
	Double yOfst = 0;
	if (this->parent)
	{
		this->parent->GetClientOfst(&xOfst, &yOfst);
	}
	this->lxPos = left * this->ddpi / this->hdpi;
	this->lyPos = top * this->ddpi / this->hdpi;
	this->selfResize = true;

/*	if (this->parent)
	{
		void *container = this->parent->GetContainer();
		gtk_fixed_move((GtkFixed*)container, (GtkWidget*)this->hwnd, Double2Int32(left + xOfst * this->hdpi / this->ddpi), Double2Int32(top + yOfst * this->hdpi / this->ddpi));
	}
	gtk_widget_set_size_request((GtkWidget*)this->hwnd, right - left, bottom - top);

	gint outW;
	gint outH;
	gtk_widget_get_size_request((GtkWidget*)this->hwnd, &outW, &outH);
	if (outW == -1)
	{
		this->lxPos2 = right * this->ddpi / this->hdpi;
	}
	else
	{
		this->lxPos2 = (left + outW) * this->ddpi / this->hdpi;
	}
	if (outH == -1)
	{
		this->lyPos2 = bottom * this->ddpi / this->hdpi;
	}
	else
	{
		this->lyPos2 = (top + outH) * this->ddpi / this->hdpi;
	}*/
	this->selfResize = false;
	this->OnSizeChanged(updateScn);
}

void UI::GUIControl::SetRect(Double left, Double top, Double width, Double height, Bool updateScn)
{
	this->SetArea(left, top, left + width, top + height, updateScn);
}

void UI::GUIControl::SetFont(const UTF8Char *name, Double size, Bool isBold)
{
/*	PangoFontDescription *font = pango_font_description_new();
	if (name)
	{
		pango_font_description_set_family(font, (const Char*)name);
	}
	pango_font_description_set_absolute_size(font, size * PANGO_SCALE);
	if (isBold)
		pango_font_description_set_weight(font, PANGO_WEIGHT_BOLD);
	gtk_widget_override_font((GtkWidget*)this->hwnd, font); 	
	pango_font_description_free(font);*/
}

void UI::GUIControl::InitFont()
{
	//////////////////////////////////////
}

void UI::GUIControl::SetDockType(UI::GUIControl::DockType dockType)
{
	if (this->dockType != dockType)
	{
		this->dockType = dockType;
		if (this->parent)
		{
			this->parent->UpdateChildrenSize(true);
		}
	}
}

UI::GUIControl::DockType UI::GUIControl::GetDockType()
{
	return this->dockType;
}

void UI::GUIControl::SetVisible(Bool isVisible)
{
//	if (isVisible)
//		gtk_widget_show((GtkWidget*)this->hwnd);
//	else
//		gtk_widget_hide((GtkWidget*)this->hwnd);
}

Bool UI::GUIControl::GetVisible()
{
	//////////////////////////////////////////
	return true;//gtk_widget_is_visible((GtkWidget*)this->window);
}

void UI::GUIControl::SetEnabled(Bool isEnable)
{
//	gtk_widget_set_sensitive((GtkWidget*)this->hwnd, isEnable);
}

void UI::GUIControl::SetBGColor(Int32 bgColor)
{
/*	if (bgColor)
	{
		GdkRGBA color;
		color.red = (bgColor & 0xff0000) / 16711680.0;
		color.green = (bgColor & 0xff00) / 65280.0;
		color.blue = (bgColor & 0xff) / 255.0;
		color.alpha = ((bgColor >> 24) & 0xff) / 255.0;
		gtk_widget_override_background_color((GtkWidget*)this->hwnd, GTK_STATE_FLAG_NORMAL, &color);
	}
	else
	{
		gtk_widget_override_background_color((GtkWidget*)this->hwnd, GTK_STATE_FLAG_NORMAL, 0);
	}*/
}

Bool UI::GUIControl::IsFormFocused()
{
	//////////////////////////////////////
	return false;
}

void UI::GUIControl::Focus()
{
//	gtk_widget_grab_focus((GtkWidget*)this->hwnd);
}

OSInt UI::GUIControl::GetScrollHPos()
{
	return 0;
}

OSInt UI::GUIControl::GetScrollVPos()
{
	return 0;
}

void UI::GUIControl::ScrollTo(OSInt x, OSInt y)
{
}

void UI::GUIControl::OnSizeChanged(Bool updateScn)
{
	if (this->selfResize)
	{
		return;
	}

/*	gint outW;
	gint outH;
	gtk_widget_get_size_request((GtkWidget*)this->hwnd, &outW, &outH);
	if (outW != -1)
	{
		this->lxPos2 = this->lxPos + outW * this->ddpi / this->hdpi;
	}
	if (outH != -1)
	{
		this->lyPos2 = this->lyPos + outH * this->ddpi / this->hdpi;
	}*/

	OSInt i = this->resizeHandlers->GetCount();
	while (i-- > 0)
	{
		this->resizeHandlers->GetItem(i)(this->resizeHandlersObjs->GetItem(i));
	}
}

void UI::GUIControl::OnPosChanged(Bool updateScn)
{
	/////////////////////////	
}

void UI::GUIControl::OnShow()
{
	/////////////////////////	
}

void UI::GUIControl::OnMonitorChanged()
{
	/////////////////////////	
}

void UI::GUIControl::HandleSizeChanged(UIEvent handler, void *userObj)
{
	this->resizeHandlers->Add(handler);
	this->resizeHandlersObjs->Add(userObj);
}

void UI::GUIControl::UpdateFont()
{
	/////////////////////////	
}

void UI::GUIControl::UpdatePos(Bool redraw)
{
	/////////////////////////	
}

void UI::GUIControl::Redraw()
{
//	gtk_widget_queue_draw((GtkWidget*)this->hwnd);
}

void UI::GUIControl::SetCapture()
{
/*	GdkDevice *dev = gtk_get_current_event_device();
	if (dev == 0)
	{
	}
	if (dev)
	{
		gdk_device_grab(dev, gtk_widget_get_window((GtkWidget*)this->hwnd), GDK_OWNERSHIP_WINDOW, TRUE, (GdkEventMask)(GDK_BUTTON_MOTION_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK), 0, GDK_CURRENT_TIME);
	}*/
}
void UI::GUIControl::ReleaseCapture()
{
/*	GdkDevice *dev = gtk_get_current_event_device();
	if (dev == 0)
	{
	}
	if (dev)
	{
		gdk_device_ungrab(dev, GDK_CURRENT_TIME);
	}*/
}

void UI::GUIControl::SetCursor(CursorType curType)
{
	///////////////////////////
	//gtk_window_set_cursor();
}

UI::GUIClientControl *UI::GUIControl::GetParent()
{
	return this->parent;
}

UI::GUIForm *UI::GUIControl::GetRootForm()
{
	UI::GUIControl *ctrl = this;
	const UTF8Char *objCls;
	while (ctrl)
	{
		objCls = ctrl->GetObjectClass();
		if (Text::StrEquals(objCls, (const UTF8Char*)"WinForm"))
		{
			if (ctrl->GetParent() == 0)
				return (UI::GUIForm*)ctrl;
		}
		ctrl = ctrl->GetParent();
	}
	return 0;
}

void *UI::GUIControl::GetHandle()
{
	return this->hwnd;
}

void *UI::GUIControl::GetHMonitor()
{
/*
#if GDK_MAJOR_VERSION > 3 || (GDK_MAJOR_VERSION == 3 && GDK_MINOR_VERSION >= 22)
	GdkDisplay *display = gtk_widget_get_display((GtkWidget*)this->hwnd);
	GdkWindow *wnd = gtk_widget_get_window((GtkWidget*)this->hwnd);
	if (display == 0)
		return 0;
	void *ret;
	if (wnd == 0)
	{
		ret = (void*)(OSInt)1;
	}
	else
	{
		GdkMonitor *mon = gdk_display_get_monitor_at_window(display, wnd);
		GdkMonitor *mon2;
		ret = (void*)(OSInt)1;
		int i = 0;
		int j = gdk_display_get_n_monitors(display);
		while (i < j)
		{
			mon2 = gdk_display_get_monitor(display, i);
			if (mon == mon2)
			{
				ret = (void*)(OSInt)(1 + i);
				break;
			}
			i++;
		}
	}
	return ret;
#else
	GdkScreen *scn = gtk_widget_get_screen((GtkWidget*)this->hwnd);
	GdkWindow *wnd = gtk_widget_get_window((GtkWidget*)this->hwnd);
	if (scn == 0)
		return 0;
	if (wnd == 0)
	{
		wnd = gdk_screen_get_active_window(scn);
	}
	void *ret;
	if (wnd == 0)
	{
		ret = (void*)(OSInt)1;
	}
	else
	{
		ret = (void*)(OSInt)(1 + gdk_screen_get_monitor_at_window(scn, wnd));
	}
	return ret;
#endif*/
}

void UI::GUIControl::SetDPI(Double hdpi, Double ddpi)
{
	this->hdpi = hdpi;
	this->ddpi = ddpi;
	this->UpdatePos(true);
	if (this->hFont)
	{
		this->InitFont();
	}
	else
	{
		this->UpdateFont();
	}
}

Double UI::GUIControl::GetHDPI()
{
	return this->hdpi;
}

Double UI::GUIControl::GetDDPI()
{
	return this->ddpi;
}

Media::DrawFont *UI::GUIControl::CreateDrawFont(Media::DrawImage *img)
{
	void *f = this->GetFont();
	if (f == 0)
		return 0;
	Media::DrawFont *fnt = (Media::DrawFont*)f;
	if (this->fontName == 0)
	{
		return img->CloneFont(fnt);
	}
	else
	{
		fnt = img->NewFontH(this->fontName, this->fontHeight * this->hdpi / this->ddpi * 72.0 / img->GetHDPI(), this->fontIsBold?Media::DrawEngine::DFS_BOLD:Media::DrawEngine::DFS_NORMAL, 0);
	}
	return fnt;
}

Int32 UI::GUIControl::GUIKey2OSKey(UI::GUIControl::GUIKey guiKey)
{
/*	switch (guiKey)
	{
	case UI::GUIControl::GK_BACKSPACE:
		return GDK_KEY_BackSpace;
	case UI::GUIControl::GK_TAB:
		return GDK_KEY_Tab;
	case UI::GUIControl::GK_CLEAR:
		return GDK_KEY_Clear;
	case UI::GUIControl::GK_ENTER:
		return GDK_KEY_Return;
	case UI::GUIControl::GK_SHIFT:
		return GDK_KEY_Shift_L;
	case UI::GUIControl::GK_CONTROL:
		return GDK_KEY_Control_L;
	case UI::GUIControl::GK_ALT:
		return GDK_KEY_Alt_L;
	case UI::GUIControl::GK_PAUSE:
		return GDK_KEY_Pause;
	case UI::GUIControl::GK_CAPITAL:
		return GDK_KEY_Caps_Lock;
	case UI::GUIControl::GK_KANA:
		return GDK_KEY_Kana_Shift;
	case UI::GUIControl::GK_JUNJA:
		return GDK_KEY_Romaji;
	case UI::GUIControl::GK_FINAL:
		return GDK_KEY_Muhenkan;
	case UI::GUIControl::GK_KANJI:
		return GDK_KEY_Kanji;
	case UI::GUIControl::GK_ESCAPE:
		return GDK_KEY_Escape;
	case UI::GUIControl::GK_CONVERT:
		return GDK_KEY_Henkan;
	case UI::GUIControl::GK_NONCONVERT:
		return GDK_KEY_Katakana;
	case UI::GUIControl::GK_ACCEPT:
		return GDK_KEY_Select;
	case UI::GUIControl::GK_MODECHANGE:
		return GDK_KEY_Mode_switch;
	case UI::GUIControl::GK_SPACE:
		return GDK_KEY_space;
	case UI::GUIControl::GK_PAGEUP:
		return GDK_KEY_Page_Up;
	case UI::GUIControl::GK_PAGEDOWN:
		return GDK_KEY_Page_Down;
	case UI::GUIControl::GK_END:
		return GDK_KEY_End;
	case UI::GUIControl::GK_HOME:
		return GDK_KEY_Home;
	case UI::GUIControl::GK_LEFT:
		return GDK_KEY_Left;
	case UI::GUIControl::GK_UP:
		return GDK_KEY_Up;
	case UI::GUIControl::GK_RIGHT:
		return GDK_KEY_Right;
	case UI::GUIControl::GK_DOWN:
		return GDK_KEY_Down;
	case UI::GUIControl::GK_SELECT:
		return GDK_KEY_Select;
	case UI::GUIControl::GK_PRINT:
		return GDK_KEY_Print;
	case UI::GUIControl::GK_EXECUTE:
		return GDK_KEY_Execute;
	case UI::GUIControl::GK_PRINTSCREEN:
		return GDK_KEY_3270_PrintScreen;
	case UI::GUIControl::GK_INSERT:
		return GDK_KEY_Insert;
	case UI::GUIControl::GK_DELETE:
		return GDK_KEY_Delete;
	case UI::GUIControl::GK_HELP:
		return GDK_KEY_Help;
	case UI::GUIControl::GK_0:
		return GDK_KEY_0;
	case UI::GUIControl::GK_1:
		return GDK_KEY_1;
	case UI::GUIControl::GK_2:
		return GDK_KEY_2;
	case UI::GUIControl::GK_3:
		return GDK_KEY_3;
	case UI::GUIControl::GK_4:
		return GDK_KEY_4;
	case UI::GUIControl::GK_5:
		return GDK_KEY_5;
	case UI::GUIControl::GK_6:
		return GDK_KEY_6;
	case UI::GUIControl::GK_7:
		return GDK_KEY_7;
	case UI::GUIControl::GK_8:
		return GDK_KEY_8;
	case UI::GUIControl::GK_9:
		return GDK_KEY_9;
	case UI::GUIControl::GK_A:
		return GDK_KEY_a;
	case UI::GUIControl::GK_B:
		return GDK_KEY_b;
	case UI::GUIControl::GK_C:
		return GDK_KEY_c;
	case UI::GUIControl::GK_D:
		return GDK_KEY_d;
	case UI::GUIControl::GK_E:
		return GDK_KEY_e;
	case UI::GUIControl::GK_F:
		return GDK_KEY_f;
	case UI::GUIControl::GK_G:
		return GDK_KEY_g;
	case UI::GUIControl::GK_H:
		return GDK_KEY_h;
	case UI::GUIControl::GK_I:
		return GDK_KEY_i;
	case UI::GUIControl::GK_J:
		return GDK_KEY_j;
	case UI::GUIControl::GK_K:
		return GDK_KEY_k;
	case UI::GUIControl::GK_L:
		return GDK_KEY_l;
	case UI::GUIControl::GK_M:
		return GDK_KEY_m;
	case UI::GUIControl::GK_N:
		return GDK_KEY_n;
	case UI::GUIControl::GK_O:
		return GDK_KEY_o;
	case UI::GUIControl::GK_P:
		return GDK_KEY_p;
	case UI::GUIControl::GK_Q:
		return GDK_KEY_q;
	case UI::GUIControl::GK_R:
		return GDK_KEY_r;
	case UI::GUIControl::GK_S:
		return GDK_KEY_s;
	case UI::GUIControl::GK_T:
		return GDK_KEY_t;
	case UI::GUIControl::GK_U:
		return GDK_KEY_u;
	case UI::GUIControl::GK_V:
		return GDK_KEY_v;
	case UI::GUIControl::GK_W:
		return GDK_KEY_w;
	case UI::GUIControl::GK_X:
		return GDK_KEY_x;
	case UI::GUIControl::GK_Y:
		return GDK_KEY_y;
	case UI::GUIControl::GK_Z:
		return GDK_KEY_z;
	case UI::GUIControl::GK_LWIN:
		return GDK_KEY_Super_L;
	case UI::GUIControl::GK_RWIN:
		return GDK_KEY_Super_R;
	case UI::GUIControl::GK_APPS:
		return GDK_KEY_WWW;
	case UI::GUIControl::GK_SLEEP:
		return GDK_KEY_Sleep;
	case UI::GUIControl::GK_NUMPAD0:
		return GDK_KEY_KP_0;
	case UI::GUIControl::GK_NUMPAD1:
		return GDK_KEY_KP_1;
	case UI::GUIControl::GK_NUMPAD2:
		return GDK_KEY_KP_2;
	case UI::GUIControl::GK_NUMPAD3:
		return GDK_KEY_KP_3;
	case UI::GUIControl::GK_NUMPAD4:
		return GDK_KEY_KP_4;
	case UI::GUIControl::GK_NUMPAD5:
		return GDK_KEY_KP_5;
	case UI::GUIControl::GK_NUMPAD6:
		return GDK_KEY_KP_6;
	case UI::GUIControl::GK_NUMPAD7:
		return GDK_KEY_KP_7;
	case UI::GUIControl::GK_NUMPAD8:
		return GDK_KEY_KP_8;
	case UI::GUIControl::GK_NUMPAD9:
		return GDK_KEY_KP_9;
	case UI::GUIControl::GK_MULTIPLY:
		return GDK_KEY_KP_Multiply;
	case UI::GUIControl::GK_ADD:
		return GDK_KEY_KP_Add;
	case UI::GUIControl::GK_SEPARATOR:
		return GDK_KEY_KP_Separator;
	case UI::GUIControl::GK_SUBTRACT:
		return GDK_KEY_KP_Subtract;
	case UI::GUIControl::GK_DECIMAL:
		return GDK_KEY_KP_Decimal;
	case UI::GUIControl::GK_DIVIDE:
		return GDK_KEY_KP_Divide;
	case UI::GUIControl::GK_F1:
		return GDK_KEY_F1;
	case UI::GUIControl::GK_F2:
		return GDK_KEY_F2;
	case UI::GUIControl::GK_F3:
		return GDK_KEY_F3;
	case UI::GUIControl::GK_F4:
		return GDK_KEY_F4;
	case UI::GUIControl::GK_F5:
		return GDK_KEY_F5;
	case UI::GUIControl::GK_F6:
		return GDK_KEY_F6;
	case UI::GUIControl::GK_F7:
		return GDK_KEY_F7;
	case UI::GUIControl::GK_F8:
		return GDK_KEY_F8;
	case UI::GUIControl::GK_F9:
		return GDK_KEY_F9;
	case UI::GUIControl::GK_F10:
		return GDK_KEY_F10;
	case UI::GUIControl::GK_F11:
		return GDK_KEY_F11;
	case UI::GUIControl::GK_F12:
		return GDK_KEY_F12;
	case UI::GUIControl::GK_F13:
		return GDK_KEY_F13;
	case UI::GUIControl::GK_F14:
		return GDK_KEY_F14;
	case UI::GUIControl::GK_F15:
		return GDK_KEY_F15;
	case UI::GUIControl::GK_F16:
		return GDK_KEY_F16;
	case UI::GUIControl::GK_F17:
		return GDK_KEY_F17;
	case UI::GUIControl::GK_F18:
		return GDK_KEY_F18;
	case UI::GUIControl::GK_F19:
		return GDK_KEY_F19;
	case UI::GUIControl::GK_F20:
		return GDK_KEY_F20;
	case UI::GUIControl::GK_F21:
		return GDK_KEY_F21;
	case UI::GUIControl::GK_F22:
		return GDK_KEY_F22;
	case UI::GUIControl::GK_F23:
		return GDK_KEY_F23;
	case UI::GUIControl::GK_F24:
		return GDK_KEY_F24;
	case UI::GUIControl::GK_NUMLOCK:
		return GDK_KEY_Num_Lock;
	case UI::GUIControl::GK_SCROLLLOCK:
		return GDK_KEY_Scroll_Lock;
	case UI::GUIControl::GK_OEM_1:
	case UI::GUIControl::GK_OEM_PLUS:
	case UI::GUIControl::GK_OEM_COMMA:
	case UI::GUIControl::GK_OEM_MINUS:
	case UI::GUIControl::GK_OEM_PERIOD:
	case UI::GUIControl::GK_OEM_2:
	case UI::GUIControl::GK_OEM_3:
	case UI::GUIControl::GK_OEM_4:
	case UI::GUIControl::GK_OEM_5:
	case UI::GUIControl::GK_OEM_6:
	case UI::GUIControl::GK_OEM_7:
	default:
		return GDK_KEY_VoidSymbol;
	}*/
}

UI::GUIControl::GUIKey UI::GUIControl::OSKey2GUIKey(Int32 osKey)
{
/*	switch (osKey)
	{
	case GDK_KEY_BackSpace:
		return UI::GUIControl::GK_BACKSPACE;
	case GDK_KEY_Tab:
		return UI::GUIControl::GK_TAB;
	case GDK_KEY_Clear:
		return UI::GUIControl::GK_CLEAR;
	case GDK_KEY_Return:
		return UI::GUIControl::GK_ENTER;
	case GDK_KEY_Shift_L:
		return UI::GUIControl::GK_SHIFT;
	case GDK_KEY_Control_L:
		return UI::GUIControl::GK_CONTROL;
	case GDK_KEY_Alt_L:
		return UI::GUIControl::GK_ALT;
	case GDK_KEY_Pause:
		return UI::GUIControl::GK_PAUSE;
	case GDK_KEY_Caps_Lock:
		return UI::GUIControl::GK_CAPITAL;
	case GDK_KEY_Kana_Shift:
		return UI::GUIControl::GK_KANA;
	case GDK_KEY_Romaji:
		return UI::GUIControl::GK_JUNJA;
	case GDK_KEY_Muhenkan:
		return UI::GUIControl::GK_FINAL;
	case GDK_KEY_Kanji:
		return UI::GUIControl::GK_KANJI;
	case GDK_KEY_Escape:
		return UI::GUIControl::GK_ESCAPE;
	case GDK_KEY_Henkan:
		return UI::GUIControl::GK_CONVERT;
	case GDK_KEY_Katakana:
		return UI::GUIControl::GK_NONCONVERT;
	case GDK_KEY_Mode_switch:
		return UI::GUIControl::GK_MODECHANGE;
	case GDK_KEY_space:
		return UI::GUIControl::GK_SPACE;
	case GDK_KEY_Page_Up:
		return UI::GUIControl::GK_PAGEUP;
	case GDK_KEY_Page_Down:
		return UI::GUIControl::GK_PAGEDOWN;
	case GDK_KEY_End:
		return UI::GUIControl::GK_END;
	case GDK_KEY_Home:
		return UI::GUIControl::GK_HOME;
	case GDK_KEY_Left:
		return UI::GUIControl::GK_LEFT;
	case GDK_KEY_Up:
		return UI::GUIControl::GK_UP;
	case GDK_KEY_Right:
		return UI::GUIControl::GK_RIGHT;
	case GDK_KEY_Down:
		return UI::GUIControl::GK_DOWN;
	case GDK_KEY_Select:
		return UI::GUIControl::GK_SELECT;
	case GDK_KEY_Print:
		return UI::GUIControl::GK_PRINT;
	case GDK_KEY_Execute:
		return UI::GUIControl::GK_EXECUTE;
	case GDK_KEY_3270_PrintScreen:
		return UI::GUIControl::GK_PRINTSCREEN;
	case GDK_KEY_Insert:
		return UI::GUIControl::GK_INSERT;
	case GDK_KEY_Delete:
		return UI::GUIControl::GK_DELETE;
	case GDK_KEY_Help:
		return UI::GUIControl::GK_HELP;
	case GDK_KEY_0:
		return UI::GUIControl::GK_0;
	case GDK_KEY_1:
		return UI::GUIControl::GK_1;
	case GDK_KEY_2:
		return UI::GUIControl::GK_2;
	case GDK_KEY_3:
		return UI::GUIControl::GK_3;
	case GDK_KEY_4:
		return UI::GUIControl::GK_4;
	case GDK_KEY_5:
		return UI::GUIControl::GK_5;
	case GDK_KEY_6:
		return UI::GUIControl::GK_6;
	case GDK_KEY_7:
		return UI::GUIControl::GK_7;
	case GDK_KEY_8:
		return UI::GUIControl::GK_8;
	case GDK_KEY_9:
		return UI::GUIControl::GK_9;
	case GDK_KEY_a:
		return UI::GUIControl::GK_A;
	case GDK_KEY_b:
		return UI::GUIControl::GK_B;
	case GDK_KEY_c:
		return UI::GUIControl::GK_C;
	case GDK_KEY_d:
		return UI::GUIControl::GK_D;
	case GDK_KEY_e:
		return UI::GUIControl::GK_E;
	case GDK_KEY_f:
		return UI::GUIControl::GK_F;
	case GDK_KEY_g:
		return UI::GUIControl::GK_G;
	case GDK_KEY_h:
		return UI::GUIControl::GK_H;
	case GDK_KEY_i:
		return UI::GUIControl::GK_I;
	case GDK_KEY_j:
		return UI::GUIControl::GK_J;
	case GDK_KEY_k:
		return UI::GUIControl::GK_K;
	case GDK_KEY_l:
		return UI::GUIControl::GK_L;
	case GDK_KEY_m:
		return UI::GUIControl::GK_M;
	case GDK_KEY_n:
		return UI::GUIControl::GK_N;
	case GDK_KEY_o:
		return UI::GUIControl::GK_O;
	case GDK_KEY_p:
		return UI::GUIControl::GK_P;
	case GDK_KEY_q:
		return UI::GUIControl::GK_Q;
	case GDK_KEY_r:
		return UI::GUIControl::GK_R;
	case GDK_KEY_s:
		return UI::GUIControl::GK_S;
	case GDK_KEY_t:
		return UI::GUIControl::GK_T;
	case GDK_KEY_u:
		return UI::GUIControl::GK_U;
	case GDK_KEY_v:
		return UI::GUIControl::GK_V;
	case GDK_KEY_w:
		return UI::GUIControl::GK_W;
	case GDK_KEY_x:
		return UI::GUIControl::GK_X;
	case GDK_KEY_y:
		return UI::GUIControl::GK_Y;
	case GDK_KEY_z:
		return UI::GUIControl::GK_Z;
	case GDK_KEY_Super_L:
		return UI::GUIControl::GK_LWIN;
	case GDK_KEY_Super_R:
		return UI::GUIControl::GK_RWIN;
	case GDK_KEY_WWW:
		return UI::GUIControl::GK_APPS;
	case GDK_KEY_Sleep:
		return UI::GUIControl::GK_SLEEP;
	case GDK_KEY_KP_0:
		return UI::GUIControl::GK_NUMPAD0;
	case GDK_KEY_KP_1:
		return UI::GUIControl::GK_NUMPAD1;
	case GDK_KEY_KP_2:
		return UI::GUIControl::GK_NUMPAD2;
	case GDK_KEY_KP_3:
		return UI::GUIControl::GK_NUMPAD3;
	case GDK_KEY_KP_4:
		return UI::GUIControl::GK_NUMPAD4;
	case GDK_KEY_KP_5:
		return UI::GUIControl::GK_NUMPAD5;
	case GDK_KEY_KP_6:
		return UI::GUIControl::GK_NUMPAD6;
	case GDK_KEY_KP_7:
		return UI::GUIControl::GK_NUMPAD7;
	case GDK_KEY_KP_8:
		return UI::GUIControl::GK_NUMPAD8;
	case GDK_KEY_KP_9:
		return UI::GUIControl::GK_NUMPAD9;
	case GDK_KEY_KP_Multiply:
		return UI::GUIControl::GK_MULTIPLY;
	case GDK_KEY_KP_Add:
		return UI::GUIControl::GK_ADD;
	case GDK_KEY_KP_Separator:
		return UI::GUIControl::GK_SEPARATOR;
	case GDK_KEY_KP_Subtract:
		return UI::GUIControl::GK_SUBTRACT;
	case GDK_KEY_KP_Decimal:
		return UI::GUIControl::GK_DECIMAL;
	case GDK_KEY_KP_Divide:
		return UI::GUIControl::GK_DIVIDE;
	case GDK_KEY_F1:
		return UI::GUIControl::GK_F1;
	case GDK_KEY_F2:
		return UI::GUIControl::GK_F2;
	case GDK_KEY_F3:
		return UI::GUIControl::GK_F3;
	case GDK_KEY_F4:
		return UI::GUIControl::GK_F4;
	case GDK_KEY_F5:
		return UI::GUIControl::GK_F5;
	case GDK_KEY_F6:
		return UI::GUIControl::GK_F6;
	case GDK_KEY_F7:
		return UI::GUIControl::GK_F7;
	case GDK_KEY_F8:
		return UI::GUIControl::GK_F8;
	case GDK_KEY_F9:
		return UI::GUIControl::GK_F9;
	case GDK_KEY_F10:
		return UI::GUIControl::GK_F10;
	case GDK_KEY_F11:
		return UI::GUIControl::GK_F11;
	case GDK_KEY_F12:
		return UI::GUIControl::GK_F12;
	case GDK_KEY_F13:
		return UI::GUIControl::GK_F13;
	case GDK_KEY_F14:
		return UI::GUIControl::GK_F14;
	case GDK_KEY_F15:
		return UI::GUIControl::GK_F15;
	case GDK_KEY_F16:
		return UI::GUIControl::GK_F16;
	case GDK_KEY_F17:
		return UI::GUIControl::GK_F17;
	case GDK_KEY_F18:
		return UI::GUIControl::GK_F18;
	case GDK_KEY_F19:
		return UI::GUIControl::GK_F19;
	case GDK_KEY_F20:
		return UI::GUIControl::GK_F20;
	case GDK_KEY_F21:
		return UI::GUIControl::GK_F21;
	case GDK_KEY_F22:
		return UI::GUIControl::GK_F22;
	case GDK_KEY_F23:
		return UI::GUIControl::GK_F23;
	case GDK_KEY_F24:
		return UI::GUIControl::GK_F24;
	case GDK_KEY_Num_Lock:
		return UI::GUIControl::GK_NUMLOCK;
	case GDK_KEY_Scroll_Lock:
		return UI::GUIControl::GK_SCROLLLOCK;
	default:
		return UI::GUIControl::GK_NONE;
	}*/
	return UI::GUIControl::GK_NONE;
}

UI::GUIControl::DragErrorType UI::GUIControl::HandleDropEvents(UI::GUIDropHandler *hdlr)
{
/*	if (this->dropHdlr)
	{
		UI::GUIDragDropGTK *dragDrop = (UI::GUIDragDropGTK*)this->dropHdlr;
		dragDrop->SetHandler(hdlr);
	}
	else
	{
		UI::GUIDragDropGTK *dragDrop;
		NEW_CLASS(dragDrop, UI::GUIDragDropGTK(this->hwnd, hdlr));
		this->dropHdlr = dragDrop;
	}*/
	return UI::GUIControl::DET_NOERROR;
}

void UI::GUIControl::DestroyObject()
{

}
