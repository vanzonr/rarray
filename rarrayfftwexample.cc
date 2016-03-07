  // rarrayfftwexample.cc
  #include <iostream>
  #include <complex>
  #include <rarray>
  #include <rarrayio>
  #include <fftw3.h>
  int main() {
     using namespace std;
     int n = 4;
     rarray<complex<double>,1> a(n), b(n);
     a = 1.3+5.0i, 1.0, 5.0i, 2.0-5.0i;
     fftw_plan plan = fftw_plan_dft_1d(a.size(),
                        (fftw_complex*)a.data(), 
                        (fftw_complex*)b.data(), 
                        FFTW_FORWARD, FFTW_ESTIMATE);
     fftw_execute(plan);
     fftw_destroy_plan(plan);
     cout << "a=" << a << '\n'
          << "FT(a)=" << b << '\n';
  }
