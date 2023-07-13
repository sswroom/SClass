#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/AVIUtl/AUIAudio.h"
#include "Media/AVIUtl/AUIPlugin.h"
#include "Media/AVIUtl/AUIVideo.h"
#include "Sync/Interlocked.h"
#include "Text/Encoding.h"
#include "Text/MyStringW.h"
#include <windows.h>
#include <mmreg.h>
#undef FindNextFile

typedef struct {
	int					flag;				//	フラグ
											//	INPUT_INFO_FLAG_VIDEO	: 画像データあり
											//	INPUT_INFO_FLAG_AUDIO	: 音声データあり
											//	INPUT_INFO_FLAG_VIDEO_RANDOM_ACCESS	: キーフレームを気にせずにfunc_read_video()を呼び出します
											//	※標準ではキーフレームからシーケンシャルにfunc_read_video()が呼ばれるように制御されます
	int					rate,scale;			//	フレームレート
	int					n;					//	フレーム数
	BITMAPINFOHEADER	*format;			//	画像フォーマットへのポインタ(次に関数が呼ばれるまで内容を有効にしておく)
	int					format_size;		//	画像フォーマットのサイズ
	int					audio_n;			//	音声サンプル数
	WAVEFORMATEX		*audio_format;		//	音声フォーマットへのポインタ(次に関数が呼ばれるまで内容を有効にしておく)
	int					audio_format_size;	//	音声フォーマットのサイズ
	DWORD				handler;			//	画像codecハンドラ
	int					reserve[7];
} INPUT_INFO;
#define	INPUT_INFO_FLAG_VIDEO				1
#define	INPUT_INFO_FLAG_AUDIO				2
#define	INPUT_INFO_FLAG_VIDEO_RANDOM_ACCESS	8
//	※画像フォーマットにはRGB,YUY2とインストールされているcodecのものが使えます。
//	また、'Y''C''4''8'(biBitCountは48)でPIXEL_YC形式フォーマットで扱えます。(YUY2フィルタモードでは使用出来ません)
//	音声フォーマットにはPCMとインストールされているcodecのものが使えます。

//	入力ファイルハンドル
typedef void*	INPUT_HANDLE;

//	入力プラグイン構造体
typedef struct {
	int		flag;				//	フラグ
								//	INPUT_PLUGIN_FLAG_VIDEO	: 画像をサポートする
								//	INPUT_PLUGIN_FLAG_AUDIO	: 音声をサポートする
	LPSTR	name;				//	プラグインの名前
	LPSTR	filefilter;			//	入力ファイルフィルタ
	LPSTR	information;		//	プラグインの情報
	BOOL 	(*func_init)( void );
								//	DLL開始時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
	BOOL 	(*func_exit)( void );
								//	DLL終了時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
	INPUT_HANDLE (*func_open)( LPSTR file );
								//	入力ファイルをオープンする関数へのポインタ
								//	file	: ファイル名
								//	戻り値	: TRUEなら入力ファイルハンドル
	BOOL 	(*func_close)( INPUT_HANDLE ih );
								//	入力ファイルをクローズする関数へのポインタ
								//	ih		: 入力ファイルハンドル
								//	戻り値	: TRUEなら成功
	BOOL 	(*func_info_get)( INPUT_HANDLE ih,INPUT_INFO *iip );
								//	入力ファイルの情報を取得する関数へのポインタ
								//	ih		: 入力ファイルハンドル
								//	iip		: 入力ファイル情報構造体へのポインタ
								//	戻り値	: TRUEなら成功
	int 	(*func_read_video)( INPUT_HANDLE ih,int frame,void *buf );
								//	画像データを読み込む関数へのポインタ
								//	ih		: 入力ファイルハンドル
								//	frame	: 読み込むフレーム番号
								//	buf		: データを読み込むバッファへのポインタ
								//	戻り値	: 読み込んだデータサイズ
	int 	(*func_read_audio)( INPUT_HANDLE ih,int start,int length,void *buf );
								//	音声データを読み込む関数へのポインタ
								//	ih		: 入力ファイルハンドル
								//	start	: 読み込み開始サンプル番号
								//	length	: 読み込むサンプル数
								//	buf		: データを読み込むバッファへのポインタ
								//	戻り値	: 読み込んだサンプル数
	BOOL 	(*func_is_keyframe)( INPUT_HANDLE ih,int frame );
								//	キーフレームか調べる関数へのポインタ (NULLなら全てキーフレーム)
								//	ih		: 入力ファイルハンドル
								//	frame	: フレーム番号
								//	戻り値	: キーフレームなら成功
	BOOL	(*func_config)( HWND hwnd,HINSTANCE dll_hinst );
								//	入力設定のダイアログを要求された時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
								//	hwnd		: ウィンドウハンドル
								//	dll_hinst	: インスタンスハンドル
								//	戻り値		: TRUEなら成功
	int		reserve[16];
} INPUT_PLUGIN_TABLE;
#define	INPUT_PLUGIN_FLAG_VIDEO		1
#define	INPUT_PLUGIN_FLAG_AUDIO		2

