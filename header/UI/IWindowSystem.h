#ifndef _SM_UI_IWINDOWSYSTEM
#define _SM_UI_IWINDOWSYSTEM
/*
namespace UI
{
	class IControl
	{
	public:
		virtual void Show() = 0;
		virtual void Close() = 0;
		virtual void SetText(const WChar *text) = 0;
		virtual WChar *GetText(WChar *buff) = 0;
		virtual void SetSize(Int32 width, Int32 height) = 0;
		virtual void GetSize(Int32 *width, Int32 *height) = 0;
		virtual void SetPosition(Int32 x, Int32 y) = 0;
		virtual void GetPosition(Int32 *x, Int32 *y) = 0;

		virtual void HandleSizeChanged(UIEvent handler) = 0;
	};

	class IForm : public virtual IControl
	{
	public:
		virtual void SetExitOnClose(Bool exitOnClose) = 0;
		virtual void HandleFormClosed(UIEvent handler) = 0;
	};

	class IWindowSystem
	{
	public:
		virtual void Run() = 0;
		virtual void Exit() = 0;
		virtual IForm *NewForm(UI::IForm *parent) = 0;
		virtual IControl *NewButton(UI::IControl *ctrl, const WChar *text, UIEvent clickHandler) = 0;
	};
};*/
#endif
