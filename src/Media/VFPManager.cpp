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
	WChar wbuff[256];
	WChar wbuff2[256];
	WChar *wptr;
	NEW_CLASS(this->plugins, Data::ArrayList<VFPluginFile*>());
	IO::Registry *reg = IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, L"VFPlugin");
	while (reg->GetName(wbuff, i))
	{
		if (reg->GetValueStr(wbuff, wbuff2))
		{
			j = Text::StrLastIndexOfChar(wbuff2, '\\');
			wptr = &wbuff2[j + 1];
			if (Text::StrCompareICase(wptr, L"DVD2AVI.vfp") == 0)
			{
				LoadPlugin(wbuff2);
			}
			else if (Text::StrCompareICase(wptr, L"aviutl.vfp") == 0)
			{
				LoadPlugin(wbuff2);
			}
			else if (Text::StrCompareICase(wptr, L"TMPGEnc.vfp") == 0)
			{
//				LoadPlugin(wbuff2);
			}
			else if (Text::StrCompareICase(wptr, L"DGVfapi.vfp") == 0)
			{
				LoadPlugin(wbuff2);
			}
			else
			{
				LoadPlugin(wbuff2);
			}
		}
		i++;
	}
	IO::Registry::CloseRegistry(reg);

	IO::Path::GetProcessFileNameW(wbuff);
	wptr = IO::Path::GetFileDirectoryW(wbuff2, wbuff);
	*wptr++ = '\\';
	Text::StrConcat(wptr, L"*.vfp");
	IO::Path::FindFileSession *sess = IO::Path::FindFileW(wbuff2);
	if (sess)
	{
		if (IO::Path::FindNextFileW(wptr, sess, 0, 0, 0))
		{
			LoadPlugin(wbuff2);
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
	WChar wbuff[512];
	UOSInt charCnt = enc.UTF8CountBytes(fileName);
	cFile = MemAlloc(Char, charCnt + 1);
	enc.UTF8ToBytes((UInt8*)cFile, fileName);
	UOSInt i = this->plugins->GetCount();
	while (i-- > 0)
	{
		plugin = this->plugins->GetItem(i);
		if (IO::Path::FilePathMatchW(wbuff, plugin->searchPattern))
		{
			funcs = (VF_PluginFunc*)plugin->funcs;
			if (funcs->OpenFile(cFile, &fhand) == VF_OK)
			{
				finfo.dwSize = sizeof(finfo);
				if (funcs->GetFileInfo(fhand, &finfo) == VF_OK)
				{
					UOSInt outCnt = 0;
					NN<VFMediaFile> mfile;
					NEW_CLASSNN(mfile, VFMediaFile());
					mfile->vfpmgr = this;
					mfile->plugin = plugin;
					mfile->file = fhand;
					mfile->fileName = Text::StrCopyNew(wbuff);
					mfile->useCnt = 0;
					Sync::Interlocked::IncrementI32(this->useCnt);

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
						funcs->CloseFile(fhand);
						Text::StrDelNew(mfile->fileName);
						mfile.Delete();
						Sync::Interlocked::DecrementI32(this->useCnt);
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
	if (Sync::Interlocked::DecrementI32(this->useCnt) == 0)
	{
		DEL_CLASS(this);
	}
}

void Media::VFPManager::PrepareSelector(NN<IO::FileSelector> selector)
{
	VFPluginFile *plugin;
	VF_GetPluginInfo GetInfo;
	VF_PluginInfo info;
	Text::Encoding enc(932);
	WChar *sarr[3];
	WChar *wptr;
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
			wptr = MemAlloc(WChar, j + 1);
			enc.WFromBytes(wptr, (UInt8*)info.cFileType, k, 0);
			sarr[2] = wptr;
			while (true)
			{
				j = Text::StrSplit(sarr, 3, sarr[2], '|');
				if (j == 1)
					break;
				NN<Text::String> wptr1 = Text::String::NewNotNull(sarr[1]);
				NN<Text::String> wptr0 = Text::String::NewNotNull(sarr[0]);
				selector->AddFilter(wptr1->ToCString(), wptr0->ToCString());
				wptr1->Release();
				wptr0->Release();
				if (j == 2)
					break;
			}
			MemFree(wptr);
		}
	}
}

void Media::VFPManager::LoadPlugin(const WChar *fileName)
{
	HMODULE hMod = LoadLibraryW(fileName);
	if (hMod == 0)
		return;
	WChar wbuff[256];
	WChar *wptrs[2];
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
	enc.WFromBytes(wbuff, (UInt8*)info.cFileType, Text::StrCharCnt(info.cFileType), 0);
	
	VFPluginFile *plugin = MemAlloc(VFPluginFile, 1);
	plugin->hMod = hMod;
	plugin->getFunc = (void*)GetFunc;
	plugin->getInfo = (void*)GetInfo;
	plugin->funcs = funcPtr;
	if (Text::StrSplit(wptrs, 2, wbuff, '|') == 2)
	{
		plugin->searchPattern = Text::StrCopyNew(wptrs[1]);
	}
	else
	{
		plugin->searchPattern = 0;
	}
	this->plugins->Add(plugin);
}
