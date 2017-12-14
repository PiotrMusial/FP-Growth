#ifndef StringTokenizer_hpp
#define StringTokenizer_hpp

#include <vector>
#include <sstream>
using namespace std;

//klasa, która dowolnego stringa zamienia na wektor, gdzie jego elementami s¹ znaki miêdzy spacjami
class StringTokenizer {
private:
    vector<string> tokens;
    string strIn;
public:
    StringTokenizer(string strIn);
    vector<string> getTokens();
};

#endif //StringTokenizer_hpp
