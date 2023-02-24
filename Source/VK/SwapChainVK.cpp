/*
Copyright (c) 2022, NVIDIA CORPORATION. All rights reserved.

NVIDIA CORPORATION and its licensors retain all intellectual property
and proprietary rights in and to this software, related documentation
and any modifications thereto. Any use, reproduction, disclosure or
distribution of this software and related documentation without an express
license agreement from NVIDIA CORPORATION is strictly prohibited.
*/

#include "SharedVK.h"
#include "SwapChainVK.h"
#include "CommandQueueVK.h"
#include "QueueSemaphoreVK.h"
#include "TextureVK.h"
#include "DeviceVK.h"

using namespace nri;

static std::array<VkFormat, 5> g_SwapChainFormats =
{
    VK_FORMAT_B8G8R8A8_SRGB,                      // BT709_G10_8BIT,
    VK_FORMAT_R16G16B16A16_SFLOAT,                // BT709_G10_16BIT,
    VK_FORMAT_B8G8R8A8_UNORM,                     // BT709_G22_8BIT,
    VK_FORMAT_A2B10G10R10_UNORM_PACK32,           // BT709_G22_10BIT,
    VK_FORMAT_A2B10G10R10_UNORM_PACK32,           // BT2020_G2084_10BIT
};

static std::array<VkFormat, 5> g_SwapChainAltFormats =
{
    VK_FORMAT_R8G8B8A8_SRGB,                      // BT709_G10_8BIT,
    VK_FORMAT_R16G16B16A16_SFLOAT,                // BT709_G10_16BIT,
    VK_FORMAT_R8G8B8A8_UNORM,                     // BT709_G22_8BIT,
    VK_FORMAT_A2R10G10B10_UNORM_PACK32,           // BT709_G22_10BIT,
    VK_FORMAT_A2R10G10B10_UNORM_PACK32,           // BT2020_G2084_10BIT
};

static std::array<VkColorSpaceKHR, 5> g_SwapChainColorSpaces =
{
    VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,              // BT709_G10_8BIT,
    VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT,        // BT709_G10_16BIT,
    VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,              // BT709_G22_8BIT,
    VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,              // BT709_G22_10BIT,
    VK_COLOR_SPACE_HDR10_ST2084_EXT,                // BT2020_G2084_10BIT
};

SwapChainVK::SwapChainVK(DeviceVK& device) :
    m_Textures(device.GetStdAllocator()),
    m_Device(device)
{
}

SwapChainVK::~SwapChainVK()
{
    Cleanup();
}

void SwapChainVK::Cleanup()
{
    const auto& vk = m_Device.GetDispatchTable();

    for (size_t i = 0; i < m_Textures.size(); i++)
    {
        m_Textures[i]->ClearHandle();
        Deallocate(m_Device.GetStdAllocator(), m_Textures[i]);
    }
    m_Textures.clear();

    if (m_Handle != VK_NULL_HANDLE)
        vk.DestroySwapchainKHR(m_Device, m_Handle, m_Device.GetAllocationCallbacks());

    if (m_Surface != VK_NULL_HANDLE)
        vk.DestroySurfaceKHR(m_Device, m_Surface, m_Device.GetAllocationCallbacks());
}

