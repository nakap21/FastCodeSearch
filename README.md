# Дипломная работа Fast Codesearch

Накапкин Александр, НИУ ВШЭ БМПИ176

## Требования к системе
1. CMake >= 3.11
2. gcc >= 8

## Скачивание и сборка
Чтобы скачать и собрать проект небходио выполнить следующие команды:
```
git clone https://github.com/nakap21/FastCodeSearch.git
cd FastCodeSearch
mkdir build
cd build 
cmake ..
make -j $N // количество ядер минус один
```

После выполнения этих команд в папке FastCodeSearch/build/utils будут лежать необходимые утилиты, а именно: ./start, ./configs, ./files, ./search, ./stop.
