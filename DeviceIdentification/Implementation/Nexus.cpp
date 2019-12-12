#include "../IdentityProvider.h"
#include "../Module.h"

#include <nexus_config.h>
#include <nexus_platform.h>
#include <nxclient.h>

#if NEXUS_SECURITY_API_VERSION == 2
#include <nexus_otp_key.h>
#else
#include <nexus_otpmsp.h>
#include <nexus_read_otp_id.h>
#endif

#if NEXUS_SECURITY_API_VERSION == 2

static class NexusId {
public:
    NexusId(const NexusId&) = delete;
    NexusId& operator= (const NexusId&) = delete;

    NexusId() : _length(0) {
    }
    ~NexusId() {
    }

    const unsigned char* Identifier(unsigned char* length) {
        if (_length == 0) {
        {
            if (NEXUS_SUCCESS == NxClient_Join(nullptr))
            {
                if (NEXUS_SUCCESS == NEXUS_OtpKey_GetInfo(0 /*key A*/, &_id)){
                    _length = NEXUS_OTP_KEY_ID_LENGTH;
                    if (length != nullptr) {
                        *length = _length;
                    }
                }
                else if (length != nullptr) {
                    *length = 2;
                }
                NxClient_Uninit();
            }
            else if (length != nullptr) {
                *length = 1;
            }
        }
        else if (length != nullptr) {
            *length = _length;
        }
        return (_length != 0 ? _id.id: nullptr);
    }
  
private: 
    unsigned char _length;
    NEXUS_OtpKeyInfo keyInfo;
} _identifier;

#else

static class NexusId {
public:
    NexusId(const NexusId&) = delete;
    NexusId& operator= (const NexusId&) = delete;

    NexusId() {
        _id.size = 0;
    }
    ~NexusId() {
    }

    const unsigned char* Identifier(unsigned char* length) {
        if (_id.size == 0)
        {
            if (NEXUS_SUCCESS == NxClient_Join(nullptr))
            {
                if (NEXUS_SUCCESS == NEXUS_Security_ReadOtpId(NEXUS_OtpIdType_eA, &_id)) {
                    if (length != nullptr) {
                        *length = static_cast<unsigned char>(_id.size);
                    }
                } else {
                    _id.size = 0;
                    if (length != nullptr) {
                        *length = 2;
                    }
                }
                NxClient_Uninit();
            }
            else if (length != nullptr) {
                *length = 1;
            }
        }
        else if (length != nullptr) {
            *length = static_cast<unsigned char>(_id.size);
        }
        return (_id.size != 0 ? _id.otpId : nullptr);
    }
  
private: 
    NEXUS_OtpIdOutput _id;
} _identifier;

#endif

static class NexusDeviceInfo {
public:
    NexusDeviceInfo(const NexusDeviceInfo&) = delete;
    NexusDeviceInfo& operator= (const NexusDeviceInfo&) = delete;

    NexusDeviceInfo()
    : _firmware()
    , _chipset()
    {}

    ~NexusDeviceInfo() {}

    const std::string FirmwareVersion()
    {
        if(_firmware.empty()){
            _firmware = "Nexus " + std::to_string(NEXUS_PLATFORM_VERSION_MAJOR) + "." + std::to_string(NEXUS_PLATFORM_VERSION_MINOR);
        }
        
        return _firmware;
    };

    const std::string Chipset()
    {
        if(_chipset.empty()){
            NxClient_Join(nullptr);

            NEXUS_PlatformStatus info;
            NEXUS_Platform_GetStatus(&info);

            // Get chipset name
            char chipsetName[62];
            ::snprintf(chipsetName, sizeof(chipsetName), "%x%c%d", info.chipId, 'A' + ((info.chipRevision >> 8) & 0xf)
                    , (info.chipRevision & 0xf));
            _chipset = chipsetName;
   
            NxClient_Uninit();
        }
        return _chipset;
    };
    
private:
    std::string _chipset;
    std::string _firmware;

} _nexusDeviceInfo;

const char* GetFirmwareVersion()
{
    return (_nexusDeviceInfo.FirmwareVersion().c_str());
}

const char* GetChipset()
{
    return (_nexusDeviceInfo.Chipset().c_str());
}

const unsigned char* GetIdentity(unsigned char* length)
{
    return (_identifier.Identifier(length));
}
