#include <string>
#include <vector>

using namespace std;

template <typename T>
string str(const T& val);

template <typename T>
string vecJoin(const vector<T>& input, char delim);

vector<string> split(string input, char delim);

string readFile(string path);

string getTime();

void saveFile(string data, string path);

void log(string text);

void clearLog();
