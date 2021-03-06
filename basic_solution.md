# Базовое решение Fast Codesearch

## Основная задача
Мы хотим реализовать поисковый движок по коду. Его основная задача - искать регулярные выражения в определенном наборе файлов.
То есть, мы должны уметь передавать в наш движок набор файлов,
по которому мы хотим производить поиск. И уметь искать регулярные выражения в этих файлах.

## Общее решение
Поисковый движок будет работать следующим образом:
- Можно будет настроить конфиги, которые влияют на логику движка
- Движок можно будет включить
- Можно добавить/удалить файлы по которым движок будет искать
- С помошью движка можно будет искать регулярные выражения, среди файлов, которые были добавлены в него 
- Движок можно будет выключить 
- Сам движок будет строить индекс при включении 
- Далее раз в какое-то время движок будет сравнивать набор файлов, на котором построен индекс и текущий набор необходимых файлов в движке, и если они не совподают, то движок будет перестраивать индекс. Интервал такой проверки определяется конфигом 

## Механизм конфигов

В нашей задаче мы хотим уметь гибко настраивать параметры нашего поиска.
Например, частота обновления индекса, библиотека поиска регулярных выражений, набор форматов файлов,
которые алгоритм будет игнорировать и пр. Для этого у нас будет механизм конфигов.

Сам из себя он будет представлять набор констант, которые будут входить в мета-информацию нашего индекса.
Будет специальная утилита, которая будет показывать все конфиги в виде *название* - *описание* - *текущее значение*.
Также с помощью нее мы сможем задавать конкретному конфигу новое значение. Интерфейс работы с этой утилитой описан в разделе **CLI**.

Полный список конфигов можно посмотреть в разделе **конфиги**.

## Построение/Обновление индекса
Построение/обновление индекса происходит в одной из двух ситуаций:
- Движок только что запустили
- Произошло сравнение наборов файлов(далее снэпшот), на которых построен текущий индекс и файлов, по которым мы должны искать, и эти наборы отличаются
- При обновлении значений конфигов, надо перепостроить индекс

Важный момент - мы никак не ограничиваем количество файлов, поэтому мы будем хранить несколько индексов.
Каждый индекс по сути является шардом. Количество файлов, которые может поместиться на одном шарде, задается конфигом.
Всю информацию об индексах мы будем хранить в мета-информации. При этом хочу заметить, что чем шардов меньше, тем быстрее мы будем искать.

Чтобы работать с шардами мы будем хранить мета информацию о них - связь file -> shard,
текущее количество шардов и количество файлов на шардах.

```
class Shard {
   private:
       std::unordered_map<std::string, int> shard;
       int shards_cnt = 0;
       std::vector<int> files_cnt_on_shrads = {};
   };
``` 


Данные о триграммах предлагаю хранить в unordered_map trigram -> unordered_set(files). Тогда индекс на шарде будет выглядить так:
```
class Index {
private:
    std::unordered_map<int, std::unordered_set<std::string>> index;
};

```

## Алгоритм построения индекса
Изначально можно считать, что индекс пустой, и нам его надо обновить. Поэтому построение будет основано алгоритме обновления индекса. 

## Алгоритм обновления индекса
Обновление индекса у нас просиходит, когда мы видим, что снэпшот файлов индекса был изменен. Такая проверка снэпшота будет просиходить раз в какое-то время,
задающееся конфигом. Если такой изменения есть, то мы начинаем перестраивать индекс. 

Само изменение снэпшота может быть двух типов:
- добавили какой-то файл
- удалили какой-то файл

### Добавление файла
Перед тем, как добавить файл в индекс, нам необходимо для начала определить на какой шард мы будем это делать. Мы будем стараться минимизировать количество шардов,
поэтому сначала попробуем найти какой-нибудь уже существующий шард, который не будет полностью заполнен. Если у нас это получилось, то добавляем файл на этот шард,
если же все шарды уже полностью заняты, то создадим новый шард и добавим файл в него.

Чтобы добавить файл в индекс, нам необходимо пройтись по файлу и прочитать все триграммы в нем, при этом обновляя мапку trigram -> unordered_set(files).

### Удаление файла
Для удаления файла из индекса нам необходимо из мета-информации шардов получить номер шарда, на котором хранится нужный файл, и далее пройтись по мапке шарда,
удаляя необходимый файл. 

