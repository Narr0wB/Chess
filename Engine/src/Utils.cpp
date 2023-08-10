#include "Utils.h"

std::string vectp(std::vector<Tile> vec) {
    std::string vecp_return;

    if (vec.size() == 0)
        return "[ERROR] vector is empty!";
        
    for (Tile tile : vec) {
        vecp_return += ( "(" + std::to_string(tile.first) + ", " + std::to_string(tile.second) + ")");
        vecp_return += " ";
    }
    return vecp_return;
}

bool invect(std::vector<Tile> haystack, Tile needle) {
    for (auto element : haystack) {
        if (element.first == needle.first && element.second == needle.second)
        return true;
    }
    return false;
}

bool is_in_table(std::vector<uint64_t>& table, uint64_t position_hash)
{
    for (auto elem : table) {
        if (position_hash == elem)
            return true;
    }
    return false;
}

std::string pairp(Tile t) {
    return std::string("(") + std::to_string(t.first) + std::string(", ") + std::to_string(t.second) + std::string(")");
}

std::vector<std::string> split(std::string string, char token) {
    std::vector<std::string> splitStrings;
    std::istringstream iss(string);

    std::string tmp;
    for (std::string s; std::getline(iss, s, token); ) {

        // Do not split what is inside quotation marks
        if (s.find('"') != std::string::npos) {
            tmp = s;
            tmp.erase(0, 1);
            while (std::getline(iss, s, token)) {
                tmp += " " + s;

                if (s.find('"') != std::string::npos) {
                    tmp.erase(tmp.end()-1, tmp.end());
                    break;
                }
            }
            splitStrings.push_back(tmp);
            continue;
        }

        splitStrings.push_back(s);
    }

    return splitStrings;
}