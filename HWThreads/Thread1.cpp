#include <iostream>
#include <thread>
#include <future>
#include <mutex>
#include <fstream>
#include <vector>
#include <chrono>

//1. Реализация функции записи данных в файл в синхронном режиме:
void write_ToFile_Sync(const std::string& filename, const char* data, size_t size) {
    std::ofstream file(filename, std::ios::binary); //открытие файла в режиме бинарной записи
    file.write(data, size); // запись данных(байт из массива data) в файл
    file.close(); // закрытие потока записи
}

//2. Реализация функции записи данных в файл в многопоточном режиме:
void write_ToFile_Multithreaded(const std::string& filename, const char* data, size_t size) {
    const int numThreads = (std::thread::hardware_concurrency() ); // узнаём количество потоков сиситемы, дотупных для работы(не обязательно все возможные[меньше фактических])
    const size_t blockSize = 0x1000;// определяем размер блока по которым будем записывать данные (4096 байт) 16чная система
    const size_t numBlocks = size / blockSize;// определяем количесво блоков, нужных для записи всех данных
    const size_t lastBlockSize = size % blockSize;// размер последнего блока

    std::vector<std::thread> threads;// создаем пустой вектор для потоков
    threads.reserve(numThreads);// резервируем место для потоков

    std::ofstream file(filename, std::ios::binary);// открытие файла в режиме бинарной записи

    for (int i = 0; i < numThreads; ++i) { // цикл для создания нужжного количесвтва потоков
        const size_t startBlock = i * numBlocks / numThreads; // индекс первого блока
        const size_t endBlock = (i + 1) * numBlocks / numThreads;// индекс последнего блока обрабатываемый этим потоком
        threads.emplace_back([&, startBlock, endBlock] { // открываем файл и записываем в него блоки данных по индексам (& нужно для захвата данных по ссылка)
            for (size_t block = startBlock; block < endBlock; ++block) { // цикл перебора блоков
                file.seekp(block * blockSize, std::ios::beg);// утанавливаем позицию записи в файл для блока
                file.write(data + block * blockSize, blockSize);// записываем дату в файл
            }
        });
    }

    for (std::thread& thread : threads) {// перебор потоков
        thread.join();// ожидаем завершения работы потока
    }

    if (lastBlockSize > 0) {// проверка, что все данные записаны
        file.seekp(numBlocks * blockSize, std::ios::beg);// устанавливаем позицию записи для оставшихся данных
        file.write(data + numBlocks * blockSize, lastBlockSize);// записываем данные
    }

    file.close();// закрываем файл
}

//3. Измерение времени выполнения:
int main() {
    constexpr size_t fileSize = 2ull  * 1024ull *  1024ull * 1024ull;//размер загружаемых данныхо (меняй перво число для изменения гб памяти)
    std::vector<char> data(fileSize); //вектор размера fileSize

     const auto startSync = std::chrono::high_resolution_clock::now(); // начало замера времени
     write_ToFile_Sync("test_sync.bin", data.data(), fileSize); // вызов метода синхронной записи
     const auto endSync = std::chrono::high_resolution_clock::now(); // конец замера времени
     const auto elapsedSync = std::chrono::duration_cast<std::chrono::milliseconds>(endSync - startSync);//разность начала и конца замеров
     std::cout << "Sync write time: " << elapsedSync.count() << " ms\n"; //вывыод выремени работы синхронной записи

    const auto startMultithreaded = std::chrono::high_resolution_clock::now();// начало замера времени
    write_ToFile_Multithreaded("test_multithreaded.bin", data.data(), fileSize);// вызов метода асинхронной записи
    const auto endMultithreaded = std::chrono::high_resolution_clock::now();// конец замера времени
    const auto elapsedMultithreaded = std::chrono::duration_cast<std::chrono::milliseconds>(endMultithreaded - startMultithreaded);// разность начала и конца замеров
    std::cout << "Multithreaded write time: " << elapsedMultithreaded.count() << " ms\n";// вывыод выремени работы асинхронной записи
    return 0;
}

//1. Функция записи данных в файл в многопоточном режиме выполняется быстрее, поскольку она использует все доступные потоки для параллельной записи блоков данных в файл
//2. измерения времени проводились через разность времени начала и окончания работы методов через std::chrono::high_resolution_clock::now()
//  узнал количесво доступных потоков через std::thread::hardware_concurrency() выбрал эту, т.к. сразу покаывает количество доступных параллельных потоков
//3. к сожалению, мой комп потянул максимум 3 гб записи
// при 3гб- синхронная запись заняла примерно 8000 - 13000мс ,а асинхронная заняла 30-60мс
// увеличить размер файла не получится, тк уперся в предел ноута, могу уменьшить
//  при 1гб- синхронная запись заняла примерно 2800 - 3050мс ,а асинхронная заняла 10мс - 32мс
//4. опять же, не могу увеличить количество потоков, могу уменьшить ==
// при 1гб (уменьшено в 2 раза) --  9мс ,31мс ,12мс ,16мс ,34мс
// при 3гб (уменьшено в 2 раза) -- 53мс, 62мс, 36мс, 46мс, 37мс
// можно заметить, что скорость многопоточной загрузки не сильно изменилась от уменьшения количества потоков, думаю, это связано с маленьким количеством данных (т.е. меньше потоков и так достаточно)
// если оставить 2 потока из 6, то при загрузке 3гб скорость сильно меняется -- 74мс, 45мс, 64мс, 88мс, 106мс, 144мс, 136мс =т.е. 2 потоков уже недостаточно для 3гб
// если оставить 2 потока из 6, то при загрузке 1гб скорость особо не меняется -- 18мс, 67мс, 11мс, 79мс, 22мс, 46мс= можно заметить, что максимально ожидание увеличелось
// но минимальная осталась той же
//
// // //