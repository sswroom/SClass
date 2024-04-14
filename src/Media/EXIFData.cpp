#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteOrderLSB.h"
#include "Data/ByteOrderMSB.h"
#include "Data/ByteTool.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/GeographicCoordinateSystem.h"
#include "Math/Math.h"
#include "Math/Size2DDbl.h"
#include "Media/EXIFData.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"

#include <stdio.h>
// http://hul.harvard.edu/jhove/tiff-tags.html

Media::EXIFData::EXIFInfo Media::EXIFData::defInfos[] = {
	{11, CSTR("ProcessingSoftware")},
	{254, CSTR("NewSubfileType")},
	{255, CSTR("SubfileType")},
	{256, CSTR("Width")},
	{257, CSTR("Height")},
	{258, CSTR("BitPerSample (R, G, B)")},
	{259, CSTR("Compression")},
	{262, CSTR("PhotometricInterpretation")},
	{263, CSTR("Threshholding")},
	{264, CSTR("CellWidth")},
	{265, CSTR("CellLength")},
	{266, CSTR("FillOrder")},
	{267, CSTR("DocumentName")},
	{270, CSTR("ImageDescription")},
	{271, CSTR("Make")},
	{272, CSTR("Model")},
	{273, CSTR("StripOffsets")},
	{274, CSTR("Orientation")},
	{277, CSTR("SamplesPerPixel")},
	{278, CSTR("RowsPerStrip")},
	{279, CSTR("StripByteCounts")},
	{280, CSTR("MinSampleValue")},
	{281, CSTR("MaxSampleValue")},
	{282, CSTR("XResolution")},
	{283, CSTR("YResolution")},
	{284, CSTR("PlanarConfiguration")},
	{285, CSTR("PageName")},
	{286, CSTR("XPosition")},
	{287, CSTR("YPosition")},
	{288, CSTR("FreeOffsets")},
	{289, CSTR("FreeByteCounts")},
	{290, CSTR("GrayResponseUnit")},
	{291, CSTR("GrayResponseCurve")},
	{292, CSTR("T4Options")},
	{293, CSTR("T6Options")},
	{296, CSTR("ResolutionUnit")},
	{297, CSTR("PageNumber")},
	{301, CSTR("TransferFunction")},
	{305, CSTR("Software")},
	{306, CSTR("DateTime")},
	{315, CSTR("Artist")},
	{316, CSTR("HostComputer")},
	{317, CSTR("Predictor")},
	{318, CSTR("WhitePoint")},
	{319, CSTR("PrimaryChromaticities")},
	{320, CSTR("ColorMap")},
	{321, CSTR("HalftoneHints")},
	{322, CSTR("TileWidth")},
	{323, CSTR("TileLength")},
	{324, CSTR("TileOffsets")},
	{325, CSTR("TileByteCounts")},
	{332, CSTR("InkSet")},
	{333, CSTR("InkNames")},
	{334, CSTR("NumberOfInks")},
	{336, CSTR("DotRange")},
	{337, CSTR("TargetPrinter")},
	{338, CSTR("ExtraSamples")},
	{339, CSTR("SampleFormat")},
	{340, CSTR("SMinSampleValue")},
	{341, CSTR("SMaxSampleValue")},
	{342, CSTR("TransferRange")},
	{343, CSTR("ClipPath")},
	{344, CSTR("XClipPathUnits")},
	{345, CSTR("YClipPathUnits")},
	{346, CSTR("Indexed")},
	{351, CSTR("OPIProxy")},
	{437, CSTR("JPEG tables")},
	{512, CSTR("JPEGProc")},
	{513, CSTR("JPEGInterchangeFormat")},
	{514, CSTR("JPEGInterchangeFormatLngth")},
	{515, CSTR("JPEGRestartInterval")},
	{517, CSTR("JPEGLosslessPredictors")},
	{518, CSTR("JPEGPointTransforms")},
	{519, CSTR("JPEGQTables")},
	{520, CSTR("JPEGDCTables")},
	{521, CSTR("JPEGACTables")},
	{529, CSTR("YCbCrCoefficients")},
	{530, CSTR("YCbCrSubSampling")},
	{531, CSTR("YCbCrPositioning")},
	{532, CSTR("ReferenceBlackWhite")},
	{700, CSTR("Photoshop XMP")},
	{32781, CSTR("ImageID")},
	{32995, CSTR("Matteing")},
	{32996, CSTR("DataType")},
	{32997, CSTR("ImageDepth")},
	{32998, CSTR("TileDepth")},
	{33421, CSTR("CFARepeatPatternDim")},
	{33422, CSTR("CFAPattern")},
	{33423, CSTR("BatteryLevel")},
	{33432, CSTR("Copyright")},
	{33434, CSTR("ExposureTime")},
	{33437, CSTR("Fnumber")},
	{33723, CSTR("IPTC/NAA")},
	{33550, CSTR("ModelPixelScaleTag")},
	{33920, CSTR("IntergraphMatrixTag")},
	{33922, CSTR("ModelTiepointTag")},
	{34016, CSTR("Site")},
	{34017, CSTR("ColorSequence")},
	{34018, CSTR("IT8Header")},
	{34019, CSTR("RasterPadding")},
	{34020, CSTR("BitsPerRunLength")},
	{34021, CSTR("BitsPerExtendedRunLength")},
	{34022, CSTR("ColorTable")},
	{34023, CSTR("ImageColorIndicator")},
	{34024, CSTR("BackgroundColorIndicator")},
	{34025, CSTR("ImageColorValue")},
	{34026, CSTR("BackgroundColorValue")},
	{34027, CSTR("PixelInensityRange")},
	{34028, CSTR("TransparencyIndicator")},
	{34029, CSTR("ColorCharacterization")},
	{34030, CSTR("HCUsage")},
	{34264, CSTR("ModelTransformationTag")},
	{34377, CSTR("PhotoshopImageResources")},
	{34665, CSTR("ExifIFD")},
	{34675, CSTR("InterColourProfile")},
	{34732, CSTR("ImageLayer")},
	{34735, CSTR("GeoKeyDirectoryTag")},
	{34736, CSTR("GeoDoubleParamsTag")},
	{34737, CSTR("GeoAsciiParamsTag")},
	{34850, CSTR("ExposureProgram")},
	{34852, CSTR("SpectralSensitivity")},
	{34853, CSTR("GPSInfo")},
	{34855, CSTR("ISOSpeedRatings")},
	{34856, CSTR("OECF")},
	{34857, CSTR("Interlace")},
	{34858, CSTR("TimeZoneOffset")},
	{34859, CSTR("SelfTimerMode")},
	{34908, CSTR("FaxRecvParams")},
	{34909, CSTR("FaxSubAddress")},
	{34910, CSTR("FaxRecvTime")},
	{36867, CSTR("DateTimeOriginal")},
	{37122, CSTR("CompressedBitsPerPixel")},
	{37377, CSTR("ShutterSpeedValue")},
	{37378, CSTR("ApertureValue")},
	{37379, CSTR("BrightnessValue")},
	{37380, CSTR("ExposureBiasValue")},
	{37381, CSTR("MaxApertureValue")},
	{37382, CSTR("SubjectDistance")},
	{37383, CSTR("MeteringMode")},
	{37384, CSTR("LightSource")},
	{37385, CSTR("Flash")},
	{37386, CSTR("FocalLength")},
	{37387, CSTR("FlashEnergy")},
	{37388, CSTR("SpatialFrequencyResponse")},
	{37389, CSTR("Noise")},
	{37390, CSTR("FocalPlaneXResolution")},
	{37391, CSTR("FocalPlaneYResolution")},
	{37392, CSTR("FocalPlaneResolutionUnit")},
	{37393, CSTR("ImageNumber")},
	{37394, CSTR("SecurityClassification")},
	{37395, CSTR("ImageHistory")},
	{37396, CSTR("SubjectLocation")},
	{37397, CSTR("ExposureIndex")},
	{37398, CSTR("TIFF/EPStandardID")},
	{37399, CSTR("SensingMethod")},
	{37439, CSTR("StoNits")},
	{37724, CSTR("ImageSourceData")},
	{40091, CSTR("XPTitle")},
	{40092, CSTR("XPComment")},
	{40093, CSTR("XPAuthor")},
	{40094, CSTR("XPKeywords")},
	{40095, CSTR("XPSubject")},
	{40965, CSTR("InteroperabilityIFD")},
	{41988, CSTR("DigitalZoomRatio")},
	{42016, CSTR("ImageUniqueID")},
	{50255, CSTR("PhotoshopAnnotations")},
	{50706, CSTR("DNGVersion")},
	{50707, CSTR("DNGBackwardVersion")},
	{50708, CSTR("UniqueCameraModel")},
	{50709, CSTR("LocalizedCameraModel")},
	{50710, CSTR("CFAPlaneColor")},
	{50711, CSTR("CFALayout")},
	{50712, CSTR("LinearizationTable")},
	{50713, CSTR("BlackLevelRepeatDim")},
	{50714, CSTR("BlackLevel")},
	{50715, CSTR("BlackLevelDeltaH")},
	{50716, CSTR("BlackLevelDeltaV")},
	{50717, CSTR("WhiteLevel")},
	{50718, CSTR("DefaultScale")},
	{50719, CSTR("DefaultCropOrigin")},
	{50720, CSTR("DefaultCropSize")},
	{50721, CSTR("ColorMatrix1")},
	{50722, CSTR("ColorMatrix2")},
	{50723, CSTR("CameraCalibration1")},
	{50724, CSTR("CameraCalibration2")},
	{50725, CSTR("ReductionMatrix1")},
	{50726, CSTR("ReductionMatrix2")},
	{50727, CSTR("AnalogBalnace")},
	{50728, CSTR("AsShortNeutral")},
	{50729, CSTR("AsShortWhiteXY")},
	{50730, CSTR("BaselineExposure")},
	{50731, CSTR("BaselineNoise")},
	{50732, CSTR("BaselineSharpness")},
	{50733, CSTR("BayerGreenSplit")},
	{50734, CSTR("LinearResponseLimit")},
	{50735, CSTR("CameraSerialNumber")},
	{50736, CSTR("LensInfo")},
	{50737, CSTR("ChromaBlurRadius")},
	{50738, CSTR("AntiAliasStrength")},
	{50740, CSTR("DNGPrivateData")},
	{50741, CSTR("MakerNoteSafety")},
	{50778, CSTR("CalibrationIlluminant1")},
	{50779, CSTR("CalibrationIlluminant2")},
	{50780, CSTR("BestQualityScale")},
	{59932, CSTR("Padding")}
};

Media::EXIFData::EXIFInfo Media::EXIFData::exifInfos[] = {
	{33434, CSTR("ExposureTime")},
	{33437, CSTR("FNumber")},
	{34850, CSTR("ExposureProgram")},
	{34852, CSTR("SpectralSensitibity")},
	{34855, CSTR("ISOSpeedRatings")},
	{34856, CSTR("OECF")},
	{36864, CSTR("ExifVersion")},
	{36867, CSTR("DateTimeOriginal")},
	{36868, CSTR("DateTimeDigitized")},
	{36880, CSTR("OffsetTime")},
	{36881, CSTR("OffsetTimeOriginal")},
	{36882, CSTR("OffsetTimeDigitized")},
	{37121, CSTR("ComponentsConfiguration")},
	{37122, CSTR("CompressedBitsPerPixel")},
	{37377, CSTR("ShutterSpeedValue")},
	{37378, CSTR("ApertureValue")},
	{37379, CSTR("BrightnessValue")},
	{37380, CSTR("ExposureBiasValue")},
	{37381, CSTR("MaxApertureValue")},
	{37382, CSTR("SubjectDistance")},
	{37383, CSTR("MeteringMode")},
	{37384, CSTR("LightSource")},
	{37385, CSTR("Flash")},
	{37386, CSTR("FocalLength")},
	{37396, CSTR("SubjectArea")},
	{37500, CSTR("MakerNote")},
	{37510, CSTR("UserComment")},
	{37520, CSTR("SubSecTime")},
	{37521, CSTR("SubSecTimeOriginal")},
	{37522, CSTR("SubSecTimeDigitized")},
	{37890, CSTR("Pressure")}, //hPa
	{37891, CSTR("WaterDepth")}, //m
	{37892, CSTR("Acceleration")}, //mGal
	{37893, CSTR("CameraElevationAngle")},
	{40960, CSTR("FlashpixVersion")},
	{40961, CSTR("ColorSpace")},
	{40962, CSTR("PixelXDimension")},
	{40963, CSTR("PixelYDimension")},
	{40964, CSTR("RelatedSoundFile")},
	{40965, CSTR("InteroperabilityIFD")},
	{41483, CSTR("FlashEnergy")},
	{41484, CSTR("SpatialFrequencyResponse")},
	{41486, CSTR("FocalPlaneXResolution")},
	{41487, CSTR("FocalPlaneYResolution")},
	{41488, CSTR("FocalPlaneResolutionUnit")},
	{41492, CSTR("SubjectLocation")},
	{41493, CSTR("ExposureIndex")},
	{41495, CSTR("SensingMethod")},
	{41728, CSTR("FileSource")},
	{41729, CSTR("SceneType")},
	{41730, CSTR("CFAPattern")},
	{41985, CSTR("CustomRendered")},
	{41986, CSTR("ExposureMode")},
	{41987, CSTR("WhiteBalance")},
	{41988, CSTR("DigitalZoomRatio")},
	{41989, CSTR("FocalLengthIn35mmFilm")},
	{41990, CSTR("SceneCaptureType")},
	{41991, CSTR("GainControl")},
	{41992, CSTR("Contrast")},
	{41993, CSTR("Saturation")},
	{41994, CSTR("Sharpness")},
	{41995, CSTR("DeviceSettingDescription")},
	{41996, CSTR("SubjectDistanceRange")},
	{42016, CSTR("ImageUniqueID")},
	{42032, CSTR("CameraOwnerName")},
	{42033, CSTR("BodySerialNumber")},
	{42034, CSTR("LensSpecification")},
	{42035, CSTR("LensMake")},
	{42036, CSTR("LensModel")},
	{42037, CSTR("LensSerialNumber")},
	{42080, CSTR("CompositeImage")},
	{42081, CSTR("SourceImageNumberOfCompositeImage")},
	{42082, CSTR("SourceExposureTimesOfCompositeImage")},
	{59932, CSTR("Padding")},
	{59933, CSTR("OffsetSchema")}
};

Media::EXIFData::EXIFInfo Media::EXIFData::gpsInfos[] = {
	{0, CSTR("GPSVersionID")},
	{1, CSTR("GPSLatitudeRef")},
	{2, CSTR("GPSLatitude")},
	{3, CSTR("GPSLongitudeRef")},
	{4, CSTR("GPSLongitude")},
	{5, CSTR("GPSAltitudeRef")},
	{6, CSTR("GPSAltitude")},
	{7, CSTR("GPSTimeStamp")},
	{8, CSTR("GPSSatellites")},
	{9, CSTR("GPSStatus")},
	{10, CSTR("GPSMeasureMode")},
	{11, CSTR("GPSDOP")},
	{12, CSTR("GPSSpeedRef")},
	{13, CSTR("GPSSpeed")},
	{14, CSTR("GPSTrackRef")},
	{15, CSTR("GPSTrack")},
	{16, CSTR("GPSImgDirectionRef")},
	{17, CSTR("GPSImgDirection")},
	{18, CSTR("GPSMapDatum")},
	{19, CSTR("GPSDestLatitudeRef")},
	{20, CSTR("GPSDestLatitude")},
	{21, CSTR("GPSDestLongitudeRef")},
	{22, CSTR("GPSDestLongitude")},
	{23, CSTR("GPSDestBearingRef")},
	{24, CSTR("GPSDestBearing")},
	{25, CSTR("GPSDestDistanceRef")},
	{26, CSTR("GPSDestDistance")},
	{27, CSTR("GPSProcessingMethod")},
	{28, CSTR("GPSAreaInformation")},
	{29, CSTR("GPSDateStamp")},
	{30, CSTR("GPSDifferential")},
	{31, CSTR("GPSHPositioningError")}
};

Media::EXIFData::EXIFInfo Media::EXIFData::panasonicInfos[] = {
	{1, CSTR("Quality")},
	{2, CSTR("FirmwareVersion")},
	{3, CSTR("WhiteBalance")},
	{7, CSTR("FocusMode")},
	{15, CSTR("AFMode")},
	{26, CSTR("ImageStabilization")},
	{28, CSTR("Macro")},
	{31, CSTR("ShootingMode")},
	{32, CSTR("Audio")},
	{36, CSTR("FlashBias")},
	{37, CSTR("InternalSerialNumber")},
	{38, CSTR("ExifVersion")},
	{40, CSTR("ColorEffect")},
	{41, CSTR("TimeSincePowerOn")},
	{42, CSTR("BurstMode")},
	{43, CSTR("SequenceNumber")},
	{44, CSTR("Contrast")},
	{45, CSTR("NoiseReduction")},
	{46, CSTR("SelfTimer")},
	{48, CSTR("Rotation")},
	{49, CSTR("AFAssistLamp")},
	{50, CSTR("ColorMode")},
	{51, CSTR("BabyAge1")},
	{52, CSTR("OpticalZoomMode")},
	{53, CSTR("ConversionLens")},
	{54, CSTR("TravelDay")},
	{57, CSTR("Contrast")},
	{58, CSTR("WorldTimeLocation")},
	{59, CSTR("TestStamp1")},
	{60, CSTR("ProgramISO")},
	{61, CSTR("AdvancedSceneType")},
	{62, CSTR("TextStampe2")},
	{63, CSTR("FacesDetected")},
	{64, CSTR("Saturation")},
	{65, CSTR("Sharpness")},
	{68, CSTR("ColorTempKelvin")},
	{69, CSTR("BracketSettings")},
	{70, CSTR("WBAdjustAB")},
	{71, CSTR("WBAdjustGM")},
	{72, CSTR("FlashCurtain")},
	{73, CSTR("LongShutterNoiseReduction")},
	{75, CSTR("ImageWidth")},
	{76, CSTR("ImageHeight")},
	{77, CSTR("AFPointPosition")},
	{78, CSTR("FaceDetInfo")},
	{81, CSTR("LensType")},
	{82, CSTR("LensSerialNumber")},
	{83, CSTR("AccessoryTyp")},
	{84, CSTR("AccessorySerialNumber")},
	{96, CSTR("LensFirmwareVersion")},
	{97, CSTR("FaceRecInfo")},
	{101, CSTR("Title")},
	{102, CSTR("BabyName")},
	{103, CSTR("Location")},
	{105, CSTR("Country")},
	{107, CSTR("State")},
	{109, CSTR("City")},
	{111, CSTR("Landmark")},
	{112, CSTR("IntelligentResolution")},
	{119, CSTR("BurstSpeed")},
	{121, CSTR("IntelligentDRange")},
	{124, CSTR("ClearRetouch")},
	{128, CSTR("City2")},
	{137, CSTR("PhotoStyle")},
	{138, CSTR("ShadingCompensation")},
	{140, CSTR("AccelerometerZ")},
	{141, CSTR("AccelerometerX")},
	{142, CSTR("AccelerometerY")},
	{143, CSTR("CameraOrientation")},
	{144, CSTR("RollAngle")},
	{145, CSTR("PitchAngle")},
	{147, CSTR("SweepPanoramaDirection")},
	{148, CSTR("PanoramaFieldOfView")},
	{150, CSTR("TimerRecording")},
	{157, CSTR("InternalNDFilter")},
	{158, CSTR("HDR")},
	{159, CSTR("ShutterType")},
	{163, CSTR("ClearRetouchValue")},
	{171, CSTR("TouchAE")},
	{32768, CSTR("MakerNoteVersion")},
	{32769, CSTR("SceneMode")},
	{32772, CSTR("WBRedLevel")},
	{32773, CSTR("WBGreenLevel")},
	{32774, CSTR("WBBlueLevel")},
	{32775, CSTR("FlashFired")},
	{32776, CSTR("TextStamp3")},
	{32777, CSTR("TextStamp4")},
	{32784, CSTR("babyAge2")}
};

