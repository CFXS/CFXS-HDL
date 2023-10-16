#pragma once
#include <Utils.hpp>
#include <string_view>
#include <vector>
#include "Language/AST.hpp"
#include "StringUtils.hpp"
#include "_ParserTypeBase.hpp"

namespace XRT {

    enum class AST_Type {
        SOURCE_LINK,
        NAMESPACE,
        SCOPE,
        EXPRESSION,
    };

    static inline const char* AST_Type_ToString(AST_Type type) {
        switch (type) {
            case AST_Type::SOURCE_LINK: return "SOURCE_LINK";
            case AST_Type::NAMESPACE: return "NAMESPACE";
            case AST_Type::SCOPE: return "SCOPE";
            case AST_Type::EXPRESSION: return "EXPRESSION";
            default: return "???";
        }
    }

    struct AST_Entry {
        AST_Entry(AST_Type t) : type(t) {
        }
        virtual ~AST_Entry() = default;

        template<typename T>
        T& Cast() {
            return *static_cast<T*>(static_cast<void*>(this));
        }

        AST_Type type;
    };

    namespace AST_Element {
        struct SourceLink : public AST_Entry {
            SourceLink(const std::wstring_view& path) : AST_Entry(AST_Type::SOURCE_LINK), source_path(path) {
            }
            std::wstring_view source_path;
        };

        struct Namespace : public AST_Entry {
            Namespace(const std::wstring_view& ns) : AST_Entry(AST_Type::NAMESPACE), name(ns) {
            }
            std::wstring_view name;
        };
    }; // namespace AST_Element

    class AST {
    public:
        AST() = default;

        void Append(Scope<AST_Entry>&& el) {
            m_Entries.emplace_back(std::move(el));
        }

        void Print() const {
            LOG_DEBUG("AST:");
            for (auto& e : m_Entries) {
                LOG_DEBUG("[{}]", AST_Type_ToString(e->type));
                switch (e->type) {
                    case AST_Type::SOURCE_LINK: {
                        LOG_DEBUG("    - {}", StringUtils::utf16_to_utf8(e->Cast<AST_Element::SourceLink>().source_path));
                        break;
                    }
                    case AST_Type::NAMESPACE: {
                        LOG_DEBUG("    - {}", StringUtils::utf16_to_utf8(e->Cast<AST_Element::Namespace>().name));
                        break;
                    }
                    default: break;
                }
            }
        }

    private:
        std::vector<Scope<AST_Entry>> m_Entries;
    };

} // namespace XRT