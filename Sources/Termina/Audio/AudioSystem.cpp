#include "AudioSystem.hpp"

#include <Termina/Core/Logger.hpp>

namespace Termina {
    void DataCallback(ma_device *pDevice, void *output, const void *input, ma_uint32 frameCount)
    {
        ma_engine* engine = (ma_engine*)pDevice->pUserData;

        if (engine->pResourceManager != nullptr) {
            if ((engine->pResourceManager->config.flags & MA_RESOURCE_MANAGER_FLAG_NO_THREADING) != 0) {
                ma_resource_manager_process_next_job(engine->pResourceManager);
            }
        }
        ma_engine_read_pcm_frames(engine, output, frameCount, nullptr);
    }

    AudioSystem::AudioSystem()
    {
        ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
        deviceConfig.playback.format = ma_format_f32;
        deviceConfig.playback.channels = 2;
        deviceConfig.sampleRate = 48000;
        deviceConfig.dataCallback = DataCallback;
        deviceConfig.pUserData = &m_Engine;

        ma_result result = ma_device_init(nullptr, &deviceConfig, &m_Device);
        if (result != MA_SUCCESS) {
            TN_FATAL("Failed to initialize audio device!");
        }

        ma_engine_config engineConfig = ma_engine_config_init();
        engineConfig.pDevice = &m_Device;
        engineConfig.listenerCount = 1;

        result = ma_engine_init(&engineConfig, &m_Engine);
        if (result != MA_SUCCESS) {
            TN_FATAL("Failed to initialize audio engine!");
        }

        char name[512];
        ma_device_get_name(&m_Device, ma_device_type_playback, name, 512, nullptr);
        TN_INFO("Audio OK. Using device %s", name);
    }

    AudioSystem::~AudioSystem()
    {
        ma_engine_uninit(&m_Engine);
        ma_device_uninit(&m_Device);
    }

    void AudioSystem::RegisterComponents()
    {
        // TODO
    }

    void AudioSystem::UnregisterComponents()
    {
        // TODO
    }