## Алгоритм перестроения индекса
Перестроение индекса будет происходить, когда мы обновляем какой-либо конфиг.
Чтобы перестроить индекс мы будем удалять все текущие шарды, и по снэпшоту заново строить индекс.

## Поиск регулярки

Чтобы с помощью индекса определить в каких файлах содержится входящая регулярка, надо на кажом из шардов с помощью алгоритма,
описанного в статье [Regular Expression Matching with a Trigram Index](https://swtch.com/~rsc/regexp/regexp4.html),
получить id файлов, в которых содержится это выражение, а потом мы делаем mmap этих файлов и полный поиск в них по этому
выражению с помощью библиотеки, которая указана в конфиге(RE2 или Hyperscan).

## Выдача результатов
Результат, который мы получим после полного поиска по файлам необходимо отсортировать по количеству встречаемости регулярного варжения в файлах.
Также в конфигах можно будет настроить CAP по количеству результатов, которые выведет поиск. 

## Многопоточность
Поиск по триграммам на разных шардах никак не связан, поэтому мы можем делать это асинхронно.

## CLI

### configs
Интерфейс для утилиты, работающей с конфигами, выглядит следующим образом:

-- Показать все конфиги
```
./configs show
UPDATE_INTERVAL_SEC Период обновления индекса(в секундах) 3600
IGNORE_FORMATS_LIST Список форматов файлов, которые индекс игнорирует ['.git', '.svn']
```

-- Обновить значения конфига
```
./configs update UPDATE_INTERVAL_SEC 4000
config updated
UPDATE_INTERVAL_SEC Период обновления индекса(в секундах) 4000
```

### fastsearch
Интерфейс для поискового движка выглядит следующим образом:

-- Запустить движок
```
./fastsearch start
Fast Codesearch started
```

-- Остановить движок
```
./fastsearch stop
Fast Codesearch stopped
```

-- Добавить файлы по указанному пути в движок
```
./fastsearch add /files
Files added
```

-- Удалить файлы по указанному пути в движок
```
./fastsearch delete /files
Files deleted
```
## Конфиги
- UPDATE_INTERVAL_SEC - промежуток времени, через которое сравниваем снэпшоты и при необходимости обновляем индекс
- IGNORE_FORMATS_LIST - типы файлов, которые не идексируются
- CNT_FILES_IN_SHARD - количество файлов, которое вмещает шард 
- RE_SEARCH_IN_FILES - библиотека поиска регулярок в файле(RE2 или Hyperscan)
- RESULT_CAP - максимальное количество выдачи результа по поиску выражения

## Сборка
Для сборки будет использовать cmake.

## Тестирование
Для тестирования будет использоваться несколько открытых кодовых баз:
- [ClickHouse](https://github.com/ClickHouse/ClickHouse)
- [NumPy](https://github.com/numpy/numpy)
- [netdata](https://github.com/netdata/netdata)
- [electron](https://github.com/electron/electron)

Сам код будет тестироваться с помощью библиотеки Gtest. Также будет замеряться скорость построения индекса и поиска по нему.

## Дальнейшее развитие
Необходимо прикрутить ctags

## Этапы разработки

### Прототип
Первый этап разработки подразумевает, что у нас есть минимально работающий прототип движка, а именно:
- умеем строить и обновлять индекс
- умеем делать поиск регулярных выоражений
- умеем добавлять, удалять файлы в поиск движка
- поддержка конфигов UPDATE_INTERVAL_SEC и CNT_FILES_IN_SHARD 
- Полный поиск по файлу происходит с помощью RE2(или Hyperscan, определяться будет на этапе разработки)
- Написано несколько простых тестов

### Доработка №0
- unorder_map заменен на более быструю структуру absl::flat_hash_map

### Доработка №1
- реализована сортировка результат по количеству вхождений
- поддержка конфига RESULT_CAP

### Доработка №2
- написана утилита ./configs для удобной регулировки значений конфигом
- движок корректно реагирует на изменение конфигов во время работы

### Доработка №3
- поддержка конфига IGNORE_FORMATS_LIST

### Доработка №4
- поддержка конфига RE_SEARCH_IN_FILES

### Тестирование
- написаны тесты над большими кодовыми базами

### Замеры скорости
- замеряется скорость построения индекса и поиска по нему

### Доработка №5
- прикручены ctags
