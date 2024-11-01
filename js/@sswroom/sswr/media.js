import * as data from "./data.js";
import * as geometry from "./geometry.js";
import * as math from "./math.js";
import * as text from "./text.js";

export const EXIFMaker = {
	Standard: "Standard",
	Panasonic: "Panasonic",
	Canon: "Canon",
	Olympus: "Olympus",
	Casio1: "Casio Type 1",
	Casio2: "Casio Type 2",
	FLIR: "FLIR",
	Nikon3: "Nikon Type 3",
	Sanyo: "Sanyo Type 1",
	Apple: "Apple"
};

export const EXIFType = {
	Unknown: 0,
	Bytes: 1,
	STRING: 2,
	UINT16: 3,
	UINT32: 4,
	Rational: 5,
	Other: 6,
	INT16: 7,
	SubExif: 8,
	INT32: 9,
	SRational: 10,
	Double: 11,
	UINT64: 12,
	INT64: 13
};

export function loadImageFromBlob(blob)
{
	return new Promise(function (resolve, reject) {
        const img = new Image();
        img.addEventListener('load', function (e) {
            resolve(e.target);
        });

        img.addEventListener('error', function () {
            reject();
        });

        img.src = URL.createObjectURL(blob);
    });
}


/**
 * @param {geometry.Vector2D} vec
 * @param {number} width
 * @param {number} height
 */
export function genVector2DPreview(vec, width, height)
{
	let img = new DrawImage(width, height);
	let bounds = vec.getBounds();
	drawVector2DPreview(img, vec, bounds);
	return img;
}

/**
 * @param {DrawImage} img
 * @param {geometry.Vector2D} vec
 * @param {math.RectArea} bounds
 */
function drawVector2DPreview(img, vec, bounds)
{
	let w = img.width;
	let h = img.height;
	let vecX = 0;
	let vecY = 0;
	let vecW = bounds.getWidth();
	let vecH = bounds.getHeight();
	if (vecW > vecH)
	{
		vecY = (vecW - vecH) * 0.5;
		vecH = vecW;
	}
	else
	{
		vecX = (vecH - vecW) * 0.5;
		vecW = vecH;
	}
	let pl = [];
	let i;
	if (vec instanceof geometry.LinearRing)
	{
		i = 0;
		while (i < vec.coordinates.length)
		{
			pl.push(new math.Coord2D((vec.coordinates[i][0] - bounds.minX + vecX) * w / vecW, (bounds.maxY - vec.coordinates[i][1] + vecY) * h / vecH));
			i++;
		}
		img.drawPolyline(pl, 1, "black");
	}
	else if (vec instanceof geometry.LineString)
	{
		i = 0;
		while (i < vec.coordinates.length)
		{
			pl.push(new math.Coord2D((vec.coordinates[i][0] - bounds.minX + vecX) * w / vecW, (bounds.maxY - vec.coordinates[i][1] + vecY) * h / vecH));
			i++;
		}
		img.drawPolyline(pl, 1, "black");
	}
}

export class DrawImage
{
	/**
	 * @param {number} width
	 * @param {number} height
	 */
	constructor(width, height)
	{
		this.canvas = document.createElement("canvas");
		this.canvas.width = width;
		this.canvas.height = height;
		this.ctx = this.canvas.getContext("2d");
	}

	/**
	 * @param {math.Coord2D} startPt
	 * @param {math.Coord2D} endPt
	 * @param {undefined | null | number} lineWidth
	 * @param {undefined | string | CanvasGradient | CanvasPattern} lineStyle
	 */
	drawLine(startPt, endPt, lineWidth, lineStyle)
	{
		let ctx = this.ctx;
		if (ctx)
		{
			ctx.beginPath();
			ctx.moveTo(startPt.x, startPt.y);
			ctx.lineTo(endPt.x, endPt.y);
			if (lineWidth)
			{
				ctx.lineWidth = lineWidth;
			}
			if (lineStyle)
			{
				ctx.strokeStyle = lineStyle;
			}
			ctx.stroke();
		}
	}

	/**
	 * @param {math.Coord2D[]} pts
	 * @param {undefined | null | number} lineWidth
	 * @param {undefined | string | CanvasGradient | CanvasPattern} lineStyle
	 */
	drawPolyline(pts, lineWidth, lineStyle)
	{
		let ctx = this.ctx;
		if (ctx && pts.length > 0)
		{
			let i;
			ctx.beginPath();
			ctx.moveTo(pts[0].x, pts[0].y);
			i = 1;
			while (i < pts.length)
			{
				ctx.lineTo(pts[i].x, pts[i].y);
				i++;
			}
			if (lineWidth)
			{
				ctx.lineWidth = lineWidth;
			}
			if (lineStyle)
			{
				ctx.strokeStyle = lineStyle;
			}
			ctx.stroke();
		}
		else
		{
			console.log("ctx not found or pts is empty")
		}
	}

	get width()
	{
		return this.canvas.width;
	}

	get height()
	{
		return this.canvas.height;
	}

	toPNGURL()
	{
		return this.canvas.toDataURL("image/png");
	}

	createStaticImage()
	{
		let url = this.canvas.toDataURL("image/png");
		let img = document.createElement("img");
		img.src = url;
		return new StaticImage(img, "DrawImage.png", "image/png");
	}
}


export class EXIFItem
{
	constructor(id, type, data)
	{
		this.id = id;
		this.type = type;
		this.data = data;
	}

	clone()
	{
		if (this.type == EXIFType.SubExif)
		{
			return new EXIFItem(this.id, this.type, this.data.clone());
		}
		else
		{
			return new EXIFItem(this.id, this.type, this.data);
		}
	}

	toDataString()
	{
		if (this.type == EXIFType.SubExif)
		{
			return "EXIF";
		}
		else if (this.type == EXIFType.Other)
		{
			return text.u8Arr2Hex(new Uint8Array(this.data), " ", "\n");
		}
		else if (this.type == EXIFType.Rational || this.type == EXIFType.SRational)
		{
			if (this.data.length == 2)
			{
				return this.data[0]+" / "+this.data[1]+" ("+(this.data[0] / this.data[1])+")";
			}
			else
			{
				let i = 0;
				let j = this.data.length;
				let ret = [];
				while (i < j)
				{
					ret.push(this.data[0]+"/"+this.data[1]);
					i += 2;
				}
				return ret.join(", ");
			}
		}
		else if (typeof this.data == "string")
		{
			return this.data;
		}
		else
		{
			return this.data.join(", ");
		}
	}
}

const EXIFNamesStandard = {
	"11": "ProcessingSoftware",
	"254": "NewSubfileType",
	"255": "SubfileType",
	"256": "Width",
	"257": "Height",
	"258": "BitPerSample (R, G, B)",
	"259": "Compression",
	"262": "PhotometricInterpretation",
	"263": "Threshholding",
	"264": "CellWidth",
	"265": "CellLength",
	"266": "FillOrder",
	"267": "DocumentName",
	"270": "ImageDescription",
	"271": "Make",
	"272": "Model",
	"273": "StripOffsets",
	"274": "Orientation",
	"277": "SamplesPerPixel",
	"278": "RowsPerStrip",
	"279": "StripByteCounts",
	"280": "MinSampleValue",
	"281": "MaxSampleValue",
	"282": "XResolution",
	"283": "YResolution",
	"284": "PlanarConfiguration",
	"285": "PageName",
	"286": "XPosition",
	"287": "YPosition",
	"288": "FreeOffsets",
	"289": "FreeByteCounts",
	"290": "GrayResponseUnit",
	"291": "GrayResponseCurve",
	"292": "T4Options",
	"293": "T6Options",
	"296": "ResolutionUnit",
	"297": "PageNumber",
	"301": "TransferFunction",
	"305": "Software",
	"306": "DateTime",
	"315": "Artist",
	"316": "HostComputer",
	"317": "Predictor",
	"318": "WhitePoint",
	"319": "PrimaryChromaticities",
	"320": "ColorMap",
	"321": "HalftoneHints",
	"322": "TileWidth",
	"323": "TileLength",
	"324": "TileOffsets",
	"325": "TileByteCounts",
	"332": "InkSet",
	"333": "InkNames",
	"334": "NumberOfInks",
	"336": "DotRange",
	"337": "TargetPrinter",
	"338": "ExtraSamples",
	"339": "SampleFormat",
	"340": "SMinSampleValue",
	"341": "SMaxSampleValue",
	"342": "TransferRange",
	"343": "ClipPath",
	"344": "XClipPathUnits",
	"345": "YClipPathUnits",
	"346": "Indexed",
	"351": "OPIProxy",
	"437": "JPEG tables",
	"512": "JPEGProc",
	"513": "JPEGInterchangeFormat",
	"514": "JPEGInterchangeFormatLngth",
	"515": "JPEGRestartInterval",
	"517": "JPEGLosslessPredictors",
	"518": "JPEGPointTransforms",
	"519": "JPEGQTables",
	"520": "JPEGDCTables",
	"521": "JPEGACTables",
	"529": "YCbCrCoefficients",
	"530": "YCbCrSubSampling",
	"531": "YCbCrPositioning",
	"532": "ReferenceBlackWhite",
	"700": "Photoshop XMP",
	"32781": "ImageID",
	"32995": "Matteing",
	"32996": "DataType",
	"32997": "ImageDepth",
	"32998": "TileDepth",
	"33421": "CFARepeatPatternDim",
	"33422": "CFAPattern",
	"33423": "BatteryLevel",
	"33432": "Copyright",
	"33434": "ExposureTime",
	"33437": "Fnumber",
	"33723": "IPTC/NAA",
	"33550": "ModelPixelScaleTag",
	"33920": "IntergraphMatrixTag",
	"33922": "ModelTiepointTag",
	"34016": "Site",
	"34017": "ColorSequence",
	"34018": "IT8Header",
	"34019": "RasterPadding",
	"34020": "BitsPerRunLength",
	"34021": "BitsPerExtendedRunLength",
	"34022": "ColorTable",
	"34023": "ImageColorIndicator",
	"34024": "BackgroundColorIndicator",
	"34025": "ImageColorValue",
	"34026": "BackgroundColorValue",
	"34027": "PixelInensityRange",
	"34028": "TransparencyIndicator",
	"34029": "ColorCharacterization",
	"34030": "HCUsage",
	"34264": "ModelTransformationTag",
	"34377": "PhotoshopImageResources",
	"34665": "ExifIFD",
	"34675": "InterColourProfile",
	"34732": "ImageLayer",
	"34735": "GeoKeyDirectoryTag",
	"34736": "GeoDoubleParamsTag",
	"34737": "GeoAsciiParamsTag",
	"34850": "ExposureProgram",
	"34852": "SpectralSensitivity",
	"34853": "GPSInfo",
	"34855": "ISOSpeedRatings",
	"34856": "OECF",
	"34857": "Interlace",
	"34858": "TimeZoneOffset",
	"34859": "SelfTimerMode",
	"34908": "FaxRecvParams",
	"34909": "FaxSubAddress",
	"34910": "FaxRecvTime",
	"36867": "DateTimeOriginal",
	"37122": "CompressedBitsPerPixel",
	"37377": "ShutterSpeedValue",
	"37378": "ApertureValue",
	"37379": "BrightnessValue",
	"37380": "ExposureBiasValue",
	"37381": "MaxApertureValue",
	"37382": "SubjectDistance",
	"37383": "MeteringMode",
	"37384": "LightSource",
	"37385": "Flash",
	"37386": "FocalLength",
	"37387": "FlashEnergy",
	"37388": "SpatialFrequencyResponse",
	"37389": "Noise",
	"37390": "FocalPlaneXResolution",
	"37391": "FocalPlaneYResolution",
	"37392": "FocalPlaneResolutionUnit",
	"37393": "ImageNumber",
	"37394": "SecurityClassification",
	"37395": "ImageHistory",
	"37396": "SubjectLocation",
	"37397": "ExposureIndex",
	"37398": "TIFF/EPStandardID",
	"37399": "SensingMethod",
	"37439": "StoNits",
	"37724": "ImageSourceData",
	"40091": "XPTitle",
	"40092": "XPComment",
	"40093": "XPAuthor",
	"40094": "XPKeywords",
	"40095": "XPSubject",
	"40965": "InteroperabilityIFD",
	"41988": "DigitalZoomRatio",
	"42016": "ImageUniqueID",
	"50255": "PhotoshopAnnotations",
	"50706": "DNGVersion",
	"50707": "DNGBackwardVersion",
	"50708": "UniqueCameraModel",
	"50709": "LocalizedCameraModel",
	"50710": "CFAPlaneColor",
	"50711": "CFALayout",
	"50712": "LinearizationTable",
	"50713": "BlackLevelRepeatDim",
	"50714": "BlackLevel",
	"50715": "BlackLevelDeltaH",
	"50716": "BlackLevelDeltaV",
	"50717": "WhiteLevel",
	"50718": "DefaultScale",
	"50719": "DefaultCropOrigin",
	"50720": "DefaultCropSize",
	"50721": "ColorMatrix1",
	"50722": "ColorMatrix2",
	"50723": "CameraCalibration1",
	"50724": "CameraCalibration2",
	"50725": "ReductionMatrix1",
	"50726": "ReductionMatrix2",
	"50727": "AnalogBalnace",
	"50728": "AsShortNeutral",
	"50729": "AsShortWhiteXY",
	"50730": "BaselineExposure",
	"50731": "BaselineNoise",
	"50732": "BaselineSharpness",
	"50733": "BayerGreenSplit",
	"50734": "LinearResponseLimit",
	"50735": "CameraSerialNumber",
	"50736": "LensInfo",
	"50737": "ChromaBlurRadius",
	"50738": "AntiAliasStrength",
	"50740": "DNGPrivateData",
	"50741": "MakerNoteSafety",
	"50778": "CalibrationIlluminant1",
	"50779": "CalibrationIlluminant2",
	"50780": "BestQualityScale",
	"59932": "Padding"
};

