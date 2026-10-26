#include "Stdafx.h"
#include "Core/ByteTool_C.h"
#include "IO/FileAnalyse/OSGBFileAnalyse.h"

namespace
{
	class OSGBSceneFieldReader
	{
	private:
		NN<IO::StreamData> fd;
		NN<IO::FileAnalyse::FrameDetail> frame;
		UInt64 frameStart;
		UInt64 position;
		UInt64 end;

		Bool ReadBytes(UIntOS size, UInt8 *buff)
		{
			if (this->position > this->end || size > this->end - this->position ||
				this->fd->GetRealData(this->position, size, BYTEARR(buff)) != size)
			{
				return false;
			}
			this->position += size;
			return true;
		}

	public:
		OSGBSceneFieldReader(NN<IO::StreamData> fd, NN<IO::FileAnalyse::FrameDetail> frame, UInt64 frameStart, UInt64 dataStart, UInt64 end) :
			fd(fd), frame(frame), frameStart(frameStart), position(dataStart), end(end)
		{
		}

		UInt64 GetPosition() const
		{
			return this->position;
		}

		Bool SkipBytes(UIntOS size, Text::CStringNN name, Text::CStringNN value)
		{
			if (this->position > this->end || size > this->end - this->position)
				return false;
			this->frame->AddField((UIntOS)(this->position - this->frameStart), size, name, value);
			this->position += size;
			return true;
		}

		Bool ReadUInt32(Text::CStringNN name, UInt32 &value)
		{
			UInt8 buff[4];
			UInt64 fieldOfst = this->position;
			if (!this->ReadBytes(4, buff))
				return false;
			value = ReadLUInt32(buff);
			this->frame->AddUInt((UIntOS)(fieldOfst - this->frameStart), 4, name, value);
			return true;
		}

		Bool ReadEnum(Text::CStringNN name, const Text::CStringNN *valueNames, UIntOS valueNameCount, UInt32 &value, UInt32 firstValue = 0)
		{
			UInt64 fieldOfst = this->position;
			UInt8 buff[4];
			if (!this->ReadBytes(4, buff))
				return false;
			value = ReadLUInt32(buff);
			Text::CString valueName = value >= firstValue && (UInt64)value - firstValue < valueNameCount ? valueNames[value - firstValue] : CSTR("Unknown");
			this->frame->AddUIntName((UIntOS)(fieldOfst - this->frameStart), 4, name, value, valueName);
			return true;
		}

		Bool ReadBool(Text::CStringNN name, Bool &value)
		{
			UInt8 buff[1];
			UInt64 fieldOfst = this->position;
			if (!this->ReadBytes(1, buff))
				return false;
			value = buff[0] != 0;
			this->frame->AddBool((UIntOS)(fieldOfst - this->frameStart), name, buff[0]);
			return true;
		}

		Bool ReadString(Text::CStringNN name)
		{
			UInt32 size;
			UInt8 lengthBuff[4];
			UInt64 lengthOfst = this->position;
			if (!this->ReadBytes(4, lengthBuff))
				return false;
			size = ReadLUInt32(lengthBuff);
			this->frame->AddUInt((UIntOS)(lengthOfst - this->frameStart), 4, CSTR("String Length"), size);
			if (size > 4096 || this->position > this->end || size > this->end - this->position)
				return false;
			if (size > 0)
			{
				UTF8Char valueBuff[4096];
				UInt64 valueOfst = this->position;
				if (!this->ReadBytes(size, (UInt8*)valueBuff))
					return false;
				this->frame->AddStrC((UIntOS)(valueOfst - this->frameStart), size, name, valueBuff);
			}
			else
			{
				this->frame->AddField((UIntOS)(this->position - this->frameStart), 0, name, CSTR(""));
			}
			return true;
		}

		Bool ReadFloat(Text::CStringNN name, UIntOS size)
		{
			UInt8 buff[8];
			UInt64 fieldOfst = this->position;
			if (size != 4 && size != 8)
				return false;
			if (!this->ReadBytes(size, buff))
				return false;
			if (size == 4)
				this->frame->AddFloat((UIntOS)(fieldOfst - this->frameStart), size, name, ReadLFloat(buff));
			else
				this->frame->AddFloat((UIntOS)(fieldOfst - this->frameStart), size, name, ReadLDouble(buff));
			return true;
		}

		Bool ReadObjectReference(Text::CStringNN name)
		{
			Bool hasObject;
			if (!this->ReadBool(name, hasObject))
				return false;
			return !hasObject;
		}
	};

	static Bool ReadObjectFields(OSGBSceneFieldReader &reader)
	{
		Text::CStringNN dataVarianceNames[3] = {CSTR("STATIC"), CSTR("DYNAMIC"), CSTR("UNSPECIFIED")};
		UInt32 value;
		if (!reader.ReadString(CSTR("Name")) ||
			!reader.ReadEnum(CSTR("Data Variance"), dataVarianceNames, 3, value))
			return false;
		if (!reader.ReadObjectReference(CSTR("User Data Container")))
			return false;
		return true;
	}

	static Bool ReadNodeFields(OSGBSceneFieldReader &reader)
	{
		Bool present;
		if (!reader.ReadBool(CSTR("Initial Bound Present"), present))
			return false;
		if (present)
		{
			if (!reader.ReadFloat(CSTR("Initial Bound Center X"), 8) ||
				!reader.ReadFloat(CSTR("Initial Bound Center Y"), 8) ||
				!reader.ReadFloat(CSTR("Initial Bound Center Z"), 8) ||
				!reader.ReadFloat(CSTR("Initial Bound Radius"), 8))
				return false;
		}
		if (!reader.ReadObjectReference(CSTR("Compute Bound Callback")) ||
			!reader.ReadObjectReference(CSTR("Update Callback")) ||
			!reader.ReadObjectReference(CSTR("Event Callback")) ||
			!reader.ReadObjectReference(CSTR("Cull Callback")))
			return false;
		if (!reader.ReadBool(CSTR("Culling Active"), present))
			return false;
		UInt32 nodeMask;
		if (!reader.ReadUInt32(CSTR("Node Mask"), nodeMask))
			return false;
		if (!reader.ReadObjectReference(CSTR("State Set")))
			return false;
		return true;
	}

