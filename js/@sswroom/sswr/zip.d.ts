import * as data from "./data";
import * as hash from "./hash";

export enum ZIPOS
{
	MSDOS,
	Amiga,
	OpenVMS,
	UNIX,
	VM_CMS,
	Atari_ST,
	OS2_HPFS,
	Macintosh,
	ZSystem,
	CP_M,
	NTFS,
	MVS,
	VSE,
	AcornRisc,
	VFAT,
	AlternateMVS,
	BeOS,
	Tandem,
	OS400,
	OSX
};

declare class FileInfo
{
	fileName: string;
	fileOfst: number;
	fileModTime: data.Timestamp;
	fileCreateTime: data.Timestamp;
	fileAccessTime: data.Timestamp;
	crcVal: number;
	uncompSize: number;
	compSize: number;
	compMeth: number;
	fileAttr: number;
}

export class ZIPBuilder
{
	crc: hash.CRC32RIEEE;
	builder: data.ByteBuilder;
	baseOfst: number;
	currOfst: number;
	files: FileInfo[];
	osType: ZIPOS;
	
	constructor(os: ZIPOS);

	addFile(fileName: string, fileContent: Uint8Array, lastModTime: data.Timestamp, lastAccessTime: data.Timestamp, createTime: data.Timestamp, unixAttr: number): boolean;
	addDir(dirName: string, lastModTime: data.Timestamp, lastAccessTime: data.Timestamp, createTime: data.Timestamp, unixAttr: number): boolean;
	addDeflate(fileName: string, buff: Uint8Array, decSize: number, crcVal: number, lastModTime: data.Timestamp, lastAccessTime: data.Timestamp, createTime: data.Timestamp, unixAttr: number): boolean;
	finalize(): Uint8Array;
};