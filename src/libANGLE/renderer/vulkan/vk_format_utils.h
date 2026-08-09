 angle::FormatID actualFormatID);
bool HasEmulatedImageChannels(const angle::Format &intendedFormat,
                              const angle::Format &actualFormat);
// Returns true if the image has a different image format than intended.
bool HasEmulatedImageFormat(angle::FormatID intendedFormatID, angle::FormatID actualFormatID);

// Describes a Vulkan format. For more information on formats in the Vulkan back-end please see
// https://chromium.googlesource.com/angle/angle/+/main/src/libANGLE/renderer/vulkan/doc/FormatTablesAndEmulation.md
class Format final : private angle::NonCopyable
{
  public:
    Format();

    bool valid() const { return mIntendedGLFormat != 0; }
    GLenum getIntendedGLFormat() const { return mIntendedGLFormat; }

    // The intended format is the front-end format. For Textures this usually correponds to a
    // GLenum in the headers. Buffer formats don't always have a corresponding GLenum type.
    // Some Surface formats and unsized types also don't have a corresponding GLenum.
    angle::FormatID getIntendedFormatID() const { return mIntendedFormatID; }
    const angle::Format &getIntendedFormat() const { return angle::Format::Get(mIntendedFormatID); }

    // The actual Image format is used to implement the front-end format for Texture/Renderbuffers.
    const angle::Format &getActualImageFormat(ImageFormatSupport support) const
    {
        return angle::Format::Get(getActualImageFormatID(support));
    }

    angle::FormatID getActualRenderableImageFormatID() const
    {
        return mActualRenderableImageFormatID;
    }
    const angle::Format &getActualRenderableImageFormat() const
    {
        return angle::Format::Get(mActualRenderableImageFormatID);
    }
    VkFormat getActualRenderableImageVkFormat(const Renderer *renderer) const
    {
        return GetVkFormatFromFormatID(renderer, mActualRenderableImageFormatID);
    }

    angle::FormatID getActualImageFormatID(ImageFormatSupport support) const
    {
        return support == ImageFormatSupport::Renderable ? mActualRenderableImageFormatID
                                                         : mActualSampleOnlyImageFormatID;
    }
    VkFormat getActualImageVkFormat(const Renderer *renderer, ImageFormatSupport support) const
    {
        return GetVkFormatFromFormatID(renderer, getActualImageFormatID(support));
    }

    LoadImageFunctionInfo getTextureLoadFunction(ImageFormatSupport support, GLenum type) const
    {
        return support == ImageFormatSupport::Renderable ? mRenderableTextureLoadFunctions(type)
                                                         : mTextureLoadFunctions(type);
    }

    static LoadFunctionMap GetRGB565TextureLoadFunction(const Renderer *renderer);

    // The actual Buffer format is used to implement the front-end format for Buffers.  This format
    // is used by vertex buffers as well as texture buffers.  Note that all formats required for
    // GL_EXT_texture_buffer have mandatory support for vertex buffers in Vulkan, so they won't be
    // using an emulated format.
    const angle::Format &getActualBufferFormat() const
    {
        return angle::Format::Get(mActualBufferFormatID);
    }

    VkFormat getActualBufferVkFormat(const Renderer *renderer) const
    {
        return GetVkFormatFromFormatID(renderer, mActualBufferFormatID);
    }

    VertexCopyFunction getVertexLoadFunction() const { return mVertexLoadFunction; }

    bool getVertexLoadRequiresConversion() const { return mVertexLoadRequiresConversion; }

    // |intendedGLFormat| always correponds to a valid GLenum type. For types that don't have a
    // corresponding GLenum we do our best to specify a GLenum that is "close".
    const gl::InternalFormat &getInternalFormatInfo(GLenum type) const
    {
        return gl::GetInternalFormatInfo(mIntendedGLFormat, type);
    }

    bool hasRenderableImageFallbackFormat() const
    {
        return mActualSampleOnlyImageFormatID != mActualRenderableImageFormatID;
    }

    // Returns the alignment for a buffer to be used with the vertex input stage in Vulkan. This
    // calculation is listed in the Vulkan spec at the end of the section 'Vertex Input
    // Description'.
    size_t getVertexInputAlignment() const;

  private:
    friend class FormatTable;

    // This is an auto-generated method in vk_format_table_autogen.cpp.
    void initialize(Renderer *renderer, const angle::Format &intendedAngleFormat);

    // These are used in the format table init.
    void initImageFallback(Renderer *renderer, const ImageFormatInitInfo *info, int numInfo);
    void initBufferFallback(Renderer *renderer,
                            const BufferFormatInitInfo *fallbackInfo,
                            int numInfo,
                            int compressedStartIndex);

    angle::FormatID mIntendedFormatID;
    GLenum mIntendedGLFormat;
    angle::FormatID mActualSampleOnlyImageFormatID;
    angle::FormatID mActualRenderableImageFormatID;
    angle::FormatID mActualBufferFormatID;

    InitializeTextureDataFunction mImageInitializerFunction;
    LoadFunctionMap mTextureLoadFunctions;
    LoadFunctionMap mRenderableTextureLoadFunctions;
    VertexCopyFunction mVertexLoadFunction;
    VertexCopyFunction mCompressedVertexLoadFunction;
};

bool operator==(const Format &lhs, const Format &rhs);
bool operator!=(const Format &lhs, const Format &rhs);

class FormatTable final : angle::NonCopyablebool HasNonRenderableTextureFormatSupport(vk::Renderer *renderer, angle::FormatID formatID);
// Checks if a Vulkan format supports all the features needed for a non-filterable texture.
bool HasNonFilterableTextureFormatSupport(vk::Renderer *renderer, angle::FormatID formatID);
// Checks if a Vulkan format supports all the features needed for a sample-only (no filtering, no
// rendering) texture.
bool HasSampleOnlyTextureFormatSupport(vk::Renderer *renderer, angle::FormatID formatID);
// Checks if a format supports filtering.
bool IsFilterableFormat(const angle::Format &format);
// Checks if it is a ETC texture format
bool IsETCFormat(angle::FormatID formatID);
// Checks if it is an ASTC texture format
bool IsASTC3DFormat(angle::FormatID formatID);
// Checks if it is a BC texture format
bool IsBCFormat(angle::FormatID formatID);

angle::FormatID GetTranscodeBCFormatID(angle::FormatID formatID);

VkFormat AdjustASTCFormatForHDR(const vk::Renderer *renderer, VkFormat vkFormat);

// Get Etc format cpu transcoding to Bc function.
LoadImageFunctionInfo GetEtcToBcTransCodingFunc(angle::FormatID formatID);

// Get the swizzle state based on format's requirements and emulations.
gl::SwizzleState GetFormatSwizzle(const angle::Format &angleFormat, const bool sized);

// Apply application's swizzle to the swizzle implied by format as received from GetFormatSwizzle.
gl::SwizzleState ApplySwizzle(const gl::SwizzleState &formatSwizzle,
                              const gl::SwizzleState &toApply);

}  // namespace rx

#endif  // LIBANGLE_RENDERER_VULKAN_VK_FORMAT_UTILS_H_