typedef INPUT_PLUGIN_TABLE *(__stdcall *AUI_GetInputPluginTable)();

Media::AVIUtl::AUIPlugin::AUIPlugin()
{
	this->plugin = MemAlloc(PluginDetail, 1);
	this->plugin->useCnt = 1;
}

Media::AVIUtl::AUIPlugin::AUIPlugin(const Media::AVIUtl::AUIPlugin *plugin)
{
	this->plugin = plugin->plugin;
	Sync::Interlocked::Increment(&this->plugin->useCnt);
}

Media::AVIUtl::AUIPlugin::~AUIPlugin()
{
	if (Sync::Interlocked::Decrement(&this->plugin->useCnt) == 0)
	{
		FreeLibrary((HMODULE)this->plugin->hMod);
		MemFree(this->plugin);
	}
}

Media::AVIUtl::AUIPlugin *Media::AVIUtl::AUIPlugin::Clone() const
{
	Media::AVIUtl::AUIPlugin *plugin;
	NEW_CLASS(plugin, Media::AVIUtl::AUIPlugin(this));
	return plugin;
}

UOSInt Media::AVIUtl::AUIPlugin::LoadFile(const Char *fileName, Data::ArrayList<Media::IMediaSource*> *outArr)
{
	INPUT_HANDLE hand;
	INPUT_PLUGIN_TABLE *pluginTab = (INPUT_PLUGIN_TABLE*)this->plugin->pluginTable;
	hand = pluginTab->func_open((Char*)fileName);
	if (hand == 0)
		return 0;
	
	AUIInput *input;
	Media::FrameInfo *frameInfo;
	Media::AudioFormat *audioFormat;
	input = MemAlloc(AUIInput, 1);
	input->hand = hand;
	input->useCnt = 0;
	NEW_CLASS(frameInfo, Media::FrameInfo());
	NEW_CLASS(audioFormat, Media::AudioFormat());

	Media::IMediaSource *media;

	UInt32 frameRateNorm;
	UInt32 frameRateDenorm;
	UInt32 frameCnt;
	if (this->GetInputVideoInfo(input->hand, frameInfo, &frameRateNorm, &frameRateDenorm, &frameCnt))
	{
		NEW_CLASS(media, Media::AVIUtl::AUIVideo(this->Clone(), input, frameInfo, frameRateNorm, frameRateDenorm, frameCnt));
		outArr->Add(media);
		input->useCnt++;
	}
	else
	{
		DEL_CLASS(frameInfo);
	}
	UInt32 nSamples;
	if (this->GetInputAudioInfo(input->hand, audioFormat, &nSamples))
	{
		NEW_CLASS(media, Media::AVIUtl::AUIAudio(this->Clone(), input, audioFormat, nSamples));
		outArr->Add(media);
		input->useCnt++;
	}
	else
	{
		DEL_CLASS(audioFormat);
	}

	if (input->useCnt == 0)
	{
		this->CloseInput(input->hand);
		MemFree(input);
		return 0;
	}
	else
	{
		return input->useCnt;
	}
}

Bool Media::AVIUtl::AUIPlugin::CloseInput(void *hand)
{
	INPUT_PLUGIN_TABLE *pluginTab = (INPUT_PLUGIN_TABLE*)this->plugin->pluginTable;
	return pluginTab->func_close((INPUT_HANDLE)hand) == TRUE;
}