	static Bool ReadPagedLODFields(OSGBSceneFieldReader &reader)
	{
		Text::CStringNN centerModeNames[3] = {CSTR("USE_BOUNDING_SPHERE_CENTER"), CSTR("USER_DEFINED_CENTER"), CSTR("UNION_OF_BOUNDING_SPHERE_AND_USER_DEFINED")};
		Text::CStringNN rangeModeNames[2] = {CSTR("DISTANCE_FROM_EYE_POINT"), CSTR("PIXEL_SIZE_ON_SCREEN")};
		UInt32 value;
		Bool present;
		if (!reader.ReadEnum(CSTR("Center Mode"), centerModeNames, 3, value))
			return false;
		if (!reader.ReadBool(CSTR("User Center Present"), present))
			return false;
		if (present &&
			(!reader.ReadFloat(CSTR("User Center X"), 8) ||
			!reader.ReadFloat(CSTR("User Center Y"), 8) ||
			!reader.ReadFloat(CSTR("User Center Z"), 8) ||
			!reader.ReadFloat(CSTR("User Radius"), 8)))
			return false;
		if (!reader.ReadEnum(CSTR("Range Mode"), rangeModeNames, 2, value))
			return false;
		if (!reader.ReadBool(CSTR("Range List Present"), present))
			return false;
		if (present)
		{
			UInt32 count;
			if (!reader.ReadUInt32(CSTR("Range Count"), count) || count > 4096)
				return false;
			UInt32 i = 0;
			while (i < count)
			{
				if (!reader.ReadFloat(CSTR("Range Minimum"), 4) || !reader.ReadFloat(CSTR("Range Maximum"), 4))
					return false;
				i++;
			}
		}

		if (!reader.ReadBool(CSTR("Database Path Serializer Present"), present))
			return false;
		if (present)
		{
			if (!reader.ReadBool(CSTR("Database Path Present"), present))
				return false;
			if (present && !reader.ReadString(CSTR("Database Path")))
				return false;
		}
		if (!reader.ReadUInt32(CSTR("Children That Cannot Expire"), value) ||
			!reader.ReadBool(CSTR("External Paging Disabled"), present) ||
			!reader.ReadBool(CSTR("Range Data List Present"), present))
			return false;
		if (present)
		{
			UInt32 count;
			if (!reader.ReadUInt32(CSTR("File Name Count"), count) || count > 4096)
				return false;
			UInt32 i = 0;
			while (i < count)
			{
				if (!reader.ReadString(CSTR("File Name")))
					return false;
				i++;
			}
			if (!reader.ReadUInt32(CSTR("Priority Count"), count) || count > 4096)
				return false;
			i = 0;
			while (i < count)
			{
				if (!reader.ReadFloat(CSTR("Priority Offset"), 4) || !reader.ReadFloat(CSTR("Priority Scale"), 4))
					return false;
				i++;
			}
		}
		if (!reader.ReadBool(CSTR("Children List Present"), present))
			return false;
		if (present)
		{
			if (!reader.ReadUInt32(CSTR("Child Count"), value))
				return false;
		}
		return true;
	}

	static Bool ReadGeodeFields(OSGBSceneFieldReader &reader)
	{
		Bool present;
		UInt32 count;
		if (!reader.ReadBool(CSTR("Drawables List Present"), present))
			return false;
		if (!present)
			return true;
		if (!reader.ReadUInt32(CSTR("Drawable Count"), count))
			return false;
		return true;
	}

	static Bool ReadDrawableFields(OSGBSceneFieldReader &reader)
	{
		Bool present;
		if (!reader.ReadObjectReference(CSTR("State Set")))
			return false;
		if (!reader.ReadBool(CSTR("Initial Bound Present"), present))
			return false;
		if (present)
		{
			if (!reader.ReadFloat(CSTR("Initial Bound Minimum X"), 8) ||
				!reader.ReadFloat(CSTR("Initial Bound Minimum Y"), 8) ||
				!reader.ReadFloat(CSTR("Initial Bound Minimum Z"), 8) ||
				!reader.ReadFloat(CSTR("Initial Bound Maximum X"), 8) ||
				!reader.ReadFloat(CSTR("Initial Bound Maximum Y"), 8) ||
				!reader.ReadFloat(CSTR("Initial Bound Maximum Z"), 8))
				return false;
		}
		if (!reader.ReadObjectReference(CSTR("Compute Bounding Box Callback")) ||
			!reader.ReadObjectReference(CSTR("Shape")))
			return false;
		if (!reader.ReadBool(CSTR("Supports Display List"), present) ||
			!reader.ReadBool(CSTR("Use Display List"), present) ||
			!reader.ReadBool(CSTR("Use Vertex Buffer Objects"), present) ||
			!reader.ReadObjectReference(CSTR("Update Callback")) ||
			!reader.ReadObjectReference(CSTR("Event Callback")) ||
			!reader.ReadObjectReference(CSTR("Cull Callback")) ||
			!reader.ReadObjectReference(CSTR("Draw Callback")))
			return false;
		return true;
	}

	static Bool ReadGeometryFields(OSGBSceneFieldReader &reader)
	{
		UInt32 count;
		if (!reader.ReadUInt32(CSTR("Primitive Set Count"), count))
			return false;
		return true;
	}

