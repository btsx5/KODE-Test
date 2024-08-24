#pragma once
#include <iostream>
#include <map>
#include <string>
#include <string_view>
#include <vector>
#include <variant>
#include <type_traits>
#include <sstream>

namespace json {

	class Node;
	// —охраните объ€влени€ Dict и Array без изменени€
	using Dict = std::map<std::string, Node>;
	using Array = std::vector<Node>;

	// Ёта ошибка должна выбрасыватьс€ при ошибках парсинга JSON
	class ParsingError : public std::runtime_error {
	public:
		using runtime_error::runtime_error;
	};

	using Number = std::variant<int, double>;

	Number LoadNumber(std::istream& input);

	std::string ParseString(std::istream& input);

	class Node {
	public:
		/* –еализуйте Node, использу€ std::variant */
		using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;

		Node() = default;

		Node(std::nullptr_t);

		Node(Array array);

		Node(Dict map);

		Node(int value);

		Node(double value);

		Node(bool value);

		Node(std::string value);

		bool IsInt() const;

		bool IsDouble() const;

		bool IsPureDouble() const;

		bool IsBool() const;

		bool IsString() const;

		bool IsNull() const;

		bool IsArray() const;

		bool IsDict() const;

		const Array& AsArray() const;

		const Dict& AsDict() const;

		int AsInt() const;

		bool AsBool() const;

		double AsDouble() const;

		const std::string& AsString() const;

		std::nullptr_t AsNull() const;

		const Value& GetValue() const;

	private:
		Value value_;
	};

	class Document {
	public:
		explicit Document(Node root);

		const Node& GetRoot() const;

	private:
		Node root_;
	};

	struct Value_Printer {
		std::ostream& out;

		void operator() (std::nullptr_t) const;

		void operator() (const Array& array) const;

		void operator() (const Dict& dict) const;

		void operator() (bool value) const;

		void operator() (int value) const;

		void operator() (double value) const;

		void operator() (const std::string& str_node) const;
	};

	Document Load(std::istream& input);

	void Print(const Document& doc, std::ostream& output);

	bool operator== (const Node& ref, const Node& ref2);

	bool operator!= (const Node& ref, const Node& ref2);

	bool operator== (const Document& ref, const Document& ref2);

	bool operator!= (const Document& ref, const Document& ref2);

}// namespace json