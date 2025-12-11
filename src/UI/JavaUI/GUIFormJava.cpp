#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/TextBinEnc/URIEncoding.h"
#include "UI/GUIForm.h"
#include "UI/JavaUI/JavaTimer.h"
#include <jni.h>
#include <stdio.h>

extern "C"
{
	extern JNIEnv *jniEnv;
}


/*gboolean GUIForm_Draw(GtkWidget *widget, cairo_t *cr, gpointer data)
{
	UI::GUIForm *me = (UI::GUIForm *)data;
	return me->OnPaint();
}*/

/*void GUIForm_OnFileDrop(GtkWidget *widget, GdkDragContext *context, gint x, gint y, GtkSelectionData *data, guint info, guint time, gpointer userData)
{
	UI::GUIForm *me = (UI::GUIForm *)userData;
	Text::StringBuilderUTF8 sb;
	Data::ArrayList<const UTF8Char *> files;
	sb.Append((const UTF8Char*)gtk_selection_data_get_data(data));
	Text::PString sarr[2];
	UTF8Char sbuff[512];
	OSInt i;
	OSInt j;
	sarr[1].v = sb.ToString();
	sarr[1].len = sb.GetLength();
	while (true)
	{
		i = Text::StrSplitP(sarr, 2, sarr[1].v, sarr[1].len, '\n');
		if (Text::StrStartsWithC(sarr[0].v, sarr[0].len, UTf8STRC("file://")))
		{
			j = Text::StrCharCnt(sarr[0]);
			if (sarr[0][j - 1] == '\r')
				sarr[0][j - 1] = 0;
			Text::TextEnc::URIEncoding::URIDecode(sbuff, &sarr[0][7]);
			files.Add(Text::StrCopyNew(sbuff));
		}
		if (i <= 1)
			break;
	}
	i = files.GetCount();
	if (i > 0)
	{
		me->OnFileDrop(files.GetArray(&j), i);
		while (i-- > 0)
		{
			Text::StrDelNew(files.GetItem(i));
		}
	}
}*/

extern "C"
{
//public native void windowActivated(java.awt.event.WindowEvent e);
JNIEXPORT void JNICALL Java_JFrameWindowListener_windowActivated(JNIEnv *env, jobject obj, jobject e)
{

}

//public native void windowClosed(java.awt.event.WindowEvent e);
JNIEXPORT void JNICALL Java_JFrameWindowListener_windowClosed(JNIEnv *env, jobject obj, jobject e)
{

}

//public native void windowClosing(java.awt.event.WindowEvent e);
JNIEXPORT void JNICALL Java_JFrameWindowListener_windowClosing(JNIEnv *env, jobject obj, jobject e)
{
	jclass cls = env->GetObjectClass(obj);
	jmethodID mid = env->GetMethodID(cls, "getMe", "()J");
	UI::GUIForm *me = (UI::GUIForm*)env->CallLongMethod(obj, mid);
	me->EventClosed();
	cls = env->GetObjectClass((jobject)me->GetHandle().OrNull());
	mid = env->GetMethodID(cls, "removeWindowListener", "(Ljava/awt/event/WindowListener;)V");
	env->CallVoidMethod((jobject)me->GetHandle().OrNull(), mid, obj);
}

//public native void windowDeactivated(java.awt.event.WindowEvent e);
JNIEXPORT void JNICALL Java_JFrameWindowListener_windowDeactivated(JNIEnv *env, jobject obj, jobject e)
{
/*	jclass cls = env->GetObjectClass(obj);
	jmethodID mid = env->GetMethodID(cls, "getMe", "()J");
	UI::GUIForm *me = (UI::GUIForm*)env->CallLongMethod(obj, mid);
	me->EventClosed();*/
}

//public native void windowDeiconified(java.awt.event.WindowEvent e);
JNIEXPORT void JNICALL Java_JFrameWindowListener_windowDeiconified(JNIEnv *env, jobject obj, jobject e)
{

}

//public native void windowIconified(java.awt.event.WindowEvent e);
JNIEXPORT void JNICALL Java_JFrameWindowListener_windowIconified(JNIEnv *env, jobject obj, jobject e)
{

}

//public native void windowOpened(java.awt.event.WindowEvent e);
JNIEXPORT void JNICALL Java_JFrameWindowListener_windowOpened(JNIEnv *env, jobject obj, jobject e)
{

}
}

