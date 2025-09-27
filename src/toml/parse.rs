use crate::toml::toml_structure::{Bin, CmateToml};
use crate::{err, info};
use std::error::Error;
use toml;

pub fn parse_toml<T: AsRef<str>>(toml: T) -> Result<CmateToml, Box<dyn Error>> {
    let parsed: CmateToml = toml::from_str(toml.as_ref()).unwrap_or_else(|e| {
        err!("Failed to parse toml: \n{}", e);
        std::process::exit(1);
    });
    Ok(parsed)
}
