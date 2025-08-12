#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <locale>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#endif

// ANSI-коды для очистки экрана
void clearScreen() {
    std::cout << "\033[2J\033[1;1H";
}

// "Квадратный" шрифт для цифр и двоеточия, как на картинке
const std::vector<std::string> font[] = {
    {" #### ", "#    #", "#    #", "#    #", " #### "}, // 0
    {"   #  ", "  ##  ", "   #  ", "   #  ", " #####"}, // 1
    {" #### ", "     #", " #### ", "#     ", " #####"}, // 2
    {" #### ", "     #", " #### ", "     #", " #### "}, // 3
    {"#    #", "#    #", " #####", "     #", "     #"}, // 4
    {" #####", "#     ", " #### ", "     #", " #### "}, // 5
    {" #### ", "#     ", " #### ", "#    #", " #### "}, // 6
    {" #####", "     #", "    # ", "   #  ", "   #  "}, // 7
    {" #### ", "#    #", " #### ", "#    #", " #### "}, // 8
    {" #### ", "#    #", " #### ", "     #", " #### "}, // 9
    {"      ", "  ##  ", "      ", "  ##  ", "      "}  // : (двоеточие)
};

// Функция для получения ширины окна терминала
int getConsoleWidth() {
    #ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        return csbi.srWindow.Right - csbi.srWindow.Left + 1;
    #else
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        return w.ws_col;
    #endif
}

// Функция для получения высоты окна терминала
int getConsoleHeight() {
    #ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    #else
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        return w.ws_row;
    #endif
}

// Функция для скрытия курсора
void hideCursor() {
    std::cout << "\033[?25l";
}

// Функция для отображения курсора
void showCursor() {
    std::cout << "\033[?25h";
}

// Функция для отображения времени в "квадратном" шрифте
void drawTime(const std::tm& timeInfo) {
    clearScreen();
    hideCursor();

    // Высота блока: 5 строк для цифр + 1 строка для даты + 2 пустые строки = 8 строк
    const int blockHeight = 8;
    const int consoleHeight = getConsoleHeight();
    const int verticalPadding = (consoleHeight - blockHeight) / 2;

    for (int i = 0; i < verticalPadding; ++i) {
        std::cout << "\n";
    }

    // Ширина блока
    const int clockWidth = 68; // Ширина 4-х цифр + двоеточие + 4 пробела между ними
    const int consoleWidth = getConsoleWidth();
    const int horizontalPadding = (consoleWidth - clockWidth) / 2;
    std::string indent(horizontalPadding, ' ');
    
    // Рисуем время
    for (int i = 0; i < 5; ++i) {
        std::cout << indent;

        // Первая цифра часа
        for (char c : font[timeInfo.tm_hour / 10][i]) {
            std::cout << (c == '#' ? "\033[44m  \033[0m" : "  ");
        }
        std::cout << "  ";
        // Вторая цифра часа
        for (char c : font[timeInfo.tm_hour % 10][i]) {
            std::cout << (c == '#' ? "\033[44m  \033[0m" : "  ");
        }
        std::cout << "  ";

        // Двоеточие
        for (char c : font[10][i]) {
            std::cout << (c == '#' ? "\033[44m  \033[0m" : "  ");
        }
        std::cout << "  ";

        // Первая цифра минуты
        for (char c : font[timeInfo.tm_min / 10][i]) {
            std::cout << (c == '#' ? "\033[44m  \033[0m" : "  ");
        }
        std::cout << "  ";
        // Вторая цифра минуты
        for (char c : font[timeInfo.tm_min % 10][i]) {
            std::cout << (c == '#' ? "\033[44m  \033[0m" : "  ");
        }

        std::cout << "\n";
    }

    // Центрируем и выводим дату
    std::string dateString = std::to_string(timeInfo.tm_mday) + "." + std::to_string(timeInfo.tm_mon + 1) + "." + std::to_string(timeInfo.tm_year + 1900);
    const int datePadding = (getConsoleWidth() - dateString.length()) / 2;
    std::cout << "\n\n" << std::string(datePadding, ' ') << dateString << "\n";
}

// Функция для отображения приветствия
void showSplashScreen() {
    clearScreen();
    hideCursor();
    const std::string welcomeText = "Welcome to Aselerada";
    const int consoleWidth = getConsoleWidth();
    const int consoleHeight = getConsoleHeight();
    const int verticalPadding = (consoleHeight - 1) / 2;
    const int horizontalPadding = (consoleWidth - welcomeText.length()) / 2;

    for (int i = 0; i < verticalPadding; ++i) {
        std::cout << "\n";
    }
    std::cout << std::string(horizontalPadding, ' ') << welcomeText << "\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
    clearScreen();
    showCursor();
}

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    showSplashScreen();

    while (true) {
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::tm* local_time = std::localtime(&now_c);

        drawTime(*local_time);

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}