	static Bool ReadStateSetFields(OSGBSceneFieldReader &reader)
	{
		Bool present;
		UInt32 count;
		if (!reader.ReadBool(CSTR("Mode List Present"), present))
			return false;
		if (present)
		{
			if (!reader.ReadUInt32(CSTR("Mode Count"), count) || count > 4096)
				return false;
			UInt32 i = 0;
			while (i < count)
			{
				UInt32 mode;
				UInt32 modeValue;
				if (!reader.ReadUInt32(CSTR("Mode GLenum"), mode) ||
					!reader.ReadUInt32(CSTR("Mode Value"), modeValue))
					return false;
				i++;
			}
		}
		if (!reader.ReadBool(CSTR("Attribute List Present"), present))
			return false;
		if (present)
		{
			if (!reader.ReadUInt32(CSTR("Attribute Count"), count))
				return false;
			if (count > 0)
				return true;
		}
		if (!reader.ReadBool(CSTR("Texture Mode List Present"), present))
			return false;
		if (present)
		{
			if (!reader.ReadUInt32(CSTR("Texture Unit Count"), count) || count > 4096)
				return false;
			UInt32 unit = 0;
			while (unit < count)
			{
				UInt32 modeCount;
				if (!reader.ReadUInt32(CSTR("Texture Mode Count"), modeCount) || modeCount > 4096)
					return false;
				UInt32 i = 0;
				while (i < modeCount)
				{
					UInt32 mode;
					UInt32 modeValue;
					if (!reader.ReadUInt32(CSTR("Texture Mode GLenum"), mode) ||
						!reader.ReadUInt32(CSTR("Texture Mode Value"), modeValue))
						return false;
					i++;
				}
				unit++;
			}
		}
		if (!reader.ReadBool(CSTR("Texture Attribute List Present"), present))
			return false;
		if (present)
		{
			if (!reader.ReadUInt32(CSTR("Texture Unit Count"), count) || count > 4096)
				return false;
			UInt32 unit = 0;
			while (unit < count)
			{
				UInt32 attributeCount;
				if (!reader.ReadUInt32(CSTR("Texture Attribute Count"), attributeCount))
					return false;
				if (attributeCount > 0)
					return true;
				unit++;
			}
		}
		if (!reader.ReadBool(CSTR("Uniform List Present"), present))
			return false;
		if (present)
		{
			if (!reader.ReadUInt32(CSTR("Uniform Count"), count))
				return false;
			if (count > 0)
				return true;
		}
		Text::CStringNN renderBinModeNames[5] = {CSTR("INHERIT_RENDERBIN_DETAILS"), CSTR("USE_RENDERBIN_DETAILS"), CSTR("OVERRIDE_RENDERBIN_DETAILS"), CSTR("PROTECTED_RENDERBIN_DETAILS"), CSTR("OVERRIDE_PROTECTED_RENDERBIN_DETAILS")};
		UInt32 value;
		if (!reader.ReadUInt32(CSTR("Rendering Hint"), value) ||
			!reader.ReadEnum(CSTR("Render Bin Mode"), renderBinModeNames, 5, value) ||
			!reader.ReadUInt32(CSTR("Render Bin Number"), value) ||
			!reader.ReadString(CSTR("Render Bin Name")) ||
			!reader.ReadBool(CSTR("Nest Render Bins"), present) ||
			!reader.ReadObjectReference(CSTR("Update Callback")) ||
			!reader.ReadObjectReference(CSTR("Event Callback")))
			return false;
		return true;
	}

	static Bool ReadMaterialValues(OSGBSceneFieldReader &reader, Text::CStringNN name, UIntOS componentCount)
	{
		Bool present;
		if (!reader.ReadBool(name, present))
			return false;
		if (!present)
			return true;
		if (!reader.ReadBool(CSTR("Front And Back"), present))
			return false;
		UIntOS i = 0;
		while (i < componentCount)
		{
			if (!reader.ReadFloat(CSTR("Front Value"), 4))
				return false;
			i++;
		}
		i = 0;
		while (i < componentCount)
		{
			if (!reader.ReadFloat(CSTR("Back Value"), 4))
				return false;
			i++;
		}
		return true;
	}

	static Bool ReadMaterialFields(OSGBSceneFieldReader &reader)
	{
		Text::CStringNN colorModeNames[6] = {CSTR("AMBIENT"), CSTR("DIFFUSE"), CSTR("SPECULAR"), CSTR("EMISSION"), CSTR("AMBIENT_AND_DIFFUSE"), CSTR("OFF")};
		UInt32 value;
		if (!reader.ReadObjectReference(CSTR("Update Callback")) ||
			!reader.ReadObjectReference(CSTR("Event Callback")) ||
			!reader.ReadEnum(CSTR("Color Mode"), colorModeNames, 6, value, 0x1600) ||
			!ReadMaterialValues(reader, CSTR("Ambient Present"), 4) ||
			!ReadMaterialValues(reader, CSTR("Diffuse Present"), 4) ||
			!ReadMaterialValues(reader, CSTR("Specular Present"), 4) ||
			!ReadMaterialValues(reader, CSTR("Emission Present"), 4) ||
			!ReadMaterialValues(reader, CSTR("Shininess Present"), 1))
			return false;
		return true;
	}

	static Bool ReadStateSetContinuation(OSGBSceneFieldReader &reader)
	{
		Bool present;
		UInt32 count;
		UInt32 value;
		if (!reader.ReadUInt32(CSTR("Containing StateSet: Material State Value"), value) ||
			!reader.ReadBool(CSTR("Containing StateSet: Texture Mode List Present"), present))
			return false;
		if (present)
		{
			if (!reader.ReadUInt32(CSTR("Containing StateSet: Texture Unit Count"), count) || count > 4096)
				return false;
			UInt32 unit = 0;
			while (unit < count)
			{
				UInt32 modeCount;
				if (!reader.ReadUInt32(CSTR("Containing StateSet: Mode Count"), modeCount) || modeCount > 4096)
					return false;
				UInt32 i = 0;
				while (i < modeCount)
				{
					if (!reader.ReadUInt32(CSTR("Containing StateSet: Mode GLenum"), value) ||
						!reader.ReadUInt32(CSTR("Containing StateSet: Mode Value"), value))
						return false;
					i++;
				}
				unit++;
			}
		}
		if (!reader.ReadBool(CSTR("Containing StateSet: Texture Attribute List Present"), present))
			return false;
		if (present)
		{
			if (!reader.ReadUInt32(CSTR("Containing StateSet: Texture Unit Count"), count) || count > 4096)
				return false;
			UInt32 unit = 0;
			while (unit < count)
			{
				UInt32 attributeCount;
				if (!reader.ReadUInt32(CSTR("Containing StateSet: Texture Attribute Count"), attributeCount))
					return false;
				if (attributeCount > 0)
					return true;
				unit++;
			}
		}
		if (!reader.ReadBool(CSTR("Containing StateSet: Uniform List Present"), present))
			return false;
		if (present && !reader.ReadUInt32(CSTR("Containing StateSet: Uniform Count"), count))
			return false;
		return true;
	}

	static Bool ReadTextureFields(OSGBSceneFieldReader &reader)
	{
		Bool present;
		UInt32 value;
		UIntOS i = 0;
		while (i < 5)
		{
			if (!reader.ReadBool(CSTR("Texture Mode Present"), present))
				return false;
			if (present && !reader.ReadUInt32(CSTR("Texture Mode"), value))
				return false;
			i++;
		}
		if (!reader.ReadFloat(CSTR("Maximum Anisotropy"), 4) ||
			!reader.ReadBool(CSTR("Hardware Mipmap Generation"), present) ||
			!reader.ReadBool(CSTR("Unreference Image Data After Apply"), present) ||
			!reader.ReadBool(CSTR("Client Storage Hint"), present) ||
			!reader.ReadBool(CSTR("Resize Non-Power-Of-Two Hint"), present))
			return false;
		i = 0;
		while (i < 4)
		{
			if (!reader.ReadFloat(CSTR("Border Color"), 8))
				return false;
			i++;
		}
		if (!reader.ReadUInt32(CSTR("Border Width"), value) ||
			!reader.ReadUInt32(CSTR("Internal Format Mode"), value) ||
			!reader.ReadBool(CSTR("Internal Format Present"), present))
			return false;
		if (present && !reader.ReadUInt32(CSTR("Internal Format"), value))
			return false;
		if (!reader.ReadBool(CSTR("Source Format Present"), present))
			return false;
		if (present && !reader.ReadUInt32(CSTR("Source Format"), value))
			return false;
		if (!reader.ReadBool(CSTR("Source Type Present"), present))
			return false;
		if (present && !reader.ReadUInt32(CSTR("Source Type"), value))
			return false;
		if (!reader.ReadBool(CSTR("Shadow Comparison"), present) ||
			!reader.ReadUInt32(CSTR("Shadow Compare Function"), value) ||
			!reader.ReadUInt32(CSTR("Shadow Texture Mode"), value) ||
			!reader.ReadFloat(CSTR("Shadow Ambient"), 4))
			return false;
		return true;
	}

