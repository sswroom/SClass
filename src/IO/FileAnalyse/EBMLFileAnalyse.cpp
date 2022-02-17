#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileAnalyse/EBMLFileAnalyse.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

IO::FileAnalyse::EBMLFileAnalyse::ElementInfo IO::FileAnalyse::EBMLFileAnalyse::elements[] = {
	{0x0,       IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("ChapterDisplay")}},
	{0x3,       IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("TrackType")}},
	{0x5,       IO::FileAnalyse::EBMLFileAnalyse::ET_UTF8,   {UTF8STRC("ChapString")}},
	{0x6,       IO::FileAnalyse::EBMLFileAnalyse::ET_STRING, {UTF8STRC("CodecID")}},
	{0x8,       IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("FlagDefault")}},
	{0x9,       IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("ChapterTrackNumber")}},
	{0xE,       IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("Slices")}},
	{0xF,       IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("ChapterTrack")}},
	{0x11,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("ChapterTimeStart")}},
	{0x12,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("ChapterTimeEnd")}},
	{0x16,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("CueRefTime")}},
	{0x17,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("CueRefCluster")}},
	{0x18,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("ChapterFlagHidden")}},
	{0x1A,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("FlagInterlaced")}},
	{0x1B,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("BlockDuration")}},
	{0x1C,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("FlagLacing")}},
	{0x1D,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("FieldOrder")}},
	{0x1F,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("Channels")}},
	{0x20,      IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("BlockGroup")}},
	{0x21,      IO::FileAnalyse::EBMLFileAnalyse::ET_BINARY, {UTF8STRC("Block")}},
	{0x22,      IO::FileAnalyse::EBMLFileAnalyse::ET_BINARY, {UTF8STRC("BlockVirtual")}},
	{0x23,      IO::FileAnalyse::EBMLFileAnalyse::ET_BINARY, {UTF8STRC("SimpleBlock")}},
	{0x24,      IO::FileAnalyse::EBMLFileAnalyse::ET_BINARY, {UTF8STRC("CodecState")}},
	{0x25,      IO::FileAnalyse::EBMLFileAnalyse::ET_BINARY, {UTF8STRC("BlockAdditional")}},
	{0x26,      IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("BlockMore")}},
	{0x27,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("Position")}},
	{0x2A,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("CodecDecodeAll")}},
	{0x2B,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("PrevSize")}},
	{0x2E,      IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("TrackEntry")}},
	{0x2F,      IO::FileAnalyse::EBMLFileAnalyse::ET_BINARY, {UTF8STRC("EncryptedBlock")}},
	{0x30,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("PixelWidth")}},
	{0x32,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("CueDuration")}},
	{0x33,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("CueTime")}},
	{0x35,      IO::FileAnalyse::EBMLFileAnalyse::ET_FLOAT,  {UTF8STRC("SamplingFrequency")}},
	{0x36,      IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("ChapterAtom")}},
	{0x37,      IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("CueTrackPositions")}},
	{0x39,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("FlagEnabled")}},
	{0x3A,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("PixelHeight")}},
	{0x3B,      IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("CuePoint")}},
	{0x3F,      IO::FileAnalyse::EBMLFileAnalyse::ET_BINARY, {UTF8STRC("CRC-32")}},
	{0x40,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("TrickTrackUID")}},
	{0x41,      IO::FileAnalyse::EBMLFileAnalyse::ET_BINARY, {UTF8STRC("TrickTrackSegmentUID")}},
	{0x44,      IO::FileAnalyse::EBMLFileAnalyse::ET_BINARY, {UTF8STRC("TrickMasterTrackSegmentUID")}},
	{0x46,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("TrickTrackFlag")}},
	{0x47,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("TrickMasterTrackUID")}},
	{0x48,      IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("ReferenceFrame")}},
	{0x49,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("ReferenceOffset")}},
	{0x4A,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("ReferenceTimestamp")}},
	{0x4B,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("BlockAdditionID")}},
	{0x4C,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("LaceNumber")}},
	{0x4D,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("FrameNumber")}},
	{0x4E,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("Delay")}},
	{0x4F,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("SliceDuration")}},
	{0x57,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("TrackNumber")}},
	{0x5B,      IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("CueReference")}},
	{0x60,      IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("Video")}},
	{0x61,      IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("Audio")}},
	{0x62,      IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("TrackOperation")}},
	{0x63,      IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("TrackCombinePlanes")}},
	{0x64,      IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("TrackPlane")}},
	{0x65,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("TrackPlaneUID")}},
	{0x66,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("TrackPlaneType")}},
	{0x67,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("Timestamp")}},
	{0x68,      IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("TimeSlice")}},
	{0x69,      IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("TrackJoinBlocks")}},
	{0x6A,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("CueCodecState")}},
	{0x6B,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("CueRefCodecState")}},
	{0x6C,      IO::FileAnalyse::EBMLFileAnalyse::ET_BINARY, {UTF8STRC("Void")}},
	{0x6D,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("TrackJoinUID")}},
	{0x6E,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("BlockAddID")}},
	{0x70,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("CueRelativePosition")}},
	{0x71,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("CueClusterPosition")}},
	{0x77,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("CueTrack")}},
	{0x7A,      IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("ReferencePriority")}},
	{0x7B,      IO::FileAnalyse::EBMLFileAnalyse::ET_SINT,   {UTF8STRC("ReferenceBlock")}},
	{0x7D,      IO::FileAnalyse::EBMLFileAnalyse::ET_SINT,   {UTF8STRC("ReferenceVirtual")}},
	{0x254,     IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("ContentCompAlgo")}},
	{0x255,     IO::FileAnalyse::EBMLFileAnalyse::ET_BINARY, {UTF8STRC("ContentCompSettings")}},
	{0x281,     IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("DocTypeExtension")}},
	{0x282,     IO::FileAnalyse::EBMLFileAnalyse::ET_STRING, {UTF8STRC("DocType")}},
	{0x283,     IO::FileAnalyse::EBMLFileAnalyse::ET_STRING, {UTF8STRC("DocTypeExtensionName")}},
	{0x284,     IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("DocTypeExtensionVersion")}},
	{0x285,     IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("DocTypeReadVersion")}},
	{0x286,     IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("EBMLVersion")}},
	{0x287,     IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("DocTypeVersion")}},
	{0x2F2,     IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("EBMLMaxIDLength")}},
	{0x2F3,     IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("EBMLMaxSizeLength")}},
	{0x2F7,     IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("EBMLReadVersion")}},
	{0x37C,     IO::FileAnalyse::EBMLFileAnalyse::ET_STRING, {UTF8STRC("ChapLanguage")}},
	{0x37D,     IO::FileAnalyse::EBMLFileAnalyse::ET_STRING, {UTF8STRC("ChapLanguageIETF")}},
	{0x37E,     IO::FileAnalyse::EBMLFileAnalyse::ET_STRING, {UTF8STRC("ChapCountry")}},
	{0x444,     IO::FileAnalyse::EBMLFileAnalyse::ET_BINARY, {UTF8STRC("SegmentFamily")}},
	{0x461,     IO::FileAnalyse::EBMLFileAnalyse::ET_DATE,   {UTF8STRC("DateUTC")}},
	{0x47A,     IO::FileAnalyse::EBMLFileAnalyse::ET_STRING, {UTF8STRC("TagLanguage")}},
	{0x47B,     IO::FileAnalyse::EBMLFileAnalyse::ET_STRING, {UTF8STRC("TagLanguageIETF")}},
	{0x484,     IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("TagDefault")}},
	{0x485,     IO::FileAnalyse::EBMLFileAnalyse::ET_BINARY, {UTF8STRC("TagBinary")}},
	{0x487,     IO::FileAnalyse::EBMLFileAnalyse::ET_UTF8,   {UTF8STRC("TagString")}},
	{0x489,     IO::FileAnalyse::EBMLFileAnalyse::ET_FLOAT,  {UTF8STRC("Duration")}},
	{0x50D,     IO::FileAnalyse::EBMLFileAnalyse::ET_BINARY, {UTF8STRC("ChapProcessPrivate")}},
	{0x598,     IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("ChapterFlagEnabled")}},
	{0x5A3,     IO::FileAnalyse::EBMLFileAnalyse::ET_UTF8,   {UTF8STRC("TagName")}},
	{0x5B9,     IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("EditionEntry")}},
	{0x5BC,     IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("EditionUID")}},
	{0x5BD,     IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("EditionFlagHidden")}},
	{0x5DB,     IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("EditionFlagDefault")}},
	{0x5DD,     IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("EditionFlagOrdered")}},
	{0x65C,     IO::FileAnalyse::EBMLFileAnalyse::ET_BINARY, {UTF8STRC("FileData")}},
	{0x660,     IO::FileAnalyse::EBMLFileAnalyse::ET_STRING, {UTF8STRC("FileMimeType")}},
	{0x661,     IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("FileUsedStartTime")}},
	{0x662,     IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("FileUsedEndTime")}},
	{0x66E,     IO::FileAnalyse::EBMLFileAnalyse::ET_UTF8,   {UTF8STRC("FileName")}},
	{0x675,     IO::FileAnalyse::EBMLFileAnalyse::ET_BINARY, {UTF8STRC("FileReferral")}},
	{0x67E,     IO::FileAnalyse::EBMLFileAnalyse::ET_UTF8,   {UTF8STRC("FileDescription")}},
	{0x6AE,     IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("FileUID")}},
	{0x7E1,     IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("ContentEncAlgo")}},
	{0x7E2,     IO::FileAnalyse::EBMLFileAnalyse::ET_BINARY, {UTF8STRC("ContentEncKeyID")}},
	{0x7E3,     IO::FileAnalyse::EBMLFileAnalyse::ET_BINARY, {UTF8STRC("ContentSignature")}},
	{0x7E4,     IO::FileAnalyse::EBMLFileAnalyse::ET_BINARY, {UTF8STRC("ContentSigKeyID")}},
	{0x7E5,     IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("ContentSigAlgo")}},
	{0x7E6,     IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("ContentSigHashAlgo")}},
	{0x7E7,     IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("ContentEncAESSettings")}},
	{0x7E8,     IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("AESSettingsCipherMode")}},
	{0xD80,     IO::FileAnalyse::EBMLFileAnalyse::ET_UTF8,   {UTF8STRC("MuxingApp")}},
	{0xDBB,     IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("Seek")}},
	{0x1031,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("ContentEncodingOrder")}},
	{0x1032,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("ContentEncodingScope")}},
	{0x1033,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("ContentEncodingType")}},
	{0x1034,    IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("ContentCompression")}},
	{0x1035,    IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("ContentEncryption")}},
	{0x135F,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("CueRefNumber")}},
	{0x136E,    IO::FileAnalyse::EBMLFileAnalyse::ET_UTF8,   {UTF8STRC("Name")}},
	{0x1378,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("CueBlockNumber")}},
	{0x137F,    IO::FileAnalyse::EBMLFileAnalyse::ET_SINT,   {UTF8STRC("TrackOffset")}},
	{0x13AB,    IO::FileAnalyse::EBMLFileAnalyse::ET_BINARY, {UTF8STRC("SeekID")}},
	{0x13AC,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("SeekPosition")}},
	{0x13B8,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("StereoMode")}},
	{0x13B9,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("OldStereoMode")}},
	{0x13C0,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("AlphaMode")}},
	{0x14AA,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("PixelCropBottom")}},
	{0x14B0,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("DisplayWidth")}},
	{0x14B2,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("DisplayUnit")}},
	{0x14B3,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("AspectRatioType")}},
	{0x14BA,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("DisplayHeight")}},
	{0x14BB,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("PixelCropTop")}},
	{0x14CC,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("PixelCropLeft")}},
	{0x14DD,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("PixelCropRight")}},
	{0x15AA,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("FlagForced")}},
	{0x15B0,    IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("Colour")}},
	{0x15B1,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("MatrixCoefficients")}},
	{0x15B2,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("BitsPerChannel")}},
	{0x15B3,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("ChromaSubsamplingHorz")}},
	{0x15B4,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("ChromaSubsamplingVert")}},
	{0x15B5,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("CbSubsamplingHorz")}},
	{0x15B6,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("CbSubsamplingVert")}},
	{0x15B7,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("ChromaSitingHorz")}},
	{0x15B8,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("ChromaSitingVert")}},
	{0x15B9,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("Range")}},
	{0x15BA,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("TransferCharacteristics")}},
	{0x15BB,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("Primaries")}},
	{0x15BC,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("MaxCLL")}},
	{0x15BD,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("MaxFALL")}},
	{0x15D0,    IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("MasteringMetadata")}},
	{0x15D1,    IO::FileAnalyse::EBMLFileAnalyse::ET_FLOAT,  {UTF8STRC("PrimaryRChromaticityX")}},
	{0x15D2,    IO::FileAnalyse::EBMLFileAnalyse::ET_FLOAT,  {UTF8STRC("PrimaryRChromaticityY")}},
	{0x15D3,    IO::FileAnalyse::EBMLFileAnalyse::ET_FLOAT,  {UTF8STRC("PrimaryGChromaticityX")}},
	{0x15D4,    IO::FileAnalyse::EBMLFileAnalyse::ET_FLOAT,  {UTF8STRC("PrimaryGChromaticityY")}},
	{0x15D5,    IO::FileAnalyse::EBMLFileAnalyse::ET_FLOAT,  {UTF8STRC("PrimaryBChromaticityX")}},
	{0x15D6,    IO::FileAnalyse::EBMLFileAnalyse::ET_FLOAT,  {UTF8STRC("PrimaryBChromaticityY")}},
	{0x15D7,    IO::FileAnalyse::EBMLFileAnalyse::ET_FLOAT,  {UTF8STRC("WhitePointChromaticityX")}},
	{0x15D8,    IO::FileAnalyse::EBMLFileAnalyse::ET_FLOAT,  {UTF8STRC("WhitePointChromaticityY")}},
	{0x15D9,    IO::FileAnalyse::EBMLFileAnalyse::ET_FLOAT,  {UTF8STRC("LuminanceMax")}},
	{0x15DA,    IO::FileAnalyse::EBMLFileAnalyse::ET_FLOAT,  {UTF8STRC("LuminanceMin")}},
	{0x15EE,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("MaxBlockAdditionID")}},
	{0x1654,    IO::FileAnalyse::EBMLFileAnalyse::ET_UTF8,   {UTF8STRC("ChapterStringUID")}},
	{0x16AA,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("CodecDelay")}},
	{0x16BB,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("SeekPreRoll")}},
	{0x1741,    IO::FileAnalyse::EBMLFileAnalyse::ET_UTF8,   {UTF8STRC("WritingApp")}},
	{0x1854,    IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("SilentTracks")}},
	{0x18D7,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("SilentTrackNumber")}},
	{0x21A7,    IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("AttachedFile")}},
	{0x2240,    IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("ContentEncoding")}},
	{0x2264,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("BitDepth")}},
	{0x23A2,    IO::FileAnalyse::EBMLFileAnalyse::ET_BINARY, {UTF8STRC("CodecPrivate")}},
	{0x23C0,    IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("Targets")}},
	{0x23C3,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("ChapterPhysicalEquiv")}},
	{0x23C4,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("TagChapterUID")}},
	{0x23C5,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("TagTrackUID")}},
	{0x23C6,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("TagAttachmentUID")}},
	{0x23C9,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("TagEditionUID")}},
	{0x23CA,    IO::FileAnalyse::EBMLFileAnalyse::ET_STRING, {UTF8STRC("TargetType")}},
	{0x2624,    IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("TrackTranslate")}},
	{0x26A5,    IO::FileAnalyse::EBMLFileAnalyse::ET_BINARY, {UTF8STRC("TrackTranslateTrackID")}},
	{0x26BF,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("TrackTranslateCodec")}},
	{0x26FC,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("TrackTranslateEditionUID")}},
	{0x27C8,    IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("SimpleTag")}},
	{0x28CA,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("TargetTypeValue")}},
	{0x2911,    IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("ChapProcessCommand")}},
	{0x2922,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("ChapProcessTime")}},
	{0x2924,    IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("ChapterTranslate")}},
	{0x2933,    IO::FileAnalyse::EBMLFileAnalyse::ET_BINARY, {UTF8STRC("ChapProcessData")}},
	{0x2944,    IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("ChapProcess")}},
	{0x2955,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("ChapProcessCodecID")}},
	{0x29A5,    IO::FileAnalyse::EBMLFileAnalyse::ET_BINARY, {UTF8STRC("ChapterTranslateID")}},
	{0x29BF,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("ChapterTranslateCodec")}},
	{0x29FC,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("ChapterTranslateEditionUID")}},
	{0x2D80,    IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("ContentEncodings")}},
	{0x2DE7,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("MinCache")}},
	{0x2DF8,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("MaxCache")}},
	{0x2E67,    IO::FileAnalyse::EBMLFileAnalyse::ET_BINARY, {UTF8STRC("ChapterSegmentUID")}},
	{0x2EBC,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("ChapterSegmentEditionUID")}},
	{0x2FAB,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("TrackOverlay")}},
	{0x3373,    IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("Tag")}},
	{0x3384,    IO::FileAnalyse::EBMLFileAnalyse::ET_UTF8,   {UTF8STRC("SegmentFilename")}},
	{0x33A4,    IO::FileAnalyse::EBMLFileAnalyse::ET_BINARY, {UTF8STRC("SegmentUID")}},
	{0x33C4,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("ChapterUID")}},
	{0x33C5,    IO::FileAnalyse::EBMLFileAnalyse::ET_BINARY, {UTF8STRC("TrackUID")}},
	{0x3446,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("AttachmentLink")}},
	{0x35A1,    IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("BlockAdditions")}},
	{0x35A2,    IO::FileAnalyse::EBMLFileAnalyse::ET_SINT,   {UTF8STRC("DiscardPadding")}},
	{0x3670,    IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("Projection")}},
	{0x3671,    IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("ProjectionType")}},
	{0x3672,    IO::FileAnalyse::EBMLFileAnalyse::ET_BINARY, {UTF8STRC("ProjectionPrivate")}},
	{0x3673,    IO::FileAnalyse::EBMLFileAnalyse::ET_FLOAT,  {UTF8STRC("ProjectionPoseYaw")}},
	{0x3674,    IO::FileAnalyse::EBMLFileAnalyse::ET_FLOAT,  {UTF8STRC("ProjectionPosePitch")}},
	{0x3675,    IO::FileAnalyse::EBMLFileAnalyse::ET_FLOAT,  {UTF8STRC("ProjectionPoseRoll")}},
	{0x38B5,    IO::FileAnalyse::EBMLFileAnalyse::ET_FLOAT,  {UTF8STRC("OutputSamplingFrequency")}},
	{0x3BA9,    IO::FileAnalyse::EBMLFileAnalyse::ET_UTF8,   {UTF8STRC("Title")}},
	{0x3D7B,    IO::FileAnalyse::EBMLFileAnalyse::ET_BINARY, {UTF8STRC("ChannelPositions")}},
	{0x2B59C,   IO::FileAnalyse::EBMLFileAnalyse::ET_STRING, {UTF8STRC("Language")}},
	{0x2B59D,   IO::FileAnalyse::EBMLFileAnalyse::ET_STRING, {UTF8STRC("LanguageIETF")}},
	{0x3314F,   IO::FileAnalyse::EBMLFileAnalyse::ET_FLOAT,  {UTF8STRC("TrackTimestampScale")}},
	{0x34E7A,   IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("DefaultDecodedFieldDuration")}},
	{0x383E3,   IO::FileAnalyse::EBMLFileAnalyse::ET_FLOAT,  {UTF8STRC("FrameRate")}},
	{0x3E383,   IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("DefaultDuration")}},
	{0x58688,   IO::FileAnalyse::EBMLFileAnalyse::ET_UTF8,   {UTF8STRC("CodecName")}},
	{0x6B240,   IO::FileAnalyse::EBMLFileAnalyse::ET_STRING, {UTF8STRC("CodecDownloadURL")}},
	{0xAD7B1,   IO::FileAnalyse::EBMLFileAnalyse::ET_UINT,   {UTF8STRC("TimestampScale")}},
	{0xEB524,   IO::FileAnalyse::EBMLFileAnalyse::ET_BINARY, {UTF8STRC("ColourSpace")}},
	{0xFB523,   IO::FileAnalyse::EBMLFileAnalyse::ET_FLOAT,  {UTF8STRC("GammaValue")}},
	{0x1A9697,  IO::FileAnalyse::EBMLFileAnalyse::ET_UTF8,   {UTF8STRC("CodecSettings")}},
	{0x1B4040,  IO::FileAnalyse::EBMLFileAnalyse::ET_STRING, {UTF8STRC("CodecInfoURL")}},
	{0x1C83AB,  IO::FileAnalyse::EBMLFileAnalyse::ET_UTF8,   {UTF8STRC("PrevFilename")}},
	{0x1CB923,  IO::FileAnalyse::EBMLFileAnalyse::ET_BINARY, {UTF8STRC("PrevUID")}},
	{0x1E83BB,  IO::FileAnalyse::EBMLFileAnalyse::ET_UTF8,   {UTF8STRC("NextFilename")}},
	{0x1EB923,  IO::FileAnalyse::EBMLFileAnalyse::ET_BINARY, {UTF8STRC("NextUID")}},
	{0x14D9B74, IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("SeekHead")}},
	{0x254C367, IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("Tags")}},
	{0x549A966, IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("Info")}},
	{0x654AE6B, IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("Tracks")}},
	{0x8538067, IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("Segment")}},
	{0xA45DFA3, IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("EBML")}},
	{0xC53BB6B, IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("Cues")}},
	{0xF43B675, IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER, {UTF8STRC("Cluster")}},
};

