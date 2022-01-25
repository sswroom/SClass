#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/GeographicCoordinateSystem.h"
#include "Math/Math.h"
#include "Media/EXIFData.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"
// http://hul.harvard.edu/jhove/tiff-tags.html

Media::EXIFData::EXIFInfo Media::EXIFData::defInfos[] = {
	{254, "NewSubfileType"},
	{255, "SubfileType"},
	{256, "Width"},
	{257, "Height"},
	{258, "BitPerSample (R, G, B)"},
	{259, "Compression"},
	{262, "PhotometricInterpretation"},
	{263, "Threshholding"},
	{264, "CellWidth"},
	{265, "CellLength"},
	{266, "FillOrder"},
	{267, "DocumentName"},
	{270, "ImageDescription"},
	{271, "Make"},
	{272, "Model"},
	{273, "StripOffsets"},
	{274, "Orientation"},
	{277, "SamplesPerPixel"},
	{278, "RowsPerStrip"},
	{279, "StripByteCounts"},
	{280, "MinSampleValue"},
	{281, "MaxSampleValue"},
	{282, "XResolution"},
	{283, "YResolution"},
	{284, "PlanarConfiguration"},
	{285, "PageName"},
	{286, "XPosition"},
	{287, "YPosition"},
	{288, "FreeOffsets"},
	{289, "FreeByteCounts"},
	{290, "GrayResponseUnit"},
	{291, "GrayResponseCurve"},
	{292, "T4Options"},
	{293, "T6Options"},
	{296, "ResolutionUnit"},
	{297, "PageNumber"},
	{301, "TransferFunction"},
	{305, "Software"},
	{306, "DateTime"},
	{315, "Artist"},
	{316, "HostComputer"},
	{317, "Predictor"},
	{318, "WhitePoint"},
	{319, "PrimaryChromaticities"},
	{320, "ColorMap"},
	{321, "HalftoneHints"},
	{322, "TileWidth"},
	{323, "TileLength"},
	{324, "TileOffsets"},
	{325, "TileByteCounts"},
	{332, "InkSet"},
	{333, "InkNames"},
	{334, "NumberOfInks"},
	{336, "DotRange"},
	{337, "TargetPrinter"},
	{338, "ExtraSamples"},
	{339, "SampleFormat"},
	{340, "SMinSampleValue"},
	{341, "SMaxSampleValue"},
	{342, "TransferRange"},
	{343, "ClipPath"},
	{344, "XClipPathUnits"},
	{345, "YClipPathUnits"},
	{346, "Indexed"},
	{351, "OPIProxy"},
	{437, "JPEG tables"},
	{512, "JPEGProc"},
	{513, "JPEGInterchangeFormat"},
	{514, "JPEGInterchangeFormatLngth"},
	{515, "JPEGRestartInterval"},
	{517, "JPEGLosslessPredictors"},
	{518, "JPEGPointTransforms"},
	{519, "JPEGQTables"},
	{520, "JPEGDCTables"},
	{521, "JPEGACTables"},
	{529, "YCbCrCoefficients"},
	{530, "YCbCrSubSampling"},
	{531, "YCbCrPositioning"},
	{532, "ReferenceBlackWhite"},
	{700, "Photoshop XMP"},
	{32781, "ImageID"},
	{32995, "Matteing"},
	{32996, "DataType"},
	{32997, "ImageDepth"},
	{32998, "TileDepth"},
	{33421, "CFARepeatPatternDim"},
	{33422, "CFAPattern"},
	{33423, "BatteryLevel"},
	{33432, "Copyright"},
	{33434, "ExposureTime"},
	{33437, "Fnumber"},
	{33723, "IPTC/NAA"},
	{33550, "ModelPixelScaleTag"},
	{33920, "IntergraphMatrixTag"},
	{33922, "ModelTiepointTag"},
	{34016, "Site"},
	{34017, "ColorSequence"},
	{34018, "IT8Header"},
	{34019, "RasterPadding"},
	{34020, "BitsPerRunLength"},
	{34021, "BitsPerExtendedRunLength"},
	{34022, "ColorTable"},
	{34023, "ImageColorIndicator"},
	{34024, "BackgroundColorIndicator"},
	{34025, "ImageColorValue"},
	{34026, "BackgroundColorValue"},
	{34027, "PixelInensityRange"},
	{34028, "TransparencyIndicator"},
	{34029, "ColorCharacterization"},
	{34030, "HCUsage"},
	{34264, "ModelTransformationTag"},
	{34377, "PhotoshopImageResources"},
	{34665, "ExifIFD"},
	{34675, "InterColourProfile"},
	{34732, "ImageLayer"},
	{34735, "GeoKeyDirectoryTag"},
	{34736, "GeoDoubleParamsTag"},
	{34737, "GeoAsciiParamsTag"},
	{34850, "ExposureProgram"},
	{34852, "SpectralSensitivity"},
	{34853, "GPSInfo"},
	{34855, "ISOSpeedRatings"},
	{34856, "OECF"},
	{34857, "Interlace"},
	{34858, "TimeZoneOffset"},
	{34859, "SelfTimerMode"},
	{34908, "FaxRecvParams"},
	{34909, "FaxSubAddress"},
	{34910, "FaxRecvTime"},
	{36867, "DateTimeOriginal"},
	{37122, "CompressedBitsPerPixel"},
	{37377, "ShutterSpeedValue"},
	{37378, "ApertureValue"},
	{37379, "BrightnessValue"},
	{37380, "ExposureBiasValue"},
	{37381, "MaxApertureValue"},
	{37382, "SubjectDistance"},
	{37383, "MeteringMode"},
	{37384, "LightSource"},
	{37385, "Flash"},
	{37386, "FocalLength"},
	{37387, "FlashEnergy"},
	{37388, "SpatialFrequencyResponse"},
	{37389, "Noise"},
	{37390, "FocalPlaneXResolution"},
	{37391, "FocalPlaneYResolution"},
	{37392, "FocalPlaneResolutionUnit"},
	{37393, "ImageNumber"},
	{37394, "SecurityClassification"},
	{37395, "ImageHistory"},
	{37396, "SubjectLocation"},
	{37397, "ExposureIndex"},
	{37398, "TIFF/EPStandardID"},
	{37399, "SensingMethod"},
	{37439, "StoNits"},
	{37724, "ImageSourceData"},
	{40091, "XPTitle"},
	{40092, "XPComment"},
	{40093, "XPAuthor"},
	{40094, "XPKeywords"},
	{40095, "XPSubject"},
	{40965, "InteroperabilityIFD"},
	{41988, "DigitalZoomRatio"},
	{42016, "ImageUniqueID"},
	{50255, "PhotoshopAnnotations"},
	{50706, "DNGVersion"},
	{50707, "DNGBackwardVersion"},
	{50708, "UniqueCameraModel"},
	{50709, "LocalizedCameraModel"},
	{50710, "CFAPlaneColor"},
	{50711, "CFALayout"},
	{50712, "LinearizationTable"},
	{50713, "BlackLevelRepeatDim"},
	{50714, "BlackLevel"},
	{50715, "BlackLevelDeltaH"},
	{50716, "BlackLevelDeltaV"},
	{50717, "WhiteLevel"},
	{50718, "DefaultScale"},
	{50719, "DefaultCropOrigin"},
	{50720, "DefaultCropSize"},
	{50721, "ColorMatrix1"},
	{50722, "ColorMatrix2"},
	{50723, "CameraCalibration1"},
	{50724, "CameraCalibration2"},
	{50725, "ReductionMatrix1"},
	{50726, "ReductionMatrix2"},
	{50727, "AnalogBalnace"},
	{50728, "AsShortNeutral"},
	{50729, "AsShortWhiteXY"},
	{50730, "BaselineExposure"},
	{50731, "BaselineNoise"},
	{50732, "BaselineSharpness"},
	{50733, "BayerGreenSplit"},
	{50734, "LinearResponseLimit"},
	{50735, "CameraSerialNumber"},
	{50736, "LensInfo"},
	{50737, "ChromaBlurRadius"},
	{50738, "AntiAliasStrength"},
	{50740, "DNGPrivateData"},
	{50741, "MakerNoteSafety"},
	{50778, "CalibrationIlluminant1"},
	{50779, "CalibrationIlluminant2"},
	{50780, "BestQualityScale"}
};

Media::EXIFData::EXIFInfo Media::EXIFData::exifInfos[] = {
	{33434, "ExposureTime"},
	{33437, "FNumber"},
	{34850, "ExposureProgram"},
	{34852, "SpectralSensitibity"},
	{34855, "ISOSpeedRatings"},
	{34856, "OECF"},
	{36864, "ExifVersion"},
	{36867, "DateTimeOriginal"},
	{36868, "DateTimeDigitized"},
	{37121, "ComponentsConfiguration"},
	{37122, "CompressedBitsPerPixel"},
	{37377, "ShutterSpeedValue"},
	{37378, "ApertureValue"},
	{37379, "BrightnessValue"},
	{37380, "ExposureBiasValue"},
	{37381, "MaxApertureValue"},
	{37382, "SubjectDistance"},
	{37383, "MeteringMode"},
	{37384, "LightSource"},
	{37385, "Flash"},
	{37386, "FocalLength"},
	{37396, "SubjectArea"},
	{37500, "MakerNote"},
	{37510, "UserComment"},
	{37520, "SubSecTime"},
	{37521, "SubSecTimeOriginal"},
	{37522, "SubSecTimeDigitized"},
	{37890, "Pressure"}, //hPa
	{37891, "WaterDepth"}, //m
	{37892, "Acceleration"}, //mGal
	{37893, "CameraElevationAngle"},
	{40960, "FlashpixVersion"},
	{40961, "ColorSpace"},
	{40962, "PixelXDimension"},
	{40963, "PixelYDimension"},
	{40964, "RelatedSoundFile"},
	{40965, "InteroperabilityIFD"},
	{41483, "FlashEnergy"},
	{41484, "SpatialFrequencyResponse"},
	{41486, "FocalPlaneXResolution"},
	{41487, "FocalPlaneYResolution"},
	{41488, "FocalPlaneResolutionUnit"},
	{41492, "SubjectLocation"},
	{41493, "ExposureIndex"},
	{41495, "SensingMethod"},
	{41728, "FileSource"},
	{41729, "SceneType"},
	{41730, "CFAPattern"},
	{41985, "CustomRendered"},
	{41986, "ExposureMode"},
	{41987, "WhiteBalance"},
	{41988, "DigitalZoomRatio"},
	{41989, "FocalLengthIn35mmFilm"},
	{41990, "SceneCaptureType"},
	{41991, "GainControl"},
	{41992, "Contrast"},
	{41993, "Saturation"},
	{41994, "Sharpness"},
	{41995, "DeviceSettingDescription"},
	{41996, "SubjectDistanceRange"},
	{42016, "ImageUniqueID"},
	{42032, "CameraOwnerName"},
	{42033, "BodySerialNumber"},
	{42034, "LensSpecification"},
	{42035, "LensMake"},
	{42036, "LensModel"},
	{42037, "LensSerialNumber"}
};

Media::EXIFData::EXIFInfo Media::EXIFData::gpsInfos[] = {
	{0, "GPSVersionID"},
	{1, "GPSLatitudeRef"},
	{2, "GPSLatitude"},
	{3, "GPSLongitudeRef"},
	{4, "GPSLongitude"},
	{5, "GPSAltitudeRef"},
	{6, "GPSAltitude"},
	{7, "GPSTimeStamp"},
	{8, "GPSSatellites"},
	{9, "GPSStatus"},
	{10, "GPSMeasureMode"},
	{11, "GPSDOP"},
	{12, "GPSSpeedRef"},
	{13, "GPSSpeed"},
	{14, "GPSTrackRef"},
	{15, "GPSTrack"},
	{16, "GPSImgDirectionRef"},
	{17, "GPSImgDirection"},
	{18, "GPSMapDatum"},
	{19, "GPSDestLatitudeRef"},
	{20, "GPSDestLatitude"},
	{21, "GPSDestLongitudeRef"},
	{22, "GPSDestLongitude"},
	{23, "GPSDestBearingRef"},
	{24, "GPSDestBearing"},
	{25, "GPSDestDistanceRef"},
	{26, "GPSDestDistance"},
	{27, "GPSProcessingMethod"},
	{28, "GPSAreaInformation"},
	{29, "GPSDateStamp"},
	{30, "GPSDifferential"}
};

Media::EXIFData::EXIFInfo Media::EXIFData::panasonicInfos[] = {
	{1, "Quality"},
	{2, "FirmwareVersion"},
	{3, "WhiteBalance"},
	{7, "FocusMode"},
	{15, "AFMode"},
	{26, "ImageStabilization"},
	{28, "Macro"},
	{31, "ShootingMode"},
	{32, "Audio"},
	{36, "FlashBias"},
	{37, "InternalSerialNumber"},
	{38, "ExifVersion"},
	{40, "ColorEffect"},
	{41, "TimeSincePowerOn"},
	{42, "BurstMode"},
	{43, "SequenceNumber"},
	{44, "Contrast"},
	{45, "NoiseReduction"},
	{46, "SelfTimer"},
	{48, "Rotation"},
	{49, "AFAssistLamp"},
	{50, "ColorMode"},
	{51, "BabyAge1"},
	{52, "OpticalZoomMode"},
	{53, "ConversionLens"},
	{54, "TravelDay"},
	{57, "Contrast"},
	{58, "WorldTimeLocation"},
	{59, "TestStamp1"},
	{60, "ProgramISO"},
	{61, "AdvancedSceneType"},
	{62, "TextStampe2"},
	{63, "FacesDetected"},
	{64, "Saturation"},
	{65, "Sharpness"},
	{68, "ColorTempKelvin"},
	{69, "BracketSettings"},
	{70, "WBAdjustAB"},
	{71, "WBAdjustGM"},
	{72, "FlashCurtain"},
	{73, "LongShutterNoiseReduction"},
	{75, "ImageWidth"},
	{76, "ImageHeight"},
	{77, "AFPointPosition"},
	{78, "FaceDetInfo"},
	{81, "LensType"},
	{82, "LensSerialNumber"},
	{83, "AccessoryTyp"},
	{84, "AccessorySerialNumber"},
	{96, "LensFirmwareVersion"},
	{97, "FaceRecInfo"},
	{101, "Title"},
	{102, "BabyName"},
	{103, "Location"},
	{105, "Country"},
	{107, "State"},
	{109, "City"},
	{111, "Landmark"},
	{112, "IntelligentResolution"},
	{119, "BurstSpeed"},
	{121, "IntelligentDRange"},
	{124, "ClearRetouch"},
	{128, "City2"},
	{137, "PhotoStyle"},
	{138, "ShadingCompensation"},
	{140, "AccelerometerZ"},
	{141, "AccelerometerX"},
	{142, "AccelerometerY"},
	{143, "CameraOrientation"},
	{144, "RollAngle"},
	{145, "PitchAngle"},
	{147, "SweepPanoramaDirection"},
	{148, "PanoramaFieldOfView"},
	{150, "TimerRecording"},
	{157, "InternalNDFilter"},
	{158, "HDR"},
	{159, "ShutterType"},
	{163, "ClearRetouchValue"},
	{171, "TouchAE"},
	{32768, "MakerNoteVersion"},
	{32769, "SceneMode"},
	{32772, "WBRedLevel"},
	{32773, "WBGreenLevel"},
	{32774, "WBBlueLevel"},
	{32775, "FlashFired"},
	{32776, "TextStamp3"},
	{32777, "TextStamp4"},
	{32784, "babyAge2"}
};

Media::EXIFData::EXIFInfo Media::EXIFData::canonInfos[] = {
	{1, "CanonCameraSettings"},
	{2, "CanonFocalLength"},
	{3, "CanonFlashInfo"},
	{4, "CanonShotInfo"},
	{5, "CanonPanorama"},
	{6, "CanonImageType"},
	{7, "CanonFirmwareVersion"},
	{8, "FileNumber"},
	{9, "OwnerName"},
	{10, "UnknownD30"},
	{12, "SerialNumber"},
	{13, "CanonCameraInfo"},
	{14, "CanonFileLength"},
	{15, "CustomFunctions"},
	{16, "CanonModelID"},
	{17, "MovieInfo"},
	{18, "CanonAFInfo"},
	{19, "ThumbnailImageValidArea"},
	{21, "SerialNumberFormat"},
	{26, "SuperMacro"},
	{28, "DateStampMode"},
	{29, "MyColors"},
	{30, "FirmwareRevision"},
	{35, "FaceDetect1"},
	{36, "FaceDetect2"},
	{37, "Categories"},
	{38, "CanonAFInfo2"},
	{39, "ContrastInfo"},
	{40, "ImageUniqueID"},
	{47, "FaceDetect3"},
	{53, "TimeInfo"},
	{56, "BatteryType"},
	{60, "AFInfo3"},
	{129, "RawDataOffset"},
	{131, "OriginalDecisionData"},
	{144, "CustomFunctions1D"},
	{145, "PersonalFunctions"},
	{146, "PersonalFunctionValues"},
	{147, "CanonFileInfo"},
	{148, "AFPointsInFocus1D"},
	{149, "LensModel"},
	{150, "InternalSerialNumber"},
	{151, "DustRemovalData"},
	{152, "CropInfo"},
	{153, "CustomFunctions2"},
	{154, "AspectInfo"},
	{160, "ProcessingInfo"},
	{161, "ToneCurveTable"},
	{162, "SharpnessTable"},
	{163, "SharpnessFreqTable"},
	{164, "WhiteBalanceTable"},
	{169, "ColorBalance"},
	{170, "MeasuredColor"},
	{174, "ColorTemperature"},
	{176, "CanonFlags"},
	{177, "ModifiedInfo"},
	{178, "ToneCurveMatching"},
	{179, "WhiteBalanceMatching"},
	{180, "ColorSpace"},
	{182, "PreviewImageInfo"},
	{208, "VROffset"},
	{224, "SensorInfo"},
	{0x4001, "ColorBalance"},
	{0x4002, "UnknownBlock1"},
	{0x4003, "ColorInfo"},
	{0x4005, "Flavor"},
	{0x4008, "PictureStyleUserDef"},
	{0x4009, "PictureStylePC"},
	{0x4010, "CustomPictureStyleFileName"},
	{0x4013, "AFMicroAdj"},
	{0x4015, "VignettingCorr"},
	{0x4016, "VignettingCorr2"},
	{0x4018, "LightingOpt"},
	{0x4019, "LensInfo"},
	{0x4020, "AmbienceInfo"},
	{0x4021, "MultiExp"},
	{0x4024, "FilterInfo"},
	{0x4025, "HDRInfo"},
	{0x4028, "AFConfig"}
};

