#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/ASIOOutRenderer.h"
#include "Sync/Thread.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

#include <windows.h>
#define ASIO_PATH L"software\\asio"
#define MAXDRVNAMELEN 128
#define IEEE754_64FLOAT 1
#define NATIVE_INT64 1

#ifdef _WIN32_WCE
#define RegCreateKeyW(key, name, outKey) RegCreateKeyEx(key, name, 0, 0, REG_OPTION_NON_VOLATILE, 0, 0, outKey, 0)
#define RegOpenKeyW(key, name, outKey) RegOpenKeyEx(key, name, 0, 0, outKey)
#define RegEnumKeyW(hKey, index, keyname, nameLen) RegEnumKeyEx(hKey, index, keyname, &nameLen, 0, 0, 0, 0)
#define CoInitialize(x) CoInitializeEx(x, COINIT_MULTITHREADED);
#endif


typedef long ASIOBool;
typedef long ASIOError;
enum {
	ASE_OK = 0,             // This value will be returned whenever the call succeeded
	ASE_SUCCESS = 0x3f4847a0,	// unique success return value for ASIOFuture calls
	ASE_NotPresent = -1000, // hardware input or output is not present or available
	ASE_HWMalfunction,      // hardware is malfunctioning (can be returned by any ASIO function)
	ASE_InvalidParameter,   // input parameter invalid
	ASE_InvalidMode,        // hardware is in a bad mode or used in a bad mode
	ASE_SPNotAdvancing,     // hardware is not running when sample position is inquired
	ASE_NoClock,            // sample clock or rate cannot be determined or is not present
	ASE_NoMemory            // not enough memory for completing the request
};

#if NATIVE_INT64
	typedef long long int ASIOSamples;
#else
	typedef struct ASIOSamples {
		unsigned long hi;
		unsigned long lo;
	} ASIOSamples;
#endif

#if NATIVE_INT64
	typedef long long int ASIOTimeStamp ;
#else
	typedef struct ASIOTimeStamp {
		unsigned long hi;
		unsigned long lo;
	} ASIOTimeStamp;
#endif

#if IEEE754_64FLOAT
	typedef double ASIOSampleRate;
#else
	typedef struct ASIOSampleRate {
		char ieee[8];
	} ASIOSampleRate;
#endif


typedef long ASIOSampleType;
enum {
	ASIOSTInt16MSB   = 0,
	ASIOSTInt24MSB   = 1,		// used for 20 bits as well
	ASIOSTInt32MSB   = 2,
	ASIOSTFloat32MSB = 3,		// IEEE 754 32 bit float
	ASIOSTFloat64MSB = 4,		// IEEE 754 64 bit double float

	// these are used for 32 bit data buffer, with different alignment of the data inside
	// 32 bit PCI bus systems can be more easily used with these
	ASIOSTInt32MSB16 = 8,		// 32 bit data with 16 bit alignment
	ASIOSTInt32MSB18 = 9,		// 32 bit data with 18 bit alignment
	ASIOSTInt32MSB20 = 10,		// 32 bit data with 20 bit alignment
	ASIOSTInt32MSB24 = 11,		// 32 bit data with 24 bit alignment
	
	ASIOSTInt16LSB   = 16,
	ASIOSTInt24LSB   = 17,		// used for 20 bits as well
	ASIOSTInt32LSB   = 18,
	ASIOSTFloat32LSB = 19,		// IEEE 754 32 bit float, as found on Intel x86 architecture
	ASIOSTFloat64LSB = 20, 		// IEEE 754 64 bit double float, as found on Intel x86 architecture

	// these are used for 32 bit data buffer, with different alignment of the data inside
	// 32 bit PCI bus systems can more easily used with these
	ASIOSTInt32LSB16 = 24,		// 32 bit data with 18 bit alignment
	ASIOSTInt32LSB18 = 25,		// 32 bit data with 18 bit alignment
	ASIOSTInt32LSB20 = 26,		// 32 bit data with 20 bit alignment
	ASIOSTInt32LSB24 = 27,		// 32 bit data with 24 bit alignment