	static Bool ReadTexture2DFields(OSGBSceneFieldReader &reader)
	{
		Text::CStringNN writeHintNames[3] = {CSTR("NO_PREFERENCE"), CSTR("STORE_INLINE"), CSTR("EXTERNAL_FILE")};
		Text::CStringNN imageDecisionNames[4] = {CSTR("INLINE_DATA"), CSTR("INLINE_FILE"), CSTR("EXTERNAL"), CSTR("WRITE_OUT")};
		Bool hasImage;
		UInt32 value;
		if (!reader.ReadObjectReference(CSTR("Update Callback")) ||
			!reader.ReadObjectReference(CSTR("Event Callback")) ||
			!ReadTextureFields(reader) ||
			!reader.ReadBool(CSTR("Image Present"), hasImage))
			return false;
		if (hasImage)
		{
			if (!reader.ReadUInt32(CSTR("Image Object ID"), value) ||
				!reader.ReadString(CSTR("Image File Name")) ||
				!reader.ReadEnum(CSTR("Image Write Hint"), writeHintNames, 3, value) ||
				!reader.ReadEnum(CSTR("Image Storage Decision"), imageDecisionNames, 4, value))
				return false;
			switch (value)
			{
			case 0:
			{
				UInt32 size;
				if (!reader.ReadUInt32(CSTR("Image Origin"), size) ||
					!reader.ReadUInt32(CSTR("Image Width"), size) ||
					!reader.ReadUInt32(CSTR("Image Height"), size) ||
					!reader.ReadUInt32(CSTR("Image Depth"), size) ||
					!reader.ReadUInt32(CSTR("Image Internal Format"), size) ||
					!reader.ReadUInt32(CSTR("Image Pixel Format"), size) ||
					!reader.ReadUInt32(CSTR("Image Data Type"), size) ||
					!reader.ReadUInt32(CSTR("Image Packing"), size) ||
					!reader.ReadUInt32(CSTR("Image Allocation Mode"), size) ||
					!reader.ReadUInt32(CSTR("Image Data Size"), size) ||
					!reader.SkipBytes(size, CSTR("Embedded Image Data"), CSTR("Inline pixel buffer")))
					return false;
				if (!reader.ReadUInt32(CSTR("Mipmap Level Count"), size) || size > 4096)
					return false;
				UInt32 i = 0;
				while (i < size)
				{
					if (!reader.ReadUInt32(CSTR("Mipmap Data Offset"), value))
						return false;
					i++;
				}
				break;
			}
			case 1:
			{
				UInt32 size;
				if (!reader.ReadUInt32(CSTR("Embedded Image Size"), size) ||
					!reader.SkipBytes(size, CSTR("Embedded Image Data"), CSTR("Encoded image file")))
					return false;
				break;
			}
			case 2:
			case 3:
				break;
			default:
				return false;
			}
			Text::CStringNN imageDataVarianceNames[3] = {CSTR("STATIC"), CSTR("DYNAMIC"), CSTR("UNSPECIFIED")};
			if (!reader.ReadString(CSTR("Image Name")) ||
				!reader.ReadEnum(CSTR("Image Data Variance"), imageDataVarianceNames, 3, value) ||
				!reader.ReadObjectReference(CSTR("Image User Data Container")))
				return false;
		}
		return reader.ReadUInt32(CSTR("Texture Width"), value) &&
			reader.ReadUInt32(CSTR("Texture Height"), value);
	}

