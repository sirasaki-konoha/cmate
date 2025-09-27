use crate::toml::toml_structure::CmateToml;
use crate::{err, info};
use std::process::{Command, Stdio};

pub fn build_project(spec: &str, args: &[String], make: String, toml: CmateToml) {
    let p = std::thread::available_parallelism().unwrap_or_else(|e| {
        err!("Failed to get cpu threads: {}", e);
        std::process::exit(1);
    });
    let mut parallel = p.get();

    if parallel != 1 {
        parallel = parallel / 2;
    }

    info!("Building project (using {} threads)", parallel);
    let status = Command::new(&make)
        .arg(format!("-j{}", parallel))
        .status()
        .unwrap_or_else(|e| {
            err!("make failed: {}", e);
            std::process::exit(1);
        });

    if !status.success() {
        _ = Command::new(&make)
            .arg("clean")
            .stdout(Stdio::null())
            .stdin(Stdio::null())
            .stderr(Stdio::null())
            .status();
    }
}