Media::EXIFData::EXIFInfo Media::EXIFData::olympusInfos[] = {
	{0x0000, "MakerNoteVersion"},
	{0x0001, "MinoltaCameraSettingsOld"},
	{0x0003, "MinoltaCameraSettings"},
	{0x0040, "CompressedImageSize"},
	{0x0081, "PreviewImageData"},
	{0x0088, "PreviewImageStart"},
	{0x0089, "PreviewImageLength"},
	{0x0100, "ThumbnailImage"},
	{0x0104, "BodyFirmwareVersion"},
	{0x0200, "SpecialMode"},
	{0x0201, "Quality"},
	{0x0202, "Macro"},
	{0x0203, "BWMode"},
	{0x0204, "DigitalZoom"},
	{0x0205, "FocalPlaneDiagonal"},
	{0x0206, "LensDistortionParmas"},
	{0x0207, "CameraType"},
	{0x0208, "CameraInfo"},
	{0x0209, "CameraID"},
	{0x020b, "EpsonImageWidth"},
	{0x020c, "EpsonImageHeight"},
	{0x020d, "EpsonSoftware"},
	{0x0280, "PreviewImage"},
	{0x0300, "PreCaptureFrames"},
	{0x0301, "WhiteBoard"},
	{0x0302, "OneTouchWB"},
	{0x0303, "WhiteBalanceBracket"},
	{0x0304, "WhiteBalanceBias"},
	{0x0401, "BlackLevel"},
	{0x0403, "SceneMode"},
	{0x0404, "SerialNumber"},
	{0x0405, "Firmware"},
	{0x0e00, "PrintIM"},
	{0x0f00, "DataDump"},
	{0x0f01, "DataDump2"},
	{0x0f04, "ZoomedPreviewStart"},
	{0x0f05, "ZoomedPreviewLength"},
	{0x0f06, "ZoomedPreviewSize"},
	{0x1000, "ShutterSpeedValue"},
	{0x1001, "ISOValue"},
	{0x1002, "ApertureValue"},
	{0x1003, "BrightnessValue"},
	{0x1004, "FlashMode"},
	{0x1005, "FlashDevice"},
	{0x1006, "ExposureCompensation"},
	{0x1007, "SensorTemperature"},
	{0x1008, "LensTemperature"},
	{0x1009, "LightCondition"},
	{0x100a, "FocusRange"},
	{0x100b, "FocusMode"},
	{0x100c, "ManualFocusDistance"},
	{0x100d, "ZoomStepCount"},
	{0x100e, "FocusStepCount"},
	{0x100f, "Sharpness"},
	{0x1010, "FlashChargeLevel"},
	{0x1011, "ColorMatrix"},
	{0x1012, "BlackLevel"},
	{0x1013, "ColorTemperatureBG"},
	{0x1014, "ColorTemperatureRG"},
	{0x1015, "WBMode"},
	{0x1017, "RedBalance"},
	{0x1018, "BlueBalance"},
	{0x1019, "ColorMatrixNumber"},
	{0x101a, "SerialNumber"},
	{0x101b, "ExternalFlashAE1_0"},
	{0x101c, "ExternalFlashAE2_0"},
	{0x101d, "InternalFlashAE1_0"},
	{0x101e, "InternalFlashAE2_0"},
	{0x101f, "ExternalFlashAE1"},
	{0x1020, "ExternalFlashAE2"},
	{0x1021, "InternalFlashAE1"},
	{0x1022, "InternalFlashAE2"},
	{0x1023, "FlashExposureComp"},
	{0x1024, "InternalFlashTable"},
	{0x1025, "ExternalFlashGValue"},
	{0x1026, "ExternalFlashBounce"},
	{0x1027, "ExternalFlashZoom"},
	{0x1028, "ExternalFlashMode"},
	{0x1029, "Contrast"},
	{0x102a, "SharpnessFactor"},
	{0x102b, "ColorControl"},
	{0x102c, "ValidBits"},
	{0x102d, "CoringFilter"},
	{0x102e, "OlympusImageWidth"},
	{0x102f, "OlympusImageHeight"},
	{0x1030, "SceneDetect"},
	{0x1031, "SceneArea"},
	{0x1033, "SceneDetectData"},
	{0x1034, "CompressionRatio"},
	{0x1035, "PreviewImageValid"},
	{0x1036, "PreviewImageStart"},
	{0x1037, "PreviewImageLength"},
	{0x1038, "AFResult"},
	{0x1039, "CCDScanMode"},
	{0x103a, "NoiseReduction"},
	{0x103b, "FocusStepInfinity"},
	{0x103c, "FocusStepNear"},
	{0x103d, "LightValueCenter"},
	{0x103e, "LightValuePeriphery"},
	{0x103f, "FieldCount"},
	{0x2010, "Equipment"},
	{0x2020, "CameraSettings"},
	{0x2030, "RawDevelopment"},
	{0x2031, "RawDev2"},
	{0x2040, "ImageProcessing"},
	{0x2050, "FocusInfo"},
	{0x2100, "Olympus2100"},
	{0x2200, "Olympus2200"},
	{0x2300, "Olympus2300"},
	{0x2400, "Olympus2400"},
	{0x2500, "Olympus2500"},
	{0x2600, "Olympus2600"},
	{0x2700, "Olympus2700"},
	{0x2800, "Olympus2800"},
	{0x2900, "Olympus2900"},
	{0x3000, "RawInfo"},
	{0x4000, "MainInfo"},
	{0x5000, "UnknownInfo"}
};

Media::EXIFData::EXIFInfo Media::EXIFData::olympus2010Infos[] = {
	{0x0000, "EquipmentVersion"},
	{0x0100, "CameraType2"},
	{0x0101, "SerialNumber"},
	{0x0102, "InternalSerialNumber"},
	{0x0103, "FocalPlaneDiagonal"},
	{0x0104, "BodyFirmwareVersion"},
	{0x0201, "LensType"},
	{0x0202, "LensSerialNumber"},
	{0x0203, "LensModel"},
	{0x0204, "LensFirmwareVersion"},
	{0x0205, "MaxApertureAtMinFocal"},
	{0x0206, "MaxApertureAtMaxFocal"},
	{0x0207, "MinFocalLength"},
	{0x0208, "MaxFocalLength"},
	{0x020a, "MaxAperture"},
	{0x020b, "LensProperties"},
	{0x0301, "Extender"},
	{0x0302, "ExtenderSerialNumber"},
	{0x0303, "ExtenderModel"},
	{0x0304, "ExtenderFirmwareVersion"},
	{0x0403, "ConversionLens"},
	{0x1000, "FlashType"},
	{0x1001, "FlashModel"},
	{0x1002, "FlashFirmwareVersion"},
	{0x1003, "FlashSerialNumber"}
};

Media::EXIFData::EXIFInfo Media::EXIFData::olympus2020Infos[] = {
	{0x0000, "CameraSettingsVersion"},
	{0x0100, "PreviewImageValid"},
	{0x0101, "PreviewImageStart"},
	{0x0102, "PreviewImageLength"},
	{0x0200, "ExposureMode"},
	{0x0201, "AELock"},
	{0x0202, "MeteringMode"},
	{0x0203, "ExposureShift"},
	{0x0204, "NDFilter"},
	{0x0300, "MacroMode"},
	{0x0301, "FocusMode"},
	{0x0302, "FocusProcess"},
	{0x0303, "AFSearch"},
	{0x0304, "AFAreas"},
	{0x0305, "AFPointSelected"},
	{0x0306, "AFFineTune"},
	{0x0307, "AFFineTuneAdj"},
	{0x0400, "FlashMode"},
	{0x0401, "FlashExposureComp"},
	{0x0403, "FlashRemoteControl"},
	{0x0404, "FlashControlMode"},
	{0x0405, "FlashIntensity"},
	{0x0406, "ManualFlashStrength"},
	{0x0500, "WhiteBalance2"},
	{0x0501, "WhiteBalanceTemperature"},
	{0x0502, "WhiteBalanceBracket"},
	{0x0503, "CustomSaturation"},
	{0x0504, "ModifiedSaturation"},
	{0x0505, "ContrastSetting"},
	{0x0506, "SharpnessSetting"},
	{0x0507, "ColorSpace"},
	{0x0509, "SceneMode"},
	{0x050a, "NoiseReduction"},
	{0x050b, "DistortionCorrection"},
	{0x050c, "ShadingCompensation"},
	{0x050d, "CompressionFactor"},
	{0x050f, "Gradation"},
	{0x0520, "PictureMode"},
	{0x0521, "PictureModeSaturation"},
	{0x0522, "PictureModeHue"},
	{0x0523, "PictureModeContrast"},
	{0x0524, "PictureModeSharpness"},
	{0x0525, "PictureModeBWFilter"},
	{0x0526, "PictureModeTone"},
	{0x0527, "NoiseFilter"},
	{0x0529, "ArtFilter"},
	{0x052c, "MagicFilter"},
	{0x052d, "PictureModeEffect"},
	{0x052e, "ToneLevel"},
	{0x052f, "ArtFilterEffect"},
	{0x0532, "ColorCreatorEffect"},
	{0x0537, "MonochromeProfileSettings"},
	{0x0538, "FilmGrainEffect"},
	{0x0539, "ColorProfileSettings"},
	{0x053a, "MonochromeVignetting"},
	{0x053b, "MonochromeColor"},
	{0x0600, "DriveMode"},
	{0x0601, "PanoramaMode"},
	{0x0603, "ImageQuality2"},
	{0x0604, "ImageStabilization"},
	{0x0804, "StackedImage"},
	{0x0900, "ManometerPressure"},
	{0x0901, "ManometerReading"},
	{0x0902, "ExtendedWBDetect"},
	{0x0903, "RollAngle"},
	{0x0904, "PitchAngle"},
	{0x0908, "DateTimeUTC"}
};

Media::EXIFData::EXIFInfo Media::EXIFData::olympus2030Infos[] = {
	{0x0000, "RawDevVersion"},
	{0x0100, "RawDevExposureBiasValue"},
	{0x0101, "RawDevWhiteBalanceValue"},
	{0x0102, "RawDevWBFineAdjustment"},
	{0x0103, "RawDevGrayPoint"},
	{0x0104, "RawDevSaturationEmphasis"},
	{0x0105, "RawDevMemoryColorEmphasis"},
	{0x0106, "RawDevContrastValue"},
	{0x0107, "RawDevSharpnessValue"},
	{0x0108, "RawDevColorSpace"},
	{0x0109, "RawDevEngine"},
	{0x010a, "RawDevNoiseReduction"},
	{0x010b, "RawDevEditStatus"},
	{0x010c, "RawDevSettings"}
};

Media::EXIFData::EXIFInfo Media::EXIFData::olympus2040Infos[] = {
	{0x0000, "ImageProcessingVersion"},
	{0x0100, "WB_RBLevels"},
	{0x0102, "WB_RBLevels3000K"},
	{0x0103, "WB_RBLevels3300K"},
	{0x0104, "WB_RBLevels3600K"},
	{0x0105, "WB_RBLevels3900K"},
	{0x0106, "WB_RBLevels4000K"},
	{0x0107, "WB_RBLevels4300K"},
	{0x0108, "WB_RBLevels4500K"},
	{0x0109, "WB_RBLevels4800K"},
	{0x010a, "WB_RBLevels5300K"},
	{0x010b, "WB_RBLevels6000K"},
	{0x010c, "WB_RBLevels6600K"},
	{0x010d, "WB_RBLevels7500K"},
	{0x010e, "WB_RBLevelsCWB1"},
	{0x010f, "WB_RBLevelsCWB2"},
	{0x0110, "WB_RBLevelsCWB3"},
	{0x0111, "WB_RBLevelsCWB4"},
	{0x0113, "WB_GLevel3000K"},
	{0x0114, "WB_GLevel3300K"},
	{0x0115, "WB_GLevel3600K"},
	{0x0116, "WB_GLevel3900K"},
	{0x0117, "WB_GLevel4000K"},
	{0x0118, "WB_GLevel4300K"},
	{0x0119, "WB_GLevel4500K"},
	{0x011a, "WB_GLevel4800K"},
	{0x011b, "WB_GLevel5300K"},
	{0x011c, "WB_GLevel6000K"},
	{0x011d, "WB_GLevel6600K"},
	{0x011e, "WB_GLevel7500K"},
	{0x011f, "WB_GLevel"},
	{0x0200, "ColorMatrix"},
	{0x0300, "Enhancer"},
	{0x0301, "EnhancerValues"},
	{0x0310, "CoringFilter"},
	{0x0311, "CoringValues"},
	{0x0600, "BlackLevel2"},
	{0x0610, "GainBase"},
	{0x0611, "ValidBits"},
	{0x0612, "CropLeft"},
	{0x0613, "CropTop"},
	{0x0614, "CropWidth"},
	{0x0615, "CropHeight"},
	{0x0635, "UnknownBlock1"},
	{0x0636, "UnknownBlock2"},
	{0x0805, "SensorCalibration"},
	{0x1010, "NoiseReduction2"},
	{0x1011, "DistortionCorrection2"},
	{0x1012, "ShadingCompensation2"},
	{0x101c, "MultipleExposureMode"},
	{0x1103, "UnknownBlock3"},
	{0x1104, "UnknownBlock4"},
	{0x1112, "AspectRatio"},
	{0x1113, "AspectFrame"},
	{0x1200, "FacesDetected"},
	{0x1201, "FaceDetectArea"},
	{0x1202, "MaxFaces"},
	{0x1203, "FaceDetectFrameSize"},
	{0x1207, "FaceDetectFrameCrop"},
	{0x1306, "CameraTemperature"},
	{0x1900, "KeystoneCompensation"},
	{0x1901, "KeystoneDirection"},
	{0x1906, "KeystoneValue"}
};

Media::EXIFData::EXIFInfo Media::EXIFData::olympus2050Infos[] = {
	{0x0000, "FocusInfoVersion"},
	{0x0209, "AutoFocus"},
	{0x0210, "SceneDetect"},
	{0x0211, "SceneArea"},
	{0x0212, "SceneDetectData"},
	{0x0300, "ZoomStepCount"},
	{0x0301, "FocusStepCount"},
	{0x0303, "FocusStepInfinity"},
	{0x0304, "FocusStepNear"},
	{0x0305, "FocusDistance"},
	{0x0308, "AFPoint"},
	{0x0328, "AFInfo"},
	{0x1201, "ExternalFlash"},
	{0x1203, "ExternalFlashGuideNumber"},
	{0x1204, "ExternalFlashBounce"},
	{0x1205, "ExternalFlashZoom"},
	{0x1208, "InternalFlash"},
	{0x1209, "ManualFlash"},
	{0x120a, "MacroLED"},
	{0x1500, "SensorTemperature"},
	{0x1600, "ImageStabilization"}
};

Media::EXIFData::EXIFInfo Media::EXIFData::casio1Infos[] = {
	{1, "RecordingMode"},
	{2, "Quality"},
	{3, "FocusingMode"},
	{4, "FlashMode"},
	{5, "FlashIntensity"},
	{6, "ObjectDistance"},
	{7, "WhiteBalance"},
	{10, "DigitalZoom"},
	{11, "Sharpness"},
	{12, "Contract"},
	{13, "Saturation"},
	{20, "CCDSensitivity"}
};

Media::EXIFData::EXIFInfo Media::EXIFData::casio2Infos[] = {
	{2, "PreviewThumbDimension"},
	{3, "PreviewThumbSize"},
	{4, "PreviewThumbOffset"},
	{8, "QualityMode"},
	{9, "ImageSize"},
	{13, "FocusMode"},
	{20, "IsoSensitivity"},
	{25, "WhiteBalance"},
	{29, "FocalLength"},
	{31, "Saturation"},
	{32, "Contrast"},
	{33, "Sharpness"},
	{0x0E00, "PIM"},
	{0x2000, "CasioPreviewThumbnail"},
	{0x2011, "WhiteBalanceBias"},
	{0x2012, "WhiteBalance"},
	{0x2022, "ObjectDistance"},
	{0x2034, "FlashDistance"},
	{0x3000, "RecordMode"},
	{0x3001, "SelfTimer"},
	{0x3002, "Quality"},
	{0x3003, "FocusMode"},
	{0x3006, "TimeZone"},
	{0x3007, "BestshotMode"},
	{0x3014, "CCDISOSensitivity"},
	{0x3015, "ColourMode"},
	{0x3016, "Enhancement"},
	{0x3017, "Filter"}
};

Media::EXIFData::EXIFInfo Media::EXIFData::flirInfos[] = {
	{1, "RTemp"},
	{2, "ATemp"},
	{3, "Emissivity"},
	{4, "IRWTemp"},
	{5, "CameraTemperatureRangeMax"},
	{6, "CameraTemperatureRangeMin"},
	{7, "Unknown"},
	{8, "Unknown"},
	{9, "Unknown"},
	{10, "Unknown"},
	{274, "Unknown"}
};

Int32 Media::EXIFData::TReadInt32(const UInt8 *pVal)
{
	return ReadInt32(pVal);
}

Int32 Media::EXIFData::TReadMInt32(const UInt8 *pVal)
{
	return ReadMInt32(pVal);
}

Int16 Media::EXIFData::TReadInt16(const UInt8 *pVal)
{
	return ReadInt16(pVal);
}

Int16 Media::EXIFData::TReadMInt16(const UInt8 *pVal)
{
	return ReadMInt16(pVal);
}

Single __stdcall Media::EXIFData::TReadFloat16(const UInt8 *pVal)
{
	UInt32 v = ReadUInt16(pVal);
	v = ((v & 0xc000) << 16) | 0x38000000 | ((v & 0x3fff) << 13);
	return *(Single*)&v;
}

Single __stdcall Media::EXIFData::TReadMFloat16(const UInt8 *pVal)
{
	UInt32 v = ReadMUInt16(pVal);
	v = ((v & 0xc000) << 16) | 0x38000000 | ((v & 0x3fff) << 13);
	return *(Single*)&v;
}

Single __stdcall Media::EXIFData::TReadFloat24(const UInt8 *pVal)
{
	UInt32 v = ReadUInt24(pVal);
	v = ((v & 0xc00000) << 8) | 0x20000000 | ((v & 0x3fffff) << 7);
	return *(Single*)&v;
}

Single __stdcall Media::EXIFData::TReadMFloat24(const UInt8 *pVal)
{
	UInt32 v = ReadMUInt24(pVal);
	v = ((v & 0xc00000) << 8) | 0x20000000 | ((v & 0x3fffff) << 7);
	return *(Single*)&v;
}

Single __stdcall Media::EXIFData::TReadFloat32(const UInt8 *pVal)
{
	return ReadFloat(pVal);
}

Single __stdcall Media::EXIFData::TReadMFloat32(const UInt8 *pVal)
{
	return ReadMFloat(pVal);
}

void Media::EXIFData::TWriteInt32(UInt8 *pVal, Int32 v)
{
	WriteInt32(pVal, v);
}

void Media::EXIFData::TWriteMInt32(UInt8 *pVal, Int32 v)
{
	WriteMInt32(pVal, v);
}

void Media::EXIFData::TWriteInt16(UInt8 *pVal, Int32 v)
{
	WriteInt16(pVal, v);
}