Media::EXIFData::EXIFInfo Media::EXIFData::canonInfos[] = {
	{1, CSTR("CanonCameraSettings")},
	{2, CSTR("CanonFocalLength")},
	{3, CSTR("CanonFlashInfo")},
	{4, CSTR("CanonShotInfo")},
	{5, CSTR("CanonPanorama")},
	{6, CSTR("CanonImageType")},
	{7, CSTR("CanonFirmwareVersion")},
	{8, CSTR("FileNumber")},
	{9, CSTR("OwnerName")},
	{10, CSTR("UnknownD30")},
	{12, CSTR("SerialNumber")},
	{13, CSTR("CanonCameraInfo")},
	{14, CSTR("CanonFileLength")},
	{15, CSTR("CustomFunctions")},
	{16, CSTR("CanonModelID")},
	{17, CSTR("MovieInfo")},
	{18, CSTR("CanonAFInfo")},
	{19, CSTR("ThumbnailImageValidArea")},
	{21, CSTR("SerialNumberFormat")},
	{26, CSTR("SuperMacro")},
	{28, CSTR("DateStampMode")},
	{29, CSTR("MyColors")},
	{30, CSTR("FirmwareRevision")},
	{35, CSTR("FaceDetect1")},
	{36, CSTR("FaceDetect2")},
	{37, CSTR("Categories")},
	{38, CSTR("CanonAFInfo2")},
	{39, CSTR("ContrastInfo")},
	{40, CSTR("ImageUniqueID")},
	{47, CSTR("FaceDetect3")},
	{53, CSTR("TimeInfo")},
	{56, CSTR("BatteryType")},
	{60, CSTR("AFInfo3")},
	{129, CSTR("RawDataOffset")},
	{131, CSTR("OriginalDecisionData")},
	{144, CSTR("CustomFunctions1D")},
	{145, CSTR("PersonalFunctions")},
	{146, CSTR("PersonalFunctionValues")},
	{147, CSTR("CanonFileInfo")},
	{148, CSTR("AFPointsInFocus1D")},
	{149, CSTR("LensModel")},
	{150, CSTR("InternalSerialNumber")},
	{151, CSTR("DustRemovalData")},
	{152, CSTR("CropInfo")},
	{153, CSTR("CustomFunctions2")},
	{154, CSTR("AspectInfo")},
	{160, CSTR("ProcessingInfo")},
	{161, CSTR("ToneCurveTable")},
	{162, CSTR("SharpnessTable")},
	{163, CSTR("SharpnessFreqTable")},
	{164, CSTR("WhiteBalanceTable")},
	{169, CSTR("ColorBalance")},
	{170, CSTR("MeasuredColor")},
	{174, CSTR("ColorTemperature")},
	{176, CSTR("CanonFlags")},
	{177, CSTR("ModifiedInfo")},
	{178, CSTR("ToneCurveMatching")},
	{179, CSTR("WhiteBalanceMatching")},
	{180, CSTR("ColorSpace")},
	{182, CSTR("PreviewImageInfo")},
	{208, CSTR("VROffset")},
	{224, CSTR("SensorInfo")},
	{0x4001, CSTR("ColorBalance")},
	{0x4002, CSTR("UnknownBlock1")},
	{0x4003, CSTR("ColorInfo")},
	{0x4005, CSTR("Flavor")},
	{0x4008, CSTR("PictureStyleUserDef")},
	{0x4009, CSTR("PictureStylePC")},
	{0x4010, CSTR("CustomPictureStyleFileName")},
	{0x4013, CSTR("AFMicroAdj")},
	{0x4015, CSTR("VignettingCorr")},
	{0x4016, CSTR("VignettingCorr2")},
	{0x4018, CSTR("LightingOpt")},
	{0x4019, CSTR("LensInfo")},
	{0x4020, CSTR("AmbienceInfo")},
	{0x4021, CSTR("MultiExp")},
	{0x4024, CSTR("FilterInfo")},
	{0x4025, CSTR("HDRInfo")},
	{0x4028, CSTR("AFConfig")}
};

Media::EXIFData::EXIFInfo Media::EXIFData::olympusInfos[] = {
	{0x0000, CSTR("MakerNoteVersion")},
	{0x0001, CSTR("MinoltaCameraSettingsOld")},
	{0x0003, CSTR("MinoltaCameraSettings")},
	{0x0040, CSTR("CompressedImageSize")},
	{0x0081, CSTR("PreviewImageData")},
	{0x0088, CSTR("PreviewImageStart")},
	{0x0089, CSTR("PreviewImageLength")},
	{0x0100, CSTR("ThumbnailImage")},
	{0x0104, CSTR("BodyFirmwareVersion")},
	{0x0200, CSTR("SpecialMode")},
	{0x0201, CSTR("Quality")},
	{0x0202, CSTR("Macro")},
	{0x0203, CSTR("BWMode")},
	{0x0204, CSTR("DigitalZoom")},
	{0x0205, CSTR("FocalPlaneDiagonal")},
	{0x0206, CSTR("LensDistortionParmas")},
	{0x0207, CSTR("CameraType")},
	{0x0208, CSTR("CameraInfo")},
	{0x0209, CSTR("CameraID")},
	{0x020b, CSTR("EpsonImageWidth")},
	{0x020c, CSTR("EpsonImageHeight")},
	{0x020d, CSTR("EpsonSoftware")},
	{0x0280, CSTR("PreviewImage")},
	{0x0300, CSTR("PreCaptureFrames")},
	{0x0301, CSTR("WhiteBoard")},
	{0x0302, CSTR("OneTouchWB")},
	{0x0303, CSTR("WhiteBalanceBracket")},
	{0x0304, CSTR("WhiteBalanceBias")},
	{0x0401, CSTR("BlackLevel")},
	{0x0403, CSTR("SceneMode")},
	{0x0404, CSTR("SerialNumber")},
	{0x0405, CSTR("Firmware")},
	{0x0e00, CSTR("PrintIM")},
	{0x0f00, CSTR("DataDump")},
	{0x0f01, CSTR("DataDump2")},
	{0x0f04, CSTR("ZoomedPreviewStart")},
	{0x0f05, CSTR("ZoomedPreviewLength")},
	{0x0f06, CSTR("ZoomedPreviewSize")},
	{0x1000, CSTR("ShutterSpeedValue")},
	{0x1001, CSTR("ISOValue")},
	{0x1002, CSTR("ApertureValue")},
	{0x1003, CSTR("BrightnessValue")},
	{0x1004, CSTR("FlashMode")},
	{0x1005, CSTR("FlashDevice")},
	{0x1006, CSTR("ExposureCompensation")},
	{0x1007, CSTR("SensorTemperature")},
	{0x1008, CSTR("LensTemperature")},
	{0x1009, CSTR("LightCondition")},
	{0x100a, CSTR("FocusRange")},
	{0x100b, CSTR("FocusMode")},
	{0x100c, CSTR("ManualFocusDistance")},
	{0x100d, CSTR("ZoomStepCount")},
	{0x100e, CSTR("FocusStepCount")},
	{0x100f, CSTR("Sharpness")},
	{0x1010, CSTR("FlashChargeLevel")},
	{0x1011, CSTR("ColorMatrix")},
	{0x1012, CSTR("BlackLevel")},
	{0x1013, CSTR("ColorTemperatureBG")},
	{0x1014, CSTR("ColorTemperatureRG")},
	{0x1015, CSTR("WBMode")},
	{0x1017, CSTR("RedBalance")},
	{0x1018, CSTR("BlueBalance")},
	{0x1019, CSTR("ColorMatrixNumber")},
	{0x101a, CSTR("SerialNumber")},
	{0x101b, CSTR("ExternalFlashAE1_0")},
	{0x101c, CSTR("ExternalFlashAE2_0")},
	{0x101d, CSTR("InternalFlashAE1_0")},
	{0x101e, CSTR("InternalFlashAE2_0")},
	{0x101f, CSTR("ExternalFlashAE1")},
	{0x1020, CSTR("ExternalFlashAE2")},
	{0x1021, CSTR("InternalFlashAE1")},
	{0x1022, CSTR("InternalFlashAE2")},
	{0x1023, CSTR("FlashExposureComp")},
	{0x1024, CSTR("InternalFlashTable")},
	{0x1025, CSTR("ExternalFlashGValue")},
	{0x1026, CSTR("ExternalFlashBounce")},
	{0x1027, CSTR("ExternalFlashZoom")},
	{0x1028, CSTR("ExternalFlashMode")},
	{0x1029, CSTR("Contrast")},
	{0x102a, CSTR("SharpnessFactor")},
	{0x102b, CSTR("ColorControl")},
	{0x102c, CSTR("ValidBits")},
	{0x102d, CSTR("CoringFilter")},
	{0x102e, CSTR("OlympusImageWidth")},
	{0x102f, CSTR("OlympusImageHeight")},
	{0x1030, CSTR("SceneDetect")},
	{0x1031, CSTR("SceneArea")},
	{0x1033, CSTR("SceneDetectData")},
	{0x1034, CSTR("CompressionRatio")},
	{0x1035, CSTR("PreviewImageValid")},
	{0x1036, CSTR("PreviewImageStart")},
	{0x1037, CSTR("PreviewImageLength")},
	{0x1038, CSTR("AFResult")},
	{0x1039, CSTR("CCDScanMode")},
	{0x103a, CSTR("NoiseReduction")},
	{0x103b, CSTR("FocusStepInfinity")},
	{0x103c, CSTR("FocusStepNear")},
	{0x103d, CSTR("LightValueCenter")},
	{0x103e, CSTR("LightValuePeriphery")},
	{0x103f, CSTR("FieldCount")},
	{0x2010, CSTR("Equipment")},
	{0x2020, CSTR("CameraSettings")},
	{0x2030, CSTR("RawDevelopment")},
	{0x2031, CSTR("RawDev2")},
	{0x2040, CSTR("ImageProcessing")},
	{0x2050, CSTR("FocusInfo")},
	{0x2100, CSTR("Olympus2100")},
	{0x2200, CSTR("Olympus2200")},
	{0x2300, CSTR("Olympus2300")},
	{0x2400, CSTR("Olympus2400")},
	{0x2500, CSTR("Olympus2500")},
	{0x2600, CSTR("Olympus2600")},
	{0x2700, CSTR("Olympus2700")},
	{0x2800, CSTR("Olympus2800")},
	{0x2900, CSTR("Olympus2900")},
	{0x3000, CSTR("RawInfo")},
	{0x4000, CSTR("MainInfo")},
	{0x5000, CSTR("UnknownInfo")}
};

Media::EXIFData::EXIFInfo Media::EXIFData::olympus2010Infos[] = {
	{0x0000, CSTR("EquipmentVersion")},
	{0x0100, CSTR("CameraType2")},
	{0x0101, CSTR("SerialNumber")},
	{0x0102, CSTR("InternalSerialNumber")},
	{0x0103, CSTR("FocalPlaneDiagonal")},
	{0x0104, CSTR("BodyFirmwareVersion")},
	{0x0201, CSTR("LensType")},
	{0x0202, CSTR("LensSerialNumber")},
	{0x0203, CSTR("LensModel")},
	{0x0204, CSTR("LensFirmwareVersion")},
	{0x0205, CSTR("MaxApertureAtMinFocal")},
	{0x0206, CSTR("MaxApertureAtMaxFocal")},
	{0x0207, CSTR("MinFocalLength")},
	{0x0208, CSTR("MaxFocalLength")},
	{0x020a, CSTR("MaxAperture")},
	{0x020b, CSTR("LensProperties")},
	{0x0301, CSTR("Extender")},
	{0x0302, CSTR("ExtenderSerialNumber")},
	{0x0303, CSTR("ExtenderModel")},
	{0x0304, CSTR("ExtenderFirmwareVersion")},
	{0x0403, CSTR("ConversionLens")},
	{0x1000, CSTR("FlashType")},
	{0x1001, CSTR("FlashModel")},
	{0x1002, CSTR("FlashFirmwareVersion")},
	{0x1003, CSTR("FlashSerialNumber")}
};

Media::EXIFData::EXIFInfo Media::EXIFData::olympus2020Infos[] = {
	{0x0000, CSTR("CameraSettingsVersion")},
	{0x0100, CSTR("PreviewImageValid")},
	{0x0101, CSTR("PreviewImageStart")},
	{0x0102, CSTR("PreviewImageLength")},
	{0x0200, CSTR("ExposureMode")},
	{0x0201, CSTR("AELock")},
	{0x0202, CSTR("MeteringMode")},
	{0x0203, CSTR("ExposureShift")},
	{0x0204, CSTR("NDFilter")},
	{0x0300, CSTR("MacroMode")},
	{0x0301, CSTR("FocusMode")},
	{0x0302, CSTR("FocusProcess")},
	{0x0303, CSTR("AFSearch")},
	{0x0304, CSTR("AFAreas")},
	{0x0305, CSTR("AFPointSelected")},
	{0x0306, CSTR("AFFineTune")},
	{0x0307, CSTR("AFFineTuneAdj")},
	{0x0400, CSTR("FlashMode")},
	{0x0401, CSTR("FlashExposureComp")},
	{0x0403, CSTR("FlashRemoteControl")},
	{0x0404, CSTR("FlashControlMode")},
	{0x0405, CSTR("FlashIntensity")},
	{0x0406, CSTR("ManualFlashStrength")},
	{0x0500, CSTR("WhiteBalance2")},
	{0x0501, CSTR("WhiteBalanceTemperature")},
	{0x0502, CSTR("WhiteBalanceBracket")},
	{0x0503, CSTR("CustomSaturation")},
	{0x0504, CSTR("ModifiedSaturation")},
	{0x0505, CSTR("ContrastSetting")},
	{0x0506, CSTR("SharpnessSetting")},
	{0x0507, CSTR("ColorSpace")},
	{0x0509, CSTR("SceneMode")},
	{0x050a, CSTR("NoiseReduction")},
	{0x050b, CSTR("DistortionCorrection")},
	{0x050c, CSTR("ShadingCompensation")},
	{0x050d, CSTR("CompressionFactor")},
	{0x050f, CSTR("Gradation")},
	{0x0520, CSTR("PictureMode")},
	{0x0521, CSTR("PictureModeSaturation")},
	{0x0522, CSTR("PictureModeHue")},
	{0x0523, CSTR("PictureModeContrast")},
	{0x0524, CSTR("PictureModeSharpness")},
	{0x0525, CSTR("PictureModeBWFilter")},
	{0x0526, CSTR("PictureModeTone")},
	{0x0527, CSTR("NoiseFilter")},
	{0x0529, CSTR("ArtFilter")},
	{0x052c, CSTR("MagicFilter")},
	{0x052d, CSTR("PictureModeEffect")},
	{0x052e, CSTR("ToneLevel")},
	{0x052f, CSTR("ArtFilterEffect")},
	{0x0532, CSTR("ColorCreatorEffect")},
	{0x0537, CSTR("MonochromeProfileSettings")},
	{0x0538, CSTR("FilmGrainEffect")},
	{0x0539, CSTR("ColorProfileSettings")},
	{0x053a, CSTR("MonochromeVignetting")},
	{0x053b, CSTR("MonochromeColor")},
	{0x0600, CSTR("DriveMode")},
	{0x0601, CSTR("PanoramaMode")},
	{0x0603, CSTR("ImageQuality2")},
	{0x0604, CSTR("ImageStabilization")},
	{0x0804, CSTR("StackedImage")},
	{0x0900, CSTR("ManometerPressure")},
	{0x0901, CSTR("ManometerReading")},
	{0x0902, CSTR("ExtendedWBDetect")},
	{0x0903, CSTR("RollAngle")},
	{0x0904, CSTR("PitchAngle")},
	{0x0908, CSTR("DateTimeUTC")}
};

Media::EXIFData::EXIFInfo Media::EXIFData::olympus2030Infos[] = {
	{0x0000, CSTR("RawDevVersion")},
	{0x0100, CSTR("RawDevExposureBiasValue")},
	{0x0101, CSTR("RawDevWhiteBalanceValue")},
	{0x0102, CSTR("RawDevWBFineAdjustment")},
	{0x0103, CSTR("RawDevGrayPoint")},
	{0x0104, CSTR("RawDevSaturationEmphasis")},
	{0x0105, CSTR("RawDevMemoryColorEmphasis")},
	{0x0106, CSTR("RawDevContrastValue")},
	{0x0107, CSTR("RawDevSharpnessValue")},
	{0x0108, CSTR("RawDevColorSpace")},
	{0x0109, CSTR("RawDevEngine")},
	{0x010a, CSTR("RawDevNoiseReduction")},
	{0x010b, CSTR("RawDevEditStatus")},
	{0x010c, CSTR("RawDevSettings")}
};

Media::EXIFData::EXIFInfo Media::EXIFData::olympus2040Infos[] = {
	{0x0000, CSTR("ImageProcessingVersion")},
	{0x0100, CSTR("WB_RBLevels")},
	{0x0102, CSTR("WB_RBLevels3000K")},
	{0x0103, CSTR("WB_RBLevels3300K")},
	{0x0104, CSTR("WB_RBLevels3600K")},
	{0x0105, CSTR("WB_RBLevels3900K")},
	{0x0106, CSTR("WB_RBLevels4000K")},
	{0x0107, CSTR("WB_RBLevels4300K")},
	{0x0108, CSTR("WB_RBLevels4500K")},
	{0x0109, CSTR("WB_RBLevels4800K")},
	{0x010a, CSTR("WB_RBLevels5300K")},
	{0x010b, CSTR("WB_RBLevels6000K")},
	{0x010c, CSTR("WB_RBLevels6600K")},
	{0x010d, CSTR("WB_RBLevels7500K")},
	{0x010e, CSTR("WB_RBLevelsCWB1")},
	{0x010f, CSTR("WB_RBLevelsCWB2")},
	{0x0110, CSTR("WB_RBLevelsCWB3")},
	{0x0111, CSTR("WB_RBLevelsCWB4")},
	{0x0113, CSTR("WB_GLevel3000K")},
	{0x0114, CSTR("WB_GLevel3300K")},
	{0x0115, CSTR("WB_GLevel3600K")},
	{0x0116, CSTR("WB_GLevel3900K")},
	{0x0117, CSTR("WB_GLevel4000K")},
	{0x0118, CSTR("WB_GLevel4300K")},
	{0x0119, CSTR("WB_GLevel4500K")},
	{0x011a, CSTR("WB_GLevel4800K")},
	{0x011b, CSTR("WB_GLevel5300K")},
	{0x011c, CSTR("WB_GLevel6000K")},
	{0x011d, CSTR("WB_GLevel6600K")},
	{0x011e, CSTR("WB_GLevel7500K")},
	{0x011f, CSTR("WB_GLevel")},
	{0x0200, CSTR("ColorMatrix")},
	{0x0300, CSTR("Enhancer")},
	{0x0301, CSTR("EnhancerValues")},
	{0x0310, CSTR("CoringFilter")},
	{0x0311, CSTR("CoringValues")},
	{0x0600, CSTR("BlackLevel2")},
	{0x0610, CSTR("GainBase")},
	{0x0611, CSTR("ValidBits")},
	{0x0612, CSTR("CropLeft")},
	{0x0613, CSTR("CropTop")},
	{0x0614, CSTR("CropWidth")},
	{0x0615, CSTR("CropHeight")},
	{0x0635, CSTR("UnknownBlock1")},
	{0x0636, CSTR("UnknownBlock2")},
	{0x0805, CSTR("SensorCalibration")},
	{0x1010, CSTR("NoiseReduction2")},
	{0x1011, CSTR("DistortionCorrection2")},
	{0x1012, CSTR("ShadingCompensation2")},
	{0x101c, CSTR("MultipleExposureMode")},
	{0x1103, CSTR("UnknownBlock3")},
	{0x1104, CSTR("UnknownBlock4")},
	{0x1112, CSTR("AspectRatio")},
	{0x1113, CSTR("AspectFrame")},
	{0x1200, CSTR("FacesDetected")},
	{0x1201, CSTR("FaceDetectArea")},
	{0x1202, CSTR("MaxFaces")},
	{0x1203, CSTR("FaceDetectFrameSize")},
	{0x1207, CSTR("FaceDetectFrameCrop")},
	{0x1306, CSTR("CameraTemperature")},
	{0x1900, CSTR("KeystoneCompensation")},
	{0x1901, CSTR("KeystoneDirection")},
	{0x1906, CSTR("KeystoneValue")}
};

Media::EXIFData::EXIFInfo Media::EXIFData::olympus2050Infos[] = {
	{0x0000, CSTR("FocusInfoVersion")},
	{0x0209, CSTR("AutoFocus")},
	{0x0210, CSTR("SceneDetect")},
	{0x0211, CSTR("SceneArea")},
	{0x0212, CSTR("SceneDetectData")},
	{0x0300, CSTR("ZoomStepCount")},
	{0x0301, CSTR("FocusStepCount")},
	{0x0303, CSTR("FocusStepInfinity")},
	{0x0304, CSTR("FocusStepNear")},
	{0x0305, CSTR("FocusDistance")},
	{0x0308, CSTR("AFPoint")},
	{0x0328, CSTR("AFInfo")},
	{0x1201, CSTR("ExternalFlash")},
	{0x1203, CSTR("ExternalFlashGuideNumber")},
	{0x1204, CSTR("ExternalFlashBounce")},
	{0x1205, CSTR("ExternalFlashZoom")},
	{0x1208, CSTR("InternalFlash")},
	{0x1209, CSTR("ManualFlash")},
	{0x120a, CSTR("MacroLED")},
	{0x1500, CSTR("SensorTemperature")},
	{0x1600, CSTR("ImageStabilization")}
};

