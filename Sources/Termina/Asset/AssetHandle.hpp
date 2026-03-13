#pragma once

#include <Termina/Core/Common.hpp>
#include <string>

namespace Termina {

    class AssetSystem;

    /// RAII handle to a ref-counted asset managed by AssetSystem.
    /// Copying increments the ref count; destruction decrements it.
    /// The asset is never freed through the handle — call AssetSystem::Clean() and
    /// AssetSystem::ProcessPendingDeletions() to evict unused assets.
    template<typename T>
    class AssetHandle
    {
    public:
        AssetHandle() = default;
        ~AssetHandle() { Reset(); }

        AssetHandle(const AssetHandle& other);
        AssetHandle& operator=(const AssetHandle& other);

        AssetHandle(AssetHandle&& other) noexcept;
        AssetHandle& operator=(AssetHandle&& other) noexcept;

        /// Returns the underlying asset pointer, or nullptr if invalid.
        T* Get() const;

        T*   operator->() const { return Get(); }
        T&   operator*()  const { return *Get(); }
        bool IsValid()    const { return m_ID != 0 && m_System != nullptr; }
        explicit operator bool() const { return IsValid(); }

        const std::string& GetPath() const { return m_Path; }

    private:
        friend class AssetSystem;

        AssetHandle(uint32 id, AssetSystem* system, std::string path)
            : m_ID(id), m_System(system), m_Path(std::move(path))
        {
        }

        void IncRef();
        void Reset();

        uint32       m_ID     = 0;
        AssetSystem* m_System = nullptr;
        std::string  m_Path;
    };

} // namespace Termina
