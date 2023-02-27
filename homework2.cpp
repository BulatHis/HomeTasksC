#include <catch2/catch_test_macros.hpp>
#include <algorithm> // copy_if
#include <numeric>   // iota
#include <vector>
#include <iostream>

std::vector<char> hw02_test(std::vector<int> ascii){
    std::vector<char> simvols(ascii.size());
    for(int i=0;i <= ascii.size();i++ ){
        simvols[i] = static_cast<char>(ascii[i]);
    }
    std::sort(simvols.begin(),simvols.end());
    return simvols;
}

TEST_CASE("hw02", "[test]") {
    REQUIRE(hw02_test(std::vector<int>{81, 68, 65}) == std::vector<char>{'A', 'D', 'Q'});
    REQUIRE(hw02_test(std::vector<int>{79, 80, 90}) == std::vector<char>{'O', 'P', 'Z'});
    REQUIRE(hw02_test(std::vector<int>{70, 71, 72}) == std::vector<char>{'F', 'G', 'H'});
    REQUIRE(hw02_test(std::vector<int>{87, 86, 80}) == std::vector<char>{'P', 'V', 'W'});
    REQUIRE(hw02_test(std::vector<int>{83, 73, 78}) == std::vector<char>{'I', 'N', 'S'});
}
