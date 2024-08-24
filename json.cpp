#include "json.h"

using namespace std;

namespace json {

    namespace {

        std::string ParseString(istream& input) {
            using namespace std::literals;

            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            std::string s;
            while (true) {
                if (it == end) {
                    // Поток закончился до того, как встретили закрывающую кавычку?
                    throw ParsingError("String parsing error");
                }
                const char ch = *it;
                if (ch == '"') {
                    // Встретили закрывающую кавычку
                    ++it;
                    break;
                }
                else if (ch == '\\') {
                    // Встретили начало escape-последовательности
                    ++it;
                    if (it == end) {
                        // Поток завершился сразу после символа обратной косой черты
                        throw ParsingError("String parsing error");
                    }
                    const char escaped_char = *(it);
                    // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
                    switch (escaped_char) {
                    case 'n':s.push_back('\n');
                        break;
                    case 't':s.push_back('\t');
                        break;
                    case 'r':s.push_back('\r');
                        break;
                    case '"':s.push_back('"');
                        break;
                    case '\\':s.push_back('\\');
                        break;
                    default:
                        // Встретили неизвестную escape-последовательность
                        throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                    }
                }
                else if (ch == '\n' || ch == '\r') {
                    // Строковый литерал внутри- JSON не может прерываться символами \r или \n
                    throw ParsingError("Unexpected end of line"s);
                }
                else {
                    // Просто считываем очередной символ и помещаем его в результирующую строку
                    s.push_back(ch);
                }
                ++it;
            }

            return s;
        }

        Node LoadNode(istream& input);

        Node LoadArray(istream& input) {
            Array result;
            bool found = false;
            char c;
            for (; input >> c && c != ']'; ) {

                if (c != ',') {
                    input.putback(c);
                }

                result.push_back(LoadNode(input));
            }
            if (c == ']') { found = true; }
            if (!found) { throw ParsingError("Array parsing error"); }
            return { std::move(result) };
        }

        Node LoadString(istream& input) {
            using namespace std::literals;
            return { std::move(ParseString(input)) };
        }

        Node LoadDict(istream& input) {
            Dict result;
            bool found = false;
            char c;
            for (; input >> c && c != '}'; ) {

                if (c == ',') {
                    input >> c;
                }
                string key = LoadString(input).AsString();
                input >> c;
                result.insert({ std::move(key), LoadNode(input) });
            }
            if (c == '}') {
                found = true;
            }
            if (!found) {
                throw ParsingError("Expected }");
            }

            return { std::move(result) };
        }

        Node LoadBool(istream& input) {
            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            std::string str;
            while (it != end && *it != ',') {
                if (str == "false" || str == "true") {
                    break;
                }
                const char ch = *it;
                if (ch == '\\') {
                    // Встретили начало escape-последовательности
                    ++it;
                    if (it == end) {
                        // Поток завершился сразу после символа обратной косой черты
                        throw ParsingError("String parsing error");
                    }
                    const char escaped_char = *(it);
                    // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
                    switch (escaped_char) {
                    case 'n':str.push_back('\n');
                        break;
                    case 't':str.push_back('\t');
                        break;
                    case 'r':str.push_back('\r');
                        break;
                    case '"':str.push_back('"');
                        break;
                    case '\\':str.push_back('\\');
                        break;
                    default:
                        // Встретили неизвестную escape-последовательность
                        throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                    }
                }
                else {
                    // Просто считываем очередной символ и помещаем его в результирующую строку
                    str.push_back(ch);
                }
                ++it;
            }

            if (str.find("true"s) != std::string::npos) {
                return Node{ true };
            }
            else if (str.find("false") != std::string::npos) {
                return Node{ false };
            }
            else { throw ParsingError("bool err"); }
        }

