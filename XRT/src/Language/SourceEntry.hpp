#pragma once

#include <filesystem>
namespace XRT {

    class SourceEntry {
    public:
        SourceEntry(const std::wstring& content, const std::filesystem::path& path) : m_Content(content), m_Path(path) {
        }

        const std::wstring_view GetContent() const {
            return m_Content;
        }

        const std::filesystem::path& GetPath() const {
            return m_Path;
        }

    private:
        std::wstring m_Content;
        std::filesystem::path m_Path;
    };

} // namespace XRT