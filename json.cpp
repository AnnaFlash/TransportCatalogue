#include "json.h"

using namespace std;

namespace json {

    namespace {
        Node LoadNode(istream& input);
        Node LoadString(std::istream& input) {
            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            std::string s;
            while (true) {
                if (it == end&&s.back()!='"') {
                    throw ParsingError("String parsing error");
                }
                if (it == end) {
                    break;
                }
                const char ch = *it;
                if (ch == '"') {
                    ++it;
                    break;
                }
                else if (ch == '\\') {
                    ++it;
                    if (it == end) {
                        throw ParsingError("String parsing error");
                    }
                    const char escaped_char = *(it);
                    switch (escaped_char) {
                    case 'n':
                        s.push_back('\n');
                        break;
                    case 't':
                        s.push_back('\t');
                        break;
                    case 'r':
                        s.push_back('\r');
                        break;
                    case '"':
                        s.push_back('"');
                        break;
                    case '\\':
                        s.push_back('\\');
                        break;
                    default:
                        throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                    }
                }
                else if (ch == '\n' || ch == '\r') {
                    throw ParsingError("Unexpected end of line");
                }
                else {
                    s.push_back(ch);
                }
                ++it;
            }

            return Node(std::move(string(s)));
        }
        Node LoadNull(istream& input) {
            string line;
            getline(input, line, ',');
            line.insert(0, "n");
            if (line == "null") {
                return Node();
            }
            else {
                throw ParsingError("Incorrect input"s);
            }
        }

        Node LoadArray(istream& input) {
            Array result;
            char c;

            for (; input >> c && c != ']';) {
                if (c != ',') {
                    input.putback(c);
                }
                result.push_back(LoadNode(input));
            }
            if (c != ']') {
                throw ParsingError("Incorrect input"s);
            }

            return Node(move(result));
        }

        Node LoadDict(istream& input) {
            Dict result;
            char c;

            for (; input >> c && c != '}';) {
                if (c == ',') {
                    input >> c;
                }
                string key = LoadString(input).AsString();
                input >> c;
                result.insert({ move(key), LoadNode(input) });
            }
            if (c != '}') {
                throw ParsingError("Incorrect input"s);
            }

            return Node(move(result));
        }

        Node LoadBool(istream& input, char c) {
            string line;
            if (c == 't') {
                input >> std::setw(3) >> line;
                line.insert(0, "t");
                if (line == "true") {
                    return Node(true);
                }
                else {
                    throw ParsingError("Incorrect input"s);
                }
            }
            else if (c == 'f') {
                input >> std::setw(4) >> line;
                line.insert(0, "f");
                if (line == "false") {
                    return Node(false);
                }
                else {
                    throw ParsingError("Incorrect input"s);
                }
            }
        }

        Number LoadNumber(std::istream& input) {
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
                        // В случае неудачи, например, при переполнении
                        // код ниже попробует преобразовать строку в double
                    }
                }
                return std::stod(parsed_num);
            }
            catch (...) {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }

        Node LoadNode(istream& input) {
            char c;
            input >> c;

            if (c == '[') {
                return LoadArray(input);
            }
            else if (c == '{') {
                return LoadDict(input);
            }
            else if (c == '"') {
                return LoadString(input);
            }
            else if (c == 'n') {
                return LoadNull(input);
            }
            else if ((c == 't') || (c == 'f')) {
                return LoadBool(input, c);
            }
            else {
                input.putback(c);
                auto number = LoadNumber(input);
                if (number.index() == 0) {
                    return std::get<int>(number);
                }
                else /*if (number.index() == 1)*/ {
                    return std::get<double>(number);
                }
            }

            throw ParsingError("Incorrect input"s);
        }

    }  // namespace

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
        Node node = doc.GetRoot();
        if (node.GetType() == 0) {
            output << "null";
        }
        else if (node.GetType() == 1) {
            bool is_first = true;
            output << '[';
            for (const Node& n : node.GetArrayValue()) {
                if (!is_first) {
                    output << ',';
                }
                Print(Document(n), output);
                output << std::endl;
                is_first = false;
            }
            output << ']';
        }
        else if (node.GetType() == 2) {
            bool is_first = true;
            output <<  std::endl;
            output << "     {" ;
            for (const auto& n : node.GetDictValue()) {
                if (!is_first) {
                    output  << ',';
                }
                output << '"' << n.first << "\" : "s;
                Print(Document(n.second), output); 
                output << std::endl;
                is_first = false;
            }
            output << "     }";
        }
        else if (node.GetType() == 3) {
            bool result = node.GetBoolValue();
            if (result == true) {
                output << "true"s;
            }
            else {
                output << "false"s;
            }
        }
        else if (node.GetType() == 4) {
            output << std::to_string(node.GetIntValue());
        }
        else if (node.GetType() == 5) {
            ostringstream ost;
            ost << node.GetDoubleValue();
            std::string result = ost.str();
            output << result;
        }
        else if (node.GetType() == 6) {
            string result = node.GetStringValue();
            result = regex_replace(result, std::regex("\\\\"s), "\\\\"s);
            result = regex_replace(result, std::regex(R"(")"), R"(\")");
            result = regex_replace(result, std::regex(R"(\n)"), R"(\n)");
            result = regex_replace(result, std::regex(R"(\r)"), R"(\r)");
            result = regex_replace(result, std::regex(R"(\t)"), R"(\t)");
            result.insert(0, "\"");
            result.push_back('"');
            output << result;
        }
    }

}  // namespace json