const UInt8 *IO::FileAnalyse::EBMLFileAnalyse::ReadInt(const UInt8 *buffPtr, UInt64 *val, UInt32 *intSize)
{
	if (*buffPtr & 0x80)
	{
		*val = buffPtr[0] & 0x7f;
		if (intSize) *intSize = 1;
		return buffPtr + 1;
	}
	else if (*buffPtr & 0x40)
	{
		*val = (((UInt32)buffPtr[0] & 0x3f) << 8) | (buffPtr[1]);
		if (intSize) *intSize = 2;
		return buffPtr + 2;
	}
	else if (*buffPtr & 0x20)
	{
		*val = (((UInt32)buffPtr[0] & 0x1f) << 16) | (((UInt32)buffPtr[1]) << 8) | (buffPtr[2]);
		if (intSize) *intSize = 3;
		return buffPtr + 3;
	}
	else if (*buffPtr & 0x10)
	{
		*val = (((UInt32)buffPtr[0] & 0xf) << 24) | (((UInt32)buffPtr[1]) << 16) | ((UInt32)buffPtr[2] << 8) | (buffPtr[3]);
		if (intSize) *intSize = 4;
		return buffPtr + 4;
	}
	else if (*buffPtr & 0x8)
	{
		*val = (((UInt64)buffPtr[0] & 0x7) << 32) | (((UInt64)buffPtr[1]) << 24) | (((UInt64)buffPtr[2]) << 16) | (((UInt64)buffPtr[3]) << 8) | ((UInt64)buffPtr[4]);
		if (intSize) *intSize = 5;
		return buffPtr + 5;
	}
	else if (*buffPtr & 0x4)
	{
		*val = (((UInt64)buffPtr[0] & 0x3) << 40) | (((UInt64)buffPtr[1]) << 32) | (((UInt64)buffPtr[2]) << 24) | (((UInt64)buffPtr[3]) << 16) | (((UInt64)buffPtr[4]) << 8) | ((UInt64)buffPtr[5]);
		if (intSize) *intSize = 6;
		return buffPtr + 6;
	}
	else if (*buffPtr & 0x2)
	{
		*val = (((UInt64)buffPtr[0] & 0x1) << 48) | (((UInt64)buffPtr[1]) << 40) | (((UInt64)buffPtr[2]) << 32) | (((UInt64)buffPtr[3]) << 24) | (((UInt64)buffPtr[4]) << 16) | (((UInt64)buffPtr[5]) << 8) | ((UInt64)buffPtr[6]);
		if (intSize) *intSize = 7;
		return buffPtr + 7;
	}
	else if (*buffPtr & 0x1)
	{
		*val = (((UInt64)buffPtr[1]) << 48) | (((UInt64)buffPtr[2]) << 40) | (((UInt64)buffPtr[3]) << 32) | (((UInt64)buffPtr[4]) << 24) | (((UInt64)buffPtr[5]) << 16) | (((UInt64)buffPtr[6]) << 8) | ((UInt64)buffPtr[7]);
		if (intSize) *intSize = 8;
		return buffPtr + 8;
	}
	else
	{
		*val = 0;
		if (intSize) *intSize = 0;
		return buffPtr;
	}
}

