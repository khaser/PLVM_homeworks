# Итеративный интерпретатор байткода Lama
[Ссылка](https://github.com/PLTools/Lama) на оригинальный репозиторий Lama.

Реализация интерпретатора располагается в `./lamai`.

### Performance

Результаты сравнения производительности с интерпретаторами из upstream репозитория:
```
Upstream (recursive AST interpreter): Sort
Sort    7.55
Upstream (recursive bytecode interpreter): Sort
Sort    3.07
Lamai (iterative bytecode interpreter): Sort
Sort    2.04
```

### Запуск performance тестов
```
make performance
```
### Запуск regression тестов
```
make regression
```
