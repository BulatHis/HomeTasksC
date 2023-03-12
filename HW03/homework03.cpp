#include <cstring> //std::memcmp
#include <fstream> //std::ifstream
#include <iostream>
#include <string>
#include <vector>

// Код домашнего задания
// Распарсить блоб
// На вход подается блоб - бинарный файл
// Требуется:
//  a. распаковать данные в структуру Blob
// СТРУКТУРА БЛОБА:
// version - 2 байта
// size - 4 байта
// data - 12 байт
//  b. написать 3 exception класса
//    a. обрабатывает значение version = валидные значения [1-7] // version_exception
//    b. обрабатывает значение size = валидное значение 12 (0xC)
//    c. обрабатывает значение data = валидное значение "Hello, World" (12 байт без нуль-терминатора)
//  с. написать обработчик 3 exception классов
//  d (*). модифицировать распаковку данных в структуру Blob с учетом того, что version и size могут быть big-endian

struct Blob {
    std::vector<char> bytes;; //наша пустая структура с местом для текста, который мы запоним данными из файла
};

class VerException : public std::exception {
public:

    VerException(const char *string, int i) {
        std::cerr << string << i << "\n";
    }
};

class SizeException : public std::exception {
public:
    SizeException(const char *string, int i) {
        std::cerr << string << i << "\n";
    }
};

class TextException : public std::exception {
public:
    TextException(const char *string, char *i) {
        std::cerr << string << i << "\n";
    }
};

void do_fs(const std::string &filename) {
    Blob blob;
    std::ifstream reader(filename, std::ios::in | std::ios::binary);
    if (reader.is_open()) {
        // read bytes from file to buffer
        int data_sz = 18;//размер данных, данный по тз
        blob.bytes.resize(data_sz);//задаем размер нашему вектору в структуре Blop
        reader.read(reinterpret_cast<char *>(blob.bytes.data()), data_sz); //кастим

        int version = blob.bytes[0 && 1];
        if (1 > version || version > 7) {
            version = blob.bytes[1]; //я искал преобразования endian`ов, но не нашел ничего, кроме функций меняющих байты местами.
            //решил вот такую штуку накалякать, работает, но не думаю, что это то что нужно
        }

        if (version >= 1 && version <= 7) {//проверяем version на соответсвие версии
            std::cout << "Версия [1-7] валидна, полученные данные:" << version << "\n";// GOOD
        } else {
            throw VerException("Версия не валидна, полученное число:", version);//BAD
        }


        if (blob.bytes[2] == 0) {
            std::swap(blob.bytes[2],
                      blob.bytes[5]); // я искал преобразования endian`ов, но не нашел ничего, кроме функций меняющих байты местами.
            // решил вот такую штуку накалякать, работает, но не думаю, что это то что нужно
        }

        std::uint32_t num = 0xC; //значение размера текста(12)
        int size = blob.bytes[2];
        if (!std::memcmp(blob.bytes.data() + 2, reinterpret_cast<char *>(&num),
                         4)) { //проверяем файл со 2го байта на 4 следующих байтов на соответствие с num
            std::cout << "Валидное значение 12 (0xC), получено:" << size << "\n";
        } else {
            throw SizeException("Валидное значение 12 (0xC), получено:", size);
        }

        const char *text_data = "Hello, World";//текст, который мы ожидаем и проверяем файл на него
        if (!std::memcmp(blob.bytes.data() + 6, text_data,
                         12)) { //проверяем файл с 6 байта на 12 следующих байтов на соответствие с text_data
            std::cout << "Данные валидные, совпадают c 'Hello, World', данные : "
                      << blob.bytes.data() + 6 << "\n";// GOOD
        } else {
            throw TextException("Text data NOT equal: валидное значение 'Hello, World', получено:  ", blob.bytes.data() + 6); //BAD
        }
    }
}


int main() {

    try {
        do_fs("data2.bin");//big endian
        do_fs("data.bin"); //little endian file
    } catch (const VerException ex) {
    }
    catch (const SizeException ex) {
    }
    catch (const TextException ex) {
    }

    return 0;
}