const EXIFNamesInfo = {
	"33434": "ExposureTime",
	"33437": "FNumber",
	"34850": "ExposureProgram",
	"34852": "SpectralSensitibity",
	"34855": "ISOSpeedRatings",
	"34856": "OECF",
	"36864": "ExifVersion",
	"36867": "DateTimeOriginal",
	"36868": "DateTimeDigitized",
	"36880": "OffsetTime",
	"36881": "OffsetTimeOriginal",
	"36882": "OffsetTimeDigitized",
	"37121": "ComponentsConfiguration",
	"37122": "CompressedBitsPerPixel",
	"37377": "ShutterSpeedValue",
	"37378": "ApertureValue",
	"37379": "BrightnessValue",
	"37380": "ExposureBiasValue",
	"37381": "MaxApertureValue",
	"37382": "SubjectDistance",
	"37383": "MeteringMode",
	"37384": "LightSource",
	"37385": "Flash",
	"37386": "FocalLength",
	"37396": "SubjectArea",
	"37500": "MakerNote",
	"37510": "UserComment",
	"37520": "SubSecTime",
	"37521": "SubSecTimeOriginal",
	"37522": "SubSecTimeDigitized",
	"37890": "Pressure", //hPa
	"37891": "WaterDepth", //m
	"37892": "Acceleration", //mGal
	"37893": "CameraElevationAngle",
	"40960": "FlashpixVersion",
	"40961": "ColorSpace",
	"40962": "PixelXDimension",
	"40963": "PixelYDimension",
	"40964": "RelatedSoundFile",
	"40965": "InteroperabilityIFD",
	"41483": "FlashEnergy",
	"41484": "SpatialFrequencyResponse",
	"41486": "FocalPlaneXResolution",
	"41487": "FocalPlaneYResolution",
	"41488": "FocalPlaneResolutionUnit",
	"41492": "SubjectLocation",
	"41493": "ExposureIndex",
	"41495": "SensingMethod",
	"41728": "FileSource",
	"41729": "SceneType",
	"41730": "CFAPattern",
	"41985": "CustomRendered",
	"41986": "ExposureMode",
	"41987": "WhiteBalance",
	"41988": "DigitalZoomRatio",
	"41989": "FocalLengthIn35mmFilm",
	"41990": "SceneCaptureType",
	"41991": "GainControl",
	"41992": "Contrast",
	"41993": "Saturation",
	"41994": "Sharpness",
	"41995": "DeviceSettingDescription",
	"41996": "SubjectDistanceRange",
	"42016": "ImageUniqueID",
	"42032": "CameraOwnerName",
	"42033": "BodySerialNumber",
	"42034": "LensSpecification",
	"42035": "LensMake",
	"42036": "LensModel",
	"42037": "LensSerialNumber",
	"42080": "CompositeImage",
	"42081": "SourceImageNumberOfCompositeImage",
	"42082": "SourceExposureTimesOfCompositeImage",
	"59932": "Padding",
	"59933": "OffsetSchema"
};

const EXIFNamesGPS = {
	"0": "GPSVersionID",
	"1": "GPSLatitudeRef",
	"2": "GPSLatitude",
	"3": "GPSLongitudeRef",
	"4": "GPSLongitude",
	"5": "GPSAltitudeRef",
	"6": "GPSAltitude",
	"7": "GPSTimeStamp",
	"8": "GPSSatellites",
	"9": "GPSStatus",
	"10": "GPSMeasureMode",
	"11": "GPSDOP",
	"12": "GPSSpeedRef",
	"13": "GPSSpeed",
	"14": "GPSTrackRef",
	"15": "GPSTrack",
	"16": "GPSImgDirectionRef",
	"17": "GPSImgDirection",
	"18": "GPSMapDatum",
	"19": "GPSDestLatitudeRef",
	"20": "GPSDestLatitude",
	"21": "GPSDestLongitudeRef",
	"22": "GPSDestLongitude",
	"23": "GPSDestBearingRef",
	"24": "GPSDestBearing",
	"25": "GPSDestDistanceRef",
	"26": "GPSDestDistance",
	"27": "GPSProcessingMethod",
	"28": "GPSAreaInformation",
	"29": "GPSDateStamp",
	"30": "GPSDifferential",
	"31": "GPSHPositioningError"
};

const EXIFNamesPanasonic = {
	"1": "Quality",
	"2": "FirmwareVersion",
	"3": "WhiteBalance",
	"7": "FocusMode",
	"15": "AFMode",
	"26": "ImageStabilization",
	"28": "Macro",
	"31": "ShootingMode",
	"32": "Audio",
	"36": "FlashBias",
	"37": "InternalSerialNumber",
	"38": "ExifVersion",
	"40": "ColorEffect",
	"41": "TimeSincePowerOn",
	"42": "BurstMode",
	"43": "SequenceNumber",
	"44": "Contrast",
	"45": "NoiseReduction",
	"46": "SelfTimer",
	"48": "Rotation",
	"49": "AFAssistLamp",
	"50": "ColorMode",
	"51": "BabyAge1",
	"52": "OpticalZoomMode",
	"53": "ConversionLens",
	"54": "TravelDay",
	"57": "Contrast",
	"58": "WorldTimeLocation",
	"59": "TestStamp1",
	"60": "ProgramISO",
	"61": "AdvancedSceneType",
	"62": "TextStampe2",
	"63": "FacesDetected",
	"64": "Saturation",
	"65": "Sharpness",
	"68": "ColorTempKelvin",
	"69": "BracketSettings",
	"70": "WBAdjustAB",
	"71": "WBAdjustGM",
	"72": "FlashCurtain",
	"73": "LongShutterNoiseReduction",
	"75": "ImageWidth",
	"76": "ImageHeight",
	"77": "AFPointPosition",
	"78": "FaceDetInfo",
	"81": "LensType",
	"82": "LensSerialNumber",
	"83": "AccessoryTyp",
	"84": "AccessorySerialNumber",
	"96": "LensFirmwareVersion",
	"97": "FaceRecInfo",
	"101": "Title",
	"102": "BabyName",
	"103": "Location",
	"105": "Country",
	"107": "State",
	"109": "City",
	"111": "Landmark",
	"112": "IntelligentResolution",
	"119": "BurstSpeed",
	"121": "IntelligentDRange",
	"124": "ClearRetouch",
	"128": "City2",
	"137": "PhotoStyle",
	"138": "ShadingCompensation",
	"140": "AccelerometerZ",
	"141": "AccelerometerX",
	"142": "AccelerometerY",
	"143": "CameraOrientation",
	"144": "RollAngle",
	"145": "PitchAngle",
	"147": "SweepPanoramaDirection",
	"148": "PanoramaFieldOfView",
	"150": "TimerRecording",
	"157": "InternalNDFilter",
	"158": "HDR",
	"159": "ShutterType",
	"163": "ClearRetouchValue",
	"171": "TouchAE",
	"32768": "MakerNoteVersion",
	"32769": "SceneMode",
	"32772": "WBRedLevel",
	"32773": "WBGreenLevel",
	"32774": "WBBlueLevel",
	"32775": "FlashFired",
	"32776": "TextStamp3",
	"32777": "TextStamp4",
	"32784": "babyAge2"
};

const EXIFNamesCanon = {
	"1": "CanonCameraSettings",
	"2": "CanonFocalLength",
	"3": "CanonFlashInfo",
	"4": "CanonShotInfo",
	"5": "CanonPanorama",
	"6": "CanonImageType",
	"7": "CanonFirmwareVersion",
	"8": "FileNumber",
	"9": "OwnerName",
	"10": "UnknownD30",
	"12": "SerialNumber",
	"13": "CanonCameraInfo",
	"14": "CanonFileLength",
	"15": "CustomFunctions",
	"16": "CanonModelID",
	"17": "MovieInfo",
	"18": "CanonAFInfo",
	"19": "ThumbnailImageValidArea",
	"21": "SerialNumberFormat",
	"26": "SuperMacro",
	"28": "DateStampMode",
	"29": "MyColors",
	"30": "FirmwareRevision",
	"35": "FaceDetect1",
	"36": "FaceDetect2",
	"37": "Categories",
	"38": "CanonAFInfo2",
	"39": "ContrastInfo",
	"40": "ImageUniqueID",
	"47": "FaceDetect3",
	"53": "TimeInfo",
	"56": "BatteryType",
	"60": "AFInfo3",
	"129": "RawDataOffset",
	"131": "OriginalDecisionData",
	"144": "CustomFunctions1D",
	"145": "PersonalFunctions",
	"146": "PersonalFunctionValues",
	"147": "CanonFileInfo",
	"148": "AFPointsInFocus1D",
	"149": "LensModel",
	"150": "InternalSerialNumber",
	"151": "DustRemovalData",
	"152": "CropInfo",
	"153": "CustomFunctions2",
	"154": "AspectInfo",
	"160": "ProcessingInfo",
	"161": "ToneCurveTable",
	"162": "SharpnessTable",
	"163": "SharpnessFreqTable",
	"164": "WhiteBalanceTable",
	"169": "ColorBalance",
	"170": "MeasuredColor",
	"174": "ColorTemperature",
	"176": "CanonFlags",
	"177": "ModifiedInfo",
	"178": "ToneCurveMatching",
	"179": "WhiteBalanceMatching",
	"180": "ColorSpace",
	"182": "PreviewImageInfo",
	"208": "VROffset",
	"224": "SensorInfo",
	"16385": "ColorBalance", //0x4001
	"16386": "UnknownBlock1", //0x4002
	"16387": "ColorInfo", //0x4003
	"16389": "Flavor", //0x4005
	"16392": "PictureStyleUserDef", //0x4008
	"16393": "PictureStylePC", //0x4009
	"16400": "CustomPictureStyleFileName", //0x4010
	"16403": "AFMicroAdj", //0x4013
	"16405": "VignettingCorr", //0x4015
	"16406": "VignettingCorr2", //0x4016
	"16408": "LightingOpt", //0x4018
	"16409": "LensInfo", //0x4019
	"16416": "AmbienceInfo", //0x4020
	"16417": "MultiExp", //0x4021
	"16420": "FilterInfo", //0x4024
	"16421": "HDRInfo", //0x4025
	"16424": "AFConfig" //0x4028
};

const EXIFNamesOlympus = {
	"0": "MakerNoteVersion", //0x0000
	"1": "MinoltaCameraSettingsOld", //0x0001
	"3": "MinoltaCameraSettings", //0x0003
	"64": "CompressedImageSize", //0x0040
	"129": "PreviewImageData", //0x0081
	"136": "PreviewImageStart", //0x0088
	"137": "PreviewImageLength", //0x0089
	"256": "ThumbnailImage", //0x0100
	"260": "BodyFirmwareVersion", //0x0104
	"512": "SpecialMode", //0x0200
	"513": "Quality", //0x0201
	"514": "Macro", //0x0202
	"515": "BWMode", //0x0203
	"516": "DigitalZoom", //0x0204
	"517": "FocalPlaneDiagonal", //0x0205
	"518": "LensDistortionParmas", //0x0206
	"519": "CameraType", //0x0207
	"520": "CameraInfo", //0x0208
	"521": "CameraID", //0x0209
	"523": "EpsonImageWidth", //0x020b
	"524": "EpsonImageHeight", //0x020c
	"525": "EpsonSoftware", //0x020d
	"640": "PreviewImage", //0x0280
	"768": "PreCaptureFrames", //0x0300
	"769": "WhiteBoard", //0x0301
	"770": "OneTouchWB", //0x0302
	"771": "WhiteBalanceBracket", //0x0303
	"772": "WhiteBalanceBias", //0x0304
	"1025": "BlackLevel", //0x0401
	"1027": "SceneMode", //0x0403
	"1028": "SerialNumber", //0x0404
	"1029": "Firmware", //0x0405
	"3584": "PrintIM", //0x0e00
	"3840": "DataDump", //0x0f00
	"3841": "DataDump2", //0x0f01
	"3844": "ZoomedPreviewStart", //0x0f04
	"3845": "ZoomedPreviewLength", //0x0f05
	"3846": "ZoomedPreviewSize", //0x0f06
	"4096": "ShutterSpeedValue", //0x1000
	"4097": "ISOValue", //0x1001
	"4098": "ApertureValue", //0x1002
	"4099": "BrightnessValue", //0x1003
	"4100": "FlashMode", //0x1004
	"4101": "FlashDevice", //0x1005
	"4102": "ExposureCompensation", //0x1006
	"4103": "SensorTemperature", //0x1007
	"4104": "LensTemperature", //0x1008
	"4105": "LightCondition", //0x1009
	"4106": "FocusRange", //0x100a
	"4107": "FocusMode", //0x100b
	"4108": "ManualFocusDistance", //0x100c
	"4109": "ZoomStepCount", //0x100d
	"4110": "FocusStepCount", //0x100e
	"4111": "Sharpness", //0x100f
	"4112": "FlashChargeLevel", //0x1010
	"4113": "ColorMatrix", //0x1011
	"4114": "BlackLevel", //0x1012
	"4115": "ColorTemperatureBG", //0x1013
	"4116": "ColorTemperatureRG", //0x1014
	"4117": "WBMode", //0x1015
	"4119": "RedBalance", //0x1017
	"4120": "BlueBalance", //0x1018
	"4121": "ColorMatrixNumber", //0x1019
	"4122": "SerialNumber", //0x101a
	"4123": "ExternalFlashAE1_0", //0x101b
	"4124": "ExternalFlashAE2_0", //0x101c
	"4125": "InternalFlashAE1_0", //0x101d
	"4126": "InternalFlashAE2_0", //0x101e
	"4127": "ExternalFlashAE1", //0x101f
	"4128": "ExternalFlashAE2", //0x1020
	"4129": "InternalFlashAE1", //0x1021
	"4130": "InternalFlashAE2", //0x1022
	"4131": "FlashExposureComp", //0x1023
	"4132": "InternalFlashTable", //0x1024
	"4133": "ExternalFlashGValue", //0x1025
	"4134": "ExternalFlashBounce", //0x1026
	"4135": "ExternalFlashZoom", //0x1027
	"4136": "ExternalFlashMode", //0x1028
	"4137": "Contrast", //0x1029
	"4138": "SharpnessFactor", //0x102a
	"4139": "ColorControl", //0x102b
	"4140": "ValidBits", //0x102c
	"4141": "CoringFilter", //0x102d
	"4142": "OlympusImageWidth", //0x102e
	"4143": "OlympusImageHeight", //0x102f
	"4144": "SceneDetect", //0x1030
	"4145": "SceneArea", //0x1031
	"4147": "SceneDetectData", //0x1033
	"4148": "CompressionRatio", //0x1034
	"4149": "PreviewImageValid", //0x1035
	"4150": "PreviewImageStart", //0x1036
	"4151": "PreviewImageLength", //0x1037
	"4152": "AFResult", //0x1038
	"4153": "CCDScanMode", //0x1039
	"4154": "NoiseReduction", //0x103a
	"4155": "FocusStepInfinity", //0x103b
	"4156": "FocusStepNear", //0x103c
	"4157": "LightValueCenter", //0x103d
	"4158": "LightValuePeriphery", //0x103e
	"4159": "FieldCount", //0x103f
	"8208": "Equipment", //0x2010
	"8224": "CameraSettings", //0x2020
	"8240": "RawDevelopment", //0x2030
	"8241": "RawDev2", //0x2031
	"8256": "ImageProcessing", //0x2040
	"8272": "FocusInfo", //0x2050
	"8448": "Olympus2100", //0x2100
	"8704": "Olympus2200", //0x2200
	"8960": "Olympus2300", //0x2300
	"9216": "Olympus2400", //0x2400
	"9472": "Olympus2500", //0x2500
	"9728": "Olympus2600", //0x2600
	"9984": "Olympus2700", //0x2700
	"10240": "Olympus2800", //0x2800
	"10496": "Olympus2900", //0x2900
	"12288": "RawInfo", //0x3000
	"16384": "MainInfo", //0x4000
	"20480": "UnknownInfo" //0x5000
};

