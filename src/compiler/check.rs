pub fn check_compiler(compiler: String) -> bool {
    which::which(compiler).is_ok()
}
