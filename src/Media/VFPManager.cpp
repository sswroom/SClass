#include "Stdafx.h"
#include "IO/Registry.h"
#include "IO/Path.h"
#include "Media/VFAudioStream.h"
#include "Media/VFPManager.h"
#include "Media/VFVideoStream.h"
#include "Sync/Interlocked.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/Encoding.h"
#include <windows.h>
#include "Media/VFAPI.h"

#undef FindNextFile

Media::VFPManager::VFPManager()
{
	UOSInt j;
	this->useCnt = 1;
	UOSInt i = 0;
	WChar sbuff[256];
	WChar sbuff2[256];
	WChar *sptr;
	NEW_CLASS(this->plugins, Data::ArrayList<VFPluginFile*>());
	IO::Registry *reg = IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, L"VFPlugin");
	while (reg->GetName(sbuff, i))
	{
		if (reg->GetValueStr(sbuff, sbuff2))
		{
			j = Text::StrLastIndexOfChar(sbuff2, '\\');
			sptr = &sbuff2[j + 1];
			if (Text::StrCompareICase(sptr, L"DVD2AVI.vfp") == 0)
			{
				LoadPlugin(sbuff2);
			}
			else if (Text::StrCompareICase(sptr, L"aviutl.vfp") == 0)
			{
				LoadPlugin(sbuff2);
			}
			else if (Text::StrCompareICase(sptr, L"TMPGEnc.vfp") == 0)
			{
//				LoadPlugin(sbuff2);
			}
			else if (Text::StrCompareICase(sptr, L"DGVfapi.vfp") == 0)
			{
				LoadPlugin(sbuff2);
			}
			else
			{
				LoadPlugin(sbuff2);
			}
		}
		i++;
	}
	IO::Registry::CloseRegistry(reg);

	IO::Path::GetProcessFileNameW(sbuff);
	sptr = IO::Path::GetFileDirectoryW(sbuff2, sbuff);
	*sptr++ = '\\';
	Text::StrConcat(sptr, L"*.vfp");
	IO::Path::FindFileSession *sess = IO::Path::FindFileW(sbuff2);
	if (sess)
	{
		if (IO::Path::FindNextFileW(sptr, sess, 0, 0, 0))
		{
			LoadPlugin(sbuff2);
		}
		IO::Path::FindFileClose(sess);
	}

}

UOSInt Media::VFPManager::LoadFile(const UTF8Char *fileName, Data::ArrayList<Media::IMediaSource *> *outArr)
{
	Char *cFile;
	Text::Encoding enc;
	VFPluginFile *plugin;
	VF_PluginFunc *funcs;
	VF_FileHandle fhand;
	VF_FileInfo finfo;
	WChar sbuff[512];
	UOSInt charCnt = enc.UTF8CountBytes(fileName);
	cFile = MemAlloc(Char, charCnt + 1);
	enc.UTF8ToBytes((UInt8*)cFile, fileName);
	UOSInt i = this->plugins->GetCount();
	while (i-- > 0)
	{
		plugin = this->plugins->GetItem(i);
		if (IO::Path::FilePathMatchW(sbuff, plugin->searchPattern))
		{
			funcs = (VF_PluginFunc*)plugin->funcs;
			if (funcs->OpenFile(cFile, &fhand) == VF_OK)
			{
				finfo.dwSize = sizeof(finfo);
				if (funcs->GetFileInfo(fhand, &finfo) == VF_OK)
				{
					UOSInt outCnt = 0;
					VFMediaFile *mfile = MemAlloc(VFMediaFile, 1);
					mfile->vfpmgr = this;
					mfile->plugin = plugin;
					mfile->file = fhand;
					mfile->fileName = Text::StrCopyNew(sbuff);
					mfile->useCnt = 0;
					Sync::Interlocked::Increment(&this->useCnt);

					NEW_CLASS(mfile->mut, Sync::Mutex());
					if (finfo.dwHasStreams & VF_STREAM_VIDEO)
					{
						VFVideoStream *vfs;
						NEW_CLASS(vfs, VFVideoStream(mfile));
						outArr->Add(vfs);
						outCnt++;
					}
					if (finfo.dwHasStreams & VF_STREAM_AUDIO)
					{
						VFAudioStream *vfs;
						NEW_CLASS(vfs, VFAudioStream(mfile));
						outArr->Add(vfs);
						outCnt++;
					}
					if (mfile->useCnt == 0)
					{
						DEL_CLASS(mfile->mut);
						funcs->CloseFile(fhand);
						Text::StrDelNew(mfile->fileName);
						MemFree(mfile);
						Sync::Interlocked::Decrement(&this->useCnt);
						MemFree(cFile);
						return 0;
					}
					else
					{
						MemFree(cFile);
						return outCnt;
					}
				}
				funcs->CloseFile(fhand);
			}
		}
	}
	MemFree(cFile);
	return 0;
}