	static Bool ReadGeometryContinuation(OSGBSceneFieldReader &reader)
	{
		Bool present;
		UInt32 value;
		Text::CStringNN renderBinModeNames[5] = {CSTR("INHERIT_RENDERBIN_DETAILS"), CSTR("USE_RENDERBIN_DETAILS"), CSTR("OVERRIDE_RENDERBIN_DETAILS"), CSTR("PROTECTED_RENDERBIN_DETAILS"), CSTR("OVERRIDE_PROTECTED_RENDERBIN_DETAILS")};
		if (!reader.ReadUInt32(CSTR("Containing StateSet: Texture Attribute Value"), value) ||
			!reader.ReadBool(CSTR("Containing StateSet: Uniform List Present"), present))
			return false;
		if (present)
		{
			if (!reader.ReadUInt32(CSTR("Containing StateSet: Uniform Count"), value) || value > 4096)
				return false;
			if (value > 0)
				return false;
		}
		if (!reader.ReadUInt32(CSTR("Containing StateSet: Rendering Hint"), value) ||
			!reader.ReadEnum(CSTR("Containing StateSet: Render Bin Mode"), renderBinModeNames, 5, value) ||
			!reader.ReadUInt32(CSTR("Containing StateSet: Render Bin Number"), value) ||
			!reader.ReadString(CSTR("Containing StateSet: Render Bin Name")) ||
			!reader.ReadBool(CSTR("Containing StateSet: Nest Render Bins"), present) ||
			!reader.ReadObjectReference(CSTR("Containing StateSet: Update Callback")) ||
			!reader.ReadObjectReference(CSTR("Containing StateSet: Event Callback")))
			return false;

		if (!reader.ReadBool(CSTR("Containing Drawable: Initial Bound Present"), present))
			return false;
		if (present)
		{
			if (!reader.ReadFloat(CSTR("Containing Drawable: Bound Minimum X"), 8) ||
				!reader.ReadFloat(CSTR("Containing Drawable: Bound Minimum Y"), 8) ||
				!reader.ReadFloat(CSTR("Containing Drawable: Bound Minimum Z"), 8) ||
				!reader.ReadFloat(CSTR("Containing Drawable: Bound Maximum X"), 8) ||
				!reader.ReadFloat(CSTR("Containing Drawable: Bound Maximum Y"), 8) ||
				!reader.ReadFloat(CSTR("Containing Drawable: Bound Maximum Z"), 8))
				return false;
		}
		if (!reader.ReadObjectReference(CSTR("Containing Drawable: Compute Bounding Box Callback")) ||
			!reader.ReadObjectReference(CSTR("Containing Drawable: Shape")) ||
			!reader.ReadBool(CSTR("Containing Drawable: Supports Display List"), present) ||
			!reader.ReadBool(CSTR("Containing Drawable: Use Display List"), present) ||
			!reader.ReadBool(CSTR("Containing Drawable: Use Vertex Buffer Objects"), present) ||
			!reader.ReadObjectReference(CSTR("Containing Drawable: Update Callback")) ||
			!reader.ReadObjectReference(CSTR("Containing Drawable: Event Callback")) ||
			!reader.ReadObjectReference(CSTR("Containing Drawable: Cull Callback")) ||
			!reader.ReadObjectReference(CSTR("Containing Drawable: Draw Callback")))
			return false;

		Text::CStringNN primitiveTypeNames[5] = {CSTR("DrawArrays"), CSTR("DrawArrayLengths"), CSTR("DrawElementsUByte"), CSTR("DrawElementsUShort"), CSTR("DrawElementsUInt")};
		UInt32 primitiveCount;
		if (!reader.ReadUInt32(CSTR("Containing Geometry: Primitive Set Count"), primitiveCount) || primitiveCount > 4096)
			return false;
		UInt32 i = 0;
		while (i < primitiveCount)
		{
			if (!reader.ReadEnum(CSTR("Containing Geometry: Primitive Type"), primitiveTypeNames, 5, value, 50))
				return false;
			switch (value)
			{
			case 50:
				if (!reader.ReadUInt32(CSTR("Containing Geometry: Primitive Mode"), value) ||
					!reader.ReadUInt32(CSTR("Containing Geometry: First Vertex"), value) ||
					!reader.ReadUInt32(CSTR("Containing Geometry: Vertex Count"), value))
					return false;
				break;
			case 51:
			{
				UInt32 first;
				UInt32 count;
				if (!reader.ReadUInt32(CSTR("Containing Geometry: Primitive Mode"), value) ||
					!reader.ReadUInt32(CSTR("Containing Geometry: First Vertex"), first) ||
					!reader.ReadUInt32(CSTR("Containing Geometry: Draw Array Length Count"), count) || count > 1000000)
					return false;
				UInt32 j = 0;
				while (j < count)
				{
					if (!reader.ReadUInt32(CSTR("Containing Geometry: Draw Array Length"), value))
						return false;
					j++;
				}
				break;
			}
			case 52:
			case 53:
			case 54:
			{
				UInt32 elementCount;
				UIntOS elementSize = value == 52 ? 1 : value == 53 ? 2 : 4;
				if (!reader.ReadUInt32(CSTR("Containing Geometry: Primitive Mode"), value) ||
					!reader.ReadUInt32(CSTR("Containing Geometry: Index Count"), elementCount) ||
					elementCount > 10000000 ||
					!reader.SkipBytes((UIntOS)elementCount * elementSize, CSTR("Containing Geometry: Primitive Indices"), CSTR("Packed index values")))
					return false;
				break;
			}
			default:
				return false;
			}
			i++;
		}

		UInt32 seenArrayIDs[256];
		UIntOS seenArrayCount = 0;
		Text::CStringNN arrayTypeNames[32] = {
			CSTR("ByteArray"), CSTR("UByteArray"), CSTR("ShortArray"), CSTR("UShortArray"),
			CSTR("IntArray"), CSTR("UIntArray"), CSTR("FloatArray"), CSTR("DoubleArray"),
			CSTR("Vec2bArray"), CSTR("Vec3bArray"), CSTR("Vec4bArray"), CSTR("Vec4ubArray"),
			CSTR("Vec2sArray"), CSTR("Vec3sArray"), CSTR("Vec4sArray"), CSTR("Vec2fArray"),
			CSTR("Vec3fArray"), CSTR("Vec4fArray"), CSTR("Vec2dArray"), CSTR("Vec3dArray"),
			CSTR("Vec4dArray"), CSTR("Vec2ubArray"), CSTR("Vec3ubArray"), CSTR("Vec2usArray"),
			CSTR("Vec3usArray"), CSTR("Vec4usArray"), CSTR("Vec2iArray"), CSTR("Vec3iArray"),
			CSTR("Vec4iArray"), CSTR("Vec2uiArray"), CSTR("Vec3uiArray"), CSTR("Vec4uiArray")
		};
		const UInt8 arrayItemSizes[32] = {1, 1, 2, 2, 4, 4, 4, 8, 2, 3, 4, 4, 4, 6, 8, 8, 12, 16, 16, 24, 32, 2, 3, 4, 6, 8, 8, 12, 16, 8, 12, 16};
		auto readArray = [&](Text::CStringNN arrayName) -> Bool
		{
			UInt32 arrayID;
			if (!reader.ReadUInt32(CSTR("Containing Geometry: Array ID"), arrayID))
				return false;
			UIntOS j = 0;
			while (j < seenArrayCount)
			{
				if (seenArrayIDs[j] == arrayID)
					return true;
				j++;
			}
			if (seenArrayCount >= 256)
				return false;
			seenArrayIDs[seenArrayCount++] = arrayID;
			UInt32 arrayType;
			UInt32 count;
			if (!reader.ReadEnum(CSTR("Containing Geometry: Array Type"), arrayTypeNames, 32, arrayType) ||
				arrayType >= 32 ||
				!reader.ReadUInt32(CSTR("Containing Geometry: Array Element Count"), count) ||
				count > 10000000 ||
				!reader.SkipBytes((UIntOS)count * arrayItemSizes[arrayType], arrayName, CSTR("Packed typed array elements")))
				return false;
			return true;
		};
		auto readArrayData = [&](Text::CStringNN name) -> Bool
		{
			Bool hasArray;
			UInt32 value;
			if (!reader.ReadBool(CSTR("Containing Geometry: Array Present"), hasArray))
				return false;
			if (hasArray && !readArray(name))
				return false;
			if (!reader.ReadBool(CSTR("Containing Geometry: Indices Present"), hasArray))
				return false;
			if (hasArray && !readArray(CSTR("Containing Geometry: Array Indices")))
				return false;
			return reader.ReadUInt32(CSTR("Containing Geometry: Array Binding"), value) &&
				reader.ReadUInt32(CSTR("Containing Geometry: Array Normalize"), value);
		};
		Text::CStringNN arrayFields[5] = {CSTR("Containing Geometry: Vertex Array Data"), CSTR("Containing Geometry: Normal Array Data"), CSTR("Containing Geometry: Color Array Data"), CSTR("Containing Geometry: Secondary Color Array Data"), CSTR("Containing Geometry: Fog Coordinate Array Data")};
		i = 0;
		while (i < 5)
		{
			if (!reader.ReadBool(CSTR("Containing Geometry: Array Serializer Present"), present))
				return false;
			if (present && !readArrayData(arrayFields[i]))
				return false;
			i++;
		}
		i = 0;
		while (i < 2)
		{
			if (!reader.ReadBool(CSTR("Containing Geometry: Array List Present"), present))
				return false;
			if (present)
			{
				UInt32 count;
				if (!reader.ReadUInt32(CSTR("Containing Geometry: Array List Count"), count) || count > 4096)
					return false;
				UInt32 j = 0;
				while (j < count)
				{
					if (!readArrayData(i == 0 ? CSTR("Containing Geometry: Texture Coordinate Array") : CSTR("Containing Geometry: Vertex Attribute Array")))
						return false;
					j++;
				}
			}
			i++;
		}
		return reader.ReadBool(CSTR("Containing Geometry: Fast Path Hint Present"), present);
	}
}

