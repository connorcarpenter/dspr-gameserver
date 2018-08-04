#pragma once
//
// Created by connor on 7/23/18.
//

#include <sstream>

namespace DsprGameServer {
    class StringUtils {
    public:
        static std::vector<std::string> split(const std::string &s, char delimiter) {
            std::vector<std::string> tokens;
            std::string token;
            std::istringstream tokenStream(s);
            while (std::getline(tokenStream, token, delimiter)) {
                auto newStr = trimEnd(token, "\r");
                tokens.push_back(*newStr);
                delete newStr;
            }
            return tokens;
        }

        static bool endsWith(std::string const &fullString, std::string const &ending) {
            if (fullString.length() >= ending.length()) {
                return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
            } else {
                return false;
            }
        }

        static std::string* trimEnd(std::string const &fullString, std::string const &ending) {
            if (endsWith(fullString, ending))
            {
                std::string* newStr = new std::string(fullString.substr(0, fullString.length() - ending.length()));
                return newStr;
            }

            return new std::string(fullString);
        }
    };
}