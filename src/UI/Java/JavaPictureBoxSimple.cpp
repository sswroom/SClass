#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/Interlocked.h"
#include "Media/GTKDrawEngine.h"
#include "Media/ImageUtil.h"
#include "UI/GUIClientControl.h"
#include "UI/Java/JavaPictureBoxSimple.h"

UI::Java::JavaPictureBoxSimple::JavaPictureBoxSimple(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, NotNullPtr<Media::DrawEngine> eng, Bool hasBorder) : UI::GUIPictureBoxSimple(ui, parent, eng, hasBorder)
{
}

UI::Java::JavaPictureBoxSimple::~JavaPictureBoxSimple()
{
}

OSInt UI::Java::JavaPictureBoxSimple::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::Java::JavaPictureBoxSimple::SetImage(Optional<Media::StaticImage> currImage)
{
}

void UI::Java::JavaPictureBoxSimple::SetImageDImg(Media::DrawImage *img)
{
}