	//	ASIO DSD format.
	ASIOSTDSDInt8LSB1   = 32,		// DSD 1 bit data, 8 samples per byte. First sample in Least significant bit.
	ASIOSTDSDInt8MSB1   = 33,		// DSD 1 bit data, 8 samples per byte. First sample in Most significant bit.
	ASIOSTDSDInt8NER8	= 40,		// DSD 8 bit data, 1 sample per byte. No Endianness required.

	ASIOSTLastEntry
};

typedef struct ASIOTimeCode
{       
	double          speed;                  // speed relation (fraction of nominal speed)
	                                        // optional; set to 0. or 1. if not supported
	ASIOSamples     timeCodeSamples;        // time in samples
	unsigned long   flags;                  // some information flags (see below)
	char future[64];
} ASIOTimeCode;

typedef enum ASIOTimeCodeFlags
{
	kTcValid                = 1,
	kTcRunning              = 1 << 1,
	kTcReverse              = 1 << 2,
	kTcOnspeed              = 1 << 3,
	kTcStill                = 1 << 4,
	
	kTcSpeedValid           = 1 << 8
}  ASIOTimeCodeFlags;

typedef struct AsioTimeInfo

{
	double          speed;                  // absolute speed (1. = nominal)
	ASIOTimeStamp   systemTime;             // system time related to samplePosition, in nanoseconds
	                                        // on mac, must be derived from Microseconds() (not UpTime()!)
	                                        // on windows, must be derived from timeGetTime()
	ASIOSamples     samplePosition;
	ASIOSampleRate  sampleRate;             // current rate
	unsigned long flags;                    // (see below)
	char reserved[12];
} AsioTimeInfo;

typedef enum AsioTimeInfoFlags
{
	kSystemTimeValid        = 1,            // must always be valid
	kSamplePositionValid    = 1 << 1,       // must always be valid
	kSampleRateValid        = 1 << 2,
	kSpeedValid             = 1 << 3,
	
	kSampleRateChanged      = 1 << 4,
	kClockSourceChanged     = 1 << 5
} AsioTimeInfoFlags;

typedef struct ASIOTime                          // both input/output
{
	long reserved[4];                       // must be 0
	struct AsioTimeInfo     timeInfo;       // required
	struct ASIOTimeCode     timeCode;       // optional, evaluated if (timeCode.flags & kTcValid)
} ASIOTime;

typedef struct ASIOCallbacks
{
	void (*bufferSwitch) (long doubleBufferIndex, ASIOBool directProcess);
		// bufferSwitch indicates that both input and output are to be processed.
		// the current buffer half index (0 for A, 1 for B) determines
		// - the output buffer that the host should start to fill. the other buffer
		//   will be passed to output hardware regardless of whether it got filled
		//   in time or not.
		// - the input buffer that is now filled with incoming data. Note that
		//   because of the synchronicity of i/o, the input always has at
		//   least one buffer latency in relation to the output.
		// directProcess suggests to the host whether it should immedeately
		// start processing (directProcess == ASIOTrue), or whether its process
		// should be deferred because the call comes from a very low level
		// (for instance, a high level priority interrupt), and direct processing
		// would cause timing instabilities for the rest of the system. If in doubt,
		// directProcess should be set to ASIOFalse.
		// Note: bufferSwitch may be called at interrupt time for highest efficiency.

	void (*sampleRateDidChange) (ASIOSampleRate sRate);
		// gets called when the AudioStreamIO detects a sample rate change
		// If sample rate is unknown, 0 is passed (for instance, clock loss
		// when externally synchronized).

	long (*asioMessage) (long selector, long value, void* message, double* opt);
		// generic callback for various purposes, see selectors below.
		// note this is only present if the asio version is 2 or higher

	ASIOTime* (*bufferSwitchTimeInfo) (ASIOTime* params, long doubleBufferIndex, ASIOBool directProcess);
		// new callback with time info. makes ASIOGetSamplePosition() and various
		// calls to ASIOGetSampleRate obsolete,
		// and allows for timecode sync etc. to be preferred; will be used if
		// the driver calls asioMessage with selector kAsioSupportsTimeInfo.
} ASIOCallbacks;

