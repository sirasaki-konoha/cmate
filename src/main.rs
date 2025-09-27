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
use std::path::Path;

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

    let mut makefile_output: String = String::from("Makefile");
    let mut toml_file: String = String::from("Cmate.toml");

    if cli.version {
        version();
        return;
    }

    if let Some(spec) = cli.output {
        makefile_output = spec;
    }
    if let Some(spec_toml) = cli.toml {
        toml_file = spec_toml;
    }

    let makefile_output: &Path = makefile_output.as_ref();

    // if cmd is None. create makefile
    let Some(cmd) = cli.subcommands else {
        // read Cmate.toml
        let cmate_toml = fs::read_to_string(&toml_file).unwrap_or_else(|e| {
            err!("{}: {}", toml_file, e);
            std::process::exit(1);
        });

        // parse Cmate.toml
        let parsed = toml::parse::parse_toml(cmate_toml).unwrap_or_else(|_| {
            std::process::exit(1);
        });

        // generate makefile content
        let makefile = makefile::gen_makefile::gen_makefile(parsed);

        // open the file
        let mut file = fs::OpenOptions::new()
            .write(true)
            .create(true)
            .truncate(true)
            .open(&makefile_output)
            .unwrap_or_else(|e| {
                err!("Failed to open {}: {}", makefile_output.display(), e);
                std::process::exit(1);
            });

        // write to Makefile file
        file.write_all(makefile.as_bytes()).unwrap_or_else(|e| {
            err!(
                "Failed to write makefile content to {}: {}",
                &makefile_output.display(),
                e
            );
            std::process::exit(1);
        });

        info!(
            "Makefile successfully generated at {}",
            makefile_output.display()
        );

        std::process::exit(0);
    };

    match cmd {
        SubCommands::Init => init::init::init(),
        SubCommands::Build => todo!(),
        SubCommands::Clean => todo!(),
        SubCommands::Run => todo!(),
    }
}

fn version() {
    info!("cmate version {}", info::CMATE_VERSION);
    info!("{}", info::CMATE_COPYLIGHT);
    info!("{}", info::CMATE_LICENSE);
    return;
}