Text::CStringNN IO::FileAnalyse::OSGBFileAnalyse::ContentTypeGetName(UInt32 contentType, Bool isWrappedHeader)
{
	if (isWrappedHeader)
	{
		switch (contentType)
		{
		case 0:
			return CSTR("Unknown");
		case 1:
			return CSTR("Scene");
		case 2:
			return CSTR("Image");
		case 3:
			return CSTR("Object");
		default:
			return CSTR("Unknown");
		}
	}

	switch (contentType)
	{
	case 0:
		return CSTR("Scene");
	case 1:
		return CSTR("Image");
	case 2:
		return CSTR("Object");
	default:
		return CSTR("Unknown");
	}
}

Bool IO::FileAnalyse::OSGBFileAnalyse::IsClassName(UnsafeArray<const UInt8> className, UInt32 nameSize)
{
	Bool hasNamespace = false;
	UInt32 i = 0;
	if (nameSize < 5 || nameSize > 128 || className[0] < 'A' || (className[0] > 'Z' && className[0] < 'a') || className[0] > 'z')
		return false;

	while (i < nameSize)
	{
		UInt8 c = className[i];
		if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_')
		{
		}
		else if (c == ':' && i + 1 < nameSize && className[i + 1] == ':')
		{
			hasNamespace = true;
			i++;
		}
		else
		{
			return false;
		}
		i++;
	}
	return hasNamespace;
}

void IO::FileAnalyse::OSGBFileAnalyse::ParseSceneObjects(NN<IO::StreamData> fd)
{
	const UIntOS chunkSize = 65536;
	const UIntOS maxClassNameSize = 128;
	const UIntOS overlapSize = maxClassNameSize + 8;
	Data::ByteBuffer scanBuff(chunkSize + overlapSize);
	UInt64 scanOfst = this->headerSize;
	UInt64 sceneEnd = this->fileSize;
	NN<SceneObject> obj;

	while (scanOfst < sceneEnd)
	{
		UInt64 remain = sceneEnd - scanOfst;
		UIntOS readSize = (UIntOS)(remain > chunkSize + overlapSize ? chunkSize + overlapSize : remain);
		if (fd->GetRealData(scanOfst, readSize, scanBuff) != readSize)
			return;

		UIntOS scanLimit = remain > chunkSize + overlapSize ? chunkSize : readSize;
		UIntOS i = 0;
		while (i < scanLimit)
		{
			UInt32 nameSize = ReadLUInt32(&scanBuff[i]);
			if (nameSize >= 5 && nameSize <= maxClassNameSize && i + 4 + (UIntOS)nameSize + 4 <= readSize &&
				IsClassName(&scanBuff[i + 4], nameSize))
			{
				obj = MemAllocNN(SceneObject);
				obj->fileOfst = scanOfst + i;
				obj->nameSize = nameSize;
				obj->objectId = ReadLUInt32(&scanBuff[i + 4 + nameSize]);
				obj->dataSize = 0;
				UInt32 j = 0;
				while (j < nameSize)
				{
					obj->className[j] = (UTF8Char)scanBuff[i + 4 + j];
					j++;
				}
				obj->className[nameSize] = 0;
				this->sceneObjects.Add(obj);
				i += 4 + nameSize + 4;
			}
			else
			{
				i++;
			}
		}
		scanOfst += scanLimit;
		if (scanLimit == 0)
			return;
	}

	UIntOS i = 0;
	while (i < this->sceneObjects.GetCount())
	{
		obj = this->sceneObjects.GetItemNoCheck(i);
		UInt64 nextOfst = i + 1 < this->sceneObjects.GetCount() ? this->sceneObjects.GetItemNoCheck(i + 1)->fileOfst : sceneEnd;
		obj->dataSize = nextOfst - obj->fileOfst;
		i++;
	}
}

void __stdcall IO::FileAnalyse::OSGBFileAnalyse::FreeSceneObject(NN<SceneObject> obj)
{
	MemFreeNN(obj);
}

