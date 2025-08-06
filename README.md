# FileToClipboard-RedSoft

## Dependencies

- `cmake`
- `g++`

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
    - absoulte file path for `FILE` data format
    - UTF-8 characters string for `STRING` data format

## License

This project is licensed under HOT POTATO LICENSE V2