// asioMessage selectors
enum
{
	kAsioSelectorSupported = 1,	// selector in <value>, returns 1L if supported,
								// 0 otherwise
    kAsioEngineVersion,			// returns engine (host) asio implementation version,
								// 2 or higher
	kAsioResetRequest,			// request driver reset. if accepted, this
								// will close the driver (ASIO_Exit() ) and
								// re-open it again (ASIO_Init() etc). some
								// drivers need to reconfigure for instance
								// when the sample rate changes, or some basic
								// changes have been made in ASIO_ControlPanel().
								// returns 1L; note the request is merely passed
								// to the application, there is no way to determine
								// if it gets accepted at this time (but it usually
								// will be).
	kAsioBufferSizeChange,		// not yet supported, will currently always return 0L.
								// for now, use kAsioResetRequest instead.
								// once implemented, the new buffer size is expected
								// in <value>, and on success returns 1L
	kAsioResyncRequest,			// the driver went out of sync, such that
								// the timestamp is no longer valid. this
								// is a request to re-start the engine and
								// slave devices (sequencer). returns 1 for ok,
								// 0 if not supported.
	kAsioLatenciesChanged, 		// the drivers latencies have changed. The engine
								// will refetch the latencies.
	kAsioSupportsTimeInfo,		// if host returns true here, it will expect the
								// callback bufferSwitchTimeInfo to be called instead
								// of bufferSwitch
	kAsioSupportsTimeCode,		// 
	kAsioMMCCommand,			// unused - value: number of commands, message points to mmc commands
	kAsioSupportsInputMonitor,	// kAsioSupportsXXX return 1 if host supports this
	kAsioSupportsInputGain,     // unused and undefined
	kAsioSupportsInputMeter,    // unused and undefined
	kAsioSupportsOutputGain,    // unused and undefined
	kAsioSupportsOutputMeter,   // unused and undefined
	kAsioOverload,              // driver detected an overload

	kAsioNumMessageSelectors
};

typedef struct ASIOClockSource
{
	long index;					// as used for ASIOSetClockSource()
	long associatedChannel;		// for instance, S/PDIF or AES/EBU
	long associatedGroup;		// see channel groups (ASIOGetChannelInfo())
	ASIOBool isCurrentSource;	// ASIOTrue if this is the current clock source
	char name[32];				// for user selection
} ASIOClockSource;

typedef struct ASIOBufferInfo
{
	ASIOBool isInput;			// on input:  ASIOTrue: input, else output
	long channelNum;			// on input:  channel index
	void *buffers[2];			// on output: double buffer addresses
} ASIOBufferInfo;

typedef struct ASIOChannelInfo
{
	long channel;			// on input, channel index
	ASIOBool isInput;		// on input
	ASIOBool isActive;		// on exit
	long channelGroup;		// dto
	ASIOSampleType type;	// dto
	char name[32];			// dto
} ASIOChannelInfo;

interface IASIO : public IUnknown
{

	virtual ASIOBool init(void *sysHandle) = 0;
	virtual void getDriverName(char *name) = 0;	
	virtual long getDriverVersion() = 0;
	virtual void getErrorMessage(char *string) = 0;	
	virtual ASIOError start() = 0;
	virtual ASIOError stop() = 0;
	virtual ASIOError getChannels(long *numInputChannels, long *numOutputChannels) = 0;
	virtual ASIOError getLatencies(long *inputLatency, long *outputLatency) = 0;
	virtual ASIOError getBufferSize(long *minSize, long *maxSize,
		long *preferredSize, long *granularity) = 0;
	virtual ASIOError canSampleRate(ASIOSampleRate sampleRate) = 0;
	virtual ASIOError getSampleRate(ASIOSampleRate *sampleRate) = 0;
	virtual ASIOError setSampleRate(ASIOSampleRate sampleRate) = 0;
	virtual ASIOError getClockSources(ASIOClockSource *clocks, long *numSources) = 0;
	virtual ASIOError setClockSource(long reference) = 0;
	virtual ASIOError getSamplePosition(ASIOSamples *sPos, ASIOTimeStamp *tStamp) = 0;
	virtual ASIOError getChannelInfo(ASIOChannelInfo *info) = 0;
	virtual ASIOError createBuffers(ASIOBufferInfo *bufferInfos, long numChannels,
		long bufferSize, ASIOCallbacks *callbacks) = 0;
	virtual ASIOError disposeBuffers() = 0;
	virtual ASIOError controlPanel() = 0;
	virtual ASIOError future(long selector,void *opt) = 0;
	virtual ASIOError outputReady() = 0;
};