void Media::EXIFData::TWriteMInt16(UInt8 *pVal, Int32 v)
{
	WriteMInt16(pVal, v);
}

void Media::EXIFData::FreeItem(Media::EXIFData::EXIFItem *item)
{
	if (item->dataBuff)
	{
		if (item->type == Media::EXIFData::ET_SUBEXIF)
		{
			Media::EXIFData *exif = (Media::EXIFData*)item->dataBuff;
			DEL_CLASS(exif);
		}
		else
		{
			MemFree(item->dataBuff);
		}
	}
	MemFree(item);
}

void Media::EXIFData::ToExifBuff(UInt8 *buff, Data::ArrayList<Media::EXIFData::EXIFItem*> *exifList, UInt32 *startOfst, UInt32 *otherOfst)
{
	UInt32 objCnt;
	UOSInt i;
	UInt32 j;
	UInt32 k;
	EXIFItem *exif;

	objCnt = 0;
	k = *otherOfst;
	j = *startOfst + 2;
	i = 0;
	while (i < exifList->GetCount())
	{
		exif = exifList->GetItem(i);
		if (exif->type == Media::EXIFData::ET_BYTES)
		{
			WriteInt16(&buff[j], exif->id);
			WriteInt16(&buff[j + 2], 1);
			WriteUInt32(&buff[j + 4], exif->size);
			if (exif->size <= 4)
			{
				WriteInt32(&buff[j + 8], 0);
				MemCopyNO(&buff[j + 8], &exif->value, exif->size);
				j += 12;
			}
			else
			{
				WriteUInt32(&buff[j + 8], k);
				MemCopyNO(&buff[k], exif->dataBuff, exif->size);
				k += exif->size;
				j += 12;
			}
			objCnt++;
		}
		else if (exif->type == Media::EXIFData::ET_STRING)
		{
			WriteInt16(&buff[j], exif->id);
			WriteInt16(&buff[j + 2], 2);
			WriteUInt32(&buff[j + 4], exif->size);
			if (exif->size <= 4)
			{
				WriteInt32(&buff[j + 8], 0);
				MemCopyNO(&buff[j + 8], exif->dataBuff, exif->size);
				j += 12;
			}
			else
			{
				WriteUInt32(&buff[j + 8], k);
				MemCopyNO(&buff[k], exif->dataBuff, exif->size);
				k += exif->size;
				j += 12;
			}
			objCnt++;
		}
		else if (exif->type == Media::EXIFData::ET_UINT16)
		{
			WriteInt16(&buff[j], exif->id);
			WriteInt16(&buff[j + 2], 3);
			WriteUInt32(&buff[j + 4], exif->size);
			if (exif->size <= 2)
			{
				WriteInt32(&buff[j + 8], 0);
				MemCopyNO(&buff[j + 8], &exif->value, exif->size << 1);
				j += 12;
			}
			else
			{
				WriteUInt32(&buff[j + 8], k);
				MemCopyNO(&buff[k], exif->dataBuff, exif->size << 1);
				k += exif->size << 1;
				j += 12;
			}
			objCnt++;
		}
		else if (exif->type == Media::EXIFData::ET_UINT32)
		{
			WriteInt16(&buff[j], exif->id);
			WriteInt16(&buff[j + 2], 4);
			WriteUInt32(&buff[j + 4], exif->size);
			if (exif->size <= 1)
			{
				WriteInt32(&buff[j + 8], 0);
				MemCopyNO(&buff[j + 8], &exif->value, exif->size << 2);
				j += 12;
			}
			else
			{
				WriteUInt32(&buff[j + 8], k);
				MemCopyNO(&buff[k], exif->dataBuff, exif->size << 2);
				k += exif->size << 2;
				j += 12;
			}
			objCnt++;
		}
		else if (exif->type == Media::EXIFData::ET_RATIONAL)
		{
			WriteInt16(&buff[j], exif->id);
			WriteInt16(&buff[j + 2], 5);
			WriteUInt32(&buff[j + 4], exif->size);
			WriteUInt32(&buff[j + 8], k);
			MemCopyNO(&buff[k], exif->dataBuff, exif->size << 3);
			k += exif->size << 3;
			j += 12;
			objCnt++;
		}
		else if (exif->type == Media::EXIFData::ET_OTHER)
		{
			WriteInt16(&buff[j], exif->id);
			WriteInt16(&buff[j + 2], 7);
			WriteUInt32(&buff[j + 4], exif->size);
			if (exif->size <= 4)
			{
				WriteInt32(&buff[j + 8], 0);
				MemCopyNO(&buff[j + 8], exif->dataBuff, exif->size);
				j += 12;
			}
			else
			{
				WriteUInt32(&buff[j + 8], k);
				MemCopyNO(&buff[k], exif->dataBuff, exif->size);
				k += exif->size;
				j += 12;
			}
			objCnt++;
		}
		else if (exif->type == Media::EXIFData::ET_INT16)
		{
			WriteInt16(&buff[j], exif->id);
			WriteInt16(&buff[j + 2], 8);
			WriteUInt32(&buff[j + 4], exif->size);
			if (exif->size <= 2)
			{
				WriteInt32(&buff[j + 8], 0);
				MemCopyNO(&buff[j + 8], &exif->value, exif->size << 1);
				j += 12;
			}
			else
			{
				WriteUInt32(&buff[j + 8], k);
				MemCopyNO(&buff[k], exif->dataBuff, exif->size << 1);
				k += exif->size << 1;
				j += 12;
			}
			objCnt++;
		}
		else if (exif->type == Media::EXIFData::ET_SUBEXIF)
		{
			WriteInt16(&buff[j], exif->id);
			WriteInt16(&buff[j + 2], 4);
			WriteInt32(&buff[j + 4], 1);
			exif->value = (Int32)j + 8;
			j += 12;
			objCnt++;
		}
		else if (exif->type == Media::EXIFData::ET_DOUBLE)
		{
			WriteInt16(&buff[j], exif->id);
			WriteInt16(&buff[j + 2], 12);
			WriteUInt32(&buff[j + 4], exif->size);
			WriteUInt32(&buff[j + 8], k);
			MemCopyNO(&buff[k], exif->dataBuff, exif->size << 3);
			k += exif->size << 3;
			j += 12;
			objCnt++;
		}
		i++;
	}
	WriteInt32(&buff[j], 0);
	WriteInt16(&buff[*startOfst], objCnt);
	j += 4;

	i = 0;
	while (i < exifList->GetCount())
	{
		exif = exifList->GetItem(i);
		if (exif->type == Media::EXIFData::ET_SUBEXIF)
		{
			WriteUInt32(&buff[exif->value], j);
			((Media::EXIFData*)exif->dataBuff)->ToExifBuff(buff, &j, &k);
		}
		i++;
	}
	*startOfst = j;
	*otherOfst = k;
}

void Media::EXIFData::GetExifBuffSize(Data::ArrayList<EXIFItem*> *exifList, UInt32 *size, UInt32 *endOfst)
{
	UInt32 i = 6;
	UInt32 j = 6;
	UOSInt k;
	UInt32 l;
	UInt32 m;
	EXIFItem *exif;

	k = exifList->GetCount();
	while (k-- > 0)
	{
		exif = exifList->GetItem(k);
		if (exif->type == Media::EXIFData::ET_BYTES)
		{
			i += 12;
			if (exif->size <= 4)
			{
				j += 12;
			}
			else
			{
				j += 12 + exif->size;
			}
		}
		else if (exif->type == Media::EXIFData::ET_STRING)
		{
			i += 12;
			if (exif->size <= 4)
				j += 12;
			else
				j += 12 + exif->size;
		}
		else if (exif->type == Media::EXIFData::ET_UINT16)
		{
			i += 12;
			if (exif->size <= 2)
				j += 12;
			else
				j += 12 + (exif->size << 1);
		}
		else if (exif->type == Media::EXIFData::ET_UINT32)
		{
			i += 12;
			if (exif->size <= 1)
				j += 12;
			else
				j += 12 + (exif->size << 2);
		}
		else if (exif->type == Media::EXIFData::ET_RATIONAL)
		{
			i += 12;
			j += 12 + (exif->size << 3);
		}
		else if (exif->type == Media::EXIFData::ET_OTHER)
		{
			i += 12;
			if (exif->size <= 4)
				j += 12;
			else
				j += 12 + exif->size;
		}
		else if (exif->type == Media::EXIFData::ET_INT16)
		{
			i += 12;
			if (exif->size <= 2)
				j += 12;
			else
				j += 12 + (exif->size << 1);
		}
		else if (exif->type == Media::EXIFData::ET_SUBEXIF)
		{
			i += 12;
			j += 12;
			((Media::EXIFData*)exif->dataBuff)->GetExifBuffSize(&l, &m);
			i += m;
			j += l;
		}
		else if (exif->type == Media::EXIFData::ET_DOUBLE)
		{
			i += 12;
			j += 12 + (exif->size << 3);
		}
	}
	*size = j;
	*endOfst = i;
}

Media::EXIFData::EXIFData(EXIFMaker exifMaker)
{
	this->exifMaker = exifMaker;
	NEW_CLASS(this->exifMap, Data::Int32Map<EXIFItem*>());
}

Media::EXIFData::~EXIFData()
{
	Data::ArrayList<EXIFItem*> *items = this->exifMap->GetValues();
	UOSInt i = items->GetCount();
	while (i-- > 0)
	{
		FreeItem(items->GetItem(i));
	}
	DEL_CLASS(this->exifMap);
}

Media::EXIFData::EXIFMaker Media::EXIFData::GetEXIFMaker()
{
	return this->exifMaker;
}

Media::EXIFData *Media::EXIFData::Clone()
{
	Media::EXIFData::EXIFItem *item;
	Data::ArrayList<EXIFItem*> *items = this->exifMap->GetValues();
	UOSInt i;
	UOSInt j;
	Media::EXIFData *newExif;
	NEW_CLASS(newExif, Media::EXIFData(this->exifMaker));
	i = 0;
	j = items->GetCount();
	while (i < j)
	{
		item = items->GetItem(i);
		switch (item->type)
		{
		case ET_BYTES:
			if (item->size <= 4)
			{
				newExif->AddBytes(item->id, item->size, (UInt8*)&item->value);
			}
			else
			{
				newExif->AddBytes(item->id, item->size, (UInt8*)item->dataBuff);
			}
			break;
		case ET_STRING:
			newExif->AddString(item->id, item->size, (Char*)item->dataBuff);
			break;
		case ET_UINT16:
			if (item->size <= 2)
			{
				newExif->AddUInt16(item->id, item->size, (UInt16*)&item->value);
			}
			else
			{
				newExif->AddUInt16(item->id, item->size, (UInt16*)item->dataBuff);
			}
			break;
		case ET_UINT32:
			if (item->size == 1)
			{
				newExif->AddUInt32(item->id, item->size, (UInt32*)&item->value);
			}
			else
			{
				newExif->AddUInt32(item->id, item->size, (UInt32*)item->dataBuff);
			}
			break;
		case ET_RATIONAL:
			newExif->AddRational(item->id, item->size, (UInt32*)item->dataBuff);
			break;
		case ET_OTHER:
			newExif->AddOther(item->id, item->size, (UInt8*)item->dataBuff);
			break;
		case ET_INT16:
			if (item->size <= 2)
			{
				newExif->AddInt16(item->id, item->size, (Int16*)&item->value);
			}
			else
			{
				newExif->AddInt16(item->id, item->size, (Int16*)item->dataBuff);
			}
			break;
		case ET_SUBEXIF:
			newExif->AddSubEXIF(item->id, ((Media::EXIFData*)item->dataBuff)->Clone());
			break;
		case ET_DOUBLE:
			newExif->AddDouble(item->id, item->size, (Double*)item->dataBuff);
			break;
		case ET_UNKNOWN:
		default:
			break;
		}
		i++;
	}
	return newExif;
}

void Media::EXIFData::AddBytes(Int32 id, UInt32 cnt, const UInt8 *buff)
{
	EXIFItem *item = MemAlloc(EXIFItem, 1);
	item->id = id;
	item->type = ET_BYTES;
	item->size = cnt;
	if (cnt <= 4)
	{
		MemCopyNO(&item->value, buff, cnt);
		item->dataBuff = 0;
	}
	else
	{
		item->value = *(Int32*)buff;
		item->dataBuff = MemAlloc(UInt8, cnt);
		MemCopyNO(item->dataBuff, buff, cnt);
	}
	item = this->exifMap->Put(id, item);
	if (item)
	{
		FreeItem(item);
	}
}

void Media::EXIFData::AddString(Int32 id, UInt32 cnt, const Char *buff)
{
	EXIFItem *item = MemAlloc(EXIFItem, 1);
	item->id = id;
	item->type = ET_STRING;
	item->size = cnt;
	item->value = 0;
	item->dataBuff = MemAlloc(UInt8, cnt);
	MemCopyNO(item->dataBuff, buff, cnt);
	item = this->exifMap->Put(id, item);
	if (item)
	{
		FreeItem(item);
	}
}

void Media::EXIFData::AddUInt16(Int32 id, UInt32 cnt, const UInt16 *buff)
{
	EXIFItem *item = MemAlloc(EXIFItem, 1);
	item->id = id;
	item->type = ET_UINT16;
	item->size = cnt;
	if (cnt <= 2)
	{
		MemCopyNO(&item->value, buff, cnt * sizeof(UInt16));
		item->dataBuff = 0;
	}
	else
	{
		item->value = *(Int32*)buff;
		item->dataBuff = MemAlloc(UInt16, cnt);
		MemCopyNO(item->dataBuff, buff, cnt * sizeof(UInt16));
	}
	item = this->exifMap->Put(id, item);
	if (item)
	{
		FreeItem(item);
	}
}

void Media::EXIFData::AddUInt32(Int32 id, UInt32 cnt, const UInt32 *buff)
{
	EXIFItem *item = MemAlloc(EXIFItem, 1);
	item->id = id;
	item->type = ET_UINT32;
	item->size = cnt;
	if (cnt == 1)
	{
		item->value = (Int32)*buff;
		item->dataBuff = 0;
	}
	else
	{
		item->value = (Int32)*buff;
		item->dataBuff = MemAlloc(UInt32, cnt);
		MemCopyNO(item->dataBuff, buff, cnt * sizeof(UInt32));
	}
	item = this->exifMap->Put(id, item);
	if (item)
	{
		FreeItem(item);
	}
}

void Media::EXIFData::AddRational(Int32 id, UInt32 cnt, const UInt32 *buff)
{
	EXIFItem *item = MemAlloc(EXIFItem, 1);
	item->id = id;
	item->type = ET_RATIONAL;
	item->size = cnt;
	item->value = 0;
	item->dataBuff = MemAlloc(UInt32, cnt << 1);
	MemCopyNO(item->dataBuff, buff, cnt * sizeof(UInt32) * 2);

	item = this->exifMap->Put(id, item);
	if (item)
	{
		FreeItem(item);
	}
}

void Media::EXIFData::AddOther(Int32 id, UInt32 cnt, const UInt8 *buff)
{
	EXIFItem *item = MemAlloc(EXIFItem, 1);
	item->id = id;
	item->type = ET_OTHER;
	item->size = cnt;
	item->value = 0;
	item->dataBuff = MemAlloc(UInt8, cnt);
	MemCopyNO(item->dataBuff, buff, cnt);

	item = this->exifMap->Put(id, item);
	if (item)
	{
		FreeItem(item);
	}
}

void Media::EXIFData::AddInt16(Int32 id, UInt32 cnt, const Int16 *buff)
{
	EXIFItem *item = MemAlloc(EXIFItem, 1);
	item->id = id;
	item->type = ET_INT16;
	item->size = cnt;
	if (cnt <= 2)
	{
		MemCopyNO(&item->value, buff, cnt * sizeof(Int16));
		item->dataBuff = 0;
	}
	else
	{
		item->value = *(Int32*)buff;
		item->dataBuff = MemAlloc(Int16, cnt);
		MemCopyNO(item->dataBuff, buff, cnt * sizeof(Int16));
	}
	item = this->exifMap->Put(id, item);
	if (item)
	{
		FreeItem(item);
	}
}

void Media::EXIFData::AddSubEXIF(Int32 id, Media::EXIFData *exif)
{
	EXIFItem *item = MemAlloc(EXIFItem, 1);
	item->id = id;
	item->type = ET_SUBEXIF;
	item->size = 1;
	item->value = 0;
	item->dataBuff = exif;
	item = this->exifMap->Put(id, item);
	if (item)
	{
		FreeItem(item);
	}
}

void Media::EXIFData::AddDouble(Int32 id, UInt32 cnt, const Double *buff)
{
	EXIFItem *item = MemAlloc(EXIFItem, 1);
	item->id = id;
	item->type = ET_DOUBLE;
	item->size = cnt;
	item->value = 0;
	item->dataBuff = MemAlloc(Double, cnt);
	MemCopyNO(item->dataBuff, buff, cnt * sizeof(Double));
	item = this->exifMap->Put(id, item);
	if (item)
	{
		FreeItem(item);
	}
}

void Media::EXIFData::Remove(Int32 id)
{
	EXIFItem *item;
	item = this->exifMap->Remove(id);
	if (item)
	{
		FreeItem(item);
	}
}

UOSInt Media::EXIFData::GetExifIds(Data::ArrayList<Int32> *idArr)
{
	Data::ArrayList<EXIFItem*> *items = this->exifMap->GetValues();
	UOSInt cnt = items->GetCount();
	UOSInt i = 0;
	while (i < cnt)
	{
		idArr->Add(items->GetItem(i)->id);
		i++;
	}
	return cnt;
}

Media::EXIFData::EXIFType Media::EXIFData::GetExifType(Int32 id)
{
	Media::EXIFData::EXIFItem *item = this->exifMap->Get(id);
	if (item == 0)
		return ET_UNKNOWN;
	return item->type;
}

UInt32 Media::EXIFData::GetExifCount(Int32 id)
{
	Media::EXIFData::EXIFItem *item = this->exifMap->Get(id);
	if (item == 0)
		return 0;
	return item->size;
}

Media::EXIFData::EXIFItem *Media::EXIFData::GetExifItem(Int32 id)
{
	return this->exifMap->Get(id);
}

UInt16 *Media::EXIFData::GetExifUInt16(Int32 id)
{
	Media::EXIFData::EXIFItem *item = this->exifMap->Get(id);
	if (item == 0)
		return 0;
	if (item->type != ET_UINT16)
		return 0;
	if (item->size > 2)
	{
		return (UInt16*)item->dataBuff;
	}
	else
	{
		return (UInt16*)&item->value;
	}
}

UInt32 *Media::EXIFData::GetExifUInt32(Int32 id)
{
	Media::EXIFData::EXIFItem *item = this->exifMap->Get(id);
	if (item == 0)
		return 0;
	if (item->type != ET_UINT32)
		return 0;
	if (item->size > 1)
	{
		return (UInt32*)item->dataBuff;
	}
	else
	{
		return (UInt32*)&item->value;
	}
}

