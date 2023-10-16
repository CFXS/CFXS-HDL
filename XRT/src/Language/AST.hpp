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
        SCOPE_START,
        SCOPE_END,
    };

    static inline const char* AST_Type_ToString(AST_Type type) {
        switch (type) {
            case AST_Type::SOURCE_LINK: return "SOURCE_LINK";
            case AST_Type::NAMESPACE: return "NAMESPACE";
            case AST_Type::SCOPE: return "SCOPE";
            case AST_Type::EXPRESSION: return "EXPRESSION";
            case AST_Type::SCOPE_START: return "SCOPE_START";
            case AST_Type::SCOPE_END: return "SCOPE_END";
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
            Namespace(const std::wstring& ns) : AST_Entry(AST_Type::NAMESPACE), name(ns) {
            }
            std::wstring name;
        };

        struct ScopeStart : public AST_Entry {
            ScopeStart() : AST_Entry(AST_Type::SCOPE_START) {
            }
        };

        struct ScopeEnd : public AST_Entry {
            ScopeEnd() : AST_Entry(AST_Type::SCOPE_END) {
            }
        };
    }; // namespace AST_Element

    class AST {
    public:
        AST() = default;

        void Append(Scope<AST_Entry>&& el) {
            m_Entries.emplace_back(std::move(el));
        }

        void EnterScope() {
            m_Entries.emplace_back(CreateScope<AST_Element::ScopeStart>());
        }

        void ExitScope() {
            m_Entries.emplace_back(CreateScope<AST_Element::ScopeEnd>());
        }

        void Print() const {
            LOG_DEBUG("AST:");

            int scope_depth             = 0;
            static constexpr int INDENT = 4;

            auto str_indent = [](int len) {
                std::string s;
                for (int i = 0; i < len; i++) {
                    s.append(" ");
                }
                return s;
            };

            for (auto& e : m_Entries) {
                if (e->type == AST_Type::SCOPE_END) {
                    scope_depth--;
                }

                LOG_TRACE("{}[{}]", str_indent(scope_depth * INDENT), AST_Type_ToString(e->type));
                switch (e->type) {
                    case AST_Type::SOURCE_LINK: {
                        LOG_DEBUG("{}path: {}",
                                  str_indent(scope_depth * INDENT),
                                  StringUtils::utf16_to_utf8(e->Cast<AST_Element::SourceLink>().source_path));
                        break;
                    }
                    case AST_Type::NAMESPACE: {
                        LOG_DEBUG("{}name: {}",
                                  str_indent(scope_depth * INDENT),
                                  StringUtils::utf16_to_utf8(e->Cast<AST_Element::Namespace>().name));
                        break;
                    }
                    default: break;
                }

                if (e->type == AST_Type::SCOPE_START) {
                    scope_depth++;
                }
            }
        }

    private:
        std::vector<Scope<AST_Entry>> m_Entries;
    };

} // namespace XRT