        Node LoadNull(istream& input) {
            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            std::string str;
            while (it != end && *it != ',') {
                const char ch = *it;
                if (ch == '\\') {
                    // Встретили начало escape-последовательности
                    ++it;
                    if (it == end) {
                        // Поток завершился сразу после символа обратной косой черты
                        throw ParsingError("String parsing error");
                    }
                    const char escaped_char = *(it);
                    // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
                    switch (escaped_char) {
                    case 'n':str.push_back('\n');
                        break;
                    case 't':str.push_back('\t');
                        break;
                    case 'r':str.push_back('\r');
                        break;
                    case '"':str.push_back('"');
                        break;
                    case '\\':str.push_back('\\');
                        break;
                    default:
                        // Встретили неизвестную escape-последовательность
                        throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                    }
                }
                else {
                    // Просто считываем очередной символ и помещаем его в результирующую строку
                    str.push_back(ch);
                }
                ++it;
            }
            if (str.find("null") != std::string::npos) {
                return Node{ };
            }
            else {
                throw ParsingError("expected NULL");
            }
        }

        Node LoadNode(istream& input) {
            char c;
            input >> c;
            if (c == 'n') {
                input.putback(c);
                return LoadNull(input);
            }
            if (c == ']') { throw ParsingError("Expected '['"); }
            if (c == '}') { throw ParsingError("Expected '{'"); }
            if (c == '[') {
                return LoadArray(input);
            }
            else if (c == '{') {
                return LoadDict(input);
            }
            else if (c == '"') {
                return LoadString(input);
            }
            else if (c == 't' || c == 'f') {
                input.putback(c);
                return LoadBool(input);
            }
            else {
                input.putback(c);
                auto number = LoadNumber(input);
                if (std::holds_alternative<int>(number)) {
                    return { std::get<int>(number) };
                }
                else {
                    return { std::get<double>(number) };
                }
            }
        }

    }  // namespace

    Node::Node(std::nullptr_t)
        : value_(nullptr) {
    }

    Node::Node(Array array)
        : value_(move(array)) {
    }

    Node::Node(Dict map)
        : value_(move(map)) {
    }

    Node::Node(int value)
        : value_(value) {
    }

    Node::Node(string value)
        : value_(move(value)) {
    }

    Node::Node(double value)
        : value_(value) {
    }

    Node::Node(bool value)
        : value_(value) {
    }

    const Array& Node::AsArray() const {
        if (!IsArray()) {
            throw std::logic_error("Wrong value type");
        }
        return std::get<Array>(value_);
    }

    const Dict& Node::AsDict() const {
        if (!IsDict()) {
            throw std::logic_error("Wrong value type");
        }
        return std::get<Dict>(value_);
    }

    int Node::AsInt() const {
        if (!IsInt()) {
            throw std::logic_error("Wrong value type");
        }
        return std::get<int>(value_);
    }

    bool Node::AsBool() const {
        if (!IsBool()) {
            throw std::logic_error("Wrong value type");
        }
        return std::get<bool>(value_);
    }

    double Node::AsDouble() const {
        if (!IsDouble()) {
            throw std::logic_error("Wrong value type");
        }
        else if (IsPureDouble()) {
            return std::get<double>(value_);
        }
        return std::get<int>(value_);
    }

    const string& Node::AsString() const {
        if (!IsString()) {
            throw std::logic_error("Wrong value type");
        }
        return std::get<std::string>(value_);
    }

    std::nullptr_t Node::AsNull() const {
        if (!IsNull()) {
            throw std::logic_error("Wrong value type");
        }
        return std::get<std::nullptr_t>(value_);
    }

    bool Node::IsInt() const {
        return std::holds_alternative<int>(value_);
    }

    bool Node::IsDouble() const {
        return holds_alternative<double>(value_) || holds_alternative<int>(value_);
    }

    bool Node::IsPureDouble() const {
        return std::holds_alternative<double>(value_);
    }

    bool Node::IsBool() const {
        return std::holds_alternative<bool>(value_);
    }

    bool Node::IsString() const {
        return std::holds_alternative<std::string>(value_);
    }

    bool Node::IsNull() const {
        return std::holds_alternative<std::nullptr_t>(value_);
    }

    bool Node::IsArray() const {
        return std::holds_alternative<Array>(value_);
    }

    bool Node::IsDict() const {
        return std::holds_alternative<Dict>(value_);
    }

    const Node::Value& Node::GetValue() const {
        return value_;
    }

    Document::Document(Node root)
        : root_(move(root)) {
    }

    const Node& Document::GetRoot() const {
        return root_;
    }

    Document Load(istream& input) {
        return Document{ LoadNode(input) };
    }

