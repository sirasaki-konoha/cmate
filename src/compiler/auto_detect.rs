pub fn auto_detect_compiler() -> Option<String> {
    let mut compiler = None;

    let compilers = &["gcc", "clang", "cc", "zig cc"];
    for exec in compilers {
        // check compiler existing
        let status = which::which(exec);

        // check status
        if let Ok(cc) = status {
            let cc = cc.to_str().unwrap();
            compiler = Some(cc.to_string());
        }
    }

    compiler
}
