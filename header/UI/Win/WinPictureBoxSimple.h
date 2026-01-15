#ifndef _SM_UI_WIN_WINPICTUREBOXSIMPLE
#define _SM_UI_WIN_WINPICTUREBOXSIMPLE
#include "UI/GUIPictureBoxSimple.h"

namespace UI
{
	namespace Win
	{
		class WinPictureBoxSimple : public GUIPictureBoxSimple
		{
		private:
			struct ClassData;
			static Int32 useCnt;
			ClassData *clsData;
			Optional<Media::StaticImage> currImage;
			Optional<Media::DrawImage> prevImageD;

			static IntOS __stdcall PBWndProc(void *hWnd, UInt32 msg, UIntOS wParam, IntOS lParam);
			void OnPaint();
			void Init(Optional<InstanceHandle> hInst);
			void Deinit(Optional<InstanceHandle> hInst);
			void UpdatePreview();
		public:
			WinPictureBoxSimple(NN<GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder);
			virtual ~WinPictureBoxSimple();

			virtual IntOS OnNotify(UInt32 code, void *lParam);
			virtual void SetImage(Optional<Media::StaticImage> currImage);
			virtual void SetImageDImg(Optional<Media::DrawImage> img);
		};
	}
}
#endif