Result SwapChainVK::CreateSurface(const SwapChainDesc& swapChainDesc)
{
    const auto& vk = m_Device.GetDispatchTable();

    VkResult result;

#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (swapChainDesc.windowSystemType == WindowSystemType::WINDOWS)
    {
        VkWin32SurfaceCreateInfoKHR win32SurfaceInfo = {};
        win32SurfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        win32SurfaceInfo.hwnd = (HWND)swapChainDesc.window.windows.hwnd;

        result = vk.CreateWin32SurfaceKHR(m_Device, &win32SurfaceInfo, m_Device.GetAllocationCallbacks(), &m_Surface);

        RETURN_ON_FAILURE(m_Device.GetLog(), result == VK_SUCCESS, GetReturnCode(result),
            "Can't create a surface: vkCreateWin32SurfaceKHR returned %d.", (int32_t)result);

        return Result::SUCCESS;
    }
#endif
#ifdef VK_USE_PLATFORM_METAL_EXT
    if (swapChainDesc.windowSystemType == WindowSystemType::METAL)
    {
        VkMetalSurfaceCreateInfoEXT metalSurfaceCreateInfo = {};
        metalSurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
        metalSurfaceCreateInfo.pLayer = (CAMetalLayer*)swapChainDesc.window.metal.caMetalLayer;

        result = vk.CreateMetalSurfaceEXT(m_Device, &metalSurfaceCreateInfo, m_Device.GetAllocationCallbacks(), &m_Surface);

        RETURN_ON_FAILURE(m_Device.GetLog(), result == VK_SUCCESS, GetReturnCode(result),
            "Can't create a surface: vkCreateMetalSurfaceEXT returned %d.", (int32_t)result);

        return Result::SUCCESS;
    }
#endif
#ifdef VK_USE_PLATFORM_XLIB_KHR
    if (swapChainDesc.windowSystemType == WindowSystemType::X11)
    {
        VkXlibSurfaceCreateInfoKHR xlibSurfaceInfo = {};
        xlibSurfaceInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
        xlibSurfaceInfo.dpy = (::Display*)swapChainDesc.window.x11.dpy;
        xlibSurfaceInfo.window = (::Window)swapChainDesc.window.x11.window;

        result = vk.CreateXlibSurfaceKHR(m_Device, &xlibSurfaceInfo, m_Device.GetAllocationCallbacks(), &m_Surface);

        RETURN_ON_FAILURE(m_Device.GetLog(), result == VK_SUCCESS, GetReturnCode(result),
            "Can't create a surface: vkCreateXlibSurfaceKHR returned %d.", (int32_t)result);

        return Result::SUCCESS;
    }
#endif
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    if (swapChainDesc.windowSystemType == WindowSystemType::WAYLAND)
    {
        VkWaylandSurfaceCreateInfoKHR waylandSurfaceInfo = {};
        waylandSurfaceInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
        waylandSurfaceInfo.display = (wl_display*)swapChainDesc.window.wayland.display;
        waylandSurfaceInfo.surface = (wl_surface*)swapChainDesc.window.wayland.surface;

        result = vk.CreateWaylandSurfaceKHR(m_Device, &waylandSurfaceInfo, m_Device.GetAllocationCallbacks(), &m_Surface);

        RETURN_ON_FAILURE(m_Device.GetLog(), result == VK_SUCCESS, GetReturnCode(result),
            "Can't create a surface: vkCreateWaylandSurfaceKHR returned %d.", (int32_t)result);

        return Result::SUCCESS;
    }
#endif
#ifdef VK_USE_PLATFORM_ANDROID_KHR
    if (swapChainDesc.windowSystemType == WindowSystemType::DROID)
    {
        VkAndroidSurfaceCreateInfoKHR androidSurfaceInfo = {};
        androidSurfaceInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
        androidSurfaceInfo.window = (ANativeWindow*)swapChainDesc.window.android.window;

        result = vk.CreateAndroidSurfaceKHR(m_Device, &androidSurfaceInfo, m_Device.GetAllocationCallbacks(), &m_Surface);

        RETURN_ON_FAILURE(m_Device.GetLog(), result == VK_SUCCESS, GetReturnCode(result),
            "Can't create a surface: vkCreateAndroidSurfaceKHR returned %d.", (int32_t)result);

        return Result::SUCCESS;
    }
#endif

    return Result::UNSUPPORTED;
}

