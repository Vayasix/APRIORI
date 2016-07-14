#include <iostream>
#include <sstream>
using namespace std;
string currentDateToString()
{
    char buf[80];
    ostringstream oss;
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    strftime(buf, sizeof(buf), "%Y-%m-%d_%H-%M-%S", &tm);
    oss << buf;
    return oss.str();
}
int main(){
    cout << currentDateToString() << endl;
}