Media::EXIFData::EXIFInfo Media::EXIFData::casio1Infos[] = {
	{1, CSTR("RecordingMode")},
	{2, CSTR("Quality")},
	{3, CSTR("FocusingMode")},
	{4, CSTR("FlashMode")},
	{5, CSTR("FlashIntensity")},
	{6, CSTR("ObjectDistance")},
	{7, CSTR("WhiteBalance")},
	{10, CSTR("DigitalZoom")},
	{11, CSTR("Sharpness")},
	{12, CSTR("Contract")},
	{13, CSTR("Saturation")},
	{20, CSTR("CCDSensitivity")}
};

Media::EXIFData::EXIFInfo Media::EXIFData::casio2Infos[] = {
	{2, CSTR("PreviewThumbDimension")},
	{3, CSTR("PreviewThumbSize")},
	{4, CSTR("PreviewThumbOffset")},
	{8, CSTR("QualityMode")},
	{9, CSTR("ImageSize")},
	{13, CSTR("FocusMode")},
	{20, CSTR("IsoSensitivity")},
	{25, CSTR("WhiteBalance")},
	{29, CSTR("FocalLength")},
	{31, CSTR("Saturation")},
	{32, CSTR("Contrast")},
	{33, CSTR("Sharpness")},
	{0x0E00, CSTR("PIM")},
	{0x2000, CSTR("CasioPreviewThumbnail")},
	{0x2011, CSTR("WhiteBalanceBias")},
	{0x2012, CSTR("WhiteBalance")},
	{0x2022, CSTR("ObjectDistance")},
	{0x2034, CSTR("FlashDistance")},
	{0x3000, CSTR("RecordMode")},
	{0x3001, CSTR("SelfTimer")},
	{0x3002, CSTR("Quality")},
	{0x3003, CSTR("FocusMode")},
	{0x3006, CSTR("TimeZone")},
	{0x3007, CSTR("BestshotMode")},
	{0x3014, CSTR("CCDISOSensitivity")},
	{0x3015, CSTR("ColourMode")},
	{0x3016, CSTR("Enhancement")},
	{0x3017, CSTR("Filter")}
};

Media::EXIFData::EXIFInfo Media::EXIFData::flirInfos[] = {
	{1, CSTR("RTemp")},
	{2, CSTR("ATemp")},
	{3, CSTR("Emissivity")},
	{4, CSTR("IRWTemp")},
	{5, CSTR("CameraTemperatureRangeMax")},
	{6, CSTR("CameraTemperatureRangeMin")},
	{7, CSTR("Unknown")},
	{8, CSTR("Unknown")},
	{9, CSTR("Unknown")},
	{10, CSTR("Unknown")},
	{274, CSTR("Unknown")}
};

Media::EXIFData::EXIFInfo Media::EXIFData::nikon3Infos[] = {
	{1, CSTR("Version")},
	{2, CSTR("ISOSpeed")},
	{3, CSTR("ColourMode")},
	{4, CSTR("Quality")},
	{5, CSTR("WhiteBalance")},
	{6, CSTR("Sharpening")},
	{7, CSTR("Focus")},
	{8, CSTR("FlashSetting")},
	{9, CSTR("FlashDevice")},
	{11, CSTR("WhiteBalanceBias")},
	{12, CSTR("WB_RBLevels")},
	{13, CSTR("ProgramShift")},
	{14, CSTR("ExposureDiff")},
	{15, CSTR("ISOSelection")},
	{16, CSTR("DataDump")},
	{17, CSTR("Preview")},
	{18, CSTR("FlashComp")},
	{19, CSTR("ISOSettings")},
	{22, CSTR("ImageBoundary")},
	{23, CSTR("FlashExposureComp")},
	{24, CSTR("FlashBracketComp")},
	{25, CSTR("ExposureBracketComp")},
	{26, CSTR("ImageProcessing")},
	{27, CSTR("CropHiSpeed")},
	{28, CSTR("ExposureTuning")},
	{29, CSTR("SerialNumber")},
	{30, CSTR("ColorSpace")},
	{31, CSTR("VRInfo")},
	{32, CSTR("ImageAuthentication")},
	{34, CSTR("ActiveDLighting")},
	{35, CSTR("PictureControl")},
	{36, CSTR("WorldTime")},
	{37, CSTR("ISOInfo")},
	{42, CSTR("VignetteControl")},
	{52, CSTR("ShutterMode")},
	{55, CSTR("MechanicalShutterCount")},
	{128, CSTR("ImageAdjustment")},
	{129, CSTR("ToneComp")},
	{130, CSTR("AuxiliaryLens")},
	{131, CSTR("LensType")},
	{132, CSTR("Lens")},
	{133, CSTR("FocusDistance")},
	{134, CSTR("DigitalZoom")},
	{135, CSTR("FlashMode")},
	{136, CSTR("AFInfo")},
	{137, CSTR("ShootingMode")},
	{138, CSTR("AutoBracketRelease")},
	{139, CSTR("LensFStops")},
	{140, CSTR("ContrastCurve")},
	{141, CSTR("ColorHue")},
	{143, CSTR("SceneMode")},
	{144, CSTR("LightSource")},
	{145, CSTR("ShotInfo")},
	{146, CSTR("HueAdjustment")},
	{147, CSTR("NEFCompression")},
	{148, CSTR("Saturation")},
	{149, CSTR("NoiseReduction")},
	{150, CSTR("LinearizationTable")},
	{151, CSTR("ColorBalance")},
	{152, CSTR("LensData")},
	{153, CSTR("RawImageCenter")},
	{154, CSTR("SensorPixelSize")},
	{156, CSTR("SceneAssist")},
	{158, CSTR("RetouchHistory")},
	{160, CSTR("SerialNO")},
	{162, CSTR("ImageDataSize")},
	{165, CSTR("ImageCount")},
	{166, CSTR("DeletedImageCount")},
	{167, CSTR("ShutterCount")},
	{168, CSTR("FlashInfo")},
	{169, CSTR("ImageOptimisation")},
	{170, CSTR("Saturation")},
	{171, CSTR("VariProgram")},
	{172, CSTR("ImageStabilization")},
	{173, CSTR("AFResponse")},
	{176, CSTR("MultiExposure")},
	{177, CSTR("HighISONoiseReduction")},
	{179, CSTR("ToningEffect")},
	{183, CSTR("AFInfo2")},
	{184, CSTR("FileInfo")},
	{185, CSTR("AFTune")},
	{195, CSTR("BarometerInfo")},
	{3584, CSTR("PrintIM")},
	{3585, CSTR("CaptureData")},
	{3593, CSTR("CaptureVersion")},
	{3598, CSTR("CaptureOffsets")},
	{3600, CSTR("ScanIFD")},
	{3613, CSTR("ICCProfile")},
	{3614, CSTR("CaptureOutput")}
};

Media::EXIFData::EXIFInfo Media::EXIFData::sanyo1Infos[] = {
	{0x0000, CSTR("Unknown")},
	{0x0001, CSTR("Version")},
	{0x0088, CSTR("Thumbnail Offset")},
	{0x0089, CSTR("Thumbnail Length")},
	{0x00FF, CSTR("Makernote Offset")},
	{0x0100, CSTR("Jpeg Thumbnail")},
	{0x0200, CSTR("Special Mode")},
	{0x0201, CSTR("Jpeg Quality")},
	{0x0202, CSTR("Macro")},
	{0x0203, CSTR("Sanyo-1-0x0203")},
	{0x0204, CSTR("Digital Zoom")},
	{0x0207, CSTR("Software Version")},
	{0x0208, CSTR("Picture Info")},
	{0x0209, CSTR("Camera ID")},
	{0x020E, CSTR("Sequential Shot Method")},
	{0x020F, CSTR("Wide Range")},
	{0x0210, CSTR("Color Adjustment Mode")},
	{0x0211, CSTR("Sanyo-1-0x0211")},
	{0x0212, CSTR("Sanyo-1-0x0212")},
	{0x0213, CSTR("Quick Shot")},
	{0x0214, CSTR("Self Timer")},
	{0x0215, CSTR("Sanyo-1-0x0215")},
	{0x0216, CSTR("Voice Memo")},
	{0x0217, CSTR("Record Shutter Release")},
	{0x0218, CSTR("Flicker Reduce")},
	{0x0219, CSTR("Optical Zoom")},
	{0x021A, CSTR("Sanyo-1-0x021a")},
	{0x021B, CSTR("Digital Zoom")},
	{0x021C, CSTR("Sanyo-1-0x021c")},
	{0x021D, CSTR("Light Source Special")},
	{0x021E, CSTR("Resaved")},
	{0x021F, CSTR("Scene Select")},
	{0x0220, CSTR("Sanyo-1-0x0220")},
	{0x0221, CSTR("Sanyo-1-0x0221")},
	{0x0222, CSTR("Sanyo-1-0x0222")},
	{0x0223, CSTR("Manual Focal Distance")},
	{0x0224, CSTR("Sequential Shot Interval")},
	{0x0225, CSTR("Flash Mode")},
	{0x0226, CSTR("Sanyo-1-0x0226")},
	{0x0300, CSTR("Sanyo-1-0x0300")},
	{0x0E00, CSTR("Print IM Data")},
	{0x0F00, CSTR("Data Dump")}
};

Media::EXIFData::EXIFInfo Media::EXIFData::appleInfos[] = {
	{0x0001, CSTR("MakerNoteVersion")},
	{0x0002, CSTR("AEMatrix?")},
	{0x0003, CSTR("RunTime")},
	{0x0004, CSTR("AEStable")},
	{0x0005, CSTR("AETarget")},
	{0x0006, CSTR("AEAverage")},
	{0x0007, CSTR("AFStable")},
	{0x0008, CSTR("FocusAccelerometerVector")},
	{0x0009, CSTR("SISMethod")},
	{0x000a, CSTR("HDRMethod")},
	{0x000b, CSTR("BurstUUID")},
	{0x000c, CSTR("SphereHealthTrackingError")},
	{0x000d, CSTR("SphereHealthAverageCurrent")},
	{0x000e, CSTR("SphereMotionDataStatus")},
	{0x000f, CSTR("OISMode")},
	{0x0010, CSTR("SphereStatus")},
	{0x0011, CSTR("AssetIdentifier")},
	{0x0012, CSTR("QRMOutputType")},
	{0x0013, CSTR("SphereExternalForceOffset")},
	{0x0014, CSTR("StillImageCaptureType")},
	{0x0015, CSTR("ImageGroupIdentifier")},
	{0x0016, CSTR("PhotosOriginatingSignature")},
	{0x0017, CSTR("LivePhotoVideoIndex")},
	{0x0018, CSTR("PhotosRenderOriginatingSignature")},
	{0x0019, CSTR("StillImageProcessingFlags")},
	{0x001a, CSTR("PhotoTranscodeQualityHint")},
	{0x001b, CSTR("PhotosRenderEffect")},
	{0x001c, CSTR("BracketedCaptureSequenceNumber")},
	{0x001d, CSTR("LuminanceNoiseAmplitude?")},
	{0x001e, CSTR("OriginatingAppID?")},
	{0x001f, CSTR("PhotosAppFeatureFlags")},
	{0x0020, CSTR("ImageCaptureRequestIdentifier")},
	{0x0021, CSTR("MeteorHeadroom")},
	{0x0022, CSTR("ARKitPhoto")},
	{0x0023, CSTR("AFPerformance")},
	{0x0024, CSTR("AFExternalOffset")},
	{0x0025, CSTR("StillImageSceneFlags")},
	{0x0026, CSTR("StillImageSNRType")},
	{0x0027, CSTR("StillImageSNR")},
	{0x0028, CSTR("UBMethod")},
	{0x0029, CSTR("SpatialOverCaptureGroupIdentifier")},
	{0x002a, CSTR("iCloudServerSoftwareVersionForDynamicallyGeneratedMedia")},
	{0x002b, CSTR("PhotoIdentifier")},
	{0x002c, CSTR("SpatialOverCaptureImageType")},
	{0x002d, CSTR("CCT")},
	{0x002e, CSTR("ApsMode")},
	{0x002f, CSTR("FocusPosition")},
	{0x0030, CSTR("MeteorPlusGainMap")},
	{0x0031, CSTR("StillImageProcessingHomography")},
	{0x0032, CSTR("IntelligentDistortionCorrection")},
	{0x0033, CSTR("NRFStatus")},
	{0x0034, CSTR("NRFInputBracketCount")},
	{0x0035, CSTR("NRFRegisteredBracketCount")},
	{0x0036, CSTR("LuxLevel")},
	{0x0037, CSTR("LastFocusingMethod")},
	{0x0038, CSTR("TimeOfFlightAssistedAutoFocusEstimatorMeasuredDepth")},
	{0x0039, CSTR("TimeOfFlightAssistedAutoFocusEstimatorROIType")},
	{0x003a, CSTR("NRFSRLStatus")},
	{0x003b, CSTR("SystemPressureLevel")},
	{0x003c, CSTR("CameraControlsStatisticsMaster")},
	{0x003d, CSTR("TimeOfFlightAssistedAutoFocusEstimatorSensorConfidence")},
	{0x003e, CSTR("ColorCorrectionMatrix?")},
	{0x003f, CSTR("GreenGhostMitigationStatus?")},
	{0x0040, CSTR("SemanticStyle")},
	{0x0041, CSTR("SemanticStyleKey_RenderingVersion")},
	{0x0042, CSTR("SemanticStyleKey_Preset")},
	{0x0043, CSTR("SemanticStyleKey_ToneBias")},
	{0x0044, CSTR("SemanticStyleKey_WarmthBias")},
	{0x0045, CSTR("FrontFacingCamera")},
	{0x0046, CSTR("TimeOfFlightAssistedAutoFocusEstimatorContainsBlindSpot")},
	{0x0047, CSTR("LeaderFollowerAutoFocusLeaderDepth")},
	{0x0048, CSTR("LeaderFollowerAutoFocusLeaderFocusMethod")},
	{0x0049, CSTR("LeaderFollowerAutoFocusLeaderConfidence")},
	{0x004a, CSTR("LeaderFollowerAutoFocusLeaderROIType")},
	{0x004b, CSTR("ZeroShutterLagFailureReason")},
	{0x004c, CSTR("TimeOfFlightAssistedAutoFocusEstimatorMSPMeasuredDepth")},
	{0x004d, CSTR("TimeOfFlightAssistedAutoFocusEstimatorMSPSensorConfidence")},
	{0x004e, CSTR("Camera")}
};

void Media::EXIFData::FreeItem(NotNullPtr<Media::EXIFData::EXIFItem> item)
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
	MemFreeNN(item);
}

void Media::EXIFData::ToExifBuffImpl(UInt8 *buff, NotNullPtr<const Data::ReadingListNN<Media::EXIFData::EXIFItem>> exifList, InOutParam<UInt32> startOfst, InOutParam<UInt32> otherOfst) const
{
	UInt32 objCnt;
	UOSInt i;
	UInt32 j;
	UInt64 k;
	NotNullPtr<EXIFItem> exif;

	objCnt = 0;
	k = otherOfst.Get();
	j = startOfst.Get() + 2;
	i = 0;
	while (i < exifList->GetCount())
	{
		exif = exifList->GetItemNoCheck(i);
		switch (exif->type)
		{
		case Media::EXIFData::ET_BYTES:
			WriteInt16(&buff[j], exif->id);
			WriteInt16(&buff[j + 2], 1);
			WriteUInt32(&buff[j + 4], (UInt32)exif->cnt);
			if (exif->cnt <= 4)
			{
				WriteInt32(&buff[j + 8], 0);
				MemCopyNO(&buff[j + 8], &exif->value, (UOSInt)exif->cnt);
				j += 12;
			}
			else
			{
				WriteUInt32(&buff[j + 8], (UInt32)k);
				MemCopyNO(&buff[k], exif->dataBuff, (UOSInt)exif->cnt);
				k += exif->cnt;
				j += 12;
			}
			objCnt++;
			break;
		case Media::EXIFData::ET_STRING:
			WriteInt16(&buff[j], exif->id);
			WriteInt16(&buff[j + 2], 2);
			WriteUInt32(&buff[j + 4], (UInt32)exif->cnt);
			if (exif->cnt <= 4)
			{
				WriteInt32(&buff[j + 8], 0);
				MemCopyNO(&buff[j + 8], exif->dataBuff, (UOSInt)exif->cnt);
				j += 12;
			}
			else
			{
				WriteUInt32(&buff[j + 8], (UInt32)k);
				MemCopyNO(&buff[k], exif->dataBuff, (UOSInt)exif->cnt);
				k += exif->cnt;
				j += 12;
			}
			objCnt++;
			break;
		case Media::EXIFData::ET_UINT16:
			WriteInt16(&buff[j], exif->id);
			WriteInt16(&buff[j + 2], 3);
			WriteUInt32(&buff[j + 4], (UInt32)exif->cnt);
			if (exif->cnt <= 2)
			{
				WriteInt32(&buff[j + 8], 0);
				MemCopyNO(&buff[j + 8], &exif->value, (UOSInt)exif->cnt << 1);
				j += 12;
			}
			else
			{
				WriteUInt32(&buff[j + 8], (UInt32)k);
				MemCopyNO(&buff[k], exif->dataBuff, (UOSInt)exif->cnt << 1);
				k += exif->cnt << 1;
				j += 12;
			}
			objCnt++;
			break;
		case Media::EXIFData::ET_UINT32:
			WriteInt16(&buff[j], exif->id);
			WriteInt16(&buff[j + 2], 4);
			WriteUInt32(&buff[j + 4], (UInt32)exif->cnt);
			if (exif->cnt <= 1)
			{
				WriteInt32(&buff[j + 8], 0);
				MemCopyNO(&buff[j + 8], &exif->value, (UOSInt)exif->cnt << 2);
				j += 12;
			}
			else
			{
				WriteUInt32(&buff[j + 8], (UInt32)k);
				MemCopyNO(&buff[k], exif->dataBuff, (UOSInt)exif->cnt << 2);
				k += exif->cnt << 2;
				j += 12;
			}
			objCnt++;
			break;
		case Media::EXIFData::ET_RATIONAL:
			WriteInt16(&buff[j], exif->id);
			WriteInt16(&buff[j + 2], 5);
			WriteUInt32(&buff[j + 4], (UInt32)exif->cnt);
			WriteUInt32(&buff[j + 8], (UInt32)k);
			MemCopyNO(&buff[k], exif->dataBuff, (UOSInt)exif->cnt << 3);
			k += exif->cnt << 3;
			j += 12;
			objCnt++;
			break;
		case Media::EXIFData::ET_OTHER:
			WriteInt16(&buff[j], exif->id);
			WriteInt16(&buff[j + 2], 7);
			WriteUInt32(&buff[j + 4], (UInt32)exif->cnt);
			if (exif->cnt <= 4)
			{
				WriteInt32(&buff[j + 8], 0);
				MemCopyNO(&buff[j + 8], exif->dataBuff, (UOSInt)exif->cnt);
				j += 12;
			}
			else
			{
				WriteUInt32(&buff[j + 8], (UInt32)k);
				MemCopyNO(&buff[k], exif->dataBuff, (UOSInt)exif->cnt);
				k += exif->cnt;
				j += 12;
			}
			objCnt++;
			break;
		case Media::EXIFData::ET_INT16:
			WriteInt16(&buff[j], exif->id);
			WriteInt16(&buff[j + 2], 8);
			WriteUInt32(&buff[j + 4], (UInt32)exif->cnt);
			if (exif->cnt <= 2)
			{
				WriteInt32(&buff[j + 8], 0);
				MemCopyNO(&buff[j + 8], &exif->value, (UOSInt)exif->cnt << 1);
				j += 12;
			}
			else
			{
				WriteUInt32(&buff[j + 8], (UInt32)k);
				MemCopyNO(&buff[k], exif->dataBuff, (UOSInt)exif->cnt << 1);
				k += exif->cnt << 1;
				j += 12;
			}
			objCnt++;
			break;
		case Media::EXIFData::ET_INT32:
			WriteInt16(&buff[j], exif->id);
			WriteInt16(&buff[j + 2], 9);
			WriteUInt32(&buff[j + 4], (UInt32)exif->cnt);
			if (exif->cnt <= 1)
			{
				WriteInt32(&buff[j + 8], 0);
				MemCopyNO(&buff[j + 8], &exif->value, exif->cnt << 2);
				j += 12;
			}
			else
			{
				WriteUInt32(&buff[j + 8], (UInt32)k);
				MemCopyNO(&buff[k], exif->dataBuff, exif->cnt << 2);
				k += exif->cnt << 2;
				j += 12;
			}
			objCnt++;
			break;
		case Media::EXIFData::ET_SUBEXIF:
			WriteInt16(&buff[j], exif->id);
			WriteInt16(&buff[j + 2], 4);
			WriteInt32(&buff[j + 4], 1);
			exif->value = (Int32)j + 8;
			j += 12;
			objCnt++;
			break;
		case Media::EXIFData::ET_SRATIONAL:
			WriteInt16(&buff[j], exif->id);
			WriteInt16(&buff[j + 2], 10);
			WriteUInt32(&buff[j + 4], (UInt32)exif->cnt);
			WriteUInt32(&buff[j + 8], (UInt32)k);
			MemCopyNO(&buff[k], exif->dataBuff, exif->cnt << 3);
			k += exif->cnt << 3;
			j += 12;
			objCnt++;
			break;
		case Media::EXIFData::ET_DOUBLE:
			WriteInt16(&buff[j], exif->id);
			WriteInt16(&buff[j + 2], 12);
			WriteUInt32(&buff[j + 4], (UInt32)exif->cnt);
			WriteUInt32(&buff[j + 8], (UInt32)k);
			MemCopyNO(&buff[k], exif->dataBuff, exif->cnt << 3);
			k += exif->cnt << 3;
			j += 12;
			objCnt++;
			break;
		case Media::EXIFData::ET_UINT64:
			WriteInt16(&buff[j], exif->id);
			WriteInt16(&buff[j + 2], 16);
			WriteUInt32(&buff[j + 4], (UInt32)exif->cnt);
			WriteUInt32(&buff[j + 8], (UInt32)k);
			MemCopyNO(&buff[k], exif->dataBuff, exif->cnt << 3);
			k += exif->cnt << 3;
			j += 12;
			objCnt++;
			break;
		case Media::EXIFData::ET_INT64:
			WriteInt16(&buff[j], exif->id);
			WriteInt16(&buff[j + 2], 17);
			WriteUInt32(&buff[j + 4], (UInt32)exif->cnt);
			WriteUInt32(&buff[j + 8], (UInt32)k);
			MemCopyNO(&buff[k], exif->dataBuff, exif->cnt << 3);
			k += exif->cnt << 3;
			j += 12;
			objCnt++;
			break;
		case Media::EXIFData::ET_UNKNOWN:
			break;
		}
		i++;
	}
	WriteInt32(&buff[j], 0);
	WriteInt16(&buff[startOfst.Get()], objCnt);
	j += 4;

	UInt32 k32 = (UInt32)k;
	i = 0;
	while (i < exifList->GetCount())
	{
		exif = exifList->GetItemNoCheck(i);
		if (exif->type == Media::EXIFData::ET_SUBEXIF)
		{
			WriteUInt32(&buff[exif->value], j);
			((Media::EXIFData*)exif->dataBuff)->ToExifBuff(buff, j, k32);
		}
		i++;
	}
	startOfst.Set(j);
	otherOfst.Set(k32);
}