/*void UI::GUIForm::UpdateHAcc()
{
	GtkAccelGroup *accel_group;
	if (this->hAcc == 0)
	{
		accel_group = gtk_accel_group_new();
		this->hAcc = accel_group;
		gtk_window_add_accel_group(GTK_WINDOW((GtkWidget*)this->hwnd), accel_group);
	}
	else
	{
		accel_group = (GtkAccelGroup*)this->hAcc;
	}
	if (this->menu)
	{
		OSInt i;
		Data::ArrayList<UI::GUIMenu::ShortcutKey*> keys;
		UI::GUIMenu::ShortcutKey *key;
		this->menu->GetAllKeys(&keys);
		i = keys.GetCount();
		while (i-- > 0)
		{
			key = keys.GetItem(i);
			GdkModifierType mods = (GdkModifierType)0;
			if (key->keyModifier & UI::GUIMenu::KM_ALT)
				mods = (GdkModifierType)(mods | GDK_MOD1_MASK);
			if (key->keyModifier & UI::GUIMenu::KM_CONTROL)
				mods = (GdkModifierType)(mods | GDK_CONTROL_MASK);
			if (key->keyModifier & UI::GUIMenu::KM_SHIFT)
				mods = (GdkModifierType)(mods | GDK_SHIFT_MASK);
			gtk_widget_add_accelerator((GtkWidget*)key->menuItem, "activate", accel_group, UI::GUIControl::GUIKey2OSKey(key->shortcutKey), mods, GTK_ACCEL_VISIBLE);
		}
	}
}*/

