struct SoundPlayerSettings
{
    bool loop = false;
    bool playing_offset_enabled = false;
    float volume = 100.0f;
    float pitch = 1.0f;
    bool relative_to_listener = false;
    Vector3f position = {0.f, 0.f, 0.f};
    Vector3f direction = {0.f, 0.f, -1.f};
    float min_distance = 1.0f;
    float max_distance = 10000.0f;
    float attenuation = 1.0f;
    Vector3f velocity = {0.f, 0.f, 0.f};
    Time playing_offset = Time::Zero;
};

class SoundPlayer
{
private:
    std::vector<Sound> players;
    float master_volume = 100.0f;

    SoundPlayer()
    {
        players.assign(32, Sound(dummy_sound_buffer));
    }

    float ComputeVolume(float local) const 
    {
        return std::clamp(local * (master_volume / 100.0f), 0.0f, 100.0f);
    }

public:
    static SoundPlayer& GetInstance()
    {
        static SoundPlayer instance;
        return instance;
    }

    static void SetMasterVolume(float volume)
    {
        auto& instance = GetInstance();
        instance.master_volume = volume;
    }

    static void Play(const SoundBuffer& buffer, SoundPlayerSettings settings = {})
    {
        auto& instance = GetInstance();

        for (auto& player : instance.players)
        {
            if (player.getStatus() != Sound::Status::Stopped) continue;

            player.setBuffer(buffer);
            player.setLooping(settings.loop);
            player.setVolume(instance.ComputeVolume(settings.volume));
            player.setPitch(settings.pitch);
            player.setPosition(settings.position);
            player.setDirection(settings.direction);
            player.setRelativeToListener(settings.relative_to_listener);
            player.setMinDistance(settings.min_distance);
            player.setMaxDistance(settings.max_distance);
            player.setAttenuation(settings.attenuation);

            player.setVelocity(settings.velocity);

            if (settings.playing_offset_enabled)
                player.setPlayingOffset(settings.playing_offset);

            player.play();
            return;
        }
    }
};