Media::ASIOOutRenderer *asioCurrRenderer;
ASIOCallbacks asioCallbacks;

long ASIOHdlrMessage(long selector, long value, void* message, double* opt)
{
	Int32 i;
	switch (selector)
	{
	case kAsioEngineVersion:
		return 2;
	case kAsioSupportsTimeInfo:
		return 1;
	case kAsioResyncRequest:
		return 1;
	case kAsioLatenciesChanged:
		return 1;
	default:
		i = 0;
		i+=0;
		return 0;
	}

}

void ASIOHdlrBuffSwitch(long doubleBufferIndex, ASIOBool directProcess)
{
	asioCurrRenderer->SwitchBuffer(doubleBufferIndex);
}

ASIOTime* ASIOHdlrBuffSwitchTI(ASIOTime* params, long doubleBufferIndex, ASIOBool directProcess)
{
//	printf("%d %ld %ld\n", doubleBufferIndex, params->timeInfo.samplePosition, params->timeInfo.systemTime);

	Int64 tc = asioCurrRenderer->SwitchBuffer(doubleBufferIndex);
	params->timeCode.timeCodeSamples = tc;
	params->timeCode.speed = 0;
	params->timeCode.flags = kTcValid | kTcRunning | kTcOnspeed;
	return params;
}

void ASIOHdlrSRChg(ASIOSampleRate sRate)
{
	Int32 i = 0;
	i += 0;
}

Int32 Media::ASIOOutRenderer::GetDeviceIndex(const UTF8Char *buff)
{
	HKEY hkEnum;
	const WChar *wbuff = 0;
	if (buff)
	{
		wbuff = Text::StrToWCharNew(buff);
	}
	WChar keyname[MAXDRVNAMELEN];
	DWORD nameSize = MAXDRVNAMELEN;
	UInt32 index = 0;
	Int32 ret = -1;
	Int32 cr = RegOpenKeyW(HKEY_LOCAL_MACHINE, ASIO_PATH, &hkEnum);
	while (cr == ERROR_SUCCESS)
	{
		if ((cr = RegEnumKeyW(hkEnum, index, keyname, nameSize))== ERROR_SUCCESS)
		{
			if (buff == 0)
			{
				ret = (Int32)index;
				break;
			}
			else if (Text::StrEquals(keyname, wbuff) == 0)
			{
				ret = (Int32)index;
				break;
			}
			else if (Text::StrEndsWith(keyname, wbuff))
			{
				ret = (Int32)index;
				break;
			}
			index++;
		}
	}
	if (hkEnum)
		RegCloseKey(hkEnum);
	SDEL_TEXT(wbuff);
	return ret;
}

