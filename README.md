# FileToClipboard-RedSoft

## Supported Platforms

Linux distributions with X-Server or Wayland.

Running in server mode is supported so external clipboard manager is not necessary.

Minor compatability bugs with supported MIME types are possible.

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