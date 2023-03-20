#include <iostream>
#include <fstream>
#include <ctime> // std::ctime, std::time_t
#include <string>

#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>
#include <valarray>

namespace po = boost::program_options;
namespace fs = boost::filesystem;


void show_Directory(const fs::path in_folder) { //изображение папки
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

void directories(boost::filesystem::path src) { //создание папок, если они отсутвуют для фалов
    try {
        fs::path a = src.string();
        fs::path b = src.string();
        fs::path c = src.string();
        fs::path d = src.string();
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
    try {
        if (src.extension() == ".jpg" ||
            src.extension() == ".png") {
            fs::path a = ("./myfiles/Image/");
            fs::path aa = ("./myfiles/Image/");
            if (exists(aa.concat(src.filename().string()))) {
                if (src.parent_path() != "./myfiles/Image") {
                    std::cout << src.parent_path();
                    a.concat(src.stem().string());
                    a.concat("_copy");
                    a.concat(src.extension().string());
                    fs::copy(src, a);
                    fs::remove(src);
                }
            } else {
                fs::copy(src, aa);
                fs::remove(src);
            }
        }
        if (src.extension() == ".txt" ||
            src.extension() == ".docx") {
            fs::path b = ("./myfiles/Docs/");
            fs::path bb = ("./myfiles/Docs/");
            if (exists(bb.concat(src.filename().string()))) {
                if (src.parent_path() != "./myfiles/Docs") {
                    b.concat(src.stem().string());
                    b.concat("_copy");
                    b.concat(src.extension().string());
                    fs::copy(src, b);
                    fs::remove(src);
                }
            } else {
                fs::copy(src, bb);
                fs::remove(src);
            }
        }
        if (src.extension() == ".mp3") {
            fs::path c = ("./myfiles/Music/");
            fs::path cc = ("./myfiles/Music/");
            if (exists(cc.concat(src.filename().string()))) {
                if (src.parent_path() != "./myfiles/Music") {
                    c.concat(src.stem().string());
                    c.concat("_copy");
                    c.concat(src.extension().string());
                    fs::copy(src, c);
                    fs::remove(src);
                }
            } else {
                fs::copy(src, cc);
                fs::remove(src);
            }
        }
        if (src.extension() != ".mp3" &&
            src.extension() != ".txt" &&
            src.extension() != ".docx" &&
            src.extension() != ".jpg" &&
            src.extension() != ".png") {
            fs::path d = ("./myfiles/Others/");
            fs::path dd = ("./myfiles/Others/");
            if (exists(dd.concat(src.filename().string()))) {
                if (src.parent_path() != "./myfiles/Others") {
                    d.concat(src.stem().string());
                    d.concat("_copy");
                    d.concat(src.extension().string());
                    fs::copy(src, d);
                    fs::remove(src);
                }
            } else {
                fs::copy(src, dd);
                fs::remove(src);
            }
        }
    } catch (const fs::filesystem_error &e) {
        std::cerr << e.what();
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
            ("rules,r", po::value<std::string>()->default_value("/rules"), "Rules /rules")
            ("directory,r", po::value<std::string>()->default_value("../myfiles"), "Directory ../myfiles");

    // Parse command line arguments
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
    po::notify(vm);

    if (vm.count("help") || !vm.count("rules") || !vm.count("directory")) {
        std::cerr << desc << std::endl;
        return 1;
    }

    std::string input = vm["rules"].as<std::string>();
    std::string filename = vm["directory"].as<std::string>();

    fs::path rules = input;
    fs::path direc = filename;

    std::cout << "Rules: " << rules << std::endl;
    std::cout << "File to refactor: " << direc << std::endl;

    fs::path a = ".";
    show_Directory(a.concat(direc.string()));
    directories(a);
    start_Work(a);
}