const IO::FileAnalyse::EBMLFileAnalyse::ElementInfo *IO::FileAnalyse::EBMLFileAnalyse::GetElementInfo(UInt32 elementId)
{
	const IO::FileAnalyse::EBMLFileAnalyse::ElementInfo *element;
	OSInt i = 0;
	OSInt j = (sizeof(elements) / sizeof(elements[0])) - 1;
	OSInt k;
	while (i <= j)
	{
		k = (i + j) >> 1;
		element = &elements[k];
		if (element->elementId > elementId)
		{
			j = k - 1;
		}
		else if (element->elementId < elementId)
		{
			i = k + 1;
		}
		else
		{
			return element;
		}
	}
	return 0;
}

void IO::FileAnalyse::EBMLFileAnalyse::ParseRange(UOSInt lev, UInt64 ofst, UInt64 size)
{
	UInt8 buff[12];
	UInt64 endOfst = ofst + size;
	const UInt8 *buffPtr;
	UInt64 id;
	UInt64 sz;
	IO::FileAnalyse::EBMLFileAnalyse::PackInfo *pack;

	while (ofst <= (endOfst - 3) && !this->threadToStop)
	{
		if (this->pauseParsing)
		{
			Sync::Thread::Sleep(100);
		}
		else
		{
			this->fd->GetRealData(ofst, 12, buff);
			buffPtr = ReadInt(buff, &id, 0);
			buffPtr = ReadInt(buffPtr, &sz, 0);
			if (ofst + sz + (UOSInt)(buffPtr - buff) > endOfst)
			{
				return;
			}
			pack = MemAlloc(IO::FileAnalyse::EBMLFileAnalyse::PackInfo, 1);
			pack->fileOfst = ofst;
			pack->packSize = (UOSInt)(sz + (UOSInt)(buffPtr - buff));
			pack->lev = lev;
			pack->hdrSize = (UOSInt)(buffPtr - buff);
			WriteNInt32(pack->packType, ReadNInt32(buff));
			this->packs->Add(pack);
			if (lev > this->maxLev)
			{
				this->maxLev = lev;
			}
			
			const IO::FileAnalyse::EBMLFileAnalyse::ElementInfo *element = GetElementInfo((UInt32)id);
			if (element && element->type == ET_MASTER)
			{
				ParseRange(lev + 1, ofst + (UOSInt)(buffPtr - buff), sz);
			}
			ofst += pack->packSize;
		}
	}
}

