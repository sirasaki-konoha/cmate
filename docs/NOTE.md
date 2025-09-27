
# Build Environment Notes

This project supports builds for both **Windows** and **Linux**.
Below are notes to ensure consistent, portable release builds.

---

## Windows Build

### Environment
- Toolchain: [MSVC](https://visualstudio.microsoft.com/ja/vs/features/cplusplus/)
- rustc version: 1.85.0 or later

### Notes
- Links statically to `msvcrt.dll`
  - No need to install additional runtime libraries
- Check DLL dependencies using `dumpbin /DEPENDENTS` or similar
  - Expected output:
    - `KERNEL32.dll`
    - `msvcrt.dll`

---

## Linux Build

### Environment
- Build runs in a container based on **Alpine Linux**
- Docker is used to ensure consistency and minimalism

### Build Notes
- **Use static linking for release builds**
- Verify that the output is fully statically linked:

```bash
file ./cmate
# Output should include: "statically linked"
```

---

## Summary

| Target  | Environment     | Notes                                                    |
| ------- | --------------- | -------------------------------------------------------- |
| Windows | MSVC       | check for minimal DLL dependencies |
| Linux   | Docker + Alpine | Use static linking for release builds, verify with `file`                        |
