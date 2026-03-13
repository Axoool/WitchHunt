#include "AudioSourceComponent.hpp"
#include "Asset/AssetUtils.hpp"
#include "Audio/AudioSystem.hpp"
#include "Core/Application.hpp"
#include "Core/Logger.hpp"
#include "Renderer/UIUtils.hpp"
#include "World/Components/Transform.hpp"
#include "World/Actor.hpp"
#include "Asset/AssetSystem.hpp"

#include <ImGui/imgui.h>
#include <GLM/gtc/type_ptr.hpp>

namespace Termina {
    AudioSourceComponent::AudioSourceComponent()
    {
        AudioSystem* system = Application::GetSystem<AudioSystem>();

        m_Source = new AudioSource(system->GetEngine());
    }

    AudioSourceComponent::~AudioSourceComponent()
    {
        delete m_Source;
    }

    void AudioSourceComponent::OnPlay()
    {
        if (!m_AudioAsset) return;

        m_Source->Play();
    }

    void AudioSourceComponent::OnStop()
    {
        if (!m_AudioAsset) return;

        m_Source->Stop();
        m_Source->Reset();
    }

    void AudioSourceComponent::OnUpdate(float deltaTime)
    {
        if (!m_AudioAsset) return;

        m_Source->SetVolume(m_Volume);
        m_Source->SetLooping(m_Loop);
        if (m_Spatialized) {
            Transform transform = m_Owner->GetComponent<Transform>();

            m_Source->SetSpatialization(true);
            m_Source->SetPosition(transform.GetPosition());
            m_Source->SetDirection(transform.GetForward());
            m_Source->SetVelocity(m_Velocity);
        } else {
            m_Source->SetSpatialization(false);
        }

        m_Source->Update();
    }

    void AudioSourceComponent::OnAssetChange(const std::string& path)
    {
        m_AudioAsset = Application::GetSystem<AssetSystem>()->Load<AudioAsset>(path);
        if (m_AudioAsset) {
            m_Source->SetData(m_AudioAsset->GetData());
        }
    }

    void AudioSourceComponent::Inspect()
    {
        UIUtils::TryReceiveAsset<AudioAsset>(m_AudioAsset, [&](const std::string& path) {
            if (AssetUtils::IsAssetType(path, AssetType::Audio)) {
                OnAssetChange(path);
            } else {
                TN_WARN("Invalid asset type: %s", path.c_str());
            }
        });
        ImGui::Separator();
        ImGui::Checkbox("Play on Awake", &m_PlayOnAwake);
        ImGui::Checkbox("Loop", &m_Loop);
        ImGui::Checkbox("Spatialized", &m_Spatialized);
        ImGui::DragFloat("Volume", &m_Volume, 0.1f, 0.0f);
        ImGui::InputFloat3("Velocity", glm::value_ptr(m_Velocity));
    }

    void AudioSourceComponent::Serialize(nlohmann::json& out) const
    {
        out["playOnAwake"] = m_PlayOnAwake;
        out["loop"] = m_Loop;
        out["spatialized"] = m_Spatialized;
        out["volume"] = m_Volume;
        out["velocity"] = { m_Velocity.x, m_Velocity.y, m_Velocity.z };
        out["asset"] = m_AudioAsset.IsValid() ? m_AudioAsset.GetPath() : "";
    }

    void AudioSourceComponent::Deserialize(const nlohmann::json& in)
    {
        m_PlayOnAwake = in.value("playOnAwake", false);
        m_Loop = in.value("loop", false);
        m_Spatialized = in.value("spatialized", false);
        m_Volume = in.value("volume", 1.0f);
        if (in.contains("velocity")) {
            m_Velocity = glm::vec3(in["velocity"][0], in["velocity"][1], in["velocity"][2]);
        }
        if (in.contains("asset") && !in["asset"].get<std::string>().empty()) {
            OnAssetChange(in["asset"].get<std::string>());
        }
    }
}
