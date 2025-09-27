use crate::err;
use std::process::{self, Command, Stdio};

pub fn get_gmake() -> String {
    if which::which("gmake").is_ok() {
        return "gmake".to_string();
    }

    let output = Command::new("make")
        .arg("--version")
        .output()
        .unwrap_or_else(|e| {
            err!("Failed to find make: {}", e);
            process::exit(1)
        });

    let stdout = String::from_utf8_lossy(&output.stdout);
    let stdout = stdout.to_string();

    let splited: Vec<&str> = stdout.split_whitespace().collect();

    if splited[1..2].join(" ") == "GNU Make" {
        return "make".to_string();
    } else {
        err!("No available GNU Make found. Please install latest GNU Make");
        process::exit(1);
    }
}
