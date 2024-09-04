import * as data from "./data.js";
import * as hash from "./hash.js"
import pako from "../../pako/dist/pako.esm.mjs";

const VERBOSE = true;
const ZIPVER = 63;


export const ZIPOS = {
	MSDOS: 0,
	Amiga: 1,
	OpenVMS: 2,
	UNIX: 3,
	VM_CMS: 4,
	Atari_ST: 5,
	OS2_HPFS: 6,
	Macintosh: 7,
	ZSystem: 8,
	CP_M: 9,
	NTFS: 10,
	MVS: 11,
	VSE: 12,
	AcornRisc: 13,
	VFAT: 14,
	AlternateMVS: 15,
	BeOS: 16,
	Tandem: 17,
	OS400: 18,
	OSX: 19
};

class FileInfo
{
	/**
	 * @param {string} fileName
	 * @param {data.Timestamp} fileModTime
	 */
	constructor(fileName, fileModTime)
	{
		this.fileName = fileName;
		this.fileOfst = 0;
		this.fileModTime = fileModTime;
		/** @type {data.Timestamp|null} */
		this.fileCreateTime = null;
		/** @type {data.Timestamp|null} */
		this.fileAccessTime = null;
		this.crcVal = 0;
		this.uncompSize = 0;
		this.compSize = 0;
		this.compMeth = 0;
		this.fileAttr = 0;
	}
}

export class ZIPBuilder
{
	/**
	 * @param {number} os
	 */
	constructor(os)
	{
		this.builder = new data.ByteBuilder();
		this.currOfst = 0;
		this.osType = os;
		/** @type {FileInfo[]} */
		this.files = [];
		this.finalized = false;
		this.crc = new hash.CRC32R(null);
	}

	/**
	 * @param {string} fileName
	 * @param {Uint8Array} fileContent
	 * @param {data.Timestamp} lastModTime
	 * @param {data.Timestamp} lastAccessTime
	 * @param {data.Timestamp} createTime
	 * @param {number} unixAttr
	 */
	addFile(fileName, fileContent, lastModTime, lastAccessTime, createTime, unixAttr)
	{
		if (this.finalized)
			return false;
		let hdrLen;
		let compSize;
		let compData = pako.deflateRaw(fileContent);
		let compress;
		if (compData == null || compData.length >= fileContent.length)
		{
			compSize = fileContent.length;
			compress = false;

		}
		else
		{
			compSize = compData.length;
			compress = true;
		}
		let crcVal = this.crc.calcDirect(fileContent.buffer);
		//Data::DateTime dt(lastModTime.inst, lastModTime.tzQhr);
		let fileNameLen = this.builder.writeUTF8(this.currOfst + 30, fileName);
		this.builder.writeInt32(this.currOfst + 0, 0x04034b50, true);
		this.builder.writeInt8(this.currOfst + 4, 20); //Verison (2.0)
		this.builder.writeInt8(this.currOfst + 5, this.osType);
		this.builder.writeInt16(this.currOfst + 6, 0, true);
		this.builder.writeInt16(this.currOfst + 8, 0x8, true);
		this.builder.writeInt16(this.currOfst + 10, lastModTime.toMSDOSTime(), true);
		this.builder.writeInt16(this.currOfst + 12, lastModTime.toMSDOSDate(), true);
		this.builder.writeInt32(this.currOfst + 14, crcVal, true);
		this.builder.writeInt32(this.currOfst + 18, compSize, true);
		this.builder.writeInt32(this.currOfst + 22, fileContent.length, true);
		this.builder.writeInt16(this.currOfst + 26, fileNameLen, true);
		this.builder.writeInt16(this.currOfst + 28, 0, true);
		hdrLen = 30 + fileNameLen;
		if (compSize >= 0xFFFFFFFF || fileContent.length >= 0xFFFFFFFF)
		{
			let len = 4;
			this.builder.writeInt16(this.currOfst + hdrLen, 1, true);
			if (fileContent.length >= 0xffffffff)
			{
				this.builder.writeInt64(this.currOfst + hdrLen + len, BigInt(fileContent.length), true);
				this.builder.writeInt32(this.currOfst + 22, 0xffffffff, true);
				len += 8;
			}
			if (compSize >= 0xffffffff)
			{
				if (compSize >= fileContent.length)
				{
					this.builder.writeInt64(this.currOfst + hdrLen + len, BigInt(fileContent.length), true);
				}
				else
				{
					this.builder.writeInt64(this.currOfst + hdrLen + len, BigInt(compSize), true);
				}
				this.builder.writeInt32(this.currOfst + 18, 0xffffffff, true);
				len += 8;
			}
			this.builder.writeInt16(this.currOfst + 28, len, true);
			this.builder.writeInt16(this.currOfst + hdrLen + 2, len - 4, true);
			hdrLen += len;
		}
	
		let file = new FileInfo(fileName, lastModTime);
		file.fileCreateTime = createTime;
		file.fileAccessTime = lastAccessTime;
		file.crcVal = crcVal;
		file.uncompSize = fileContent.length;
		file.compMeth = 8;
		file.compSize = compSize;
		if (this.osType == ZIPOS.UNIX)
		{
			file.fileAttr = unixAttr << 16;
		}
		else if (unixAttr == 0)
		{
			file.fileAttr = 0;
		}
		else
		{
			if (unixAttr & 0x200)
			{
				file.fileAttr = 0;
			}
			else
			{
				file.fileAttr = 1;
			}
		}
		file.fileOfst = this.currOfst;
		this.files.push(file);
		if (compress == false || compData == null || !(compData instanceof Uint8Array))
		{
			this.builder.writeInt16(this.currOfst + 8, 0x0, true);
			this.builder.writeInt32(this.currOfst + 18, fileContent.length, true);
			file.compMeth = 0;
			file.compSize = fileContent.length;
			this.currOfst += hdrLen;
			this.builder.writeUInt8Array(this.currOfst, fileContent);
			this.currOfst += fileContent.length;
		}
		else
		{
			this.currOfst += hdrLen;
			this.builder.writeUInt8Array(this.currOfst, compData);
			this.currOfst += compData.length;
		}
		return true;
	}