Media::EXIFData *Media::EXIFData::GetExifSubexif(Int32 id)
{
	Media::EXIFData::EXIFItem *item = this->exifMap->Get(id);
	if (item == 0)
		return 0;
	if (item->type != ET_SUBEXIF)
		return 0;
	return (Media::EXIFData*)item->dataBuff;
}

UInt8 *Media::EXIFData::GetExifOther(Int32 id)
{
	Media::EXIFData::EXIFItem *item = this->exifMap->Get(id);
	if (item == 0)
		return 0;
	if (item->type != ET_OTHER)
		return 0;
	return (UInt8*)item->dataBuff;
}

Bool Media::EXIFData::GetPhotoDate(Data::DateTime *dt)
{
	Media::EXIFData::EXIFItem *item;
	if (this->exifMaker == EM_STANDARD)
	{
		if ((item = this->exifMap->Get(36867)) != 0)
		{
			if (item->type == ET_STRING)
			{
				dt->SetValue((Char*)item->dataBuff);
				return true;
			}
		}
		if ((item = this->exifMap->Get(36868)) != 0)
		{
			if (item->type == ET_STRING)
			{
				dt->SetValue((Char*)item->dataBuff);
				return true;
			}
		}
		if ((item = this->exifMap->Get(34665)) != 0)
		{
			if (item->type == ET_SUBEXIF)
			{
				Media::EXIFData *exif = (Media::EXIFData*)item->dataBuff;
				if (exif->GetPhotoDate(dt))
					return true;
			}
		}
		if ((item = this->exifMap->Get(306)) != 0)
		{
			if (item->type == ET_STRING)
			{
				dt->SetValue((Char*)item->dataBuff);
				return true;
			}
		}
	}
	return false;
}

const Char *Media::EXIFData::GetPhotoMake()
{
	Media::EXIFData::EXIFItem *item;
	if (this->exifMaker == EM_STANDARD)
	{
		if ((item = this->exifMap->Get(271)) != 0)
		{
			if (item->type == ET_STRING)
			{
				return (const Char*)item->dataBuff;
			}
		}
	}
	return 0;
}

const Char *Media::EXIFData::GetPhotoModel()
{
	Media::EXIFData::EXIFItem *item;
	if (this->exifMaker == EM_STANDARD)
	{
		if ((item = this->exifMap->Get(272)) != 0)
		{
			if (item->type == ET_STRING)
			{
				return (const Char*)item->dataBuff;
			}
		}
	}
	if (this->exifMaker == EM_CANON)
	{
		if ((item = this->exifMap->Get(6)) != 0)
		{
			if (item->type == ET_STRING)
			{
				return (const Char*)item->dataBuff;
			}
		}
	}
	return 0;
}

const Char *Media::EXIFData::GetPhotoLens()
{
	Media::EXIFData::EXIFItem *item;
	if (this->exifMaker == EM_CANON)
	{
		if ((item = this->exifMap->Get(149)) != 0)
		{
			if (item->type == ET_STRING)
			{
				return (const Char*)item->dataBuff;
			}
		}
	}
	if (this->exifMaker == EM_PANASONIC)
	{
		if ((item = this->exifMap->Get(81)) != 0)
		{
			if (item->type == ET_STRING)
			{
				return (const Char*)item->dataBuff;
			}
		}
	}
	return 0;
}

Double Media::EXIFData::GetPhotoFNumber()
{
	Media::EXIFData::EXIFItem *item;
	if (this->exifMaker == EM_STANDARD)
	{
		if ((item = this->exifMap->Get(33437)) != 0)
		{
			if (item->type == ET_RATIONAL && item->size == 1)
			{
				UInt32 *dataBuff = (UInt32*)item->dataBuff;
				return dataBuff[0] / (Double)dataBuff[1];
			}
		}
		if ((item = this->exifMap->Get(34665)) != 0)
		{
			if (item->type == ET_SUBEXIF)
			{
				Media::EXIFData *exif = (Media::EXIFData*)item->dataBuff;
				return exif->GetPhotoFNumber();
			}
		}
	}
	return 0;
}

Double Media::EXIFData::GetPhotoExpTime()
{
	Media::EXIFData::EXIFItem *item;
	if (this->exifMaker == EM_STANDARD)
	{
		if ((item = this->exifMap->Get(33434)) != 0)
		{
			if (item->type == ET_RATIONAL && item->size == 1)
			{
				UInt32 *dataBuff = (UInt32*)item->dataBuff;
				return dataBuff[0] / (Double)dataBuff[1];
			}
		}
		if ((item = this->exifMap->Get(34665)) != 0)
		{
			if (item->type == ET_SUBEXIF)
			{
				Media::EXIFData *exif = (Media::EXIFData*)item->dataBuff;
				return exif->GetPhotoExpTime();
			}
		}
	}
	return 0;
}

UInt32 Media::EXIFData::GetPhotoISO()
{
	Media::EXIFData::EXIFItem *item;
	if (this->exifMaker == EM_STANDARD)
	{
		if ((item = this->exifMap->Get(34855)) != 0)
		{
			if (item->type == ET_UINT16 && item->size == 1)
			{
				return *(UInt16*)&item->value;
			}
			else if (item->type == ET_UINT32 && item->size == 1)
			{
				return *(UInt32*)&item->value;
			}
		}
		if ((item = this->exifMap->Get(34665)) != 0)
		{
			if (item->type == ET_SUBEXIF)
			{
				Media::EXIFData *exif = (Media::EXIFData*)item->dataBuff;
				return exif->GetPhotoISO();
			}
		}
	}
	return 0;
}

Double Media::EXIFData::GetPhotoFocalLength()
{
	Media::EXIFData::EXIFItem *item;
	if (this->exifMaker == EM_STANDARD)
	{
		if ((item = this->exifMap->Get(37386)) != 0)
		{
			if (item->type == ET_RATIONAL && item->size == 1)
			{
				UInt32 *dataBuff = (UInt32*)item->dataBuff;
				return dataBuff[0] / (Double)dataBuff[1];
			}
		}
		if ((item = this->exifMap->Get(34665)) != 0)
		{
			if (item->type == ET_SUBEXIF)
			{
				Media::EXIFData *exif = (Media::EXIFData*)item->dataBuff;
				return exif->GetPhotoFocalLength();
			}
		}
	}
	return 0;
}

Bool Media::EXIFData::GetPhotoLocation(Double *lat, Double *lon, Double *altitude, Int64 *gpsTimeTick)
{
	Media::EXIFData *subExif = this->GetExifSubexif(34853);
	if (subExif)
	{
		Bool succ = true;
/*
	{1, L"GPSLatitudeRef"},
	{2, L"GPSLatitude"},
	{3, L"GPSLongitudeRef"},
	{4, L"GPSLongitude"},
	{5, L"GPSAltitudeRef"},
	{6, L"GPSAltitude"},
	{7, L"GPSTimeStamp"},*/
		Media::EXIFData::EXIFItem *item1;
		Media::EXIFData::EXIFItem *item2;
		Double val = 0;
		item1 = subExif->GetExifItem(1);
		item2 = subExif->GetExifItem(2);
		if (item1 && item2)
		{
			if (item2->type == Media::EXIFData::ET_RATIONAL)
			{
				if (item2->size == 3)
				{
					val = ReadInt32(&((UInt8*)item2->dataBuff)[0]) / (Double)ReadInt32(&((UInt8*)item2->dataBuff)[4]);
					val += ReadInt32(&((UInt8*)item2->dataBuff)[8]) / (Double)ReadInt32(&((UInt8*)item2->dataBuff)[12]) / 60.0;
					val += ReadInt32(&((UInt8*)item2->dataBuff)[16]) / (Double)ReadInt32(&((UInt8*)item2->dataBuff)[20]) / 3600.0;
				}
				else if (item2->size == 1)
				{
					val = ReadInt32(&((UInt8*)item2->dataBuff)[0]) / (Double)ReadInt32(&((UInt8*)item2->dataBuff)[4]);
				}
				else
				{
					succ = false;
				}
			}
			else
			{
				succ = false;
			}
			if (item1->type == Media::EXIFData::ET_STRING)
			{
				if (Text::StrEquals((Char*)item1->dataBuff, "S"))
				{
					val = -val;
				}
			}
			else
			{
				succ = false;
			}
			if (lat)
			{
				*lat = val;
			}
		}
		else
		{
			succ = false;
		}
		item1 = subExif->GetExifItem(3);
		item2 = subExif->GetExifItem(4);
		if (item1 && item2)
		{
			if (item2->type == Media::EXIFData::ET_RATIONAL)
			{
				if (item2->size == 3)
				{
					val = ReadInt32(&((UInt8*)item2->dataBuff)[0]) / (Double)ReadInt32(&((UInt8*)item2->dataBuff)[4]);
					val += ReadInt32(&((UInt8*)item2->dataBuff)[8]) / (Double)ReadInt32(&((UInt8*)item2->dataBuff)[12]) / 60.0;
					val += ReadInt32(&((UInt8*)item2->dataBuff)[16]) / (Double)ReadInt32(&((UInt8*)item2->dataBuff)[20]) / 3600.0;
				}
				else if (item2->size == 1)
				{
					val = ReadInt32(&((UInt8*)item2->dataBuff)[0]) / (Double)ReadInt32(&((UInt8*)item2->dataBuff)[4]);
				}
				else
				{
					succ = false;
				}
			}
			else
			{
				succ = false;
			}
			if (item1->type == Media::EXIFData::ET_STRING)
			{
				if (Text::StrEquals((Char*)item1->dataBuff, "W"))
				{
					val = -val;
				}
			}
			else
			{
				succ = false;
			}
			if (lon)
			{
				*lon = val;
			}
		}
		else
		{
			succ = false;
		}
		item1 = subExif->GetExifItem(5);
		item2 = subExif->GetExifItem(6);
		if (item1 && item2)
		{
			if (item2->type == Media::EXIFData::ET_RATIONAL)
			{
				if (item2->size == 1)
				{
					val = ReadInt32(&((UInt8*)item2->dataBuff)[0]) / (Double)ReadInt32(&((UInt8*)item2->dataBuff)[4]);
				}
				else
				{
					succ = false;
				}
			}
			else
			{
				succ = false;
			}
			if (item1->type == Media::EXIFData::ET_BYTES && item1->size == 1)
			{
				if ((item1->value & 0xff) == 1)
				{
					val = -val;
				}
			}
			else
			{
				succ = false;
			}
			if (altitude)
			{
				*altitude = val;
			}
		}
		else
		{
			succ = false;
		}
		item1 = subExif->GetExifItem(7);
		item2 = subExif->GetExifItem(29);
		if (item1 && item2)
		{
			Int32 hh = 0;
			Int32 mm = 0;
			Int32 ss = 0;
			Int32 ms = 0;

			if (item1->type == Media::EXIFData::ET_RATIONAL && item1->size == 3)
			{
				if (ReadInt32(&((UInt8*)item1->dataBuff)[4]) != 1 || ReadInt32(&((UInt8*)item1->dataBuff)[12]) != 1)
				{
					succ = false;
				}
				else
				{
					hh = ReadInt32(&((UInt8*)item1->dataBuff)[0]);
					mm = ReadInt32(&((UInt8*)item1->dataBuff)[8]);
					val = ReadInt32(&((UInt8*)item1->dataBuff)[16]) / (Double)ReadInt32(&((UInt8*)item1->dataBuff)[20]);
					ss = (Int32)val;
					ms = (Int32)((val - ss) * 1000);
				}
			}
			else
			{
				succ = false;
			}
			if (item2->type == Media::EXIFData::ET_STRING && item2->size == 11)
			{
				Char dateStr[12];
				UOSInt dateCnt;
				Char *dateArr[3];
				MemCopyNO(dateStr, item2->dataBuff, 11);
				dateStr[11] = 0;
				dateCnt = Text::StrSplit(dateArr, 3, dateStr, ':');
				if (dateCnt != 3)
				{
					succ = false;
				}
				else if (gpsTimeTick)
				{
					Data::DateTime dt;
					dt.SetValue((UInt16)Text::StrToUInt32(dateArr[0]), Text::StrToInt32(dateArr[1]), Text::StrToInt32(dateArr[2]), hh, mm, ss, ms);
					*gpsTimeTick = dt.ToTicks();
				}
			}
			else
			{
				succ = false;
			}
		}
		else
		{
			succ = false;
		}
		return succ;
	}
	return false;
}

Bool Media::EXIFData::GetGeoBounds(UOSInt imgW, UOSInt imgH, UInt32 *srid, Double *minX, Double *minY, Double *maxX, Double *maxY)
{
	Media::EXIFData::EXIFItem *item;
	Media::EXIFData::EXIFItem *item2;
	item = this->exifMap->Get(33922);
	item2 = this->exifMap->Get(33550);
	if (item == 0 || item2 == 0)
		return false;

	if (srid)
	{
		*srid = 0;
	}
//	Math::CoordinateSystem *coord = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::GeographicCoordinateSystem::GCST_WGS84);
	Double *ptr = (Double*)item->dataBuff;
	Double imgX = ptr[0];
	Double imgY = ptr[1];
	Double mapX = ptr[3];
	Double mapY = ptr[4];
	Double mppX;
	Double mppY;

	ptr = (Double*)item2->dataBuff;
	mppX = ptr[0];
	mppY = ptr[1];

/*	*minX = coord->CalLonByDist(mapY, mapX, -imgX * mppX);
	*maxY = coord->CalLatByDist(mapY, imgY * mppY);
	*maxX = coord->CalLonByDist(mapY, mapX, (imgW - imgX) * mppX);
	*minY = coord->CalLatByDist(mapY, (imgH - imgY) * mppY);*/
	*minX = mapX - imgX * mppX;
	*maxY = mapY + imgY * mppY;
	*maxX = mapX + (UOSInt2Double(imgW) - imgX) * mppX;
	*minY = mapY - (UOSInt2Double(imgH) - imgY) * mppY;

//	DEL_CLASS(coord);
	return true;
}

Media::EXIFData::RotateType Media::EXIFData::GetRotateType()
{
	Media::EXIFData::EXIFItem *item;
	item = this->exifMap->Get(274);
	if (item == 0)
	{
		return Media::EXIFData::RT_NONE;
	}
	UInt32 v;
	if (item->type == Media::EXIFData::ET_UINT16)
	{
		v = item->value & 0xffff;
	}
	else
	{
		v = (UInt32)item->value;
	}
	if (v == 6)
	{
		return Media::EXIFData::RT_CW90;
	}
	else if (v == 3)
	{
		return Media::EXIFData::RT_CW180;
	}
	else if (v == 8)
	{
		return Media::EXIFData::RT_CW270;
	}
	return Media::EXIFData::RT_NONE;
}

Double Media::EXIFData::GetHDPI()
{
	Media::EXIFData::EXIFItem *item;
	item = this->exifMap->Get(282);
	if (item == 0)
	{
		return 0;
	}
	if (item->type != Media::EXIFData::ET_RATIONAL)
	{
		return 0;
	}
	Int32 *arr = (Int32*)item->dataBuff;
	return arr[0] / (Double)arr[1];
}

Double Media::EXIFData::GetVDPI()
{
	Media::EXIFData::EXIFItem *item;
	item = this->exifMap->Get(283);
	if (item == 0)
	{
		return 0;
	}
	if (item->type != Media::EXIFData::ET_RATIONAL)
	{
		return 0;
	}
	Int32 *arr = (Int32*)item->dataBuff;
	return arr[0] / (Double)arr[1];
}

void Media::EXIFData::SetWidth(UInt32 width)
{
	this->AddUInt32(256, 1, &width);

	Media::EXIFData::EXIFItem *item;
	item = this->exifMap->Get(34665);
	if (item && item->type == Media::EXIFData::ET_SUBEXIF)
	{
		((EXIFData*)item->dataBuff)->AddUInt32(40962, 1, &width);
	}
}

void Media::EXIFData::SetHeight(UInt32 height)
{
	this->AddUInt32(257, 1, &height);

	Media::EXIFData::EXIFItem *item;
	item = this->exifMap->Get(34665);
	if (item && item->type == Media::EXIFData::ET_SUBEXIF)
	{
		((EXIFData*)item->dataBuff)->AddUInt32(40963, 1, &height);
	}
}