Bool Media::AVIUtl::AUIPlugin::GetInputVideoInfo(void *hand, Media::FrameInfo *frameInfo, UInt32 *frameRateNorm, UInt32 *frameRateDenorm, UInt32 *frameCnt)
{
	INPUT_PLUGIN_TABLE *pluginTab = (INPUT_PLUGIN_TABLE*)this->plugin->pluginTable;
	INPUT_INFO info;
	if ((pluginTab->flag & INPUT_PLUGIN_FLAG_VIDEO) == 0)
		return false;
	if (pluginTab->func_info_get((INPUT_HANDLE)hand, &info) != TRUE)
		return false;
	if ((info.flag & INPUT_INFO_FLAG_VIDEO) == 0)
		return false;
	
	frameInfo->storeSize.x = (ULONG)info.format->biWidth;
	frameInfo->storeSize.y = (ULONG)info.format->biHeight;
	frameInfo->dispSize = frameInfo->storeSize;
	frameInfo->fourcc = info.format->biCompression;
	frameInfo->storeBPP = info.format->biBitCount;
	frameInfo->pf = Media::PixelFormatGetDef(info.format->biCompression, info.format->biBitCount);
	frameInfo->byteSize = info.format->biSizeImage;
	frameInfo->par2 = 1;
	frameInfo->hdpi = 96;
	frameInfo->ftype = Media::FT_NON_INTERLACE;
	frameInfo->atype = Media::AT_NO_ALPHA;
	frameInfo->color->SetCommonProfile(Media::ColorProfile::CPT_VUNKNOWN);
	frameInfo->yuvType = Media::ColorProfile::YUVT_BT601;
	frameInfo->ycOfst = Media::YCOFST_C_CENTER_LEFT;

	if (frameInfo->dispSize.x == 352 && frameInfo->dispSize.y == 240)
	{
		frameInfo->par2 = 1.1;
		frameInfo->color->GetRTranParam()->Set(Media::CS::TRANT_GAMMA, 2.2);
		frameInfo->color->GetGTranParam()->Set(Media::CS::TRANT_GAMMA, 2.2);
		frameInfo->color->GetRTranParam()->Set(Media::CS::TRANT_GAMMA, 2.2);
	}
	else if (frameInfo->dispSize.x == 352 && frameInfo->dispSize.y == 288)
	{
		frameInfo->par2 = 0.91666666666666666666666666666667;
		frameInfo->color->GetRTranParam()->Set(Media::CS::TRANT_GAMMA, 2.2);
		frameInfo->color->GetGTranParam()->Set(Media::CS::TRANT_GAMMA, 2.2);
		frameInfo->color->GetRTranParam()->Set(Media::CS::TRANT_GAMMA, 2.2);
	}
	else if (frameInfo->dispSize.x == 720 && frameInfo->dispSize.y == 480)
	{
		frameInfo->par2 = 1.1;
		frameInfo->color->GetRTranParam()->Set(Media::CS::TRANT_GAMMA, 2.2);
		frameInfo->color->GetGTranParam()->Set(Media::CS::TRANT_GAMMA, 2.2);
		frameInfo->color->GetRTranParam()->Set(Media::CS::TRANT_GAMMA, 2.2);
		frameInfo->yuvType = Media::ColorProfile::YUVT_BT601;
	}
	else if (frameInfo->dispSize.x == 720 && frameInfo->dispSize.y == 576)
	{
		frameInfo->par2 = 0.91666666666666666666666666666667;
		frameInfo->color->GetRTranParam()->Set(Media::CS::TRANT_GAMMA, 2.2);
		frameInfo->color->GetGTranParam()->Set(Media::CS::TRANT_GAMMA, 2.2);
		frameInfo->color->GetRTranParam()->Set(Media::CS::TRANT_GAMMA, 2.2);
		frameInfo->yuvType = Media::ColorProfile::YUVT_BT601;
	}
	*frameRateNorm = (UInt32)info.rate;
	*frameRateDenorm = (UInt32)info.scale;
	*frameCnt = (UInt32)info.n;
	return true;
}

Bool Media::AVIUtl::AUIPlugin::GetInputAudioInfo(void *hand, Media::AudioFormat *af, UInt32 *nSamples)
{
	INPUT_PLUGIN_TABLE *pluginTab = (INPUT_PLUGIN_TABLE*)this->plugin->pluginTable;
	INPUT_INFO info;
	if ((pluginTab->flag & INPUT_PLUGIN_FLAG_AUDIO) == 0)
		return false;
	if (pluginTab->func_info_get((INPUT_HANDLE)hand, &info) != TRUE)
		return false;
	if ((info.flag & INPUT_INFO_FLAG_AUDIO) == 0)
		return false;
	af->FromWAVEFORMATEX((UInt8*)info.audio_format);
	*nSamples = (UInt32)info.audio_n;
	return true;
}

