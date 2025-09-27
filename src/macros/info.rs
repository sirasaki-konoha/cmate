#[macro_export]
macro_rules! info {
    ($($msg: expr), *) => {{
        use crossterm::style::Stylize;
        let fmt = format!($($msg), *);
        println!("{} {}", ">>".green(), fmt);
    }};
}
