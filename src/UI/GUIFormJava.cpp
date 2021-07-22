#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/TextEnc/URIEncoding.h"
#include "UI/GUIForm.h"
#include "UI/GUITimer.h"
#include <jni.h>
#include <stdio.h>

extern "C"
{
	extern void *jniEnv;
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
	UTF8Char *sarr[2];
	UTF8Char sbuff[512];
	OSInt i;
	OSInt j;
	sarr[1] = sb.ToString();
	while (true)
	{
		i = Text::StrSplit(sarr, 2, sarr[1], '\n');
		if (Text::StrStartsWith(sarr[0], (const UTF8Char*)"file://"))
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
	cls = env->GetObjectClass((jobject)me->GetHandle());
	mid = env->GetMethodID(cls, "removeWindowListener", "(Ljava/awt/event/WindowListener;)V");
	env->CallVoidMethod((jobject)me->GetHandle(), mid, obj);
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

UI::GUIForm::GUIForm(GUIClientControl *parent, Double initW, Double initH, GUICore *ui) : UI::GUIClientControl(ui, parent)
{
	this->exitOnClose = false;
	this->isDialog = false;
	this->dialogResult = DR_UNKNOWN;
	this->timers = 0;
	this->okBtn = 0;
	this->cancelBtn = 0;
	NEW_CLASS(this->closeHandlers, Data::ArrayList<FormClosedEvent>());
	NEW_CLASS(this->closeHandlersObj, Data::ArrayList<void *>());
	NEW_CLASS(this->dropFileHandlers, Data::ArrayList<FileEvent>());
	NEW_CLASS(this->dropFileHandlersObj, Data::ArrayList<void *>());
	NEW_CLASS(this->menuClickedHandlers, Data::ArrayList<MenuEvent>());
	NEW_CLASS(this->menuClickedHandlersObj, Data::ArrayList<void *>());
	NEW_CLASS(this->keyDownHandlers, Data::ArrayList<KeyEvent>());
	NEW_CLASS(this->keyDownHandlersObj, Data::ArrayList<void *>());
	NEW_CLASS(this->keyUpHandlers, Data::ArrayList<KeyEvent>());
	NEW_CLASS(this->keyUpHandlersObj, Data::ArrayList<void *>());
	NEW_CLASS(this->timers, Data::ArrayList<UI::GUITimer*>());
	this->closingHdlr = 0;
	this->closingHdlrObj = 0;
	this->menu = 0;
	this->hAcc = 0;
	this->hwnd = 0;

	JNIEnv *env = (JNIEnv*)jniEnv;
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
	this->hwnd = env->NewObject(cls, mid);
	mid = env->GetMethodID(cls, "getContentPane", "()Ljava/awt/Container;");
	if (mid)
	{
		this->container = env->CallObjectMethod((jobject)this->hwnd, mid);
	}
	cls = env->FindClass("JFrameWindowListener");
	mid = env->GetMethodID(cls, "<init>", "(J)V");
	jobject listener = env->NewObject(cls, mid, this);
	cls = env->GetObjectClass((jobject)this->hwnd);
	mid = env->GetMethodID(cls, "addWindowListener", "(Ljava/awt/event/WindowListener;)V");
	env->CallVoidMethod((jobject)this->hwnd, mid, listener);

	this->selfResize = true;
	Double w;
	Double h;
	Double initX;
	Double initY;
	if (parent)
	{
		parent->GetClientSize(&w, &h);
		initX = (w - initW) * 0.5;
		initY = (h - initH) * 0.5;
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
	cls = env->GetObjectClass((jobject)this->hwnd);
	mid = env->GetMethodID(cls, "setBounds", "(IIII)V");
	if (mid)
	{
		env->CallVoidMethod((jobject)this->hwnd, mid, Math::Double2Int32(initX), Math::Double2Int32(initY), Math::Double2Int32(initW * this->hdpi / 96.0), Math::Double2Int32(initH * this->hdpi / 96.0));
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
	GUITimer *tmr;
	if (this->timers != 0)
	{
		OSInt i;
		i = this->timers->GetCount();
		while (i-- > 0)
		{
			tmr = this->timers->GetItem(i);
			DEL_CLASS(tmr);
		}
		DEL_CLASS(this->timers);
	}
	DEL_CLASS(this->dropFileHandlers);
	DEL_CLASS(this->dropFileHandlersObj);
	DEL_CLASS(this->closeHandlers);
	DEL_CLASS(this->closeHandlersObj);
	DEL_CLASS(this->menuClickedHandlers);
	DEL_CLASS(this->menuClickedHandlersObj);
	DEL_CLASS(this->keyDownHandlers);
	DEL_CLASS(this->keyDownHandlersObj);
	DEL_CLASS(this->keyUpHandlers);
	DEL_CLASS(this->keyUpHandlersObj);
	if (this->menu)
	{
		DEL_CLASS(this->menu);
		this->menu = 0;
	}
}

/*void UI::GUIForm::SetText(const WChar *text)
{
	const Char *title = Text::StrToUTF8New(text);
	gtk_window_set_title((GtkWindow*)this->hwnd, title);
	Text::StrDelNew(title);
}*/

void UI::GUIForm::SetFormState(FormState fs)
{
	JNIEnv *env = (JNIEnv*)jniEnv;
	jclass cls = env->GetObjectClass((jobject)this->hwnd);
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
	env->CallVoidMethod((jobject)this->hwnd, mid, env->GetStaticIntField(cls, fid));
}

UI::GUIForm::DialogResult UI::GUIForm::ShowDialog(UI::GUIForm *owner)
{
	if (owner)
	{
		owner->SetEnabled(false);
	}
	this->isDialog = true;
	this->Show();
	while (this->isDialog)
	{
		Sync::Thread::Sleep(100);
		//gtk_main_iteration();
	}
	if (owner)
	{
		owner->SetEnabled(true);
		owner->MakeForeground();
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
	JNIEnv *env = (JNIEnv*)jniEnv;
	jclass cls = env->GetObjectClass((jobject)this->hwnd);
	jmethodID mid = env->GetMethodID(cls, "setAlwaysOnTop", "(Z)V");
	env->CallVoidMethod((jobject)this->hwnd, mid, alwaysOnTop);
}

void UI::GUIForm::MakeForeground()
{
	JNIEnv *env = (JNIEnv*)jniEnv;
	jclass cls = env->GetObjectClass((jobject)this->hwnd);
	jmethodID mid = env->GetMethodID(cls, "requestFocus", "()V");
	env->CallVoidMethod((jobject)this->hwnd, mid);
}

void UI::GUIForm::Close()
{
	JNIEnv *env = (JNIEnv*)jniEnv;
	jclass cls = env->GetObjectClass((jobject)this->hwnd);
	jmethodID mid = env->GetMethodID(cls, "setVisible", "(Z)V");
	env->CallVoidMethod((jobject)this->hwnd, mid, false);
}

void UI::GUIForm::SetText(const UTF8Char *text)
{
	JNIEnv *env = (JNIEnv*)jniEnv;
	jclass cls = env->GetObjectClass((jobject)this->hwnd);
	jmethodID mid = env->GetMethodID(cls, "setTitle", "(Ljava/lang/String;)V");
	env->CallVoidMethod((jobject)this->hwnd, mid, env->NewStringUTF((const Char*)text));
}

void UI::GUIForm::GetSizeP(OSInt *width, OSInt *height)
{
	JNIEnv *env = (JNIEnv*)jniEnv;
	jclass cls = env->GetObjectClass((jobject)this->hwnd);
	jmethodID mid;
	if (width)
	{
		mid = env->GetMethodID(cls, "getWidth", "()I");
		*width = env->CallIntMethod((jobject)this->hwnd, mid) * this->ddpi / this->hdpi;
	}
	if (height)
	{
		mid = env->GetMethodID(cls, "getHeight", "()I");
		*height = env->CallIntMethod((jobject)this->hwnd, mid) * this->ddpi / this->hdpi;
	}
//	printf("Form GetSizeP: %ld, %ld\r\n", (Int32)*width, (Int32)*height);
}

void UI::GUIForm::SetExitOnClose(Bool exitOnClose)
{
	this->exitOnClose = exitOnClose;
}

void UI::GUIForm::SetNoResize(Bool noResize)
{
	JNIEnv *env = (JNIEnv*)jniEnv;
	jclass cls = env->GetObjectClass((jobject)this->hwnd);
	jmethodID mid = env->GetMethodID(cls, "setResizable", "(Z)V");
	env->CallVoidMethod((jobject)this->hwnd, mid, !noResize);
	if (noResize)
	{
	//	gtk_widget_set_size_request((GtkWidget*)this->hwnd, Math::Double2Int32((this->lxPos2 - this->lxPos)* this->hdpi / 96.0), Math::Double2Int32((this->lyPos2 - this->lyPos) * this->hdpi / 96.0));
	}
}

UI::GUITimer *UI::GUIForm::AddTimer(UInt32 interval, UI::UIEvent handler, void *userObj)
{
	if (this->timers == 0)
	{
		NEW_CLASS(this->timers, Data::ArrayList<UI::GUITimer*>());
	}
	UI::GUITimer *tmr;
	NEW_CLASS(tmr, UI::GUITimer(this->ui, this, 0, interval, handler, userObj));
	this->timers->Add(tmr);
	return tmr;
}

void UI::GUIForm::RemoveTimer(UI::GUITimer *tmr)
{
	if (this->timers)
	{
		OSInt i;
		i = this->timers->GetCount();	
		while (i-- > 0)
		{
			if (tmr == this->timers->GetItem(i))
			{
				this->timers->RemoveAt(i);
				DEL_CLASS(tmr);
				break;
			}
		}
	}
}

void UI::GUIForm::SetMenu(UI::GUIMainMenu *menu)
{
	if (this->menu)
	{
		DEL_CLASS(this->menu);
	}
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

UI::GUIMainMenu *UI::GUIForm::GetMenu()
{
	return this->menu;
}

void UI::GUIForm::UpdateMenu()
{
	this->UpdateHAcc();
}

void UI::GUIForm::SetDefaultButton(UI::GUIButton *btn)
{
	this->okBtn = btn;
//	btn->SetDefaultBtnLook();
}

void UI::GUIForm::SetCancelButton(UI::GUIButton *btn)
{
	this->cancelBtn = btn;
}

Bool UI::GUIForm::IsChildVisible()
{
	return true;
}

const UTF8Char *UI::GUIForm::GetObjectClass()
{
	return (const UTF8Char*)"WinForm";
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
	JNIEnv *env = (JNIEnv*)jniEnv;
	jclass cls = env->GetObjectClass((jobject)this->hwnd);
	jmethodID mid = env->GetMethodID(cls, "getWidth", "()I");
	outW = env->CallIntMethod((jobject)this->hwnd, mid);
	mid = env->GetMethodID(cls, "getHeight", "()I");
	outH = env->CallIntMethod((jobject)this->hwnd, mid);
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
		OSInt i = this->resizeHandlers->GetCount();
		while (i-- > 0)
		{
			this->resizeHandlers->GetItem(i)(this->resizeHandlersObjs->GetItem(i));
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
	i = this->menuClickedHandlers->GetCount();
	while (i-- > 0)
	{
		this->menuClickedHandlers->GetItem(i)(this->menuClickedHandlersObj->GetItem(i), cmdId);
	}
}

void UI::GUIForm::ShowMouseCursor(Bool toShow)
{
//	GdkCursor *cursor = gdk_cursor_new_from_name(gdk_display_get_default(), toShow?"default":"none");
//	gdk_window_set_cursor(gtk_widget_get_window((GtkWidget*)this->hwnd), cursor);
}

void UI::GUIForm::HandleFormClosed(FormClosedEvent handler, void *userObj)
{
	this->closeHandlers->Add(handler);
	this->closeHandlersObj->Add(userObj);
}

void UI::GUIForm::HandleDropFiles(FileEvent handler, void *userObj)
{
	this->dropFileHandlers->Add(handler);
	this->dropFileHandlersObj->Add(userObj);
/*	static GtkTargetEntry target_table[] = {
	{ (gchar*)"text/uri-list", 0, 0 }
	};

	gtk_drag_dest_set((GtkWidget*)this->hwnd, GTK_DEST_DEFAULT_ALL, target_table, 1, GDK_ACTION_COPY);
	g_signal_connect((GtkWindow*)this->hwnd, "drag-data-received", G_CALLBACK(GUIForm_OnFileDrop), this);*/
}

void UI::GUIForm::HandleMenuClicked(MenuEvent handler, void *userObj)
{
	this->menuClickedHandlers->Add(handler);
	this->menuClickedHandlersObj->Add(userObj);
}

void UI::GUIForm::HandleKeyDown(KeyEvent handler, void *userObj)
{
	this->keyDownHandlers->Add(handler);
	this->keyDownHandlersObj->Add(userObj);
}

void UI::GUIForm::HandleKeyUp(KeyEvent handler, void *userObj)
{
	this->keyUpHandlers->Add(handler);
	this->keyUpHandlersObj->Add(userObj);
}

void UI::GUIForm::SetClosingHandler(FormClosingEvent handler, void *userObj)
{
	this->closingHdlr = handler;
	this->closingHdlrObj = userObj;
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
	OSInt i = this->children->GetCount();
	while (i-- > 0)
	{
		this->children->GetItem(i)->SetDPI(hdpi, ddpi);
	}
	this->UpdateChildrenSize(true);
}

void UI::GUIForm::EventClosed()
{
	OSInt i;
	i = this->closeHandlers->GetCount();
	while (i-- > 0)
	{
		this->closeHandlers->GetItem(i)(this->closeHandlersObj->GetItem(i), this);
	}
	if (this->exitOnClose)
	{
		this->ui->Exit();
	}
	if (!this->isDialog)
	{
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

void UI::GUIForm::OnFileDrop(const UTF8Char **files, OSInt nFiles)
{
	OSInt i;
	i = this->dropFileHandlers->GetCount();
	while (i-- > 0)
	{
		this->dropFileHandlers->GetItem(i)(this->dropFileHandlersObj->GetItem(i), files, nFiles);
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

UI::GUICore *UI::GUIForm::GetUI()
{
	return this->ui;
}
