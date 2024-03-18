#include <iostream>
#include <windows.h>
#include <vector>
#include <chrono>
#include <thread>

using namespace std;

const int N = 5; // Количество буферов в пуле
vector<int> bufferPool(N);
vector<int> dataVector; // Вектор для хранения данных

CRITICAL_SECTION criticalSection; // Критическая секция

bool isBufferFull() {
    for (int i = 0; i < N; ++i) {
        if (bufferPool[i] == 0) {
            return false;
        }
    }
    return true;
}

bool isBufferEmpty() {
    for (int i = 0; i < N; ++i) {
        if (bufferPool[i] != 0) {
            return false;
        }
    }
    return true;
}

void writer() {
    int data = 1;
    while (true) {
        while (isBufferFull()) {
            // Ждем, пока появится хотя бы один свободный буфер
            this_thread::sleep_for(chrono::milliseconds(10));
        }

        EnterCriticalSection(&criticalSection); // Вход в критическую секцию

        // Ищем свободный буфер и пишем данные
        for (int i = 0; i < N; ++i) {
            if (bufferPool[i] == 0) {
                bufferPool[i] = data++;
                cout << "Данные записаны." << endl;
                break;
            }
        }

        LeaveCriticalSection(&criticalSection); // Выход из критической секции
    }
}

void reader() {
    while (true) {
        while (isBufferEmpty()) {
            // Ждем, пока появится хотя бы одна запись в буфере
            this_thread::sleep_for(chrono::milliseconds(10));
        }

        EnterCriticalSection(&criticalSection); // Вход в критическую секцию
        
        // Считываем данные из буферов
        for (int i = 0; i < N; ++i) {
            if (bufferPool[i] != 0) {
                int data = bufferPool[i];
                bufferPool[i] = 0;
                dataVector.push_back(data); // Добавляем данные в вектор
                cout << "Данные прочитаны: " << data << endl;
                this_thread::sleep_for(chrono::seconds(1)); // Предполагаем, что чтение занимает некоторое время
            }
        }

        LeaveCriticalSection(&criticalSection); // Выход из критической секции
    }
}

int main() {
    InitializeCriticalSection(&criticalSection); // Инициализация критической секции
    //TryEnterCriticalSection(&criticalSection);
    thread writerThread(writer);
    thread readerThread(reader);

    writerThread.join();
    readerThread.join();

    //DeleteCriticalSection(&criticalSection); // Удаление критической секции

    return 0;
}
