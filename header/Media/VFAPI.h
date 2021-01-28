#ifndef _SM_MEDIA_VFAPI
#define _SM_MEDIA_VFAPI

#define VF_STREAM_VIDEO  0x00000001
#define VF_STREAM_AUDIO  0x00000002
#define VF_OK            0x00000000
#define VF_ERROR         0x80004005

typedef struct
{
	DWORD  dwSize;
	DWORD  dwAPIVersion;
	DWORD  dwVersion;
	DWORD  dwSupportStreamType;
	char   cPluginInfo[256];
	char   cFileType[256];
} VF_PluginInfo, *LPVF_PluginInfo;

typedef DWORD VF_FileHandle, *LPVF_FileHandle;

typedef struct
{
	DWORD  dwSize;
	DWORD  dwHasStreams;
} VF_FileInfo, *LPVF_FileInfo;

typedef struct
{
	DWORD  dwSize;
	DWORD  dwLengthL;
	DWORD  dwLengthH;
	DWORD  dwRate;
	DWORD  dwScale;
	DWORD  dwWidth;
	DWORD  dwHeight;
	DWORD  dwBitCount;
} VF_StreamInfo_Video, *LPVF_StreamInfo_Video;

typedef struct
{
	DWORD  dwSize;
	DWORD  dwLengthL;
	DWORD  dwLengthH;
	DWORD  dwRate;
	DWORD  dwScale;
	DWORD  dwChannels;
	DWORD  dwBitsPerSample;
	DWORD  dwBlockAlign;
} VF_StreamInfo_Audio, *LPVF_StreamInfo_Audio;

typedef struct
{
	DWORD  dwSize;
	DWORD  dwFrameNumberL;
	DWORD  dwFrameNumberH;
	void  *lpData;
	int    lPitch;
} VF_ReadData_Video, *LPVF_ReadData_Video;

typedef struct
{
	DWORD  dwSize;
	DWORD  dwSamplePosL;
	DWORD  dwSamplePosH;
	DWORD  dwSampleCount;
	DWORD  dwReadedSampleCount;
	DWORD  dwBufSize;
	void  *lpBuf;
} VF_ReadData_Audio, *LPVF_ReadData_Audio;

typedef struct
{
	DWORD  dwSize;
	HRESULT (__stdcall *OpenFile)(char*, LPVF_FileHandle);
	HRESULT (__stdcall *CloseFile)(VF_FileHandle);
	HRESULT (__stdcall *GetFileInfo)(VF_FileHandle, LPVF_FileInfo);
	HRESULT (__stdcall *GetStreamInfo)(VF_FileHandle, DWORD, void*);
	HRESULT (__stdcall *ReadData)(VF_FileHandle, DWORD, void*); 
} VF_PluginFunc, *LPVF_PluginFunc;

typedef HRESULT (__stdcall *VF_GetPluginInfo)(LPVF_PluginInfo info);
typedef HRESULT (__stdcall *VF_GetPluginFunc)(LPVF_PluginFunc func);
#endif
