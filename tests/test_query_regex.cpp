#include <iostream>
#include <regex>
#include <string>

int main() {
    std::string input = R"(SELECT users WHERE name:"Alicia":TEXT score:90.0:DOUBLE)";
    
    std::regex QUERY_PATTERN(R"(SELECT\s+(\w+)\s+WHERE\s+((?:\w+:(?:"[^"]*"|[^:\s]+):\w+)(?:\s+\w+:(?:"[^"]*"|[^:\s]+):\w+)*))");

    std::smatch match;
    if (std::regex_match(input, match, QUERY_PATTERN)) {
        std::cout << "✅ Match!\n";
        std::cout << "Table: " << match[1] << "\n";
        std::cout << "Filters: " << match[2] << "\n";
    } else {
        std::cout << "❌ No match\n";
    }

    return 0;
}