Result SwapChainVK::Create(const SwapChainDesc& swapChainDesc)
{
    m_CommandQueue = (CommandQueueVK*)swapChainDesc.commandQueue;

    {
        const Result result = CreateSurface(swapChainDesc);
        if (result != Result::SUCCESS)
            return result;
    }

    const auto& vk = m_Device.GetDispatchTable();

    VkBool32 supported = VK_FALSE;
    vk.GetPhysicalDeviceSurfaceSupportKHR(m_Device, m_CommandQueue->GetFamilyIndex(), m_Surface, &supported);

    if (supported == VK_FALSE)
    {
        REPORT_ERROR(m_Device.GetLog(), "The specified surface is not supported by the physical device.");
        return Result::UNSUPPORTED;
    }

    VkSurfaceCapabilitiesKHR capabilites = {};
    VkResult result = vk.GetPhysicalDeviceSurfaceCapabilitiesKHR(m_Device, m_Surface, &capabilites);

    RETURN_ON_FAILURE(m_Device.GetLog(), result == VK_SUCCESS, GetReturnCode(result),
        "Can't get physical device surface capabilities: vkGetPhysicalDeviceSurfaceCapabilitiesKHR returned %d.", (int32_t)result);

    const bool isWidthValid = swapChainDesc.width >= capabilites.minImageExtent.width &&
        swapChainDesc.width <= capabilites.maxImageExtent.width;
    const bool isHeightValid = swapChainDesc.height >= capabilites.minImageExtent.height &&
        swapChainDesc.height <= capabilites.maxImageExtent.height;

    if (!isWidthValid || !isHeightValid)
    {
        REPORT_ERROR(m_Device.GetLog(), "Invalid SwapChainVK buffer size.");
        return Result::INVALID_ARGUMENT;
    }

    uint32_t formatNum = 0;
    result = vk.GetPhysicalDeviceSurfaceFormatsKHR(m_Device, m_Surface, &formatNum, nullptr);

    RETURN_ON_FAILURE(m_Device.GetLog(), result == VK_SUCCESS, GetReturnCode(result),
        "Can't get physical device surface formats: vkGetPhysicalDeviceSurfaceFormatsKHR returned %d.", (int32_t)result);

    VkSurfaceFormatKHR* surfaceFormats = STACK_ALLOC(VkSurfaceFormatKHR, formatNum);
    result = vk.GetPhysicalDeviceSurfaceFormatsKHR(m_Device, m_Surface, &formatNum, surfaceFormats);

    RETURN_ON_FAILURE(m_Device.GetLog(), result == VK_SUCCESS, GetReturnCode(result),
        "Can't get physical device surface formats: vkGetPhysicalDeviceSurfaceFormatsKHR returned %d.", (int32_t)result);

    VkSurfaceFormatKHR surfaceFormat = surfaceFormats[0];

    // Find desired surface format
    for (uint32_t i = 0; i < formatNum; i++)
    {
        if ((surfaceFormats[i].format == g_SwapChainFormats[(int)swapChainDesc.format] ||
            surfaceFormats[i].format == g_SwapChainAltFormats[(int)swapChainDesc.format]) &&
            surfaceFormats[i].colorSpace == g_SwapChainColorSpaces[(int)swapChainDesc.format])
        {
			surfaceFormat = surfaceFormats[i];
			break;
		}
    }

    m_Format = VKFormatToNRIFormat(surfaceFormat.format);

    uint32_t presentModeNum = 0;
    result = vk.GetPhysicalDeviceSurfacePresentModesKHR(m_Device, m_Surface, &presentModeNum, nullptr);

    RETURN_ON_FAILURE(m_Device.GetLog(), result == VK_SUCCESS, GetReturnCode(result),
        "Can't get supported present modes for the surface: vkGetPhysicalDeviceSurfacePresentModesKHR returned %d.", (int32_t)result);

    VkPresentModeKHR* presentModes = STACK_ALLOC(VkPresentModeKHR, presentModeNum);
    result = vk.GetPhysicalDeviceSurfacePresentModesKHR(m_Device, m_Surface, &presentModeNum, presentModes);

    RETURN_ON_FAILURE(m_Device.GetLog(), result == VK_SUCCESS, GetReturnCode(result),
        "Can't get supported present modes for the surface: vkGetPhysicalDeviceSurfacePresentModesKHR returned %d.", (int32_t)result);

    // Both of these modes use v-sync for preseting, but FIFO blocks execution
    VkPresentModeKHR desiredPresentMode = swapChainDesc.verticalSyncInterval ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_MAILBOX_KHR;

    supported = false;
    for (uint32_t i = 0; i < presentModeNum && !supported; i++)
        supported = desiredPresentMode == presentModes[i];

    if (!supported)
    {
        REPORT_WARNING(m_Device.GetLog(),
            "The present mode is not supported. Using the first mode from the list of supported modes. (Mode: %d)", (int32_t)desiredPresentMode);

        desiredPresentMode = presentModes[0];
    }

    const uint32_t familyIndex = m_CommandQueue->GetFamilyIndex();
    const uint32_t minImageNum = std::max<uint32_t>(capabilites.minImageCount, swapChainDesc.textureNum);

    const VkSwapchainCreateInfoKHR swapchainInfo = {
        VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        nullptr,
        (VkSwapchainCreateFlagsKHR)0,
        m_Surface,
        minImageNum,
        surfaceFormat.format,
        surfaceFormat.colorSpace,
        { swapChainDesc.width, swapChainDesc.height },
        1,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
        VK_SHARING_MODE_EXCLUSIVE,
        1,
        &familyIndex,
        VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        desiredPresentMode,
        VK_FALSE,
        VK_NULL_HANDLE
    };

    result = vk.CreateSwapchainKHR(m_Device, &swapchainInfo, m_Device.GetAllocationCallbacks(), &m_Handle);

    RETURN_ON_FAILURE(m_Device.GetLog(), result == VK_SUCCESS, GetReturnCode(result),
        "Can't create a swapchain: vkCreateSwapchainKHR returned %d.", (int32_t)result);

    uint32_t imageNum = 0;
    vk.GetSwapchainImagesKHR(m_Device, m_Handle, &imageNum, nullptr);

    VkImage* imageHandles = STACK_ALLOC(VkImage, imageNum);
    vk.GetSwapchainImagesKHR(m_Device, m_Handle, &imageNum, imageHandles);

    m_Textures.resize(imageNum);
    for (uint32_t i = 0; i < imageNum; i++)
    {
        TextureVK* texture = Allocate<TextureVK>(m_Device.GetStdAllocator(), m_Device);
        const VkExtent3D extent = { swapchainInfo.imageExtent.width, swapchainInfo.imageExtent.height, 1 };
        texture->Create(imageHandles[i], VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_TYPE_2D, extent, m_Format);
        m_Textures[i] = texture;
    }

    m_Desc = swapChainDesc;

    return Result::SUCCESS;
}

