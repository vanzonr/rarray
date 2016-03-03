#include "rarray"
#include "rarrayio"
#include <sstream>
#include <iostream>
#include <string>
#include <complex>
using namespace std;
int main()
{
    //{
    //{1,2},
    //{3,4}
    //}    
    string s("\n{\n{\n{1,2},\n{3,4},\n{1,2}\n},\n{\n{5,6},\n{7,8},\n{10,11}\n}\n}\n");
    stringstream ss(s);
    rarray<int,3> a;
    ss >> a;
    cout << s << endl;
    cout << a << endl;
    rarray<complex<double>,1> d(1);
    d[0]=complex<double>(1,2);
    rarray<complex<double>,1> c;
    string cs="{(1,2),(2.3,-1.4)}";
    stringstream css(cs);
    css >> c;
    cout << c << endl;
    cout << d << endl;
}