UInt32 __stdcall IO::FileAnalyse::EBMLFileAnalyse::ParseThread(void *userObj)
{
	IO::FileAnalyse::EBMLFileAnalyse *me = (IO::FileAnalyse::EBMLFileAnalyse*)userObj;
	me->threadRunning = true;
	me->threadStarted = true;
	me->ParseRange(0, 0, me->fd->GetDataSize());
	me->threadRunning = false;
	return 0;
}

UOSInt IO::FileAnalyse::EBMLFileAnalyse::GetFrameIndex(UOSInt lev, UInt64 ofst)
{
	OSInt i = 0;
	OSInt j = (OSInt)this->packs->GetCount() - 1;
	OSInt k;
	PackInfo *pack;
	while (i <= j)
	{
		k = (i + j) >> 1;
		pack = this->packs->GetItem((UOSInt)k);
		if (ofst < pack->fileOfst)
		{
			j = k - 1;
		}
		else if (ofst >= pack->fileOfst + pack->packSize)
		{
			i = k + 1;
		}
		else if (pack->lev < lev)
		{
			i = k + 1;
		}
		else
		{
			return (UOSInt)k;
		}
	}
	return INVALID_INDEX;
}

IO::FileAnalyse::EBMLFileAnalyse::EBMLFileAnalyse(IO::IStreamData *fd)
{
	UInt8 buff[256];
	this->fd = 0;
	this->threadRunning = false;
	this->pauseParsing = false;
	this->threadToStop = false;
	this->threadStarted = false;
	this->maxLev = 0;
	NEW_CLASS(this->packs, Data::SyncArrayList<IO::FileAnalyse::EBMLFileAnalyse::PackInfo*>());
	fd->GetRealData(0, 256, buff);
	if (ReadMInt32(buff) != 0x1A45DFA3)
	{
		return;
	}
	this->fd = fd->GetPartialData(0, fd->GetDataSize());
	Sync::Thread::Create(ParseThread, this);
	while (!this->threadStarted)
	{
		Sync::Thread::Sleep(10);
	}
}