inline void SwapChainVK::SetDebugName(const char* name)
{
    m_Device.SetDebugNameToTrivialObject(VK_OBJECT_TYPE_SURFACE_KHR, (uint64_t)m_Surface, name);
    m_Device.SetDebugNameToTrivialObject(VK_OBJECT_TYPE_SWAPCHAIN_KHR, (uint64_t)m_Handle, name);
}

inline Texture* const* SwapChainVK::GetTextures(uint32_t& textureNum, Format& format) const
{
    textureNum = (uint32_t)m_Textures.size();
    format = m_Format;
    return (Texture* const*)m_Textures.data();
}

inline uint32_t SwapChainVK::AcquireNextTexture(QueueSemaphore& textureReadyForRender)
{
    const uint64_t timeout = 5000000000; // 5 seconds
    m_TextureIndex = std::numeric_limits<uint32_t>::max();

    const auto& vk = m_Device.GetDispatchTable();
    const VkResult result = vk.AcquireNextImageKHR(m_Device, m_Handle, timeout, *(QueueSemaphoreVK*)&textureReadyForRender,
        VK_NULL_HANDLE, &m_TextureIndex);

    RETURN_ON_FAILURE(m_Device.GetLog(), result == VK_SUCCESS, m_TextureIndex,
        "Can't acquire the next texture of the swapchain: vkAcquireNextImageKHR returned %d.", (int32_t)result);

    return m_TextureIndex;
}

inline Result SwapChainVK::Resize(uint16_t width, uint16_t height)
{
    Cleanup();

    auto desc = m_Desc;
    desc.width = width;
    desc.height = height;

    return Create(desc);
}

inline Result SwapChainVK::Present(QueueSemaphore& textureReadyForPresent)
{
    const VkSemaphore semaphore = (QueueSemaphoreVK&)textureReadyForPresent;

    VkResult presentRes = VK_SUCCESS;

    const VkPresentInfoKHR info = {
        VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        nullptr,
        1,
        &semaphore,
        1,
        &m_Handle,
        &m_TextureIndex,
        &presentRes
    };

    const auto& vk = m_Device.GetDispatchTable();
    const VkResult result = vk.QueuePresentKHR(*m_CommandQueue, &info);

    if (result == VkResult::VK_SUBOPTIMAL_KHR)
        return Result::SWAPCHAIN_RESIZE;

    RETURN_ON_FAILURE(m_Device.GetLog(), result == VK_SUCCESS, GetReturnCode(result),
        "Can't present the swapchain: vkQueuePresentKHR returned %d.", (int32_t)result);

    return Result::SUCCESS;
}

inline Result SwapChainVK::SetHdrMetadata(const HdrMetadata& hdrMetadata)
{
    const auto& vk = m_Device.GetDispatchTable();
    if (!vk.SetHdrMetadataEXT)
        return Result::UNSUPPORTED;

    const VkHdrMetadataEXT data = {
        VK_STRUCTURE_TYPE_HDR_METADATA_EXT,
        nullptr,
        {hdrMetadata.displayPrimaryRed[0], hdrMetadata.displayPrimaryRed[1]},
        {hdrMetadata.displayPrimaryGreen[0], hdrMetadata.displayPrimaryGreen[1]},
        {hdrMetadata.displayPrimaryBlue[0], hdrMetadata.displayPrimaryBlue[1]},
        {hdrMetadata.whitePoint[0], hdrMetadata.whitePoint[1]},
        hdrMetadata.luminanceMax,
        hdrMetadata.luminanceMin,
        hdrMetadata.contentLightLevelMax,
        hdrMetadata.frameAverageLightLevelMax
    };

    vk.SetHdrMetadataEXT(m_Device, 1, &m_Handle, &data);
    return Result::SUCCESS;
}

#include "SwapChainVK.hpp"
