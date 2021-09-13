#include "json_builder.h"
using namespace std;
namespace json {

    BaseContext::BaseContext(Builder& builder) : builder_(builder) {}
    StartContext::StartContext(Builder& builder) : BaseContext(builder) {}

    MapContext& StartContext::StartDict()
    {
        return builder_.StartDict();
    }

    ArrayContext& StartContext::StartArray()
    {
        return builder_.StartArray();
    }

    EndContext::EndContext(Builder& builder) : BaseContext(builder) {}

    KeyContext& EndContext::Key(std::string key)
    {
        return builder_.Key(key);
    }

    Builder& EndContext::EndDict()
    {
        return builder_.EndDict();
    }

    KeyContext::KeyContext(Builder& builder) : StartContext(builder) {}

    ValueAfterKeyContext& KeyContext::Value(Node::Value value)
    {
        return builder_.Value(value);
    }

    ValueAfterKeyContext::ValueAfterKeyContext(Builder& builder) : EndContext(builder) {}
    ValueAfterArrayContext::ValueAfterArrayContext(Builder& builder) : StartContext(builder) {}

    ValueAfterArrayContext& ValueAfterArrayContext::Value(Node::Value value)
    {
        return builder_.Value(value);
    }

    Builder& ValueAfterArrayContext::EndArray()
    {
        return builder_.EndArray();
    }

    MapContext::MapContext(Builder& builder) : EndContext(builder) {}

    ArrayContext::ArrayContext(Builder& builder) : ValueAfterArrayContext(builder) {}
    KeyContext& Builder::Key(string key) {
        if (CanTApplyKey()) {
            throw logic_error("Key ñan't be applied");
        }
        nodes_.push(make_unique<Node>(key));
        return *this;
    }

    Builder& Builder::Value(Node::Value value) {
        if (CanTApplyValue()) {
            throw std::logic_error("Value ñan't be applied");
        }
        PushNode(value);
        return AddNode(*nodes_.top().release());
    }

    MapContext& Builder::StartDict() {
        if (CanTApplyStartDict()) {
            throw logic_error("StartDict ñan't be applied");
        }
        nodes_.push(make_unique<Node>(Dict()));
        return *this;
    }

    Builder& Builder::EndDict() {
        if (CanTApplyEndDict()) {
            throw logic_error("EndDict ñan't be applied");
        }
        return AddNode(*nodes_.top().release());
    }

    ArrayContext& Builder::StartArray() {
        if (CanTApplyStartArray()) {
            throw logic_error("StartArray ñan't be applied");
        }
        nodes_.push(make_unique<Node>(Array()));
        return *this;
    }

    Builder& Builder::EndArray() {
        if (CanTApplyEndArray()) {
            throw logic_error("EndArray ñan't be applied");
        }
        return AddNode(*nodes_.top().release());
    }

    Node Builder::Build() const {
        if (CanTApplyBuild()) {
            throw logic_error("Builder ñan't be applied");
        }
        return root_;
    }

    bool Builder::CanTAdd() const {
        return !(nodes_.empty()
            || nodes_.top()->IsArray()
            || nodes_.top()->IsString());
    }

    bool Builder::IsMakeNode() const {
        return !root_.IsNull();
    }

    bool Builder::CanTApplyKey() const {
        return (IsMakeNode() || nodes_.empty() || !nodes_.top()->IsMap());
    }

    bool Builder::CanTApplyValue() const {
        return (IsMakeNode() || CanTAdd());
    }

    bool Builder::CanTApplyStartDict() const {
        return CanTApplyValue();
    }

    bool Builder::CanTApplyEndDict() const {
        return (IsMakeNode() || nodes_.empty() || !nodes_.top()->IsMap());
    }

    bool Builder::CanTApplyStartArray() const {
        return CanTApplyValue();
    }

    bool Builder::CanTApplyEndArray() const {
        return (IsMakeNode() || nodes_.empty() || !nodes_.top()->IsArray());
    }

    bool Builder::CanTApplyBuild() const {
        return !IsMakeNode();
    }

    Builder& Builder::AddNode(const Node& node) {
        nodes_.pop();
        if (nodes_.empty()) {
            root_ = node;
        }
        else if (nodes_.top()->IsArray()) {
            (nodes_.top()->AsArray()).push_back(node);
        }
        else {
            const Node& key = *nodes_.top().release();
            nodes_.pop();
            (nodes_.top()->AsMap()).emplace(key.AsString(), node);
        }
        return *this;
    }

    void Builder::PushNode(Node::Value value) {
        visit([this](auto&& val) {nodes_.push(make_unique<Node>(val)); }, value);
    }
}