    void Print(const Document& doc, std::ostream& output) {
        std::visit(Value_Printer{ output }, doc.GetRoot().GetValue());
    }

    Number LoadNumber(istream& input) {
        using namespace std::literals;

        std::string parsed_num;

        // Считывает в parsed_num очередной символ из input
        auto read_char = [&parsed_num, &input] {
            parsed_num += static_cast<char>(input.get());
            if (!input) {
                throw ParsingError("Failed to read number from stream"s);
            }
            };

        // Считывает одну или более цифр в parsed_num из input
        auto read_digits = [&input, read_char] {
            if (!std::isdigit(input.peek())) {
                throw ParsingError("A digit is expected"s);
            }
            while (std::isdigit(input.peek())) {
                read_char();
            }
            };

        if (input.peek() == '-') {
            read_char();
        }
        // Парсим целую часть числа
        if (input.peek() == '0') {
            read_char();
            // После 0 в JSON не могут идти другие цифры
        }
        else {
            read_digits();
        }

        bool is_int = true;
        // Парсим дробную часть числа
        if (input.peek() == '.') {
            read_char();
            read_digits();
            is_int = false;
        }

        // Парсим экспоненциальную часть числа
        if (int ch = input.peek(); ch == 'e' || ch == 'E') {
            read_char();
            if (ch = input.peek(); ch == '+' || ch == '-') {
                read_char();
            }
            read_digits();
            is_int = false;
        }

        try {
            if (is_int) {
                // Сначала пробуем преобразовать строку в int
                try {
                    return std::stoi(parsed_num);
                }
                catch (...) {
                    // В случае неудачи, например, при переполнении,
                    // код ниже попробует преобразовать строку в double
                }
            }
            return std::stod(parsed_num);
        }
        catch (...) {
            throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
        }
    }

    bool operator== (const Node& ref, const Node& ref2) {
        if (ref.GetValue().index() != ref2.GetValue().index()) {
            return false;
        }
        return ref.GetValue() == ref2.GetValue();
    }

    bool operator!= (const Node& ref, const Node& ref2) {
        return !(ref.GetValue() == ref2.GetValue());
    }

    bool operator== (const Document& ref, const Document& ref2) {
        if (ref2.GetRoot().GetValue().index() != ref.GetRoot().GetValue().index()) {
            return false;
        }
        return ref.GetRoot().GetValue() == ref2.GetRoot().GetValue();
    }

    bool operator!= (const Document& ref, const Document& ref2) {
        return !(ref.GetRoot().GetValue() == ref2.GetRoot().GetValue());
    }

    void Value_Printer::operator()(std::nullptr_t) const {
        out << "null";
    }

    void Value_Printer::operator()(const Array& array) const {
        bool first = true;
        out << '[';
        for (const auto& value : array) {
            if (first) {
                std::visit(Value_Printer{ out }, value.GetValue());
                first = false;
                continue;
            }
            else {
                out << ',';
                std::visit(Value_Printer{ out }, value.GetValue());
            }
        }
        out << ']';
    }

    void Value_Printer::operator()(const Dict& dict) const {
        bool first = true;
        out << '{';
        for (const auto& [key, value] : dict) {
            if (first) {
                out << "\"" << key << "\":";
                std::visit(Value_Printer{ out }, value.GetValue());
                first = false;
            }
            else {
                out << ',';
                out << "\"" << key << "\":";
                std::visit(Value_Printer{ out }, value.GetValue());
            }
        }
        out << '}';
    }

    void Value_Printer::operator()(bool value) const {
        if (value) {
            out << "true";
        }
        else {
            out << "false";
        }
    }

    void Value_Printer::operator()(int value) const {
        out << value;
    }

    void Value_Printer::operator()(double value) const {
        out << value;
    }

    void Value_Printer::operator()(const string& str_node) const {
        out << '"';
        for (char c : str_node) {
            switch (c) {
            case '\n': out << "\\n";
                break;
            case '\t': out << "\t";
                break;
            case '\r': out << "\\r";
                break;
            case '\"': out << "\\\"";
                break;
            case '\\': out << "\\\\";
                break;
            default: out << c;
                break;
            }
        }
        out << '"';
    }
}  // namespace json