const EXIFNamesOlympus2010 = {
	"0x0000": "EquipmentVersion",
	"0x0100": "CameraType2",
	"0x0101": "SerialNumber",
	"0x0102": "InternalSerialNumber",
	"0x0103": "FocalPlaneDiagonal",
	"0x0104": "BodyFirmwareVersion",
	"0x0201": "LensType",
	"0x0202": "LensSerialNumber",
	"0x0203": "LensModel",
	"0x0204": "LensFirmwareVersion",
	"0x0205": "MaxApertureAtMinFocal",
	"0x0206": "MaxApertureAtMaxFocal",
	"0x0207": "MinFocalLength",
	"0x0208": "MaxFocalLength",
	"0x020a": "MaxAperture",
	"0x020b": "LensProperties",
	"0x0301": "Extender",
	"0x0302": "ExtenderSerialNumber",
	"0x0303": "ExtenderModel",
	"0x0304": "ExtenderFirmwareVersion",
	"0x0403": "ConversionLens",
	"0x1000": "FlashType",
	"0x1001": "FlashModel",
	"0x1002": "FlashFirmwareVersion",
	"0x1003": "FlashSerialNumber"
};

const EXIFNamesOlympus2020 = {
	"0x0000": "CameraSettingsVersion",
	"0x0100": "PreviewImageValid",
	"0x0101": "PreviewImageStart",
	"0x0102": "PreviewImageLength",
	"0x0200": "ExposureMode",
	"0x0201": "AELock",
	"0x0202": "MeteringMode",
	"0x0203": "ExposureShift",
	"0x0204": "NDFilter",
	"0x0300": "MacroMode",
	"0x0301": "FocusMode",
	"0x0302": "FocusProcess",
	"0x0303": "AFSearch",
	"0x0304": "AFAreas",
	"0x0305": "AFPointSelected",
	"0x0306": "AFFineTune",
	"0x0307": "AFFineTuneAdj",
	"0x0400": "FlashMode",
	"0x0401": "FlashExposureComp",
	"0x0403": "FlashRemoteControl",
	"0x0404": "FlashControlMode",
	"0x0405": "FlashIntensity",
	"0x0406": "ManualFlashStrength",
	"0x0500": "WhiteBalance2",
	"0x0501": "WhiteBalanceTemperature",
	"0x0502": "WhiteBalanceBracket",
	"0x0503": "CustomSaturation",
	"0x0504": "ModifiedSaturation",
	"0x0505": "ContrastSetting",
	"0x0506": "SharpnessSetting",
	"0x0507": "ColorSpace",
	"0x0509": "SceneMode",
	"0x050a": "NoiseReduction",
	"0x050b": "DistortionCorrection",
	"0x050c": "ShadingCompensation",
	"0x050d": "CompressionFactor",
	"0x050f": "Gradation",
	"0x0520": "PictureMode",
	"0x0521": "PictureModeSaturation",
	"0x0522": "PictureModeHue",
	"0x0523": "PictureModeContrast",
	"0x0524": "PictureModeSharpness",
	"0x0525": "PictureModeBWFilter",
	"0x0526": "PictureModeTone",
	"0x0527": "NoiseFilter",
	"0x0529": "ArtFilter",
	"0x052c": "MagicFilter",
	"0x052d": "PictureModeEffect",
	"0x052e": "ToneLevel",
	"0x052f": "ArtFilterEffect",
	"0x0532": "ColorCreatorEffect",
	"0x0537": "MonochromeProfileSettings",
	"0x0538": "FilmGrainEffect",
	"0x0539": "ColorProfileSettings",
	"0x053a": "MonochromeVignetting",
	"0x053b": "MonochromeColor",
	"0x0600": "DriveMode",
	"0x0601": "PanoramaMode",
	"0x0603": "ImageQuality2",
	"0x0604": "ImageStabilization",
	"0x0804": "StackedImage",
	"0x0900": "ManometerPressure",
	"0x0901": "ManometerReading",
	"0x0902": "ExtendedWBDetect",
	"0x0903": "RollAngle",
	"0x0904": "PitchAngle",
	"0x0908": "DateTimeUTC"
};

const EXIFNamesOlympus2030 = {
	"0x0000": "RawDevVersion",
	"0x0100": "RawDevExposureBiasValue",
	"0x0101": "RawDevWhiteBalanceValue",
	"0x0102": "RawDevWBFineAdjustment",
	"0x0103": "RawDevGrayPoint",
	"0x0104": "RawDevSaturationEmphasis",
	"0x0105": "RawDevMemoryColorEmphasis",
	"0x0106": "RawDevContrastValue",
	"0x0107": "RawDevSharpnessValue",
	"0x0108": "RawDevColorSpace",
	"0x0109": "RawDevEngine",
	"0x010a": "RawDevNoiseReduction",
	"0x010b": "RawDevEditStatus",
	"0x010c": "RawDevSettings"
};

const EXIFNamesOlympus2040 = {
	"0x0000": "ImageProcessingVersion",
	"0x0100": "WB_RBLevels",
	"0x0102": "WB_RBLevels3000K",
	"0x0103": "WB_RBLevels3300K",
	"0x0104": "WB_RBLevels3600K",
	"0x0105": "WB_RBLevels3900K",
	"0x0106": "WB_RBLevels4000K",
	"0x0107": "WB_RBLevels4300K",
	"0x0108": "WB_RBLevels4500K",
	"0x0109": "WB_RBLevels4800K",
	"0x010a": "WB_RBLevels5300K",
	"0x010b": "WB_RBLevels6000K",
	"0x010c": "WB_RBLevels6600K",
	"0x010d": "WB_RBLevels7500K",
	"0x010e": "WB_RBLevelsCWB1",
	"0x010f": "WB_RBLevelsCWB2",
	"0x0110": "WB_RBLevelsCWB3",
	"0x0111": "WB_RBLevelsCWB4",
	"0x0113": "WB_GLevel3000K",
	"0x0114": "WB_GLevel3300K",
	"0x0115": "WB_GLevel3600K",
	"0x0116": "WB_GLevel3900K",
	"0x0117": "WB_GLevel4000K",
	"0x0118": "WB_GLevel4300K",
	"0x0119": "WB_GLevel4500K",
	"0x011a": "WB_GLevel4800K",
	"0x011b": "WB_GLevel5300K",
	"0x011c": "WB_GLevel6000K",
	"0x011d": "WB_GLevel6600K",
	"0x011e": "WB_GLevel7500K",
	"0x011f": "WB_GLevel",
	"0x0200": "ColorMatrix",
	"0x0300": "Enhancer",
	"0x0301": "EnhancerValues",
	"0x0310": "CoringFilter",
	"0x0311": "CoringValues",
	"0x0600": "BlackLevel2",
	"0x0610": "GainBase",
	"0x0611": "ValidBits",
	"0x0612": "CropLeft",
	"0x0613": "CropTop",
	"0x0614": "CropWidth",
	"0x0615": "CropHeight",
	"0x0635": "UnknownBlock1",
	"0x0636": "UnknownBlock2",
	"0x0805": "SensorCalibration",
	"0x1010": "NoiseReduction2",
	"0x1011": "DistortionCorrection2",
	"0x1012": "ShadingCompensation2",
	"0x101c": "MultipleExposureMode",
	"0x1103": "UnknownBlock3",
	"0x1104": "UnknownBlock4",
	"0x1112": "AspectRatio",
	"0x1113": "AspectFrame",
	"0x1200": "FacesDetected",
	"0x1201": "FaceDetectArea",
	"0x1202": "MaxFaces",
	"0x1203": "FaceDetectFrameSize",
	"0x1207": "FaceDetectFrameCrop",
	"0x1306": "CameraTemperature",
	"0x1900": "KeystoneCompensation",
	"0x1901": "KeystoneDirection",
	"0x1906": "KeystoneValue"
};

const EXIFNamesOlympus2050 = {
	"0x0000": "FocusInfoVersion",
	"0x0209": "AutoFocus",
	"0x0210": "SceneDetect",
	"0x0211": "SceneArea",
	"0x0212": "SceneDetectData",
	"0x0300": "ZoomStepCount",
	"0x0301": "FocusStepCount",
	"0x0303": "FocusStepInfinity",
	"0x0304": "FocusStepNear",
	"0x0305": "FocusDistance",
	"0x0308": "AFPoint",
	"0x0328": "AFInfo",
	"0x1201": "ExternalFlash",
	"0x1203": "ExternalFlashGuideNumber",
	"0x1204": "ExternalFlashBounce",
	"0x1205": "ExternalFlashZoom",
	"0x1208": "InternalFlash",
	"0x1209": "ManualFlash",
	"0x120a": "MacroLED",
	"0x1500": "SensorTemperature",
	"0x1600": "ImageStabilization"
};

const EXIFNamesCasio1 = {
	"1": "RecordingMode",
	"2": "Quality",
	"3": "FocusingMode",
	"4": "FlashMode",
	"5": "FlashIntensity",
	"6": "ObjectDistance",
	"7": "WhiteBalance",
	"10": "DigitalZoom",
	"11": "Sharpness",
	"12": "Contract",
	"13": "Saturation",
	"20": "CCDSensitivity"
};

const EXIFNamesCasio2 = {
	"2": "PreviewThumbDimension",
	"3": "PreviewThumbSize",
	"4": "PreviewThumbOffset",
	"8": "QualityMode",
	"9": "ImageSize",
	"13": "FocusMode",
	"20": "IsoSensitivity",
	"25": "WhiteBalance",
	"29": "FocalLength",
	"31": "Saturation",
	"32": "Contrast",
	"33": "Sharpness",
	"3584": "PIM", //0x0E00
	"8192": "CasioPreviewThumbnail", //0x2000
	"8209": "WhiteBalanceBias", //0x2011
	"8210": "WhiteBalance", //0x2012
	"8226": "ObjectDistance", //0x2022
	"8244": "FlashDistance", //0x2034
	"12288": "RecordMode", //0x3000
	"12289": "SelfTimer", //0x3001
	"12290": "Quality", //0x3002
	"12291": "FocusMode", //0x3003
	"12294": "TimeZone", //0x3006
	"12295": "BestshotMode", //0x3007
	"12308": "CCDISOSensitivity", //0x3014
	"12309": "ColourMode", //0x3015
	"12310": "Enhancement", //0x3016
	"12311": "Filter" //0x3017
};

const EXIFNamesFLIR = {
	"1": "RTemp",
	"2": "ATemp",
	"3": "Emissivity",
	"4": "IRWTemp",
	"5": "CameraTemperatureRangeMax",
	"6": "CameraTemperatureRangeMin",
	"7": "Unknown",
	"8": "Unknown",
	"9": "Unknown",
	"10": "Unknown",
	"274": "Unknown"
};

const EXIFNamesNikon3 = {
	"1": "Version",
	"2": "ISOSpeed",
	"3": "ColourMode",
	"4": "Quality",
	"5": "WhiteBalance",
	"6": "Sharpening",
	"7": "Focus",
	"8": "FlashSetting",
	"9": "FlashDevice",
	"11": "WhiteBalanceBias",
	"12": "WB_RBLevels",
	"13": "ProgramShift",
	"14": "ExposureDiff",
	"15": "ISOSelection",
	"16": "DataDump",
	"17": "Preview",
	"18": "FlashComp",
	"19": "ISOSettings",
	"22": "ImageBoundary",
	"23": "FlashExposureComp",
	"24": "FlashBracketComp",
	"25": "ExposureBracketComp",
	"26": "ImageProcessing",
	"27": "CropHiSpeed",
	"28": "ExposureTuning",
	"29": "SerialNumber",
	"30": "ColorSpace",
	"31": "VRInfo",
	"32": "ImageAuthentication",
	"34": "ActiveDLighting",
	"35": "PictureControl",
	"36": "WorldTime",
	"37": "ISOInfo",
	"42": "VignetteControl",
	"52": "ShutterMode",
	"55": "MechanicalShutterCount",
	"128": "ImageAdjustment",
	"129": "ToneComp",
	"130": "AuxiliaryLens",
	"131": "LensType",
	"132": "Lens",
	"133": "FocusDistance",
	"134": "DigitalZoom",
	"135": "FlashMode",
	"136": "AFInfo",
	"137": "ShootingMode",
	"138": "AutoBracketRelease",
	"139": "LensFStops",
	"140": "ContrastCurve",
	"141": "ColorHue",
	"143": "SceneMode",
	"144": "LightSource",
	"145": "ShotInfo",
	"146": "HueAdjustment",
	"147": "NEFCompression",
	"148": "Saturation",
	"149": "NoiseReduction",
	"150": "LinearizationTable",
	"151": "ColorBalance",
	"152": "LensData",
	"153": "RawImageCenter",
	"154": "SensorPixelSize",
	"156": "SceneAssist",
	"158": "RetouchHistory",
	"160": "SerialNO",
	"162": "ImageDataSize",
	"165": "ImageCount",
	"166": "DeletedImageCount",
	"167": "ShutterCount",
	"168": "FlashInfo",
	"169": "ImageOptimisation",
	"170": "Saturation",
	"171": "VariProgram",
	"172": "ImageStabilization",
	"173": "AFResponse",
	"176": "MultiExposure",
	"177": "HighISONoiseReduction",
	"179": "ToningEffect",
	"183": "AFInfo2",
	"184": "FileInfo",
	"185": "AFTune",
	"195": "BarometerInfo",
	"3584": "PrintIM",
	"3585": "CaptureData",
	"3593": "CaptureVersion",
	"3598": "CaptureOffsets",
	"3600": "ScanIFD",
	"3613": "ICCProfile",
	"3614": "CaptureOutput"
};

