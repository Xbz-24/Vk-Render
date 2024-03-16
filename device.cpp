/**
 * @file device.cpp
 * @brief Defines functions for creating Vulkan devices.
 * @author Renato German Chavez Chicoma
 * @date Created on 27-12-23.
 */
#include "device.hpp"
#ifndef VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
#define VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME "VK_KHR_portability_subset"
#endif
/**
 * @brief Checks if a Vulkan physical device_ supports the required extensions.
 *
 * @param device The Vulkan physical device_ to check.
 * @param requested_extensions A vector of extension names to be checked.
 * @param debug Flag indicating whether to enable debug logging.
 * @return true if the device_ supports all requested extensions, false otherwise
 */
bool vkinit::CheckDeviceExtensionSupport(const vk::PhysicalDevice& device,
                                         const std::vector<const char*>& requested_extensions,
                                         const bool& debug) {
//    std::set<std::string> requiredExtensions(requested_extensions.begin(), requested_extensions.end());
    if (debug) {
        std::cout << std::format("Device can support extensions: \n");
    }
//
//    for (vk::ExtensionProperties& extension : device.enumerateDeviceExtensionProperties()) {
//        if (debug) {
//            std::cout << "\t\""<<extension.extensionName << "\"\n";
//        }
//        requiredExtensions.erase(extension.extensionName);
//    }
//    return requiredExtensions.empty();
    
    
//    std::vector<vk::ExtensionProperties> availableExtensions = device.enumerateDeviceExtensionProperties();
//    std::function<bool(const char*)> extensionSupported = [&](const char* extensionName) {
//        return std::ranges::any_of(availableExtensions, [&](const auto& ext) {
//            return std::strcmp(ext.extensionName, extensionName) == 0;
//        });
//    };
//    for (const auto& extension : requested_extensions) {
//        bool found = extensionSupported(extension);
//        if (debug) {
//            std::cout << std::format("\t\"{}\" is {}\n", extension, found ? "supported" : "NOT supported");
//        }
//        if (!found) return false;
//    }
//    return true;
    
    std::vector<vk::ExtensionProperties> availableExtensions = device.enumerateDeviceExtensionProperties();
    for (const char* requested_extension : requested_extensions) {
        bool found = std::ranges::any_of(availableExtensions, [&](const vk::ExtensionProperties& ext) {
            return std::strcmp(ext.extensionName, requested_extension) == 0;
        });

        if (debug) {
            std::cout << std::format("\t\"{}\" is {}\n", requested_extension, found ? "supported" : "NOT supported");
        }

        if (!found) return false;
    }
    return true;
}
/**
 * @brief Determines if a Vulkan physical device_ is suitable for the application's needs.
 *
 * @param device The vulkan physical device_ to evaluate.
 * @param debug Flag indicating whether to enable debug logging.
 * @return true if the device_ is suitable, false otherwise.
 */
bool vkinit::IsSuitable(const vk::PhysicalDevice& device, 
                        const bool debug){
    if (debug) {
        std::cout << std::format("Checking if device_ is suitable\n");
    }
    const std::vector<const char*> requestedExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    if (debug) {
        std::cout << "We are requesting device_ extensions:\n";
        for(const char* extension : requestedExtensions) {
            std::cout << "\t\"" << extension << "\"\n";
        }
    }
    if (bool extensionSupported = CheckDeviceExtensionSupport(device,
                                                              requestedExtensions,
                                                              debug)) {
        if (debug && extensionSupported) {
            std::cout << std::format("Device can support the requested extensions!\n");
        }
    }
    else {
        if(debug) {
            std::cout << std::format("Device can't support the requested extensions!\n");
        }
        return false;
    }
    return true;
}
/**
 * @brief Chooses a suitable Vulkan physical device_ from available devices.
 *
 * @param instance The Vulkan instance_.
 * @param debug Flag indicating whether to enable debug logging.
 * @return The chosen Vulkan physical device_.
 */
