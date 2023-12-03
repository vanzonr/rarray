//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
 
#if __cplusplus >= 202002L

#ifndef NORANGE

#include <ranges>

TEST_CASE("xrange_is_a_view")
{
    using namespace std::views;
    auto odd = [](int i) -> bool { return i%2; };
    auto shift3 = [](int i) { return i-3; };
    int sum = 0;
    for (int i:xrange(6)|filter(odd)|transform(shift3)) {
        sum+=i;
    }
    REQUIRE(sum==0);
}

#endif

#if __cplusplus >= 202100L

#ifndef NOZIP

TEST_CASE("xrange_with_zip") 
{
    using namespace std::views;
    rvector<double> r = linspace(0.,5.,6);
    for (auto [x,y] : zip(xrange(sizeof(r)), r)) {
        REQUIRE(x==y);
    }   
}

#endif

#endif

#endif

//             # check if c++23 knows std::view::zip
//             echo -en "Checking for (optional) std::view::zip..."
//             sourcefile=$(mktemp --suffix=.cpp)
//             cat >  ${sourcefile} <<EOF
// #include <ranges>
// #include <vector>
// int main() {
// auto x = std::views::zip(std::vector{0});
// }
// EOF
//             if ! $CXX23 $sourcefile &>/dev/null ; then
//                 echo " not found"
//                 echo 'CXX23:=${CXX23} -DNOZIP' >> config.mk
//             else
//                 echo " found"
//             fi
//             rm -f $sourcefile