IO::FileAnalyse::EBMLFileAnalyse::~EBMLFileAnalyse()
{
	if (this->threadRunning)
	{
		this->threadToStop = true;
		while (this->threadRunning)
		{
			Sync::Thread::Sleep(10);
		}
	}
	SDEL_CLASS(this->fd);
	LIST_FREE_FUNC(this->packs, MemFree);
	DEL_CLASS(this->packs);
}

Text::CString IO::FileAnalyse::EBMLFileAnalyse::GetFormatName()
{
	return {UTF8STRC("EMBL")};
}

UOSInt IO::FileAnalyse::EBMLFileAnalyse::GetFrameCount()
{
	return this->packs->GetCount();
}

Bool IO::FileAnalyse::EBMLFileAnalyse::GetFrameName(UOSInt index, Text::StringBuilderUTF8 *sb)
{
	IO::FileAnalyse::EBMLFileAnalyse::PackInfo *pack;
	pack = this->packs->GetItem(index);
	if (pack == 0)
		return false;
	sb->AppendChar('+', pack->lev);
	sb->AppendU64(pack->fileOfst);
	sb->AppendC(UTF8STRC(": Type="));
	if (pack->packType[0] & 0x80)
	{
		sb->AppendHexBuff(pack->packType, 1, ' ', Text::LineBreakType::None);
	}
	else if (pack->packType[0] & 0x40)
	{
		sb->AppendHexBuff(pack->packType, 2, ' ', Text::LineBreakType::None);
	}
	else if (pack->packType[0] & 0x20)
	{
		sb->AppendHexBuff(pack->packType, 3, ' ', Text::LineBreakType::None);
	}
	else if (pack->packType[0] & 0x10)
	{
		sb->AppendHexBuff(pack->packType, 4, ' ', Text::LineBreakType::None);
	}
	UInt64 eleId;
	ReadInt(pack->packType, &eleId, 0);
	const IO::FileAnalyse::EBMLFileAnalyse::ElementInfo *element = GetElementInfo((UInt32)eleId);
	if (element)
	{
		sb->AppendUTF8Char(' ');
		sb->Append(element->elementName);
	}
	sb->AppendC(UTF8STRC(", size="));
	sb->AppendI32((Int32)pack->packSize);
	return true;
}

