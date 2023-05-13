#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/process.hpp>
#include <boost/algorithm/string.hpp>

#include <fstream>
#include <iostream>
#include <list>
#include <memory>
#include <unistd.h>

using btcp = boost::asio::ip::tcp;
namespace fs = boost::filesystem;

struct Connection //объект соединения клиента и сервера
{
    btcp::socket socket; //сокет
    boost::asio::streambuf read_buffer;//буфер чтения
    Connection(boost::asio::io_service &io_service) : socket(io_service), read_buffer() {}//конструктор для инициализации
};

class Server
{
    boost::asio::io_service ioservice;//ввод\ввывод в boost::asio
    btcp::acceptor acceptor;//слушает заданный порт и принимае новые соединения
    std::list<Connection> connections;// список соединений

public:
    Server(uint16_t port) : acceptor(ioservice, btcp::endpoint(btcp::v4(), port)) { start_accept(); }

    void on_read(std::list<Connection>::iterator con_handle, boost::system::error_code const &err, size_t bytes_transfered) //метод чтения сообщений
    {
        if (bytes_transfered > 0)//если количесво прочитанных байт больше 0
        {
            std::istream is(&con_handle->read_buffer);// буфер чтения
            std::string line;
            std::getline(is, line);// считываем строку из потока в переменную line
            std::cout << line << std::endl;
                if (line =="username\r"||line == "hostname\r" || line == "serverdatetime\r" || line.substr(0, 7) == "mkfile "
                ||line.substr(0, 6) == "mkdir " || line.substr(0, 3) == "ls " || line == "quit\r"
                ||line.substr(0, 6) == "rmdir " || line.substr(0, 7) == "rmfile "
                || line.substr(0, 9) == "readfile "){
                handle_command(con_handle,line.substr(0,line.length()-1)) ;
                //проверка на команды внутри сообщений
            }
            std::cout << "Message Received: " << line << std::endl;
        }

        if (!err) //если ошибок нет,
        {
            do_async_read(con_handle);//продолжаем асинхронное чтение
        }
        else //обработка ошибок
        {
            std::cerr << "Error: " << err.message() << std::endl;
            connections.erase(con_handle);//удаляем соединение
        }
    }

    void do_async_read(std::list<Connection>::iterator con_handle) //асинхронное чтение из сокета соединения
    {
        auto handler = boost::bind(&Server::on_read, this, con_handle, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred);
        //асинхронно читаем из сокета con_handle->socket, в буфер con_handle->read_buffer, затем handler
        boost::asio::async_read_until(con_handle->socket, con_handle->read_buffer, "\n", handler);
    }

    void on_write(std::list<Connection>::iterator con_handle, std::shared_ptr<std::string> msg_buffer, boost::system::error_code const &err)
    {//вызывается после завершения записи
        if (!err)
        {//если не было ошибки при записи
            std::cout << "Finished sending message\n";
        }
        else
        {//есил была
            std::cerr << "Error: " << err.message() << std::endl;
            connections.erase(con_handle);//удаление соединения из списка
        }
    }

    void on_accept(std::list<Connection>::iterator con_handle, boost::system::error_code const &err)
    {//после успешного нового соединения в start_accept
        if (!err)
        {
            //инфа о новом соединение
            std::cout << "Connection from: " << con_handle->socket.remote_endpoint().address().to_string() << "\n";
            std::cout << "Sending message\n";
            auto buff = std::make_shared<std::string>("Hello Client!\n\n\n");//создаем шаред птр на объект чтобы управлять временем жизни и удялять при отсутвие ссылок (данные живут только до отправки) + асинхронная работа
            //хендлер для вызова on_write при завершении операции
            auto handler = boost::bind(&Server::on_write, this, con_handle, buff, boost::asio::placeholders::error);
            //запись содержимого в buff
            boost::asio::async_write(con_handle->socket, boost::asio::buffer(*buff), handler);
            do_async_read(con_handle);//читаем данные из сокета
        }
        else //обработка ошибки
        {
            std::cerr << "Error: " << err.message() << std::endl;
            connections.erase(con_handle);//ужаляем соеднинение
        }
        start_accept();
    }

    void start_accept() // начало принятия сообщений
    {
        //создание нового элемента списка connections связанный с новым сокетом
        auto con_handle = connections.emplace(connections.begin(), ioservice);
        //хендлер для вызова on_accept
        auto handler = boost::bind(&Server::on_accept, this, con_handle, boost::asio::placeholders::error);
        acceptor.async_accept(con_handle->socket, handler);//ожидание нового соединения
    }

    std::string get_server_datetime() // метод проверки времени на сервере
    {
        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        std::string datetime = std::ctime(&now_time_t);
        // Удаляем символ перевода строки в конце строки
        datetime.erase(std::remove(datetime.begin(), datetime.end(), '\n'), datetime.end());
        return datetime;
    }