UI::GUIForm::GUIForm(Optional<GUIClientControl> parent, Double initW, Double initH, NN<GUICore> ui) : UI::GUIClientControl(ui, parent)
{
	this->exitOnClose = false;
	this->isDialog = false;
	this->dialogResult = DR_UNKNOWN;
	this->okBtn = 0;
	this->cancelBtn = 0;
	this->closingHdlr = 0;
	this->menu = 0;
	this->hAcc = 0;
	this->hwnd = 0;

	JNIEnv *env = jniEnv;
	jclass cls = env->FindClass("javax/swing/JFrame");
	if (cls == 0)
	{
		return;
	}
	jmethodID mid = env->GetMethodID(cls, "<init>", "()V");
	jmethodID mid2;
	if (mid == 0)
	{
		return;
	}
	this->hwnd = (ControlHandle*)env->NewObject(cls, mid);
	mid = env->GetMethodID(cls, "getContentPane", "()Ljava/awt/Container;");
	if (mid)
	{
		this->container = env->CallObjectMethod((jobject)this->hwnd.OrNull(), mid);
	}
	cls = env->FindClass("JFrameWindowListener");
	mid = env->GetMethodID(cls, "<init>", "(J)V");
	jobject listener = env->NewObject(cls, mid, this);
	cls = env->GetObjectClass((jobject)this->hwnd.OrNull());
	mid = env->GetMethodID(cls, "addWindowListener", "(Ljava/awt/event/WindowListener;)V");
	env->CallVoidMethod((jobject)this->hwnd.OrNull(), mid, listener);

	this->selfResize = true;
	Double w;
	Double h;
	Double initX;
	Double initY;
	NN<GUIClientControl> nnparent;
	if (parent.SetTo(nnparent))
	{
		Math::Size2DDbl size = nnparent->GetClientSize();
		initX = (size.x - initW) * 0.5;
		initY = (size.y - initH) * 0.5;
	}
	else
	{
		cls = env->FindClass("java/awt/MouseInfo");
		mid = env->GetStaticMethodID(cls, "getPointerInfo", "()Ljava/awt/PointerInfo;");
		jobject pointer = env->CallStaticObjectMethod(cls, mid); //PointerInfo
		cls = env->GetObjectClass(pointer);
		mid = env->GetMethodID(cls, "getDevice", "()Ljava/awt/GraphicsDevice;");
		jobject gd = env->CallObjectMethod(pointer, mid);
		cls = env->GetObjectClass(gd);
		mid = env->GetMethodID(cls, "getDisplayMode", "()Ljava/awt/DisplayMode;");
		jobject dm = env->CallObjectMethod(gd, mid);
		cls = env->GetObjectClass(dm);
		mid = env->GetMethodID(cls, "getWidth", "()I");
		mid2 = env->GetMethodID(cls, "getHeight", "()I");

		w = env->CallIntMethod(dm, mid) * 96.0 / this->hdpi;
		h = env->CallIntMethod(dm, mid2) * 96.0 / this->hdpi;
		initX = ((w - initW) * 0.5);
		initY = ((h - initH) * 0.5);
	}
	cls = env->GetObjectClass((jobject)this->hwnd.OrNull());
	mid = env->GetMethodID(cls, "setBounds", "(IIII)V");
	if (mid)
	{
		env->CallVoidMethod((jobject)this->hwnd.OrNull(), mid, Double2Int32(initX), Double2Int32(initY), Double2Int32(initW * this->hdpi / 96.0), Double2Int32(initH * this->hdpi / 96.0));
	}
	//	g_signal_connect((GtkWindow*)this->hwnd, "draw", G_CALLBACK(GUIForm_Draw), this);
	this->lxPos = initX;
	this->lyPos = initY;
	this->lxPos2 = initX + initW;
	this->lyPos2 = initY + initH;
	this->selfResize = false;
}

UI::GUIForm::~GUIForm()
{
	this->timers.DeleteAll();
	this->menu.Delete();
}

/*void UI::GUIForm::SetText(const WChar *text)
{
	const Char *title = Text::StrToUTF8New(text);
	gtk_window_set_title((GtkWindow*)this->hwnd, title);
	Text::StrDelNew(title);
}*/

void UI::GUIForm::SetFormState(FormState fs)
{
	JNIEnv *env = jniEnv;
	jclass cls = env->GetObjectClass((jobject)this->hwnd.OrNull());
	jfieldID fid;
	switch (fs)
	{
	case FS_MAXIMIZED:
		fid = env->GetStaticFieldID(cls, "MAXIMIZED_BOTH", "I");
		break;
	case FS_NORMAL:
		fid = env->GetStaticFieldID(cls, "NORMAL", "I");
		break;
	case FS_MINIMIZED:
		fid = env->GetStaticFieldID(cls, "ICONIFIED", "I");
		break;
	default:
		return;
	}
	jmethodID mid = env->GetMethodID(cls, "setExtendedState", "(I)V");
	env->CallVoidMethod((jobject)this->hwnd.OrNull(), mid, env->GetStaticIntField(cls, fid));
}

UI::GUIForm::DialogResult UI::GUIForm::ShowDialog(Optional<UI::GUIForm> owner)
{
	NN<UI::GUIForm> nnowner;
	if (owner.SetTo(nnowner))
	{
		nnowner->SetEnabled(false);
	}
	this->isDialog = true;
	this->Show();
	while (this->isDialog)
	{
		Sync::SimpleThread::Sleep(100);
		//gtk_main_iteration();
	}
	if (owner.SetTo(nnowner))
	{
		nnowner->SetEnabled(true);
		nnowner->MakeForeground();
	}
	return this->dialogResult;
}

void UI::GUIForm::SetDialogResult(DialogResult dr)
{
	if (this->isDialog)
	{
		this->dialogResult = dr;
		this->Close();
	}
}

