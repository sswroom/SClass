#include "Stdafx.h"
#include "IO/Path.h"
#include "Media/AVIUtl/AUIManager.h"
#include "Text/Encoding.h"

Media::AVIUtl::AUIManager::AUIManager()
{
	WChar wbuff[512];
	IO::Path::PathType pt;
	NN<Media::AVIUtl::AUIPlugin> plugin;

	NN<IO::Path::FindFileSession> sess;
	if (IO::Path::FindFileW(L"*.aui").SetTo(sess))
	{
		while (IO::Path::FindNextFileW(wbuff, sess, 0, pt, 0).NotNull())
		{
			if (Media::AVIUtl::AUIPlugin::LoadPlugin(wbuff).SetTo(plugin))
			{
				this->plugins.Add(plugin);
			}
		}
		IO::Path::FindFileClose(sess);
	}
}

Media::AVIUtl::AUIManager::~AUIManager()
{
	this->plugins.DeleteAll();
}

UOSInt Media::AVIUtl::AUIManager::LoadFile(UnsafeArray<const UTF8Char> fileName, NN<Data::ArrayListNN<Media::MediaSource>> outArr)
{
	Char sbuff[512];
	Text::Encoding enc;
	enc.UTF8ToBytes((UInt8*)sbuff, fileName);

	UOSInt retSize = 0;
	UOSInt i;
	i = this->plugins.GetCount();
	while (i-- > 0)
	{
		retSize = this->plugins.GetItemNoCheck(i)->LoadFile(sbuff, outArr);
		if (retSize > 0)
			return retSize;
	}
	return 0;
}

void Media::AVIUtl::AUIManager::PrepareSelector(NN<IO::FileSelector> selector)
{
	UOSInt i;
	i = this->plugins.GetCount();
	while (i-- > 0)
	{
		this->plugins.GetItemNoCheck(i)->PrepareSelector(selector);
	}
}
