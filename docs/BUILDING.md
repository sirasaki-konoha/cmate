# 🚧 Building cmate

This document explains how to build **cmate-rs** from source.

---

## 🔁 1. Clone the repository

```sh
git clone https://github.com/sirasaki-konoha/cmate
cd cmate
````

---

## 🧱 2. Build the project

You can build cmate in two ways:

* 🧪 **Debug build** :

  ```sh
  cargo build
  ```

* 🚀 **Release build** (optimized):

  ```sh
  cargo build --release
  ```

---

## ✅ Requirements

Make sure the following tools are installed:

* Rust tools (cargo, rustc, ...etc)

---

## 📁 Output

Built binaries will appear in the `target/<release or debug>` directory.

---

## 🐛 Troubleshooting

If you encounter issues during build:

* Make sure your rust environment is available (`which cargo`, `rustc --version`, etc.)
* Run `cargo build` and try again

