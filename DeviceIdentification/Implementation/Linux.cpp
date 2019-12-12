#include "../Module.h"

static class LinuxDeviceInfo {
public:
    LinuxDeviceInfo(const LinuxDeviceInfo&) = delete;
    LinuxDeviceInfo& operator= (const LinuxDeviceInfo&) = delete;

    LinuxDeviceInfo()
    : _firmwareVersion()
    , _chipset()
    {}

    ~LinuxDeviceInfo() {}

    const std::string FirmwareVersion()
    {
        if(_firmwareVersion.empty())
        {
            _firmwareVersion = ExecuteCmd("uname -rs");
        }

        return _firmwareVersion;
    };

    const std::string Chipset()
    {
        if(_chipset.empty())
        {
            _chipset = ExecuteCmd("grep '^model name' /proc/cpuinfo | cut -d':' -f 2 | awk '{$1=$1};1' | sort -u");
        }
        
        return _chipset;
    };

private: 
    const std::string ExecuteCmd(const std::string command)
    {
        FILE *cmd = popen(command.c_str(), "rb");

        if (cmd == NULL){
            return std::string();
        }

        size_t n;
        char buffer[80];

        if ((n = fread(buffer, 1, sizeof(buffer)-1, cmd)) <= 0){
            return std::string();
        }

        pclose(cmd);

        buffer[n] = '\0';

        return std::string(buffer);
    }
    
private:
    std::string _firmwareVersion;
    std::string _chipset;

} _linuxDeviceInfo;

const char* GetFirmwareVersion()
{
    return (_linuxDeviceInfo.FirmwareVersion().c_str());
}

const char* GetChipset()
{
    return (_linuxDeviceInfo.Chipset().c_str());
}

const unsigned char* GetIdentity(unsigned char* length)
{
    return nullptr;
}