Media::VFPManager::~VFPManager()
{
	VFPluginFile *plugin;
	UOSInt i = this->plugins->GetCount();
	while (i-- > 0)
	{
		plugin = plugins->GetItem(i);
		MemFree(plugin->funcs);
		if (plugin->searchPattern)
		{
			Text::StrDelNew(plugin->searchPattern);
		}
		FreeLibrary((HMODULE)plugin->hMod);
		MemFree(plugin);
	}
	DEL_CLASS(plugins);
}

void Media::VFPManager::Release()
{
	if (Sync::Interlocked::Decrement(&this->useCnt) == 0)
	{
		DEL_CLASS(this);
	}
}

void Media::VFPManager::PrepareSelector(IO::IFileSelector *selector)
{
	VFPluginFile *plugin;
	VF_GetPluginInfo GetInfo;
	VF_PluginInfo info;
	Text::Encoding enc(932);
	WChar *sarr[3];
	WChar *sptr;
	UOSInt k;
	UOSInt j;
	UOSInt i = this->plugins->GetCount();
	while (i-- > 0)
	{
		plugin = plugins->GetItem(i);
		GetInfo = (VF_GetPluginInfo)plugin->getInfo;
		info.dwSize = sizeof(info);
		if (GetInfo(&info) == VF_OK)
		{
			k = Text::StrCharCnt(info.cFileType);
			j = enc.CountWChars((UInt8*)info.cFileType, k);
			sptr = MemAlloc(WChar, j + 1);
			enc.WFromBytes(sptr, (UInt8*)info.cFileType, k, 0);
			sarr[2] = sptr;
			while (true)
			{
				j = Text::StrSplit(sarr, 3, sarr[2], '|');
				if (j == 1)
					break;
				Text::String *u8ptr1 = Text::String::NewNotNull(sarr[1]);
				Text::String *u8ptr0 = Text::String::NewNotNull(sarr[0]);
				selector->AddFilter(u8ptr1->ToCString(), u8ptr0->ToCString());
				u8ptr1->Release();
				u8ptr0->Release();
				if (j == 2)
					break;
			}
			MemFree(sptr);
		}
	}
}

void Media::VFPManager::LoadPlugin(const WChar *fileName)
{
	HMODULE hMod = LoadLibraryW(fileName);
	if (hMod == 0)
		return;
	WChar sbuff[256];
	WChar *sptrs[2];
	VF_GetPluginInfo GetInfo;
	VF_GetPluginFunc GetFunc;
#ifdef _WIN32_WCE
	GetInfo = (VF_GetPluginInfo)GetProcAddressW(hMod, L"vfGetPluginInfo");
	GetFunc = (VF_GetPluginFunc)GetProcAddressW(hMod, L"vfGetPluginFunc");
#else
	GetInfo = (VF_GetPluginInfo)GetProcAddress(hMod, "vfGetPluginInfo");
	GetFunc = (VF_GetPluginFunc)GetProcAddress(hMod, "vfGetPluginFunc");
#endif
	if (GetInfo == 0 || GetFunc == 0)
	{
		FreeLibrary(hMod);
		return;
	}
	VF_PluginInfo info;
	VF_PluginFunc func;
	info.dwSize = sizeof(info);
	if (GetInfo(&info) != VF_OK)
	{
		FreeLibrary(hMod);
		return;
	}
	func.dwSize = sizeof(func);
	if (GetFunc(&func) != VF_OK)
	{
		FreeLibrary(hMod);
		return;
	}
	VF_PluginFunc *funcPtr = MemAlloc(VF_PluginFunc, 1);
	MemCopyNO(funcPtr, &func, sizeof(func));

	Text::Encoding enc;
	enc.WFromBytes(sbuff, (UInt8*)info.cFileType, Text::StrCharCnt(info.cFileType), 0);
	
	VFPluginFile *plugin = MemAlloc(VFPluginFile, 1);
	plugin->hMod = hMod;
	plugin->getFunc = (void*)GetFunc;
	plugin->getInfo = (void*)GetInfo;
	plugin->funcs = funcPtr;
	if (Text::StrSplit(sptrs, 2, sbuff, '|') == 2)
	{
		plugin->searchPattern = Text::StrCopyNew(sptrs[1]);
	}
	else
	{
		plugin->searchPattern = 0;
	}
	this->plugins->Add(plugin);
}
