#pragma once

#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <stdexcept>
#include <regex>

namespace json {
    using namespace std::literals;

    class Node;

    using Number = std::variant<int, double>;
    using Array = std::vector<Node>;
    using Dict = std::map<std::string, Node>;

    // Эта ошибка должна выбрасываться при ошибках парсинга JSON
    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Node {
    public:
        Node() = default;

        template<typename T>
        Node(T node) : node_(std::move(node)) {}

        bool operator==(const Node& other) const {
            return node_ == other.node_;
        }

        bool operator!=(const Node& other) const {
            return !(*this == other);
        }

        bool IsInt() const {
            try {
                int var = std::get<int>(node_);
                (void)&var;
                return true;
            }
            catch (std::bad_variant_access&) {
                return false;
            }
        }

        int AsInt() const {
            try {
                return std::get<int>(node_);
            }
            catch (std::bad_variant_access&) {
                throw std::logic_error("Logic error");
            }
        }

        bool IsDouble() const {
            try {
                if (IsInt()) {
                    return true;
                }
                double var = std::get<double>(node_);
                (void)&var;
                return true;
            }
            catch (std::bad_variant_access&) {
                try {
                    double var = std::get<double>(node_);
                    (void)&var;
                    return true;
                }
                catch (std::bad_variant_access&) {
                    return false;
                }
            }
        }

        double AsDouble() const {
            try {
                if (IsPureDouble()) {
                    return std::get<double>(node_);
                }
                return AsInt() + 0.0;
            }
            catch (std::bad_variant_access&) {
                throw std::logic_error("Logic error");
            }
        }

        bool IsPureDouble() const {
            try {
                return (std::to_string(std::get<double>(node_)).find('.') != std::string::npos) ? true : false;
            }
            catch (std::bad_variant_access&) {
                return false;
            }
        }

        bool IsNull() const {
            try {
                auto var = std::get<std::nullptr_t>(node_);
                (void)&var;
                return true;
            }
            catch (std::bad_variant_access&) {
                return false;
            }
        }

        bool IsString() const {
            try {
                std::string var = std::get<std::string>(node_);
                (void)&var;
                return true;
            }
            catch (std::bad_variant_access&) {
                return false;
            }
        }

        std::string AsString() const {
            try {
                return std::get<std::string>(node_);
            }
            catch (std::bad_variant_access&) {
                throw std::logic_error("Logic error");
            }
        }

        bool IsBool() const {
            try {
                auto var = std::get<bool>(node_);
                (void)&var;
                return true;
            }
            catch (std::bad_variant_access&) {
                return false;
            }
        }

        bool AsBool() const {
            try {
                return std::get<bool>(node_);
            }
            catch (std::bad_variant_access&) {
                throw std::logic_error("Logic error");
            }
        }

        bool IsArray() const {
            try {
                auto var = std::get<Array>(node_);
                (void)&var;
                return true;
            }
            catch (std::bad_variant_access&) {
                return false;
            }
        }

        Array AsArray() const {
            try {
                return std::get<Array>(node_);
            }
            catch (std::bad_variant_access&) {
                throw std::logic_error("Logic error");
            }
        }

        bool IsMap() const {
            try {
                auto var = std::get<Dict>(node_);
                (void)&var;
                return true;
            }
            catch (std::bad_variant_access&) {
                return false;
            }
        }

        Dict AsMap() const {
            try {
                return std::get<Dict>(node_);
            }
            catch (std::bad_variant_access&) {
                throw std::logic_error("Logic error");
            }
        }

        std::size_t GetType() const {
            return node_.index();
        }

        Array GetArrayValue() const {
            try {
                return std::get<Array>(node_);
            }
            catch (std::bad_variant_access&) {
                throw std::logic_error("Logic error");
            }
        }

        Dict GetDictValue() const {
            try {
                return std::get<Dict>(node_);
            }
            catch (std::bad_variant_access&) {
                throw std::logic_error("Logic error");
            }
        }

        bool GetBoolValue() const {
            try {
                return std::get<bool>(node_);
            }
            catch (std::bad_variant_access&) {
                throw std::logic_error("Logic error");
            }
        }

        int GetIntValue() const {
            try {
                return std::get<int>(node_);
            }
            catch (std::bad_variant_access&) {
                throw std::logic_error("Logic error");
            }
        }

        double GetDoubleValue() const {
            try {
                return std::get<double>(node_);
            }
            catch (std::bad_variant_access&) {
                throw std::logic_error("Logic error");
            }
        }

        std::string GetStringValue() const {
            try {
                return std::get<std::string>(node_);
            }
            catch (std::bad_variant_access&) {
                throw std::logic_error("Logic error");
            }
        }

    private:
        std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string> node_;
    };

    class Document {
    public:
        explicit Document(Node root);

        inline bool operator==(const Document& other) const {
            return root_ == other.root_;
        }

        inline bool operator!=(const Document& other) const {
            return !(*this == other);
        }

        const Node& GetRoot() const;

    private:
        Node root_;
    };

    Document Load(std::istream& input);

    void Print(const Document& doc, std::ostream& output);

}  // namespace json