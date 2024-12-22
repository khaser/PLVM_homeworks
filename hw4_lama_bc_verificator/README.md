# Интерпретатор Lama с предварительной верификацией
[Ссылка](https://github.com/PLTools/Lama) на оригинальный репозиторий Lama.

Реализация интерпретатора/верификатора располагается в `./lamav`.

### Сборка
```
make -C lamav
```
### Запуск
```
./lamav/lamav test.bc
```

### Сравнение времени работы
Сравнение интерпретатора на тесте `performance/Sort.lama` по лучшему времени работы на 5ти запусках.
```
Upstream (recursive AST interpreter): 7.55 s
Upstream (recursive bytecode interpreter): 3.07 s
Lamai (runtime validation): ~1.95 s
Lamav (preprocess validation): ~1.87 s
```
