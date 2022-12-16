#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <semaphore>
#include <algorithm>
#include <vector>
#include <ctime>

// Текущее количество читателей файла.
int viewers = 0;
// Количество выполненных задач.
int completed = 0;

// Состояние файла: открыт или закрыт.
bool open_ = true;

// Семафор для контроля одновременного подклюлчения редакторов.
std::binary_semaphore semaphore(0);

// База данных.
std::vector<int> data;

// Метод работы писателя.
void* redactorJob(void* arg) {
    semaphore.acquire(); // Пытаемся уменьшить семафор.
    srand( time( 0 ) );
    std::cout << "redactor join\n";
    open_ = false; // Закрываем доступ для читателей.
    while (viewers != 0) { // Ждем пока все читатели заблокируются.
    }
    std::cout << "redactor start working\n";
    int n = rand() % 10;
    data.push_back(n);
    std::sort(data.begin(), data.end());
    std::cout << "redactor add " << n << " to data\n";
    sleep(rand() % 3);
    std::cout << "redactor's job done!\n";
    open_ = true; // Открываем доступ для читателей.
    semaphore.release(); // Увеличиваем семафор.
    completed++; // Записываем выполненную задачу.
}

// Метод работы читателя.
void* viewerJob(void* arg) {
    while (!open) { // Ждем, пока доступ не откроется.
    }
    srand( time( 0 ) );
    viewers++; // Увеличиваем количество активных читателей.
    std::cout << "viewer join\n";
    for (int i = 0; i < 5; ++i) {
        sleep(rand() % 3);
        if (!open_) { // Проверяем открыт ли доступ.
            std::cout << "viewer blocked\n";
            viewers--;
            while (!open_) { // Ждем пока доступ откроется.
            }
            std::cout << "viewer unblocked\n";
            viewers++;
            i--;
            continue;
        }
        int n = data[rand() % data.size()];
        std::cout << "viewer find " << n <<  " in data. viewer's job done for " << 20*(i + 1) << "%\n";
    }
    std::cout << "viewer's job done!\n";
    viewers--; // Уменьшаем количество активных читателей.
    completed++; // Записываем выполненную задачу.
}

int main() {
    srand( time( 0 ) );
    int n = 0;
    int x;
    std::cin >> n;
    for (int i = 0; i < n; ++i) {
        std::cin >> x;
        data.push_back(x);
    }
    char ch;
    std::cin >> n;
    pthread_t thread[n];
    for (int i = 0; i < n; ++i) {
        sleep(rand() % 2);
        std::cin >> ch;
        if (ch == 'R') {
            pthread_create(&thread[i], NULL, redactorJob, NULL);
        }
        if (ch == 'V') {
            pthread_create(&thread[i], NULL, viewerJob, NULL);
        }
    }
    semaphore.release();
    while (completed != n) { // Ждем, пока не выполнятся все задачи.
        // sleep(rand() % 500);
        // std::cout << "viewers at the moment: " << viewers << "\n";
    }
}
