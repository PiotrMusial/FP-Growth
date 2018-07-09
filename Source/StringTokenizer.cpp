#include "StringTokenizer.hpp"

StringTokenizer::StringTokenizer(string strIn) {
    this->strIn = strIn;

    string str(strIn);
    string tmpstr;
    stringstream ss(str);

    while(ss >> tmpstr)
        tokens.push_back(tmpstr);
}

vector<string> StringTokenizer::getTokens() {
    return tokens;
}
