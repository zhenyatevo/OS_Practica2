/*
второе - "ситуация: два человека одновременно нажимают на кнопку, 
лайк" - инкрементирование одной переменной двумя потоками (увидеть ошибку). 
устранить её сначала с помощью mutex, 
затем с помощью atomic, 
сравнить все три реализации по времени.
*/



#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <iomanip>
#include <atomic>

using namespace std;

// Переменная для хранения целого числа
int number;

// Переменные для инкрементирования
int j = 0;
int jj = 0;

// Мьютекс для защиты данных
mutex m1;

// Переменная для атомарного инкрементирования
atomic<int> atomicJ = 0;

// Функция инкрементирования без защиты
void likeIncrement(int number, int& j) {
    for (int i = 0; i < number; i++) {
        j += 1;
    }
}

// Функция инкрементирования с мьютексом
void likeIncrementMutex(int number, int& j) {
    for (int i = 0; i < number; i++) {
        lock_guard<mutex> guard(m1); // Используем lock_guard для автоматического освобождения мьютекса
        j += 1;
    }
}

// Функция инкрементирования с атомарной переменной
void likeIncrementAtomic(int number, atomic<int>& atomicJ) {
    for (int i = 0; i < number; i++) {
        atomicJ += 1;
    }
}

int main() {
    setlocale(LC_ALL, "ru");
    cout << "Введите количество инкрементов: ";
    cin >> number;

    // ПЕРВЫЙ СЛУЧАЙ
    auto start1 = chrono::high_resolution_clock::now();
    thread person1(likeIncrement, number, ref(j));
    thread person2(likeIncrement, number, ref(j));

    person1.join();
    person2.join();
    auto end1 = chrono::high_resolution_clock::now();

    double time_taken1 = chrono::duration_cast<chrono::nanoseconds>(end1 - start1).count();
    time_taken1 *= 1e-9;

    cout << "Time taken для simple is : " << fixed << time_taken1 << setprecision(9) << " sec" << endl;
    cout << "Result j: " << j << endl << "\n";

    // Сброс значения j для следующего теста
    j = 0;

    // ВТОРОЙ СЛУЧАЙ 
    auto start2 = chrono::high_resolution_clock::now();
    thread person3(likeIncrementMutex, number, ref(j));
    thread person4(likeIncrementMutex, number, ref(j));

    person3.join();
    person4.join();
    auto end2 = chrono::high_resolution_clock::now();

    double time_taken2 = chrono::duration_cast<chrono::nanoseconds>(end2 - start2).count();
    time_taken2 *= 1e-9;

    cout << "Time taken для mutex is : " << fixed << time_taken2 << setprecision(9) << " sec" << endl;
    cout << "Result j: " << j << endl << "\n";

    // ТРЕТИЙ СЛУЧАЙ
    auto start3 = chrono::high_resolution_clock::now();
    thread person5(likeIncrementAtomic, number, ref(atomicJ));
    thread person6(likeIncrementAtomic, number, ref(atomicJ));

    person5.join();
    person6.join();
    auto end3 = chrono::high_resolution_clock::now();

    double time_taken3 = chrono::duration_cast<chrono::nanoseconds>(end3 - start3).count();
    time_taken3 *= 1e-9;

    cout << "Time taken для atomic is : " << fixed << time_taken3 << setprecision(9) << " sec" << endl;
    cout << "Result atomicJ: " << atomicJ << endl << "\n";

    return 0;
}
