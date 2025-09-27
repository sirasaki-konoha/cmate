#[macro_export]
macro_rules! warn {
    ($($msg: expr), *) => {{
        use crossterm::style::Stylize;
        let fmt = format!($($msg), *);
        eprintln!("{} {}", "!!".yellow(), fmt);
    }};
}
