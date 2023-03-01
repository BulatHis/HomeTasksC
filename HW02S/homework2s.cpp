#include <iostream>
#include <cstring>
#include <fstream>

struct Blob {
    char data[26]; //наша пустая структура с местом для текста, который мы запоним данными ниже
};

int main(){
    Blob blob;
    std::ifstream fs_rd("data.bin", std::ios::in | std::ios::binary);// обращаемся к нашим данным в файле data.bin, чтобы заполнить ими структуру
    fs_rd.read(reinterpret_cast<char*>(&blob), sizeof(Blob)); // кастим и считываем данные в структуру Blob
    fs_rd.close();//закрываем
    std::cout << "" << blob.data << std::endl;// выводим данные в консоль
}


