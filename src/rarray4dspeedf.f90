! rarray4dspeedf.f90 
! Ramses van Zon
! Dec 29, 2013
program rarray4dspeedf
  !
  ! Speed test for 4d fortran arrays for comparison with rarray4dspeed.cc. 
  !
  ! Unresolved issues:
  !   - does not give right exact answer
  !   - does not call the 'pass' c routine
  !   - does not do internal timing
  !
  use, intrinsic :: iso_c_binding, only: c_int, c_double, c_ptr, c_loc
  implicit none

  ! call c routine pass
  ! extern void pass(float*,float*,int&);
  interface
     subroutine pass(a, b, c) bind(c)
       use, intrinsic :: iso_c_binding, only: c_int, c_double, c_ptr, c_loc
       implicit none
       type(c_ptr),value :: a, b
       type(c_ptr),value :: c
     end subroutine pass
  end interface
  
  !integer, parameter :: n = 13376 ! requires ~2GB of storage
  !integer, parameter :: n = 9458  ! requires ~1GB of storage
  integer, parameter   :: n = 96    ! requires little storage
  integer, parameter   :: repeat = 3
  real(8), parameter   :: eps = 1.0e-6
  integer              :: i,j,k,l,r
  real(8)              :: check, exact, answer
  real(8), allocatable :: a(:,:,:,:), b(:,:,:,:), c(:,:,:,:)
  
  ! compute exact result first
  check = repeat*(n-1);
  if (mod(n,2) .eq. 0) then
     check = check + (repeat/2)*(3*repeat/2-2)
  else
     check = check + (repeat-1)*(3*repeat-1)/4    
  endif
  exact = (1.0*n)**4*check+(1.0*n)**4*(n-1)*repeat
  
  print *, "fortran dynamic: "
  
  !time it Stopwatch s = START;
  
  answer = 0.0
  allocate(a(n,n,n,n))
  allocate(b(n,n,n,n))
  allocate(c(n,n,n,n))
  
  do r=1,repeat
     do l=1,n
        do k=1,n
           do j=1,n
              do i=1,n
                 a(i,j,k,l) = (l-1)+(i-1)+(repeat-r)
                 b(i,j,k,l) = (k-1)+(j-1)+(repeat-r)/2
              enddo
           enddo
        enddo
     enddo
     !call pass(c_loc(a(1,1,1,1)),c_loc(b(1,1,1,1)),c_loc(repeat))
     do l=1,n
        do k=1,n
           do j=1,n
              do i=1,n
                 c(i,j,k,l) = a(i,j,k,l)+b(i,j,k,l)
              enddo
           enddo
        enddo
     enddo
     !call pass(c,c,repeat)
     do l=1,n
        do k=1,n
           do j=1,n
              do i=1,n
                 anwer = anwer + c(i,j,k,l)
              enddo
           enddo
        enddo
     enddo
     !call pass(c,d,repeat)
  end do
  
  deallocate(a)
  deallocate(b)
  deallocate(c)
  
  !check result
  if (ABS(1-answer/exact)<eps) then
     print *, answer/n/n, "matches exact result!"
  else
     print *, answer/n/n, "does not match exact result of ", exact/n/n
  endif
  
end program rarray4dspeedf