std::optional<vk::PhysicalDevice> vkinit::ChoosePhysicalDevice(vk::Instance& instance,
                                                               bool debug){
    /**
     * Choose a suitable physical device_ from a list of candidates.
     * Note: Physical device_ are neither created nor destroyed, they exist
     * independently to the program.
     */
    if(debug){
        std::cout << std::format("Choosing physical device_...\n");
    }
    /**
     * ResultValueType<std::vector<PhysicalDevice, PhysicalDeviceAllocator>>::type
     * Instance::enumeratePhysicalDevices( Dispatch const & d )
     * std::vector<vk::PhysicalDevice> instance_.enumeratePhysicalDevices( Dispatch const & d = static/default )
     */
    std::vector<vk::PhysicalDevice>availableDevices = instance.enumeratePhysicalDevices();
    if(debug){
        std::cout << std::format("There are {} physical devices available on this system\n", availableDevices.size());
    }
    for(vk::PhysicalDevice device : availableDevices){
        if (debug) {
            log_device_properties(device);
        }
        if (IsSuitable(device, debug)) {
            return device;
        }
    }
    return std::nullopt;
}
/**
 * @brief Creates a Vulkan logical device_ from a physical device_.
 *
 * @param physical_device The Vulkan physical device_.
 * @param surface The Vulkan surface_.
 * @param debug The Vulkan surface_.
 * @return The created Vulkan logical device_.
 */
std::optional<vk::Device> vkinit::CreateLogicalDevice(vk::PhysicalDevice physical_device,
                                                      vk::SurfaceKHR surface,
                                                      bool debug){
    vkutil::QueueFamilyIndices indices = vkutil::findQueueFamilies(physical_device, 
                                                                   surface,
                                                                   debug);

    std::set<uint32_t> uniqueIndices = {indices.graphicsFamily.value(), indices.presentFamily.value()};
    float queuePriority = 1.0f;
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfo;
    for([[maybe_unused]] uint32_t queueFamilyIndex : uniqueIndices) {
        queueCreateInfo.emplace_back(vk::DeviceQueueCreateFlags(),
                                     indices.graphicsFamily.value(),
                                     1,
                                     &queuePriority);
    }


    std::array<const char*, 2> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME};
    std::function<bool(const std::array<const char*, 2>&)> extensionSupported = [&physical_device, &debug](const std::array<const char*, 2>& extensions) {
        std::vector<vk::ExtensionProperties> availableExtensions = physical_device.enumerateDeviceExtensionProperties();
        for (const char* extensionName : extensions) {
            bool found = std::ranges::any_of(availableExtensions.begin(), availableExtensions.end(),
                                             [extensionName](const vk::ExtensionProperties& ext) {
                                                 return std::strcmp(ext.extensionName, extensionName) == 0;
                                             });
            if (!found) {
                if (debug) {
                    std::cout << std::format("\t\"{}\" is NOT supported\n", extensionName);
                }
                return false;
            }
        }
        return true;
    };
    if (!extensionSupported(deviceExtensions)) {
        if (debug) std::cout << "Not all required device extensions are supported.\n";
        return std::nullopt;
    }
    
    vk::PhysicalDeviceFeatures deviceFeatures = vk::PhysicalDeviceFeatures();
    std::vector<const char*> enabledLayers;
    if(debug) {
        enabledLayers.push_back("VK_LAYER_KHRONOS_validation");
    }
    vk::DeviceCreateInfo deviceInfo = vk::DeviceCreateInfo(vk::DeviceCreateFlags(),
                                                           static_cast<uint32_t>(queueCreateInfo.size()),
                                                           queueCreateInfo.data(),
                                                           static_cast<uint32_t>(enabledLayers.size()),
                                                           enabledLayers.data(),
                                                           static_cast<uint32_t>(deviceExtensions.size()),
                                                           deviceExtensions.data(),
                                                           &deviceFeatures);
    try {
        vk::Device device = physical_device.createDevice(deviceInfo);
        if(debug) {
            std::cout << std::format("GPU has been successfully abstracted!\n");
        }
        return device;
    }
    catch(const vk::SystemError &err) {
        if (debug) std::cout << std::format("Device creation failed: {}\n", err.what());
        return std::nullopt;
    }
    return std::nullopt;
}
/**
 * @brief Retrieves graphics and presentation queues from a Vulkan device_.
 *
 * @param physical_device The Vulkan physical device_.
 * @param device The Vulkan logical device_.
 * @param surface The Vulkan surface_.
 * @param debug Flag indicating whether to enable debug logging.
 * @return An array containing the graphics and presentation queues.
 */
std::array<vk::Queue, 2> vkinit::GetQueues(vk::PhysicalDevice physical_device,
                                           vk::Device device,
                                           vk::SurfaceKHR surface,
                                           bool debug){
    vkutil::QueueFamilyIndices indices = vkutil::findQueueFamilies(physical_device, 
                                                                   surface,
                                                                   debug);
    return { {
        device.getQueue(indices.graphicsFamily.value(), 0),
        device.getQueue(indices.presentFamily.value(), 0)
    } };
}
