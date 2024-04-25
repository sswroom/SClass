#ifndef _SM_UI_GUIFORM
#define _SM_UI_GUIFORM
#include "Data/ArrayListNN.h"
#include "Data/CallbackStorage.h"
#include "Text/String.h"
#include "UI/GUICore.h"
#include "UI/GUIClientControl.h"
#include "UI/GUIMainMenu.h"

namespace UI
{
	class GUITimer;
	class GUIButton;
	class GUIIcon;

	typedef void (__stdcall *FileEvent)(AnyType userObj, Data::DataArray<NN<Text::String>> files);
	typedef void (__stdcall *MenuEvent)(AnyType userObj, UInt16 cmdId);

	class GUIForm : public GUIClientControl
	{
	public:
		typedef enum
		{
			FS_NORMAL,
			FS_MAXIMIZED,
			FS_MINIMIZED
		} FormState;

		typedef enum
		{
			DR_UNKNOWN,
			DR_OK,
			DR_CANCEL
		} DialogResult;

		typedef enum
		{
			CR_USER
		} CloseReason;
		typedef Bool (__stdcall *FormClosingEvent)(AnyType userObj, CloseReason reason); //true to cancel
		typedef void (__stdcall *FormClosedEvent)(AnyType userObj, NN<UI::GUIForm> frm);
		typedef void (__stdcall *KeyEvent)(AnyType userObj, UOSInt keyCode, Bool extendedKey);

	private:
		Bool virtualMode;
		Data::ArrayList<Data::CallbackStorage<FormClosedEvent>> closeHandlers;
		Data::ArrayList<Data::CallbackStorage<FileEvent>> dropFileHandlers;
		Data::ArrayList<Data::CallbackStorage<MenuEvent>> menuClickedHandlers;
		Data::ArrayList<Data::CallbackStorage<KeyEvent>> keyDownHandlers;
		Data::ArrayList<Data::CallbackStorage<KeyEvent>> keyUpHandlers;
		Data::ArrayListNN<GUITimer> timers;
		Data::CallbackStorage<FormClosingEvent> closingHdlr;
		Bool exitOnClose;
		UInt32 nextTmrId;
		Optional<UI::GUIMainMenu> menu;
		Optional<UI::GUIButton> okBtn;
		Optional<UI::GUIButton> cancelBtn;

		Bool fs;
		UInt32 fsStyle;
		Int32 fsX;
		Int32 fsY;
		Int32 fsW;
		Int32 fsH;

		UI::GUIForm *currDialog;
		Bool isDialog;
		void *hAcc;
	protected:
		DialogResult dialogResult;

	private:
		static Int32 useCnt;
		static OSInt __stdcall FormWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam);
		static void Init(InstanceHandle *hInst);
		static void Deinit(InstanceHandle *hInst);

		void UpdateHAcc();
		GUIForm(NN<UI::GUICore> ui, ControlHandle *hWnd);
	public:
		static GUIForm *FindForm(NN<UI::GUICore> ui, const UTF8Char *formName);

	public:
		GUIForm(Optional<UI::GUIClientControl> parent, Double initW, Double initH, NN<UI::GUICore> ui);
		virtual ~GUIForm();

		void SetFormState(FormState fs);
		DialogResult ShowDialog(Optional<UI::GUIForm> owner);
		void SetDialogResult(DialogResult dr);
		void ShowTitleBar(Bool show);
		void SetAlwaysOnTop(Bool alwaysOnTop);

		void MakeActive();
		void MakeForeground();
		virtual void Close();
		virtual void SetText(Text::CStringNN text);
		virtual Math::Size2D<UOSInt> GetSizeP();
		virtual void SetExitOnClose(Bool exitOnClose);
		virtual void SetNoResize(Bool noResize);
		virtual NN<UI::GUITimer> AddTimer(UInt32 interval, UI::UIEvent handler, AnyType userObj);
		virtual void RemoveTimer(NN<UI::GUITimer> tmr);
		virtual void SetMenu(NN<UI::GUIMainMenu> menu);
		virtual Optional<UI::GUIMainMenu> GetMenu();
		virtual void UpdateMenu();
		virtual void SetDefaultButton(NN<UI::GUIButton> btn);
		virtual void SetCancelButton(NN<UI::GUIButton> btn);
		Optional<UI::GUIButton> GetDefaultButton();
		Optional<UI::GUIButton> GetCancelButton();

		virtual Math::Size2DDbl GetClientSize();
		virtual Bool IsChildVisible();

		virtual Text::CStringNN GetObjectClass() const;
		virtual OSInt OnNotify(UInt32 code, void *lParam);
		virtual void OnSizeChanged(Bool updateScn);
		virtual Bool OnPaint();
		void OnDropFiles(void *hDrop);
		virtual void EventMenuClicked(UInt16 cmdId);
		void SetSmallIcon(UI::GUIIcon *icon);
		void SetLargeIcon(UI::GUIIcon *icon);
		void ShowMouseCursor(Bool toShow);

		virtual void HandleFormClosed(FormClosedEvent handler, AnyType userObj);
		virtual void HandleDropFiles(FileEvent handler, AnyType userObj);
		virtual void HandleMenuClicked(MenuEvent handler, AnyType userObj);
		virtual void HandleKeyDown(KeyEvent handler, AnyType userObj);
		virtual void HandleKeyUp(KeyEvent handler, AnyType userObj);
		void SetClosingHandler(FormClosingEvent handler, AnyType userObj);
		virtual void SetDPI(Double hdpi, Double ddpi);

		void EventClosed();
		void EventTimer(UOSInt tmrId);
		virtual void OnFocus();
		virtual void OnFocusLost();
		virtual void OnDisplaySizeChange(UOSInt dispWidth, UOSInt dispHeight);
		void OnFileDrop(Data::DataArray<NN<Text::String>> files);

		void ToFullScn();
		void FromFullScn();
		NN<UI::GUICore> GetUI();
	};
}
#endif