const EXIFNamesSanyo1 = {
	"0": "Unknown",
	"1": "Version",
	"136": "Thumbnail Offset", //0x0088
	"137": "Thumbnail Length", //0x0089
	"255": "Makernote Offset", //0x00FF
	"256": "Jpeg Thumbnail", //0x0100
	"512": "Special Mode", //0x0200
	"513": "Jpeg Quality", //0x0201
	"514": "Macro", //0x0202
	"515": "Sanyo-1-0x0203", //0x0203
	"516": "Digital Zoom", //0x0204
	"519": "Software Version", //0x0207
	"520": "Picture Info", //0x0208
	"521": "Camera ID", //0x0209
	"526": "Sequential Shot Method", //0x020E
	"527": "Wide Range", //0x020F
	"528": "Color Adjustment Mode", //0x0210
	"529": "Sanyo-1-0x0211", //0x0211
	"530": "Sanyo-1-0x0212", //0x0212
	"531": "Quick Shot", //0x0213
	"532": "Self Timer", //0x0214
	"533": "Sanyo-1-0x0215", //0x0215
	"534": "Voice Memo", //0x0216
	"535": "Record Shutter Release", //0x0217
	"536": "Flicker Reduce", //0x0218
	"537": "Optical Zoom", //0x0219
	"538": "Sanyo-1-0x021a", //0x021A
	"539": "Digital Zoom", //0x021B
	"540": "Sanyo-1-0x021c", //0x021C
	"541": "Light Source Special", //0x021D
	"542": "Resaved", //0x021E
	"543": "Scene Select", //0x021F
	"544": "Sanyo-1-0x0220", //0x0220
	"545": "Sanyo-1-0x0221", //0x0221
	"546": "Sanyo-1-0x0222", //0x0222
	"547": "Manual Focal Distance", //0x0223
	"548": "Sequential Shot Interval", //0x0224
	"549": "Flash Mode", //0x0225
	"550": "Sanyo-1-0x0226", //0x0226
	"768": "Sanyo-1-0x0300", //0x0300
	"3584": "Print IM Data", //0x0E00
	"3840": "Data Dump" //0x0F00
};

const EXIFNamesApple = {
	"1": "MakerNoteVersion",
	"2": "AEMatrix?",
	"3": "RunTime",
	"4": "AEStable",
	"5": "AETarget",
	"6": "AEAverage",
	"7": "AFStable",
	"8": "FocusAccelerometerVector",
	"9": "SISMethod",
	"10": "HDRMethod",
	"11": "BurstUUID",
	"12": "SphereHealthTrackingError",
	"13": "SphereHealthAverageCurrent",
	"14": "SphereMotionDataStatus",
	"15": "OISMode",
	"16": "SphereStatus",
	"17": "AssetIdentifier",
	"18": "QRMOutputType",
	"19": "SphereExternalForceOffset",
	"20": "StillImageCaptureType",
	"21": "ImageGroupIdentifier",
	"22": "PhotosOriginatingSignature",
	"23": "LivePhotoVideoIndex",
	"24": "PhotosRenderOriginatingSignature",
	"25": "StillImageProcessingFlags",
	"26": "PhotoTranscodeQualityHint",
	"27": "PhotosRenderEffect",
	"28": "BracketedCaptureSequenceNumber",
	"29": "LuminanceNoiseAmplitude?",
	"30": "OriginatingAppID?",
	"31": "PhotosAppFeatureFlags",
	"32": "ImageCaptureRequestIdentifier",
	"33": "MeteorHeadroom",
	"34": "ARKitPhoto",
	"35": "AFPerformance",
	"36": "AFExternalOffset",
	"37": "StillImageSceneFlags",
	"38": "StillImageSNRType",
	"39": "StillImageSNR",
	"40": "UBMethod",
	"41": "SpatialOverCaptureGroupIdentifier",
	"42": "iCloudServerSoftwareVersionForDynamicallyGeneratedMedia",
	"43": "PhotoIdentifier",
	"44": "SpatialOverCaptureImageType",
	"45": "CCT",
	"46": "ApsMode",
	"47": "FocusPosition",
	"48": "MeteorPlusGainMap",
	"49": "StillImageProcessingHomography",
	"50": "IntelligentDistortionCorrection",
	"51": "NRFStatus",
	"52": "NRFInputBracketCount",
	"53": "NRFRegisteredBracketCount",
	"54": "LuxLevel",
	"55": "LastFocusingMethod",
	"56": "TimeOfFlightAssistedAutoFocusEstimatorMeasuredDepth",
	"57": "TimeOfFlightAssistedAutoFocusEstimatorROIType",
	"58": "NRFSRLStatus",
	"59": "SystemPressureLevel",
	"60": "CameraControlsStatisticsMaster",
	"61": "TimeOfFlightAssistedAutoFocusEstimatorSensorConfidence",
	"62": "ColorCorrectionMatrix?",
	"63": "GreenGhostMitigationStatus?",
	"64": "SemanticStyle",
	"65": "SemanticStyleKey_RenderingVersion",
	"66": "SemanticStyleKey_Preset",
	"67": "SemanticStyleKey_ToneBias",
	"68": "SemanticStyleKey_WarmthBias",
	"69": "FrontFacingCamera",
	"70": "TimeOfFlightAssistedAutoFocusEstimatorContainsBlindSpot",
	"71": "LeaderFollowerAutoFocusLeaderDepth",
	"72": "LeaderFollowerAutoFocusLeaderFocusMethod",
	"73": "LeaderFollowerAutoFocusLeaderConfidence",
	"74": "LeaderFollowerAutoFocusLeaderROIType",
	"75": "ZeroShutterLagFailureReason",
	"76": "TimeOfFlightAssistedAutoFocusEstimatorMSPMeasuredDepth",
	"77": "TimeOfFlightAssistedAutoFocusEstimatorMSPSensorConfidence",
	"78": "Camera"
};

export class EXIFData
{
	constructor(exifMaker)
	{
		this.exifMaker = exifMaker;
		this.exifMap = {};
	}

	getEXIFMaker()
	{
		return this.exifMaker;
	}

	clone()
	{
		let item;
		let i;
		let newExif = new EXIFData(this.exifMaker);
		for (i in this.exifMap)
		{
			item = this.exifMap[i];
			newExif.exifMap[i] = item.clone();
		}
		return newExif;
	}

	addBytes(id, data)
	{
		this.exifMap[id] = new EXIFItem(id, EXIFType.Bytes, data);
	}

	addString(id, data)
	{
		this.exifMap[id] = new EXIFItem(id, EXIFType.STRING, data);
	}

	addUInt16(id, data)
	{
		this.exifMap[id] = new EXIFItem(id, EXIFType.UINT16, data);
	}

	addUInt32(id, data)
	{
		this.exifMap[id] = new EXIFItem(id, EXIFType.UINT32, data);
	}

	addInt16(id, data)
	{
		this.exifMap[id] = new EXIFItem(id, EXIFType.INT16, data);
	}

	addInt32(id, data)
	{
		this.exifMap[id] = new EXIFItem(id, EXIFType.INT32, data);
	}

	addRational(id, data)
	{
		this.exifMap[id] = new EXIFItem(id, EXIFType.Rational, data);
	}

	addSRational(id, data)
	{
		this.exifMap[id] = new EXIFItem(id, EXIFType.SRational, data);
	}

	addOther(id, data)
	{
		this.exifMap[id] = new EXIFItem(id, EXIFType.Other, data);
	}

	addSubEXIF(id, data)
	{
		this.exifMap[id] = new EXIFItem(id, EXIFType.SubExif, data);
	}

	addDouble(id, data)
	{
		this.exifMap[id] = new EXIFItem(id, EXIFType.Double, data);
	}

	addUInt64(id, data)
	{
		this.exifMap[id] = new EXIFItem(id, EXIFType.UINT64, data);
	}

	addInt64(id, data)
	{
		this.exifMap[id] = new EXIFItem(id, EXIFType.INT64, data);
	}

	remove(id)
	{
		delete this.exifMap[id];
	}

	getExifIds()
	{
		let ret = [];
		let i;
		for (i in this.exifMap)
			ret.push(Number(i));
		return ret;
	}

	getExifType(id)
	{
		let item = this.exifMap[id];
		if (item)
			return item.type;
		return EXIFType.Unknown;
	}

	/**
	 * 
	 * @param {number} id 
	 * @returns {EXIFItem|null}
	 */
	getExifItem(id)
	{
		return this.exifMap[id];
	}

	/**
	 * 
	 * @param {number} id 
	 */
	getExifUInt16(id)
	{
		let item = this.exifMap[id];
		if (item && item.type == EXIFType.UINT16)
			return item.data;
		return null;
	}

	getExifUInt32(id)
	{
		let item = this.exifMap[id];
		if (item && item.type == EXIFType.UINT32)
			return item.data;
		return null;
	}

	getExifSubexif(id)
	{
		let item = this.exifMap[id];
		if (item && item.type == EXIFType.SubExif)
			return item.data;
		return null;
	}

	getExifOther(id)
	{
		let item = this.exifMap[id];
		if (item && item.type == EXIFType.Other)
			return item.data;
		return null;
	}

	getPhotoDate()
	{
		let item;
		if (this.exifMaker == EXIFMaker.Standard)
		{
			if ((item = this.exifMap[36867]))
			{
				if (item.type == EXIFType.STRING)
				{
					return data.Timestamp.fromStr(item.data, data.DateTimeUtil.getLocalTzQhr());
				}
			}
			if ((item = this.exifMap[36868]))
			{
				if (item.type == EXIFType.STRING)
				{
					return data.Timestamp.fromStr(item.data, data.DateTimeUtil.getLocalTzQhr());
				}
			}
			if ((item = this.exifMap[34665]))
			{
				if (item.type == EXIFType.SubExif)
				{
					let ret = item.data.getPhotoDate();
					if (ret)
						return ret;
				}
			}
			if ((item = this.exifMap[306]))
			{
				if (item.type == EXIFType.STRING)
				{
					return data.Timestamp.fromStr(item.data, data.DateTimeUtil.getLocalTzQhr());
				}
			}
		}
		return null;
	}

	getPhotoMake()
	{
		let item;
		if (this.exifMaker == EXIFMaker.Standard)
		{
			if ((item = this.exifMap[271]))
			{
				if (item.type == EXIFType.STRING)
				{
					return item.data;
				}
			}
		}
		return null;
	}

	getPhotoModel()
	{
		let item;
		if (this.exifMaker == EXIFMaker.Standard)
		{
			if ((item = this.exifMap[272]))
			{
				if (item.type == EXIFType.STRING)
				{
					return item.data;
				}
			}
		}
		if (this.exifMaker == EXIFMaker.Canon)
		{
			if ((item = this.exifMap[6]))
			{
				if (item.type == EXIFType.STRING)
				{
					return item.data;
				}
			}
		}
		return null;
	}

	getPhotoLens()
	{
		let item;
		if (this.exifMaker == EXIFMaker.Canon)
		{
			if ((item = this.exifMap[149]))
			{
				if (item.type == EXIFType.STRING)
				{
					return item.data;
				}
			}
		}
		if (this.exifMaker == EXIFMaker.Panasonic)
		{
			if ((item = this.exifMap[81]))
			{
				if (item.type == EXIFType.STRING)
				{
					return item.data;
				}
			}
		}
		return null;
	}

	getPhotoFNumber()
	{
		let item;
		if (this.exifMaker == EXIFMaker.Standard)
		{
			if ((item = this.exifMap[33437]))
			{
				if (item.type == EXIFType.Rational && item.data.length == 2)
				{
					return item.data[0] / item.data[1];
				}
			}
			if ((item = this.exifMap[34665]))
			{
				if (item.type == EXIFType.SubExif)
				{
					return item.data.getPhotoFNumber();
				}
			}
		}
		return null;
	}

	getPhotoExpTime()
	{
		let item;
		if (this.exifMaker == EXIFMaker.Standard)
		{
			if ((item = this.exifMap[33434]))
			{
				if (item.type == EXIFType.Rational && item.data.length == 2)
				{
					return item.data[0] / item.data[1];
				}
			}
			if ((item = this.exifMap[34665]))
			{
				if (item.type == EXIFType.SubExif)
				{
					return item.data.getPhotoExpTime();
				}
			}
		}
		return null;
	}

	getPhotoISO()
	{
		let item;
		if (this.exifMaker == EXIFMaker.Standard)
		{
			if ((item = this.exifMap[34855]))
			{
				if (item.type == EXIFType.UINT16 && item.data.length == 1)
				{
					return item.data[0];
				}
				else if (item.type == EXIFType.UINT32 && item.data.length == 1)
				{
					return item.data[0];
				}
			}
			if ((item = this.exifMap[34665]))
			{
				if (item.type == EXIFType.SubExif)
				{
					return item.data.getPhotoISO();
				}
			}
		}
		return null;
	}

	getPhotoFocalLength()
	{
		let item;
		if (this.exifMaker == EXIFMaker.Standard)
		{
			if ((item = this.exifMap[37386]))
			{
				if (item.type == EXIFType.Rational && item.data.length == 2)
				{
					return item.data[0] / item.data[1];
				}
			}
			if ((item = this.exifMap[34665]))
			{
				if (item.type == EXIFType.SubExif)
				{
					return item.data.getPhotoFocalLength();
				}
			}
		}
		return null;
	}

