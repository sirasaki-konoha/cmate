use serde::{Deserialize, Serialize};

#[derive(Debug, PartialEq, Eq, PartialOrd, Ord, Deserialize, Serialize)]
pub struct CmateToml {
    pub default_compiler: String,
    pub bin: Option<Vec<Bin>>,
}

#[derive(Debug, PartialEq, Eq, PartialOrd, Ord, Deserialize, Serialize)]
pub struct Bin {
    pub project: String,
    pub compiler: Option<String>,
    pub flags: Option<Vec<String>>,
    pub ldflags: Option<Vec<String>>,
    pub src: Option<Vec<String>>,
    pub include: Option<Vec<String>>,
    pub compile: Option<Vec<String>>,
    pub libs: Option<Vec<String>>,
}
