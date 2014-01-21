! rarray4dspeedf.f90 
! Ramses van Zon
! Dec 29-30, 2013
program rarray4dspeedf
  !
  ! Speed test for 4d fortran arrays for comparison with rarray4dspeed.cc. 
  !
  implicit none
  
  !integer, parameter :: n = 13376 ! requires ~2GB of storage
  integer, parameter :: n = 140  ! requires ~1GB of storage
  !integer, parameter   :: n = 96    ! requires little storage
  integer, parameter   :: repeat = 3
  real(8), parameter   :: eps = 1.0e-6
  integer              :: i,j,k,l,r,countstart,countfinish,countrate,countmax
  real(8)              :: check, exact, answer
  real(4), allocatable :: a(:,:,:,:), b(:,:,:,:), c(:,:,:,:)
  real(4)              :: d

  ! compute exact result first
  check = repeat*(n-1);
  if (mod(repeat,2) .eq. 0) then
     check = check + (repeat/2)*(3*repeat/2-2)
  else
     check = check + (repeat-1)*(3*repeat-1)/4    
  endif
  exact = (1.0*n)**4*check+(1.0*n)**4*(n-1)*repeat
  
  write (*,'(A)',advance='no'), "fortran: "
    
  call system_clock(countstart,countrate,countmax)

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
     call passf(a(1,1,1,1),b(1,1,1,1),repeat)
     do l=1,n
        do k=1,n
           do j=1,n
              do i=1,n
                 c(i,j,k,l) = a(i,j,k,l)+b(i,j,k,l)
              enddo
           enddo
        enddo
     enddo
     call passf(c(1,1,1,1),c(1,1,1,1),repeat)
     do l=1,n
        do k=1,n
           do j=1,n
              do i=1,n
                 answer = answer + c(i,j,k,l)
              enddo
           enddo
        enddo
     enddo
     d = answer
     call passf(c(1,1,1,1),d,repeat)
  end do
  
  deallocate(a)
  deallocate(b)
  deallocate(c)

  ! Stopwatch STOP
  call system_clock(countfinish,countrate,countmax)

  
  !check result
  if (ABS(1-answer/exact)>=eps) then
     print *, answer/n/n, "does not match exact result of ", exact/n/n
  endif

  write (*,*), (1.0*countfinish-1.0*countstart)/(1.0*countrate), "s elapsed"

end program rarray4dspeedf

