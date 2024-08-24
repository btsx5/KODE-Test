#include "json.h"
#include <string>
#include <memory>

namespace json {
	class KeyItemContext;
	class DictItemContext;
	class ArrayItemContext;
	class ValueContextAfterKey;
	class ValueContextAfterArray;

	class Builder {
	public:
		Builder() = default;

		KeyItemContext Key(const std::string& key);
		Builder& Value(json::Node value);
		DictItemContext StartDict();
		ArrayItemContext StartArray();
		Builder& EndDict();
		Builder& EndArray();
		json::Node Build();

		std::vector<json::Node*> nodes_stack_;
		json::Node root_;
	};

	class BaseContext {
	public:
		BaseContext(Builder& builder) : builder_(builder) {}
		KeyItemContext Key(std::string key);
		Builder& Value(json::Node value);
		DictItemContext StartDict();
		ArrayItemContext StartArray();
		Builder& EndDict();
		Builder& EndArray();
		json::Node Build();
	protected:
		Builder& builder_;
	};

	class ValueContextAfterKey : public BaseContext {
	public:
		Builder& Value(json::Node value) = delete;
		ArrayItemContext StartArray() = delete;
		Builder& EndArray() = delete;
		json::Node Build() = delete;
	};

	class KeyItemContext : public BaseContext {
	public:

		KeyItemContext Key(std::string key) = delete;
		Builder& EndDict() = delete;
		Builder& EndArray() = delete;
		json::Node Build() = delete;
		ValueContextAfterKey Value(json::Node value);
	};

	class DictItemContext : public BaseContext {
	public:
		Builder& Value(json::Node value) = delete;
		ArrayItemContext StartArray() = delete;
		Builder& EndArray() = delete;
		DictItemContext StartDict() = delete;
		json::Node Build() = delete;
	};

	class ValueContextAfterArray : public BaseContext {
	public:
		KeyItemContext Key(std::string key) = delete;
		Builder& EndDict() = delete;
		json::Node Build() = delete;
		ValueContextAfterArray Value(json::Node value);
	};

	class ArrayItemContext : public BaseContext {
	public:
		KeyItemContext Key(std::string key) = delete;
		Builder& EndDict() = delete;
		json::Node Build() = delete;
		ValueContextAfterArray Value(json::Node value);
	};

}