	/**
	 * @param {string} dirName
	 * @param {data.Timestamp} lastModTime
	 * @param {data.Timestamp} lastAccessTime
	 * @param {data.Timestamp} createTime
	 * @param {number} unixAttr
	 */
	addDir(dirName, lastModTime, lastAccessTime, createTime, unixAttr)
	{
		if (this.finalized)
			return false;
		if (!dirName.endsWith('/'))
			return false;
	
		let hdrLen;
		let dirNameLen = this.builder.writeUTF8(this.currOfst + 30, dirName);
		this.builder.writeInt32(this.currOfst + 0, 0x04034b50, true);
		this.builder.writeInt8(this.currOfst + 4, 45);
		this.builder.writeInt8(this.currOfst + 5, this.osType);
		this.builder.writeInt16(this.currOfst + 6, 0, true);
		this.builder.writeInt16(this.currOfst + 8, 0, true);
		this.builder.writeInt16(this.currOfst + 10, lastModTime.toMSDOSTime(), true);
		this.builder.writeInt16(this.currOfst + 12, lastModTime.toMSDOSDate(), true);
		this.builder.writeInt32(this.currOfst + 14, 0, true);
		this.builder.writeInt32(this.currOfst + 18, 0, true);
		this.builder.writeInt32(this.currOfst + 22, 0, true);
		this.builder.writeInt16(this.currOfst + 26, dirNameLen, true);
		this.builder.writeInt16(this.currOfst + 28, 0, true);
		hdrLen = 30 + dirNameLen;
	
		let file = new FileInfo(dirName, lastModTime);
		file.fileCreateTime = createTime;
		file.fileAccessTime = lastAccessTime;
		file.crcVal = 0;
		file.uncompSize = 0;
		file.compMeth = 0;
		file.compSize = 0;
		if (this.osType == ZIPOS.UNIX)
		{
			file.fileAttr = (unixAttr << 16) | 0x10;
		}
		else if (unixAttr == 0)
		{
			file.fileAttr = 0x10;
		}
		else
		{
			if (unixAttr & 0x200)
			{
				file.fileAttr = 0x10;
			}
			else
			{
				file.fileAttr = 0x11;
			}
		}
		file.fileOfst = this.currOfst;
		this.files.push(file);
		this.currOfst += hdrLen;
		return true;
	}

