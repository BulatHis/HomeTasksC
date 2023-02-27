#include <catch2/catch_test_macros.hpp>

#include <algorithm> // copy_if
#include <numeric>   // iota
#include <vector>
#include <iostream>




std::vector<int> hw01_test(std::vector<int> numbers) {

    //вставка в начало вектора
    numbers.insert(numbers.begin(), 5);

    //массив неченых чисел
    std::vector<int> noteven_nums(numbers.size());
    auto last_elem_it = std::copy_if(numbers.begin(), numbers.end(),
                                     noteven_nums.begin(), [](int i) { return i % 2 != 0; });
    //удаление 0й
    noteven_nums.resize(std::distance(noteven_nums.begin(), last_elem_it));
    std::cout << "New container (resized)\n";
    for (auto st = noteven_nums.begin(); st < noteven_nums.end(); st++)
        std::cout << *st << " ";
    std::cout << '\n';

    //массив чисел, которые делятся на 3
    std::vector<int> third_Nums(numbers.size());
    auto last_elem_third = std::copy_if(numbers.begin(), numbers.end(),
                                        third_Nums.begin(), [](int i) { return i % 3 == 0; });

    //массив чисел, которые делятся на 11
    std::vector<int> eleven_Nums(numbers.size());
    auto last_elem_eleven = std::copy_if(numbers.begin(), numbers.end(),
                                         eleven_Nums.begin(), [](int i) { return i % 11 == 0; });

    //удаление 0й в деленой на 3 + вывод
    third_Nums.resize(std::distance(third_Nums.begin(), last_elem_third));
    std::cout << "New container (resized)(3)\n";
    for (auto st = third_Nums.begin(); st < third_Nums.end(); st++)
        std::cout << *st << " ";
    std::cout << '\n';

    //удаление 0й в деленой на 11 + вывод
    eleven_Nums.resize(std::distance(eleven_Nums.begin(), last_elem_eleven));
    std::cout << "New container (resized)(11)\n";
    for (auto st = eleven_Nums.begin(); st < eleven_Nums.end(); st++)
        std::cout << *st << " ";
    std::cout << '\n';

    //сортировка по убыванию
    std::sort(numbers.begin(), numbers.end());
    std::reverse(numbers.begin(), numbers.end());
    std::cout << " Descending sort\n";
    for (auto st = numbers.begin(); st < numbers.end(); st++)
        std::cout << *st << " ";
    std::cout << '\n';

    //замена чисел [5....15] на x*2
    for (int i = 0; i < numbers.size(); i++) {
        if (numbers[i] > 5 and numbers[i] < 16) {
            numbers[i] = numbers[i] * 2;
        }
    }
    std::cout << " Doubling number between 5-15\n";
    for (auto st = numbers.begin(); st < numbers.end(); st++)
        std::cout << *st << " ";
    std::cout << '\n';
    return numbers;
}

std::vector<int> get_eleven_Nums(std::vector<int> numbers) {
    std::vector<int> eleven_Nums(numbers.size());
    auto last_elem_eleven = std::copy_if(numbers.begin(), numbers.end(),
                                         eleven_Nums.begin(), [](int i) { return i % 11 == 0; });
    eleven_Nums.resize(std::distance(eleven_Nums.begin(), last_elem_eleven));
    return eleven_Nums;
}
int main(){
    //создание массива
    std::vector<int> numbersFirst(25);
    std::iota(numbersFirst.begin(), numbersFirst.end(), 1);
    hw01_test(numbersFirst);
};

TEST_CASE("Number whith 11", "[eleven_Nums]") {
    REQUIRE(get_eleven_Nums(std::vector<int>{1, 2, 3, 4}) == std::vector<int>{});
    REQUIRE(get_eleven_Nums(std::vector<int>{1, 2, 11, 4}) == std::vector<int>{11});
    REQUIRE(get_eleven_Nums(std::vector<int>{11, 22,11, 44}) == std::vector<int>{11,22,11,44});
}

TEST_CASE("AllHW", "[all_HW]") {
    REQUIRE(hw01_test(std::vector<int>{1, 11 , 22,33 }) == std::vector<int>{33,22,22,5,1});
    REQUIRE(hw01_test(std::vector<int>{4, 12 , 15,25 }) == std::vector<int>{25,30,24,5,4});
    REQUIRE(hw01_test(std::vector<int>{10}) == std::vector<int>{20, 5});
}