void Media::EXIFData::GetExifBuffSize(NotNullPtr<const Data::ReadingListNN<EXIFItem>> exifList, OutParam<UInt64> size, OutParam<UInt64> endOfst) const
{
	UInt64 i = 6;
	UInt64 j = 6;
	UOSInt k;
	UInt64 l;
	UInt64 m;
	NotNullPtr<EXIFItem> exif;

	k = exifList->GetCount();
	while (k-- > 0)
	{
		exif = exifList->GetItemNoCheck(k);
		if (exif->type == Media::EXIFData::ET_BYTES)
		{
			i += 12;
			if (exif->cnt <= 4)
			{
				j += 12;
			}
			else
			{
				j += 12 + exif->cnt;
			}
		}
		else if (exif->type == Media::EXIFData::ET_STRING)
		{
			i += 12;
			if (exif->cnt <= 4)
				j += 12;
			else
				j += 12 + exif->cnt;
		}
		else if (exif->type == Media::EXIFData::ET_UINT16)
		{
			i += 12;
			if (exif->cnt <= 2)
				j += 12;
			else
				j += 12 + (exif->cnt << 1);
		}
		else if (exif->type == Media::EXIFData::ET_UINT32)
		{
			i += 12;
			if (exif->cnt <= 1)
				j += 12;
			else
				j += 12 + (exif->cnt << 2);
		}
		else if (exif->type == Media::EXIFData::ET_RATIONAL)
		{
			i += 12;
			j += 12 + (exif->cnt << 3);
		}
		else if (exif->type == Media::EXIFData::ET_OTHER)
		{
			i += 12;
			if (exif->cnt <= 4)
				j += 12;
			else
				j += 12 + exif->cnt;
		}
		else if (exif->type == Media::EXIFData::ET_INT16)
		{
			i += 12;
			if (exif->cnt <= 2)
				j += 12;
			else
				j += 12 + (exif->cnt << 1);
		}
		else if (exif->type == Media::EXIFData::ET_INT32)
		{
			i += 12;
			if (exif->cnt <= 1)
				j += 12;
			else
				j += 12 + (exif->cnt << 2);
		}
		else if (exif->type == Media::EXIFData::ET_SUBEXIF)
		{
			i += 12;
			j += 12;
			((Media::EXIFData*)exif->dataBuff)->GetExifBuffSize(l, m);
			i += m;
			j += l;
		}
		else if (exif->type == Media::EXIFData::ET_SRATIONAL)
		{
			i += 12;
			j += 12 + (exif->cnt << 3);
		}
		else if (exif->type == Media::EXIFData::ET_DOUBLE)
		{
			i += 12;
			j += 12 + (exif->cnt << 3);
		}
	}
	size.Set(j);
	endOfst.Set(i);
}

Media::EXIFData::EXIFData(EXIFMaker exifMaker)
{
	this->exifMaker = exifMaker;
}

Media::EXIFData::~EXIFData()
{
	UOSInt i = this->exifMap.GetCount();
	while (i-- > 0)
	{
		FreeItem(this->exifMap.GetItemNoCheck(i));
	}
}

Media::EXIFData::EXIFMaker Media::EXIFData::GetEXIFMaker() const
{
	return this->exifMaker;
}

NotNullPtr<Media::EXIFData> Media::EXIFData::Clone() const
{
	NotNullPtr<Media::EXIFData::EXIFItem> item;
	UOSInt i;
	UOSInt j;
	NotNullPtr<Media::EXIFData> newExif;
	NEW_CLASSNN(newExif, Media::EXIFData(this->exifMaker));
	i = 0;
	j = this->exifMap.GetCount();
	while (i < j)
	{
		item = this->exifMap.GetItemNoCheck(i);
		switch (item->type)
		{
		case ET_BYTES:
			if (item->cnt <= 4)
			{
				newExif->AddBytes(item->id, item->cnt, (UInt8*)&item->value);
			}
			else
			{
				newExif->AddBytes(item->id, item->cnt, (UInt8*)item->dataBuff);
			}
			break;
		case ET_STRING:
			newExif->AddString(item->id, item->cnt, (Char*)item->dataBuff);
			break;
		case ET_UINT16:
			if (item->cnt <= 2)
			{
				newExif->AddUInt16(item->id, item->cnt, (UInt16*)&item->value);
			}
			else
			{
				newExif->AddUInt16(item->id, item->cnt, (UInt16*)item->dataBuff);
			}
			break;
		case ET_UINT32:
			if (item->cnt == 1)
			{
				newExif->AddUInt32(item->id, item->cnt, (UInt32*)&item->value);
			}
			else
			{
				newExif->AddUInt32(item->id, item->cnt, (UInt32*)item->dataBuff);
			}
			break;
		case ET_RATIONAL:
			newExif->AddRational(item->id, item->cnt, (UInt32*)item->dataBuff);
			break;
		case ET_OTHER:
			newExif->AddOther(item->id, item->cnt, (UInt8*)item->dataBuff);
			break;
		case ET_INT16:
			if (item->cnt <= 2)
			{
				newExif->AddInt16(item->id, item->cnt, (Int16*)&item->value);
			}
			else
			{
				newExif->AddInt16(item->id, item->cnt, (Int16*)item->dataBuff);
			}
			break;
		case ET_INT32:
			if (item->cnt <= 1)
			{
				newExif->AddInt32(item->id, item->cnt, (Int32*)&item->value);
			}
			else
			{
				newExif->AddInt32(item->id, item->cnt, (Int32*)item->dataBuff);
			}
			break;
		case ET_SUBEXIF:
			newExif->AddSubEXIF(item->id, ((Media::EXIFData*)item->dataBuff)->Clone());
			break;
		case ET_SRATIONAL:
			newExif->AddSRational(item->id, item->cnt, (Int32*)item->dataBuff);
			break;
		case ET_DOUBLE:
			newExif->AddDouble(item->id, item->cnt, (Double*)item->dataBuff);
			break;
		case ET_UINT64:
			newExif->AddUInt64(item->id, item->cnt, (UInt64*)item->dataBuff);
			break;
		case ET_INT64:
			newExif->AddInt64(item->id, item->cnt, (Int64*)item->dataBuff);
			break;
		case ET_UNKNOWN:
		default:
			break;
		}
		i++;
	}
	return newExif;
}

void Media::EXIFData::AddBytes(UInt32 id, UInt64 cnt, const UInt8 *buff)
{
	NotNullPtr<EXIFItem> item = MemAllocNN(EXIFItem);
	item->id = id;
	item->type = ET_BYTES;
	item->cnt = cnt;
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
	if (this->exifMap.Put(id, item).SetTo(item))
	{
		FreeItem(item);
	}
}

void Media::EXIFData::AddString(UInt32 id, UInt64 cnt, const Char *buff)
{
	NotNullPtr<EXIFItem> item = MemAllocNN(EXIFItem);
	item->id = id;
	item->type = ET_STRING;
	item->cnt = cnt;
	item->value = 0;
	item->dataBuff = MemAlloc(UInt8, cnt);
	MemCopyNO(item->dataBuff, buff, cnt);
	if (this->exifMap.Put(id, item).SetTo(item))
	{
		FreeItem(item);
	}
}

void Media::EXIFData::AddUInt16(UInt32 id, UInt64 cnt, const UInt16 *buff)
{
	NotNullPtr<EXIFItem> item = MemAllocNN(EXIFItem);
	item->id = id;
	item->type = ET_UINT16;
	item->cnt = cnt;
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
	if (this->exifMap.Put(id, item).SetTo(item))
	{
		FreeItem(item);
	}
}

void Media::EXIFData::AddUInt32(UInt32 id, UInt64 cnt, const UInt32 *buff)
{
	NotNullPtr<EXIFItem> item = MemAllocNN(EXIFItem);
	item->id = id;
	item->type = ET_UINT32;
	item->cnt = cnt;
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
	if (this->exifMap.Put(id, item).SetTo(item))
	{
		FreeItem(item);
	}
}

void Media::EXIFData::AddInt16(UInt32 id, UInt64 cnt, const Int16 *buff)
{
	NotNullPtr<EXIFItem> item = MemAllocNN(EXIFItem);
	item->id = id;
	item->type = ET_INT16;
	item->cnt = cnt;
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
	if (this->exifMap.Put(id, item).SetTo(item))
	{
		FreeItem(item);
	}
}

void Media::EXIFData::AddInt32(UInt32 id, UInt64 cnt, const Int32 *buff)
{
	NotNullPtr<EXIFItem> item = MemAllocNN(EXIFItem);
	item->id = id;
	item->type = ET_INT32;
	item->cnt = cnt;
	if (cnt <= 1)
	{
		MemCopyNO(&item->value, buff, cnt * sizeof(Int32));
		item->dataBuff = 0;
	}
	else
	{
		item->value = *(Int32*)buff;
		item->dataBuff = MemAlloc(Int32, cnt);
		MemCopyNO(item->dataBuff, buff, cnt * sizeof(Int32));
	}
	if (this->exifMap.Put(id, item).SetTo(item))
	{
		FreeItem(item);
	}
}

void Media::EXIFData::AddRational(UInt32 id, UInt64 cnt, const UInt32 *buff)
{
	NotNullPtr<EXIFItem> item = MemAllocNN(EXIFItem);
	item->id = id;
	item->type = ET_RATIONAL;
	item->cnt = cnt;
	item->value = 0;
	item->dataBuff = MemAlloc(UInt32, cnt << 1);
	MemCopyNO(item->dataBuff, buff, cnt * sizeof(UInt32) * 2);

	if (this->exifMap.Put(id, item).SetTo(item))
	{
		FreeItem(item);
	}
}

void Media::EXIFData::AddSRational(UInt32 id, UInt64 cnt, const Int32 *buff)
{
	NotNullPtr<EXIFItem> item = MemAllocNN(EXIFItem);
	item->id = id;
	item->type = ET_SRATIONAL;
	item->cnt = cnt;
	item->value = 0;
	item->dataBuff = MemAlloc(Int32, cnt << 1);
	MemCopyNO(item->dataBuff, buff, cnt * sizeof(Int32) * 2);

	if (this->exifMap.Put(id, item).SetTo(item))
	{
		FreeItem(item);
	}
}

void Media::EXIFData::AddOther(UInt32 id, UInt64 cnt, const UInt8 *buff)
{
	NotNullPtr<EXIFItem> item = MemAllocNN(EXIFItem);
	item->id = id;
	item->type = ET_OTHER;
	item->cnt = cnt;
	item->value = 0;
	item->dataBuff = MemAlloc(UInt8, cnt);
	MemCopyNO(item->dataBuff, buff, cnt);

	if (this->exifMap.Put(id, item).SetTo(item))
	{
		FreeItem(item);
	}
}

void Media::EXIFData::AddSubEXIF(UInt32 id, NotNullPtr<Media::EXIFData> exif)
{
	NotNullPtr<EXIFItem> item = MemAllocNN(EXIFItem);
	item->id = id;
	item->type = ET_SUBEXIF;
	item->cnt = 1;
	item->value = 0;
	item->dataBuff = exif.Ptr();
	if (this->exifMap.Put(id, item).SetTo(item))
	{
		FreeItem(item);
	}
}

void Media::EXIFData::AddDouble(UInt32 id, UInt64 cnt, const Double *buff)
{
	NotNullPtr<EXIFItem> item = MemAllocNN(EXIFItem);
	item->id = id;
	item->type = ET_DOUBLE;
	item->cnt = cnt;
	item->value = 0;
	item->dataBuff = MemAlloc(Double, cnt);
	MemCopyNO(item->dataBuff, buff, cnt * sizeof(Double));
	if (this->exifMap.Put(id, item).SetTo(item))
	{
		FreeItem(item);
	}
}

void Media::EXIFData::AddUInt64(UInt32 id, UInt64 cnt, const UInt64 *buff)
{
	NotNullPtr<EXIFItem> item = MemAllocNN(EXIFItem);
	item->id = id;
	item->type = ET_UINT64;
	item->cnt = cnt;
	item->value = 0;
	item->dataBuff = MemAlloc(UInt64, cnt);
	MemCopyNO(item->dataBuff, buff, cnt * sizeof(UInt64));
	if (this->exifMap.Put(id, item).SetTo(item))
	{
		FreeItem(item);
	}
}

void Media::EXIFData::AddInt64(UInt32 id, UInt64 cnt, const Int64 *buff)
{
	NotNullPtr<EXIFItem> item = MemAllocNN(EXIFItem);
	item->id = id;
	item->type = ET_INT64;
	item->cnt = cnt;
	item->value = 0;
	item->dataBuff = MemAlloc(Int64, cnt);
	MemCopyNO(item->dataBuff, buff, cnt * sizeof(Int64));
	if (this->exifMap.Put(id, item).SetTo(item))
	{
		FreeItem(item);
	}
}

void Media::EXIFData::Remove(UInt32 id)
{
	NotNullPtr<EXIFItem> item;
	if (this->exifMap.Remove(id).SetTo(item))
	{
		FreeItem(item);
	}
}

UOSInt Media::EXIFData::GetExifIds(Data::ArrayList<UInt32> *idArr) const
{
	UOSInt cnt = this->exifMap.GetCount();
	UOSInt i = 0;
	while (i < cnt)
	{
		idArr->Add(this->exifMap.GetItemNoCheck(i)->id);
		i++;
	}
	return cnt;
}

Media::EXIFData::EXIFType Media::EXIFData::GetExifType(UInt32 id) const
{
	NotNullPtr<Media::EXIFData::EXIFItem> item;
	if (!this->exifMap.Get(id).SetTo(item))
		return ET_UNKNOWN;
	return item->type;
}

UInt64 Media::EXIFData::GetExifCount(UInt32 id) const
{
	NotNullPtr<Media::EXIFData::EXIFItem> item;
	if (!this->exifMap.Get(id).SetTo(item))
		return 0;
	return item->cnt;
}

Optional<Media::EXIFData::EXIFItem> Media::EXIFData::GetExifItem(UInt32 id) const
{
	return this->exifMap.Get(id);
}

UInt16 *Media::EXIFData::GetExifUInt16(UInt32 id) const
{
	NotNullPtr<Media::EXIFData::EXIFItem> item;
	if (!this->exifMap.Get(id).SetTo(item))
		return 0;
	if (item->type != ET_UINT16)
		return 0;
	if (item->cnt > 2)
	{
		return (UInt16*)item->dataBuff;
	}
	else
	{
		return (UInt16*)&item->value;
	}
}

UInt32 *Media::EXIFData::GetExifUInt32(UInt32 id) const
{
	NotNullPtr<Media::EXIFData::EXIFItem> item;
	if (!this->exifMap.Get(id).SetTo(item))
		return 0;
	if (item->type != ET_UINT32)
		return 0;
	if (item->cnt > 1)
	{
		return (UInt32*)item->dataBuff;
	}
	else
	{
		return (UInt32*)&item->value;
	}
}

Media::EXIFData *Media::EXIFData::GetExifSubexif(UInt32 id) const
{
	NotNullPtr<Media::EXIFData::EXIFItem> item;
	if (!this->exifMap.Get(id).SetTo(item))
		return 0;
	if (item->type != ET_SUBEXIF)
		return 0;
	return (Media::EXIFData*)item->dataBuff;
}

UInt8 *Media::EXIFData::GetExifOther(UInt32 id) const
{
	NotNullPtr<Media::EXIFData::EXIFItem> item;
	if (!this->exifMap.Get(id).SetTo(item))
		return 0;
	if (item->type != ET_OTHER)
		return 0;
	return (UInt8*)item->dataBuff;
}

Bool Media::EXIFData::GetPhotoDate(NotNullPtr<Data::DateTime> dt) const
{
	NotNullPtr<Media::EXIFData::EXIFItem> item;
	if (this->exifMaker == EM_STANDARD)
	{
		if (this->exifMap.Get(36867).SetTo(item))
		{
			if (item->type == ET_STRING)
			{
				dt->SetValue(Text::CStringNN((const UTF8Char*)item->dataBuff, item->cnt - 1));
				return true;
			}
		}
		if (this->exifMap.Get(36868).SetTo(item))
		{
			if (item->type == ET_STRING)
			{
				dt->SetValue(Text::CStringNN((const UTF8Char*)item->dataBuff, item->cnt - 1));
				return true;
			}
		}
		if (this->exifMap.Get(34665).SetTo(item))
		{
			if (item->type == ET_SUBEXIF)
			{
				Media::EXIFData *exif = (Media::EXIFData*)item->dataBuff;
				if (exif->GetPhotoDate(dt))
					return true;
			}
		}
		if (this->exifMap.Get(306).SetTo(item))
		{
			if (item->type == ET_STRING)
			{
				dt->SetValue(Text::CStringNN((const UTF8Char*)item->dataBuff, item->cnt - 1));
				return true;
			}
		}
	}
	return false;
}

Bool Media::EXIFData::GetPhotoDate(OutParam<Data::Timestamp> ts) const
{
	NotNullPtr<Media::EXIFData::EXIFItem> item;
	if (this->exifMaker == EM_STANDARD)
	{
		if (this->exifMap.Get(36867).SetTo(item))
		{
			if (item->type == ET_STRING)
			{
				ts.Set(Data::Timestamp::FromStr(Text::CStringNN((const UTF8Char*)item->dataBuff, item->cnt - 1), Data::DateTimeUtil::GetLocalTzQhr()));
				return true;
			}
		}
		if (this->exifMap.Get(36868).SetTo(item))
		{
			if (item->type == ET_STRING)
			{
				ts.Set(Data::Timestamp::FromStr(Text::CStringNN((const UTF8Char*)item->dataBuff, item->cnt - 1), Data::DateTimeUtil::GetLocalTzQhr()));
				return true;
			}
		}
		if (this->exifMap.Get(34665).SetTo(item))
		{
			if (item->type == ET_SUBEXIF)
			{
				Media::EXIFData *exif = (Media::EXIFData*)item->dataBuff;
				if (exif->GetPhotoDate(ts))
					return true;
			}
		}
		if (this->exifMap.Get(306).SetTo(item))
		{
			if (item->type == ET_STRING)
			{
				ts.Set(Data::Timestamp::FromStr(Text::CStringNN((const UTF8Char*)item->dataBuff, item->cnt - 1), Data::DateTimeUtil::GetLocalTzQhr()));
				return true;
			}
		}
	}
	return false;
}