void UI::GUIForm::SetAlwaysOnTop(Bool alwaysOnTop)
{
	JNIEnv *env = jniEnv;
	jclass cls = env->GetObjectClass((jobject)this->hwnd.OrNull());
	jmethodID mid = env->GetMethodID(cls, "setAlwaysOnTop", "(Z)V");
	env->CallVoidMethod((jobject)this->hwnd.OrNull(), mid, alwaysOnTop);
}

void UI::GUIForm::MakeForeground()
{
	JNIEnv *env = jniEnv;
	jclass cls = env->GetObjectClass((jobject)this->hwnd.OrNull());
	jmethodID mid = env->GetMethodID(cls, "requestFocus", "()V");
	env->CallVoidMethod((jobject)this->hwnd.OrNull(), mid);
}

void UI::GUIForm::Close()
{
	JNIEnv *env = jniEnv;
	jclass cls = env->GetObjectClass((jobject)this->hwnd.OrNull());
	jmethodID mid = env->GetMethodID(cls, "setVisible", "(Z)V");
	env->CallVoidMethod((jobject)this->hwnd.OrNull(), mid, false);
}

void UI::GUIForm::SetText(Text::CStringNN text)
{
	JNIEnv *env = jniEnv;
	jclass cls = env->GetObjectClass((jobject)this->hwnd.OrNull());
	jmethodID mid = env->GetMethodID(cls, "setTitle", "(Ljava/lang/String;)V");
	env->CallVoidMethod((jobject)this->hwnd.OrNull(), mid, env->NewStringUTF((const Char*)text.v.Ptr()));
}

Math::Size2D<UOSInt> UI::GUIForm::GetSizeP()
{
	JNIEnv *env = jniEnv;
	jclass cls = env->GetObjectClass((jobject)this->hwnd.OrNull());
	jmethodID mid;
	mid = env->GetMethodID(cls, "getWidth", "()I");
	Int32 width = env->CallIntMethod((jobject)this->hwnd.OrNull(), mid) * this->ddpi / this->hdpi;
	mid = env->GetMethodID(cls, "getHeight", "()I");
	Int32 height = env->CallIntMethod((jobject)this->hwnd.OrNull(), mid) * this->ddpi / this->hdpi;
	return Math::Size2D<UOSInt>((UOSInt)width, (UOSInt)height);
//	printf("Form GetSizeP: %ld, %ld\r\n", (Int32)*width, (Int32)*height);
}

void UI::GUIForm::SetExitOnClose(Bool exitOnClose)
{
	this->exitOnClose = exitOnClose;
}

void UI::GUIForm::SetNoResize(Bool noResize)
{
	JNIEnv *env = jniEnv;
	jclass cls = env->GetObjectClass((jobject)this->hwnd.OrNull());
	jmethodID mid = env->GetMethodID(cls, "setResizable", "(Z)V");
	env->CallVoidMethod((jobject)this->hwnd.OrNull(), mid, !noResize);
	if (noResize)
	{
	//	gtk_widget_set_size_request((GtkWidget*)this->hwnd, Double2Int32((this->lxPos2 - this->lxPos)* this->hdpi / 96.0), Double2Int32((this->lyPos2 - this->lyPos) * this->hdpi / 96.0));
	}
}

NN<UI::GUITimer> UI::GUIForm::AddTimer(UInt32 interval, UI::UIEvent handler, AnyType userObj)
{
	NN<UI::JavaUI::JavaTimer> tmr;
	NEW_CLASSNN(tmr, UI::JavaUI::JavaTimer(interval, handler, userObj));
	this->timers.Add(tmr);
	return tmr;
}

void UI::GUIForm::RemoveTimer(NN<UI::GUITimer> tmr)
{
	UOSInt i;
	i = this->timers.GetCount();	
	while (i-- > 0)
	{
		if (tmr.Ptr() == this->timers.GetItem(i).OrNull())
		{
			this->timers.RemoveAt(i);
			tmr.Delete();
			break;
		}
	}
}

