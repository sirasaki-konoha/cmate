mod build;
mod clean;
mod compiler;
mod includes;
mod info;
mod init;
mod macros;
mod makefile;
mod run;
mod toml;

use clap::{Parser, Subcommand};
use std::fs;
use std::io::Write;
use std::path::{Path, PathBuf};

const DEFAULT_MAKEFILE: &str = "Makefile";
const DEFAULT_TOML_FILE: &str = "Cmate.toml";

#[derive(Parser)]
struct Cli {
    /// Specify the output file (default: Makefile)
    #[arg(short, long)]
    pub output: Option<String>,

    /// Specify the TOML file to parse (default: Cmate.toml)
    #[arg(short, long)]
    pub toml: Option<String>,

    /// Display version information
    #[arg(short, long)]
    pub version: bool,

    #[command(subcommand)]
    subcommands: Option<SubCommands>,
}

#[derive(Subcommand)]
enum SubCommands {
    /// Generate a project template
    Init,
    /// Build the project
    Build,
    /// Clean the project
    Clean,
    /// Run the specified project (use -- to pass arguments to the program)
    Run,
}

fn main() {
    let cli = Cli::parse();

    if cli.version {
        show_version();
        return;
    }

    let config = Config::from_cli(cli);

    match config.subcommand {
        Some(cmd) => handle_subcommand(cmd),
        None => generate_makefile(&config),
    }
}

/// Configuration extracted from CLI arguments
struct Config {
    makefile_output: String,
    toml_file: String,
    subcommand: Option<SubCommands>,
}

impl Config {
    fn from_cli(cli: Cli) -> Self {
        let makefile_output = cli.output.unwrap_or_else(|| DEFAULT_MAKEFILE.to_string());
        let toml_file = cli.toml.unwrap_or_else(|| DEFAULT_TOML_FILE.to_string());

        Self {
            makefile_output,
            toml_file,
            subcommand: cli.subcommands,
        }
    }
}

fn show_version() {
    info!("cmate version {}", info::CMATE_VERSION);
    info!("{}", info::CMATE_COPYLIGHT);
    info!("{}", info::CMATE_LICENSE);
}

fn handle_subcommand(cmd: SubCommands) {
    match cmd {
        SubCommands::Init => init::init::init(),
        SubCommands::Build => todo!(),
        SubCommands::Clean => todo!(),
        SubCommands::Run => todo!(),
    }
}

fn generate_makefile(config: &Config) {
    let toml_path = resolve_toml_path(&config.toml_file);
    let cmate_toml = read_toml_file(&toml_path);
    let parsed = parse_toml_content(cmate_toml);
    let makefile_content = makefile::gen_makefile::gen_makefile(parsed);

    write_makefile(&config.makefile_output, &makefile_content);

    info!(
        "Makefile successfully generated at {}",
        config.makefile_output
    );
}

fn resolve_toml_path(toml_file: &str) -> String {
    if toml_file == DEFAULT_TOML_FILE {
        if let Some(toml_dir) = find_toml_directory(PathBuf::from(toml_file)) {
            return toml_dir.to_string_lossy().to_string();
        } else {
            err!("Cmate.toml not found!");
            std::process::exit(1);
        }
    }
    toml_file.to_string()
}

fn read_toml_file(path: &str) -> String {
    fs::read_to_string(path).unwrap_or_else(|e| {
        err!("{}: {}", path, e);
        std::process::exit(1);
    })
}

fn parse_toml_content(content: String) -> toml::toml_structure::CmateToml {
    toml::parse::parse_toml(content).unwrap_or_else(|_| {
        std::process::exit(1);
    })
}

fn write_makefile(output_path: &str, content: &str) {
    let makefile_path = Path::new(output_path);

    let mut file = fs::OpenOptions::new()
        .write(true)
        .create(true)
        .truncate(true)
        .open(makefile_path)
        .unwrap_or_else(|e| {
            err!("Failed to open {}: {}", makefile_path.display(), e);
            std::process::exit(1);
        });

    file.write_all(content.as_bytes()).unwrap_or_else(|e| {
        err!(
            "Failed to write makefile content to {}: {}",
            makefile_path.display(),
            e
        );
        std::process::exit(1);
    });
}

/// Find the directory containing Cmate.toml by searching upward from the given base path
fn find_toml_directory(mut base: PathBuf) -> Option<PathBuf> {
    loop {
        let candidate = base.join(DEFAULT_TOML_FILE);
        if candidate.exists() {
            return Some(candidate);
        }

        if !base.pop() {
            return None;
        }
    }
}
