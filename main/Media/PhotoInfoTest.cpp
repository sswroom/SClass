#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/StmData/FileData.h"
#include "Media/PhotoInfo.h"

#include <stdio.h>

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Text::CString filePath = CSTR("/media/sswroom/Extreme SSD/PictureProc/IMG_0003.HEIC");
	IO::StmData::FileData fd(filePath, false);
	Media::PhotoInfo photoInfo(&fd);
	Text::StringBuilderUTF8 sb;
	photoInfo.ToString(&sb);
	printf("%s\r\n", sb.ToString());
	return 0;
}