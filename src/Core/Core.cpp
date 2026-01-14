#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Data/ArrayListObj.hpp"

Data::ArrayListObj<Core::DecodeVideoFunc> *Core_decVFuncs = 0;
Data::ArrayListObj<Core::DecodeAudioFunc> *Core_decAFuncs = 0;
Data::ArrayListObj<Core::SimpleFunc> *Core_onExitFuncs = 0;

void Core::CoreStart()
{
	MemInit();
}

void Core::CoreEnd()
{
	if (Core_decVFuncs)
	{
		DEL_CLASS(Core_decVFuncs);
		Core_decVFuncs = 0;
	}
	if (Core_decAFuncs)
	{
		DEL_CLASS(Core_decAFuncs);
		Core_decAFuncs = 0;
	}
	if (Core_onExitFuncs)
	{
		UOSInt i = Core_onExitFuncs->GetCount();
		while (i-- > 0)
		{
			Core_onExitFuncs->GetItem(i)();
		}
		DEL_CLASS(Core_onExitFuncs);
		Core_onExitFuncs = 0;
	}
	MemDeinit();
}

void Core::CoreAddVideoDecFunc(DecodeVideoFunc func)
{
	if (Core_decVFuncs == 0)
	{
		NEW_CLASS(Core_decVFuncs, Data::ArrayListObj<DecodeVideoFunc>());
	}
	Core_decVFuncs->Add(func);
}

void Core::CoreAddAudioDecFunc(DecodeAudioFunc func)
{
	if (Core_decAFuncs == 0)
	{
		NEW_CLASS(Core_decAFuncs, Data::ArrayListObj<DecodeAudioFunc>());
	}
	Core_decAFuncs->Add(func);
}

void Core::CoreAddOnExitFunc(SimpleFunc func)
{
	if (Core_onExitFuncs == 0)
	{
		NEW_CLASS(Core_onExitFuncs, Data::ArrayListObj<SimpleFunc>());
	}
	Core_onExitFuncs->Add(func);
}

Optional<Media::VideoSource> Core::DecodeVideo(NN<Media::VideoSource> video)
{
	Optional<Media::VideoSource> decoder = nullptr;
	if (Core_decVFuncs == 0)
		return nullptr;
	UOSInt i = 0;
	UOSInt j = Core_decVFuncs->GetCount();
	while (decoder.IsNull() && i < j)
	{
		decoder = Core_decVFuncs->GetItem(i)(video);
		i++;
	}
	return decoder;
}

Optional<Media::AudioSource> Core::DecodeAudio(NN<Media::AudioSource> audio)
{
	Optional<Media::AudioSource> decoder = nullptr;
	if (Core_decAFuncs == 0)
		return nullptr;
	UOSInt i = 0;
	UOSInt j = Core_decAFuncs->GetCount();
	while (decoder.IsNull() && i < j)
	{
		decoder = Core_decAFuncs->GetItem(i)(audio);
		i++;
	}
	return decoder;
}
