#include <iostream>
#include "rarray.h"
#include "rarrayex.h"

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

// Test

int main() 
{
    rarray<double,1> va(3);
    va[0] = 1; va[1] = 1; va[2] = 1;
    rarray<int,1>    vb(3);
    vb[0] = 4; vb[1] = 4; vb[2] = 2;

    rarray<double,1> vc(2*(va + va - va) + 2/convert<double>(4.6*vb));

    std::cout << __LINE__ << ": " <<  vc[0] << ' '<< vc[1] << ' '<< vc[2] << '\n';
    rarray<double,1> vd(vb.shape());

    vd = 1/convert<double>(express(vb)); // express should happen auto?

    std::cout << __LINE__ << ": " <<  vd[0] << ' '<< vd[1] << ' '<< vd[2] << '\n';
    rarray<int,1> ve(vb.shape());

    ve = vb%2;

    std::cout << __LINE__ << ": " <<  ve[0] << ' '<< ve[1] << ' '<< ve[2] << '\n';

    rarray<bool,1> z (vb<3);

    rarray<double,1> y(vd.shape());
    std::cout << __LINE__ << ": " <<  "va = " << va[0] << ' '<< va[1] << ' '<< va[2] << '\n';
    std::cout << __LINE__ << ": " <<  "vd = " << vd[0] << ' '<< vd[1] << ' '<< vd[2] << '\n';
    std::cout << __LINE__ << ": " <<  "vb = " << vb[0] << ' '<< vb[1] << ' '<< vb[2] << '\n';
    std::cout << __LINE__ << ": " <<  "z  = " <<  z[0] << ' '<<  z[1] << ' '<<  z[2] << '\n';

    y = ifelse(!z,vd,va/2);

    std::cout << __LINE__ << ": " <<  y[0] << ' '<< y[1] << ' '<< y[2] << '\n';
    bool b = true;
    std::cout << __LINE__ << ": " <<  b << '\n';

    auto x = (-vb)<-1;

    b=all(x);

    std::cout << __LINE__ << ": " <<  b << '\n';
    //    std::cout << __LINE__ << ": " <<  sum(-vb) << '\n';
    return 0;
}