Bool Media::EXIFData::ToString(Text::StringBuilderUTF8 *sb, const UTF8Char *linePrefix)
{
	Data::ArrayList<Int32> exifIds;
	Media::EXIFData::EXIFItem *exItem;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Int32 v;

	sb->AppendC(UTF8STRC("EXIF Content:"));
	this->GetExifIds(&exifIds);
	i = 0;
	j = exifIds.GetCount();
	while (i < j)
	{
		v = exifIds.GetItem(i);
		sb->AppendC(UTF8STRC("\r\n"));
		if (linePrefix)
			sb->Append(linePrefix);
		sb->AppendC(UTF8STRC("Id = "));
		sb->AppendI32(v);
		sb->AppendC(UTF8STRC(", name = "));
		sb->Append(Media::EXIFData::GetEXIFName(this->exifMaker, v));
		exItem = this->GetExifItem(v);
		if (exItem->type == Media::EXIFData::ET_SUBEXIF)
		{
			Data::ArrayList<Int32> subExIds;
			UOSInt i2;
			UOSInt j2;
			Int32 v2;
			Media::EXIFData::EXIFItem *subExItem;
			Media::EXIFData *subExif = (Media::EXIFData*)exItem->dataBuff;
			i2 = 0;
			j2 = subExif->GetExifIds(&subExIds);
			while (i2 < j2)
			{
				v2 = subExIds.GetItem(i2);
				sb->AppendC(UTF8STRC("\r\n"));
				if (linePrefix)
					sb->Append(linePrefix);
				sb->AppendC(UTF8STRC(" Subid = "));
				sb->AppendI32(v2);
				sb->AppendC(UTF8STRC(", name = "));
				sb->Append(Media::EXIFData::GetEXIFName(this->exifMaker, v, v2));

				subExItem = subExif->GetExifItem(v2);
				if (subExItem->type == Media::EXIFData::ET_STRING)
				{
					sb->AppendC(UTF8STRC(", value = "));
					if (((Char*)subExItem->dataBuff)[subExItem->size - 1])
					{
						sb->AppendC((UTF8Char*)subExItem->dataBuff, subExItem->size);
					}
					else
					{
						sb->Append((UTF8Char*)subExItem->dataBuff);
					}
				}
				else if (subExItem->type == Media::EXIFData::ET_DOUBLE)
				{
					Double *valBuff = (Double*)subExItem->dataBuff;
					k = 0;
					while (k < subExItem->size)
					{
						if (k == 0)
						{
							sb->AppendC(UTF8STRC(", value = "));
						}
						else
						{
							sb->AppendC(UTF8STRC(", "));
						}
						Text::SBAppendF64(sb, valBuff[k]);
						k++;
					}
				}
				else if (subExItem->type == Media::EXIFData::ET_BYTES)
				{
					UInt8 *valBuff;
					if (subExItem->size <= 4)
					{
						valBuff = (UInt8*)&subExItem->value;
					}
					else
					{
						valBuff = (UInt8*)subExItem->dataBuff;
					}
					sb->AppendC(UTF8STRC(", value = "));
					if (subExItem->size > 1024)
					{
						sb->AppendU32(subExItem->size);
						sb->AppendC(UTF8STRC(" bytes: "));
						sb->AppendHexBuff(valBuff, 256, ' ', Text::LineBreakType::CRLF);
						sb->AppendC(UTF8STRC("\r\n...\r\n"));
						sb->AppendHexBuff(&valBuff[((Int32)subExItem->size & ~15) - 256], 256 + (subExItem->size & 15), ' ', Text::LineBreakType::CRLF);
					}
					else
					{
						sb->AppendHexBuff(valBuff, subExItem->size, ' ', Text::LineBreakType::CRLF);
					}
				}
				else if (subExItem->type == Media::EXIFData::ET_UINT16)
				{
					UInt16 *valBuff;
					if (subExItem->size <= 2)
					{
						valBuff = (UInt16*)&subExItem->value;
					}
					else
					{
						valBuff = (UInt16*)subExItem->dataBuff;
					}
					k = 0;
					while (k < subExItem->size)
					{
						if (k == 0)
						{
							sb->AppendC(UTF8STRC(", value = "));
						}
						else
						{
							sb->AppendC(UTF8STRC(", "));
						}
						sb->AppendU16(valBuff[k]);
						k++;
					}
				}
				else if (subExItem->type == Media::EXIFData::ET_UINT32)
				{
					UInt32 *valBuff;
					if (subExItem->size <= 1)
					{
						valBuff = (UInt32*)&subExItem->value;
					}
					else
					{
						valBuff = (UInt32*)subExItem->dataBuff;
					}
					k = 0;
					while (k < subExItem->size)
					{
						if (k == 0)
						{
							sb->AppendC(UTF8STRC(", value = "));
						}
						else
						{
							sb->AppendC(UTF8STRC(", "));
						}
						sb->AppendU32(valBuff[k]);
						k++;
					}
				}
				else if (subExItem->type == Media::EXIFData::ET_RATIONAL)
				{
					UInt32 *valBuff;
					valBuff = (UInt32*)subExItem->dataBuff;
					k = 0;
					while (k < subExItem->size)
					{
						if (k == 0)
						{
							sb->AppendC(UTF8STRC(", value = "));
						}
						else
						{
							sb->AppendC(UTF8STRC(", "));
						}
						sb->AppendU32(valBuff[k * 2]);
						sb->AppendC(UTF8STRC(" / "));
						sb->AppendU32(valBuff[k * 2 + 1]);
						if (valBuff[k * 2 + 1] != 0)
						{
							sb->AppendC(UTF8STRC(" ("));
							Text::SBAppendF64(sb, valBuff[k * 2] / (Double)valBuff[k * 2 + 1]);
							sb->AppendC(UTF8STRC(")"));
						}
						k++;
					}
				}
				else if (subExItem->type == Media::EXIFData::ET_INT16)
				{
					Int16 *valBuff;
					if (subExItem->size <= 2)
					{
						valBuff = (Int16*)&subExItem->value;
					}
					else
					{
						valBuff = (Int16*)subExItem->dataBuff;
					}
					k = 0;
					while (k < subExItem->size)
					{
						if (k == 0)
						{
							sb->AppendC(UTF8STRC(", value = "));
						}
						else
						{
							sb->AppendC(UTF8STRC(", "));
						}
						sb->AppendI16(valBuff[k]);
						k++;
					}
				}
				else if (subExItem->id == 37500)
				{
					UInt8 *valBuff;
					if (subExItem->size <= 4)
					{
						valBuff = (UInt8*)&subExItem->value;
					}
					else
					{
						valBuff = (UInt8*)subExItem->dataBuff;
					}
					Media::EXIFData *innerExif = ParseMakerNote(valBuff, subExItem->size);
					if (innerExif)
					{
						UTF8Char sbuff[32];
						sb->AppendC(UTF8STRC(", Format = "));
						sb->Append(GetEXIFMakerName(innerExif->GetEXIFMaker()));
						sb->AppendC(UTF8STRC(", Inner "));
						if (linePrefix)
						{
							Text::StrConcat(Text::StrConcatC(sbuff, UTF8STRC("  ")), linePrefix);
						}
						else
						{
							Text::StrConcatC(sbuff, UTF8STRC("  "));
						}
						innerExif->ToString(sb, sbuff);
						DEL_CLASS(innerExif);
					}
					else
					{
						sb->AppendC(UTF8STRC(", value (Other) = "));
						sb->AppendHexBuff(valBuff, subExItem->size, ' ', Text::LineBreakType::CRLF);
					}
				}
				else if (subExItem->type == Media::EXIFData::ET_OTHER)
				{
					UInt8 *valBuff;
					valBuff = (UInt8*)subExItem->dataBuff;
					if (this->exifMaker == Media::EXIFData::EM_OLYMPUS && subExItem->id == 0)
					{
						sb->AppendC(UTF8STRC(", value = "));
						sb->AppendC((const UTF8Char*)valBuff, subExItem->size);
					}
					else
					{
						sb->AppendC(UTF8STRC(", value (Other) = "));
						sb->AppendHexBuff(valBuff, subExItem->size, ' ', Text::LineBreakType::CRLF);
					}
				}
				else
				{
					UInt8 *valBuff;
					if (subExItem->size <= 4 && subExItem->type != Media::EXIFData::ET_OTHER)
					{
						valBuff = (UInt8*)&subExItem->value;
					}
					else
					{
						valBuff = (UInt8*)subExItem->dataBuff;
					}
					sb->AppendC(UTF8STRC(", value (Unk) = "));
					sb->AppendHexBuff(valBuff, subExItem->size, ' ', Text::LineBreakType::CRLF);
				}
				
				i2++;
			}
		}
		else if (exItem->type == Media::EXIFData::ET_STRING)
		{
			sb->AppendC(UTF8STRC(", value = "));
			if (((Char*)exItem->dataBuff)[exItem->size - 1])
			{
				sb->AppendC((UTF8Char*)exItem->dataBuff, exItem->size);
			}
			else
			{
				sb->Append((UTF8Char*)exItem->dataBuff);
			}
		}
		else if (exItem->type == Media::EXIFData::ET_DOUBLE)
		{
			Double *valBuff = (Double*)exItem->dataBuff;
			k = 0;
			while (k < exItem->size)
			{
				if (k == 0)
				{
					sb->AppendC(UTF8STRC(", value = "));
				}
				else
				{
					sb->AppendC(UTF8STRC(", "));
				}
				Text::SBAppendF64(sb, valBuff[k]);
				k++;
			}
		}
		else if (exItem->type == Media::EXIFData::ET_BYTES)
		{
			UInt8 *valBuff;
			if (exItem->size <= 4)
			{
				valBuff = (UInt8*)&exItem->value;
			}
			else
			{
				valBuff = (UInt8*)exItem->dataBuff;
			}
			sb->AppendC(UTF8STRC(", value = "));
			if (exItem->id >= 40091 && exItem->id <= 40095)
			{
				if (valBuff[exItem->size - 2] == 0)
				{
					const UTF8Char *csptr = Text::StrToUTF8New((const UTF16Char*)valBuff);
					sb->Append(csptr);
					Text::StrDelNew(csptr);
				}
				else
				{
					const UTF16Char *csptr = Text::StrCopyNewC((const UTF16Char*)valBuff, exItem->size >> 1);
					const UTF8Char *csptr2 = Text::StrToUTF8New(csptr);
					sb->Append(csptr2);
					Text::StrDelNew(csptr2);
					Text::StrDelNew(csptr);
				}
			}
			else
			{
				if (exItem->size > 1024)
				{
					sb->AppendU32(exItem->size);
					sb->AppendC(UTF8STRC(" bytes: "));
					sb->AppendHexBuff(valBuff, 256, ' ', Text::LineBreakType::CRLF);
					sb->AppendC(UTF8STRC("\r\n...\r\n"));
					sb->AppendHexBuff(&valBuff[((Int32)exItem->size & ~15) - 256], 256 + (exItem->size & 15), ' ', Text::LineBreakType::CRLF);
				}
				else
				{
					sb->AppendHexBuff(valBuff, exItem->size, ' ', Text::LineBreakType::CRLF);
				}
			}
		}
		else if (exItem->type == Media::EXIFData::ET_UINT16)
		{
			UInt16 *valBuff;
			if (exItem->size <= 2)
			{
				valBuff = (UInt16*)&exItem->value;
			}
			else
			{
				valBuff = (UInt16*)exItem->dataBuff;
			}
			if (this->exifMaker == Media::EXIFData::EM_CANON && exItem->id == 1)
			{
				this->ToStringCanonCameraSettings(sb, linePrefix, valBuff, exItem->size);
			}
			else if (this->exifMaker == Media::EXIFData::EM_CANON && exItem->id == 2)
			{
				this->ToStringCanonFocalLength(sb, linePrefix, valBuff, exItem->size);
			}
			else if (this->exifMaker == Media::EXIFData::EM_CANON && exItem->id == 4)
			{
				this->ToStringCanonShotInfo(sb, linePrefix, valBuff, exItem->size);
			}
			else
			{
				k = 0;
				while (k < exItem->size)
				{
					if (k == 0)
					{
						sb->AppendC(UTF8STRC(", value = "));
					}
					else
					{
						sb->AppendC(UTF8STRC(", "));
					}
					sb->AppendU16(valBuff[k]);
					k++;
				}
			}
		}
		else if (exItem->type == Media::EXIFData::ET_UINT32)
		{
			UInt32 *valBuff;
			if (exItem->size <= 1)
			{
				valBuff = (UInt32*)&exItem->value;
			}
			else
			{
				valBuff = (UInt32*)exItem->dataBuff;
			}
			k = 0;
			while (k < exItem->size)
			{
				if (k == 0)
				{
					sb->AppendC(UTF8STRC(", value = "));
				}
				else
				{
					sb->AppendC(UTF8STRC(", "));
				}
				sb->AppendU32(valBuff[k]);
				k++;
			}
		}
		else if (exItem->type == Media::EXIFData::ET_RATIONAL)
		{
			UInt32 *valBuff;
			valBuff = (UInt32*)exItem->dataBuff;
			k = 0;
			while (k < exItem->size)
			{
				if (k == 0)
				{
					sb->AppendC(UTF8STRC(", value = "));
				}
				else
				{
					sb->AppendC(UTF8STRC(", "));
				}
				sb->AppendU32(valBuff[k * 2]);
				sb->AppendC(UTF8STRC(" / "));
				sb->AppendU32(valBuff[k * 2 + 1]);
				if (valBuff[k * 2 + 1] != 0)
				{
					sb->AppendC(UTF8STRC(" ("));
					Text::SBAppendF64(sb, valBuff[k * 2] / (Double)valBuff[k * 2 + 1]);
					sb->AppendC(UTF8STRC(")"));
				}
				k++;
			}
		}
		else if (exItem->type == Media::EXIFData::ET_INT16)
		{
			Int16 *valBuff;
			if (exItem->size <= 2)
			{
				valBuff = (Int16*)&exItem->value;
			}
			else
			{
				valBuff = (Int16*)exItem->dataBuff;
			}
			k = 0;
			while (k < exItem->size)
			{
				if (k == 0)
				{
					sb->AppendC(UTF8STRC(", value = "));
				}
				else
				{
					sb->AppendC(UTF8STRC(", "));
				}
				sb->AppendI16(valBuff[k]);
				k++;
			}
		}
		else if (exItem->type == Media::EXIFData::ET_OTHER)
		{
			if (this->exifMaker == EM_OLYMPUS && exItem->id == 521)
			{
				sb->AppendC(UTF8STRC(", value = "));
				if (((Char*)exItem->dataBuff)[exItem->size - 1])
				{
					sb->AppendC((UTF8Char*)exItem->dataBuff, exItem->size);
				}
				else
				{
					sb->Append((UTF8Char*)exItem->dataBuff);
				}
			}
			else
			{
	//			UInt8 *valBuff;
	//			valBuff = (UInt8*)exItem->dataBuff;
				sb->AppendC(UTF8STRC(", Other: size = "));
				sb->AppendU32(exItem->size);
	//			sb->AppendHex(valBuff, subExItem->size, ' ', Text::StringBuilder::LBT_CRLF);
			}
		}
		else
		{
/*			UInt8 *valBuff;
			if (exItem->size <= 4)
			{
				valBuff = (UInt8*)&exItem->value;
			}
			else
			{
				valBuff = (UInt8*)exItem->dataBuff;
			}*/
			sb->AppendC(UTF8STRC(", Unknown: size = "));
			sb->AppendU32(exItem->size);
//			sb->AppendHex(valBuff, subExItem->size, ' ', Text::StringBuilder::LBT_CRLF);
		}

		i++;
	}
	return true;
}

