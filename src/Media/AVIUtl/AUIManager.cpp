#include "Stdafx.h"
#include "IO/Path.h"
#include "Media/AVIUtl/AUIManager.h"
#include "Text/Encoding.h"

Media::AVIUtl::AUIManager::AUIManager()
{
	WChar wbuff[512];
	IO::Path::PathType pt;
	Media::AVIUtl::AUIPlugin *plugin;
	NEW_CLASS(this->plugins, Data::ArrayList<Media::AVIUtl::AUIPlugin *>());

	IO::Path::FindFileSession *sess = IO::Path::FindFileW(L"*.aui");
	if (sess)
	{
		while (IO::Path::FindNextFileW(wbuff, sess, 0, &pt, 0))
		{
			plugin = Media::AVIUtl::AUIPlugin::LoadPlugin(wbuff);
			if (plugin)
			{
				this->plugins->Add(plugin);
			}
		}
		IO::Path::FindFileClose(sess);
	}
}

Media::AVIUtl::AUIManager::~AUIManager()
{
	Media::AVIUtl::AUIPlugin *plugin;
	UOSInt i;
	i = this->plugins->GetCount();
	while (i-- > 0)
	{
		plugin = this->plugins->GetItem(i);
		DEL_CLASS(plugin);
	}
	DEL_CLASS(this->plugins);
}

UOSInt Media::AVIUtl::AUIManager::LoadFile(const UTF8Char *fileName, Data::ArrayList<Media::IMediaSource*> *outArr)
{
	Char sbuff[512];
	Text::Encoding enc;
	enc.UTF8ToBytes((UInt8*)sbuff, fileName);

	UOSInt retSize = 0;
	UOSInt i;
	i = this->plugins->GetCount();
	while (i-- > 0)
	{
		retSize = this->plugins->GetItem(i)->LoadFile(sbuff, outArr);
		if (retSize > 0)
			return retSize;
	}
	return 0;
}

void Media::AVIUtl::AUIManager::PrepareSelector(NotNullPtr<IO::FileSelector> selector)
{
	UOSInt i;
	i = this->plugins->GetCount();
	while (i-- > 0)
	{
		this->plugins->GetItem(i)->PrepareSelector(selector);
	}
}
