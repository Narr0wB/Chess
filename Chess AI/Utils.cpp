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

std::string pairp(Tile t) {
    return std::string("(") + std::to_string(t.first) + std::string(", ") + std::to_string(t.second) + std::string(")");
}