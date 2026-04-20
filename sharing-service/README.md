# DBus File Sharing Framework

## Описание

Фреймворк для обмена файлами между приложениями через DBus.

Позволяет:
- регистрировать сервисы обработки файлов
- вызывать обработку файлов по формату
- централизовать логику открытия файлов

---

## Архитектура

Система состоит из двух типов сервисов:

### 1. Main Service (`com.system.sharing`)
- центральный координатор
- хранит список сервисов
- принимает регистрацию

### 2. Sharing Services (например `com.system.sharing.text`)
- обрабатывают файлы
- регистрируются в main service
- реализуют метод `OpenFile`

---

## Взаимодействие

1. Сервис запускается
2. Регистрируется в `com.system.sharing`
3. Клиент вызывает `OpenFile`
4. Сервис обрабатывает файл

---

## Сборка

```bash
mkdir build
cd build
cmake ..
make
```

## Запуск

### 1. Запуск main service
```bash
./sharing-service
```

### 2. Запуск example service
```bash
./example-service
```

## Тестирование

### Вызов метода
```bash
gdbus call --session \
  --dest com.system.sharing.text \
  --object-path / \
  --method com.system.sharing.text.OpenFile \
  "/tmp/test.txt"
```
### Ожидаемый результат
```bash
Opening file: /tmp/test.txt
File opened successfully
```

### Проверка ошибки
```bash
gdbus call --session \
  --dest com.system.sharing.text \
  --object-path / \
  --method com.system.sharing.text.OpenFile \
  ""
```

## Структура проекта
```bash
lib/                # библиотека фреймворка
example/            # пример сервиса
sharing-service/    # основной сервис
include/            # заголовки
```

## Использование
Пример создания сервиса:

```bash
SharingService service(
    "com.system.sharing.text",
    {"txt"},
    [](Request& req) {
        std::cout << req.getPath() << std::endl;
    }
);

service.start();
```

### Особенности
-* Использует sdbus-c++
-* Обработка ошибок через исключения
-* Простое API для интеграции

## Автор
Батраз Дзесов