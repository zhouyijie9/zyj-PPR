//implement PPR
#include <iostream>
#include <ctime>
using namespace std;
int main()
{
    int b = 10;
    int &a = b;
    a = 20;
    cout << "a=" << a << ",b=" << b << endl;

    return 0;
}