Bool Media::EXIFData::ToStringCanonCameraSettings(Text::StringBuilderUTF8 *sb, const UTF8Char *linePrefix, UInt16 *valBuff, UOSInt valCnt)
{
	Bool isInt16;
	Bool isUInt16;
	UOSInt k;
	k = 0;
	while (k < valCnt)
	{
		sb->AppendC(UTF8STRC("\r\n"));
		if (linePrefix)
			sb->Append(linePrefix);
		sb->AppendC(UTF8STRC(" "));
		isInt16 = false;
		isUInt16 = false;
		switch (k)
		{
		case 1:
			sb->AppendC(UTF8STRC("MacroMode = "));
			switch ((Int16)valBuff[k])
			{
			case 1:
				sb->AppendC(UTF8STRC("1-Macro"));
				break;
			case 2:
				sb->AppendC(UTF8STRC("2-Normal"));
				break;
			default:
				sb->AppendI16((Int16)valBuff[k]);
				break;
			}
			break;
		case 2:
			sb->AppendC(UTF8STRC("SelfTimer = "));
			isInt16 = true;
			break;
		case 3:
			sb->AppendC(UTF8STRC("Quality = "));
			switch ((Int16)valBuff[k])
			{
			case -1:
				sb->AppendC(UTF8STRC("-1-n/a"));
				break;
			case 1:
				sb->AppendC(UTF8STRC("1-Economy"));
				break;
			case 2:
				sb->AppendC(UTF8STRC("2-Normal"));
				break;
			case 3:
				sb->AppendC(UTF8STRC("3-Fine"));
				break;
			case 4:
				sb->AppendC(UTF8STRC("4-RAW"));
				break;
			case 5:
				sb->AppendC(UTF8STRC("5-Superfine"));
				break;
			case 7:
				sb->AppendC(UTF8STRC("7-CRAW"));
				break;
			case 130:
				sb->AppendC(UTF8STRC("130-Normal Movie"));
				break;
			case 131:
				sb->AppendC(UTF8STRC("131-Movie (2)"));
				break;
			default:
				sb->AppendI16((Int16)valBuff[k]);
				break;
			}
			break;
		case 4:
			sb->AppendC(UTF8STRC("CanonFlashMode = "));
			switch ((Int16)valBuff[k])
			{
			case -1:
				sb->AppendC(UTF8STRC("-1-n/a"));
				break;
			case 0:
				sb->AppendC(UTF8STRC("0-Off"));
				break;
			case 1:
				sb->AppendC(UTF8STRC("1-Auto"));
				break;
			case 2:
				sb->AppendC(UTF8STRC("2-On"));
				break;
			case 3:
				sb->AppendC(UTF8STRC("3-Red-eye Reduction"));
				break;
			case 4:
				sb->AppendC(UTF8STRC("4-Slow Sync"));
				break;
			case 5:
				sb->AppendC(UTF8STRC("5-Red-eye Reduction (Auto)"));
				break;
			case 6:
				sb->AppendC(UTF8STRC("6-Red-eye Reduction (On)"));
				break;
			case 16:
				sb->AppendC(UTF8STRC("16-External Flash"));
				break;
			default:
				sb->AppendI16((Int16)valBuff[k]);
				break;
			}
			break;
		case 5:
			sb->AppendC(UTF8STRC("ContinuousDrive = "));
			switch ((Int16)valBuff[k])
			{
			case 0:
				sb->AppendC(UTF8STRC("0-Single"));
				break;
			case 1:
				sb->AppendC(UTF8STRC("1-Continuous"));
				break;
			case 2:
				sb->AppendC(UTF8STRC("2-Movie"));
				break;
			case 3:
				sb->AppendC(UTF8STRC("3-Continuous, Speed Priority"));
				break;
			case 4:
				sb->AppendC(UTF8STRC("4-Continuous, Low"));
				break;
			case 5:
				sb->AppendC(UTF8STRC("5-Continuous, High"));
				break;
			case 6:
				sb->AppendC(UTF8STRC("6-Silent Single"));
				break;
			case 9:
				sb->AppendC(UTF8STRC("9-Single, Silent"));
				break;
			case 10:
				sb->AppendC(UTF8STRC("10-Continuous, Silent"));
				break;
			default:
				sb->AppendI16((Int16)valBuff[k]);
				break;
			}
			break;
		case 7:
			sb->AppendC(UTF8STRC("FocusMode = "));
			switch ((Int16)valBuff[k])
			{
			case 0:
				sb->AppendC(UTF8STRC("0-One-shot AF"));
				break;
			case 1:
				sb->AppendC(UTF8STRC("1-AI Servo AF"));
				break;
			case 2:
				sb->AppendC(UTF8STRC("2-AI Focus AF"));
				break;
			case 3:
				sb->AppendC(UTF8STRC("3-Manual Focus"));
				break;
			case 4:
				sb->AppendC(UTF8STRC("4-Single"));
				break;
			case 5:
				sb->AppendC(UTF8STRC("5-Continuous"));
				break;
			case 6:
				sb->AppendC(UTF8STRC("6-Manual Focus"));
				break;
			case 16:
				sb->AppendC(UTF8STRC("16-Pan Focus"));
				break;
			case 256:
				sb->AppendC(UTF8STRC("256-AF+MF"));
				break;
			case 512:
				sb->AppendC(UTF8STRC("512-Movie Snap Focus"));
				break;
			case 519:
				sb->AppendC(UTF8STRC("519-Movie Servo AF"));
				break;
			default:
				sb->AppendI16((Int16)valBuff[k]);
				break;
			}
			break;
		case 9:
			sb->AppendC(UTF8STRC("RecordMode = "));
			switch ((Int16)valBuff[k])
			{
			case 1:
				sb->AppendC(UTF8STRC("1-JPEG"));
				break;
			case 2:
				sb->AppendC(UTF8STRC("2-CRW+THM"));
				break;
			case 3:
				sb->AppendC(UTF8STRC("3-AVI+THM"));
				break;
			case 4:
				sb->AppendC(UTF8STRC("4-TIF"));
				break;
			case 5:
				sb->AppendC(UTF8STRC("5-TIF+JPEG"));
				break;
			case 6:
				sb->AppendC(UTF8STRC("6-CR2"));
				break;
			case 7:
				sb->AppendC(UTF8STRC("7-CR2+JPEG"));
				break;
			case 9:
				sb->AppendC(UTF8STRC("9-MOV"));
				break;
			case 10:
				sb->AppendC(UTF8STRC("10-MP4"));
				break;
			case 11:
				sb->AppendC(UTF8STRC("11-CRM"));
				break;
			case 12:
				sb->AppendC(UTF8STRC("12-CR3"));
				break;
			case 13:
				sb->AppendC(UTF8STRC("13-CR3+JPEG"));
				break;
			default:
				sb->AppendI16((Int16)valBuff[k]);
				break;
			}
			break;
		case 10:
			sb->AppendC(UTF8STRC("CanonImageSize = "));
			switch ((Int16)valBuff[k])
			{
			case -1:
				sb->AppendC(UTF8STRC("-1-n/a"));
				break;
			case 0:
				sb->AppendC(UTF8STRC("0-Large"));
				break;
			case 1:
				sb->AppendC(UTF8STRC("1-Medium"));
				break;
			case 2:
				sb->AppendC(UTF8STRC("2-Small"));
				break;
			case 5:
				sb->AppendC(UTF8STRC("5-Medium 1"));
				break;
			case 6:
				sb->AppendC(UTF8STRC("6-Medium 2"));
				break;
			case 7:
				sb->AppendC(UTF8STRC("7-Medium 3"));
				break;
			case 8:
				sb->AppendC(UTF8STRC("8-Postcard"));
				break;
			case 9:
				sb->AppendC(UTF8STRC("9-Widescreen"));
				break;
			case 10:
				sb->AppendC(UTF8STRC("10-Medium Widescreen"));
				break;
			case 14:
				sb->AppendC(UTF8STRC("14-Small 1"));
				break;
			case 15:
				sb->AppendC(UTF8STRC("15-Small 2"));
				break;
			case 16:
				sb->AppendC(UTF8STRC("16-Small 3"));
				break;
			case 128:
				sb->AppendC(UTF8STRC("128-640x480 Movie"));
				break;
			case 129:
				sb->AppendC(UTF8STRC("129-Medium Movie"));
				break;
			case 130:
				sb->AppendC(UTF8STRC("130-Small Movie"));
				break;
			case 137:
				sb->AppendC(UTF8STRC("137-1280x720 Movie"));
				break;
			case 142:
				sb->AppendC(UTF8STRC("142-1920x1080 Movie"));
				break;
			case 143:
				sb->AppendC(UTF8STRC("143-4096x2160 Movie"));
				break;
			default:
				sb->AppendI16((Int16)valBuff[k]);
				break;
			}
			break;
		case 11:
			sb->AppendC(UTF8STRC("EasyMode = "));
			switch ((Int16)valBuff[k])
			{
			case 0:
				sb->AppendC(UTF8STRC("0-Full auto"));
				break;
			case 1:
				sb->AppendC(UTF8STRC("1-Manual"));
				break;
			case 2:
				sb->AppendC(UTF8STRC("2-Landscape"));
				break;
			case 3:
				sb->AppendC(UTF8STRC("3-Fast shutter"));
				break;
			case 4:
				sb->AppendC(UTF8STRC("4-Slow shutter"));
				break;
			case 5:
				sb->AppendC(UTF8STRC("5-Night"));
				break;
			case 6:
				sb->AppendC(UTF8STRC("6-Grey Scale"));
				break;
			case 7:
				sb->AppendC(UTF8STRC("7-Sepia"));
				break;
			case 8:
				sb->AppendC(UTF8STRC("8-Portrait"));
				break;
			case 9:
				sb->AppendC(UTF8STRC("9-Sports"));
				break;
			case 10:
				sb->AppendC(UTF8STRC("10-Macro"));
				break;
			case 11:
				sb->AppendC(UTF8STRC("11-Black & White"));
				break;
			case 12:
				sb->AppendC(UTF8STRC("12-Pan focus"));
				break;
			case 13:
				sb->AppendC(UTF8STRC("13-Vivid"));
				break;
			case 14:
				sb->AppendC(UTF8STRC("14-Neutral"));
				break;
			case 15:
				sb->AppendC(UTF8STRC("15-Flash Off"));
				break;
			case 16:
				sb->AppendC(UTF8STRC("16-Long Shutter"));
				break;
			case 17:
				sb->AppendC(UTF8STRC("17-Super Macro"));
				break;
			case 18:
				sb->AppendC(UTF8STRC("18-Foliage"));
				break;
			case 19:
				sb->AppendC(UTF8STRC("19-Indoor"));
				break;
			case 20:
				sb->AppendC(UTF8STRC("20-Fireworks"));
				break;
			case 21:
				sb->AppendC(UTF8STRC("21-Beach"));
				break;
			case 22:
				sb->AppendC(UTF8STRC("22-Underwater"));
				break;
			case 23:
				sb->AppendC(UTF8STRC("23-Snow"));
				break;
			case 24:
				sb->AppendC(UTF8STRC("24-Kids & Pets"));
				break;
			case 25:
				sb->AppendC(UTF8STRC("25-Night Snapshot"));
				break;
			case 26:
				sb->AppendC(UTF8STRC("26-Digital Macro"));
				break;
			case 27:
				sb->AppendC(UTF8STRC("27-My Colors"));
				break;
			case 28:
				sb->AppendC(UTF8STRC("28-Movie Snap"));
				break;
			case 29:
				sb->AppendC(UTF8STRC("29-Super Macro 2"));
				break;
			case 30:
				sb->AppendC(UTF8STRC("30-Color Accent"));
				break;
			case 31:
				sb->AppendC(UTF8STRC("31-Color Swap"));
				break;
			case 32:
				sb->AppendC(UTF8STRC("32-Aquarium"));
				break;
			case 33:
				sb->AppendC(UTF8STRC("33-ISO 3200"));
				break;
			case 34:
				sb->AppendC(UTF8STRC("34-ISO 6400"));
				break;
			case 35:
				sb->AppendC(UTF8STRC("35-Creative Light Effect"));
				break;
			case 36:
				sb->AppendC(UTF8STRC("36-Easy"));
				break;
			case 37:
				sb->AppendC(UTF8STRC("37-Quick Shot"));
				break;
			case 38:
				sb->AppendC(UTF8STRC("38-Creative Auto"));
				break;
			case 39:
				sb->AppendC(UTF8STRC("39-Zoom Blur"));
				break;
			case 40:
				sb->AppendC(UTF8STRC("40-Low Light"));
				break;
			case 41:
				sb->AppendC(UTF8STRC("41-Nostalgic"));
				break;
			case 42:
				sb->AppendC(UTF8STRC("42-Super Vivid"));
				break;
			case 43:
				sb->AppendC(UTF8STRC("43-Poster Effect"));
				break;
			case 44:
				sb->AppendC(UTF8STRC("44-Face Self-Time"));
				break;
			case 45:
				sb->AppendC(UTF8STRC("45-Smile"));
				break;
			case 46:
				sb->AppendC(UTF8STRC("46-Wink Self-Timer"));
				break;
			case 47:
				sb->AppendC(UTF8STRC("47-Fisheye Effect"));
				break;
			case 48:
				sb->AppendC(UTF8STRC("48-Miniature Effect"));
				break;
			case 49:
				sb->AppendC(UTF8STRC("49-High-speed Burst"));
				break;
			case 50:
				sb->AppendC(UTF8STRC("50-Best Image Selection"));
				break;
			case 51:
				sb->AppendC(UTF8STRC("51-High Dynamic Range"));
				break;
			case 52:
				sb->AppendC(UTF8STRC("52-Handheld Night Scene"));
				break;
			case 53:
				sb->AppendC(UTF8STRC("53-Movie Digest"));
				break;
			case 54:
				sb->AppendC(UTF8STRC("54-Live View Control"));
				break;
			case 55:
				sb->AppendC(UTF8STRC("55-Discreet"));
				break;
			case 56:
				sb->AppendC(UTF8STRC("56-Blur Reduction"));
				break;
			case 57:
				sb->AppendC(UTF8STRC("57-Monochrome"));
				break;
			case 58:
				sb->AppendC(UTF8STRC("58-Toy Camera Effect"));
				break;
			case 59:
				sb->AppendC(UTF8STRC("59-Scene Intelligent Auto"));
				break;
			case 60:
				sb->AppendC(UTF8STRC("60-High-speed Burst HQ"));
				break;
			case 61:
				sb->AppendC(UTF8STRC("61-Smooth Skin"));
				break;
			case 62:
				sb->AppendC(UTF8STRC("62-Soft Focus"));
				break;
			case 257:
				sb->AppendC(UTF8STRC("257-Spotlight"));
				break;
			case 258:
				sb->AppendC(UTF8STRC("258-Night 2"));
				break;
			case 259:
				sb->AppendC(UTF8STRC("259-Night+"));
				break;
			case 260:
				sb->AppendC(UTF8STRC("260-Super Night"));
				break;
			case 261:
				sb->AppendC(UTF8STRC("261-Sunset"));
				break;
			case 263:
				sb->AppendC(UTF8STRC("263-Night Scene"));
				break;
			case 264:
				sb->AppendC(UTF8STRC("264-Surface"));
				break;
			case 265:
				sb->AppendC(UTF8STRC("265-Low Light 2"));
				break;
			default:
				sb->AppendI16((Int16)valBuff[k]);
				break;
			}
			break;
		case 12:
			sb->AppendC(UTF8STRC("DigitalZoom = "));
			switch ((Int16)valBuff[k])
			{
			case 0:
				sb->AppendC(UTF8STRC("0-None"));
				break;
			case 1:
				sb->AppendC(UTF8STRC("1-2x"));
				break;
			case 2:
				sb->AppendC(UTF8STRC("2-4x"));
				break;
			case 3:
				sb->AppendC(UTF8STRC("3-Other"));
				break;
			default:
				sb->AppendI16((Int16)valBuff[k]);
				break;
			}
			break;
		case 13:
			sb->AppendC(UTF8STRC("Contrast = "));
			isInt16 = true;
			break;
		case 14:
			sb->AppendC(UTF8STRC("Saturation = "));
			isInt16 = true;
			break;
		case 15:
			sb->AppendC(UTF8STRC("Sharpness = "));
			isInt16 = true;
			break;
		case 16:
			sb->AppendC(UTF8STRC("CameraISO = "));
			isInt16 = true;
			break;
		case 17:
			sb->AppendC(UTF8STRC("MeteringMode = "));
			switch ((Int16)valBuff[k])
			{
			case 0:
				sb->AppendC(UTF8STRC("0-Default"));
				break;
			case 1:
				sb->AppendC(UTF8STRC("1-Spot"));
				break;
			case 2:
				sb->AppendC(UTF8STRC("2-Average"));
				break;
			case 3:
				sb->AppendC(UTF8STRC("3-Evaluative"));
				break;
			case 4:
				sb->AppendC(UTF8STRC("4-Partial"));
				break;
			case 5:
				sb->AppendC(UTF8STRC("5-Center-weighted average"));
				break;
			default:
				sb->AppendI16((Int16)valBuff[k]);
				break;
			}
			break;
		case 18:
			sb->AppendC(UTF8STRC("FocusRange = "));
			switch ((Int16)valBuff[k])
			{
			case 0:
				sb->AppendC(UTF8STRC("0-Manual"));
				break;
			case 1:
				sb->AppendC(UTF8STRC("1-Auto"));
				break;
			case 2:
				sb->AppendC(UTF8STRC("2-Not Known"));
				break;
			case 3:
				sb->AppendC(UTF8STRC("3-Macro"));
				break;
			case 4:
				sb->AppendC(UTF8STRC("4-Very Close"));
				break;
			case 5:
				sb->AppendC(UTF8STRC("5-Close"));
				break;
			case 6:
				sb->AppendC(UTF8STRC("6-Middle Range"));
				break;
			case 7:
				sb->AppendC(UTF8STRC("7-Far Range"));
				break;
			case 8:
				sb->AppendC(UTF8STRC("8-Pan Focus"));
				break;
			case 9:
				sb->AppendC(UTF8STRC("9-Super Macro"));
				break;
			case 10:
				sb->AppendC(UTF8STRC("10-Infinity"));
				break;
			default:
				sb->AppendI16((Int16)valBuff[k]);
				break;
			}
			break;
		case 19:
			sb->AppendC(UTF8STRC("AFPoint = "));
			switch ((Int16)valBuff[k])
			{
			case 0x2005:
				sb->AppendC(UTF8STRC("0x2005-Manual AF point selection "));
				break;
			case 0x3000:
				sb->AppendC(UTF8STRC("0x3000-None (MF)"));
				break;
			case 0x3001:
				sb->AppendC(UTF8STRC("0x3001-Auto AF point selection"));
				break;
			case 0x3002:
				sb->AppendC(UTF8STRC("0x3002-Right"));
				break;
			case 0x3003:
				sb->AppendC(UTF8STRC("0x3003-Center"));
				break;
			case 0x3004:
				sb->AppendC(UTF8STRC("0x3004-Left"));
				break;
			case 0x4001:
				sb->AppendC(UTF8STRC("0x4001-Auto AF point selection"));
				break;
			case 0x4006:
				sb->AppendC(UTF8STRC("0x4006-Face Detect"));
				break;
			default:
				sb->AppendI16((Int16)valBuff[k]);
				break;
			}
			break;
		case 20:
			sb->AppendC(UTF8STRC("CanonExposureMode = "));
			switch ((Int16)valBuff[k])
			{
			case 0:
				sb->AppendC(UTF8STRC("0-Easy"));
				break;
			case 1:
				sb->AppendC(UTF8STRC("1-Program AE"));
				break;
			case 2:
				sb->AppendC(UTF8STRC("2-Shutter speed priority AE"));
				break;
			case 3:
				sb->AppendC(UTF8STRC("3-Aperture-priority AE"));
				break;
			case 4:
				sb->AppendC(UTF8STRC("4-Manual"));
				break;
			case 5:
				sb->AppendC(UTF8STRC("5-Depth-of-field AE"));
				break;
			case 6:
				sb->AppendC(UTF8STRC("6-M-Dep"));
				break;
			case 7:
				sb->AppendC(UTF8STRC("7-Bulb"));
				break;
			default:
				sb->AppendI16((Int16)valBuff[k]);
				break;
			}
			break;
		case 22:
			sb->AppendC(UTF8STRC("LensType = "));
			this->ToStringCanonLensType(sb, valBuff[k]);
			break;
		case 23:
			sb->AppendC(UTF8STRC("MaxFocalLength = "));
			isUInt16 = true;
			break;
		case 24:
			sb->AppendC(UTF8STRC("MinFocalLength = "));
			isUInt16 = true;
			break;
		case 25:
			sb->AppendC(UTF8STRC("FocalUnits = "));
			isInt16 = true;
			break;
		case 26:
			sb->AppendC(UTF8STRC("MaxAperture = "));
			isInt16 = true;
			break;
		case 27:
			sb->AppendC(UTF8STRC("MinAperture = "));
			isInt16 = true;
			break;
		case 28:
			sb->AppendC(UTF8STRC("FlashActivity = "));
			isInt16 = true;
			break;
		case 29:
			sb->AppendC(UTF8STRC("FlashBits = 0x"));
			sb->AppendHex16(valBuff[k]);
			break;
		case 32:
			sb->AppendC(UTF8STRC("FocusContinuous = "));
			switch ((Int16)valBuff[k])
			{
			case 0:
				sb->AppendC(UTF8STRC("0-Single"));
				break;
			case 1:
				sb->AppendC(UTF8STRC("1-Continuous"));
				break;
			case 8:
				sb->AppendC(UTF8STRC("8-Manual"));
				break;
			default:
				sb->AppendI16((Int16)valBuff[k]);
				break;
			}
			break;
		case 33:
			sb->AppendC(UTF8STRC("AESetting = "));
			switch ((Int16)valBuff[k])
			{
			case 0:
				sb->AppendC(UTF8STRC("0-Normal AE"));
				break;
			case 1:
				sb->AppendC(UTF8STRC("1-Exposure Compensation"));
				break;
			case 2:
				sb->AppendC(UTF8STRC("2-AE Lock"));
				break;
			case 3:
				sb->AppendC(UTF8STRC("3-AE Lock + Exposure Comp."));
				break;
			case 4:
				sb->AppendC(UTF8STRC("4-No AE"));
				break;
			default:
				sb->AppendI16((Int16)valBuff[k]);
				break;
			}
			break;
		case 34:
			sb->AppendC(UTF8STRC("ImageStabilization = "));
			switch ((Int16)valBuff[k])
			{
			case 0:
				sb->AppendC(UTF8STRC("0-Off"));
				break;
			case 1:
				sb->AppendC(UTF8STRC("1-On"));
				break;
			case 2:
				sb->AppendC(UTF8STRC("2-Shoot Only"));
				break;
			case 3:
				sb->AppendC(UTF8STRC("3-Panning"));
				break;
			case 4:
				sb->AppendC(UTF8STRC("4-Dynamic"));
				break;
			case 256:
				sb->AppendC(UTF8STRC("256-Off"));
				break;
			case 257:
				sb->AppendC(UTF8STRC("257-On"));
				break;
			case 258:
				sb->AppendC(UTF8STRC("258-Shoot Only"));
				break;
			case 259:
				sb->AppendC(UTF8STRC("259-Panning"));
				break;
			case 260:
				sb->AppendC(UTF8STRC("260-Dynamic"));
				break;
			default:
				sb->AppendI16((Int16)valBuff[k]);
				break;
			}
			break;
		case 35:
			sb->AppendC(UTF8STRC("DisplayAperture = "));
			isInt16 = true;
			break;
		case 36:
			sb->AppendC(UTF8STRC("ZoomSourceWidth = "));
			isInt16 = true;
			break;
		case 37:
			sb->AppendC(UTF8STRC("ZoomTargetWidth = "));
			isInt16 = true;
			break;
		case 39:
			sb->AppendC(UTF8STRC("AESetting = "));
			switch ((Int16)valBuff[k])
			{
			case 0:
				sb->AppendC(UTF8STRC("0-Center"));
				break;
			case 1:
				sb->AppendC(UTF8STRC("1-AF Point"));
				break;
			default:
				sb->AppendI16((Int16)valBuff[k]);
				break;
			}
			break;
		case 40:
			sb->AppendC(UTF8STRC("PhotoEffect = "));
			switch ((Int16)valBuff[k])
			{
			case 0:
				sb->AppendC(UTF8STRC("0-Off"));
				break;
			case 1:
				sb->AppendC(UTF8STRC("1-Vivid"));
				break;
			case 2:
				sb->AppendC(UTF8STRC("2-Neutral"));
				break;
			case 3:
				sb->AppendC(UTF8STRC("3-Smooth"));
				break;
			case 4:
				sb->AppendC(UTF8STRC("4-Sepia"));
				break;
			case 5:
				sb->AppendC(UTF8STRC("5-B&W"));
				break;
			case 6:
				sb->AppendC(UTF8STRC("6-Custom"));
				break;
			case 100:
				sb->AppendC(UTF8STRC("100-My Color Data"));
				break;
			default:
				sb->AppendI16((Int16)valBuff[k]);
				break;
			}
			break;
		case 41:
			sb->AppendC(UTF8STRC("ManualFlashOutput = "));
			switch ((Int16)valBuff[k])
			{
			case 0:
				sb->AppendC(UTF8STRC("0-n/a"));
				break;
			case 0x500:
				sb->AppendC(UTF8STRC("0x500-Full"));
				break;
			case 0x502:
				sb->AppendC(UTF8STRC("0x500-Medium"));
				break;
			case 0x504:
				sb->AppendC(UTF8STRC("0x500-Low"));
				break;
			case 0x7fff:
				sb->AppendC(UTF8STRC("0x7fff-n/a"));
				break;
			default:
				sb->AppendC(UTF8STRC("0x"));
				sb->AppendHex16(valBuff[k]);
				break;
			}
			break;
		case 42:
			sb->AppendC(UTF8STRC("ColorTone = "));
			isInt16 = true;
			break;
		case 46:
			sb->AppendC(UTF8STRC("SRAWQuality = "));
			switch ((Int16)valBuff[k])
			{
			case 0:
				sb->AppendC(UTF8STRC("0-n/a"));
				break;
			case 1:
				sb->AppendC(UTF8STRC("1-sRAW1(mRAW)"));
				break;
			case 2:
				sb->AppendC(UTF8STRC("2-sRAW2(sRAW)"));
				break;
			default:
				sb->AppendC(UTF8STRC("0x"));
				sb->AppendHex16(valBuff[k]);
				break;
			}
			break;
		default:
			sb->AppendC(UTF8STRC("Unknown("));
			sb->AppendUOSInt(k);
			sb->AppendC(UTF8STRC(") = "));
			isInt16 = true;
			break;
		}
		if (isInt16)
		{
			sb->AppendI16((Int16)valBuff[k]);
		}
		else if (isUInt16)
		{
			sb->AppendU16(valBuff[k]);
		}
		k++;
	}
	return true;
}

