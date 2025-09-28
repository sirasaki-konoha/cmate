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
use std::env;
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
        Some(ref cmd) => handle_subcommand(&config, cmd),
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

fn handle_subcommand(config: &Config, cmd: &SubCommands) {
    match cmd {
        SubCommands::Init => init::init_projects::init(),
        SubCommands::Build => {
            let (toml_dir, _) = resolve_toml_path_and_dir(&config.toml_file);

            // TOMLファイルがあるディレクトリに移動
            env::set_current_dir(&toml_dir).unwrap_or_else(|e| {
                err!("Failed to set directory: {}", e);
                std::process::exit(1);
            });

            // ビルド実行
            build::build_project::build_project(makefile::get_gmake::get_gmake());
        }
        SubCommands::Clean => todo!(),
        SubCommands::Run => todo!(),
    }
}

fn generate_makefile(config: &Config) {
    // 元の作業ディレクトリを保存
    let original_dir = env::current_dir().unwrap_or_else(|e| {
        err!("Failed to get current directory: {}", e);
        std::process::exit(1);
    });

    let (toml_dir, toml_path) = resolve_toml_path_and_dir(&config.toml_file);

    // TOMLファイルを読み込み（絶対パスを使用）
    let cmate_toml = read_toml_file(&toml_path);
    let parsed = parse_toml_content(cmate_toml);
    let makefile_content = makefile::gen_makefile::gen_makefile(parsed);

    // 出力ファイルのパスを解決
    let output_path = if Path::new(&config.makefile_output).is_absolute() {
        config.makefile_output.clone()
    } else {
        // 相対パスの場合、TOMLファイルがあるディレクトリからの相対パス
        toml_dir
            .join(&config.makefile_output)
            .to_string_lossy()
            .to_string()
    };

    // Makefileを書き込み（元のディレクトリから）
    write_makefile(&output_path, &makefile_content);

    // 出力パスの表示用（相対パス表示を保持）
    let display_path = if Path::new(&config.makefile_output).is_absolute() {
        config.makefile_output.clone()
    } else {
        // TOMLディレクトリからの相対パスとして表示
        let relative_toml_dir =
            pathdiff::diff_paths(&toml_dir, &original_dir).unwrap_or(toml_dir.clone());
        relative_toml_dir
            .join(&config.makefile_output)
            .to_string_lossy()
            .to_string()
    };

    info!("Makefile successfully generated at {}", display_path);
}

/// Resolve TOML path and return both directory and file path
fn resolve_toml_path_and_dir(toml_file: &str) -> (PathBuf, String) {
    if toml_file == DEFAULT_TOML_FILE {
        if let Some(toml_path) = find_toml_directory(PathBuf::from(".")) {
            let toml_dir = toml_path.parent().unwrap_or(Path::new(".")).to_path_buf();
            return (toml_dir, toml_path.to_string_lossy().to_string());
        } else {
            err!("Cmate.toml not found!");
            std::process::exit(1);
        }
    }

    let path = Path::new(toml_file);

    // 絶対パスか相対パスかに関係なく、正しく処理
    let canonical_path = if path.is_absolute() {
        path.to_path_buf()
    } else {
        env::current_dir()
            .unwrap_or_else(|e| {
                err!("Failed to get current directory: {}", e);
                std::process::exit(1);
            })
            .join(path)
    };

    let dir = canonical_path
        .parent()
        .unwrap_or(Path::new("."))
        .to_path_buf();
    (dir, canonical_path.to_string_lossy().to_string())
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

    // 出力ディレクトリが存在しない場合は作成
    if let Some(parent) = makefile_path.parent() {
        fs::create_dir_all(parent).unwrap_or_else(|e| {
            err!("Failed to create directory {}: {}", parent.display(), e);
            std::process::exit(1);
        });
    }

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

#[allow(unused)]
/// Find the directory containing Cmate.toml by searching upward from the given base path
fn find_toml_directory(mut base: PathBuf) -> Option<PathBuf> {
    // 現在のディレクトリから始める
    let current_dir = env::current_dir().unwrap_or_else(|e| {
        err!("Failed to get current directory: {}", e);
        std::process::exit(1);
    });

    base = current_dir;

    loop {
        let candidate = base.join(DEFAULT_TOML_FILE);

        // デバッグ情報（必要に応じてコメントアウト）
        // eprintln!("Checking: {}", candidate.display());

        if candidate.exists() {
            return Some(candidate);
        }

        // 親ディレクトリに移動
        if !base.pop() {
            return None;
        }
    }
}