    void handle_command(std::list<Connection>::iterator con_handle , const std::string& cmd) //обработчик специальных команд
    {
        std::string response; // переменная для ответа
        if (cmd == "username") { // если строка == username
            response = getlogin() ; // узнаем логин ползователя
            std::cout << "Login: " << response << std::endl;
        } else if (cmd == "hostname") {
            char buf[256]; // массив чаров для записи имени хоста
            gethostname(buf,256); // имя хоста записываем в файл
            response =  buf;
            std::cout << "Host Name: " << buf << std::endl;
        } else if (cmd == "serverdatetime") { // обработка команды запроса времени
            response = "Server datetime: " + get_server_datetime() + "\n";
            std::cout << response << std::endl;
        } else if (cmd.substr(0, 3) == "ls ") { //проверяем первые 3 символа строки, если они == ls , то обрабатываем
            boost::process::ipstream out;//создание потока для чтения вывода команды
            boost::process::system(cmd, boost::process::std_out > out);// выполнение команды и запись ответа в out
            std::stringstream response_stream; //создание потока для записи вывода в переменную response_stream
            response_stream << out.rdbuf();// запись вывода команды в переменную response_stream
            response = response_stream.str(); //вывод в терминал
        } else if (cmd.substr(0, 7) == "mkfile ") {
            std::string path = cmd.substr(7);// запишем в переменную имя файла для красивого вывода
            boost::process::system(cmd);// ну и просто выполняем команду mkfile + path
            response = "File created: " + path + "\n";
        } else if (cmd.substr(0, 6) == "mkdir ") {
            std::string path = cmd.substr(6);
            boost::process::system(cmd); // опять же команда в теринал
             //решил попробовать так, а не через  fs::create_directory(path);
            response = "Directory created: " + path + "\n";
        }else if (cmd.substr(0, 7) == "rmfile ") {
                std::string path = cmd.substr(7); //запишем в переменную имя файла
                std::string command = "rm " + path;// команда, которую будем вводить в терминал
                boost::process::system(command);
                response = "File removed: " + path + "\n";
        } else if (cmd.substr(0, 6) == "rmdir ") { // аналогично
                std::string path = cmd.substr(6);
                std::string command = "rm -rf " + path; // немного отличается команда
                boost::process::system(command);
                response = "Directory removed: " + path + "\n";
        } else if (cmd.substr(0, 9) == "readfile ") {
            std::vector<std::string> tokens;//создадим вектор, что бы удобно разделить команду на 3 части (readfile, имя файла,размер чтения*если есть*)
            boost::algorithm::split(tokens,cmd,boost::is_any_of(" "));// такой вот метод сплита :)
            if (tokens.size() >= 2) { //проверка на наличие имени файла
                std::string path = tokens[1]; //выносим в переменную имя файла
                int num_bytes = -1; // переменная для размера чтения (-1, чтобы начать с 0)
                if (tokens.size() == 3) {//проверка на присутсвия параметра размера чтения
                    num_bytes = std::stoi(tokens[2]); //запись размера в переменную
                }
                std::ifstream file(path);// открываем файл
                if (file.is_open()) {//проверка открытия
                    std::stringstream buffer;
                    buffer << file.rdbuf();
                    std::string file_contents = buffer.str(); // запись данного количества байт в переменную
                    if (num_bytes > 0 && num_bytes < file_contents.length()) { // если указано меньше, чем размер файла
                        file_contents = file_contents.substr(0, num_bytes);// сокращаем file_contents до нужного размера
                    }
                    response = file_contents;
                    file.close();//закрываем файл
                } else {//если не открылся
                    response = "Error: Could not open file " + path + "\n";
                }
            } else {//если нет имени
                response = "Error: readfile command requires a file path argument.\n";
            }
        } else if (cmd == "quit") { //выключаем сервер
                response = "Goodbye!\n";
                std::cout << response;
            ioservice.stop(); //выключение сервера
    } else {
            response = "Invalid command\n"; //если  команда не подходит под условия
        }
        //создание шарпера прт, который содержит стироку response
        auto buff = std::make_shared<std::string>(response);
        //создаем функцию-обработчик, которая будет срабатывать после того, как данные будут записаны в сокет
        //+ различные параметры для обработыки
        auto handler = boost::bind(&Server::on_write, this, con_handle, buff, boost::asio::placeholders::error);
        //асинхронная запись данных из *buff (response) в сокет связанный с соединением con_handle
        //при завершении вызывается handler
        boost::asio::async_write(con_handle->socket, boost::asio::buffer(*buff), handler);
    }

    void run() // запуск сервера
    {
        ioservice.run();
    }
};

int main() {
    Server srv(12345); //хост сервера
    srv.run();//запускаем сервер
    return 0;
}