IO::FileAnalyse::OSGBFileAnalyse::OSGBFileAnalyse(NN<IO::StreamData> fd)
{
	UInt8 buff[25];
	UInt32 leType;
	UInt32 leVersion;
	UInt32 leAttributes;
	UInt32 beType;
	UInt32 beVersion;
	UInt32 beAttributes;
	UInt8 classNameSizeBuff[4];
	UInt8 classNamePrefix[5];
	UInt32 classNameSize;
	Bool hasWrappedHeader;
	UInt8 compressionName[256];

	this->fd = nullptr;
	this->fileSize = fd->GetDataSize();
	this->signatureHigh = 0;
	this->signatureLow = 0;
	this->contentType = 0;
	this->version = 0;
	this->attributes = 0;
	this->compressionSize = 0;
	this->compressionFlag = 0;
	this->headerSize = 0;
	this->isWrappedHeader = false;
	this->isCompressed = false;
	this->isBigEndian = false;
	if (this->fileSize < 12 || fd->GetRealData(0, 12, BYTEARR(buff)) != 12)
	{
		return;
	}

	leType = ReadLUInt32(&buff[0]);
	leVersion = ReadLUInt32(&buff[4]);
	leAttributes = ReadLUInt32(&buff[8]);
	beType = ReadMUInt32(&buff[0]);
	beVersion = ReadMUInt32(&buff[4]);
	beAttributes = ReadMUInt32(&buff[8]);
	hasWrappedHeader = false;
	if (this->fileSize >= 34 && fd->GetRealData(0, sizeof(buff), BYTEARR(buff)) == sizeof(buff))
	{
		this->compressionSize = ReadLUInt32(&buff[20]);
		if (ReadLUInt32(&buff[8]) <= 3 && ReadLUInt32(&buff[12]) > 0 && ReadLUInt32(&buff[12]) <= 1000 &&
			this->compressionSize > 0 && this->compressionSize <= sizeof(compressionName) &&
			(UInt64)this->compressionSize + 24 < this->fileSize &&
			fd->GetRealData(24, this->compressionSize, BYTEARR(compressionName)) == this->compressionSize)
		{
			this->headerSize = 24 + this->compressionSize;
			this->isCompressed = !(this->compressionSize == 1 && compressionName[0] == '0');
			if (fd->GetRealData(this->headerSize, 4, BYTEARR(classNameSizeBuff)) == 4)
			{
				classNameSize = ReadLUInt32(classNameSizeBuff);
				hasWrappedHeader = classNameSize >= sizeof(classNamePrefix) && classNameSize <= 128 &&
					(UInt64)classNameSize + 8 <= this->fileSize - this->headerSize &&
					fd->GetRealData(this->headerSize + 4, sizeof(classNamePrefix), BYTEARR(classNamePrefix)) == sizeof(classNamePrefix) &&
					Text::StrEqualsC(classNamePrefix, sizeof(classNamePrefix), UTF8STRC("osg::"));
			}
		}
	}
	if (hasWrappedHeader)
	{
		this->signatureHigh = ReadLUInt32(&buff[0]);
		this->signatureLow = ReadLUInt32(&buff[4]);
		this->contentType = ReadLUInt32(&buff[8]);
		this->version = ReadLUInt32(&buff[12]);
		this->attributes = ReadLUInt32(&buff[16]);
		this->compressionFlag = buff[24];
		this->isWrappedHeader = true;
	}
	else if (leType <= 2 && leVersion > 0 && leVersion <= 1000 && leAttributes <= 7)
	{
		this->contentType = leType;
		this->version = leVersion;
		this->attributes = leAttributes;
		this->headerSize = 12;
	}
	else if (beType <= 2 && beVersion > 0 && beVersion <= 1000 && beAttributes <= 7)
	{
		this->contentType = beType;
		this->version = beVersion;
		this->attributes = beAttributes;
		this->headerSize = 12;
		this->isBigEndian = true;
	}
	else
	{
		return;
	}
	this->fd = fd->GetPartialData(0, this->fileSize);
	if (this->isWrappedHeader && !this->isCompressed)
	{
		this->ParseSceneObjects(fd);
	}
}

IO::FileAnalyse::OSGBFileAnalyse::~OSGBFileAnalyse()
{
	this->fd.Delete();
	this->sceneObjects.FreeAll(FreeSceneObject);
}

Text::CStringNN IO::FileAnalyse::OSGBFileAnalyse::GetFormatName()
{
	return CSTR("OSGB");
}

UIntOS IO::FileAnalyse::OSGBFileAnalyse::GetFrameCount()
{
	if (this->fd.IsNull())
		return 0;
	if (this->sceneObjects.GetCount() > 0)
		return this->sceneObjects.GetCount() + 1;
	return this->fileSize > this->headerSize ? 2 : 1;
}

Bool IO::FileAnalyse::OSGBFileAnalyse::GetFrameName(UIntOS index, NN<Text::StringBuilderUTF8> sb)
{
	if (this->fd.IsNull() || index >= this->GetFrameCount())
		return false;

	if (index == 0)
	{
		sb->AppendC(UTF8STRC("0: OSGB header, size="));
		sb->AppendUIntOS(this->headerSize);
	}
	else
	{
		if (this->sceneObjects.GetCount() > 0)
		{
			NN<SceneObject> obj = this->sceneObjects.GetItemNoCheck(index - 1);
			sb->AppendU64(obj->fileOfst);
			sb->AppendC(UTF8STRC(": "));
			sb->AppendC(obj->className, obj->nameSize);
			sb->AppendC(UTF8STRC(", object id="));
			sb->AppendU32(obj->objectId);
			sb->AppendC(UTF8STRC(", size="));
			sb->AppendU64(obj->dataSize);
		}
		else
		{
			sb->AppendU64(this->headerSize);
			sb->AppendC(UTF8STRC(": Serialized scene data, size="));
			sb->AppendU64(this->fileSize - this->headerSize);
		}
	}
	return true;
}

UIntOS IO::FileAnalyse::OSGBFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	if (this->fd.IsNull() || ofst >= this->fileSize)
		return INVALID_INDEX;
	if (ofst < this->headerSize)
		return 0;
	if (this->sceneObjects.GetCount() > 0)
	{
		IntOS i = 0;
		IntOS j = (IntOS)this->sceneObjects.GetCount() - 1;
		while (i <= j)
		{
			IntOS k = (i + j) >> 1;
			NN<SceneObject> obj = this->sceneObjects.GetItemNoCheck((UIntOS)k);
			if (ofst >= obj->fileOfst && ofst < obj->fileOfst + obj->dataSize)
				return (UIntOS)k + 1;
			if (ofst < obj->fileOfst)
				j = k - 1;
			else
				i = k + 1;
		}
	}
	return 1;
}

