#ifndef DeviceIdentification_DeviceIdentification_H
#define DeviceIdentification_DeviceIdentification_H

#include "Module.h"

#include <interfaces/IDeviceInfo.h>

namespace WPEFramework {
namespace Plugin {

    class DeviceIdentification : public PluginHost::IPlugin, public PluginHost::ISubSystem::IIdentifier, public Exchange::IDeviceInfo  {
    public:
        DeviceIdentification(const DeviceIdentification&) = delete;
        DeviceIdentification& operator=(const DeviceIdentification&) = delete;

        DeviceIdentification()
        {
        }

        virtual ~DeviceIdentification()
        {
        }

        BEGIN_INTERFACE_MAP(DeviceIdentification)
            INTERFACE_ENTRY(PluginHost::IPlugin)
            INTERFACE_ENTRY(PluginHost::ISubSystem::IIdentifier)
            INTERFACE_ENTRY(Exchange::IDeviceInfo)
        END_INTERFACE_MAP

    public:
        //   IPlugin methods
        // -------------------------------------------------------------------------------------------------------
        virtual const string Initialize(PluginHost::IShell* service) override;
        virtual void Deinitialize(PluginHost::IShell* service) override;
        virtual string Information() const override;

        //   IIdentifier methods
        // -------------------------------------------------------------------------------------------------------
        virtual uint8_t Identifier(const uint8_t length, uint8_t buffer[]) const override;

        //   IDeviceInfo methods
        // -------------------------------------------------------------------------------------------------------
        virtual const std::string FirmwareVersion() const override;
        virtual const std::string Chipset() const override;
    };

} // namespace Plugin
} // namespace WPEFramework

#endif // DeviceIdentification_DeviceIdentification_H
