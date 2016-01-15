#include <cmath>
#include "rarray.h"
#include "rarrayex.h"
#include "elapsed.h"
#define REP 400

int main(int argc, char** argv)
{
    int N = argc>1?atoi(argv[1]):1000;
    std::cout << N << " ";
    rarray<double,1> a(N),b(N),c(N);
    for (int i = 0; i<N; i++)
        a[i] = i;
    for (int i = 0; i<N; i++)
        b[i] = N-i;
    double sumdt=0.0, sumdt2=0.0;
    for (int i = 0; i < REP; i++) {
        double t1 = elapsed_etime();
        c = a + b;
        double t2 = elapsed_etime();
        double dt = t2-t1;
        sumdt += dt;
        sumdt2 += dt*dt;        
    }
    double avdt= sumdt / REP;
    double avdt2 = sumdt2 / REP;
    double erdt = sqrt((avdt2-avdt*avdt)/(REP-1));
    double relerr = erdt/avdt;
    double mflops = 0.000001*N/avdt;
    std::cout << mflops << " +/- " << relerr*mflops << " MFlops\n";
    return mflops;
}
