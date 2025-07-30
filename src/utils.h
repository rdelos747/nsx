#include <cstdio>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

template <typename T>
string vecJoin(const vector<T>& input, char delim);

vector<string> split(string input, char delim);

string readFile(string path);