void UI::GUIForm::SetMenu(NN<UI::GUIMainMenu> menu)
{
	this->menu.Delete();
/*	if (this->container == 0) this->InitContainer();
	ClientControlData *data = (ClientControlData*)this->container;
	GtkWidget *menuBar = (GtkWidget*)menu->GetHMenu();
//	gtk_container_add(GTK_CONTAINER((GtkWidget*)this->hwnd), menuBar);
	g_object_ref(data->scrolledWin);
	gtk_container_remove(GTK_CONTAINER(data->vbox), data->scrolledWin);
	gtk_box_pack_start((GtkBox*)data->vbox, menuBar, false, false, 0);
	gtk_box_pack_start(GTK_BOX(data->vbox), data->scrolledWin, true, true, 0);
	g_object_unref(data->scrolledWin);
	gtk_widget_show(menuBar);
	this->menu = menu;
	this->menu->SetMenuForm(this);
	this->UpdateHAcc();*/
}

Optional<UI::GUIMainMenu> UI::GUIForm::GetMenu()
{
	return this->menu;
}

void UI::GUIForm::UpdateMenu()
{
	this->UpdateHAcc();
}

void UI::GUIForm::SetDefaultButton(NN<UI::GUIButton> btn)
{
	this->okBtn = btn;
//	btn->SetDefaultBtnLook();
}

void UI::GUIForm::SetCancelButton(NN<UI::GUIButton> btn)
{
	this->cancelBtn = btn;
}

Math::Size2DDbl UI::GUIForm::GetClientSize()
{
	JNIEnv *env = jniEnv;
	jclass cls = env->GetObjectClass((jobject)this->container);
	jmethodID mid;
	mid = env->GetMethodID(cls, "getWidth", "()I");
	Int32 width = env->CallIntMethod((jobject)this->container, mid) * this->ddpi / this->hdpi;
	mid = env->GetMethodID(cls, "getHeight", "()I");
	Int32 height = env->CallIntMethod((jobject)this->container, mid) * this->ddpi / this->hdpi;
	return Math::Size2DDbl(width, height);
}

Bool UI::GUIForm::IsChildVisible()
{
	return true;
}

Text::CStringNN UI::GUIForm::GetObjectClass() const
{
	return CSTR("WinForm");
}

OSInt UI::GUIForm::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GUIForm::OnSizeChanged(Bool updateScn)
{
	if (this->selfResize)
	{
		return;
	}
	Bool sizeChg = false;

	Double newSize;
	int outW;
	int outH;
	JNIEnv *env = jniEnv;
	jclass cls = env->GetObjectClass((jobject)this->hwnd.OrNull());
	jmethodID mid = env->GetMethodID(cls, "getWidth", "()I");
	outW = env->CallIntMethod((jobject)this->hwnd.OrNull(), mid);
	mid = env->GetMethodID(cls, "getHeight", "()I");
	outH = env->CallIntMethod((jobject)this->hwnd.OrNull(), mid);
	if (outW != -1)
	{
		newSize = this->lxPos + outW * this->ddpi / this->hdpi;
		if (newSize != this->lxPos2)
		{
			this->lxPos2 = newSize;
			sizeChg = true;
		}
	}
	if (outH != -1)
	{
		newSize = this->lyPos + outH * this->ddpi / this->hdpi;
		if (newSize != this->lyPos2)
		{
			this->lyPos2 = newSize;
			sizeChg = true;
		}
	}
	if (sizeChg)
	{
		this->UpdateChildrenSize(false);
		this->selfResize = true;
		OSInt i = this->resizeHandlers.GetCount();
		while (i-- > 0)
		{
			Data::CallbackStorage<UI::UIEvent> evt = this->resizeHandlers.GetItem(i);
			evt.func(evt.userObj);
		}
		this->selfResize = false;
	}
}

