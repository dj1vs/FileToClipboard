# FileToClipboard-RedSoft

## Supported Platforms

Дистрибутивы Linux с X-Server или Wayland.

Программа поддерживает работу в режиме сервера, поэтому наличие в системе clipboard manager-а не обязательно.

Возможны небольшие неточности касательно поддерживаемых mime-типов.

## Dependencies

- `cmake`
- `g++`
- `libx11-dev`

## Building

```bash
cmake -S . -B build
cmake --build build
```

## Running

```bash
build/FileToClipboard [data_format] [data]
```

- `[data_format]` - `FILE` or `STRING`
- `[data]`:
    - `FILE`: absoulte file path
    - `STRING`: UTF-8 characters string

## License

This project is licensed under HOT POTATO LICENSE V2