UOSInt Media::AVIUtl::AUIPlugin::GetVideoFrame(void *hand, UOSInt frameNum, UInt8 *buff)
{
	INPUT_PLUGIN_TABLE *pluginTab = (INPUT_PLUGIN_TABLE*)this->plugin->pluginTable;
	return (UInt32)pluginTab->func_read_video((INPUT_HANDLE)hand, (int)frameNum, buff);
}

UOSInt Media::AVIUtl::AUIPlugin::GetAudioData(void *hand, UOSInt startSample, UOSInt sampleLength, UInt8 *buff)
{
	INPUT_PLUGIN_TABLE *pluginTab = (INPUT_PLUGIN_TABLE*)this->plugin->pluginTable;
	return (UInt32)pluginTab->func_read_audio((INPUT_HANDLE)hand, (int)startSample, (int)sampleLength, buff);
}

Bool Media::AVIUtl::AUIPlugin::IsVideoKeyFrame(void *hand, UInt32 frameNum)
{
	INPUT_PLUGIN_TABLE *pluginTab = (INPUT_PLUGIN_TABLE*)this->plugin->pluginTable;
	if (pluginTab->func_is_keyframe)
	{
		return pluginTab->func_is_keyframe((INPUT_HANDLE)hand, (int)frameNum) == TRUE;
	}
	return true;
}

Bool Media::AVIUtl::AUIPlugin::ConfigInput(void *hInst, void *hWnd)
{
	INPUT_PLUGIN_TABLE *pluginTab = (INPUT_PLUGIN_TABLE*)this->plugin->pluginTable;
	if (pluginTab->func_config == 0)
		return false;
	return pluginTab->func_config((HWND)hWnd, (HINSTANCE)hInst) == TRUE;
}

void Media::AVIUtl::AUIPlugin::PrepareSelector(IO::FileSelector *selector)
{
	INPUT_PLUGIN_TABLE *pluginTab = (INPUT_PLUGIN_TABLE*)this->plugin->pluginTable;
	Text::Encoding enc(932);
	WChar *wptr;
	WChar *sarr[3];
	Char *filter = pluginTab->filefilter;
	UOSInt i;
	UOSInt j;
	i = Text::StrCharCnt(filter);
	j = enc.CountWChars((UInt8*)filter, i);
	wptr = MemAlloc(WChar, j + 1);
	enc.WFromBytes(wptr, (UInt8*)filter, i, 0);
	sarr[2] = wptr;
	while (true)
	{
		j = Text::StrSplit(sarr, 3, sarr[2], '|');
		if (j == 1)
			break;
		NotNullPtr<Text::String> sptr1 = Text::String::NewNotNull(sarr[1]);
		NotNullPtr<Text::String> sptr0 = Text::String::NewNotNull(sarr[0]);
		selector->AddFilter(sptr1->ToCString(), sptr0->ToCString());
		sptr1->Release();
		sptr0->Release();
		if (j == 2)
			break;
	}
	MemFree(wptr);
}

Media::AVIUtl::AUIPlugin *Media::AVIUtl::AUIPlugin::LoadPlugin(const WChar *fileName)
{
	HMODULE hMod = LoadLibraryW(fileName);
	if (hMod == 0)
		return 0;

	AUI_GetInputPluginTable GetInputPluginTable;
#ifdef _WIN32_WCE
	GetInputPluginTable = (AUI_GetInputPluginTable)GetProcAddressW(hMod, L"GetInputPluginTable");
#else
	GetInputPluginTable = (AUI_GetInputPluginTable)GetProcAddress(hMod, "GetInputPluginTable");
#endif
	if (GetInputPluginTable == 0)
	{
		FreeLibrary(hMod);
		return 0;
	}

	INPUT_PLUGIN_TABLE *pluginTable = GetInputPluginTable();
	if (pluginTable == 0)
	{
		FreeLibrary(hMod);
		return 0;
	}

	Media::AVIUtl::AUIPlugin *plugin;
	NEW_CLASS(plugin, Media::AVIUtl::AUIPlugin());
	plugin->plugin->hMod = hMod;
	plugin->plugin->pluginTable = pluginTable;
	return plugin;
}
