## ЛР1 - Многопоточное программирование

```
Необходимо распараллелить умножение матриц используя OpenMP
```

#### Запускалось на
- ОС: **Ubuntu 16.04.4 LTS**
- Процессор: **AMD Ryzen 3 2200G with Radeon Vega Graphics**
  - Ядер: **4**
  - Потоков: **4**
  
#### Тест кейсы
- [input1](https://github.com/unvir/parallel-programming-course/blob/lab1/input.txt) умножение матриц 2x3 и 3x2 
- [input2](https://github.com/unvir/parallel-programming-course/blob/lab1/input2.txt) умножение матриц 20x40 и 40x30
- [input3](https://github.com/unvir/parallel-programming-course/blob/lab1/input3.txt) умножение матриц 2x60 и 60x2

Запуск программы на каждом тестовом кейсе подразумевает **100000** умножений приведенных матриц.
*Команды для компиляции*:
```console
g++ -o omp_inputN -std=gnu++11 -fopenmp main.cpp
```
```console
g++ -o inputN -std=gnu++11 main.cpp
```

#### Результаты
[Результат](https://github.com/unvir/parallel-programming-course/blob/lab1/runner.out) выполнения [runner.sh](https://github.com/unvir/parallel-programming-course/blob/lab1/runner.sh)

*Среднее значение по 10 замерам (в секундах)*:

| Потоки | omp input1 | omp input2 | omp input3 | input1 | input2 | input3 |
|--------|------------|------------|------------|--------|--------|--------|
|1|0.048208|17.7966|0.215351|0.0260543|15.9993|0.176758|
|2|0.102437|11.3339|0.260857|0|0|0|
|4|0.125671|6.91229|0.303936|0|0|0|
|6|1.28173|7.46951|1.14831|0|0|0|
|10|1.83022|7.72546|1.80828|0|0|0|