Bool Media::EXIFData::ToStringCanonFocalLength(Text::StringBuilderUTF8 *sb, const UTF8Char *linePrefix, UInt16 *valBuff, UOSInt valCnt)
{
	Bool isInt16;
	Bool isUInt16;
	UOSInt k;
	k = 0;
	while (k < valCnt)
	{
		sb->AppendC(UTF8STRC("\r\n"));
		if (linePrefix)
			sb->Append(linePrefix);
		sb->AppendC(UTF8STRC(" "));
		isInt16 = false;
		isUInt16 = false;
		switch (k)
		{
		case 0:
			sb->AppendC(UTF8STRC("FocalType = "));
			switch ((Int16)valBuff[k])
			{
			case 1:
				sb->AppendC(UTF8STRC("1-Fixed"));
				break;
			case 2:
				sb->AppendC(UTF8STRC("2-Zoom"));
				break;
			default:
				sb->AppendI16((Int16)valBuff[k]);
				break;
			}
			break;
		case 1:
			sb->AppendC(UTF8STRC("FocalLength = "));
			isUInt16 = true;
			break;
		case 2:
			sb->AppendC(UTF8STRC("FocalPlaneXSize = "));
			isUInt16 = true;
			break;
		case 3:
			sb->AppendC(UTF8STRC("FocalPlaneYSize = "));
			isUInt16 = true;
			break;
		default:
			sb->AppendC(UTF8STRC("Unknown("));
			sb->AppendUOSInt(k);
			sb->AppendC(UTF8STRC(") = "));
			isInt16 = true;
			break;
		}
		if (isInt16)
		{
			sb->AppendI16((Int16)valBuff[k]);
		}
		else if (isUInt16)
		{
			sb->AppendU16(valBuff[k]);
		}
		k++;
	}
	return true;
}

Bool Media::EXIFData::ToStringCanonShotInfo(Text::StringBuilderUTF8 *sb, const UTF8Char *linePrefix, UInt16 *valBuff, UOSInt valCnt)
{
	Bool isInt16;
	Bool isUInt16;
	UOSInt k;
	k = 0;
	while (k < valCnt)
	{
		sb->AppendC(UTF8STRC("\r\n"));
		if (linePrefix)
			sb->Append(linePrefix);
		sb->AppendC(UTF8STRC(" "));
		isInt16 = false;
		isUInt16 = false;
		switch (k)
		{
		case 1:
			sb->AppendC(UTF8STRC("AutoISO = "));
			isInt16 = true;
			break;
		case 2:
			sb->AppendC(UTF8STRC("BaseISO = "));
			isInt16 = true;
			break;
		case 3:
			sb->AppendC(UTF8STRC("MeasuredEV = "));
			isInt16 = true;
			break;
		case 4:
			sb->AppendC(UTF8STRC("TargetAperture = "));
			isInt16 = true;
			break;
		case 5:
			sb->AppendC(UTF8STRC("TargetExposureTime = "));
			isInt16 = true;
			break;
		case 6:
			sb->AppendC(UTF8STRC("ExposureCompensation = "));
			isInt16 = true;
			break;
		case 7:
			sb->AppendC(UTF8STRC("WhiteBalance = "));
			isInt16 = true;
			break;
		case 8:
			sb->AppendC(UTF8STRC("SlowShutter = "));
			switch ((Int16)valBuff[k])
			{
			case -1:
				sb->AppendC(UTF8STRC("-1-n/a"));
				break;
			case 0:
				sb->AppendC(UTF8STRC("0-Off"));
				break;
			case 1:
				sb->AppendC(UTF8STRC("1-Night Scene"));
				break;
			case 2:
				sb->AppendC(UTF8STRC("2-On"));
				break;
			case 3:
				sb->AppendC(UTF8STRC("3-None"));
				break;
			default:
				sb->AppendI16((Int16)valBuff[k]);
				break;
			}
			break;
		case 9:
			sb->AppendC(UTF8STRC("SequenceNumber = "));
			isInt16 = true;
			break;
		case 10:
			sb->AppendC(UTF8STRC("OpticalZoomCode = "));
			isInt16 = true;
			break;
		case 12:
			sb->AppendC(UTF8STRC("CameraTemperature = "));
			isInt16 = true;
			break;
		case 13:
			sb->AppendC(UTF8STRC("FlashGuideNumber = "));
			isInt16 = true;
			break;
		case 14:
			sb->AppendC(UTF8STRC("AFPointsInFocus = "));
			switch ((Int16)valBuff[k])
			{
			case 0x3000:
				sb->AppendC(UTF8STRC("0x3000-None (MF)"));
				break;
			case 0x3001:
				sb->AppendC(UTF8STRC("0x3001-Right"));
				break;
			case 0x3002:
				sb->AppendC(UTF8STRC("0x3002-Center"));
				break;
			case 0x3003:
				sb->AppendC(UTF8STRC("0x3003-Center+Right"));
				break;
			case 0x3004:
				sb->AppendC(UTF8STRC("0x3004-Left"));
				break;
			case 0x3005:
				sb->AppendC(UTF8STRC("0x3005-Left+Right"));
				break;
			case 0x3006:
				sb->AppendC(UTF8STRC("0x3006-Left+Center"));
				break;
			case 0x3007:
				sb->AppendC(UTF8STRC("0x3007-All"));
				break;
			default:
				sb->AppendC(UTF8STRC("0x"));
				sb->AppendHex16(valBuff[k]);
				break;
			}
			break;
		case 15:
			sb->AppendC(UTF8STRC("FlashExposureComp = "));
			isInt16 = true;
			break;
		case 16:
			sb->AppendC(UTF8STRC("AutoExposureBracketing = "));
			switch ((Int16)valBuff[k])
			{
			case -1:
				sb->AppendC(UTF8STRC("-1-On"));
				break;
			case 0:
				sb->AppendC(UTF8STRC("0-Off"));
				break;
			case 1:
				sb->AppendC(UTF8STRC("1-On (shot 1)"));
				break;
			case 2:
				sb->AppendC(UTF8STRC("2-On (shot 2)"));
				break;
			case 3:
				sb->AppendC(UTF8STRC("3-On (shot 3)"));
				break;
			default:
				sb->AppendI16((Int16)valBuff[k]);
				break;
			}
			break;
		case 17:
			sb->AppendC(UTF8STRC("AEBBracketValue = "));
			isInt16 = true;
			break;
		case 18:
			sb->AppendC(UTF8STRC("ControlMode = "));
			switch ((Int16)valBuff[k])
			{
			case 0:
				sb->AppendC(UTF8STRC("0-n/a"));
				break;
			case 1:
				sb->AppendC(UTF8STRC("1-Camera Local Control"));
				break;
			case 3:
				sb->AppendC(UTF8STRC("3-Computer Remote Control"));
				break;
			default:
				sb->AppendI16((Int16)valBuff[k]);
				break;
			}
			break;
		case 19:
			sb->AppendC(UTF8STRC("FocusDistanceUpper = "));
			isUInt16 = true;
			break;
		case 20:
			sb->AppendC(UTF8STRC("FocusDistanceLower = "));
			isUInt16 = true;
			break;
		case 21:
			sb->AppendC(UTF8STRC("FNumber = "));
			isInt16 = true;
			break;
		case 22:
			sb->AppendC(UTF8STRC("ExposureTime = "));
			isInt16 = true;
			break;
		case 23:
			sb->AppendC(UTF8STRC("MeasuredEV2 = "));
			isInt16 = true;
			break;
		case 24:
			sb->AppendC(UTF8STRC("BulbDuration = "));
			isInt16 = true;
			break;
		case 26:
			sb->AppendC(UTF8STRC("CameraType = "));
			switch ((Int16)valBuff[k])
			{
			case 0:
				sb->AppendC(UTF8STRC("0-n/a"));
				break;
			case 248:
				sb->AppendC(UTF8STRC("248-EOS High-end"));
				break;
			case 250:
				sb->AppendC(UTF8STRC("250-Compact"));
				break;
			case 252:
				sb->AppendC(UTF8STRC("252-EOS Mid-range"));
				break;
			case 255:
				sb->AppendC(UTF8STRC("255-DV Camera"));
				break;
			default:
				sb->AppendI16((Int16)valBuff[k]);
				break;
			}
			break;
		case 27:
			sb->AppendC(UTF8STRC("AutoRotate = "));
			switch ((Int16)valBuff[k])
			{
			case -1:
				sb->AppendC(UTF8STRC("-1-n/a"));
				break;
			case 0:
				sb->AppendC(UTF8STRC("0-None"));
				break;
			case 1:
				sb->AppendC(UTF8STRC("1-Rotate 90 CW"));
				break;
			case 2:
				sb->AppendC(UTF8STRC("1-Rotate 180"));
				break;
			case 3:
				sb->AppendC(UTF8STRC("3-Rotate 270 CW"));
				break;
			default:
				sb->AppendI16((Int16)valBuff[k]);
				break;
			}
			break;
		case 28:
			sb->AppendC(UTF8STRC("NDFilter = "));
			switch ((Int16)valBuff[k])
			{
			case -1:
				sb->AppendC(UTF8STRC("-1-n/a"));
				break;
			case 0:
				sb->AppendC(UTF8STRC("0-Off"));
				break;
			case 1:
				sb->AppendC(UTF8STRC("1-On"));
				break;
			default:
				sb->AppendI16((Int16)valBuff[k]);
				break;
			}
			break;
		case 29:
			sb->AppendC(UTF8STRC("SelfTimer2 = "));
			isInt16 = true;
			break;
		case 33:
			sb->AppendC(UTF8STRC("FlashOutput = "));
			isInt16 = true;
			break;
		default:
			sb->AppendC(UTF8STRC("Unknown("));
			sb->AppendUOSInt(k);
			sb->AppendC(UTF8STRC(") = "));
			isInt16 = true;
			break;
		}
		if (isInt16)
		{
			sb->AppendI16((Int16)valBuff[k]);
		}
		else if (isUInt16)
		{
			sb->AppendU16(valBuff[k]);
		}
		k++;
	}
	return true;
}

Bool Media::EXIFData::ToStringCanonLensType(Text::StringBuilderUTF8 *sb, UInt16 lensType)
{
	sb->AppendC(UTF8STRC("0x"));
	sb->AppendHex16(lensType);
	return true;
}

void Media::EXIFData::ToExifBuff(UInt8 *buff, UInt32 *startOfst, UInt32 *otherOfst)
{
	ToExifBuff(buff, this->exifMap->GetValues(), startOfst, otherOfst);
}

void Media::EXIFData::GetExifBuffSize(UInt32 *size, UInt32 *endOfst)
{
	GetExifBuffSize(this->exifMap->GetValues(), size, endOfst);
}

Media::EXIFData *Media::EXIFData::ParseMakerNote(const UInt8 *buff, UOSInt buffSize)
{
	Media::EXIFData *ret = 0;
	if (Text::StrEquals(buff, (const UTF8Char*)"Panasonic"))
	{
		ret = ParseIFD(&buff[12], buffSize - 12, Media::EXIFData::TReadInt32, Media::EXIFData::TReadInt16, 0, Media::EXIFData::EM_PANASONIC, 0);
		return ret;
	}
	else if (Text::StrEquals(buff, (const UTF8Char*)"OLYMPUS"))
	{
		if (buff[8] == 'I' && buff[9] == 'I')
		{
			ret = ParseIFD(&buff[12], buffSize - 12, Media::EXIFData::TReadInt32, Media::EXIFData::TReadInt16, 0, Media::EXIFData::EM_OLYMPUS, 0);
			return ret;
		}
	}
	else if (Text::StrEquals(buff, (const UTF8Char*)"OLYMP"))
	{
		ret = ParseIFD(&buff[8], buffSize - 8, Media::EXIFData::TReadInt32, Media::EXIFData::TReadInt16, 0, Media::EXIFData::EM_OLYMPUS, 0);
		return ret;
	}
	else if (Text::StrEquals(buff, (const UTF8Char*)"QVC"))
	{
		ret = ParseIFD(&buff[6], buffSize - 6, Media::EXIFData::TReadMInt32, Media::EXIFData::TReadMInt16, 0, Media::EXIFData::EM_CASIO2, 0);
		return ret;
	}
	else
	{
		const Char *maker = this->GetPhotoMake();
		if (maker)
		{
			if (Text::StrEquals(maker, "Canon"))
			{
				ret = ParseIFD(buff, buffSize, Media::EXIFData::TReadInt32, Media::EXIFData::TReadInt16, 0, Media::EXIFData::EM_CANON, 0);
				return ret;
			}
			else if (Text::StrEquals(maker, "CASIO"))
			{
				ret = ParseIFD(buff, buffSize, Media::EXIFData::TReadMInt32, Media::EXIFData::TReadMInt16, 0, Media::EXIFData::EM_CASIO1, 0);
				return ret;
			}
			else if (Text::StrEquals(maker, "FLIR Systems AB"))
			{
				ret = ParseIFD(buff, buffSize, Media::EXIFData::TReadInt32, Media::EXIFData::TReadInt16, 0, Media::EXIFData::EM_FLIR, 0);
				return ret;
			}
		}
	}
	return ret;
}

const UTF8Char *Media::EXIFData::GetEXIFMakerName(EXIFMaker exifMaker)
{
	switch (exifMaker)
	{
	case Media::EXIFData::EM_PANASONIC:
		return (const UTF8Char*)"Panasonic";
	case Media::EXIFData::EM_CANON:
		return (const UTF8Char*)"Canon";
	case Media::EXIFData::EM_OLYMPUS:
		return (const UTF8Char*)"Olympus";
	case Media::EXIFData::EM_CASIO1:
		return (const UTF8Char*)"Casio Type 1";
	case Media::EXIFData::EM_CASIO2:
		return (const UTF8Char*)"Casio Type 2";
	case Media::EXIFData::EM_FLIR:
		return (const UTF8Char*)"FLIR";
	case Media::EXIFData::EM_STANDARD:
	default:
		return (const UTF8Char*)"Standard";
	}
}

const UTF8Char *Media::EXIFData::GetEXIFName(EXIFMaker exifMaker, Int32 id)
{
	return GetEXIFName(exifMaker, 0, id);
}

const UTF8Char *Media::EXIFData::GetEXIFName(EXIFMaker exifMaker, Int32 id, Int32 subId)
{
	EXIFInfo *infos;
	OSInt cnt;
	if (id == 0)
	{
		if (exifMaker == EM_PANASONIC)
		{
			infos = panasonicInfos;
			cnt = sizeof(panasonicInfos) / sizeof(panasonicInfos[0]);
		}
		else if (exifMaker == EM_CANON)
		{
			infos = canonInfos;
			cnt = sizeof(canonInfos) / sizeof(canonInfos[0]);
		}
		else if (exifMaker == EM_OLYMPUS)
		{
			infos = olympusInfos;
			cnt = sizeof(olympusInfos) / sizeof(olympusInfos[0]);
		}
		else if (exifMaker == EM_CASIO1)
		{
			infos = casio1Infos;
			cnt = sizeof(casio1Infos) / sizeof(casio1Infos[0]);
		}
		else if (exifMaker == EM_CASIO2)
		{
			infos = casio2Infos;
			cnt = sizeof(casio2Infos) / sizeof(casio2Infos[0]);
		}
		else if (exifMaker == EM_FLIR)
		{
			infos = flirInfos;
			cnt = sizeof(flirInfos) / sizeof(flirInfos[0]);
		}
		else
		{
			infos = defInfos;
			cnt = sizeof(defInfos) / sizeof(defInfos[0]);
		}
	}
	else if (id == 34665)
	{
		infos = exifInfos;
		cnt = sizeof(exifInfos) / sizeof(exifInfos[0]);
	}
	else if (id == 34853)
	{
		infos = gpsInfos;
		cnt = sizeof(gpsInfos) / sizeof(gpsInfos[0]);
	}
	else if (exifMaker == EM_OLYMPUS)
	{
		if (id == 0x2010)
		{
			infos = olympus2010Infos;
			cnt = sizeof(olympus2010Infos) / sizeof(olympus2010Infos[0]);
		}
		else if (id == 0x2020)
		{
			infos = olympus2020Infos;
			cnt = sizeof(olympus2020Infos) / sizeof(olympus2020Infos[0]);
		}
		else if (id == 0x2030)
		{
			infos = olympus2030Infos;
			cnt = sizeof(olympus2030Infos) / sizeof(olympus2030Infos[0]);
		}
		else if (id == 0x2040)
		{
			infos = olympus2040Infos;
			cnt = sizeof(olympus2040Infos) / sizeof(olympus2040Infos[0]);
		}
		else if (id == 0x2050)
		{
			infos = olympus2050Infos;
			cnt = sizeof(olympus2050Infos) / sizeof(olympus2050Infos[0]);
		}
		else
		{
			return (const UTF8Char*)"Unknown";
		}
	}
	else
	{
		return (const UTF8Char*)"Unknown";
	}
	OSInt i = 0;
	OSInt j = cnt - 1;
	OSInt k;
	while (i <= j)
	{
		k = (i + j) >> 1;
		if (infos[k].id > subId)
		{
			j = k - 1;
		}
		else if (infos[k].id < subId)
		{
			i = k + 1;
		}
		else
		{
			return (const UTF8Char*)infos[k].name;
		}
	}
	return (const UTF8Char*)"Unknown";
}