UInt32 __stdcall Media::ASIOOutRenderer::PlayThread(void *obj)
{
	Media::ASIOOutRenderer *me = (Media::ASIOOutRenderer*)obj;
	IASIO *asio = (IASIO*)me->asiodrv;
	AudioFormat fmt;
	UInt32 ch;
	UInt32 nSamples;
//	Int32 nextSample;
	UInt32 audStartTime;
	UInt8 *sampleBuff;
	UInt32 blkAlign;
	Sync::Event *evt;
	UInt32 i;
	UInt32 j;
	UInt32 k;

	Sync::Thread::SetPriority(Sync::Thread::TP_REALTIME);
	ASIOBufferInfo *buffInfos = (ASIOBufferInfo *)me->bufferInfos;
	me->threadInit = true;
	me->audSrc->GetFormat(&fmt);
	me->bufferIndex = 0;
	me->bufferOfst = 0;
	me->bufferFilled = true;
	me->bufferEvt->Set();

	if (me->sampleTypes[0] == ASIOSTInt16LSB)
	{
		nSamples = me->bufferSize >> 1;
	}
	else if (me->sampleTypes[0] == ASIOSTInt24LSB)
	{
		if (me->bufferSize % 3)
		{
			nSamples = me->bufferSize / 3 + 1;
		}
		else
		{
			nSamples = me->bufferSize / 3;
		}
	}
	else if (me->sampleTypes[0] == ASIOSTInt32LSB)
	{
		nSamples = me->bufferSize >> 2;
	}
	else
	{
		me->playing = false;
		if (me->endHdlr)
		{
			me->endHdlr(me->endHdlrObj);
		}
		return 0;
	}
	nSamples = me->bufferSize;
	NEW_CLASS(evt, Sync::Event());
	blkAlign = fmt.nChannels * (UInt32)fmt.bitpersample >> 3;
	sampleBuff = MemAlloc(UInt8, nSamples * blkAlign);
	audStartTime = me->audSrc->GetCurrTime();
	me->audSrc->Start(evt, nSamples * blkAlign);

	me->audSrc->ReadBlockLPCM(sampleBuff, nSamples * blkAlign, &fmt);
	asio->start();
	while (!me->toStop)
	{
		if (!me->bufferFilled)
		{
			me->clk->Start(audStartTime + (UInt32)(me->bufferOfst * 1000 / fmt.frequency));
			if (me->sampleTypes[0] == ASIOSTInt16LSB)
			{
				if (fmt.bitpersample == 16)
				{
					Int16 *srcPtr = (Int16*)sampleBuff;
					Int16 *destPtr;
					ch = fmt.nChannels;
					while (ch-- > 0)
					{
						destPtr = (Int16*)buffInfos[ch].buffers[me->bufferIndex];
						i = 0;
						j = ch;
						while (i < nSamples)
						{
							destPtr[i] = srcPtr[j];
							i++;
							j += fmt.nChannels;
						}
					}
				}
				else if (fmt.bitpersample == 24 || fmt.bitpersample == 32)
				{
					k = fmt.nChannels * (UInt32)fmt.bitpersample >> 3;
					UInt8 *srcPtr = (UInt8*)sampleBuff;
					Int16 *destPtr;
					ch = fmt.nChannels;
					while (ch-- > 0)
					{
						destPtr = (Int16*)buffInfos[ch].buffers[me->bufferIndex];
						i = 0;
						j = ((ch + 1) * fmt.bitpersample >> 3) - 2;
						while (i < nSamples)
						{
							destPtr[i] = *(Int16*)&srcPtr[j];
							i += 1;
							j += k;
						}
					}
				}
				me->bufferFilled = true;
				asio->outputReady();
			}
			else if (me->sampleTypes[0] == ASIOSTInt24LSB)
			{
				if (fmt.bitpersample == 16)
				{
					Int16 *srcPtr = (Int16*)sampleBuff;
					UInt8 *destPtr;
					ch = fmt.nChannels;
					while (ch-- > 0)
					{
						destPtr = (UInt8*)buffInfos[ch].buffers[me->bufferIndex];
						i = 0;
						j = ch;
						k = 0;
						while (i < nSamples)
						{
							*(Int16*)&destPtr[k + 1] = *(Int16*)&srcPtr[j];
							*(UInt8*)&destPtr[k] = (UInt8)((*(Int16*)&srcPtr[j]) >> 8);
							i += 1;
							j += fmt.nChannels;
							k += 3;
						}
					}
				}
				else if (fmt.bitpersample == 24)
				{
					UInt8 *srcPtr = (UInt8*)sampleBuff;
					UInt8 *destPtr;
					ch = fmt.nChannels;
					while (ch-- > 0)
					{
						destPtr = (UInt8*)buffInfos[ch].buffers[me->bufferIndex];
						i = 0;
						j = ch * 3;
						k = 0;
						while (i < nSamples)
						{
							*(Int16*)&destPtr[k + 1] = *(Int16*)&srcPtr[j + 1];
							*(UInt8*)&destPtr[k] = *(UInt8*)&srcPtr[j];
							i += 1;
							j += (UInt32)fmt.nChannels * 3;
							k += 3;
						}
					}
				}
				else if (fmt.bitpersample == 32)
				{
					UInt8 *srcPtr = (UInt8*)sampleBuff;
					UInt8 *destPtr;
					ch = fmt.nChannels;
					while (ch-- > 0)
					{
						destPtr = (UInt8*)buffInfos[ch].buffers[me->bufferIndex];
						i = 0;
						j = ch << 2;
						k = 0;
						while (i < nSamples)
						{
							*(Int16*)&destPtr[k + 1] = *(Int16*)&srcPtr[j + 2];
							*(UInt8*)&destPtr[k] = *(UInt8*)&srcPtr[j + 1];
							i += 1;
							j += (UInt32)fmt.nChannels << 2;
							k += 3;
						}
					}
				}
				me->bufferFilled = true;
				asio->outputReady();
			}
			else if (me->sampleTypes[0] == ASIOSTInt32LSB)
			{
				if (me->debug & 1)
				{
					ch = fmt.nChannels;
					while (ch-- > 0)
					{
						ZeroMemory(buffInfos[ch].buffers[me->bufferIndex], nSamples << 2);
					}
				}
				else
				{
					if (fmt.bitpersample == 16)
					{
						Int16 *srcPtr = (Int16*)sampleBuff;
						Int16 *destPtr;
						ch = fmt.nChannels;
						while (ch-- > 0)
						{
							destPtr = (Int16*)buffInfos[ch].buffers[me->bufferIndex];
							i = 0;
							j = ch;
							k = 0;
							while (i < nSamples)
							{
								destPtr[k + 1] = srcPtr[j];
								destPtr[k] = srcPtr[j];
								i += 1;
								j += fmt.nChannels;
								k += 2;
							}
						}
					}
					else if (fmt.bitpersample == 24)
					{
						UInt8 *srcPtr = (UInt8*)sampleBuff;
						UInt8 *destPtr;
						ch = fmt.nChannels;
						while (ch-- > 0)
						{
							destPtr = (UInt8*)buffInfos[ch].buffers[me->bufferIndex];
							i = 0;
							j = ch * 3;
							k = 0;
							while (i < nSamples)
							{
								*(Int16*)&destPtr[k + 2] = *(Int16*)&srcPtr[j + 1];
								*(UInt8*)&destPtr[k + 1] = *(UInt8*)&srcPtr[j];
								*(UInt8*)&destPtr[k] = *(UInt8*)&srcPtr[j + 2];
								i += 1;
								j += (UInt32)fmt.nChannels * 3;
								k += 4;
							}
						}
					}
					else if (fmt.bitpersample == 32)
					{
						Int32 *srcPtr = (Int32*)sampleBuff;
						Int32 *destPtr;
						ch = fmt.nChannels;
						while (ch-- > 0)
						{
							destPtr = (Int32*)buffInfos[ch].buffers[me->bufferIndex];
							i = 0;
							j = ch;
							while (i < nSamples)
							{
								destPtr[i] = srcPtr[j];
								i += 1;
								j += fmt.nChannels;
							}
						}
					}
				}
				me->bufferFilled = true;
				asio->outputReady();
			}
			if (me->audSrc->ReadBlockLPCM(sampleBuff, nSamples * blkAlign, &fmt) == 0)
				break;
		}
		me->bufferEvt->Wait();
	}
	asio->stop();
	MemFree(sampleBuff);
	DEL_CLASS(evt);
	me->playing = false;
	if (me->endHdlr)
	{
		me->endHdlr(me->endHdlrObj);
	}
	return 0;
}

