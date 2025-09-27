use std::{
    error::Error,
    fs::{self, File},
    io,
};

use crate::{
    compiler, err, includes::makefile::MAKEFILE_TEMPLATE, info, toml::toml_structure::CmateToml,
};

pub fn gen_makefile(toml: CmateToml) -> Result<String, Box<dyn Error>> {
    Ok(String::new())
}

fn gen_bin(toml: CmateToml) -> String {
    // get default compiler
    let cc_base = if compiler::check::check_compiler(toml.default_compiler.clone()) {
        toml.default_compiler.clone()
    } else {
        eprintln!("default compiler not found!");
        eprintln!("Please install gcc or clang, or zig cc and retry cmate");
        std::process::exit(1);
    };

    let Some(toml) = toml.bin else {
        err!(
            "Exit called from {}. This is a bug. UnExpected exit.",
            file!()
        );
        err!("Option {{ None }} is not expected. Please report this bug to issue");
        std::process::exit(1);
    };

    let mut current_makefile = String::new();
    let mut project_names: Vec<String> = Vec::new();
    let mut cc_flags: Vec<String> = Vec::new();
    let mut ld_flags: Vec<String> = Vec::new();
    let mut src: Vec<String> = Vec::new();
    let mut include: Vec<String> = Vec::new();
    let mut compile: Vec<String> = Vec::new();
    let mut libs: Vec<String> = Vec::new();
    let mut cc = String::new();

    for project in toml {
        project_names.push(project.project);

        // compiler
        if let Some(cc_compiler) = project.compiler {
            if cc_compiler == "auto" {
                let compiler = compiler::auto_detect::auto_detect_compiler();

                // add to compiler
                match compiler {
                    Some(compiler) => cc = compiler.to_string(),
                    None => {
                        err!("C Compiler is not found!");
                        err!("Supporting 'auto' compilers: gcc, clang, cc, zig cc");
                        std::process::exit(1);
                    }
                }
            } else {
                // check compiler exists
                if !compiler::check::check_compiler(cc_compiler.clone()) {
                    err!("Compiler '{}' is not set up!", &cc_compiler);
                    std::process::exit(1);
                }
            }
        } else {
            cc = cc_base.clone();
        }

        // flags
        if let Some(ccflags) = project.flags {
            cc_flags = ccflags;
        }

        // ldflags
        if let Some(ldflags) = project.ldflags {
            ld_flags = ldflags;
        }

        // src
        if let Some(src_dirs) = project.src {
            src = src_dirs;
        }

        // include
        if let Some(include_dirs) = project.include {
            include = include_dirs;
        }

        // compile
        if let Some(compile_files) = project.compile {
            compile = compile_files;
        }

        // libs
        if let Some(libraries) = project.libs {
            libs = libraries;
        }
    }

    let project_names = format!("PROJECT_NAMES := {}", project_names.join(" "));

    let cc_fields;

    String::new()
}

/// base: will replace '%s'.
/// %s is replaced to projects.
/// example: gen_fields("TEST_%s := ", &["test", "example"], &[gcc, coled]);
fn gen_fields_double(base: String, projects: &[(String, &[String])]) -> String {
    let mut current = String::new();

    for (proj, double) in projects {
        current.push_str(&format!(""));
    }

    String::new();
}