Text::CString Media::EXIFData::GetPhotoMake() const
{
	NotNullPtr<Media::EXIFData::EXIFItem> item;
	if (this->exifMaker == EM_STANDARD)
	{
		if (this->exifMap.Get(271).SetTo(item))
		{
			if (item->type == ET_STRING)
			{
				return Text::CString((const UTF8Char*)item->dataBuff, item->cnt - 1);
			}
		}
	}
	return CSTR_NULL;
}

Text::CString Media::EXIFData::GetPhotoModel() const
{
	NotNullPtr<Media::EXIFData::EXIFItem> item;
	if (this->exifMaker == EM_STANDARD)
	{
		if (this->exifMap.Get(272).SetTo(item))
		{
			if (item->type == ET_STRING)
			{
				return Text::CString((const UTF8Char*)item->dataBuff, item->cnt - 1);
			}
		}
	}
	if (this->exifMaker == EM_CANON)
	{
		if (this->exifMap.Get(6).SetTo(item))
		{
			if (item->type == ET_STRING)
			{
				return Text::CString((const UTF8Char*)item->dataBuff, item->cnt - 1);
			}
		}
	}
	return CSTR_NULL;
}

Text::CString Media::EXIFData::GetPhotoLens() const
{
	NotNullPtr<Media::EXIFData::EXIFItem> item;
	if (this->exifMaker == EM_CANON)
	{
		if (this->exifMap.Get(149).SetTo(item))
		{
			if (item->type == ET_STRING)
			{
				return Text::CString((const UTF8Char*)item->dataBuff, item->cnt - 1);
			}
		}
	}
	if (this->exifMaker == EM_PANASONIC)
	{
		if (this->exifMap.Get(81).SetTo(item))
		{
			if (item->type == ET_STRING)
			{
				return Text::CString((const UTF8Char*)item->dataBuff, item->cnt - 1);
			}
		}
	}
	return CSTR_NULL;
}