void Media::ASIOOutRenderer::InitDevice(UInt32 devId)
{
	HKEY hkEnum;
	HKEY hkDrv;
	WChar keyname[MAXDRVNAMELEN];
	WChar clsidstr[40];
	//WChar *ret = 0;
	UInt32 clsidsize;
	DWORD nameLen = MAXDRVNAMELEN;
	bufferCreated = false;
	playing = false;
	NEW_CLASS(bufferEvt, Sync::Event());
	Int32 cr = RegOpenKeyW(HKEY_LOCAL_MACHINE, ASIO_PATH, &hkEnum);
	if (cr == ERROR_SUCCESS)
	{
		if ((cr = RegEnumKeyW(hkEnum, devId, keyname, nameLen))== ERROR_SUCCESS)
		{
			if ((cr = RegOpenKeyW(hkEnum, keyname, &hkDrv)) == ERROR_SUCCESS)
			{
				clsidsize = 80;
				cr = RegQueryValueExW(hkDrv, L"CLSID", 0, 0, (BYTE*)clsidstr, (LPDWORD)&clsidsize);
				RegCloseKey(hkDrv);

				if (cr == ERROR_SUCCESS)
				{
					CLSID clsid;
					if (CLSIDFromString(clsidstr, &clsid) == S_OK)
					{
						CoInitialize(0);
						cr = CoCreateInstance(clsid, 0,CLSCTX_INPROC_SERVER, clsid, &asiodrv);
						if (cr == S_OK)
						{
							IASIO *asio = (IASIO*)asiodrv;
							ASIOBool succ = asio->init(0);
							if (succ)
							{
								this->drvName = Text::StrCopyNew(keyname);
							}
							else
							{
								Char sbuff[256];
								asio->getErrorMessage(sbuff);
								
								this->drvName = Text::StrToWCharNew((const UTF8Char*)sbuff);
							}
						}
					}
				}
			}
		}
	}
	if (hkEnum)
		RegCloseKey(hkEnum);
}