Optional<IO::FileAnalyse::FrameDetail> IO::FileAnalyse::OSGBFileAnalyse::GetFrameDetail(UIntOS index)
{
	if (this->fd.IsNull() || index >= this->GetFrameCount())
		return nullptr;

	NN<IO::FileAnalyse::FrameDetail> frame;
	if (index == 0)
	{
		NEW_CLASSNN(frame, IO::FileAnalyse::FrameDetail(0, this->headerSize));
		frame->AddHeader(CSTR("OSGB binary header"));
		if (this->isWrappedHeader)
		{
			NN<IO::StreamData> fd;
			frame->AddHex32(0, CSTR("Header High"), this->signatureHigh);
			frame->AddHex32(4, CSTR("Header Low"), this->signatureLow);
			frame->AddUInt(8, 4, CSTR("Content Type"), this->contentType);
			frame->AddField(8, 4, CSTR("Content Type Name"), ContentTypeGetName(this->contentType, true));
			frame->AddUInt(12, 4, CSTR("Version"), this->version);
			frame->AddUInt(16, 4, CSTR("Attributes"), this->attributes);
			frame->AddUInt(20, 4, CSTR("Compressor Name Length"), this->compressionSize);
			if (this->fd.SetTo(fd) && this->compressionSize > 0)
			{
				Data::ByteBuffer compressorName(this->compressionSize);
				if (fd->GetRealData(24, this->compressionSize, compressorName) == this->compressionSize)
				{
					frame->AddStrC(24, this->compressionSize, CSTR("Compressor Name"), compressorName.Arr());
				}
			}
		}
		else
		{
			frame->AddUInt(0, 4, CSTR("Content Type"), this->contentType);
			frame->AddField(0, 4, CSTR("Content Type Name"), ContentTypeGetName(this->contentType, false));
			frame->AddUInt(4, 4, CSTR("Version"), this->version);
			frame->AddUInt(8, 4, CSTR("Attributes"), this->attributes);
			frame->AddField(8, 4, CSTR("Byte Order"), this->isBigEndian ? CSTR("Big-endian") : CSTR("Little-endian"));
		}
	}
	else
	{
		if (this->sceneObjects.GetCount() > 0)
		{
			NN<SceneObject> obj = this->sceneObjects.GetItemNoCheck(index - 1);
			NEW_CLASSNN(frame, IO::FileAnalyse::FrameDetail(obj->fileOfst, obj->dataSize));
			frame->AddHeader(Text::CStringNN(obj->className, obj->nameSize));
			frame->AddUInt(0, 4, CSTR("Class Name Length"), obj->nameSize);
			frame->AddField(4, obj->nameSize, CSTR("Class Name"), Text::CStringNN(obj->className, obj->nameSize));
			frame->AddUInt(4 + obj->nameSize, 4, CSTR("Object ID"), obj->objectId);
			UIntOS dataOfst = 8 + obj->nameSize;
			NN<IO::StreamData> fd;
			if (this->fd.SetTo(fd) && obj->dataSize > dataOfst)
			{
				UInt64 dataStart = obj->fileOfst + dataOfst;
				UInt64 dataEnd = obj->fileOfst + obj->dataSize;
				OSGBSceneFieldReader reader(fd, frame, obj->fileOfst, dataStart, dataEnd);
				Bool supported = false;
				if (this->version == 80 && !this->isBigEndian &&
					Text::StrEqualsC(obj->className, obj->nameSize, UTF8STRC("osg::PagedLOD")))
				{
					supported = true;
					if (ReadObjectFields(reader) && ReadNodeFields(reader))
						ReadPagedLODFields(reader);
				}
				else if (this->version == 80 && !this->isBigEndian &&
					Text::StrEqualsC(obj->className, obj->nameSize, UTF8STRC("osg::Geode")))
				{
					supported = true;
					if (ReadObjectFields(reader) && ReadNodeFields(reader))
						ReadGeodeFields(reader);
				}
				else if (this->version == 80 && !this->isBigEndian &&
					Text::StrEqualsC(obj->className, obj->nameSize, UTF8STRC("osg::Geometry")))
				{
					supported = true;
					if (ReadObjectFields(reader) && ReadDrawableFields(reader))
						ReadGeometryFields(reader);
				}
				else if (this->version == 80 && !this->isBigEndian &&
					Text::StrEqualsC(obj->className, obj->nameSize, UTF8STRC("osg::StateSet")))
				{
					supported = true;
					if (ReadObjectFields(reader))
						ReadStateSetFields(reader);
				}
				else if (this->version == 80 && !this->isBigEndian &&
					Text::StrEqualsC(obj->className, obj->nameSize, UTF8STRC("osg::Material")))
				{
					supported = true;
					if (ReadObjectFields(reader))
					{
						if (ReadMaterialFields(reader))
						{
							if (index < this->sceneObjects.GetCount() &&
								Text::StrEqualsC(this->sceneObjects.GetItemNoCheck(index)->className,
									this->sceneObjects.GetItemNoCheck(index)->nameSize, UTF8STRC("osg::Texture2D")))
								ReadStateSetContinuation(reader);
						}
					}
				}
				else if (this->version == 80 && !this->isBigEndian &&
					Text::StrEqualsC(obj->className, obj->nameSize, UTF8STRC("osg::Texture2D")))
				{
					supported = true;
					if (ReadObjectFields(reader))
					{
						if (ReadTexture2DFields(reader) &&
							index >= 4 &&
							Text::StrEqualsC(this->sceneObjects.GetItemNoCheck(index - 2)->className,
								this->sceneObjects.GetItemNoCheck(index - 2)->nameSize, UTF8STRC("osg::Material")) &&
							Text::StrEqualsC(this->sceneObjects.GetItemNoCheck(index - 3)->className,
								this->sceneObjects.GetItemNoCheck(index - 3)->nameSize, UTF8STRC("osg::StateSet")) &&
							Text::StrEqualsC(this->sceneObjects.GetItemNoCheck(index - 4)->className,
								this->sceneObjects.GetItemNoCheck(index - 4)->nameSize, UTF8STRC("osg::Geometry")))
							ReadGeometryContinuation(reader);
					}
				}

				UInt64 decodedEnd = reader.GetPosition();
				if (!supported)
				{
					frame->AddField(dataOfst, obj->dataSize - dataOfst, CSTR("Serialized Properties"), CSTR("No version-80 field decoder for this class"));
				}
				else if (decodedEnd < dataEnd)
				{
					Text::CStringNN remainderName = Text::StrEqualsC(obj->className, obj->nameSize, UTF8STRC("osg::Texture2D")) ?
						CSTR("Following serialized scene data") : CSTR("Undecoded serialized data");
					Text::CString remainderValue = Text::StrEqualsC(obj->className, obj->nameSize, UTF8STRC("osg::Texture2D")) ?
						CSTR("Geometry or containing-scene fields after the texture object") : CSTR("Additional class-specific or nested OSG data");
					frame->AddField((UIntOS)(decodedEnd - obj->fileOfst), (UIntOS)(dataEnd - decodedEnd), remainderName, remainderValue);
				}
			}
		}
		else
		{
			NEW_CLASSNN(frame, IO::FileAnalyse::FrameDetail(this->headerSize, this->fileSize - this->headerSize));
			frame->AddHeader(CSTR("Serialized scene data"));
			frame->AddField(0, this->fileSize - this->headerSize, CSTR("Payload"), this->isCompressed ? CSTR("Compressed OSG object stream (not decoded)") : CSTR("OSG binary object stream (not decoded)"));
		}
	}
	return frame;
}

Bool IO::FileAnalyse::OSGBFileAnalyse::IsError()
{
	return this->fd.IsNull();
}

Bool IO::FileAnalyse::OSGBFileAnalyse::IsParsing()
{
	return false;
}

Bool IO::FileAnalyse::OSGBFileAnalyse::TrimPadding(Text::CStringNN outputFile)
{
	return false;
}