	getPhotoLocation()
	{
		let subExif = this.getExifSubexif(34853);
		if (subExif == null)
		{
			return null;
		}

		let ret = {};
		let item1;
		let item2;
		let val;
		item1 = subExif.getExifItem(1);
		item2 = subExif.getExifItem(2);
		if (item1 && item2)
		{
			if (item2.type == EXIFType.Rational)
			{
				if (item2.data.length == 6)
				{
					val = item2.data[0] / item2.data[1];
					val += item2.data[2] / item2.data[3] / 60.0;
					val += item2.data[4] / item2.data[5] / 3600.0;
				}
				else if (item2.data.length == 2)
				{
					val = item2.data[0] / item2.data[1];
				}
				else
				{
					console.log("Latitude length is not valid");
					return null;
				}
			}
			else
			{
				console.log("Latitude type not valid");
				return null;
			}
			if (item1.type == EXIFType.STRING)
			{
				if (item1.data == "S")
				{
					val = -val;
				}
			}
			else
			{
				console.log("LatitudeRef type not valid");
				return null;
			}
			ret.lat = val;
		}
		else
		{
			console.log("Latitude / LatitudeRef not found");
			return null;
		}
		item1 = subExif.getExifItem(3);
		item2 = subExif.getExifItem(4);
		if (item1 && item2)
		{
			if (item2.type == EXIFType.Rational)
			{
				if (item2.data.length == 6)
				{
					val = item2.data[0] / item2.data[1];
					val += item2.data[2] / item2.data[3] / 60.0;
					val += item2.data[4] / item2.data[5] / 3600.0;
				}
				else if (item2.data.length == 2)
				{
					val = item2.data[0] / item2.data[1];
				}
				else
				{
					console.log("Longitude length is not valid");
					return null;
				}
			}
			else
			{
				console.log("Longitude type not valid");
				return null;
			}
			if (item1.type == EXIFType.STRING)
			{
				if (item1.data == "W")
				{
					val = -val;
				}
			}
			else
			{
				console.log("LongitudeRef type not valid");
				return null;
			}
			ret.lon = val;
		}
		else
		{
			console.log("Longitude / LongitudeRef not found");
			return null;
		}
		item1 = subExif.getExifItem(5);
		item2 = subExif.getExifItem(6);
		if (item1 && item2)
		{
			if (item2.type == EXIFType.Rational)
			{
				if (item2.data.length == 2)
				{
					val = item2.data[0] / item2.data[1];
				}
				else
				{
					console.log("Altitude length is not valid");
					return null;
				}
			}
			else
			{
				console.log("Altitude Type is not valid");
				return null;
			}
			if (item1.type == EXIFType.Bytes && item1.data.length == 1)
			{
				if (item1.data[0] == 1)
				{
					val = -val;
				}
			}
			else if (item1.type == EXIFType.UINT16 && item1.data.length == 1)
			{
				if (item1.data[0] == 1)
				{
					val = -val;
				}
			}
			else
			{
				console.log("AltitudeRef Type is not valid");
				return null;
			}
			ret.altitude = val;
		}
		else
		{
			ret.altitude = null;
		}
		item1 = subExif.getExifItem(7);
		item2 = subExif.getExifItem(29);
		if (item1 && item2)
		{
			let hh = 0;
			let mm = 0;
			let ss = 0;
			let ms = 0;

			if (item1.type == EXIFType.Rational && item1.data.length == 6)
			{
				if (item1.data[1] != 1 || item1.data[3] != 1)
				{
					console.log("GPSTime denominator is not valid");
					return null;
				}
				else
				{
					hh = item1.data[0];
					mm = item1.data[2];
					val = item1.data[4] / item1.data[5];
					ss = Math.floor(val);
					ms = (val - ss);
				}
			}
			else
			{
				console.log("GPSTime Type is not valid");
				return null;
			}
			if (item2.type == EXIFType.STRING && item2.data.length >= 10)
			{
				let dateArr = item2.data.split(":");
				if (dateArr.length != 3)
				{
					console.log("GPSDate is not valid");
					return null;
				}
				else
				{
					let tv = new data.TimeValue();
					tv.year = Number.parseInt(dateArr[0]);
					tv.month = Number.parseInt(dateArr[1]);
					tv.day = Number.parseInt(dateArr[2]);
					tv.hour = hh;
					tv.minute = mm;
					tv.second = ss;
					tv.nanosec = Math.floor(ms * 1000000000);
					tv.tzQhr = 0;
					ret.gpsTime = data.Timestamp.fromTimeValue(tv);
				}
			}
			else
			{
				console.log("GPSDate format is not valid", item2.data);
				return null;
			}
		}
		return ret;
	}

	getProperties(id)
	{
		if (id == null)
			id = 0;
		let ret = {};
		let i;
		let item;
		let name;
		for (i in this.exifMap)
		{
			item = this.exifMap[i];
			name = EXIFData.getEXIFName(this.exifMaker, i, id);
			if (item.type == EXIFType.SubExif)
			{
				ret[name] = item.data.getProperties(i);
			}
			else if (item.id == 37500)
			{
				let innerExif = this.parseMakerNote(item.data);
				if (innerExif)
				{
					ret[name] = innerExif.getProperties(0);
				}
				else
				{
					ret[name] = item.toDataString();
				}
			}
			else
			{
				ret[name] = item.toDataString();
			}
		}
		return ret;
	}

