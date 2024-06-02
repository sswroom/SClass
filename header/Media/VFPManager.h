#ifndef _SM_MEDIA_VFPMANAGER
#define _SM_MEDIA_VFPMANAGER
#include "Data/ArrayListNN.h"
#include "IO/FileSelector.h"
#include "Media/IMediaSource.h"
#include "Sync/Mutex.h"

namespace Media
{
	class VFPManager;

	typedef struct
	{
		void *hMod;
		void *funcs;
		void *getInfo;
		void *getFunc;
		const WChar *searchPattern;
	} VFPluginFile;

	struct VFMediaFile
	{
		NN<Media::VFPManager> vfpmgr;
		NN<Media::VFPluginFile> plugin;
		UInt32 file;
		const WChar *fileName;
		UInt32 useCnt;
		Sync::Mutex mut;
	};

	class VFPManager
	{
	private:
		Data::ArrayListNN<VFPluginFile> plugins;
		Int32 useCnt;
	public:
		VFPManager();
		UOSInt LoadFile(UnsafeArray<const UTF8Char> fileName, NN<Data::ArrayListNN<Media::IMediaSource>> outArr);
		void Release();
		void PrepareSelector(NN<IO::FileSelector> selector);

	private:
		~VFPManager();
		void LoadPlugin(const WChar *fileName);
	};

};
#endif
