# gmk

**gmk** is a tool to generate a Makefile from a TOML configuration file (`project.toml`).

It aims to simplify project setup and build management for C projects by using declarative configuration.

## Features

- Generate a Makefile from a TOML file describing your project.
- Initialize a new project structure with `src/`, `include/`, and a sample `main.c`.
- Supports custom output filenames and TOML input files via command-line arguments.
- MIT licensed.

## Usage

```
gmk [options]
```

### Options

- `-h`, `--help` : Display help message.
- `-v`, `--version` : Show version information.
- `-i`, `--init` : Generate a sample `project.toml` configuration and scaffold directories.
- `-o`, `--output <file>` : Set the output Makefile name (default: `Makefile`).
- `-t`, `--toml <file>` : Set the TOML configuration file to use (default: `project.toml`).

### Example

1. Initialize a new project:
   ```
   gmk --init
   ```
   This creates a sample `project.toml`, `src/main.c`, and `include/`.

2. Edit `project.toml` to describe your project.

3. Generate the Makefile:
   ```
   gmk
   ```
   or specify custom files:
   ```
   gmk -t myproject.toml -o CustomMakefile
   ```

## Project Structure

After initialization:
```
project/
├── include/
├── src/
│   └── main.c
├── project.toml
└── Makefile
```

## Dependencies
- argtable2
- tomlc99
- Makefile

## Building
```
git clone --recurse-submoduleshttps://github.com/noa-vliz/gmk
cd gmk
make
```

## License

[MIT License](./LICENSE)