UOSInt Media::ASIOOutRenderer::GetDeviceCount()
{
	HKEY hkEnum;
	DWORD nameLen = MAXDRVNAMELEN;
	WChar keyname[MAXDRVNAMELEN];
	UInt32 index = 0;
	Int32 cr = RegOpenKeyW(HKEY_LOCAL_MACHINE, ASIO_PATH, &hkEnum);
	while (cr == ERROR_SUCCESS)
	{
		if ((cr = RegEnumKeyW(hkEnum, index, keyname, nameLen))== ERROR_SUCCESS)
		{
			index++;
		}
	}
	if (hkEnum)
		RegCloseKey(hkEnum);
	return index;
}

UTF8Char *Media::ASIOOutRenderer::GetDeviceName(UTF8Char *buff, UOSInt devNo)
{
	WChar sbuff[MAXDRVNAMELEN];
	HKEY hkEnum;
	UTF8Char *ret = 0;
	DWORD nameLen = MAXDRVNAMELEN;
	Int32 cr = RegOpenKeyW(HKEY_LOCAL_MACHINE, ASIO_PATH, &hkEnum);
	if (cr == ERROR_SUCCESS)
	{
		if ((cr = RegEnumKeyW(hkEnum, (UInt32)devNo, sbuff, nameLen))== ERROR_SUCCESS)
		{
			ret = Text::StrWChar_UTF8(buff, sbuff);
		}
	}
	if (hkEnum)
		RegCloseKey(hkEnum);
	return ret;
}

Int64 Media::ASIOOutRenderer::SwitchBuffer(Int32 index)
{
	if (this->bufferIndex == index)
	{
		this->bufferIndex = index;
		this->bufferOfst += this->bufferSize;
		this->bufferFilled = false;
		this->bufferEvt->Set();
	}
	else
	{
		this->bufferIndex = index;
		this->bufferOfst += this->bufferSize;
		this->bufferFilled = false;
		this->bufferEvt->Set();
	}
	return this->bufferOfst;
}

Media::ASIOOutRenderer::ASIOOutRenderer(const UTF8Char *devName)
{
	asiodrv = 0;
	drvName = 0;
	this->endHdlr = 0;
	InitDevice((UInt32)GetDeviceIndex(devName));
}

Media::ASIOOutRenderer::ASIOOutRenderer(UInt32 devId)
{
	asiodrv = 0;
	drvName = 0;
	this->endHdlr = 0;
	this->buffTime = 0;
	InitDevice(devId);
}

Media::ASIOOutRenderer::~ASIOOutRenderer()
{
	Stop();
	if (drvName)
	{
		Text::StrDelNew(drvName);
		drvName = 0;
	}
	if (asiodrv)
	{
		IASIO *asio = (IASIO*)asiodrv;
		if (bufferCreated)
		{
			asio->disposeBuffers();
			MemFree(this->bufferInfos);
			MemFree(this->sampleTypes);
			bufferCreated = false;
		}
		asio->Release();
		asiodrv = 0;
	}

	DEL_CLASS(bufferEvt);
}

Bool Media::ASIOOutRenderer::IsError()
{
	return this->asiodrv == 0;
}

