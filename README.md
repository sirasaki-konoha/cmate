# cate

[![DeepWiki](https://img.shields.io/badge/DeepWiki-rock--db%2Fcate-blue.svg?logo=data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACwAAAAyCAYAAAAnWDnqAAAAAXNSR0IArs4c6QAAA05JREFUaEPtmUtyEzEQhtWTQyQLHNak2AB7ZnyXZMEjXMGeK/AIi+QuHrMnbChYY7MIh8g01fJoopFb0uhhEqqcbWTp06/uv1saEDv4O3n3dV60RfP947Mm9/SQc0ICFQgzfc4CYZoTPAswgSJCCUJUnAAoRHOAUOcATwbmVLWdGoH//PB8mnKqScAhsD0kYP3j/Yt5LPQe2KvcXmGvRHcDnpxfL2zOYJ1mFwrryWTz0advv1Ut4CJgf5uhDuDj5eUcAUoahrdY/56ebRWeraTjMt/00Sh3UDtjgHtQNHwcRGOC98BJEAEymycmYcWwOprTgcB6VZ5JK5TAJ+fXGLBm3FDAmn6oPPjR4rKCAoJCal2eAiQp2x0vxTPB3ALO2CRkwmDy5WohzBDwSEFKRwPbknEggCPB/imwrycgxX2NzoMCHhPkDwqYMr9tRcP5qNrMZHkVnOjRMWwLCcr8ohBVb1OMjxLwGCvjTikrsBOiA6fNyCrm8V1rP93iVPpwaE+gO0SsWmPiXB+jikdf6SizrT5qKasx5j8ABbHpFTx+vFXp9EnYQmLx02h1QTTrl6eDqxLnGjporxl3NL3agEvXdT0WmEost648sQOYAeJS9Q7bfUVoMGnjo4AZdUMQku50McDcMWcBPvr0SzbTAFDfvJqwLzgxwATnCgnp4wDl6Aa+Ax283gghmj+vj7feE2KBBRMW3FzOpLOADl0Isb5587h/U4gGvkt5v60Z1VLG8BhYjbzRwyQZemwAd6cCR5/XFWLYZRIMpX39AR0tjaGGiGzLVyhse5C9RKC6ai42ppWPKiBagOvaYk8lO7DajerabOZP46Lby5wKjw1HCRx7p9sVMOWGzb/vA1hwiWc6jm3MvQDTogQkiqIhJV0nBQBTU+3okKCFDy9WwferkHjtxib7t3xIUQtHxnIwtx4mpg26/HfwVNVDb4oI9RHmx5WGelRVlrtiw43zboCLaxv46AZeB3IlTkwouebTr1y2NjSpHz68WNFjHvupy3q8TFn3Hos2IAk4Ju5dCo8B3wP7VPr/FGaKiG+T+v+TQqIrOqMTL1VdWV1DdmcbO8KXBz6esmYWYKPwDL5b5FA1a0hwapHiom0r/cKaoqr+27/XcrS5UwSMbQAAAABJRU5ErkJggg==)](https://deepwiki.com/rock-db/cate)

**cate** is a tool to generate a Makefile from a TOML configuration file (`project.toml`).

It aims to simplify project setup and build management for C projects by using declarative configuration.

## Features

- Generate a Makefile from a TOML file describing your project.
- Initialize a new project structure with `src/`, `include/`, and a sample `main.c`.
- Supports custom output filenames and TOML input files via command-line arguments.
- MIT licensed.

## Usage

```
cate [options]
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
   cate --init
   ```
   This creates a sample `project.toml`, `src/main.c`, and `include/`.

2. Edit `project.toml` to describe your project.

3. Generate the Makefile:
   ```
   cate
   ```
   or specify custom files:
   ```
   cate -t myproject.toml -o CustomMakefile
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
git clone --recurse-submodules https://github.com/rock-db/cate
cd cate
make
```

## License

[MIT License](./LICENSE)
