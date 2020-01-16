#include "../Module.h"
#include "../VolumeControlPlatform.h"

#include <alsa/asoundlib.h>

namespace WPEFramework {
namespace Plugin {

namespace {

constexpr uint8_t kMinVolume = 0;
constexpr uint8_t kMaxVolume = 100;

class VolumeControlPlatformRPI : public VolumeControlPlatform {
public:
    VolumeControlPlatformRPI()
    {
        snd_mixer_open(&_handle, 0);
        if (_handle != nullptr) {
          snd_mixer_attach(_handle, "default");
          snd_mixer_selem_id_t* sid{nullptr};
          snd_mixer_selem_register(_handle, nullptr, nullptr);
          snd_mixer_load(_handle);
          snd_mixer_selem_id_alloca(&sid);
          snd_mixer_selem_id_set_index(sid, 0);
          snd_mixer_selem_id_set_name(sid, "PCM");
          _mixer = snd_mixer_find_selem(_handle, sid);
        }
    }

    ~VolumeControlPlatformRPI() override
    {
      snd_mixer_close(_handle);
    }

    uint32_t Muted(bool muted) override
    {
        return _mixer != nullptr &&
            snd_mixer_selem_set_playback_switch_all(_mixer, muted ? 0 : 1) >= 0 ?
            Core::ERROR_NONE : Core::ERROR_GENERAL;
    }

    bool Muted() const override
    {
        int val = 1;
        if (_mixer != nullptr)
          snd_mixer_selem_get_playback_switch(_mixer, SND_MIXER_SCHN_FRONT_LEFT, &val);
        return val == 0;
    }

    uint32_t Volume(uint8_t volume) override
    {
        uint32_t result = Core::ERROR_GENERAL;
        long min = 0;
        long max = 0;
        if (_mixer != nullptr && snd_mixer_selem_get_playback_volume_range(_mixer, &min, &max) >= 0) {
          fprintf(stderr, "SETTER snd_mixer_selem_get_playback_volume_range: %ld %ld\n", min, max);
          long scaled = volume * (max - min) * 100 / (kMaxVolume - kMinVolume) / 100;
          fprintf(stderr, "SCALED: %ld\n", scaled);
          result = snd_mixer_selem_set_playback_volume_all(_mixer, min + scaled) >= 0 ?
                  Core::ERROR_NONE : Core::ERROR_GENERAL;
        }

        return result;
    }

    uint8_t Volume() const override
    {
        uint8_t vol = 100;
        long min = 0;
        long max = 0;
        if (_mixer != nullptr && snd_mixer_selem_get_playback_volume_range(_mixer, &min, &max) >= 0) {
          long value = 100;
          fprintf(stderr, "GETTER snd_mixer_selem_get_playback_volume_range: %ld %ld\n", min, max);
          if (snd_mixer_selem_get_playback_volume(_mixer, SND_MIXER_SCHN_FRONT_LEFT, &value) >= 0) {
            vol = (value - min) * (kMaxVolume - kMinVolume) * 100 / (max - min) / 100;
            fprintf(stderr, "snd_mixer_selem_get_playback_volume: %ld\n", value);
            fprintf(stderr, "--> VOL: %d\n", vol);
          }
        }
        return vol;
    }

private:
    snd_mixer_t* _handle;
    snd_mixer_elem_t* _mixer;
};

}

// static
std::unique_ptr<VolumeControlPlatform> VolumeControlPlatform::Create(
    VolumeControlPlatform::VolumeChangedCallback&& volumeChanged,
    VolumeControlPlatform::MutedChangedCallback&& mutedChanged)
{
    return std::unique_ptr<VolumeControlPlatform>{new VolumeControlPlatformRPI};
}

}  // namespace Plugin
}  // namespace WPEFramework
