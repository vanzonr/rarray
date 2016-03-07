  #include <iostream>
  #include <rarray>
  #include <rarrayio>
  #include <cblas.h>
  int main() {
     int m = 2, k=3, n =4;
     rarray<double,2> a(m,k), b(k,n), c(m,n);
     a = 1,-2,3,
         2,-1,0;
     b = -1,3,-2,1,
         -2,1,-3,2,
         -3,2,-1,3;
     cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                 c.extent(0),c.extent(1),a.extent(1), 1.0,
                 a.data(), a.extent(1),
                 b.data(), b.extent(1),
                 1.0,
                 c.data(), c.extent(1));
     std::cout << "a=" << a << '\n'
               << "a=" << b << '\n' 
               << "c=a*b=" << c << '\n';
  }

