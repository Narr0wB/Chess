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