Double Media::EXIFData::GetPhotoFNumber() const
{
	NotNullPtr<Media::EXIFData::EXIFItem> item;
	if (this->exifMaker == EM_STANDARD)
	{
		if (this->exifMap.Get(33437).SetTo(item))
		{
			if (item->type == ET_RATIONAL && item->cnt == 1)
			{
				UInt32 *dataBuff = (UInt32*)item->dataBuff;
				return dataBuff[0] / (Double)dataBuff[1];
			}
		}
		if (this->exifMap.Get(34665).SetTo(item))
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

Double Media::EXIFData::GetPhotoExpTime() const
{
	NotNullPtr<Media::EXIFData::EXIFItem> item;
	if (this->exifMaker == EM_STANDARD)
	{
		if (this->exifMap.Get(33434).SetTo(item))
		{
			if (item->type == ET_RATIONAL && item->cnt == 1)
			{
				UInt32 *dataBuff = (UInt32*)item->dataBuff;
				return dataBuff[0] / (Double)dataBuff[1];
			}
		}
		if (this->exifMap.Get(34665).SetTo(item))
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

UInt32 Media::EXIFData::GetPhotoISO() const
{
	NotNullPtr<Media::EXIFData::EXIFItem> item;
	if (this->exifMaker == EM_STANDARD)
	{
		if (this->exifMap.Get(34855).SetTo(item))
		{
			if (item->type == ET_UINT16 && item->cnt == 1)
			{
				return *(UInt16*)&item->value;
			}
			else if (item->type == ET_UINT32 && item->cnt == 1)
			{
				return *(UInt32*)&item->value;
			}
		}
		if (this->exifMap.Get(34665).SetTo(item))
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

Double Media::EXIFData::GetPhotoFocalLength() const
{
	NotNullPtr<Media::EXIFData::EXIFItem> item;
	if (this->exifMaker == EM_STANDARD)
	{
		if (this->exifMap.Get(37386).SetTo(item))
		{
			if (item->type == ET_RATIONAL && item->cnt == 1)
			{
				UInt32 *dataBuff = (UInt32*)item->dataBuff;
				return dataBuff[0] / (Double)dataBuff[1];
			}
		}
		if (this->exifMap.Get(34665).SetTo(item))
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

Bool Media::EXIFData::GetPhotoLocation(Double *lat, Double *lon, Double *altitude, Int64 *gpsTimeTick) const
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
		NotNullPtr<Media::EXIFData::EXIFItem> item1;
		NotNullPtr<Media::EXIFData::EXIFItem> item2;
		Double val = 0;
		if (subExif->GetExifItem(1).SetTo(item1) && subExif->GetExifItem(2).SetTo(item2))
		{
			if (item2->type == Media::EXIFData::ET_RATIONAL)
			{
				if (item2->cnt == 3)
				{
					val = ReadInt32(&((UInt8*)item2->dataBuff)[0]) / (Double)ReadInt32(&((UInt8*)item2->dataBuff)[4]);
					val += ReadInt32(&((UInt8*)item2->dataBuff)[8]) / (Double)ReadInt32(&((UInt8*)item2->dataBuff)[12]) / 60.0;
					val += ReadInt32(&((UInt8*)item2->dataBuff)[16]) / (Double)ReadInt32(&((UInt8*)item2->dataBuff)[20]) / 3600.0;
				}
				else if (item2->cnt == 1)
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
		if (subExif->GetExifItem(3).SetTo(item1) && subExif->GetExifItem(4).SetTo(item2))
		{
			if (item2->type == Media::EXIFData::ET_RATIONAL)
			{
				if (item2->cnt == 3)
				{
					val = ReadInt32(&((UInt8*)item2->dataBuff)[0]) / (Double)ReadInt32(&((UInt8*)item2->dataBuff)[4]);
					val += ReadInt32(&((UInt8*)item2->dataBuff)[8]) / (Double)ReadInt32(&((UInt8*)item2->dataBuff)[12]) / 60.0;
					val += ReadInt32(&((UInt8*)item2->dataBuff)[16]) / (Double)ReadInt32(&((UInt8*)item2->dataBuff)[20]) / 3600.0;
				}
				else if (item2->cnt == 1)
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
		if (subExif->GetExifItem(5).SetTo(item1) && subExif->GetExifItem(6).SetTo(item2))
		{
			if (item2->type == Media::EXIFData::ET_RATIONAL)
			{
				if (item2->cnt == 1)
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
			if (item1->type == Media::EXIFData::ET_BYTES && item1->cnt == 1)
			{
				if ((item1->value & 0xff) == 1)
				{
					val = -val;
				}
			}
			else if (item1->type == Media::EXIFData::ET_UINT16 && item1->cnt == 1)
			{
				if ((item1->value & 0xffff) == 1)
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
			if (altitude)
			{
				*altitude = 0;
			}
		}
		if (subExif->GetExifItem(7).SetTo(item1) && subExif->GetExifItem(29).SetTo(item2))
		{
			Int32 hh = 0;
			Int32 mm = 0;
			Int32 ss = 0;
			Int32 ms = 0;

			if (item1->type == Media::EXIFData::ET_RATIONAL && item1->cnt == 3)
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
			if (item2->type == Media::EXIFData::ET_STRING && item2->cnt == 11)
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
			if (gpsTimeTick)
			{
				*gpsTimeTick = 0;
			}
		}
		return succ;
	}
	return false;
}

Bool Media::EXIFData::GetGeoBounds(Math::Size2D<UOSInt> imgSize, UInt32 *srid, Double *minX, Double *minY, Double *maxX, Double *maxY) const
{
	NotNullPtr<Media::EXIFData::EXIFItem> item;
	NotNullPtr<Media::EXIFData::EXIFItem> item2;
	if (this->exifMap.Get(33922).SetTo(item) && this->exifMap.Get(33550).SetTo(item2))
	{
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
		*maxX = mapX + (UOSInt2Double(imgSize.x) - imgX) * mppX;
		*minY = mapY - (UOSInt2Double(imgSize.y) - imgY) * mppY;

	//	DEL_CLASS(coord);
		return true;
	}

	if (this->exifMap.Get(34264).SetTo(item) && this->exifMap.Get(34735).SetTo(item2))
	{
		UInt32 fileSRID = 0;
		UInt16 *keyBuff = (UInt16*)item2->dataBuff;
		UOSInt keyCnt = item2->cnt >> 2;
		UOSInt i = 1;
		while (i < keyCnt)
		{
			if (keyBuff[(i << 2)] == 3072)
			{
				fileSRID = keyBuff[(i << 2) + 3];
			}
			i++;
		}

		if (item->cnt == 16)
		{
			Math::Size2DDbl dimgSize = imgSize.ToDouble();
			Double *transfMatrix = (Double*)item->dataBuff;
			*minX = transfMatrix[3];
			*maxY = transfMatrix[7];
			*maxX = transfMatrix[3] + transfMatrix[0] * dimgSize.x + transfMatrix[1] * dimgSize.y;
			*minY = transfMatrix[7] + transfMatrix[4] * dimgSize.x + transfMatrix[5] * dimgSize.y;
			if (srid) *srid = fileSRID;
			return true;
		}
	}
	return false;
}

Media::RotateType Media::EXIFData::GetRotateType() const
{
	NotNullPtr<Media::EXIFData::EXIFItem> item;
	if (!this->exifMap.Get(274).SetTo(item))
	{
		return Media::RotateType::None;
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
	switch (v)
	{
	case 0:
	case 1:
		return Media::RotateType::None;
	case 2:
		return Media::RotateType::HFLIP;
	case 3:
		return Media::RotateType::CW_180;
	case 4:
		return Media::RotateType::HFLIP_CW_180;
	case 5:
		return Media::RotateType::HFLIP_CW_90;
	case 6:
		return Media::RotateType::CW_90;
	case 7:
		return Media::RotateType::HFLIP_CW_270;
	case 8:
		return Media::RotateType::CW_270;
	default:
		return Media::RotateType::None;
	}
}

void Media::EXIFData::SetRotateType(RotateType rotType)
{
	UInt32 v;
	switch (rotType)
	{
	case RotateType::None:
		v = 1;
		break;
	case RotateType::CW_90:
		v = 6;
		break;
	case RotateType::CW_180:
		v = 3;
		break;
	case RotateType::CW_270:
		v = 8;
		break;
	case RotateType::HFLIP:
		v = 2;
		break;
	case RotateType::HFLIP_CW_90:
		v = 5;
		break;
	case RotateType::HFLIP_CW_180:
		v = 4;
		break;
	case RotateType::HFLIP_CW_270:
		v = 7;
		break;
	default:
		v = 0;
		break;
	}
	this->AddUInt32(274, 1, &v);
}

Double Media::EXIFData::GetHDPI() const
{
	NotNullPtr<Media::EXIFData::EXIFItem> item;
	if (!this->exifMap.Get(282).SetTo(item))
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

Double Media::EXIFData::GetVDPI() const
{
	NotNullPtr<Media::EXIFData::EXIFItem> item;
	if (!this->exifMap.Get(283).SetTo(item))
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

	NotNullPtr<Media::EXIFData::EXIFItem> item;
	if (this->exifMap.Get(34665).SetTo(item) && item->type == Media::EXIFData::ET_SUBEXIF)
	{
		((EXIFData*)item->dataBuff)->AddUInt32(40962, 1, &width);
	}
}

void Media::EXIFData::SetHeight(UInt32 height)
{
	this->AddUInt32(257, 1, &height);

	NotNullPtr<Media::EXIFData::EXIFItem> item;
	if (this->exifMap.Get(34665).SetTo(item) && item->type == Media::EXIFData::ET_SUBEXIF)
	{
		((EXIFData*)item->dataBuff)->AddUInt32(40963, 1, &height);
	}
}

Bool Media::EXIFData::ToString(NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString linePrefix) const
{
	Data::ArrayList<UInt32> exifIds;
	NotNullPtr<Media::EXIFData::EXIFItem> exItem;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UInt32 v;

	sb->AppendC(UTF8STRC("EXIF Content:"));
	this->GetExifIds(&exifIds);
	i = 0;
	j = exifIds.GetCount();
	while (i < j)
	{
		v = exifIds.GetItem(i);
		sb->AppendC(UTF8STRC("\r\n"));
		if (linePrefix.v)
			sb->Append(linePrefix);
		sb->AppendC(UTF8STRC("Id = "));
		sb->AppendU32(v);
		sb->AppendC(UTF8STRC(", name = "));
		sb->Append(Media::EXIFData::GetEXIFName(this->exifMaker, v));
		if (this->GetExifItem(v).SetTo(exItem))
		{
			if (exItem->type == Media::EXIFData::ET_SUBEXIF)
			{
				Data::ArrayList<UInt32> subExIds;
				UOSInt i2;
				UOSInt j2;
				UInt32 v2;
				NotNullPtr<Media::EXIFData::EXIFItem> subExItem;
				Media::EXIFData *subExif = (Media::EXIFData*)exItem->dataBuff;
				i2 = 0;
				j2 = subExif->GetExifIds(&subExIds);
				while (i2 < j2)
				{
					v2 = subExIds.GetItem(i2);
					sb->AppendC(UTF8STRC("\r\n"));
					if (linePrefix.v)
						sb->Append(linePrefix);
					sb->AppendC(UTF8STRC(" Subid = "));
					sb->AppendU32(v2);
					sb->AppendC(UTF8STRC(", name = "));
					sb->Append(Media::EXIFData::GetEXIFName(this->exifMaker, v, v2));

					if (subExif->GetExifItem(v2).SetTo(subExItem))
					{
						if (subExItem->type == Media::EXIFData::ET_STRING)
						{
							sb->AppendC(UTF8STRC(", value = "));
							if (((Char*)subExItem->dataBuff)[subExItem->cnt - 1])
							{
								sb->AppendC((UTF8Char*)subExItem->dataBuff, subExItem->cnt);
							}
							else
							{
								sb->AppendSlow((UTF8Char*)subExItem->dataBuff);
							}
						}
						else if (subExItem->type == Media::EXIFData::ET_DOUBLE)
						{
							Double *valBuff = (Double*)subExItem->dataBuff;
							k = 0;
							while (k < subExItem->cnt)
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
							if (subExItem->cnt <= 4)
							{
								valBuff = (UInt8*)&subExItem->value;
							}
							else
							{
								valBuff = (UInt8*)subExItem->dataBuff;
							}
							sb->AppendC(UTF8STRC(", value = "));
							if (subExItem->cnt > 1024)
							{
								sb->AppendU64(subExItem->cnt);
								sb->AppendC(UTF8STRC(" bytes: "));
								sb->AppendHexBuff(valBuff, 256, ' ', Text::LineBreakType::CRLF);
								sb->AppendC(UTF8STRC("\r\n...\r\n"));
								sb->AppendHexBuff(&valBuff[((Int32)subExItem->cnt & ~15) - 256], 256 + (subExItem->cnt & 15), ' ', Text::LineBreakType::CRLF);
							}
							else
							{
								sb->AppendHexBuff(valBuff, subExItem->cnt, ' ', Text::LineBreakType::CRLF);
							}
						}
						else if (subExItem->type == Media::EXIFData::ET_UINT16)
						{
							UInt16 *valBuff;
							if (subExItem->cnt <= 2)
							{
								valBuff = (UInt16*)&subExItem->value;
							}
							else
							{
								valBuff = (UInt16*)subExItem->dataBuff;
							}
							k = 0;
							while (k < subExItem->cnt)
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
							if (subExItem->cnt <= 1)
							{
								valBuff = (UInt32*)&subExItem->value;
							}
							else
							{
								valBuff = (UInt32*)subExItem->dataBuff;
							}
							k = 0;
							while (k < subExItem->cnt)
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
							while (k < subExItem->cnt)
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
						else if (subExItem->type == Media::EXIFData::ET_SRATIONAL)
						{
							Int32 *valBuff;
							valBuff = (Int32*)subExItem->dataBuff;
							k = 0;
							while (k < subExItem->cnt)
							{
								if (k == 0)
								{
									sb->AppendC(UTF8STRC(", value = "));
								}
								else
								{
									sb->AppendC(UTF8STRC(", "));
								}
								sb->AppendI32(valBuff[k * 2]);
								sb->AppendC(UTF8STRC(" / "));
								sb->AppendI32(valBuff[k * 2 + 1]);
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
							if (subExItem->cnt <= 2)
							{
								valBuff = (Int16*)&subExItem->value;
							}
							else
							{
								valBuff = (Int16*)subExItem->dataBuff;
							}
							k = 0;
							while (k < subExItem->cnt)
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
						else if (subExItem->type == Media::EXIFData::ET_INT32)
						{
							Int32 *valBuff;
							if (subExItem->cnt <= 1)
							{
								valBuff = (Int32*)&subExItem->value;
							}
							else
							{
								valBuff = (Int32*)subExItem->dataBuff;
							}
							k = 0;
							while (k < subExItem->cnt)
							{
								if (k == 0)
								{
									sb->AppendC(UTF8STRC(", value = "));
								}
								else
								{
									sb->AppendC(UTF8STRC(", "));
								}
								sb->AppendI32(valBuff[k]);
								k++;
							}
						}
						else if (subExItem->id == 37500)
						{
							UInt8 *valBuff;
							if (subExItem->cnt <= 4)
							{
								valBuff = (UInt8*)&subExItem->value;
							}
							else
							{
								valBuff = (UInt8*)subExItem->dataBuff;
							}
							NotNullPtr<Media::EXIFData> innerExif;
							if (ParseMakerNote(valBuff, subExItem->cnt).SetTo(innerExif))
							{
								UTF8Char sbuff[32];
								UTF8Char *sptr;
								sb->AppendC(UTF8STRC(", Format = "));
								sb->Append(GetEXIFMakerName(innerExif->GetEXIFMaker()));
								sb->AppendC(UTF8STRC(", Inner "));
								if (linePrefix.v)
								{
									sptr = linePrefix.ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("  ")));
								}
								else
								{
									sptr = Text::StrConcatC(sbuff, UTF8STRC("  "));
								}
								innerExif->ToString(sb, CSTRP(sbuff, sptr));
								innerExif.Delete();
							}
							else
							{
								sb->AppendC(UTF8STRC(", value (Other) = "));
								sb->AppendHexBuff(valBuff, subExItem->cnt, ' ', Text::LineBreakType::CRLF);
							}
						}
						else if (subExItem->type == Media::EXIFData::ET_OTHER)
						{
							UInt8 *valBuff;
							valBuff = (UInt8*)subExItem->dataBuff;
							if (this->exifMaker == Media::EXIFData::EM_OLYMPUS && subExItem->id == 0)
							{
								sb->AppendC(UTF8STRC(", value = "));
								sb->AppendC((const UTF8Char*)valBuff, subExItem->cnt);
							}
							else
							{
								sb->AppendC(UTF8STRC(", value (Other) = "));
								sb->AppendHexBuff(valBuff, subExItem->cnt, ' ', Text::LineBreakType::CRLF);
							}
						}
						else
						{
							UInt8 *valBuff;
							if (subExItem->cnt <= 4 && subExItem->type != Media::EXIFData::ET_OTHER)
							{
								valBuff = (UInt8*)&subExItem->value;
							}
							else
							{
								valBuff = (UInt8*)subExItem->dataBuff;
							}
							sb->AppendC(UTF8STRC(", value (Unk) = "));
							sb->AppendHexBuff(valBuff, subExItem->cnt, ' ', Text::LineBreakType::CRLF);
						}
					}	
					i2++;
				}
			}
			else if (exItem->type == Media::EXIFData::ET_STRING)
			{
				sb->AppendC(UTF8STRC(", value = "));
				if (((Char*)exItem->dataBuff)[exItem->cnt - 1])
				{
					sb->AppendC((UTF8Char*)exItem->dataBuff, exItem->cnt);
				}
				else
				{
					sb->AppendSlow((UTF8Char*)exItem->dataBuff);
				}
			}
			else if (exItem->type == Media::EXIFData::ET_DOUBLE)
			{
				Double *valBuff = (Double*)exItem->dataBuff;
				k = 0;
				while (k < exItem->cnt)
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
				if (exItem->cnt <= 4)
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
					if (valBuff[exItem->cnt - 2] == 0)
					{
						NotNullPtr<Text::String> s = Text::String::NewNotNull((const UTF16Char*)valBuff);
						sb->Append(s);
						s->Release();
					}
					else
					{
						NotNullPtr<Text::String> s = Text::String::New((const UTF16Char*)valBuff, exItem->cnt >> 1);
						sb->Append(s);
						s->Release();
					}
				}
				else
				{
					if (exItem->cnt > 1024)
					{
						sb->AppendU64(exItem->cnt);
						sb->AppendC(UTF8STRC(" bytes: "));
						sb->AppendHexBuff(valBuff, 256, ' ', Text::LineBreakType::CRLF);
						sb->AppendC(UTF8STRC("\r\n...\r\n"));
						sb->AppendHexBuff(&valBuff[((Int32)exItem->cnt & ~15) - 256], 256 + (exItem->cnt & 15), ' ', Text::LineBreakType::CRLF);
					}
					else
					{
						sb->AppendHexBuff(valBuff, exItem->cnt, ' ', Text::LineBreakType::CRLF);
					}
				}
			}
			else if (exItem->type == Media::EXIFData::ET_UINT16)
			{
				UInt16 *valBuff;
				if (exItem->cnt <= 2)
				{
					valBuff = (UInt16*)&exItem->value;
				}
				else
				{
					valBuff = (UInt16*)exItem->dataBuff;
				}
				if (this->exifMaker == Media::EXIFData::EM_CANON && exItem->id == 1)
				{
					this->ToStringCanonCameraSettings(sb, linePrefix, valBuff, exItem->cnt);
				}
				else if (this->exifMaker == Media::EXIFData::EM_CANON && exItem->id == 2)
				{
					this->ToStringCanonFocalLength(sb, linePrefix, valBuff, exItem->cnt);
				}
				else if (this->exifMaker == Media::EXIFData::EM_CANON && exItem->id == 4)
				{
					this->ToStringCanonShotInfo(sb, linePrefix, valBuff, exItem->cnt);
				}
				else
				{
					k = 0;
					while (k < exItem->cnt)
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
				if (exItem->cnt <= 1)
				{
					valBuff = (UInt32*)&exItem->value;
				}
				else
				{
					valBuff = (UInt32*)exItem->dataBuff;
				}
				k = 0;
				while (k < exItem->cnt)
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
				while (k < exItem->cnt)
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
			else if (exItem->type == Media::EXIFData::ET_SRATIONAL)
			{
				Int32 *valBuff;
				valBuff = (Int32*)exItem->dataBuff;
				k = 0;
				while (k < exItem->cnt)
				{
					if (k == 0)
					{
						sb->AppendC(UTF8STRC(", value = "));
					}
					else
					{
						sb->AppendC(UTF8STRC(", "));
					}
					sb->AppendI32(valBuff[k * 2]);
					sb->AppendC(UTF8STRC(" / "));
					sb->AppendI32(valBuff[k * 2 + 1]);
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
				if (exItem->cnt <= 2)
				{
					valBuff = (Int16*)&exItem->value;
				}
				else
				{
					valBuff = (Int16*)exItem->dataBuff;
				}
				k = 0;
				while (k < exItem->cnt)
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
			else if (exItem->type == Media::EXIFData::ET_INT32)
			{
				Int32 *valBuff;
				if (exItem->cnt <= 2)
				{
					valBuff = (Int32*)&exItem->value;
				}
				else
				{
					valBuff = (Int32*)exItem->dataBuff;
				}
				k = 0;
				while (k < exItem->cnt)
				{
					if (k == 0)
					{
						sb->AppendC(UTF8STRC(", value = "));
					}
					else
					{
						sb->AppendC(UTF8STRC(", "));
					}
					sb->AppendI32(valBuff[k]);
					k++;
				}
			}
			else if (exItem->type == Media::EXIFData::ET_OTHER)
			{
				if (this->exifMaker == EM_OLYMPUS && exItem->id == 521)
				{
					sb->AppendC(UTF8STRC(", value = "));
					if (((Char*)exItem->dataBuff)[exItem->cnt - 1])
					{
						sb->AppendC((UTF8Char*)exItem->dataBuff, exItem->cnt);
					}
					else
					{
						sb->AppendSlow((UTF8Char*)exItem->dataBuff);
					}
				}
				else
				{
		//			UInt8 *valBuff;
		//			valBuff = (UInt8*)exItem->dataBuff;
					sb->AppendC(UTF8STRC(", Other: size = "));
					sb->AppendU64(exItem->cnt);
		//			sb->AppendHex(valBuff, subExItem->cnt, ' ', Text::StringBuilder::LBT_CRLF);
				}
			}
			else
			{
	/*			UInt8 *valBuff;
				if (exItem->cnt <= 4)
				{
					valBuff = (UInt8*)&exItem->value;
				}
				else
				{
					valBuff = (UInt8*)exItem->dataBuff;
				}*/
				sb->AppendC(UTF8STRC(", Unknown: size = "));
				sb->AppendU64(exItem->cnt);
	//			sb->AppendHex(valBuff, subExItem->cnt, ' ', Text::StringBuilder::LBT_CRLF);
			}
		}
		i++;
	}
	Double lat;
	Double lon;
	Double altitude;
	Int64 gpsTimeTick;
	if (this->GetPhotoLocation(&lat, &lon, &altitude, &gpsTimeTick))
	{
		sb->AppendC(UTF8STRC("\r\nGPS Location: "));
		sb->AppendDouble(lat);
		sb->AppendC(UTF8STRC(", "));
		sb->AppendDouble(lon);
		sb->AppendC(UTF8STRC(", "));
		sb->AppendDouble(altitude);
	}
	return true;
}

Bool Media::EXIFData::ToStringCanonCameraSettings(NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString linePrefix, UInt16 *valBuff, UOSInt valCnt) const
{
	Bool isInt16;
	Bool isUInt16;
	UOSInt k;
	k = 0;
	while (k < valCnt)
	{
		sb->AppendC(UTF8STRC("\r\n"));
		if (linePrefix.v)
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

Bool Media::EXIFData::ToStringCanonFocalLength(NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString linePrefix, UInt16 *valBuff, UOSInt valCnt) const
{
	Bool isInt16;
	Bool isUInt16;
	UOSInt k;
	k = 0;
	while (k < valCnt)
	{
		sb->AppendC(UTF8STRC("\r\n"));
		if (linePrefix.v)
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

Bool Media::EXIFData::ToStringCanonShotInfo(NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString linePrefix, UInt16 *valBuff, UOSInt valCnt) const
{
	Bool isInt16;
	Bool isUInt16;
	UOSInt k;
	k = 0;
	while (k < valCnt)
	{
		sb->AppendC(UTF8STRC("\r\n"));
		if (linePrefix.v)
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

Bool Media::EXIFData::ToStringCanonLensType(NotNullPtr<Text::StringBuilderUTF8> sb, UInt16 lensType) const
{
	sb->AppendC(UTF8STRC("0x"));
	sb->AppendHex16(lensType);
	return true;
}

void Media::EXIFData::ToExifBuff(UInt8 *buff, InOutParam<UInt32> startOfst, InOutParam<UInt32> otherOfst) const
{
	ToExifBuffImpl(buff, this->exifMap, startOfst, otherOfst);
}

void Media::EXIFData::GetExifBuffSize(OutParam<UInt64> size, OutParam<UInt64> endOfst) const
{
	GetExifBuffSize(this->exifMap, size, endOfst);
}

Optional<Media::EXIFData> Media::EXIFData::ParseMakerNote(const UInt8 *buff, UOSInt buffSize) const
{
	Optional<Media::EXIFData> ret = 0;
	if (Text::StrEquals(buff, (const UTF8Char*)"Panasonic"))
	{
		Data::ByteOrderLSB bo;
		ret = ParseIFD(&buff[12], buffSize - 12, bo, 0, Media::EXIFData::EM_PANASONIC, 0);
		return ret;
	}
	else if (Text::StrEquals(buff, (const UTF8Char*)"OLYMPUS"))
	{
		if (buff[8] == 'I' && buff[9] == 'I')
		{
			Data::ByteOrderLSB bo;
			ret = ParseIFD(&buff[12], buffSize - 12, bo, 0, Media::EXIFData::EM_OLYMPUS, &buff[8]);
			return ret;
		}
	}
	else if (Text::StrEquals(buff, (const UTF8Char*)"OLYMP"))
	{
		Data::ByteOrderLSB bo;
		ret = ParseIFD(&buff[8], buffSize - 8, bo, 0, Media::EXIFData::EM_OLYMPUS, 0);
		return ret;
	}
	else if (Text::StrEquals(buff, (const UTF8Char*)"Nikon"))
	{
		if (buff[6] == 2)
		{
			if (buff[10] == 'I' && buff[11] == 'I')
			{
				Data::ByteOrderLSB bo;
				ret = ParseIFD(&buff[18], buffSize - 18, bo, 0, Media::EXIFData::EM_NIKON3, &buff[10]);
				return ret;
			}
		}
	}
	else if (Text::StrEquals(buff, (const UTF8Char*)"QVC"))
	{
		Data::ByteOrderMSB bo;
		ret = ParseIFD(&buff[6], buffSize - 6, bo, 0, Media::EXIFData::EM_CASIO2, 0);
		return ret;
	}
	else if (Text::StrEquals(buff, (const UTF8Char*)"SANYO"))
	{
		Data::ByteOrderLSB bo;
		ret = ParseIFD(&buff[8], buffSize - 8, bo, 0, Media::EXIFData::EM_SANYO, 0);
		return ret;
	}
	else if (Text::StrEquals(buff, (const UTF8Char*)"Apple iOS"))
	{
		if (buff[12] == 'M' && buff[13] == 'M')
		{
			Data::ByteOrderMSB bo;
			ret = ParseIFD(&buff[14], buffSize - 14, bo, 0, Media::EXIFData::EM_APPLE, 0);
		}
		else
		{
			ret = 0;
		}
		return ret;
	}
	else
	{
		Text::CString maker = this->GetPhotoMake();
		if (maker.v)
		{
			if (maker.Equals(UTF8STRC("Canon")))
			{
				Data::ByteOrderLSB bo;
				ret = ParseIFD(buff, buffSize, bo, 0, Media::EXIFData::EM_CANON, 0);
				return ret;
			}
			else if (maker.Equals(UTF8STRC("CASIO")))
			{
				Data::ByteOrderMSB bo;
				ret = ParseIFD(buff, buffSize, bo, 0, Media::EXIFData::EM_CASIO1, 0);
				return ret;
			}
			else if (maker.Equals(UTF8STRC("FLIR Systems AB")))
			{
				Data::ByteOrderLSB bo;
				ret = ParseIFD(buff, buffSize, bo, 0, Media::EXIFData::EM_FLIR, 0);
				return ret;
			}
		}
	}
	return ret;
}

Text::CString Media::EXIFData::GetEXIFMakerName(EXIFMaker exifMaker)
{
	switch (exifMaker)
	{
	case Media::EXIFData::EM_PANASONIC:
		return CSTR("Panasonic");
	case Media::EXIFData::EM_CANON:
		return CSTR("Canon");
	case Media::EXIFData::EM_OLYMPUS:
		return CSTR("Olympus");
	case Media::EXIFData::EM_CASIO1:
		return CSTR("Casio Type 1");
	case Media::EXIFData::EM_CASIO2:
		return CSTR("Casio Type 2");
	case Media::EXIFData::EM_FLIR:
		return CSTR("FLIR");
	case Media::EXIFData::EM_NIKON3:
		return CSTR("Nikon Type 3");
	case Media::EXIFData::EM_SANYO:
		return CSTR("Sanyo Type 1");
	case Media::EXIFData::EM_APPLE:
		return CSTR("Apple");
	case Media::EXIFData::EM_STANDARD:
	default:
		return CSTR("Standard");
	}
}

Text::CStringNN Media::EXIFData::GetEXIFName(EXIFMaker exifMaker, UInt32 id)
{
	return GetEXIFName(exifMaker, 0, id);
}

Text::CStringNN Media::EXIFData::GetEXIFName(EXIFMaker exifMaker, UInt32 id, UInt32 subId)
{
	EXIFInfo *infos;
	OSInt cnt;
	if (id == 0)
	{
		switch (exifMaker)
		{
		case EM_PANASONIC:
			infos = panasonicInfos;
			cnt = sizeof(panasonicInfos) / sizeof(panasonicInfos[0]);
			break;
		case EM_CANON:
			infos = canonInfos;
			cnt = sizeof(canonInfos) / sizeof(canonInfos[0]);
			break;
		case EM_OLYMPUS:
			infos = olympusInfos;
			cnt = sizeof(olympusInfos) / sizeof(olympusInfos[0]);
			break;
		case EM_CASIO1:
			infos = casio1Infos;
			cnt = sizeof(casio1Infos) / sizeof(casio1Infos[0]);
			break;
		case EM_CASIO2:
			infos = casio2Infos;
			cnt = sizeof(casio2Infos) / sizeof(casio2Infos[0]);
			break;
		case EM_FLIR:
			infos = flirInfos;
			cnt = sizeof(flirInfos) / sizeof(flirInfos[0]);
			break;
		case EM_NIKON3:
			infos = nikon3Infos;
			cnt = sizeof(nikon3Infos) / sizeof(nikon3Infos[0]);
			break;
		case EM_SANYO:
			infos = sanyo1Infos;
			cnt = sizeof(sanyo1Infos) / sizeof(sanyo1Infos[0]);
			break;
		case EM_APPLE:
			infos = appleInfos;
			cnt = sizeof(appleInfos) / sizeof(appleInfos[0]);
			break;
		case EM_STANDARD:
		default:
			infos = defInfos;
			cnt = sizeof(defInfos) / sizeof(defInfos[0]);
			break;
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
			return CSTR("Unknown");
		}
	}
	else
	{
		return CSTR("Unknown");
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
			return infos[k].name;
		}
	}
	return CSTR("Unknown");
}

Text::CString Media::EXIFData::GetEXIFTypeName(Media::EXIFData::EXIFType type)
{
	switch (type)
	{
	case ET_BYTES:
		return CSTR("Bytes");
	case ET_STRING:
		return CSTR("String");
	case ET_UINT16:
		return CSTR("UInt16");
	case ET_UINT32:
		return CSTR("UInt32");
	case ET_RATIONAL:
		return CSTR("Rational");
	case ET_OTHER:
		return CSTR("Other");
	case ET_INT16:
		return CSTR("Int16");
	case ET_INT32:
		return CSTR("Int32");
	case ET_SUBEXIF:
		return CSTR("Exif");
	case ET_SRATIONAL:
		return CSTR("SRational");
	case ET_DOUBLE:
		return CSTR("Double");
	case ET_UINT64:
		return CSTR("UInt64");
	case ET_INT64:
		return CSTR("Int64");
	case ET_UNKNOWN:
	default:
		return CSTR("Unknown");
	};
}

Text::CString Media::EXIFData::GetFieldTypeName(UInt32 ftype)
{
	switch (ftype)
	{
	case 1:
		return CSTR("BYTE");
	case 2:
		return CSTR("ASCII");
	case 3:
		return CSTR("SHORT");
	case 4:
		return CSTR("LONG");
	case 5:
		return CSTR("RATIONAL");
	case 6:
		return CSTR("SBYTE");
	case 7:
		return CSTR("UNDEFINED");
	case 8:
		return CSTR("SSHORT");
	case 9:
		return CSTR("SLONG");
	case 10:
		return CSTR("SRATIONAL");
	case 11:
		return CSTR("FLOAT");
	case 12:
		return CSTR("DOUBLE");
	case 16:
		return CSTR("UINT64");
	case 17:
		return CSTR("INT64");
	case 18:
		return CSTR("IFD4");
	default:
		return CSTR("Unknown");
	}
}

Optional<Media::EXIFData> Media::EXIFData::ParseIFD(const UInt8 *buff, UOSInt buffSize, NotNullPtr<Data::ByteOrder> bo, UInt64 *nextOfst, EXIFMaker exifMaker, const UInt8 *basePtr)
{
	Media::EXIFData *exif;
	const UInt8 *ifdEntries;
	OSInt ifdCnt;
	OSInt i;
	OSInt ifdOfst;
	UInt32 tag;
	UInt32 ftype;
	UInt32 fcnt;

	ifdCnt = bo->GetUInt16(buff);
	ifdEntries = &buff[2];

	UInt8 *tmpBuff;
	UInt32 j;
	NEW_CLASS(exif, Media::EXIFData(exifMaker));

	if (basePtr == 0)
	{
		UInt32 readBase = 0x7fffffff;
		ifdOfst = 0;
		i = 0;
		while (i < ifdCnt)
		{
			tag = bo->GetUInt16(&ifdEntries[ifdOfst]);
			ftype = bo->GetUInt16(&ifdEntries[ifdOfst + 2]);
			fcnt = bo->GetUInt32(&ifdEntries[ifdOfst + 4]);

			if (ftype == 1)
			{
				if (fcnt <= 4)
				{
				}
				else
				{
					if (readBase > bo->GetUInt32(&ifdEntries[ifdOfst + 8]))
					{
						readBase = bo->GetUInt32(&ifdEntries[ifdOfst + 8]);
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
					if (readBase > bo->GetUInt32(&ifdEntries[ifdOfst + 8]))
					{
						readBase = bo->GetUInt32(&ifdEntries[ifdOfst + 8]);
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
					if (readBase > bo->GetUInt32(&ifdEntries[ifdOfst + 8]))
					{
						readBase = bo->GetUInt32(&ifdEntries[ifdOfst + 8]);
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
					if (readBase > bo->GetUInt32(&ifdEntries[ifdOfst + 8]))
					{
						readBase = bo->GetUInt32(&ifdEntries[ifdOfst + 8]);
					}
				}
			}
			else if (ftype == 5)
			{
				if (readBase > bo->GetUInt32(&ifdEntries[ifdOfst + 8]))
				{
					readBase = bo->GetUInt32(&ifdEntries[ifdOfst + 8]);
				}
			}
			else if (ftype == 7)
			{
				if (fcnt <= 4)
				{
				}
				else
				{
					if (readBase > bo->GetUInt32(&ifdEntries[ifdOfst + 8]))
					{
						readBase = bo->GetUInt32(&ifdEntries[ifdOfst + 8]);
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
					if (readBase > bo->GetUInt32(&ifdEntries[ifdOfst + 8]))
					{
						readBase = bo->GetUInt32(&ifdEntries[ifdOfst + 8]);
					}
				}
			}
			else if (ftype == 12)
			{
				if (readBase > bo->GetUInt32(&ifdEntries[ifdOfst + 8]))
				{
					readBase = bo->GetUInt32(&ifdEntries[ifdOfst + 8]);
				}
			}
			else if (ftype == 13)
			{
				if (readBase > bo->GetUInt32(&ifdEntries[ifdOfst + 8]))
				{
					readBase = bo->GetUInt32(&ifdEntries[ifdOfst + 8]);
				}
			}
			else
			{
				j = 0;
			}

			ifdOfst += 12;
			i++;
		}
		basePtr = &buff[(Int32)ifdCnt * 12 + 2 + 4 - (Int32)readBase];
	}

	ifdOfst = 0;
	i = 0;
	while (i < ifdCnt)
	{
		tag = bo->GetUInt16(&ifdEntries[ifdOfst]);
		ftype = bo->GetUInt16(&ifdEntries[ifdOfst + 2]);
		fcnt = bo->GetUInt32(&ifdEntries[ifdOfst + 4]);

		switch (ftype)
		{
		case 1:
			if (fcnt <= 4)
			{
				exif->AddBytes(tag, fcnt, (UInt8*)&ifdEntries[ifdOfst + 8]);
			}
			else
			{
				exif->AddBytes(tag, fcnt, &basePtr[bo->GetUInt32(&ifdEntries[ifdOfst + 8])]);
			}
			break;
		case 2:
			if (fcnt <= 4)
			{
				exif->AddString(tag, fcnt, (Char*)&ifdEntries[ifdOfst + 8]);
			}
			else
			{
				exif->AddString(tag, fcnt, (Char*)&basePtr[bo->GetUInt32(&ifdEntries[ifdOfst + 8])]);
			}
			break;
		case 3:
		{
			UInt16 tmp[2];
			if (fcnt == 1)
			{
				tmp[0] = bo->GetUInt16(&ifdEntries[ifdOfst + 8]);
				exif->AddUInt16(tag, fcnt, tmp);
			}
			else if (fcnt == 2)
			{
				tmp[0] = bo->GetUInt16(&ifdEntries[ifdOfst + 8]);
				tmp[1] = bo->GetUInt16(&ifdEntries[ifdOfst + 10]);
				exif->AddUInt16(tag, fcnt, tmp);
			}
			else
			{
				tmpBuff = MemAlloc(UInt8, fcnt << 1);
				MemCopyNO(tmpBuff, &basePtr[bo->GetUInt32(&ifdEntries[ifdOfst + 8])], fcnt << 1);
				j = fcnt << 1;
				while (j > 0)
				{
					j -= 2;
					*(UInt16*)&tmpBuff[j] = bo->GetUInt16(&tmpBuff[j]);
				}
				exif->AddUInt16(tag, fcnt, (UInt16*)tmpBuff);
				MemFree(tmpBuff);
			}
			break;
		}
		case 4:
		{
			UInt32 tmp;
			if (fcnt == 1)
			{
				tmp = bo->GetUInt32(&ifdEntries[ifdOfst + 8]);
				if (tag == 34665 || tag == 34853)
				{
					NotNullPtr<Media::EXIFData> subexif;
					if (ParseIFD(&basePtr[tmp], buffSize - (UOSInt)(&basePtr[tmp] - buff), bo, 0, exifMaker, basePtr).SetTo(subexif))
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
				MemCopyNO(tmpBuff, &basePtr[bo->GetUInt32(&ifdEntries[ifdOfst + 8])], fcnt << 2);
				j = fcnt << 2;
				while (j > 0)
				{
					j -= 4;
					*(UInt32*)&tmpBuff[j] = bo->GetUInt32(&tmpBuff[j]);
				}
				exif->AddUInt32(tag, fcnt, (UInt32*)tmpBuff);
				MemFree(tmpBuff);
			}
			break;
		}
		case 5:
			tmpBuff = MemAlloc(UInt8, fcnt << 3);
			MemCopyNO(tmpBuff, &basePtr[bo->GetUInt32(&ifdEntries[ifdOfst + 8])], fcnt << 3);
			j = fcnt << 3;
			while (j > 0)
			{
				j -= 8;
				*(UInt32*)&tmpBuff[j] = bo->GetUInt32(&tmpBuff[j]);
				*(UInt32*)&tmpBuff[j + 4] = bo->GetUInt32(&tmpBuff[j + 4]);
			}
			exif->AddRational(tag, fcnt, (UInt32*)tmpBuff);
			MemFree(tmpBuff);
			break;
		case 7:
			if (fcnt <= 4)
			{
				exif->AddOther(tag, fcnt, (UInt8*)&ifdEntries[ifdOfst + 8]);
			}
			else
			{
				UOSInt ofst = bo->GetUInt32(&ifdEntries[ifdOfst + 8]);
				if (ofst + fcnt > buffSize)
				{
					ofst = buffSize - fcnt;
				}
				exif->AddOther(tag, fcnt, &basePtr[ofst]);
			}
			break;
		case 8:
		{
			Int16 tmp[2];
			if (fcnt == 1)
			{
				tmp[0] = bo->GetInt16(&ifdEntries[ifdOfst + 8]);
				exif->AddInt16(tag, fcnt, tmp);
			}
			else if (fcnt == 2)
			{
				tmp[0] = bo->GetInt16(&ifdEntries[ifdOfst + 8]);
				tmp[1] = bo->GetInt16(&ifdEntries[ifdOfst + 10]);
				exif->AddInt16(tag, fcnt, tmp);
			}
			else
			{
				tmpBuff = MemAlloc(UInt8, fcnt << 1);
				MemCopyNO(tmpBuff, &basePtr[bo->GetUInt32(&ifdEntries[ifdOfst + 8])], fcnt << 1);
				j = fcnt << 1;
				while (j > 0)
				{
					j -= 2;
					*(Int16*)&tmpBuff[j] = bo->GetInt16(&tmpBuff[j]);
				}
				exif->AddInt16(tag, fcnt, (Int16*)tmpBuff);
				MemFree(tmpBuff);
			}
			break;
		}
		case 9:
			if (fcnt == 1)
			{
				Int32 v = bo->GetInt32(&ifdEntries[ifdOfst + 8]);
				exif->AddInt32(tag, fcnt, &v);
			}
			else
			{
				tmpBuff = MemAlloc(UInt8, fcnt << 2);
				MemCopyNO(tmpBuff, &basePtr[bo->GetUInt32(&ifdEntries[ifdOfst + 8])], fcnt << 2);
				j = fcnt << 2;
				while (j > 0)
				{
					j -= 4;
					*(Int32*)&tmpBuff[j] = bo->GetInt32(&tmpBuff[j]);
				}
				exif->AddInt32(tag, fcnt, (Int32*)tmpBuff);
				MemFree(tmpBuff);
			}
			break;
		case 10:
			tmpBuff = MemAlloc(UInt8, fcnt << 3);
			MemCopyNO(tmpBuff, &basePtr[bo->GetUInt32(&ifdEntries[ifdOfst + 8])], fcnt << 3);
			j = fcnt << 3;
			while (j > 0)
			{
				j -= 8;
				*(Int32*)&tmpBuff[j] = bo->GetInt32(&tmpBuff[j]);
				*(Int32*)&tmpBuff[j + 4] = bo->GetInt32(&tmpBuff[j + 4]);
			}
			exif->AddSRational(tag, fcnt, (Int32*)tmpBuff);
			MemFree(tmpBuff);
			break;
		case 12:
			tmpBuff = MemAlloc(UInt8, fcnt << 3);
			MemCopyNO(tmpBuff, &basePtr[bo->GetUInt32(&ifdEntries[ifdOfst + 8])], fcnt << 3);
			j = fcnt << 3;
			while (j > 0)
			{
				j -= 8;
				*(Double*)&tmpBuff[j] = bo->GetFloat64(&tmpBuff[j]);
			}
			exif->AddDouble(tag, fcnt, (Double*)tmpBuff);
			MemFree(tmpBuff);
			break;
		case 13: //Olympus innerIFD
		{
			NotNullPtr<Media::EXIFData> subexif;
			if (ParseIFD(&basePtr[bo->GetUInt32(&ifdEntries[ifdOfst + 8])], buffSize - (UOSInt)(&basePtr[bo->GetUInt32(&ifdEntries[ifdOfst + 8])] - buff), bo, 0, exifMaker, basePtr).SetTo(subexif))
			{
				exif->AddSubEXIF(tag, subexif);
			}
			break;
		}
		default:
			printf("EXIF ParseIFD: Unknown type %d, tag = %d, cnt = %d\r\n", ftype, tag, fcnt);
			break;
		}

		ifdOfst += 12;
		i++;
	}

	if (nextOfst)
	{
		*nextOfst = bo->GetUInt32(&ifdEntries[ifdCnt * 12]);
	}
	return exif;
}

Optional<Media::EXIFData> Media::EXIFData::ParseIFD(NotNullPtr<IO::StreamData> fd, UInt64 ofst, NotNullPtr<Data::ByteOrder> bo, UInt64 *nextOfst, UInt64 readBase)
{
	NotNullPtr<Media::EXIFData> exif;
	UInt8 ifdBuff[2];
	UOSInt ifdCnt;
	UOSInt i;
	UOSInt readSize;
	OSInt ifdOfst;
	UInt32 tag;
	UInt32 ftype;
	UInt32 fcnt;
	if (fd->GetRealData(ofst, 2, BYTEARR(ifdBuff)) != 2)
	{
		return 0;
	}
	ifdCnt = bo->GetUInt16(ifdBuff);

	Data::ByteBuffer ifdEntries(readSize = ifdCnt * 12 + 4);
	if (fd->GetRealData(ofst + 2, readSize, ifdEntries) != readSize)
	{
		return 0;
	}

	UInt32 j;
	NEW_CLASSNN(exif, Media::EXIFData(Media::EXIFData::EM_STANDARD));

	ifdOfst = 0;
	i = 0;
	while (i < ifdCnt)
	{
		tag = bo->GetUInt16(&ifdEntries[ifdOfst]);
		ftype = bo->GetUInt16(&ifdEntries[ifdOfst + 2]);
		fcnt = bo->GetUInt32(&ifdEntries[ifdOfst + 4]);

		switch (ftype)
		{
		case 1:
			if (fcnt <= 4)
			{
				exif->AddBytes(tag, fcnt, (UInt8*)&ifdEntries[ifdOfst + 8]);
			}
			else
			{
				Data::ByteBuffer tmpBuff(fcnt);
				fd->GetRealData(bo->GetUInt32(&ifdEntries[ifdOfst + 8]) + readBase, fcnt, tmpBuff);
				exif->AddBytes(tag, fcnt, tmpBuff.Ptr());
			}
			break;
		case 2:
			if (fcnt <= 4)
			{
				exif->AddString(tag, fcnt, (Char*)&ifdEntries[ifdOfst + 8]);
			}
			else
			{
				Data::ByteBuffer tmpBuff(fcnt);
				fd->GetRealData(bo->GetUInt32(&ifdEntries[ifdOfst + 8]) + readBase, fcnt, tmpBuff);
				exif->AddString(tag, fcnt, (Char*)tmpBuff.Ptr());
			}
			break;
		case 3:
		{
			UInt16 tmp[2];
			if (fcnt == 1)
			{
				tmp[0] = bo->GetUInt16(&ifdEntries[ifdOfst + 8]);
				exif->AddUInt16(tag, fcnt, tmp);
			}
			else if (fcnt == 2)
			{
				tmp[0] = bo->GetUInt16(&ifdEntries[ifdOfst + 8]);
				tmp[1] = bo->GetUInt16(&ifdEntries[ifdOfst + 10]);
				exif->AddUInt16(tag, fcnt, tmp);
			}
			else
			{
				Data::ByteBuffer tmpBuff(fcnt << 1);
				fd->GetRealData(bo->GetUInt32(&ifdEntries[ifdOfst + 8]) + readBase, fcnt << 1, tmpBuff);
				j = fcnt << 1;
				while (j > 0)
				{
					j -= 2;
					*(UInt16*)&tmpBuff[j] = bo->GetUInt16(&tmpBuff[j]);
				}
				exif->AddUInt16(tag, fcnt, (UInt16*)tmpBuff.Ptr());
			}
			break;
		}
		case 4:
		{
			UInt32 tmp;
			if (fcnt == 1)
			{
				tmp = bo->GetUInt32(&ifdEntries[ifdOfst + 8]);
				if (tag == 34665 || tag == 34853)
				{
					NotNullPtr<Media::EXIFData> subexif;
					if (ParseIFD(fd, tmp + readBase, bo, 0, readBase).SetTo(subexif))
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
				Data::ByteBuffer tmpBuff(fcnt << 2);
				fd->GetRealData(bo->GetUInt32(&ifdEntries[ifdOfst + 8]) + readBase, fcnt << 2, tmpBuff);
				j = fcnt << 2;
				while (j > 0)
				{
					j -= 4;
					*(UInt32*)&tmpBuff[j] = bo->GetUInt32(&tmpBuff[j]);
				}
				exif->AddUInt32(tag, fcnt, (UInt32*)tmpBuff.Ptr());
			}
			break;
		}
		case 5:
			{
				Data::ByteBuffer tmpBuff(fcnt << 3);
				fd->GetRealData(bo->GetUInt32(&ifdEntries[ifdOfst + 8]) + readBase, fcnt << 3, tmpBuff);
				j = fcnt << 3;
				while (j > 0)
				{
					j -= 8;
					*(UInt32*)&tmpBuff[j] = bo->GetUInt32(&tmpBuff[j]);
					*(UInt32*)&tmpBuff[j + 4] = bo->GetUInt32(&tmpBuff[j + 4]);
				}
				exif->AddRational(tag, fcnt, (UInt32*)tmpBuff.Ptr());
			}
			break;
		case 7:
			if (fcnt <= 4)
			{
				exif->AddOther(tag, fcnt, (UInt8*)&ifdEntries[ifdOfst + 8]);
			}
			else
			{
				Data::ByteBuffer tmpBuff(fcnt);
				fd->GetRealData(bo->GetUInt32(&ifdEntries[ifdOfst + 8]) + readBase, fcnt, tmpBuff);
				exif->AddOther(tag, fcnt, tmpBuff.Ptr());
			}
			break;
		case 8:
		{
			Int16 tmp;
			if (fcnt == 1)
			{
				tmp = bo->GetInt16(&ifdEntries[ifdOfst + 8]);
				exif->AddInt16(tag, fcnt, &tmp);
			}
			else
			{
				Data::ByteBuffer tmpBuff(fcnt << 1);
				fd->GetRealData(bo->GetUInt32(&ifdEntries[ifdOfst + 8]) + readBase, fcnt << 1, tmpBuff);
				j = fcnt << 1;
				while (j > 0)
				{
					j -= 2;
					*(Int16*)&tmpBuff[j] = bo->GetInt16(&tmpBuff[j]);
				}
				exif->AddInt16(tag, fcnt, (Int16*)tmpBuff.Ptr());
			}
			break;
		}
		case 9:
		{
			Int32 tmp;
			if (fcnt == 1)
			{
				tmp = bo->GetInt32(&ifdEntries[ifdOfst + 8]);
				exif->AddInt32(tag, fcnt, &tmp);
			}
			else
			{
				Data::ByteBuffer tmpBuff(fcnt << 2);
				fd->GetRealData(bo->GetUInt32(&ifdEntries[ifdOfst + 8]) + readBase, fcnt << 2, tmpBuff);
				j = fcnt << 2;
				while (j > 0)
				{
					j -= 4;
					*(Int32*)&tmpBuff[j] = bo->GetInt32(&tmpBuff[j]);
				}
				exif->AddInt32(tag, fcnt, (Int32*)tmpBuff.Ptr());
			}
			break;
		}
		case 10:
		{
			Data::ByteBuffer tmpBuff(fcnt << 3);
			fd->GetRealData(bo->GetUInt32(&ifdEntries[ifdOfst + 8]) + readBase, fcnt << 3, tmpBuff);
			j = fcnt << 3;
			while (j > 0)
			{
				j -= 8;
				*(Int32*)&tmpBuff[j] = bo->GetInt32(&tmpBuff[j]);
				*(Int32*)&tmpBuff[j + 4] = bo->GetInt32(&tmpBuff[j + 4]);
			}
			exif->AddSRational(tag, fcnt, (Int32*)tmpBuff.Ptr());
		}
			break;
		case 12:
			{
				Data::ByteBuffer tmpBuff(fcnt << 3);
				fd->GetRealData(bo->GetUInt32(&ifdEntries[ifdOfst + 8]) + readBase, fcnt << 3, tmpBuff);
				exif->AddDouble(tag, fcnt, (Double*)tmpBuff.Ptr());
			}
			break;
		default:
			printf("EXIFData.ParseIFD: Unsupported field type: %d, tag = %d\r\n", ftype, tag);
			j = 0;
			break;
		}

		ifdOfst += 12;
		i++;
	}

	if (nextOfst)
	{
		*nextOfst = bo->GetUInt32(&ifdEntries[ifdCnt * 12]);
	}
	return exif;
}

Optional<Media::EXIFData> Media::EXIFData::ParseIFD64(NotNullPtr<IO::StreamData> fd, UInt64 ofst, NotNullPtr<Data::ByteOrder> bo, UInt64 *nextOfst, UInt64 readBase)
{
	NotNullPtr<Media::EXIFData> exif;
	UInt8 ifdBuff[8];
	UInt64 ifdCnt;
	UOSInt i;
	UOSInt readSize;
	OSInt ifdOfst;
	UInt32 tag;
	UInt32 ftype;
	UInt64 fcnt;
	if (fd->GetRealData(ofst, 8, BYTEARR(ifdBuff)) != 8)
	{
		return 0;
	}
	ifdCnt = bo->GetUInt64(ifdBuff);

	Data::ByteBuffer ifdEntries(readSize = ifdCnt * 20 + 8);
	if (fd->GetRealData(ofst + 8, readSize, ifdEntries) != readSize)
	{
		return 0;
	}

	UOSInt j;
	NEW_CLASSNN(exif, Media::EXIFData(Media::EXIFData::EM_STANDARD));

	ifdOfst = 0;
	i = 0;
	while (i < ifdCnt)
	{
		tag = bo->GetUInt16(&ifdEntries[ifdOfst]);
		ftype = bo->GetUInt16(&ifdEntries[ifdOfst + 2]);
		fcnt = bo->GetUInt64(&ifdEntries[ifdOfst + 4]);

		switch (ftype)
		{
		case 1:
			if (fcnt <= 8)
			{
				exif->AddBytes(tag, fcnt, (UInt8*)&ifdEntries[ifdOfst + 12]);
			}
			else
			{
				Data::ByteBuffer tmpBuff(fcnt);
				fd->GetRealData(bo->GetUInt64(&ifdEntries[ifdOfst + 12]) + readBase, fcnt, tmpBuff);
				exif->AddBytes(tag, fcnt, tmpBuff.Ptr());
			}
			break;
		case 2:
			if (fcnt <= 8)
			{
				exif->AddString(tag, fcnt, (Char*)&ifdEntries[ifdOfst + 12]);
			}
			else
			{
				Data::ByteBuffer tmpBuff(fcnt);
				fd->GetRealData(bo->GetUInt64(&ifdEntries[ifdOfst + 12]) + readBase, fcnt, tmpBuff);
				exif->AddString(tag, fcnt, (Char*)tmpBuff.Ptr());
			}
			break;
		case 3:
		{
			UInt16 tmp[4];
			if (fcnt == 1)
			{
				tmp[0] = bo->GetUInt16(&ifdEntries[ifdOfst + 12]);
				exif->AddUInt16(tag, fcnt, tmp);
			}
			else if (fcnt == 2)
			{
				tmp[0] = bo->GetUInt16(&ifdEntries[ifdOfst + 12]);
				tmp[1] = bo->GetUInt16(&ifdEntries[ifdOfst + 14]);
				exif->AddUInt16(tag, fcnt, tmp);
			}
			else if (fcnt == 3)
			{
				tmp[0] = bo->GetUInt16(&ifdEntries[ifdOfst + 12]);
				tmp[1] = bo->GetUInt16(&ifdEntries[ifdOfst + 14]);
				tmp[2] = bo->GetUInt16(&ifdEntries[ifdOfst + 16]);
				exif->AddUInt16(tag, fcnt, tmp);
			}
			else if (fcnt == 4)
			{
				tmp[0] = bo->GetUInt16(&ifdEntries[ifdOfst + 12]);
				tmp[1] = bo->GetUInt16(&ifdEntries[ifdOfst + 14]);
				tmp[2] = bo->GetUInt16(&ifdEntries[ifdOfst + 16]);
				tmp[3] = bo->GetUInt16(&ifdEntries[ifdOfst + 18]);
				exif->AddUInt16(tag, fcnt, tmp);
			}
			else
			{
				Data::ByteBuffer tmpBuff(fcnt << 1);
				fd->GetRealData(bo->GetUInt64(&ifdEntries[ifdOfst + 12]) + readBase, fcnt << 1, tmpBuff);
				j = fcnt << 1;
				while (j > 0)
				{
					j -= 2;
					*(UInt16*)&tmpBuff[j] = bo->GetUInt16(&tmpBuff[j]);
				}
				exif->AddUInt16(tag, fcnt, (UInt16*)tmpBuff.Ptr());
			}
			break;
		}
		case 4:
		{
			UInt32 tmp[2];
			if (fcnt == 1)
			{
				tmp[0] = bo->GetUInt32(&ifdEntries[ifdOfst + 12]);
				if (tag == 34665 || tag == 34853)
				{
					NotNullPtr<Media::EXIFData> subexif;
					if (ParseIFD(fd, tmp[0] + readBase, bo, 0, readBase).SetTo(subexif))
					{
						exif->AddSubEXIF(tag, subexif);
					}
				}
				else
				{
					exif->AddUInt32(tag, fcnt, tmp);
				}
			}
			else if (fcnt == 2)
			{
				tmp[0] = bo->GetUInt32(&ifdEntries[ifdOfst + 12]);
				tmp[1] = bo->GetUInt32(&ifdEntries[ifdOfst + 16]);
				exif->AddUInt32(tag, fcnt, tmp);
			}
			else
			{
				Data::ByteBuffer tmpBuff(fcnt << 2);
				fd->GetRealData(bo->GetUInt64(&ifdEntries[ifdOfst + 12]) + readBase, fcnt << 2, tmpBuff);
				j = fcnt << 2;
				while (j > 0)
				{
					j -= 4;
					*(UInt32*)&tmpBuff[j] = bo->GetUInt32(&tmpBuff[j]);
				}
				exif->AddUInt32(tag, fcnt, (UInt32*)tmpBuff.Ptr());
			}
			break;
		}
		case 5:
			if (fcnt == 1)
			{
				UInt32 tmp[2];
				tmp[0] = bo->GetUInt32(&ifdEntries[ifdOfst + 12]);
				tmp[1] = bo->GetUInt32(&ifdEntries[ifdOfst + 16]);
				exif->AddRational(tag, fcnt, tmp);
			}
			else
			{
				Data::ByteBuffer tmpBuff(fcnt << 3);
				fd->GetRealData(bo->GetUInt64(&ifdEntries[ifdOfst + 12]) + readBase, fcnt << 3, tmpBuff);
				j = fcnt << 3;
				while (j > 0)
				{
					j -= 8;
					*(UInt32*)&tmpBuff[j] = bo->GetUInt32(&tmpBuff[j]);
					*(UInt32*)&tmpBuff[j + 4] = bo->GetUInt32(&tmpBuff[j + 4]);
				}
				exif->AddRational(tag, fcnt, (UInt32*)tmpBuff.Ptr());
			}
			break;
		case 7:
			if (fcnt <= 8)
			{
				exif->AddOther(tag, fcnt, (UInt8*)&ifdEntries[ifdOfst + 12]);
			}
			else
			{
				Data::ByteBuffer tmpBuff(fcnt);
				fd->GetRealData(bo->GetUInt64(&ifdEntries[ifdOfst + 12]) + readBase, fcnt, tmpBuff);
				exif->AddOther(tag, fcnt, tmpBuff.Ptr());
			}
			break;
		case 8:
		{
			Int16 tmp[4];
			if (fcnt == 1)
			{
				tmp[0] = bo->GetInt16(&ifdEntries[ifdOfst + 12]);
				exif->AddInt16(tag, fcnt, tmp);
			}
			else if (fcnt == 2)
			{
				tmp[0] = bo->GetInt16(&ifdEntries[ifdOfst + 12]);
				tmp[1] = bo->GetInt16(&ifdEntries[ifdOfst + 14]);
				exif->AddInt16(tag, fcnt, tmp);
			}
			else if (fcnt == 3)
			{
				tmp[0] = bo->GetInt16(&ifdEntries[ifdOfst + 12]);
				tmp[1] = bo->GetInt16(&ifdEntries[ifdOfst + 14]);
				tmp[2] = bo->GetInt16(&ifdEntries[ifdOfst + 16]);
				exif->AddInt16(tag, fcnt, tmp);
			}
			else if (fcnt == 4)
			{
				tmp[0] = bo->GetInt16(&ifdEntries[ifdOfst + 12]);
				tmp[1] = bo->GetInt16(&ifdEntries[ifdOfst + 14]);
				tmp[2] = bo->GetInt16(&ifdEntries[ifdOfst + 16]);
				tmp[3] = bo->GetInt16(&ifdEntries[ifdOfst + 18]);
				exif->AddInt16(tag, fcnt, tmp);
			}
			else
			{
				Data::ByteBuffer tmpBuff(fcnt << 1);
				fd->GetRealData(bo->GetUInt64(&ifdEntries[ifdOfst + 12]) + readBase, fcnt << 1, tmpBuff);
				j = fcnt << 1;
				while (j > 0)
				{
					j -= 2;
					*(Int16*)&tmpBuff[j] = bo->GetInt16(&tmpBuff[j]);
				}
				exif->AddInt16(tag, fcnt, (Int16*)tmpBuff.Ptr());
			}
			break;
		}
		case 12:
			if (fcnt == 1)
			{
				Double tmp = bo->GetFloat64(&ifdEntries[ifdOfst + 12]);
				exif->AddDouble(tag, fcnt, &tmp);
			}
			else
			{
				Data::ByteBuffer tmpBuff(fcnt << 3);
				fd->GetRealData(bo->GetUInt64(&ifdEntries[ifdOfst + 12]) + readBase, fcnt << 3, tmpBuff);
				j = fcnt << 3;
				while (j > 0)
				{
					j -= 8;
					*(Double*)&tmpBuff[j] = bo->GetFloat64(&tmpBuff[j]);
				}
				exif->AddDouble(tag, fcnt, (Double*)tmpBuff.Ptr());
			}
			break;
		case 16: //TIFF_LONG8
			if (fcnt == 1)
			{
				UInt64 tmp = bo->GetUInt64(&ifdEntries[ifdOfst + 12]);
				exif->AddUInt64(tag, fcnt, &tmp);
			}
			else
			{
				Data::ByteBuffer tmpBuff(fcnt << 3);
				fd->GetRealData(bo->GetUInt64(&ifdEntries[ifdOfst + 12]) + readBase, fcnt << 3, tmpBuff);
				j = fcnt << 3;
				while (j > 0)
				{
					j -= 8;
					*(UInt64*)&tmpBuff[j] = bo->GetUInt64(&tmpBuff[j]);
				}
				exif->AddUInt64(tag, fcnt, (UInt64*)tmpBuff.Ptr());
			}
			break;
		case 17: //TIFF_SLONG8
			if (fcnt == 1)
			{
				Int64 tmp = bo->GetInt64(&ifdEntries[ifdOfst + 12]);
				exif->AddInt64(tag, fcnt, &tmp);
			}
			else
			{
				Data::ByteBuffer tmpBuff(fcnt << 3);
				fd->GetRealData(bo->GetUInt64(&ifdEntries[ifdOfst + 12]) + readBase, fcnt << 3, tmpBuff);
				j = fcnt << 3;
				while (j > 0)
				{
					j -= 8;
					*(Int64*)&tmpBuff[j] = bo->GetInt64(&tmpBuff[j]);
				}
				exif->AddInt64(tag, fcnt, (Int64*)tmpBuff.Ptr());
			}
			break;
		case 18: //TIFF_IFD8
			if (fcnt == 1)
			{
				UInt64 tmp = bo->GetUInt64(&ifdEntries[ifdOfst + 12]);
				NotNullPtr<Media::EXIFData> subexif;
				if (ParseIFD(fd, tmp + readBase, bo, 0, readBase).SetTo(subexif))
				{
					exif->AddSubEXIF(tag, subexif);
				}
				else
				{
					exif->AddUInt64(tag, fcnt, &tmp);
				}
			}
			else
			{
				printf("EXIFData.ParseIFD64: Unsupported IFD8 type\r\n");
			}
			break;
		default:
			printf("EXIFData.ParseIFD64: Unsupported field type: %d, tag = %d\r\n", ftype, tag);
			j = 0;
			break;
		}

		ifdOfst += 20;
		i++;
	}

	if (nextOfst)
	{
		*nextOfst = bo->GetUInt32(&ifdEntries[(UOSInt)(ifdCnt * 12)]);
	}
	return exif;
}

Bool Media::EXIFData::ParseEXIFFrame(NotNullPtr<IO::FileAnalyse::FrameDetailHandler> frame, UOSInt frameOfst, NotNullPtr<IO::StreamData> fd, UInt64 ofst)
{
	UInt8 hdr[8];
	if (fd->GetRealData(ofst, 8, BYTEARR(hdr)) != 8)
	{
		return false;
	}
	NotNullPtr<Data::ByteOrder> bo;
	if (*(Int16*)&hdr[0] == *(Int16*)"II")
	{
		NEW_CLASSNN(bo, Data::ByteOrderLSB());
	}
	else if (*(Int16*)&hdr[0] == *(Int16*)"MM")
	{
		NEW_CLASSNN(bo, Data::ByteOrderMSB());
	}
	else
	{
		return false;
	}
	if (bo->GetUInt16(&hdr[2]) != 42)
	{
		bo.Delete();
		return false;
	}
	if (bo->GetUInt32(&hdr[4]) != 8)
	{
		bo.Delete();
		return false;
	}
	frame->AddStrC(frameOfst, 2, CSTR("Byte Order"), hdr);
	frame->AddUInt(frameOfst + 2, 2, CSTR("Signature"), 42);
	frame->AddUInt(frameOfst + 4, 4, CSTR("IFD Offset"), 8);
	UInt32 nextOfst = 8;
	UOSInt i = 3;
	while (i-- > 0)
	{
		if (!Media::EXIFData::ParseFrame(frame, frameOfst + nextOfst, fd, ofst + nextOfst, bo, &nextOfst, 0, ofst))
		{
			bo.Delete();
			return false;
		}
		if (nextOfst == 0)
		{
			bo.Delete();
			return true;
		}
	}
	bo.Delete();
	return true;
}

Bool Media::EXIFData::ParseFrame(NotNullPtr<IO::FileAnalyse::FrameDetailHandler> frame, UOSInt frameOfst, NotNullPtr<IO::StreamData> fd, UInt64 ofst, NotNullPtr<Data::ByteOrder> bo, UInt32 *nextOfst, UInt32 ifdId, UInt64 readBase)
{
	UInt8 ifdBuff[2];
	UOSInt ifdCnt;
	UOSInt i;
	UOSInt readSize;
	UOSInt ifdOfst;
	UInt32 tag;
	UInt32 ftype;
	UInt32 fcnt;
	UInt32 fofst;
	if (fd->GetRealData(ofst, 2, BYTEARR(ifdBuff)) != 2)
	{
		return 0;
	}
	ifdCnt = bo->GetUInt16(ifdBuff);
	frame->AddUInt(frameOfst, 2, CSTR("IFD Count"), ifdCnt);

	Data::ByteBuffer ifdEntries(readSize = ifdCnt * 12 + 4);
	if (fd->GetRealData(ofst + 2, readSize, ifdEntries) != readSize)
	{
		return 0;
	}

	UInt32 j;

	ifdOfst = 0;
	i = 0;
	while (i < ifdCnt)
	{
		tag = bo->GetUInt16(&ifdEntries[ifdOfst]);
		ftype = bo->GetUInt16(&ifdEntries[ifdOfst + 2]);
		fcnt = bo->GetUInt32(&ifdEntries[ifdOfst + 4]);
		frame->AddUIntName(frameOfst + 2 + ifdOfst, 2, CSTR("Tag"), tag, GetEXIFName(EM_STANDARD, ifdId, tag));
		frame->AddUIntName(frameOfst + 4 + ifdOfst, 2, CSTR("Field Type"), ftype, GetFieldTypeName(ftype));
		frame->AddUInt(frameOfst + 6 + ifdOfst, 4, CSTR("Field Count"), fcnt);

		switch (ftype)
		{
		case 1:
			if (fcnt < 4)
			{
				frame->AddHexBuff(frameOfst + 10 + ifdOfst, fcnt, GetEXIFName(EM_STANDARD, ifdId, tag), &ifdEntries[ifdOfst + 8], false);
				frame->AddHexBuff(frameOfst + 10 + fcnt + ifdOfst, 4 - fcnt, CSTR("Reserved"), &ifdEntries[ifdOfst + 8 + fcnt], false);
			}
			else if (fcnt == 4)
			{
				frame->AddHexBuff(frameOfst + 10 + ifdOfst, fcnt, GetEXIFName(EM_STANDARD, ifdId, tag), &ifdEntries[ifdOfst + 8], false);
			}
			else
			{
				fofst = bo->GetUInt32(&ifdEntries[ifdOfst + 8]);
				frame->AddUInt(frameOfst + 10 + ifdOfst, 4, CSTR("Field Offset"), fofst);
				Data::ByteBuffer tmpBuff(fcnt);
				fd->GetRealData(fofst + readBase, fcnt, tmpBuff);
				frame->AddHexBuff((UOSInt)(fofst + readBase - ofst + frameOfst), GetEXIFName(EM_STANDARD, ifdId, tag), tmpBuff, true);
			}
			break;
		case 2:
			if (fcnt < 4)
			{
				frame->AddStrC(frameOfst + 10 + ifdOfst, fcnt, GetEXIFName(EM_STANDARD, ifdId, tag), &ifdEntries[ifdOfst + 8]);
				frame->AddHexBuff(frameOfst + 10 + fcnt + ifdOfst, 4 - fcnt, CSTR("Reserved"), &ifdEntries[ifdOfst + 8 + fcnt], false);
			}
			else if (fcnt == 4)
			{
				frame->AddStrC(frameOfst + 10 + ifdOfst, fcnt, GetEXIFName(EM_STANDARD, ifdId, tag), &ifdEntries[ifdOfst + 8]);
			}
			else
			{
				fofst = bo->GetUInt32(&ifdEntries[ifdOfst + 8]);
				frame->AddUInt(frameOfst + 10 + ifdOfst, 4, CSTR("Field Offset"), fofst);
				Data::ByteBuffer tmpBuff(fcnt);
				fd->GetRealData(fofst + readBase, fcnt, tmpBuff);
				frame->AddStrS((UOSInt)(fofst + readBase - ofst + frameOfst), fcnt, GetEXIFName(EM_STANDARD, ifdId, tag), tmpBuff.Ptr());
			}
			break;
		case 3:
			if (fcnt == 1)
			{
				frame->AddUInt(frameOfst + 10 + ifdOfst, 2, GetEXIFName(EM_STANDARD, ifdId, tag), bo->GetUInt16(&ifdEntries[ifdOfst + 8]));
				frame->AddUInt(frameOfst + 12 + ifdOfst, 2, CSTR("Reserved"), bo->GetUInt16(&ifdEntries[ifdOfst + 10]));
			}
			else if (fcnt == 2)
			{
				frame->AddUInt(frameOfst + 10 + ifdOfst, 2, GetEXIFName(EM_STANDARD, ifdId, tag), bo->GetUInt16(&ifdEntries[ifdOfst + 8]));
				frame->AddUInt(frameOfst + 12 + ifdOfst, 2, GetEXIFName(EM_STANDARD, ifdId, tag), bo->GetUInt16(&ifdEntries[ifdOfst + 10]));
			}
			else
			{
				fofst = bo->GetUInt32(&ifdEntries[ifdOfst + 8]);
				frame->AddUInt(frameOfst + 10 + ifdOfst, 4, CSTR("Field Offset"), fofst);
				Data::ByteBuffer tmpBuff(fcnt << 1);
				fd->GetRealData(fofst + readBase, fcnt << 1, tmpBuff);
				j = fcnt << 1;
				while (j > 0)
				{
					j -= 2;
					frame->AddUInt((UOSInt)(fofst + readBase + j - ofst + frameOfst), 2, GetEXIFName(EM_STANDARD, ifdId, tag), bo->GetUInt16(&tmpBuff[j]));
				}
			}
			break;
		case 4:
			if (fcnt == 1)
			{
				fofst = bo->GetUInt32(&ifdEntries[ifdOfst + 8]);
				if (tag == 34665 || tag == 34853)
				{
					frame->AddUInt(frameOfst + 10 + ifdOfst, 4, CSTR("Field Offset"), fofst);
					ParseFrame(frame, (UOSInt)(fofst + readBase - ofst + frameOfst), fd, fofst + readBase, bo, 0, tag, readBase);
				}
				else
				{
					frame->AddUInt(frameOfst + 10 + ifdOfst, 4, GetEXIFName(EM_STANDARD, ifdId, tag), fofst);
				}
			}
			else
			{
				fofst = bo->GetUInt32(&ifdEntries[ifdOfst + 8]);
				frame->AddUInt(frameOfst + 10 + ifdOfst, 4, CSTR("Field Offset"), fofst);
				Data::ByteBuffer tmpBuff(fcnt << 2);
				fd->GetRealData(fofst + readBase, fcnt << 2, tmpBuff);
				j = fcnt << 2;
				while (j > 0)
				{
					j -= 4;
					frame->AddUInt((UOSInt)(fofst + readBase + j - ofst + frameOfst), 4, GetEXIFName(EM_STANDARD, ifdId, tag), bo->GetUInt32(&tmpBuff[j]));
				}
			}
			break;
		case 5:
		{
			Text::StringBuilderUTF8 sb;
			fofst = bo->GetUInt32(&ifdEntries[ifdOfst + 8]);
			frame->AddUInt(frameOfst + 10 + ifdOfst, 4, CSTR("Field Offset"), fofst);
			Data::ByteBuffer tmpBuff(fcnt << 3);
			fd->GetRealData(fofst + readBase, fcnt << 3, tmpBuff);
			j = fcnt << 3;
			while (j > 0)
			{
				j -= 8;
				sb.ClearStr();
				sb.AppendU32(bo->GetUInt32(&tmpBuff[j]));
				sb.AppendC(UTF8STRC(" / "));
				sb.AppendU32(bo->GetUInt32(&tmpBuff[j + 4]));
				frame->AddField((UOSInt)(fofst + readBase + j - ofst + frameOfst), 8, GetEXIFName(EM_STANDARD, ifdId, tag), sb.ToCString());
			}
			break;
		}
		case 7: //Other
			if (fcnt < 4)
			{
				frame->AddHexBuff(frameOfst + 10 + ifdOfst, fcnt, GetEXIFName(EM_STANDARD, ifdId, tag), &ifdEntries[ifdOfst + 8], false);
				frame->AddHexBuff(frameOfst + 10 + fcnt + ifdOfst, 4 - fcnt, CSTR("Reserved"), &ifdEntries[ifdOfst + 8 + fcnt], false);
			}
			else if (fcnt == 4)
			{
				frame->AddHexBuff(frameOfst + 10 + ifdOfst, fcnt, GetEXIFName(EM_STANDARD, ifdId, tag), &ifdEntries[ifdOfst + 8], false);
			}
			else
			{
				fofst = bo->GetUInt32(&ifdEntries[ifdOfst + 8]);
				frame->AddUInt(frameOfst + 10 + ifdOfst, 4, CSTR("Field Offset"), fofst);
				Data::ByteBuffer tmpBuff(fcnt);
				fd->GetRealData(fofst + readBase, fcnt, tmpBuff);
				frame->AddHexBuff((UOSInt)(fofst + readBase - ofst + frameOfst), GetEXIFName(EM_STANDARD, ifdId, tag), tmpBuff, true);
			}
			break;
		case 8:
			if (fcnt == 1)
			{
				frame->AddInt(frameOfst + 10 + ifdOfst, 2, GetEXIFName(EM_STANDARD, ifdId, tag), bo->GetInt16(&ifdEntries[ifdOfst + 8]));
				frame->AddInt(frameOfst + 12 + ifdOfst, 2, CSTR("Reserved"), bo->GetInt16(&ifdEntries[ifdOfst + 10]));
			}
			else if (fcnt == 2)
			{
				frame->AddInt(frameOfst + 10 + ifdOfst, 2, GetEXIFName(EM_STANDARD, ifdId, tag), bo->GetInt16(&ifdEntries[ifdOfst + 8]));
				frame->AddInt(frameOfst + 12 + ifdOfst, 2, GetEXIFName(EM_STANDARD, ifdId, tag), bo->GetInt16(&ifdEntries[ifdOfst + 10]));
			}
			else
			{
				fofst = bo->GetUInt32(&ifdEntries[ifdOfst + 8]);
				frame->AddUInt(frameOfst + 10 + ifdOfst, 4, CSTR("Field Offset"), fofst);
				Data::ByteBuffer tmpBuff(fcnt << 1);
				fd->GetRealData(fofst + readBase, fcnt << 1, tmpBuff);
				j = fcnt << 1;
				while (j > 0)
				{
					j -= 2;
					frame->AddInt((UOSInt)(fofst + readBase + j - ofst + frameOfst), 2, GetEXIFName(EM_STANDARD, ifdId, tag), bo->GetInt16(&tmpBuff[j]));
				}
			}
			break;
		case 10:
		{
			Text::StringBuilderUTF8 sb;
			fofst = bo->GetUInt32(&ifdEntries[ifdOfst + 8]);
			frame->AddUInt(frameOfst + 10 + ifdOfst, 4, CSTR("Field Offset"), fofst);
			Data::ByteBuffer tmpBuff(fcnt << 3);
			fd->GetRealData(fofst + readBase, fcnt << 3, tmpBuff);
			j = fcnt << 3;
			while (j > 0)
			{
				j -= 8;
				sb.ClearStr();
				sb.AppendI32(bo->GetInt32(&tmpBuff[j]));
				sb.AppendC(UTF8STRC(" / "));
				sb.AppendI32(bo->GetInt32(&tmpBuff[j + 4]));
				frame->AddField((UOSInt)(fofst + readBase + j - ofst + frameOfst), 8, GetEXIFName(EM_STANDARD, ifdId, tag), sb.ToCString());
			}
			break;
		}
		case 12:
		{
			fofst = bo->GetUInt32(&ifdEntries[ifdOfst + 8]);
			frame->AddUInt(frameOfst + 10 + ifdOfst, 4, CSTR("Field Offset"), fofst);
			Data::ByteBuffer tmpBuff(fcnt << 3);
			fd->GetRealData(fofst + readBase, fcnt << 3, tmpBuff);
			j = fcnt << 3;
			while (j > 0)
			{
				j -= 8;
				frame->AddFloat((UOSInt)(fofst + readBase + j - ofst + frameOfst), 8, GetEXIFName(EM_STANDARD, ifdId, tag), ReadDouble(&tmpBuff[j]));
			}
			break;
		}
		default:
			j = 0;
			break;
		}

		ifdOfst += 12;
		i++;
	}

	fofst = bo->GetUInt32(&ifdEntries[ifdCnt * 12]);
	frame->AddUInt(frameOfst + ifdCnt * 12 + 2, 4, CSTR("Next Offset"), fofst);
	if (nextOfst)
	{
		*nextOfst = fofst;
	}
	return true;
}

Optional<Media::EXIFData> Media::EXIFData::ParseExif(const UInt8 *buff, UOSInt buffSize)
{
	if (buff[4] == 'E' && buff[5] == 'x' && buff[6] == 'i' && buff[7] == 'f' && buff[8] == 0)
	{
		NotNullPtr<Data::ByteOrder> bo;
		if (*(Int16*)&buff[10] == *(Int16*)"II")
		{
			NEW_CLASSNN(bo, Data::ByteOrderLSB());
		}
		else if (*(Int16*)&buff[10] == *(Int16*)"MM")
		{
			NEW_CLASSNN(bo, Data::ByteOrderMSB());
		}
		else
		{
			return 0;
		}
		Bool valid = true;
		if (bo->GetUInt16(&buff[12]) != 42)
		{
			valid = false;
		}
		if (bo->GetUInt32(&buff[14]) != 8)
		{
			valid = false;
		}
		Optional<Media::EXIFData> ret = 0;
		if (valid)
		{
			UInt64 nextOfst;
			ret = ParseIFD(&buff[18], buffSize - 18, bo, &nextOfst, Media::EXIFData::EM_STANDARD, &buff[10]);
		}
		bo.Delete();
		return ret;
	}
	return 0;
}