Bool Media::ASIOOutRenderer::BindAudio(Media::IAudioSource *audsrc)
{
	Media::AudioFormat format;
	IASIO *asio = (IASIO*)asiodrv;
	if (asiodrv == 0)
		return false;

	Stop();
	if (bufferCreated)
	{
		asio->disposeBuffers();
		MemFree(this->bufferInfos);
		MemFree(this->sampleTypes);
		bufferCreated = false;
	}
	if (audsrc == 0)
		return false;

	audsrc->GetFormat(&format);
	if (format.formatId != 1)
	{
		return false;
	}
	Int32 ich;
	Int32 och;
	asio->getChannels((long*)&ich, (long*)&och);
	if (och < format.nChannels)
		return false;
	ASIOError err = asio->canSampleRate((Double)format.frequency);
	if (err != ASE_OK)
		return false;
	ASIOSampleRate srate;
	if (asio->getSampleRate(&srate) != ASE_OK)
		return false;
	if (srate != format.frequency)
	{
		err = asio->setSampleRate((Double)format.frequency);
		if (err != ASE_OK)
		{
			if (asio->getSampleRate(&srate) != ASE_OK)
				return false;
			if (srate != format.frequency)
				return false;
		}
	}

	this->bufferInfos = MemAlloc(ASIOBufferInfo, format.nChannels);
	this->sampleTypes = MemAlloc(Int32, format.nChannels);
	ASIOBufferInfo *buffInfos = (ASIOBufferInfo *)bufferInfos;
	ASIOChannelInfo chInfo;
	Int32 i = format.nChannels;
	while (i-- > 0)
	{
		buffInfos[i].isInput = 0;
		buffInfos[i].channelNum = i;
		chInfo.channel = i;
		chInfo.isInput = 0;
		asio->getChannelInfo(&chInfo);
		this->sampleTypes[i] = chInfo.type;
	}
//	ASIOCallbacks callbacks;
	asioCallbacks.asioMessage = ASIOHdlrMessage;
	asioCallbacks.bufferSwitch = ASIOHdlrBuffSwitch;
	asioCallbacks.bufferSwitchTimeInfo = ASIOHdlrBuffSwitchTI;
	asioCallbacks.sampleRateDidChange = ASIOHdlrSRChg;
	asioCurrRenderer = this;
	long minSize;
	long maxSize;
	long preferredSize;
	long g;
	asio->getBufferSize(&minSize, &maxSize, &preferredSize, &g);
	if (this->buffTime)
	{
		preferredSize = (long)((this->buffTime * format.frequency / 1000) * format.align);
		if (preferredSize < minSize)
			preferredSize = minSize;
		if (preferredSize > maxSize)
			preferredSize = maxSize;
	}
	this->bufferSize = (UInt32)preferredSize;
	err = asio->createBuffers(buffInfos, format.nChannels, preferredSize, &asioCallbacks);
	this->playing = false;
	if (err != ASE_OK)
	{
		MemFree(this->bufferInfos);
		MemFree(this->sampleTypes);
		return false;
	}
	else
	{
		bufferCreated = true;
		this->audSrc = audsrc;
		return true;
	}
}

void Media::ASIOOutRenderer::AudioInit(Media::RefClock *clk)
{
	if (playing)
		return;
	if (this->audSrc == 0)
		return;
	this->clk = clk;
}

void Media::ASIOOutRenderer::Start()
{
	if (!bufferCreated)
		return;

	if (playing)
		return;

	threadInit = false;
	toStop = false;
	playing = true;
	debug = 0;
	Sync::Thread::Create(PlayThread, this);
	while (!threadInit)
	{
		Sync::Thread::Sleep(10);
	}
}

void Media::ASIOOutRenderer::Stop()
{
	if (!playing)
	{
		return;
	}
	toStop = true;
	this->bufferEvt->Set();
	while (this->playing)
	{
		Sync::Thread::Sleep(10);
	}
}

Bool Media::ASIOOutRenderer::IsPlaying()
{
	return playing;
}

void Media::ASIOOutRenderer::SetEndNotify(EndNotifier endHdlr, void *endHdlrObj)
{
	this->endHdlr = endHdlr;
	this->endHdlrObj = endHdlrObj;
}

void Media::ASIOOutRenderer::SetBufferTime(UInt32 ms)
{
	this->buffTime = ms;
}

Int32 Media::ASIOOutRenderer::GetDeviceVolume()
{
	return 65535;
}

void Media::ASIOOutRenderer::SetDeviceVolume(Int32 volume)
{
}


void Media::ASIOOutRenderer::Test()
{
	this->debug ^= 1;
}
