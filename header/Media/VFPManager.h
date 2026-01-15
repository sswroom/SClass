#ifndef _SM_MEDIA_VFPMANAGER
#define _SM_MEDIA_VFPMANAGER
#include "Data/ArrayListNN.hpp"
#include "IO/FileSelector.h"
#include "Media/MediaSource.h"
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
		UnsafeArrayOpt<const WChar> searchPattern;
	} VFPluginFile;

	struct VFMediaFile
	{
		NN<Media::VFPManager> vfpmgr;
		NN<Media::VFPluginFile> plugin;
		UInt32 file;
		UnsafeArray<const WChar> fileName;
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
		UIntOS LoadFile(UnsafeArray<const UTF8Char> fileName, NN<Data::ArrayListNN<Media::MediaSource>> outArr);
		void Release();
		void PrepareSelector(NN<IO::FileSelector> selector);

	private:
		~VFPManager();
		void LoadPlugin(const WChar *fileName);
	};

};
#endif