const UTF8Char *Media::EXIFData::GetEXIFTypeName(Media::EXIFData::EXIFType type)
{
	switch (type)
	{
	case ET_BYTES:
		return (const UTF8Char*)"Bytes";
	case ET_STRING:
		return (const UTF8Char*)"String";
	case ET_UINT16:
		return (const UTF8Char*)"UInt16";
	case ET_UINT32:
		return (const UTF8Char*)"UInt32";
	case ET_RATIONAL:
		return (const UTF8Char*)"Rational";
	case ET_OTHER:
		return (const UTF8Char*)"Other";
	case ET_INT16:
		return (const UTF8Char*)"Int16";
	case ET_SUBEXIF:
		return (const UTF8Char*)"Exif";
	case ET_DOUBLE:
		return (const UTF8Char*)"Double";
	case ET_UNKNOWN:
	default:
		return (const UTF8Char*)"Unknown";
	};
}

Media::EXIFData *Media::EXIFData::ParseIFD(const UInt8 *buff, UOSInt buffSize, RInt32Func readInt32, RInt16Func readInt16, UInt32 *nextOfst, EXIFMaker exifMaker, UInt32 readBase)
{
	Media::EXIFData *exif;
	const UInt8 *ifdEntries;
	OSInt ifdCnt;
	OSInt i;
	OSInt ifdOfst;
	Int32 tag;
	Int32 ftype;
	UInt32 fcnt;

	ifdCnt = readInt16(buff);
	ifdEntries = &buff[2];

	UInt8 *tmpBuff;
	UInt32 j;
	NEW_CLASS(exif, Media::EXIFData(exifMaker));

	if (readBase == 0)
	{
		readBase = 0x7fffffff;
		ifdOfst = 0;
		i = 0;
		while (i < ifdCnt)
		{
			tag = (UInt16)readInt16(&ifdEntries[ifdOfst]);
			ftype = readInt16(&ifdEntries[ifdOfst + 2]);
			fcnt = (UInt32)readInt32(&ifdEntries[ifdOfst + 4]);

			if (ftype == 1)
			{
				if (fcnt <= 4)
				{
				}
				else
				{
					if (readBase > (UInt32)readInt32(&ifdEntries[ifdOfst + 8]))
					{
						readBase = (UInt32)readInt32(&ifdEntries[ifdOfst + 8]);
					}
				}
			}
			else if (ftype == 2)
			{
				if (fcnt <= 4)
				{
				}
				else
				{
					if (readBase > (UInt32)readInt32(&ifdEntries[ifdOfst + 8]))
					{
						readBase = (UInt32)readInt32(&ifdEntries[ifdOfst + 8]);
					}
				}
			}
			else if (ftype == 3)
			{
				if (fcnt == 1)
				{
				}
				else if (fcnt == 2)
				{
				}
				else
				{
					if (readBase > (UInt32)readInt32(&ifdEntries[ifdOfst + 8]))
					{
						readBase = (UInt32)readInt32(&ifdEntries[ifdOfst + 8]);
					}
				}
			}
			else if (ftype == 4)
			{
				if (fcnt == 1)
				{
				}
				else
				{
					if (readBase > (UInt32)readInt32(&ifdEntries[ifdOfst + 8]))
					{
						readBase = (UInt32)readInt32(&ifdEntries[ifdOfst + 8]);
					}
				}
			}
			else if (ftype == 5)
			{
				if (readBase > (UInt32)readInt32(&ifdEntries[ifdOfst + 8]))
				{
					readBase = (UInt32)readInt32(&ifdEntries[ifdOfst + 8]);
				}
			}
			else if (ftype == 7)
			{
				if (fcnt <= 4)
				{
				}
				else
				{
					if (readBase > (UInt32)readInt32(&ifdEntries[ifdOfst + 8]))
					{
						readBase = (UInt32)readInt32(&ifdEntries[ifdOfst + 8]);
					}
				}
			}
			else if (ftype == 8)
			{
				if (fcnt == 1)
				{
				}
				else if (fcnt == 2)
				{
				}
				else
				{
					if (readBase > (UInt32)readInt32(&ifdEntries[ifdOfst + 8]))
					{
						readBase = (UInt32)readInt32(&ifdEntries[ifdOfst + 8]);
					}
				}
			}
			else if (ftype == 12)
			{
				if (readBase > (UInt32)readInt32(&ifdEntries[ifdOfst + 8]))
				{
					readBase = (UInt32)readInt32(&ifdEntries[ifdOfst + 8]);
				}
			}
			else if (ftype == 13)
			{
				if (readBase > (UInt32)readInt32(&ifdEntries[ifdOfst + 8]))
				{
					readBase = (UInt32)readInt32(&ifdEntries[ifdOfst + 8]);
				}
			}
			else
			{
				j = 0;
			}

			ifdOfst += 12;
			i++;
		}
		readBase = (UInt32)ifdCnt * 12 + 2 + 4 - readBase;
	}

	ifdOfst = 0;
	i = 0;
	while (i < ifdCnt)
	{
		tag = (UInt16)readInt16(&ifdEntries[ifdOfst]);
		ftype = readInt16(&ifdEntries[ifdOfst + 2]);
		fcnt = (UInt32)readInt32(&ifdEntries[ifdOfst + 4]);

		if (ftype == 1)
		{
			if (fcnt <= 4)
			{
				exif->AddBytes(tag, fcnt, (UInt8*)&ifdEntries[ifdOfst + 8]);
			}
			else
			{
				exif->AddBytes(tag, fcnt, &buff[(UInt32)readInt32(&ifdEntries[ifdOfst + 8]) + readBase]);
			}
		}
		else if (ftype == 2)
		{
			if (fcnt <= 4)
			{
				exif->AddString(tag, fcnt, (Char*)&ifdEntries[ifdOfst + 8]);
			}
			else
			{
				exif->AddString(tag, fcnt, (Char*)&buff[(UInt32)readInt32(&ifdEntries[ifdOfst + 8]) + readBase]);
			}
		}
		else if (ftype == 3)
		{
			UInt16 tmp[2];
			if (fcnt == 1)
			{
				tmp[0] = (UInt16)readInt16(&ifdEntries[ifdOfst + 8]);
				exif->AddUInt16(tag, fcnt, tmp);
			}
			else if (fcnt == 2)
			{
				tmp[0] = (UInt16)readInt16(&ifdEntries[ifdOfst + 8]);
				tmp[1] = (UInt16)readInt16(&ifdEntries[ifdOfst + 10]);
				exif->AddUInt16(tag, fcnt, tmp);
			}
			else
			{
				tmpBuff = MemAlloc(UInt8, fcnt << 1);
				MemCopyNO(tmpBuff, &buff[(UInt32)readInt32(&ifdEntries[ifdOfst + 8]) + readBase], fcnt << 1);
				j = fcnt << 1;
				while (j > 0)
				{
					j -= 2;
					*(UInt16*)&tmpBuff[j] = (UInt16)readInt16(&tmpBuff[j]);
				}
				exif->AddUInt16(tag, fcnt, (UInt16*)tmpBuff);
				MemFree(tmpBuff);
			}
		}
		else if (ftype == 4)
		{
			UInt32 tmp;
			if (fcnt == 1)
			{
				tmp = (UInt32)readInt32(&ifdEntries[ifdOfst + 8]);
				exif->AddUInt32(tag, fcnt, &tmp);
			}
			else
			{
				tmpBuff = MemAlloc(UInt8, fcnt << 2);
				MemCopyNO(tmpBuff, &buff[(UInt32)readInt32(&ifdEntries[ifdOfst + 8]) + readBase], fcnt << 2);
				j = fcnt << 2;
				while (j > 0)
				{
					j -= 4;
					*(UInt32*)&tmpBuff[j] = (UInt32)readInt32(&tmpBuff[j]);
				}
				exif->AddUInt32(tag, fcnt, (UInt32*)tmpBuff);
				MemFree(tmpBuff);
			}
		}
		else if (ftype == 5)
		{
			tmpBuff = MemAlloc(UInt8, fcnt << 3);
			MemCopyNO(tmpBuff, &buff[(UInt32)readInt32(&ifdEntries[ifdOfst + 8]) + readBase], fcnt << 3);
			j = fcnt << 3;
			while (j > 0)
			{
				j -= 8;
				*(UInt32*)&tmpBuff[j] = (UInt32)readInt32(&tmpBuff[j]);
				*(UInt32*)&tmpBuff[j + 4] = (UInt32)readInt32(&tmpBuff[j + 4]);
			}
			exif->AddRational(tag, fcnt, (UInt32*)tmpBuff);
			MemFree(tmpBuff);
		}
		else if (ftype == 7)
		{
			if (fcnt <= 4)
			{
				exif->AddOther(tag, fcnt, (UInt8*)&ifdEntries[ifdOfst + 8]);
			}
			else
			{
				UOSInt ofst = (UInt32)readInt32(&ifdEntries[ifdOfst + 8]) + readBase;
				if (ofst + fcnt > buffSize)
				{
					ofst = buffSize - fcnt;
				}
				exif->AddOther(tag, fcnt, &buff[ofst]);
			}
		}
		else if (ftype == 8)
		{
			Int16 tmp[2];
			if (fcnt == 1)
			{
				tmp[0] = readInt16(&ifdEntries[ifdOfst + 8]);
				exif->AddInt16(tag, fcnt, tmp);
			}
			else if (fcnt == 2)
			{
				tmp[0] = readInt16(&ifdEntries[ifdOfst + 8]);
				tmp[1] = readInt16(&ifdEntries[ifdOfst + 10]);
				exif->AddInt16(tag, fcnt, tmp);
			}
			else
			{
				tmpBuff = MemAlloc(UInt8, fcnt << 1);
				MemCopyNO(tmpBuff, &buff[(UInt32)readInt32(&ifdEntries[ifdOfst + 8]) + readBase], fcnt << 1);
				j = fcnt << 1;
				while (j > 0)
				{
					j -= 2;
					*(Int16*)&tmpBuff[j] = readInt16(&tmpBuff[j]);
				}
				exif->AddInt16(tag, fcnt, (Int16*)tmpBuff);
				MemFree(tmpBuff);
			}
		}
		else if (ftype == 12)
		{
			exif->AddDouble(tag, fcnt, (Double*)&buff[(UInt32)readInt32(&ifdEntries[ifdOfst + 8]) + readBase]);
		}
		else if (ftype == 13) //Olympus innerIFD
		{
			Media::EXIFData *subexif = ParseIFD(&buff[(UInt32)readInt32(&ifdEntries[ifdOfst + 8]) + readBase], buffSize - (UInt32)readInt32(&ifdEntries[ifdOfst + 8]) - readBase, readInt32, readInt16, 0, exifMaker, (UInt32)-readInt32(&ifdEntries[ifdOfst + 8]));
			if (subexif)
			{
				exif->AddSubEXIF(tag, subexif);
			}
		}
		else
		{
			j = 0;
		}

		ifdOfst += 12;
		i++;
	}

	if (nextOfst)
	{
		*nextOfst = (UInt32)readInt32(&ifdEntries[ifdCnt * 12]);
	}
	return exif;
}

Media::EXIFData *Media::EXIFData::ParseIFD(IO::IStreamData *fd, UInt64 ofst, RInt32Func readInt32, RInt16Func readInt16, UInt32 *nextOfst, UInt64 readBase)
{
	Media::EXIFData *exif;
	UInt8 *ifdEntries;
	UInt8 ifdBuff[2];
	UOSInt ifdCnt;
	UOSInt i;
	UOSInt readSize;
	OSInt ifdOfst;
	Int32 tag;
	Int32 ftype;
	UInt32 fcnt;
	if (fd->GetRealData(ofst, 2, ifdBuff) != 2)
	{
		return 0;
	}
	ifdCnt = (UInt16)readInt16(ifdBuff);

	ifdEntries = MemAlloc(UInt8, readSize = ifdCnt * 12 + 4);
	if (fd->GetRealData(ofst + 2, readSize, ifdEntries) != readSize)
	{
		MemFree(ifdEntries);
		return 0;
	}

	UInt8 *tmpBuff;
	UInt32 j;
	NEW_CLASS(exif, Media::EXIFData(Media::EXIFData::EM_STANDARD));

	ifdOfst = 0;
	i = 0;
	while (i < ifdCnt)
	{
		tag = (UInt16)readInt16(&ifdEntries[ifdOfst]);
		ftype = readInt16(&ifdEntries[ifdOfst + 2]);
		fcnt = (UInt32)readInt32(&ifdEntries[ifdOfst + 4]);

		if (ftype == 1)
		{
			if (fcnt <= 4)
			{
				exif->AddBytes(tag, fcnt, (UInt8*)&ifdEntries[ifdOfst + 8]);
			}
			else
			{
				tmpBuff = MemAlloc(UInt8, fcnt);
				fd->GetRealData((UInt32)readInt32(&ifdEntries[ifdOfst + 8]) + readBase, fcnt, tmpBuff);
				exif->AddBytes(tag, fcnt, tmpBuff);
				MemFree(tmpBuff);
			}
		}
		else if (ftype == 2)
		{
			if (fcnt <= 4)
			{
				exif->AddString(tag, fcnt, (Char*)&ifdEntries[ifdOfst + 8]);
			}
			else
			{
				tmpBuff = MemAlloc(UInt8, fcnt);
				fd->GetRealData((UInt32)readInt32(&ifdEntries[ifdOfst + 8]) + readBase, fcnt, tmpBuff);
				exif->AddString(tag, fcnt, (Char*)tmpBuff);
				MemFree(tmpBuff);
			}
		}
		else if (ftype == 3)
		{
			UInt16 tmp[2];
			if (fcnt == 1)
			{
				tmp[0] = (UInt16)readInt16(&ifdEntries[ifdOfst + 8]);
				exif->AddUInt16(tag, fcnt, tmp);
			}
			else if (fcnt == 2)
			{
				tmp[0] = (UInt16)readInt16(&ifdEntries[ifdOfst + 8]);
				tmp[1] = (UInt16)readInt16(&ifdEntries[ifdOfst + 10]);
				exif->AddUInt16(tag, fcnt, tmp);
			}
			else
			{
				tmpBuff = MemAlloc(UInt8, fcnt << 1);
				fd->GetRealData((UInt32)readInt32(&ifdEntries[ifdOfst + 8]) + readBase, fcnt << 1, tmpBuff);
				j = fcnt << 1;
				while (j > 0)
				{
					j -= 2;
					*(UInt16*)&tmpBuff[j] = (UInt16)readInt16(&tmpBuff[j]);
				}
				exif->AddUInt16(tag, fcnt, (UInt16*)tmpBuff);
				MemFree(tmpBuff);
			}
		}
		else if (ftype == 4)
		{
			UInt32 tmp;
			if (fcnt == 1)
			{
				tmp = (UInt32)readInt32(&ifdEntries[ifdOfst + 8]);
				if (tag == 34665 || tag == 34853)
				{
					Media::EXIFData *subexif = ParseIFD(fd, tmp + readBase, readInt32, readInt16, 0, readBase);
					if (subexif)
					{
						exif->AddSubEXIF(tag, subexif);
					}
				}
				else
				{
					exif->AddUInt32(tag, fcnt, &tmp);
				}
			}
			else
			{
				tmpBuff = MemAlloc(UInt8, fcnt << 2);
				fd->GetRealData((UInt32)readInt32(&ifdEntries[ifdOfst + 8]) + readBase, fcnt << 2, tmpBuff);
				j = fcnt << 2;
				while (j > 0)
				{
					j -= 4;
					*(UInt32*)&tmpBuff[j] = (UInt32)readInt32(&tmpBuff[j]);
				}
				exif->AddUInt32(tag, fcnt, (UInt32*)tmpBuff);
				MemFree(tmpBuff);
			}
		}
		else if (ftype == 5)
		{
			tmpBuff = MemAlloc(UInt8, fcnt << 3);
			fd->GetRealData((UInt32)readInt32(&ifdEntries[ifdOfst + 8]) + readBase, fcnt << 3, tmpBuff);
			j = fcnt << 3;
			while (j > 0)
			{
				j -= 8;
				*(UInt32*)&tmpBuff[j] = (UInt32)readInt32(&tmpBuff[j]);
				*(UInt32*)&tmpBuff[j + 4] = (UInt32)readInt32(&tmpBuff[j + 4]);
			}
			exif->AddRational(tag, fcnt, (UInt32*)tmpBuff);
			MemFree(tmpBuff);
		}
		else if (ftype == 7)
		{
			if (fcnt <= 4)
			{
				exif->AddOther(tag, fcnt, (UInt8*)&ifdEntries[ifdOfst + 8]);
			}
			else
			{
				tmpBuff = MemAlloc(UInt8, fcnt);
				fd->GetRealData((UInt32)readInt32(&ifdEntries[ifdOfst + 8]) + readBase, fcnt, tmpBuff);
				exif->AddOther(tag, fcnt, tmpBuff);
				MemFree(tmpBuff);
			}
		}
		else if (ftype == 8)
		{
			Int16 tmp;
			if (fcnt == 1)
			{
				tmp = readInt16(&ifdEntries[ifdOfst + 8]);
				exif->AddInt16(tag, fcnt, &tmp);
			}
			else
			{
				tmpBuff = MemAlloc(UInt8, fcnt << 1);
				fd->GetRealData((UInt32)readInt32(&ifdEntries[ifdOfst + 8]) + readBase, fcnt << 1, tmpBuff);
				j = fcnt << 1;
				while (j > 0)
				{
					j -= 2;
					*(Int16*)&tmpBuff[j] = readInt16(&tmpBuff[j]);
				}
				exif->AddInt16(tag, fcnt, (Int16*)tmpBuff);
				MemFree(tmpBuff);
			}
		}
		else if (ftype == 12)
		{
			tmpBuff = MemAlloc(UInt8, fcnt << 3);
			fd->GetRealData((UInt32)readInt32(&ifdEntries[ifdOfst + 8]) + readBase, fcnt << 3, tmpBuff);
			exif->AddDouble(tag, fcnt, (Double*)tmpBuff);
			MemFree(tmpBuff);
		}
		else
		{
			j = 0;
		}

		ifdOfst += 12;
		i++;
	}

	if (nextOfst)
	{
		*nextOfst = (UInt32)readInt32(&ifdEntries[ifdCnt * 12]);
	}
	MemFree(ifdEntries);
	return exif;
}
