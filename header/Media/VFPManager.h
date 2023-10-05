#ifndef _SM_MEDIA_VFPMANAGER
#define _SM_MEDIA_VFPMANAGER
#include "Data/ArrayList.h"
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
		Media::VFPManager *vfpmgr;
		Media::VFPluginFile *plugin;
		UInt32 file;
		const WChar *fileName;
		UInt32 useCnt;
		Sync::Mutex mut;
	};

	class VFPManager
	{
	private:
		Data::ArrayList<VFPluginFile*> *plugins;
		Int32 useCnt;
	public:
		VFPManager();
		UOSInt LoadFile(const UTF8Char *fileName, Data::ArrayList<Media::IMediaSource*> *outArr);
		void Release();
		void PrepareSelector(NotNullPtr<IO::FileSelector> selector);

	private:
		~VFPManager();
		void LoadPlugin(const WChar *fileName);
	};

};
#endif
