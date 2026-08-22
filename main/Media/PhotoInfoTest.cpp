#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/StmData/FileData.h"
#include "Media/PhotoInfo.h"

#include <stdio.h>

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Text::CStringNN filePath = CSTR("/mnt/QDisk1/Data/UserFile/1/202607/1785386552000_BCFAAD18.jpg");
	IO::StmData::FileData fd(filePath, false);
	Media::PhotoInfo photoInfo(fd);
	Text::StringBuilderUTF8 sb;
	photoInfo.ToString(sb);
	printf("%s\r\n", sb.ToPtr());
	return 0;
}
