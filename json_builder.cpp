#include "json_builder.h"

json::KeyItemContext json::Builder::Key(const std::string& key) {
    if (nodes_stack_.empty()) {
        throw std::logic_error("Stack empty at Key (Dict is expected)");
    }
    if (!nodes_stack_.back()->IsDict()) {
        throw std::logic_error("Dict expected before Key");
    }
    if (nodes_stack_.back()->IsDict()) {
        Dict* current_node = reinterpret_cast<Dict*>(nodes_stack_.back());
        current_node->emplace(key, json::Node{});
        nodes_stack_.push_back(&current_node->at(key));
        return { *this };
    }
    return { *this };
}

json::Builder& json::Builder::Value(json::Node value) {
    if (root_ == nullptr) {
        root_ = value;
        return *this;
    }
    if (nodes_stack_.empty()) {
        throw std::logic_error("Stack empty at Value (Array or Key expected)");
    }

    if (nodes_stack_.back()->IsArray()) {
        Array* current_node = reinterpret_cast<Array*>(nodes_stack_.back());
        current_node->push_back(value);
        return *this;
    }
    if (nodes_stack_.back()->IsDict()) {
        throw std::logic_error("Key is expected before Value");
    }
    *nodes_stack_.back() = value;
    nodes_stack_.pop_back();
    return *this;
}

json::DictItemContext json::Builder::StartDict() {
    if (root_ == nullptr) {
        root_ = json::Dict{};
        nodes_stack_.emplace_back(&root_);
        return { *this };
    }
    if (nodes_stack_.empty()) {
        throw std::logic_error("Stack empty at Dict (Array or Key expected)");
    }
    if (nodes_stack_.back()->IsDict()) {
        throw std::logic_error("Key is expected before Value");
    }
    if (nodes_stack_.back()->IsArray()) {
        Array* current_node = reinterpret_cast<Array*>(nodes_stack_.back());
        current_node->push_back(json::Dict{});
        nodes_stack_.emplace_back(&current_node->back());
        return { *this };
    }
    *nodes_stack_.back() = json::Dict{};
    return { *this };
}

json::ArrayItemContext json::Builder::StartArray() {
    if (root_ == nullptr) {
        root_ = json::Array{};
        nodes_stack_.emplace_back(&root_);
        return { *this };
    }
    if (nodes_stack_.empty()) {
        throw std::logic_error("Stack empty at Array (Array or Key expected)");
    }
    if (nodes_stack_.back()->IsDict()) {
        throw std::logic_error("Key is expected before (Key is expected before Array");
    }
    if (nodes_stack_.back()->IsArray()) {
        Array* current_node = reinterpret_cast<Array*>(nodes_stack_.back());
        current_node->push_back(json::Array{});
        nodes_stack_.emplace_back(&current_node->back());
        return { *this };
    }
    *nodes_stack_.back() = json::Array{};

    return { *this };
}

json::Builder& json::Builder::EndDict() {
    if (nodes_stack_.empty()) {
        throw std::logic_error("Stack empty at Value (Dict is expected)");
    }
    if (!nodes_stack_.back()->IsDict()) {
        throw std::logic_error("Dict is expected before EndDict");
    }
    nodes_stack_.pop_back();
    return *this;
}

json::Builder& json::Builder::EndArray() {
    if (nodes_stack_.empty()) {
        throw std::logic_error("Stack empty at Value (Array is expected)");
    }
    if (!nodes_stack_.back()->IsArray()) {
        throw std::logic_error("Array is expected before EndArray");
    }
    nodes_stack_.pop_back();
    return *this;
}

json::Node json::Builder::Build() {
    if (!nodes_stack_.empty()) {
        throw std::logic_error("Stack not empty");
    }
    if (root_.IsNull()) {
        throw std::logic_error("Root is null");
    }
    return root_;
}

json::KeyItemContext json::BaseContext::Key(std::string key) {
    return builder_.Key(key);
}

json::Builder& json::BaseContext::Value(json::Node value) {
    return builder_.Value(value);
}

json::DictItemContext json::BaseContext::StartDict() {
    return builder_.StartDict();
}

json::ArrayItemContext json::BaseContext::StartArray() {
    return builder_.StartArray();
}

json::Builder& json::BaseContext::EndDict() {
    return builder_.EndDict();
}

json::Builder& json::BaseContext::EndArray() {
    return builder_.EndArray();
}

json::Node json::BaseContext::Build() {
    return builder_.Build();
}

json::ValueContextAfterKey json::KeyItemContext::Value(json::Node value) {
    builder_.Value(value);
    return ValueContextAfterKey{ builder_ };
}

json::ValueContextAfterArray json::ValueContextAfterArray::Value(json::Node value) {
    builder_.Value(value);
    return ValueContextAfterArray{ builder_ };
}

json::ValueContextAfterArray json::ArrayItemContext::Value(json::Node value) {
    builder_.Value(value);
    return ValueContextAfterArray{ builder_ };
}