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
			Optional<Media::DrawImage> prevImageD;

			static IntOS __stdcall PBWndProc(void *hWnd, UInt32 msg, UInt32 wParam, IntOS lParam);
			void OnPaint();
			void Init(Optional<InstanceHandle> hInst);
			void Deinit(Optional<InstanceHandle> hInst);
			virtual void UpdatePreview();
		public:
			WinPictureBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder, Bool allowResize);
			virtual ~WinPictureBox();

			virtual IntOS OnNotify(UInt32 code, void *lParam);
		};
	}
}
#endif
