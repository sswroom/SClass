#ifndef _SM_UI_WIN_WINPICTUREBOX
#define _SM_UI_WIN_WINPICTUREBOX
#include "UI/GUIPictureBox.h"

namespace UI
{
	namespace Win
	{
		class WinPictureBox : public GUIPictureBox
		{
		private:
			static Int32 useCnt;
			Media::DrawImage *prevImageD;

			static OSInt __stdcall PBWndProc(void *hWnd, UInt32 msg, UInt32 wParam, OSInt lParam);
			void OnPaint();
			void Init(void *hInst);
			void Deinit(void *hInst);
			virtual void UpdatePreview();
		public:
			WinPictureBox(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, NotNullPtr<Media::DrawEngine> eng, Bool hasBorder, Bool allowResize);
			virtual ~WinPictureBox();

			virtual OSInt OnNotify(UInt32 code, void *lParam);
		};
	}
}
#endif