use crate::{err, includes::init, info};
use std::fs::{self, File};
use std::io::Write;
use std::path::Path;

pub fn init() {
    // initialize Cmate.toml
    init_cmate_toml();

    // initialize .gitignore
    init_gitignore();

    // initialize main.c
    init_main();
}

fn write_to_file(filename: &str, content: &str) {
    let path: &Path = filename.as_ref();

    if path.exists() {
        return;
    }

    let mut file = File::create(path).unwrap_or_else(|e| {
        err!("Failed to initialize {}: {}", filename, e);
        std::process::exit(1);
    });

    file.write_all(content.as_bytes()).unwrap_or_else(|e| {
        err!("Failed to initialize {} in writing: {}", filename, e);
        std::process::exit(1);
    });

    info!("Successfully created: {}", filename);
}

fn init_cmate_toml() {
    write_to_file("Cmate.toml", init::CMATE_TOML);
}

fn init_gitignore() {
    write_to_file(".gitignore", init::GITIGNORE);
}

fn init_main() {
    let path: &Path = "src".as_ref();

    if path.is_dir() {
        return;
    }
    fs::create_dir_all(path).unwrap_or_else(|e| {
        err!("Failed to initialize directory {}: {}", path.display(), e);
        std::process::exit(1);
    });

    let bind = path.join("main.c");
    let path = bind.to_str().unwrap();

    write_to_file(path, init::MAIN_C);
}