	/**
	 * @param {string} fileName
	 * @param {Uint8Array} buff
	 * @param {number} decSize
	 * @param {number} crcVal
	 * @param {data.Timestamp} lastModTime
	 * @param {data.Timestamp} lastAccessTime
	 * @param {data.Timestamp} createTime
	 * @param {number} unixAttr
	 */
	addDeflate(fileName, buff, decSize, crcVal, lastModTime, lastAccessTime, createTime, unixAttr)
	{
		if (this.finalized)
			return false;
		let hdrLen;
		//Data::DateTime dt(lastModTime.inst, lastModTime.tzQhr);
		let fileNameLen = this.builder.writeUTF8(this.currOfst + 30, fileName);
		this.builder.writeInt32(this.currOfst + 0, 0x04034b50, true);
		this.builder.writeInt8(this.currOfst + 4, 20); //Verison (2.0)
		this.builder.writeInt8(this.currOfst + 5, this.osType);
		this.builder.writeInt16(this.currOfst + 6, 0, true);
		this.builder.writeInt16(this.currOfst + 8, 0x8, true);
		this.builder.writeInt16(this.currOfst + 10, lastModTime.toMSDOSTime(), true);
		this.builder.writeInt16(this.currOfst + 12, lastModTime.toMSDOSDate(), true);
		this.builder.writeInt32(this.currOfst + 14, crcVal, true);
		this.builder.writeInt32(this.currOfst + 18, buff.length, true);
		this.builder.writeInt32(this.currOfst + 22, decSize, true);
		this.builder.writeInt16(this.currOfst + 26, fileNameLen, true);
		this.builder.writeInt16(this.currOfst + 28, 0, true);
		hdrLen = 30 + fileNameLen;
		if (buff.length >= 0xFFFFFFFF || decSize >= 0xFFFFFFFF)
		{
			let len = 4;
			this.builder.writeInt16(this.currOfst + hdrLen, 1, true);
			if (decSize >= 0xffffffff)
			{
				this.builder.writeInt64(this.currOfst + hdrLen + len, BigInt(decSize), true);
				this.builder.writeInt32(this.currOfst + 22, 0xffffffff, true);
				len += 8;
			}
			if (buff.length >= 0xffffffff)
			{
				this.builder.writeInt64(this.currOfst + hdrLen + len, BigInt(buff.length), true);
				this.builder.writeInt32(this.currOfst + 18, 0xffffffff, true);
				len += 8;
			}
			this.builder.writeInt16(this.currOfst + 28, len, true);
			this.builder.writeInt16(this.currOfst + hdrLen + 2, len - 4, true);
			hdrLen += len;
		}
	
		let file = new FileInfo(fileName, lastModTime);
		file.fileCreateTime = createTime;
		file.fileAccessTime = lastAccessTime;
		file.crcVal = crcVal;
		file.uncompSize = decSize;
		file.compMeth = 8;
		file.compSize = buff.length;
		if (this.osType == ZIPOS.UNIX)
		{
			file.fileAttr = unixAttr << 16;
		}
		else if (unixAttr == 0)
		{
			file.fileAttr = 0;
		}
		else
		{
			if (unixAttr & 0x200)
			{
				file.fileAttr = 0;
			}
			else
			{
				file.fileAttr = 1;
			}
		}
		file.fileOfst = this.currOfst;
		this.files.push(file);
		this.currOfst += hdrLen;
		this.builder.writeUInt8Array(this.currOfst, buff);
		this.currOfst += buff.length;
		return true;
	}

