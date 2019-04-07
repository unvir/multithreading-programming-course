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
- [input1](https://github.com/unvir/parallel-programming-course/blob/lab1/input.txt) умножение матриц 2*3 и 3*2 
- [input2](https://github.com/unvir/parallel-programming-course/blob/lab1/input2.txt) умножение матриц 20*40 и 40*30
- [input3](https://github.com/unvir/parallel-programming-course/blob/lab1/input3.txt) умножение матриц 2*60 и 60*2

#### Результаты
[Результат]() выполнения [runner.sh](https://github.com/unvir/parallel-programming-course/blob/lab1/runner.sh)

*Среднее значение по 10 замерам*:

| Потоки | omp input1 | omp input2 | omp input3 | input1 | input2 | input3 |
|--------|------------|------------|------------|--------|--------|--------|
|1|0|0|0|0|0|0|
|2|0|0|0|0|0|0|
|4|0|0|0|0|0|0|
|6|0|0|0|0|0|0|
|10|0|0|0|0|0|0|
