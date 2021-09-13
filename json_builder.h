#pragma once

#include <memory>
#include <stack>
#include <string>

#include "json.h"

namespace json {
    class Builder;
    class BaseContext;
    class StartContext;
    class EndContext;
    class KeyContext;
    class ValueAfterKeyContext;
    class ValueAfterArrayContext;
    class MapContext;
    class ArrayContext;

    class BaseContext {
    public:
        explicit BaseContext(Builder& builder);
    protected:
        Builder& builder_;
    };
    class StartContext : public BaseContext {
    public:
        explicit StartContext(Builder& builder);
        MapContext& StartDict();
        ArrayContext& StartArray();
    };
    class EndContext : public BaseContext {
    public:
        explicit EndContext(Builder& builder);
        KeyContext& Key(std::string key);
        Builder& EndDict();
    };
    class KeyContext : public StartContext {
    public:
        explicit KeyContext(Builder& builder);
        ValueAfterKeyContext& Value(Node::Value value);
    };
    class ValueAfterKeyContext : public EndContext {
    public:
        explicit ValueAfterKeyContext(Builder& builder);
    };
    class ValueAfterArrayContext : public StartContext {
    public:
        explicit ValueAfterArrayContext(Builder& builder);
        ValueAfterArrayContext& Value(Node::Value value);
        Builder& EndArray();
    };
    class MapContext : public EndContext {
    public:
        explicit MapContext(Builder& builder);
    };
    class ArrayContext : public ValueAfterArrayContext {
    public:
        explicit ArrayContext(Builder& builder);
    };

    class Builder final : virtual public KeyContext, virtual public ValueAfterKeyContext,
        virtual public MapContext, virtual public ArrayContext {
    public:
        Builder() : KeyContext(*this), ValueAfterKeyContext(*this),
            MapContext(*this), ArrayContext(*this) {}

        KeyContext& Key(std::string key);
        Builder& Value(Node::Value value);
        MapContext& StartDict();
        Builder& EndDict();
        ArrayContext& StartArray();
        Builder& EndArray();
        Node Build() const;

    private:
        bool CanTAdd() const;
        bool IsMakeNode() const;
        bool CanTApplyKey() const;
        bool CanTApplyValue() const;
        bool CanTApplyStartDict() const;
        bool CanTApplyEndDict() const;
        bool CanTApplyStartArray() const;
        bool CanTApplyEndArray() const;
        bool CanTApplyBuild() const;

        Builder& AddNode(const Node& node);
        void PushNode(Node::Value value);

    private:
        Node root_ = nullptr;
        std::stack<std::unique_ptr<Node>> nodes_;
    };

}