	finalize()
	{
		if (!this.finalized)
		{
			this.finalized = true;

			let cdStart = this.currOfst;
			let file;
			let hdrLen;
			let cdLen = 0;
			let i = 0;
			let j = this.files.length;
			let minVer;
			while (i < j)
			{
				file = this.files[i];
				minVer = 20;
				let fileNameLen = this.builder.writeUTF8(this.currOfst + 46, file.fileName);
				this.builder.writeInt32(this.currOfst + 0, 0x02014b50, true);
				this.builder.writeInt8(this.currOfst + 4, ZIPVER);
				this.builder.writeInt8(this.currOfst + 5, this.osType);
				this.builder.writeInt8(this.currOfst + 7, this.osType);
				this.builder.writeInt16(this.currOfst + 8, 0, true); //General purpose flag
				this.builder.writeInt16(this.currOfst + 10, file.compMeth, true);
				this.builder.writeInt16(this.currOfst + 12, file.fileModTime.toMSDOSTime(), true);
				this.builder.writeInt16(this.currOfst + 14, file.fileModTime.toMSDOSDate(), true);
				this.builder.writeInt32(this.currOfst + 16, file.crcVal, true);
				this.builder.writeInt32(this.currOfst + 20, file.compSize, true);
				this.builder.writeInt32(this.currOfst + 24, file.uncompSize, true);
				this.builder.writeInt16(this.currOfst + 28, fileNameLen, true);
				this.builder.writeInt16(this.currOfst + 30, 0, true); //extra field length
				this.builder.writeInt16(this.currOfst + 32, 0, true); //File comment length
				this.builder.writeInt16(this.currOfst + 34, 0, true); //Disk number where file starts
				this.builder.writeInt16(this.currOfst + 36, 0, true); //Internal file attributes
				this.builder.writeInt32(this.currOfst + 38, file.fileAttr, true); //External file attributes
				this.builder.writeInt32(this.currOfst + 42, file.fileOfst, true);
				hdrLen = 46 + fileNameLen;
				if (file.compSize >= 0xFFFFFFFF || file.fileOfst >= 0xFFFFFFFF || file.uncompSize >= 0xFFFFFFFF)
				{
					let len = 0;
					this.builder.writeInt16(this.currOfst + hdrLen, 1, true);
					if (file.uncompSize >= 0xFFFFFFFFn)
					{
						this.builder.writeInt64(this.currOfst + hdrLen + 4 + len, BigInt(file.uncompSize), true);
						this.builder.writeInt32(this.currOfst + 24, 0xffffffff, true);
						len += 8;
					}
					if (file.compSize >= 0xFFFFFFFFn)
					{
						this.builder.writeInt64(this.currOfst + hdrLen + 4 + len, BigInt(file.compSize), true);
						this.builder.writeInt32(this.currOfst + 20, 0xffffffff, true);
						len += 8;
					}
					if (file.fileOfst >= 0xFFFFFFFFn)
					{
						this.builder.writeInt64(this.currOfst + hdrLen + 4 + len, BigInt(file.fileOfst), true);
						this.builder.writeInt32(this.currOfst + 42, 0xffffffff, true);
						len += 8;
					}
					this.builder.writeInt16(this.currOfst + hdrLen + 2, len, true);
					hdrLen += 4 + len;
				}
				if (file.fileModTime != null && file.fileAccessTime != null && file.fileCreateTime != null)
				{
					if (minVer < 45)
						minVer = 45;
					this.builder.writeInt16(this.currOfst + hdrLen, 10, true);
					this.builder.writeInt16(this.currOfst + hdrLen + 2, 32, true);
					this.builder.writeInt32(this.currOfst + hdrLen + 4, 0, true);
					this.builder.writeInt16(this.currOfst + hdrLen + 8, 1, true);
					this.builder.writeInt16(this.currOfst + hdrLen + 10, 24, true);
					this.builder.writeInt64(this.currOfst + hdrLen + 12, file.fileModTime.toFILETIME(), true);
					this.builder.writeInt64(this.currOfst + hdrLen + 20, file.fileAccessTime.toFILETIME(), true);
					this.builder.writeInt64(this.currOfst + hdrLen + 28, file.fileCreateTime.toFILETIME(), true);
					hdrLen += 36;
				}
				this.builder.writeInt16(this.currOfst + 30, hdrLen - 46 - fileNameLen, true);
				this.builder.writeInt8(this.currOfst + 6, minVer);
		
				this.currOfst += hdrLen;
				cdLen += hdrLen;
		
				i++;
			}
			if (this.currOfst >= 0xffffffff || j >= 0xffff)
			{
				let cdOfst = this.currOfst;
				this.builder.writeInt32(this.currOfst + 0, 0x06064b50, true); //Record Type (Zip64 End of central directory record)
				this.builder.writeInt64(this.currOfst + 4, 44n, true); //Size of zip64 end of central directory record
				this.builder.writeInt16(this.currOfst + 12, ZIPVER, true); //Version made by
				this.builder.writeInt16(this.currOfst + 14, 45, true); //Version needed to extract
				this.builder.writeInt32(this.currOfst + 16, 0, true); //Number of this disk
				this.builder.writeInt32(this.currOfst + 20, 0, true); //Number of the disk with the start of the central directory
				this.builder.writeInt64(this.currOfst + 24, BigInt(j), true); //Total number of entries in the central directory on this disk
				this.builder.writeInt64(this.currOfst + 32, BigInt(j), true); //Total number of entries in the central directory
				this.builder.writeInt64(this.currOfst + 40, BigInt(cdLen), true); //Size of central directory
				this.builder.writeInt64(this.currOfst + 48, BigInt(cdStart), true); //Offset of start of central directory with respect to the starting disk number
				this.currOfst += 56;
		
				this.builder.writeInt32(this.currOfst + 0, 0x07064b50, true); //Record Type (Zip64 end of central directory locator)
				this.builder.writeInt32(this.currOfst + 4, 0, true); //Number of the disk with the start of the zip64 end of central directory
				this.builder.writeInt64(this.currOfst + 8, BigInt(cdOfst), true); //Relative offset of the zip64 end of central directory record
				this.builder.writeInt32(this.currOfst + 16, 1, true); //Total number of disks
				this.currOfst += 20;

				this.builder.writeInt32(this.currOfst + 0, 0x06054b50, true); //Record Type (End of central directory record)
				this.builder.writeInt16(this.currOfst + 4, 0, true); //Number of this disk
				this.builder.writeInt16(this.currOfst + 6, 0, true); //Disk where central directory starts
				if (j >= 0xffff)
				{
					this.builder.writeInt16(this.currOfst + 8, 0xffff, true); //Number of central directory of this disk
					this.builder.writeInt16(this.currOfst + 10, 0xffff, true); //Total number of central directory records
				}
				else
				{
					this.builder.writeInt16(this.currOfst + 8, j, true); //Number of central directory of this disk
					this.builder.writeInt16(this.currOfst + 10, j, true); //Total number of central directory records
				}
				this.builder.writeInt32(this.currOfst + 12, cdLen, true); //Size of central directory
				this.builder.writeInt32(this.currOfst + 16, 0xffffffff, true); //Offset of start of central directory
				this.builder.writeInt16(this.currOfst + 20, 0, true); //Comment Length
				this.currOfst += 22;
			}
			else
			{
				this.builder.writeInt32(this.currOfst + 0, 0x06054b50, true); //Record Type (End of central directory record)
				this.builder.writeInt16(this.currOfst + 4, 0, true); //Number of this disk
				this.builder.writeInt16(this.currOfst + 6, 0, true); //Disk where central directory starts
				this.builder.writeInt16(this.currOfst + 8, j, true); //Number of central directory of this disk
				this.builder.writeInt16(this.currOfst + 10, j, true); //Total number of central directory records
				this.builder.writeInt32(this.currOfst + 12, cdLen, true); //Size of central directory
				this.builder.writeInt32(this.currOfst + 16, cdStart, true); //Offset of start of central directory
				this.builder.writeInt16(this.currOfst + 20, 0, true); //Comment Length
				this.currOfst += 22;
			}
		}
		return this.builder.build();
	}
}
