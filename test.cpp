//implement PPR
#include <iostream>
#include <ctime>
using namespace std;
int main()
{
    double start1 = clock();
    for (int i = 0; i < 1303995284; i++)
    {
        double m = 1238.9;
        m = m * i;
    }
    cout << "t1 = " << (clock() - start1) / CLOCKS_PER_SEC << endl;

    double start2 = clock();
    for (int i = 0; i < 1303995284; i++)
    {
        double m = 1238.9;
        m = m + i;
    }
    cout << "t2 = " << (clock() - start2) / CLOCKS_PER_SEC << endl;

    return 0;
}