Bool IO::FileAnalyse::EBMLFileAnalyse::GetFrameDetail(UOSInt index, Text::StringBuilderUTF8 *sb)
{
	IO::FileAnalyse::EBMLFileAnalyse::PackInfo *pack;
	pack = this->packs->GetItem(index);
	if (pack == 0)
		return false;

	sb->AppendU64(pack->fileOfst);
	sb->AppendC(UTF8STRC(":"));
	sb->AppendC(UTF8STRC("\r\nType="));
	if (pack->packType[0] & 0x80)
	{
		sb->AppendHexBuff(pack->packType, 1, ' ', Text::LineBreakType::None);
	}
	else if (pack->packType[0] & 0x40)
	{
		sb->AppendHexBuff(pack->packType, 2, ' ', Text::LineBreakType::None);
	}
	else if (pack->packType[0] & 0x20)
	{
		sb->AppendHexBuff(pack->packType, 3, ' ', Text::LineBreakType::None);
	}
	else if (pack->packType[0] & 0x10)
	{
		sb->AppendHexBuff(pack->packType, 4, ' ', Text::LineBreakType::None);
	}
	UInt64 eleId;
	ReadInt(pack->packType, &eleId, 0);
	sb->AppendC(UTF8STRC("\r\nElement ID=0x"));
	sb->AppendHex64V(eleId);
	sb->AppendC(UTF8STRC("\r\nsize="));
	sb->AppendI32((Int32)pack->packSize);
	const IO::FileAnalyse::EBMLFileAnalyse::ElementInfo *element = GetElementInfo((UInt32)eleId);
	if (element)
	{
		sb->AppendC(UTF8STRC("\r\nElement Name="));
		sb->Append(element->elementName);
		sb->AppendC(UTF8STRC("\r\nElement Type="));
		switch (element->type)
		{
		case IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER:
			sb->AppendC(UTF8STRC("Master Element"));
			break;
		case IO::FileAnalyse::EBMLFileAnalyse::ET_SINT:
			sb->AppendC(UTF8STRC("Signed Integer Element"));
			break;
		case IO::FileAnalyse::EBMLFileAnalyse::ET_UINT:
			sb->AppendC(UTF8STRC("Unsigned Integer Element"));
			break;
		case IO::FileAnalyse::EBMLFileAnalyse::ET_FLOAT:
			sb->AppendC(UTF8STRC("Float Element"));
			break;
		case IO::FileAnalyse::EBMLFileAnalyse::ET_STRING:
			sb->AppendC(UTF8STRC("String Element"));
			break;
		case IO::FileAnalyse::EBMLFileAnalyse::ET_UTF8:
			sb->AppendC(UTF8STRC("UTF-8 Element"));
			break;
		case IO::FileAnalyse::EBMLFileAnalyse::ET_DATE:
			sb->AppendC(UTF8STRC("Date Element"));
			break;
		case IO::FileAnalyse::EBMLFileAnalyse::ET_BINARY:
			sb->AppendC(UTF8STRC("Binary Element"));
			break;
		case IO::FileAnalyse::EBMLFileAnalyse::ET_UNKNOWN:
		default:
			sb->AppendC(UTF8STRC("Unknown Element"));
			break;
		}
		UInt8 *buff;
		if (eleId == 0x23) //SimpleBlock
		{
			UInt8 hdr[12];
			const UInt8 *buffPtr;
			UInt64 iVal;
			this->fd->GetRealData(pack->fileOfst + pack->hdrSize, 12, hdr);
			buffPtr = ReadInt(hdr, &iVal, 0);
			sb->AppendC(UTF8STRC("\r\nTrack Number="));
			sb->AppendU64(iVal);
			sb->AppendC(UTF8STRC("\r\nTimecode="));
			sb->AppendI16(ReadMInt16(buffPtr));
			sb->AppendC(UTF8STRC("\r\nFlags=0x"));
			sb->AppendHex8(buffPtr[2]);
			buffPtr += 3;
			sb->AppendC(UTF8STRC("\r\nData:\r\n"));
			UOSInt sz = pack->packSize - pack->hdrSize - (UOSInt)(buffPtr - hdr);
			if (sz <= 64)
			{
				buff = MemAlloc(UInt8, sz);
				this->fd->GetRealData(pack->fileOfst + pack->hdrSize + (UOSInt)(buffPtr - hdr), sz, buff);
				sb->AppendHexBuff(buff, sz, ' ', Text::LineBreakType::CRLF);
				MemFree(buff);
			}
			else
			{
				buff = MemAlloc(UInt8, 32);
				this->fd->GetRealData(pack->fileOfst + pack->hdrSize + (UOSInt)(buffPtr - hdr), 32, buff);
				sb->AppendHexBuff(buff, 32, ' ', Text::LineBreakType::CRLF);
				sb->AppendC(UTF8STRC("\r\n..\r\n"));
				this->fd->GetRealData(pack->fileOfst + pack->packSize - 32, 32, buff);
				sb->AppendHexBuff(buff, 32, ' ', Text::LineBreakType::CRLF);
				MemFree(buff);
			}
		}
		else if (element->type == IO::FileAnalyse::EBMLFileAnalyse::ET_STRING || element->type == IO::FileAnalyse::EBMLFileAnalyse::ET_UTF8)
		{
			sb->AppendC(UTF8STRC("\r\nValue="));
			buff = MemAlloc(UInt8, pack->packSize - pack->hdrSize);
			this->fd->GetRealData(pack->fileOfst + pack->hdrSize, pack->packSize - pack->hdrSize, buff);
			if (buff[pack->packSize - pack->hdrSize - 1] == 0)
			{
				sb->AppendC(buff, pack->packSize - pack->hdrSize - 1);
			}
			else
			{
				sb->AppendC(buff, pack->packSize - pack->hdrSize);
			}
			MemFree(buff);
		}
		else if (element->type == IO::FileAnalyse::EBMLFileAnalyse::ET_UINT)
		{
			sb->AppendC(UTF8STRC("\r\nValue="));
			UOSInt sz = pack->packSize - pack->hdrSize;
			buff = MemAlloc(UInt8, sz);
			this->fd->GetRealData(pack->fileOfst + pack->hdrSize, sz, buff);
			if (sz == 1)
			{
				sb->AppendU16(buff[0]);
			}
			else if (sz == 2)
			{
				sb->AppendU16(ReadMUInt16(&buff[0]));
			}
			else if (sz == 3)
			{
				sb->AppendU32(ReadMUInt24(&buff[0]));
			}
			else if (sz == 4)
			{
				sb->AppendU32(ReadMUInt32(&buff[0]));
			}
			else if (sz == 8)
			{
				sb->AppendU64(ReadMUInt64(&buff[0]));
			}
			MemFree(buff);
		}
		else if (element->type == IO::FileAnalyse::EBMLFileAnalyse::ET_SINT)
		{
			sb->AppendC(UTF8STRC("\r\nValue="));
			UOSInt sz = pack->packSize - pack->hdrSize;
			buff = MemAlloc(UInt8, sz);
			this->fd->GetRealData(pack->fileOfst + pack->hdrSize, sz, buff);
			if (sz == 1)
			{
				sb->AppendI16((Int8)buff[0]);
			}
			else if (sz == 2)
			{
				sb->AppendI16(ReadMInt16(&buff[0]));
			}
			else if (sz == 3)
			{
				sb->AppendI32(ReadMInt24(&buff[0]));
			}
			else if (sz == 4)
			{
				sb->AppendI32(ReadMInt32(&buff[0]));
			}
			else if (sz == 8)
			{
				sb->AppendI64(ReadMInt64(&buff[0]));
			}
			MemFree(buff);
		}
		else if (element->type == IO::FileAnalyse::EBMLFileAnalyse::ET_FLOAT)
		{
			sb->AppendC(UTF8STRC("\r\nValue="));
			UOSInt sz = pack->packSize - pack->hdrSize;
			buff = MemAlloc(UInt8, sz);
			this->fd->GetRealData(pack->fileOfst + pack->hdrSize, sz, buff);
			if (sz == 4)
			{
				Text::SBAppendF32(sb, ReadMFloat(&buff[0]));
			}
			else if (sz == 8)
			{
				Text::SBAppendF64(sb, ReadMDouble(&buff[0]));
			}
			MemFree(buff);
		}
		else if (element->type == IO::FileAnalyse::EBMLFileAnalyse::ET_BINARY)
		{
			sb->AppendC(UTF8STRC("\r\nData:\r\n"));
			UOSInt sz = pack->packSize - pack->hdrSize;
			if (sz <= 64)
			{
				buff = MemAlloc(UInt8, sz);
				this->fd->GetRealData(pack->fileOfst + pack->hdrSize, sz, buff);
				sb->AppendHexBuff(buff, sz, ' ', Text::LineBreakType::CRLF);
				MemFree(buff);
			}
			else
			{
				buff = MemAlloc(UInt8, 32);
				this->fd->GetRealData(pack->fileOfst + pack->hdrSize, 32, buff);
				sb->AppendHexBuff(buff, 32, ' ', Text::LineBreakType::CRLF);
				sb->AppendC(UTF8STRC("\r\n..\r\n"));
				this->fd->GetRealData(pack->fileOfst + pack->packSize - 32, 32, buff);
				sb->AppendHexBuff(buff, 32, ' ', Text::LineBreakType::CRLF);
				MemFree(buff);
			}
		}
	}
	return true;
}

