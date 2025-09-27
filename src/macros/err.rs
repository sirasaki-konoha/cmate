#[macro_export]
macro_rules! err{
    ($($msg: expr), *) => {{
        use crossterm::style::Stylize;
        let fmt = format!($($msg), *);
        println!("{} {}", "!!".green(), fmt);
    }};
}
