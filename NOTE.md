
# Build Environment Notes

This project supports builds for both **Windows** and **Linux**.
Below are notes to ensure consistent, portable release builds.

---

## Windows Build

### Environment
- Toolchain: [w64devkit](https://github.com/skeeto/w64devkit)
- GCC version: 15.1.0

### Notes
- Links statically to `msvcrt.dll`
  - No need to install additional runtime libraries
- Check DLL dependencies using `dumpbin /DEPENDENTS` or similar
  - Expected output:
    - `KERNEL32.dll`
    - `msvcrt.dll`
- **Always build from the w64devkit terminal**
  - Do **not** use MSYS2, Git Bash, or PowerShell
  - Other environments may fail to build or change linkage behavior

---

## Linux Build

### Environment
- Build runs in a container based on **Alpine Linux**
- Docker is used to ensure consistency and minimalism

### Build Notes
- **Use static linking for release builds**
  - Add `-static` to the compiler flags
- Verify that the output is fully statically linked:

```bash
file ./cate
# Output should include: "statically linked"
````

> **Do not forget `-static` for release builds!**
>
> This ensures the binary can run on any target system without requiring external shared libraries.

---

## Summary

| Target  | Environment     | Notes                                                    |
| ------- | --------------- | -------------------------------------------------------- |
| Windows | w64devkit       | Use bundled terminal, check for minimal DLL dependencies |
| Linux   | Docker + Alpine | Use `-static`, verify with `file`                        |