	toString(linePrefix)
	{
		let sb = [];
		let exItem;
		let i;
		let j;
		let k;
		let v;
	
		sb.push("EXIF Content:");
		let exifIds = this.getExifIds();
		i = 0;
		j = exifIds.length;
		while (i < j)
		{
			v = exifIds[i];
			sb.push("\r\n");
			if (linePrefix) sb.push(linePrefix);
			sb.push("Id = ");
			sb.push(v);
			sb.push(", name = ");
			sb.push(EXIFData.getEXIFName(this.exifMaker, v));
			if ((exItem = this.getExifItem(v)) != null)
			{
				if (exItem.type == EXIFType.SubExif)
				{
					let i2;
					let j2;
					let v2;
					let subExItem;
					let subExif = exItem.data;
					let subExIds = subExif.getExifIds();
					i2 = 0;
					j2 = subExIds.length;
					while (i2 < j2)
					{
						v2 = subExIds[i2];
						sb.push("\r\n");
						if (linePrefix) sb.push(linePrefix);
						sb.push(" Subid = ");
						sb.push(v2);
						sb.push(", name = ");
						sb.push(EXIFData.getEXIFName(this.exifMaker, v2, v));
	
						if ((subExItem = subExif.getExifItem(v2)) != null)
						{
							if (subExItem.type == EXIFType.STRING)
							{
								sb.push(", value = ");
								sb.push(subExItem.data);
							}
							else if (subExItem.type == EXIFType.Double)
							{
								let valBuff = subExItem.data;
								k = 0;
								while (k < valBuff.length)
								{
									if (k == 0)
									{
										sb.push(", value = ");
									}
									else
									{
										sb.push(", ");
									}
									sb.push(valBuff[k]);
									k++;
								}
							}
							else if (subExItem.type == EXIFType.Bytes)
							{
								let valBuff = subExItem.data;
								sb.push(", value = ");
								if (valBuff.length > 1024)
								{
									sb.push(valBuff.length);
									sb.push(" bytes: ");
									sb.push(text.u8Arr2Hex(new Uint8Array(valBuff, 0, 256), ' ', "\r\n"));
									sb.push("\r\n...\r\n");
									sb.push(text.u8Arr2Hex(new Uint8Array(valBuff, (valBuff.length & ~15) - 256, 256 + (valBuff.length & 15)), ' ', "\r\n"));
								}
								else
								{
									sb.push(text.u8Arr2Hex(new Uint8Array(valBuff), ' ', "\r\n"));
								}
							}
							else if (subExItem.type == EXIFType.UINT16)
							{
								let valBuff = subExItem.data;
								k = 0;
								while (k < valBuff.length)
								{
									if (k == 0)
									{
										sb.push(", value = ");
									}
									else
									{
										sb.push(", ");
									}
									sb.push(valBuff[k]);
									k++;
								}
							}
							else if (subExItem.type == EXIFType.UINT32)
							{
								let valBuff = subExItem.data;
								k = 0;
								while (k < valBuff.length)
								{
									if (k == 0)
									{
										sb.push(", value = ");
									}
									else
									{
										sb.push(", ");
									}
									sb.push(valBuff[k]);
									k++;
								}
							}
							else if (subExItem.type == EXIFType.Rational)
							{
								let valBuff = subExItem.data;
								k = 0;
								while (k < valBuff.length)
								{
									if (k == 0)
									{
										sb.push(", value = ");
									}
									else
									{
										sb.push(", ");
									}
									sb.push(valBuff[k]);
									sb.push(" / ");
									sb.push(valBuff[k + 1]);
									if (valBuff[k + 1] != 0)
									{
										sb.push(" (");
										sb.push(valBuff[k] / valBuff[k + 1]);
										sb.push(")");
									}
									k += 2;
								}
							}
							else if (subExItem.type == EXIFType.SRational)
							{
								let valBuff = subExItem.data;
								k = 0;
								while (k < valBuff.length)
								{
									if (k == 0)
									{
										sb.push(", value = ");
									}
									else
									{
										sb.push(", ");
									}
									sb.push(valBuff[k]);
									sb.push(" / ");
									sb.push(valBuff[k + 1]);
									if (valBuff[k + 1] != 0)
									{
										sb.push(" (");
										sb.push(valBuff[k] / valBuff[k + 1]);
										sb.push(")");
									}
									k += 2;
								}
							}
							else if (subExItem.type == EXIFType.INT16)
							{
								let valBuff = subExItem.data;
								k = 0;
								while (k < valBuff.length)
								{
									if (k == 0)
									{
										sb.push(", value = ");
									}
									else
									{
										sb.push(", ");
									}
									sb.push(valBuff[k]);
									k++;
								}
							}
							else if (subExItem.type == EXIFType.INT32)
							{
								let valBuff = subExItem.data;
								k = 0;
								while (k < valBuff.length)
								{
									if (k == 0)
									{
										sb.push(", value = ");
									}
									else
									{
										sb.push(", ");
									}
									sb.push(valBuff[k]);
									k++;
								}
							}
							else if (subExItem.id == 37500)
							{
								let valBuff = subExItem.data;
								let innerExif;
								let nnlinePrefix;
								if ((innerExif = this.parseMakerNote(valBuff)) != null)
								{
									sb.push(", Format = ");
									sb.push(innerExif.getEXIFMaker());
									sb.push(", Inner ");
									if (linePrefix)
									{
										sb.push(innerExif.toString(" "+linePrefix));
									}
									else
									{
										sb.push(innerExif.toString(" "));
									}
								}
								else
								{
									sb.push(", value (Other) = ");
									sb.push(text.u8Arr2Hex(valBuff, ' ', "\r\n"));
								}
							}
							else if (subExItem.type == EXIFType.Other)
							{
								let valBuff = subExItem.data;
								if (this.exifMaker == EXIFMaker.Olympus && subExItem.id == 0)
								{
									sb.push(", value = ");
									sb.push(valBuff); //////////////////
								}
								else
								{
									sb.push(", value (Other) = ");
									sb.push(text.u8Arr2Hex(valBuff, ' ', "\r\n"));
								}
							}
							else
							{
								let valBuff = subExItem.data;
								sb.push(", value (Unk) = ");
								sb.push(text.u8Arr2Hex(valBuff, ' ', "\r\n"));
							}
						}	
						i2++;
					}
				}
				else if (exItem.type == EXIFType.STRING)
				{
					sb.push(", value = ");
					sb.push(exItem.data);
				}
				else if (exItem.type == EXIFType.Double)
				{
					let valBuff = exItem.data;
					k = 0;
					while (k < valBuff.length)
					{
						if (k == 0)
						{
							sb.push(", value = ");
						}
						else
						{
							sb.push(", ");
						}
						sb.push(valBuff[k]);
						k++;
					}
				}
				else if (exItem.type == EXIFType.Bytes)
				{
					let valBuff = exItem.data;
					sb.push(", value = ");
					if (exItem.id >= 40091 && exItem.id <= 40095)
					{
						let enc = new text.UTF16LETextBinEnc();
						if (valBuff[valBuff.length - 2] == 0)
						{
							sb.push(enc.encodeBin(new Uint8Array(valBuff, 0, valBuff.length - 2)));
						}
						else
						{
							sb.push(enc.encodeBin(new Uint8Array(valBuff, 0, valBuff.length)));
						}
					}
					else
					{
						if (exItem.data.length > 1024)
						{
							sb.push(exItem.data.length);
							sb.push(" bytes: ");
							sb.push(text.u8Arr2Hex(new Uint8Array(valBuff, 0, 256), ' ', "\r\n"));
							sb.push("\r\n...\r\n");
							sb.push(text.u8Arr2Hex(new Uint8Array(valBuff, (valBuff.length & ~15) - 256, 256 + (valBuff.length & 15)), ' ', "\r\n"));
						}
						else
						{
							sb.push(text.u8Arr2Hex(new Uint8Array(valBuff), ' ', "\r\n"));
						}
					}
				}
				else if (exItem.type == EXIFType.UINT16)
				{
					let valBuff = exItem.data;
					if (this.exifMaker == EXIFMaker.Canon && exItem.id == 1)
					{
						sb.push(this.toStringCanonCameraSettings(linePrefix, valBuff));
					}
					else if (this.exifMaker == EXIFMaker.Canon && exItem.id == 2)
					{
						sb.push(this.toStringCanonFocalLength(linePrefix, valBuff));
					}
					else if (this.exifMaker == EXIFMaker.Canon && exItem.id == 4)
					{
						sb.push(this.toStringCanonShotInfo(linePrefix, valBuff));
					}
					else
					{
						k = 0;
						while (k < valBuff.length)
						{
							if (k == 0)
							{
								sb.push(", value = ");
							}
							else
							{
								sb.push(", ");
							}
							sb.push(valBuff[k]);
							k++;
						}
					}
				}
				else if (exItem.type == EXIFType.UINT32)
				{
					let valBuff = exItem.data;
					k = 0;
					while (k < valBuff.length)
					{
						if (k == 0)
						{
							sb.push(", value = ");
						}
						else
						{
							sb.push(", ");
						}
						sb.push(valBuff[k]);
						k++;
					}
				}
				else if (exItem.type == EXIFType.Rational)
				{
					let valBuff = exItem.data;
					k = 0;
					while (k < valBuff.length)
					{
						if (k == 0)
						{
							sb.push(", value = ");
						}
						else
						{
							sb.push(", ");
						}
						sb.push(valBuff[k]);
						sb.push(" / ");
						sb.push(valBuff[k + 1]);
						if (valBuff[k + 1] != 0)
						{
							sb.push(" (");
							sb.push(valBuff[k] / valBuff[k + 1]);
							sb.push(")");
						}
						k += 2;
					}
				}
				else if (exItem.type == EXIFType.SRational)
				{
					let valBuff = exItem.data;
					k = 0;
					while (k < valBuff.length)
					{
						if (k == 0)
						{
							sb.push(", value = ");
						}
						else
						{
							sb.push(", ");
						}
						sb.push(valBuff[k]);
						sb.push(" / ");
						sb.push(valBuff[k + 1]);
						if (valBuff[k + 1] != 0)
						{
							sb.push(" (");
							sb.push(valBuff[k] / valBuff[k + 1]);
							sb.push(")");
						}
						k += 2;
					}
				}
				else if (exItem.type == EXIFType.INT16)
				{
					let valBuff = exItem.data;
					k = 0;
					while (k < valBuff.length)
					{
						if (k == 0)
						{
							sb.push(", value = ");
						}
						else
						{
							sb.push(", ");
						}
						sb.push(valBuff[k]);
						k++;
					}
				}
				else if (exItem.type == EXIFType.INT32)
				{
					let valBuff = exItem.data;
					k = 0;
					while (k < valBuff.length)
					{
						if (k == 0)
						{
							sb.push(", value = ");
						}
						else
						{
							sb.push(", ");
						}
						sb.push(valBuff[k]);
						k++;
					}
				}
				else if (exItem.type == EXIFType.Other)
				{
					if (this.exifMaker == EXIFMaker.Olympus && exItem.id == 521)
					{
						sb.push(", value = ");
						sb.push(exItem.data);
					}
					else
					{
			//			UInt8 *valBuff;
			//			valBuff = (UInt8*)exItem->dataBuff;
						sb.push(", Other: size = ");
						sb.push(exItem.data.length);
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
					sb.push(", Unknown: size = ");
					sb.push(exItem.data.length);
		//			sb->AppendHex(valBuff, subExItem->cnt, ' ', Text::StringBuilder::LBT_CRLF);
				}
			}
			i++;
		}
		let loc;
		if ((loc = this.getPhotoLocation()) != null)
		{
			sb.push("\r\nGPS Location: ");
			sb.push(loc.lat);
			sb.push(", ");
			sb.push(loc.lon);
			sb.push(", ");
			sb.push(loc.altitude);
		}
		return sb.join("");
	}

	toStringCanonCameraSettings(linePrefix, valBuff)
	{
		let sb = [];
		let isInt16;
		let isUInt16;
		let k;
		k = 0;
		while (k < valBuff.length)
		{
			sb.push("\r\n");
			if (linePrefix) sb.push(linePrefix);
			sb.push(" ");
			isInt16 = false;
			isUInt16 = false;
			switch (k)
			{
			case 1:
				sb.push("MacroMode = ");
				switch (valBuff[k])
				{
				case 1:
					sb.push("1-Macro");
					break;
				case 2:
					sb.push("2-Normal");
					break;
				default:
					sb.push(valBuff[k]);
					break;
				}
				break;
			case 2:
				sb.push("SelfTimer = ");
				isInt16 = true;
				break;
			case 3:
				sb.push("Quality = ");
				switch (valBuff[k])
				{
				case -1:
					sb.push("-1-n/a");
					break;
				case 1:
					sb.push("1-Economy");
					break;
				case 2:
					sb.push("2-Normal");
					break;
				case 3:
					sb.push("3-Fine");
					break;
				case 4:
					sb.push("4-RAW");
					break;
				case 5:
					sb.push("5-Superfine");
					break;
				case 7:
					sb.push("7-CRAW");
					break;
				case 130:
					sb.push("130-Normal Movie");
					break;
				case 131:
					sb.push("131-Movie (2)");
					break;
				default:
					sb.push(valBuff[k]);
					break;
				}
				break;
			case 4:
				sb.push("CanonFlashMode = ");
				switch (valBuff[k])
				{
				case -1:
					sb.push("-1-n/a");
					break;
				case 0:
					sb.push("0-Off");
					break;
				case 1:
					sb.push("1-Auto");
					break;
				case 2:
					sb.push("2-On");
					break;
				case 3:
					sb.push("3-Red-eye Reduction");
					break;
				case 4:
					sb.push("4-Slow Sync");
					break;
				case 5:
					sb.push("5-Red-eye Reduction (Auto)");
					break;
				case 6:
					sb.push("6-Red-eye Reduction (On)");
					break;
				case 16:
					sb.push("16-External Flash");
					break;
				default:
					sb.push(valBuff[k]);
					break;
				}
				break;
			case 5:
				sb.push("ContinuousDrive = ");
				switch (valBuff[k])
				{
				case 0:
					sb.push("0-Single");
					break;
				case 1:
					sb.push("1-Continuous");
					break;
				case 2:
					sb.push("2-Movie");
					break;
				case 3:
					sb.push("3-Continuous, Speed Priority");
					break;
				case 4:
					sb.push("4-Continuous, Low");
					break;
				case 5:
					sb.push("5-Continuous, High");
					break;
				case 6:
					sb.push("6-Silent Single");
					break;
				case 9:
					sb.push("9-Single, Silent");
					break;
				case 10:
					sb.push("10-Continuous, Silent");
					break;
				default:
					sb.push(valBuff[k]);
					break;
				}
				break;
			case 7:
				sb.push("FocusMode = ");
				switch (valBuff[k])
				{
				case 0:
					sb.push("0-One-shot AF");
					break;
				case 1:
					sb.push("1-AI Servo AF");
					break;
				case 2:
					sb.push("2-AI Focus AF");
					break;
				case 3:
					sb.push("3-Manual Focus");
					break;
				case 4:
					sb.push("4-Single");
					break;
				case 5:
					sb.push("5-Continuous");
					break;
				case 6:
					sb.push("6-Manual Focus");
					break;
				case 16:
					sb.push("16-Pan Focus");
					break;
				case 256:
					sb.push("256-AF+MF");
					break;
				case 512:
					sb.push("512-Movie Snap Focus");
					break;
				case 519:
					sb.push("519-Movie Servo AF");
					break;
				default:
					sb.push(valBuff[k]);
					break;
				}
				break;
			case 9:
				sb.push("RecordMode = ");
				switch (valBuff[k])
				{
				case 1:
					sb.push("1-JPEG");
					break;
				case 2:
					sb.push("2-CRW+THM");
					break;
				case 3:
					sb.push("3-AVI+THM");
					break;
				case 4:
					sb.push("4-TIF");
					break;
				case 5:
					sb.push("5-TIF+JPEG");
					break;
				case 6:
					sb.push("6-CR2");
					break;
				case 7:
					sb.push("7-CR2+JPEG");
					break;
				case 9:
					sb.push("9-MOV");
					break;
				case 10:
					sb.push("10-MP4");
					break;
				case 11:
					sb.push("11-CRM");
					break;
				case 12:
					sb.push("12-CR3");
					break;
				case 13:
					sb.push("13-CR3+JPEG");
					break;
				default:
					sb.push(valBuff[k]);
					break;
				}
				break;
			case 10:
				sb.push("CanonImageSize = ");
				switch (valBuff[k])
				{
				case -1:
					sb.push("-1-n/a");
					break;
				case 0:
					sb.push("0-Large");
					break;
				case 1:
					sb.push("1-Medium");
					break;
				case 2:
					sb.push("2-Small");
					break;
				case 5:
					sb.push("5-Medium 1");
					break;
				case 6:
					sb.push("6-Medium 2");
					break;
				case 7:
					sb.push("7-Medium 3");
					break;
				case 8:
					sb.push("8-Postcard");
					break;
				case 9:
					sb.push("9-Widescreen");
					break;
				case 10:
					sb.push("10-Medium Widescreen");
					break;
				case 14:
					sb.push("14-Small 1");
					break;
				case 15:
					sb.push("15-Small 2");
					break;
				case 16:
					sb.push("16-Small 3");
					break;
				case 128:
					sb.push("128-640x480 Movie");
					break;
				case 129:
					sb.push("129-Medium Movie");
					break;
				case 130:
					sb.push("130-Small Movie");
					break;
				case 137:
					sb.push("137-1280x720 Movie");
					break;
				case 142:
					sb.push("142-1920x1080 Movie");
					break;
				case 143:
					sb.push("143-4096x2160 Movie");
					break;
				default:
					sb.push(valBuff[k]);
					break;
				}
				break;
			case 11:
				sb.push("EasyMode = ");
				switch (valBuff[k])
				{
				case 0:
					sb.push("0-Full auto");
					break;
				case 1:
					sb.push("1-Manual");
					break;
				case 2:
					sb.push("2-Landscape");
					break;
				case 3:
					sb.push("3-Fast shutter");
					break;
				case 4:
					sb.push("4-Slow shutter");
					break;
				case 5:
					sb.push("5-Night");
					break;
				case 6:
					sb.push("6-Grey Scale");
					break;
				case 7:
					sb.push("7-Sepia");
					break;
				case 8:
					sb.push("8-Portrait");
					break;
				case 9:
					sb.push("9-Sports");
					break;
				case 10:
					sb.push("10-Macro");
					break;
				case 11:
					sb.push("11-Black & White");
					break;
				case 12:
					sb.push("12-Pan focus");
					break;
				case 13:
					sb.push("13-Vivid");
					break;
				case 14:
					sb.push("14-Neutral");
					break;
				case 15:
					sb.push("15-Flash Off");
					break;
				case 16:
					sb.push("16-Long Shutter");
					break;
				case 17:
					sb.push("17-Super Macro");
					break;
				case 18:
					sb.push("18-Foliage");
					break;
				case 19:
					sb.push("19-Indoor");
					break;
				case 20:
					sb.push("20-Fireworks");
					break;
				case 21:
					sb.push("21-Beach");
					break;
				case 22:
					sb.push("22-Underwater");
					break;
				case 23:
					sb.push("23-Snow");
					break;
				case 24:
					sb.push("24-Kids & Pets");
					break;
				case 25:
					sb.push("25-Night Snapshot");
					break;
				case 26:
					sb.push("26-Digital Macro");
					break;
				case 27:
					sb.push("27-My Colors");
					break;
				case 28:
					sb.push("28-Movie Snap");
					break;
				case 29:
					sb.push("29-Super Macro 2");
					break;
				case 30:
					sb.push("30-Color Accent");
					break;
				case 31:
					sb.push("31-Color Swap");
					break;
				case 32:
					sb.push("32-Aquarium");
					break;
				case 33:
					sb.push("33-ISO 3200");
					break;
				case 34:
					sb.push("34-ISO 6400");
					break;
				case 35:
					sb.push("35-Creative Light Effect");
					break;
				case 36:
					sb.push("36-Easy");
					break;
				case 37:
					sb.push("37-Quick Shot");
					break;
				case 38:
					sb.push("38-Creative Auto");
					break;
				case 39:
					sb.push("39-Zoom Blur");
					break;
				case 40:
					sb.push("40-Low Light");
					break;
				case 41:
					sb.push("41-Nostalgic");
					break;
				case 42:
					sb.push("42-Super Vivid");
					break;
				case 43:
					sb.push("43-Poster Effect");
					break;
				case 44:
					sb.push("44-Face Self-Time");
					break;
				case 45:
					sb.push("45-Smile");
					break;
				case 46:
					sb.push("46-Wink Self-Timer");
					break;
				case 47:
					sb.push("47-Fisheye Effect");
					break;
				case 48:
					sb.push("48-Miniature Effect");
					break;
				case 49:
					sb.push("49-High-speed Burst");
					break;
				case 50:
					sb.push("50-Best Image Selection");
					break;
				case 51:
					sb.push("51-High Dynamic Range");
					break;
				case 52:
					sb.push("52-Handheld Night Scene");
					break;
				case 53:
					sb.push("53-Movie Digest");
					break;
				case 54:
					sb.push("54-Live View Control");
					break;
				case 55:
					sb.push("55-Discreet");
					break;
				case 56:
					sb.push("56-Blur Reduction");
					break;
				case 57:
					sb.push("57-Monochrome");
					break;
				case 58:
					sb.push("58-Toy Camera Effect");
					break;
				case 59:
					sb.push("59-Scene Intelligent Auto");
					break;
				case 60:
					sb.push("60-High-speed Burst HQ");
					break;
				case 61:
					sb.push("61-Smooth Skin");
					break;
				case 62:
					sb.push("62-Soft Focus");
					break;
				case 257:
					sb.push("257-Spotlight");
					break;
				case 258:
					sb.push("258-Night 2");
					break;
				case 259:
					sb.push("259-Night+");
					break;
				case 260:
					sb.push("260-Super Night");
					break;
				case 261:
					sb.push("261-Sunset");
					break;
				case 263:
					sb.push("263-Night Scene");
					break;
				case 264:
					sb.push("264-Surface");
					break;
				case 265:
					sb.push("265-Low Light 2");
					break;
				default:
					sb.push(valBuff[k]);
					break;
				}
				break;
			case 12:
				sb.push("DigitalZoom = ");
				switch (valBuff[k])
				{
				case 0:
					sb.push("0-None");
					break;
				case 1:
					sb.push("1-2x");
					break;
				case 2:
					sb.push("2-4x");
					break;
				case 3:
					sb.push("3-Other");
					break;
				default:
					sb.push(valBuff[k]);
					break;
				}
				break;
			case 13:
				sb.push("Contrast = ");
				isInt16 = true;
				break;
			case 14:
				sb.push("Saturation = ");
				isInt16 = true;
				break;
			case 15:
				sb.push("Sharpness = ");
				isInt16 = true;
				break;
			case 16:
				sb.push("CameraISO = ");
				isInt16 = true;
				break;
			case 17:
				sb.push("MeteringMode = ");
				switch (valBuff[k])
				{
				case 0:
					sb.push("0-Default");
					break;
				case 1:
					sb.push("1-Spot");
					break;
				case 2:
					sb.push("2-Average");
					break;
				case 3:
					sb.push("3-Evaluative");
					break;
				case 4:
					sb.push("4-Partial");
					break;
				case 5:
					sb.push("5-Center-weighted average");
					break;
				default:
					sb.push(valBuff[k]);
					break;
				}
				break;
			case 18:
				sb.push("FocusRange = ");
				switch (valBuff[k])
				{
				case 0:
					sb.push("0-Manual");
					break;
				case 1:
					sb.push("1-Auto");
					break;
				case 2:
					sb.push("2-Not Known");
					break;
				case 3:
					sb.push("3-Macro");
					break;
				case 4:
					sb.push("4-Very Close");
					break;
				case 5:
					sb.push("5-Close");
					break;
				case 6:
					sb.push("6-Middle Range");
					break;
				case 7:
					sb.push("7-Far Range");
					break;
				case 8:
					sb.push("8-Pan Focus");
					break;
				case 9:
					sb.push("9-Super Macro");
					break;
				case 10:
					sb.push("10-Infinity");
					break;
				default:
					sb.push(valBuff[k]);
					break;
				}
				break;
			case 19:
				sb.push("AFPoint = ");
				switch (valBuff[k])
				{
				case 0x2005:
					sb.push("0x2005-Manual AF point selection ");
					break;
				case 0x3000:
					sb.push("0x3000-None (MF)");
					break;
				case 0x3001:
					sb.push("0x3001-Auto AF point selection");
					break;
				case 0x3002:
					sb.push("0x3002-Right");
					break;
				case 0x3003:
					sb.push("0x3003-Center");
					break;
				case 0x3004:
					sb.push("0x3004-Left");
					break;
				case 0x4001:
					sb.push("0x4001-Auto AF point selection");
					break;
				case 0x4006:
					sb.push("0x4006-Face Detect");
					break;
				default:
					sb.push(valBuff[k]);
					break;
				}
				break;
			case 20:
				sb.push("CanonExposureMode = ");
				switch (valBuff[k])
				{
				case 0:
					sb.push("0-Easy");
					break;
				case 1:
					sb.push("1-Program AE");
					break;
				case 2:
					sb.push("2-Shutter speed priority AE");
					break;
				case 3:
					sb.push("3-Aperture-priority AE");
					break;
				case 4:
					sb.push("4-Manual");
					break;
				case 5:
					sb.push("5-Depth-of-field AE");
					break;
				case 6:
					sb.push("6-M-Dep");
					break;
				case 7:
					sb.push("7-Bulb");
					break;
				default:
					sb.push(valBuff[k]);
					break;
				}
				break;
			case 22:
				sb.push("LensType = ");
				sb.push(this.toStringCanonLensType(valBuff[k]));
				break;
			case 23:
				sb.push("MaxFocalLength = ");
				isUInt16 = true;
				break;
			case 24:
				sb.push("MinFocalLength = ");
				isUInt16 = true;
				break;
			case 25:
				sb.push("FocalUnits = ");
				isInt16 = true;
				break;
			case 26:
				sb.push("MaxAperture = ");
				isInt16 = true;
				break;
			case 27:
				sb.push("MinAperture = ");
				isInt16 = true;
				break;
			case 28:
				sb.push("FlashActivity = ");
				isInt16 = true;
				break;
			case 29:
				sb.push("FlashBits = 0x");
				sb.push(text.toHex16(valBuff[k]));
				break;
			case 32:
				sb.push("FocusContinuous = ");
				switch (valBuff[k])
				{
				case 0:
					sb.push("0-Single");
					break;
				case 1:
					sb.push("1-Continuous");
					break;
				case 8:
					sb.push("8-Manual");
					break;
				default:
					sb.push(valBuff[k]);
					break;
				}
				break;
			case 33:
				sb.push("AESetting = ");
				switch (valBuff[k])
				{
				case 0:
					sb.push("0-Normal AE");
					break;
				case 1:
					sb.push("1-Exposure Compensation");
					break;
				case 2:
					sb.push("2-AE Lock");
					break;
				case 3:
					sb.push("3-AE Lock + Exposure Comp.");
					break;
				case 4:
					sb.push("4-No AE");
					break;
				default:
					sb.push(valBuff[k]);
					break;
				}
				break;
			case 34:
				sb.push("ImageStabilization = ");
				switch (valBuff[k])
				{
				case 0:
					sb.push("0-Off");
					break;
				case 1:
					sb.push("1-On");
					break;
				case 2:
					sb.push("2-Shoot Only");
					break;
				case 3:
					sb.push("3-Panning");
					break;
				case 4:
					sb.push("4-Dynamic");
					break;
				case 256:
					sb.push("256-Off");
					break;
				case 257:
					sb.push("257-On");
					break;
				case 258:
					sb.push("258-Shoot Only");
					break;
				case 259:
					sb.push("259-Panning");
					break;
				case 260:
					sb.push("260-Dynamic");
					break;
				default:
					sb.push(valBuff[k]);
					break;
				}
				break;
			case 35:
				sb.push("DisplayAperture = ");
				isInt16 = true;
				break;
			case 36:
				sb.push("ZoomSourceWidth = ");
				isInt16 = true;
				break;
			case 37:
				sb.push("ZoomTargetWidth = ");
				isInt16 = true;
				break;
			case 39:
				sb.push("AESetting = ");
				switch (valBuff[k])
				{
				case 0:
					sb.push("0-Center");
					break;
				case 1:
					sb.push("1-AF Point");
					break;
				default:
					sb.push(valBuff[k]);
					break;
				}
				break;
			case 40:
				sb.push("PhotoEffect = ");
				switch (valBuff[k])
				{
				case 0:
					sb.push("0-Off");
					break;
				case 1:
					sb.push("1-Vivid");
					break;
				case 2:
					sb.push("2-Neutral");
					break;
				case 3:
					sb.push("3-Smooth");
					break;
				case 4:
					sb.push("4-Sepia");
					break;
				case 5:
					sb.push("5-B&W");
					break;
				case 6:
					sb.push("6-Custom");
					break;
				case 100:
					sb.push("100-My Color Data");
					break;
				default:
					sb.push(valBuff[k]);
					break;
				}
				break;
			case 41:
				sb.push("ManualFlashOutput = ");
				switch (valBuff[k])
				{
				case 0:
					sb.push("0-n/a");
					break;
				case 0x500:
					sb.push("0x500-Full");
					break;
				case 0x502:
					sb.push("0x500-Medium");
					break;
				case 0x504:
					sb.push("0x500-Low");
					break;
				case 0x7fff:
					sb.push("0x7fff-n/a");
					break;
				default:
					sb.push("0x");
					sb.push(text.toHex16(valBuff[k]));
					break;
				}
				break;
			case 42:
				sb.push("ColorTone = ");
				isInt16 = true;
				break;
			case 46:
				sb.push("SRAWQuality = ");
				switch (valBuff[k])
				{
				case 0:
					sb.push("0-n/a");
					break;
				case 1:
					sb.push("1-sRAW1(mRAW)");
					break;
				case 2:
					sb.push("2-sRAW2(sRAW)");
					break;
				default:
					sb.push("0x");
					sb.push(text.toHex16(valBuff[k]));
					break;
				}
				break;
			default:
				sb.push("Unknown(");
				sb.push(k);
				sb.push(") = ");
				isInt16 = true;
				break;
			}
			if (isInt16)
			{
				sb.push(valBuff[k]);
			}
			else if (isUInt16)
			{
				sb.push(valBuff[k]);
			}
			k++;
		}
		return sb.join("");
	}

	toStringCanonFocalLength(linePrefix, valBuff)
	{
		let sb = [];
		let isInt16;
		let isUInt16;
		let k;
		k = 0;
		while (k < valBuff.length)
		{
			sb.push("\r\n");
			if (linePrefix) sb.push(linePrefix);
			sb.push(" ");
			isInt16 = false;
			isUInt16 = false;
			switch (k)
			{
			case 0:
				sb.push("FocalType = ");
				switch (valBuff[k])
				{
				case 1:
					sb.push("1-Fixed");
					break;
				case 2:
					sb.push("2-Zoom");
					break;
				default:
					sb.push(valBuff[k]);
					break;
				}
				break;
			case 1:
				sb.push("FocalLength = ");
				isUInt16 = true;
				break;
			case 2:
				sb.push("FocalPlaneXSize = ");
				isUInt16 = true;
				break;
			case 3:
				sb.push("FocalPlaneYSize = ");
				isUInt16 = true;
				break;
			default:
				sb.push("Unknown(");
				sb.push(k);
				sb.push(") = ");
				isInt16 = true;
				break;
			}
			if (isInt16)
			{
				sb.push(valBuff[k]);
			}
			else if (isUInt16)
			{
				sb.push(valBuff[k]);
			}
			k++;
		}
		return sb.join("");
	}

	toStringCanonShotInfo(linePrefix, valBuff)
	{
		let sb = [];
		let isInt16;
		let isUInt16;
		let k;
		k = 0;
		while (k < valBuff.length)
		{
			sb.push("\r\n");
			if (linePrefix) sb.push(linePrefix);
			sb.push(" ");
			isInt16 = false;
			isUInt16 = false;
			switch (k)
			{
			case 1:
				sb.push("AutoISO = ");
				isInt16 = true;
				break;
			case 2:
				sb.push("BaseISO = ");
				isInt16 = true;
				break;
			case 3:
				sb.push("MeasuredEV = ");
				isInt16 = true;
				break;
			case 4:
				sb.push("TargetAperture = ");
				isInt16 = true;
				break;
			case 5:
				sb.push("TargetExposureTime = ");
				isInt16 = true;
				break;
			case 6:
				sb.push("ExposureCompensation = ");
				isInt16 = true;
				break;
			case 7:
				sb.push("WhiteBalance = ");
				isInt16 = true;
				break;
			case 8:
				sb.push("SlowShutter = ");
				switch (valBuff[k])
				{
				case -1:
					sb.push("-1-n/a");
					break;
				case 0:
					sb.push("0-Off");
					break;
				case 1:
					sb.push("1-Night Scene");
					break;
				case 2:
					sb.push("2-On");
					break;
				case 3:
					sb.push("3-None");
					break;
				default:
					sb.push(valBuff[k]);
					break;
				}
				break;
			case 9:
				sb.push("SequenceNumber = ");
				isInt16 = true;
				break;
			case 10:
				sb.push("OpticalZoomCode = ");
				isInt16 = true;
				break;
			case 12:
				sb.push("CameraTemperature = ");
				isInt16 = true;
				break;
			case 13:
				sb.push("FlashGuideNumber = ");
				isInt16 = true;
				break;
			case 14:
				sb.push("AFPointsInFocus = ");
				switch (valBuff[k])
				{
				case 0x3000:
					sb.push("0x3000-None (MF)");
					break;
				case 0x3001:
					sb.push("0x3001-Right");
					break;
				case 0x3002:
					sb.push("0x3002-Center");
					break;
				case 0x3003:
					sb.push("0x3003-Center+Right");
					break;
				case 0x3004:
					sb.push("0x3004-Left");
					break;
				case 0x3005:
					sb.push("0x3005-Left+Right");
					break;
				case 0x3006:
					sb.push("0x3006-Left+Center");
					break;
				case 0x3007:
					sb.push("0x3007-All");
					break;
				default:
					sb.push("0x");
					sb.push(text.toHex16(valBuff[k]));
					break;
				}
				break;
			case 15:
				sb.push("FlashExposureComp = ");
				isInt16 = true;
				break;
			case 16:
				sb.push("AutoExposureBracketing = ");
				switch (valBuff[k])
				{
				case -1:
					sb.push("-1-On");
					break;
				case 0:
					sb.push("0-Off");
					break;
				case 1:
					sb.push("1-On (shot 1)");
					break;
				case 2:
					sb.push("2-On (shot 2)");
					break;
				case 3:
					sb.push("3-On (shot 3)");
					break;
				default:
					sb.push(valBuff[k]);
					break;
				}
				break;
			case 17:
				sb.push("AEBBracketValue = ");
				isInt16 = true;
				break;
			case 18:
				sb.push("ControlMode = ");
				switch (valBuff[k])
				{
				case 0:
					sb.push("0-n/a");
					break;
				case 1:
					sb.push("1-Camera Local Control");
					break;
				case 3:
					sb.push("3-Computer Remote Control");
					break;
				default:
					sb.push(valBuff[k]);
					break;
				}
				break;
			case 19:
				sb.push("FocusDistanceUpper = ");
				isUInt16 = true;
				break;
			case 20:
				sb.push("FocusDistanceLower = ");
				isUInt16 = true;
				break;
			case 21:
				sb.push("FNumber = ");
				isInt16 = true;
				break;
			case 22:
				sb.push("ExposureTime = ");
				isInt16 = true;
				break;
			case 23:
				sb.push("MeasuredEV2 = ");
				isInt16 = true;
				break;
			case 24:
				sb.push("BulbDuration = ");
				isInt16 = true;
				break;
			case 26:
				sb.push("CameraType = ");
				switch (valBuff[k])
				{
				case 0:
					sb.push("0-n/a");
					break;
				case 248:
					sb.push("248-EOS High-end");
					break;
				case 250:
					sb.push("250-Compact");
					break;
				case 252:
					sb.push("252-EOS Mid-range");
					break;
				case 255:
					sb.push("255-DV Camera");
					break;
				default:
					sb.push(valBuff[k]);
					break;
				}
				break;
			case 27:
				sb.push("AutoRotate = ");
				switch (valBuff[k])
				{
				case -1:
					sb.push("-1-n/a");
					break;
				case 0:
					sb.push("0-None");
					break;
				case 1:
					sb.push("1-Rotate 90 CW");
					break;
				case 2:
					sb.push("1-Rotate 180");
					break;
				case 3:
					sb.push("3-Rotate 270 CW");
					break;
				default:
					sb.push(valBuff[k]);
					break;
				}
				break;
			case 28:
				sb.push("NDFilter = ");
				switch (valBuff[k])
				{
				case -1:
					sb.push("-1-n/a");
					break;
				case 0:
					sb.push("0-Off");
					break;
				case 1:
					sb.push("1-On");
					break;
				default:
					sb.push(valBuff[k]);
					break;
				}
				break;
			case 29:
				sb.push("SelfTimer2 = ");
				isInt16 = true;
				break;
			case 33:
				sb.push("FlashOutput = ");
				isInt16 = true;
				break;
			default:
				sb.push("Unknown(");
				sb.push(k);
				sb.push(") = ");
				isInt16 = true;
				break;
			}
			if (isInt16)
			{
				sb.push(valBuff[k]);
			}
			else if (isUInt16)
			{
				sb.push(valBuff[k]);
			}
			k++;
		}
		return sb.join("");
	}

	toStringCanonLensType(lensType)
	{
		return "0x" + text.toHex16(lensType);
	}

	parseMakerNote(buff)
	{
		let reader = new data.ByteReader(buff);
		let type = reader.readUTF8Z(0, 10);
		if (type == "Panasonic")
		{
			return EXIFData.parseIFD(reader, 12, true, null, null, EXIFMaker.Panasonic);
		}
		else if (type == "OLYMPUS")
		{
			if (reader.readUTF8(8, 2) == "II")
			{
				return EXIFData.parseIFD(reader, 12, true, null, 8, EXIFMaker.Olympus);
			}
		}
		else if (type == "OLYMP")
		{
			return EXIFData.parseIFD(reader, 8, true, null, 8, EXIFMaker.Olympus);
		}
		else if (type == "Nikon")
		{
			if (reader.readUInt8(6) == 2)
			{
				if (reader.readUTF8(10, 2) == "II")
				{
					return EXIFData.parseIFD(reader, 18, true, null, 10, EXIFMaker.Nikon3);
				}
			}
		}
		else if (type == "QVC")
		{
			return EXIFData.parseIFD(reader, 6, false, null, 6, EXIFMaker.Casio2);
		}
		else if (type == "SANYO")
		{
			return EXIFData.parseIFD(reader, 8, true, null, 8, EXIFMaker.Sanyo);
		}
		else if (type == "Apple iOS")
		{
			if (reader.readUTF8(12, 2) == "MM")
			{
				return EXIFData.parseIFD(reader, 14, false, null, 14, EXIFMaker.Apple);
			}
			else
			{
				return null;
			}
		}
		else
		{
			let maker = this.getPhotoMake();
			if (maker)
			{
				if (maker == "Canon")
				{
					return EXIFData.parseIFD(reader, 0, true, null, null, EXIFMaker.Canon);
				}
				else if (maker == "CASIO")
				{
					return EXIFData.parseIFD(reader, 0, false, null, null, EXIFMaker.Casio1);
				}
				else if (maker == "FLIR Systems AB")
				{
					return EXIFData.parseIFD(reader, 0, true, null, null, EXIFMaker.FLIR);
				}
			}
		}
		return null;
	}

	/**
	 * @param {string} exifMaker
	 * @param {string | number} id
	 * @param {number | undefined} [parentId]
	 */
	static getEXIFName(exifMaker, id, parentId)
	{
		let name = null;
		if (parentId === undefined)
		{
			switch (exifMaker)
			{
			case EXIFMaker.Panasonic:
				name = EXIFNamesPanasonic[id];
				break;
			case EXIFMaker.Canon:
				name = EXIFNamesCanon[id];
				break;
			case EXIFMaker.Olympus:
				name = EXIFNamesOlympus[id];
				break;
			case EXIFMaker.Casio1:
				name = EXIFNamesCasio1[id];
				break;
			case EXIFMaker.Casio2:
				name = EXIFNamesCasio2[id];
				break;
			case EXIFMaker.FLIR:
				name = EXIFNamesFLIR[id];
				break;
			case EXIFMaker.Nikon3:
				name = EXIFNamesNikon3[id];
				break;
			case EXIFMaker.Sanyo:
				name = EXIFNamesSanyo1[id];
				break;
			case EXIFMaker.Apple:
				name = EXIFNamesApple[id];
				break;
			case EXIFMaker.Standard:
			default:
				name = EXIFNamesStandard[id];
				break;
			}
		}
		else if (parentId == 34665)
		{
			name = EXIFNamesInfo[id];
		}
		else if (parentId == 34853)
		{
			name = EXIFNamesGPS[id];
		}
		else if (exifMaker == EXIFMaker.Olympus)
		{
			if (parentId == 0x2010)
			{
				name = EXIFNamesOlympus2010[id];
			}
			else if (parentId == 0x2020)
			{
				name = EXIFNamesOlympus2020[id];
			}
			else if (parentId == 0x2030)
			{
				name = EXIFNamesOlympus2030[id];
			}
			else if (parentId == 0x2040)
			{
				name = EXIFNamesOlympus2040[id];
			}
			else if (parentId == 0x2050)
			{
				name = EXIFNamesOlympus2050[id];
			}
		}
		return name || ("Tag "+id);
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} ofst
	 * @param {boolean} lsb
	 * @param {object | null} nextOfst
	 * @param {number | null} readBase
	 * @param {string | null | undefined} [maker]
	 */
	static parseIFD(reader, ofst, lsb, nextOfst, readBase, maker)
	{
		if (!(reader instanceof data.ByteReader))
			return null;

		if (maker == null)
			maker = EXIFMaker.Standard;
		let exif;
		let ifdCnt = reader.readUInt16(ofst, lsb);
		let readSize = ifdCnt * 12 + 4;
		if (ofst + 2 + readSize > reader.getLength())
			return null;
		exif = new EXIFData(maker);
		let i = 0;
		let ifdOfst = ofst + 2;
		let tag;
		let ftype;
		let fcnt;

		if (readBase == null)
		{
			readBase = 0x7fffffff;
			ifdOfst = ofst + 2;
			i = 0;
			while (i < ifdCnt)
			{
				tag = reader.readUInt16(ifdOfst, lsb);
				ftype = reader.readUInt16(ifdOfst + 2, lsb);
				fcnt = reader.readUInt32(ifdOfst + 4, lsb);

				if (ftype == 1)
				{
					if (fcnt <= 4)
					{
					}
					else
					{
						if (readBase > reader.readUInt32(ifdOfst + 8, lsb))
						{
							readBase = reader.readUInt32(ifdOfst + 8, lsb);
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
						if (readBase > reader.readUInt32(ifdOfst + 8, lsb))
						{
							readBase = reader.readUInt32(ifdOfst + 8, lsb);
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
						if (readBase > reader.readUInt32(ifdOfst + 8, lsb))
						{
							readBase = reader.readUInt32(ifdOfst + 8, lsb);
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
						if (readBase > reader.readUInt32(ifdOfst + 8, lsb))
						{
							readBase = reader.readUInt32(ifdOfst + 8, lsb);
						}
					}
				}
				else if (ftype == 5)
				{
					if (readBase > reader.readUInt32(ifdOfst + 8, lsb))
					{
						readBase = reader.readUInt32(ifdOfst + 8, lsb);
					}
				}
				else if (ftype == 7)
				{
					if (fcnt <= 4)
					{
					}
					else
					{
						if (readBase > reader.readUInt32(ifdOfst + 8, lsb))
						{
							readBase = reader.readUInt32(ifdOfst + 8, lsb);
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
						if (readBase > reader.readUInt32(ifdOfst + 8, lsb))
						{
							readBase = reader.readUInt32(ifdOfst + 8, lsb);
						}
					}
				}
				else if (ftype == 12)
				{
					if (readBase > reader.readUInt32(ifdOfst + 8, lsb))
					{
						readBase = reader.readUInt32(ifdOfst + 8, lsb);
					}
				}
				else if (ftype == 13)
				{
					if (readBase > reader.readUInt32(ifdOfst + 8, lsb))
					{
						readBase = reader.readUInt32(ifdOfst + 8, lsb);
					}
				}
				else
				{
					console.log("Unknown type "+ ftype);
				}

				ifdOfst += 12;
				i++;
			}
			readBase = ofst + ifdCnt * 12 + 2 + 4 - readBase;
		}

		ifdOfst = ofst + 2;
		i = 0;
		while (i < ifdCnt)
		{
			tag = reader.readUInt16(ifdOfst, lsb);
			ftype = reader.readUInt16(ifdOfst + 2, lsb);
			fcnt = reader.readUInt32(ifdOfst + 4, lsb);
	
			switch (ftype)
			{
			case 1:
				if (fcnt <= 4)
				{
					exif.addBytes(tag, reader.readUInt8Arr(ifdOfst + 8, fcnt));
				}
				else
				{
					exif.addBytes(tag, reader.readUInt8Arr(reader.readUInt32(ifdOfst + 8, lsb) + readBase, fcnt));
				}
				break;
			case 2:
				if (fcnt <= 4)
				{
					exif.addString(tag, reader.readUTF8Z(ifdOfst + 8, fcnt));
				}
				else
				{
					exif.addString(tag, reader.readUTF8Z(reader.readUInt32(ifdOfst + 8, lsb) + readBase, fcnt));
				}
				break;
			case 3:
			{
				if (fcnt <= 2)
				{
					exif.addUInt16(tag, reader.readUInt16Arr(ifdOfst + 8, lsb, fcnt));
				}
				else
				{
					try
					{
						exif.addUInt16(tag, reader.readUInt16Arr(reader.readUInt32(ifdOfst + 8, lsb) + readBase, lsb, fcnt));
					}
					catch (e)
					{
						console.log("Error in parsing file", e, ifdOfst, readBase, lsb, fcnt, tag);
						console.log(reader, reader.getLength(), reader.readUInt32(ifdOfst + 8, lsb));
					}
				}
				break;
			}
			case 4:
			{
				if (fcnt == 1)
				{
					let tmp = reader.readUInt32(ifdOfst + 8, lsb);
					if (tag == 34665 || tag == 34853)
					{
						let subexif = EXIFData.parseIFD(reader, tmp + readBase, lsb, null, readBase);
						if (subexif)
						{
							exif.addSubEXIF(tag, subexif);
						}
						else
						{
							console.log("Error in parsing subExif of tag "+tag);
						}
					}
					else
					{
						exif.addUInt32(tag, [tmp]);
					}
				}
				else
				{
					exif.addUInt32(tag, reader.readUInt32Arr(reader.readUInt32(ifdOfst + 8, lsb) + readBase, lsb, fcnt));
				}
				break;
			}
			case 5:
				exif.addRational(tag, reader.readUInt32Arr(reader.readUInt32(ifdOfst + 8, lsb) + readBase, lsb, fcnt << 1));
				break;
			case 7:
				if (fcnt <= 4)
				{
					exif.addOther(tag, reader.getArrayBuffer(ifdOfst + 8, fcnt));
				}
				else
				{
					exif.addOther(tag, reader.getArrayBuffer(reader.readUInt32(ifdOfst + 8, lsb) + readBase, fcnt));
				}
				break;
			case 8:
				if (fcnt <= 2)
				{
					exif.addInt16(tag, reader.readInt16Arr(ifdOfst + 8, lsb, fcnt));
				}
				else
				{
					exif.addInt16(tag, reader.readInt16Arr(reader.readUInt32(ifdOfst + 8, lsb) + readBase, lsb, fcnt));
				}
				break;
			case 9:
				if (fcnt == 1)
				{
					exif.addInt32(tag, reader.readInt32Arr(ifdOfst + 8, lsb, fcnt));
				}
				else
				{
					exif.addInt32(tag, reader.readInt32Arr(reader.readInt32(ifdOfst + 8, lsb) + readBase, lsb, fcnt));
				}
				break;
			case 10:
				exif.addSRational(tag, reader.readInt32Arr(reader.readUInt32(ifdOfst + 8, lsb) + readBase, lsb, fcnt << 1));
				break;
			case 12:
				exif.addDouble(tag, reader.readFloat64Arr(reader.readUInt32(ifdOfst + 8, lsb) + readBase, lsb, fcnt));
				break;
			default:
				console.log("EXIFData.parseIFD: Unsupported field type: "+ftype+", tag = "+tag);
				break;
			}
	
			ifdOfst += 12;
			i++;
		}
	
		if (nextOfst)
		{
			nextOfst.nextOfst = reader.readUInt32(ifdOfst + ifdCnt * 12, lsb);
		}
		return exif;
	}
}

export class StaticImage extends data.ParsedObject
{
	/**
	 * @param {HTMLImageElement} img
	 * @param {string} sourceName
	 * @param {string} objType MIME
	 */
	constructor(img, sourceName, objType)
	{
		super(sourceName, objType);
		this.img = img;
	}

	setExif(exif)
	{
		this.exif = exif;
	}

	getWidth()
	{
		return this.img.naturalWidth || this.img.width;
	}

	getHeight()
	{
		return this.img.naturalHeight || this.img.height;
	}

	getProperties()
	{
		let ret = {width: this.getWidth(), height: this.getHeight()};
		if (this.exif)
			ret.exif = this.exif.getProperties();
		return ret;
	}

	createCanvas()
	{
		let canvas = document.createElement("canvas");
		canvas.width = this.img.naturalWidth;
		canvas.height = this.img.naturalHeight;
		let ctx = canvas.getContext("2d");
		if (ctx)
		{
			ctx.drawImage(this.img, 0, 0, this.img.naturalWidth, this.img.naturalHeight);
			return canvas;
		}
		throw new Error("Error in creating canvas");
	}

	exportJPG(quality)
	{
		let canvas = this.createCanvas();
		return new Promise(function (resolve, reject) {
			canvas.toBlob((blob)=>{resolve(blob);}, "image/jpeg", quality);
		});
	}
	exportWEBP(quality)
	{
		let canvas = this.createCanvas();
		return new Promise(function (resolve, reject) {
			canvas.toBlob((blob)=>{resolve(blob);}, "image/webp", quality);
		});
	}

	exportPNG()
	{
		let canvas = this.createCanvas();
		return new Promise(function (resolve, reject) {
			canvas.toBlob((blob)=>{resolve(blob);}, "image/png");
		});
	}
}
