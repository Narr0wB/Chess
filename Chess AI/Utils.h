
#include <vector>
#include <string>

#define isinbtw(x, l1, l2) x > l2 && x < l1

typedef unsigned short uint;
typedef std::pair<short, short> Tile;

std::string vectp(std::vector<Tile> vec); // vector print, returns a string that can be printed that visualizes the contents of the vector | <Tile> type

bool invect(std::vector<Tile> haystack, Tile needle);

bool is_in_table(std::vector<uint64_t>& table, uint64_t position_hash);

std::string pairp(Tile t);