UOSInt IO::FileAnalyse::EBMLFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	UOSInt ret;
	UOSInt i = this->maxLev;
	while (true)
	{
		ret = this->GetFrameIndex(i, ofst);
		if (ret != INVALID_INDEX)
		{
			return ret;
		}
		if (i == 0)
		{
			return INVALID_INDEX;
		}
		i--;
	}
}

IO::FileAnalyse::FrameDetail *IO::FileAnalyse::EBMLFileAnalyse::GetFrameDetail(UOSInt index)
{
	IO::FileAnalyse::EBMLFileAnalyse::PackInfo *pack;
	pack = this->packs->GetItem(index);
	if (pack == 0)
		return 0;

	IO::FileAnalyse::FrameDetail *frame;
	NEW_CLASS(frame, IO::FileAnalyse::FrameDetail(pack->fileOfst, (UInt32)pack->packSize));
	UInt64 eleId;
	UInt32 intSize;
	ReadInt(pack->packType, &eleId, &intSize);
	frame->AddHexBuff(0, intSize, CSTR("Type"), pack->packType, false);
	frame->AddHex64V(0, intSize, CSTR("Element ID"), eleId);
	frame->AddUInt(intSize, pack->hdrSize - intSize, CSTR("Size"), pack->packSize - pack->hdrSize);
	const IO::FileAnalyse::EBMLFileAnalyse::ElementInfo *element = GetElementInfo((UInt32)eleId);
	if (element)
	{
		frame->AddField(0, intSize, {UTF8STRC("Element Name")}, element->elementName);
		switch (element->type)
		{
		case IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER:
			frame->AddField(0, intSize, {UTF8STRC("Element Type")}, {UTF8STRC("Master Element")});
			break;
		case IO::FileAnalyse::EBMLFileAnalyse::ET_SINT:
			frame->AddField(0, intSize, {UTF8STRC("Element Type")}, {UTF8STRC("Signed Integer Element")});
			break;
		case IO::FileAnalyse::EBMLFileAnalyse::ET_UINT:
			frame->AddField(0, intSize, {UTF8STRC("Element Type")}, {UTF8STRC("Unsigned Integer Element")});
			break;
		case IO::FileAnalyse::EBMLFileAnalyse::ET_FLOAT:
			frame->AddField(0, intSize, {UTF8STRC("Element Type")}, {UTF8STRC("Float Element")});
			break;
		case IO::FileAnalyse::EBMLFileAnalyse::ET_STRING:
			frame->AddField(0, intSize, {UTF8STRC("Element Type")}, {UTF8STRC("String Element")});
			break;
		case IO::FileAnalyse::EBMLFileAnalyse::ET_UTF8:
			frame->AddField(0, intSize, {UTF8STRC("Element Type")}, {UTF8STRC("UTF-8 Element")});
			break;
		case IO::FileAnalyse::EBMLFileAnalyse::ET_DATE:
			frame->AddField(0, intSize, {UTF8STRC("Element Type")}, {UTF8STRC("Date Element")});
			break;
		case IO::FileAnalyse::EBMLFileAnalyse::ET_BINARY:
			frame->AddField(0, intSize, {UTF8STRC("Element Type")}, {UTF8STRC("Binary Element")});
			break;
		case IO::FileAnalyse::EBMLFileAnalyse::ET_UNKNOWN:
		default:
			frame->AddField(0, intSize, {UTF8STRC("Element Type")}, {UTF8STRC("Unknown Element")});
			break;
		}
		UInt8 *buff;
		if (eleId == 0x23) //SimpleBlock
		{
			UInt8 hdr[12];
			const UInt8 *buffPtr;
			UInt64 iVal;
			this->fd->GetRealData(pack->fileOfst + pack->hdrSize, 12, hdr);
			buffPtr = ReadInt(hdr, &iVal, &intSize);
			frame->AddUInt(pack->hdrSize, intSize, CSTR("Track Number"), (UOSInt)iVal);
			frame->AddInt(pack->hdrSize + (UOSInt)(buffPtr - hdr), 2, CSTR("Timecode"), ReadMInt16(buffPtr));
			frame->AddHex8(pack->hdrSize + (UOSInt)(buffPtr - hdr) + 2, CSTR("Flags"), buffPtr[2]);
			buffPtr += 3;
			UOSInt sz = pack->packSize - pack->hdrSize - (UOSInt)(buffPtr - hdr);
			if (sz <= 64)
			{
				buff = MemAlloc(UInt8, sz);
				this->fd->GetRealData(pack->fileOfst + pack->hdrSize + (UOSInt)(buffPtr - hdr), sz, buff);
				frame->AddHexBuff(pack->hdrSize + (UOSInt)(buffPtr - hdr), sz, CSTR("Data"), buff, true);
				MemFree(buff);
			}
			else
			{
				buff = MemAlloc(UInt8, 32);
				this->fd->GetRealData(pack->fileOfst + pack->hdrSize + (UOSInt)(buffPtr - hdr), 32, buff);
				Text::StringBuilderUTF8 sb;
				sb.AppendHexBuff(buff, 32, ' ', Text::LineBreakType::CRLF);
				sb.AppendC(UTF8STRC("\r\n..\r\n"));
				this->fd->GetRealData(pack->fileOfst + pack->packSize - 32, 32, buff);
				sb.AppendHexBuff(buff, 32, ' ', Text::LineBreakType::CRLF);
				frame->AddField(pack->hdrSize + (UOSInt)(buffPtr - hdr), sz, {UTF8STRC("Data")}, sb.ToCString());
				MemFree(buff);
			}
		}
		else if (element->type == IO::FileAnalyse::EBMLFileAnalyse::ET_STRING || element->type == IO::FileAnalyse::EBMLFileAnalyse::ET_UTF8)
		{
			buff = MemAlloc(UInt8, pack->packSize - pack->hdrSize);
			this->fd->GetRealData(pack->fileOfst + pack->hdrSize, pack->packSize - pack->hdrSize, buff);
			if (buff[pack->packSize - pack->hdrSize - 1] == 0)
			{
				frame->AddStrS(pack->hdrSize, pack->packSize - pack->hdrSize, element->elementName, buff);
			}
			else
			{
				frame->AddStrC(pack->hdrSize, pack->packSize - pack->hdrSize, element->elementName, buff);
			}
			MemFree(buff);
		}
		else if (element->type == IO::FileAnalyse::EBMLFileAnalyse::ET_UINT)
		{
			UOSInt sz = pack->packSize - pack->hdrSize;
			buff = MemAlloc(UInt8, sz);
			this->fd->GetRealData(pack->fileOfst + pack->hdrSize, sz, buff);
			if (sz == 1)
			{
				frame->AddUInt(pack->hdrSize, sz, element->elementName, buff[0]);
			}
			else if (sz == 2)
			{
				frame->AddUInt(pack->hdrSize, sz, element->elementName, ReadMUInt16(&buff[0]));
			}
			else if (sz == 3)
			{
				frame->AddUInt(pack->hdrSize, sz, element->elementName, ReadMUInt24(&buff[0]));
			}
			else if (sz == 4)
			{
				frame->AddUInt(pack->hdrSize, sz, element->elementName, ReadMUInt32(&buff[0]));
			}
			else if (sz == 8)
			{
				frame->AddUInt64(pack->hdrSize, element->elementName, ReadMUInt64(&buff[0]));
			}
			MemFree(buff);
		}
		else if (element->type == IO::FileAnalyse::EBMLFileAnalyse::ET_SINT)
		{
			UOSInt sz = pack->packSize - pack->hdrSize;
			buff = MemAlloc(UInt8, sz);
			this->fd->GetRealData(pack->fileOfst + pack->hdrSize, sz, buff);
			if (sz == 1)
			{
				frame->AddInt(pack->hdrSize, sz, element->elementName, (Int8)buff[0]);
			}
			else if (sz == 2)
			{
				frame->AddInt(pack->hdrSize, sz, element->elementName, ReadMInt16(&buff[0]));
			}
			else if (sz == 3)
			{
				frame->AddInt(pack->hdrSize, sz, element->elementName, ReadMInt24(&buff[0]));
			}
			else if (sz == 4)
			{
				frame->AddInt(pack->hdrSize, sz, element->elementName, ReadMInt32(&buff[0]));
			}
			else if (sz == 8)
			{
				frame->AddInt64(pack->hdrSize, element->elementName, ReadMInt64(&buff[0]));
			}
			MemFree(buff);
		}
		else if (element->type == IO::FileAnalyse::EBMLFileAnalyse::ET_FLOAT)
		{
			UOSInt sz = pack->packSize - pack->hdrSize;
			buff = MemAlloc(UInt8, sz);
			this->fd->GetRealData(pack->fileOfst + pack->hdrSize, sz, buff);
			if (sz == 4)
			{
				frame->AddFloat(pack->hdrSize, 4, element->elementName, ReadMFloat(&buff[0]));
			}
			else if (sz == 8)
			{
				frame->AddFloat(pack->hdrSize, 8, element->elementName, ReadMDouble(&buff[0]));
			}
			MemFree(buff);
		}
		else if (element->type == IO::FileAnalyse::EBMLFileAnalyse::ET_BINARY)
		{
			UOSInt sz = pack->packSize - pack->hdrSize;
			if (sz <= 64)
			{
				buff = MemAlloc(UInt8, sz);
				this->fd->GetRealData(pack->fileOfst + pack->hdrSize, sz, buff);
				frame->AddHexBuff(pack->hdrSize, sz, element->elementName, buff, true);
				MemFree(buff);
			}
			else
			{
				buff = MemAlloc(UInt8, 32);
				this->fd->GetRealData(pack->fileOfst + pack->hdrSize, 32, buff);
				Text::StringBuilderUTF8 sb;
				sb.AppendHexBuff(buff, 32, ' ', Text::LineBreakType::CRLF);
				sb.AppendC(UTF8STRC("\r\n..\r\n"));
				this->fd->GetRealData(pack->fileOfst + pack->packSize - 32, 32, buff);
				sb.AppendHexBuff(buff, 32, ' ', Text::LineBreakType::CRLF);
				MemFree(buff);
				frame->AddField(pack->hdrSize, sz, element->elementName, sb.ToCString());
			}
		}
		else if (element->type == IO::FileAnalyse::EBMLFileAnalyse::ET_MASTER)
		{
			frame->AddSubframe(pack->hdrSize, pack->packSize - pack->hdrSize);
		}
	}
	return frame;
}

