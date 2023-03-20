#include <iostream>
#include <fstream>
#include <ctime> // std::ctime, std::time_t
#include <string>

#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>
#include <valarray>

namespace po = boost::program_options;
namespace fs = boost::filesystem;


void directories(boost::filesystem::path src) { //создание папок, если они отсутвуют для фалов
    try {
        fs::path a = src;
        fs::path b = src;
        fs::path c = src;
        fs::path d = src;
        fs::path img(a.concat("/Image"));
        fs::path doc(b.concat("/Docs"));
        fs::path mus(c.concat("/Music"));
        fs::path oth(d.concat("/Others"));
        if (!fs::exists(img)) {
            fs::create_directory(img);
        }
        if (!fs::exists(doc)) {
            fs::create_directory(doc);
        }
        if (!fs::exists(mus)) {
            fs::create_directory(mus);
        }
        if (!fs::exists(oth)) {
            fs::create_directory(oth);
        }
    } catch (const fs::filesystem_error &e) {
        std::cerr << e.what();
    }
}

void delete_Directory(boost::filesystem::path src) { //удаление пустых папок после распредления
    if (fs::is_directory(src)) {
        if (is_empty(src)) {
            fs::remove(src);
        }
    }
}

void redirect(boost::filesystem::path src) { //распределение папок по их расширению на папки
    if (src.extension() == ".jpg" ||
        src.extension() == ".png") {
        fs::path a = ("../myfiles/Image/");
        fs::copy(src, a.concat(src.filename().string()));
        fs::remove(src);
    }
    if (src.extension() == ".txt" ||
        src.extension() == ".docx") {
        fs::path b = ("../myfiles/Docs/");
        fs::copy(src, b.concat(src.filename().string()));
        fs::remove(src);
    }
    if (src.extension() == ".mp3") {
        fs::path c = ("../myfiles/Music/");
        fs::copy(src, c.concat(src.filename().string()));
        fs::remove(src);
    }
    if (src.extension() != ".mp3" &&
        src.extension() != ".txt" &&
        src.extension() != ".docx" &&
        src.extension() != ".jpg" &&
        src.extension() != ".png") {
        fs::path d = ("../myfiles/Others/");
        fs::copy(src, d.concat(src.filename().string()));
        fs::remove(src);
    }
}

void show_Directory(const fs::path &in_folder) { //изображение папки
    if (fs::exists(in_folder)) {
        if (fs::is_regular_file(in_folder)) {
            std::cout << "Regular file: " << in_folder << std::endl;
        } else if (fs::is_directory(in_folder)) {
            for (const auto &entry: fs::directory_iterator(in_folder)) {
                fs::file_status st = entry.status();
                switch (st.type()) {
                    case fs::regular_file:
                        std::cout << "--" << entry.path() << " (File) \n" << std::endl;
                        break;

                    case fs::directory_file:
                        std::cout << "-----------------" << entry.path() << " (Directory) " << std::endl;
                        show_Directory(entry);
                        break;

                    default:
                        break;
                }
            }
        }
    } else {
        std::cout << in_folder << " does not exist!\n";
    }
}

void start_Work(boost::filesystem::path src) {
    try {
        if (fs::is_directory(src)) {
            for (const auto &entry: fs::directory_iterator(src)) {
                if (is_directory(entry)) {
                    start_Work(entry.path()); //рекурсия для вложенных папок
                }
                if (is_regular(entry)) { //отправляем фалйлы на редирект
                    redirect(entry.path());
                }
            }
            for (const auto &entry: fs::directory_iterator(src)) { //удаление лишних папок после распределения
                if (is_directory(entry)) {
                    delete_Directory(entry.path());
                }
            }
        }
    } catch (const fs::filesystem_error &e) {
        std::cerr << e.what();
    }
}

int main(int argc, char **argv) {
    // options
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help,h", "print usage message")
            ("input,i", po::value<std::string>()->default_value("text"), "Input text")
            ("output,o", po::value<std::string>()->default_value("out.txt"), "Output file")
            ("rules,r", po::value<std::string>()->default_value(":\n"           //выводим правила нашей программы
                                                                "jpg:Images\n"
                                                                "png:Images\n"
                                                                "mp3:Music\n"
                                                                "txt:Docs\n"
                                                                "docx:Docs\n"), " Rules:\n"
                                                                                "jpg:Images\n"
                                                                                "png:Images\n"
                                                                                "mp3:Music\n"
                                                                                "txt:Docs\n"
                                                                                "docx:Docs\n");

    // Parse command line arguments
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
    po::notify(vm);

    if (vm.count("help") || !vm.count("input") || !vm.count("output") || !vm.count("rules")) {
        std::cerr << desc << std::endl;
        return 1;
    }

    std::string input = vm["input"].as<std::string>();
    std::string output = vm["output"].as<std::string>();
    std::string rules = vm["rules"].as<std::string>();

    std::cout << "Input " << input << std::endl;
    std::cout << "Output " << output << std::endl;
    std::cout << "Rules " << rules << std::endl;

    show_Directory("../myfiles");
    directories("../myfiles");
    start_Work("../myfiles");
    show_Directory("../myfiles");
}
