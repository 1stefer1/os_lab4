#include <iostream>
#include <vector>
#include <thread>
#include <windows.h>

using namespace std;

const int N = 10; 

vector<int> buffer(N); 
int readIndex = 0; 
int writeIndex = 0; 

CRITICAL_SECTION cs; 
int numFreeBuffers = N; 
int numUsedBuffers = 0; 

bool isBufferFull() {
    return numFreeBuffers == 0;
}

bool isBufferEmpty() {
    return numUsedBuffers == 0;
}

void writer() {
    int count = 0;
    while (count <= 50) {
        EnterCriticalSection(&cs);
        while (isBufferFull()) {
            LeaveCriticalSection(&cs);
            Sleep(100);
            EnterCriticalSection(&cs);
        }
        int value = rand() % 100;
        buffer[writeIndex] = value;
        cout << "Записано значение " << value << " в буфер " << writeIndex << std::endl;
        writeIndex = (writeIndex + 1) % N;
        numFreeBuffers--;
        numUsedBuffers++;
        LeaveCriticalSection(&cs);
        Sleep(rand() % 1000);
        count += 1;
    }
}

void reader() {
    int count = 0;
    while (count <= 50) {
        EnterCriticalSection(&cs);
        while (isBufferEmpty()) {
            LeaveCriticalSection(&cs);
            Sleep(100);
            EnterCriticalSection(&cs);
        }
        int value = buffer[readIndex];
        cout << "Прочитано значение " << value << " из буфера " << readIndex << std::endl;
        readIndex = (readIndex + 1) % N;
        numFreeBuffers++;
        numUsedBuffers--;
        LeaveCriticalSection(&cs);
        Sleep(rand() % 1000);
        count += 1;
    }
}

int main() {
    InitializeCriticalSection(&cs);

    thread writerThread(writer);
    thread readerThread(reader);

    writerThread.join();
    readerThread.join();

    //DeleteCriticalSection(&cs);

    return 0;
}