Bool IO::FileAnalyse::EBMLFileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::EBMLFileAnalyse::IsParsing()
{
	return this->threadRunning;
}

Bool IO::FileAnalyse::EBMLFileAnalyse::TrimPadding(Text::CString outputFile)
{
/*	UInt8 *readBuff;
	OSInt readSize;
	OSInt buffSize;
	OSInt j;
	OSInt frameSize;
	Int64 readOfst;
	Bool valid = true;
	IO::FileStream *dfs;
	NEW_CLASS(dfs, IO::FileStream(outputFile, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (dfs->IsError())
	{
		DEL_CLASS(dfs);
		return false;
	}
	readBuff = MemAlloc(UInt8, 1048576);
	buffSize = 0;
	readOfst = 0;
	while (true)
	{
		if (buffSize < 256)
		{
			readSize = this->fd->GetRealData(readOfst, 256, &readBuff[buffSize]);
			readOfst += readSize;
			buffSize += readSize;
		}
		if (buffSize < 4)
			break;
		j = 0;
		while (readBuff[j] != 0 || readBuff[j + 1] != 0 || readBuff[j + 2] != 1)
		{
			j++;
			if (j >= buffSize - 4)
			{
				break;
			}
		}
		if (j >= buffSize - 4 && buffSize > 4)
		{
			MemCopy(readBuff, &readBuff[j], buffSize - j);
			buffSize -= j;
			continue;
		}
		if (readBuff[j] != 0 || readBuff[j + 1] != 0 || readBuff[j + 2] != 1)
		{
			valid = false;
			break;
		}
		if (readBuff[j + 3] == 0xB9) //End Of File
		{
			dfs->Write(&readBuff[j], 4);
			break;
		}
		if (readBuff[j + 3] == 0xba) 
		{
			if ((readBuff[j + 4] & 0xc0) == 0x40)
			{
				frameSize = 14 + (readBuff[j + 13] & 7);
			}
			else if ((readBuff[j + 4] & 0xf0) == 0x20)
			{
				frameSize = 12;
			}
			else
			{
				valid = false;
				break;
			}
		}
		else
		{
			frameSize = 6 + ReadMUInt16(&readBuff[j + 4]);
		}
		if (j + frameSize <= buffSize)
		{
			dfs->Write(&readBuff[j], frameSize);
			if (j + frameSize < buffSize)
			{
				MemCopy(readBuff, &readBuff[j + frameSize], buffSize - j - frameSize);
				buffSize -= j + frameSize;
			}
			else
			{
				buffSize = 0;
			}
		}
		else
		{
			readSize = this->fd->GetRealData(readOfst, j + frameSize - buffSize, &readBuff[buffSize]);
			readOfst += readSize;
			if (readSize == j + frameSize - buffSize)
			{
				dfs->Write(&readBuff[j], frameSize);
				buffSize = 0;
			}
			else
			{
				valid = false;
				break;
			}
		}
	}

	MemFree(readBuff);
	DEL_CLASS(dfs);
	return valid;*/
	return false;
}