Bool UI::GUIForm::OnPaint()
{
	return false;
}

void UI::GUIForm::EventMenuClicked(UInt16 cmdId)
{
	OSInt i;
	i = this->menuClickedHandlers.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<UI::MenuEvent> evt = this->menuClickedHandlers.GetItem(i);
		evt.func(evt.userObj, cmdId);
	}
}

void UI::GUIForm::ShowMouseCursor(Bool toShow)
{
//	GdkCursor *cursor = gdk_cursor_new_from_name(gdk_display_get_default(), toShow?"default":"none");
//	gdk_window_set_cursor(gtk_widget_get_window((GtkWidget*)this->hwnd), cursor);
}

void UI::GUIForm::HandleFormClosed(FormClosedEvent handler, AnyType userObj)
{
	this->closeHandlers.Add(Data::CallbackStorage<FormClosedEvent>(handler, userObj));
}

void UI::GUIForm::HandleDropFiles(FileEvent handler, AnyType userObj)
{
	this->dropFileHandlers.Add({handler, userObj});
/*	static GtkTargetEntry target_table[] = {
	{ (gchar*)"text/uri-list", 0, 0 }
	};

	gtk_drag_dest_set((GtkWidget*)this->hwnd, GTK_DEST_DEFAULT_ALL, target_table, 1, GDK_ACTION_COPY);
	g_signal_connect((GtkWindow*)this->hwnd, "drag-data-received", G_CALLBACK(GUIForm_OnFileDrop), this);*/
}

void UI::GUIForm::HandleMenuClicked(MenuEvent handler, AnyType userObj)
{
	this->menuClickedHandlers.Add({handler, userObj});
}

void UI::GUIForm::HandleKeyDown(KeyEvent handler, AnyType userObj)
{
	this->keyDownHandlers.Add({handler, userObj});
}

void UI::GUIForm::HandleKeyUp(KeyEvent handler, AnyType userObj)
{
	this->keyUpHandlers.Add({handler, userObj});
}

void UI::GUIForm::SetClosingHandler(FormClosingEvent handler, AnyType userObj)
{
	this->closingHdlr = {handler, userObj};
}

void UI::GUIForm::SetDPI(Double hdpi, Double ddpi)
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

/*	if (this->menu)
	{
		this->menu->SetDPI(hdpi, ddpi);
	}*/
	UOSInt i = this->children.GetCount();
	while (i-- > 0)
	{
		this->children.GetItemNoCheck(i)->SetDPI(hdpi, ddpi);
	}
	this->UpdateChildrenSize(true);
}

void UI::GUIForm::EventClosed()
{
	UOSInt i;
	i = this->closeHandlers.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<FormClosedEvent> evt = this->closeHandlers.GetItem(i);
		evt.func(evt.userObj, *this);
	}
	if (this->exitOnClose)
	{
		this->ui->Exit();
	}
	if (!this->isDialog)
	{
		this->DestroyObject();
		DEL_CLASS(this);
	}
	else
	{
		this->isDialog = false;
	}
}

void UI::GUIForm::OnFocus()
{
}

void UI::GUIForm::OnFocusLost()
{
}

void UI::GUIForm::OnDisplaySizeChange(UOSInt dispWidth, UOSInt dispHeight)
{
}

void UI::GUIForm::OnFileDrop(Data::DataArray<NN<Text::String>> files)
{
	OSInt i;
	i = this->dropFileHandlers.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<FileEvent> evt = this->dropFileHandlers.GetItem(i);
		evt.func(evt.userObj, files);
	}
}

void UI::GUIForm::ToFullScn()
{
//	gtk_window_fullscreen((GtkWindow*)this->hwnd);
}

void UI::GUIForm::FromFullScn()
{
//	gtk_window_unfullscreen((GtkWindow*)this->hwnd);
}

NN<UI::GUICore> UI::GUIForm::GetUI()
{
	return this->ui;
}
