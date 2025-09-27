use crate::{compiler, err, info, toml::toml_structure::CmateToml};

pub fn gen_bin(toml: CmateToml, default_cc: String, projects: &mut Vec<String>) -> String {
    // get default compiler
    let cc_base = default_cc;
    // if bin is None. it is Unexpected.
    let Some(toml) = toml.bin else {
        err!(
            "Exit called from {}. This is a bug. UnExpected exit.",
            file!()
        );
        err!("Option {{ None }} is not expected. Please report this bug to issue");
        std::process::exit(1);
    };

    // makefile content in memory
    let mut current_makefile = String::new();

    for project in toml {
        // CFLAGS
        let mut cc_flags: Vec<String> = Vec::new();
        // LDFLAGS
        let mut ld_flags: Vec<String> = Vec::new();
        // SRC
        let mut src: Vec<String> = vec!["src/".to_string()];
        // INCLUDE
        let mut include: Vec<String> = vec!["include/".to_string()];
        // EXTRA_SOURCES
        let mut compile: Vec<String> = Vec::new();
        // LDLIBS
        let mut libs: Vec<String> = Vec::new();
        // CC
        let mut cc = String::new();

        // add to project names
        projects.push(project.project.clone());

        // compiler
        if let Some(cc_compiler) = project.compiler {
            // if compiler value is 'auto'. auto detect c compiler
            if cc_compiler == "auto" {
                let compiler = compiler::auto_detect::auto_detect_compiler();

                // add to compiler
                match compiler {
                    Some(compiler) => {
                        cc = compiler.to_string();
                        info!(
                            "{}: auto detect compiler: {}",
                            project.project.clone(),
                            compiler
                        );
                    }
                    None => {
                        // compiler not found
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
            // if compiler is None. using project's default compiler
            info!(
                "{}: using the project's default compiler: {}",
                project.project.clone(),
                cc_base
            );

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
            src.clear();
            src = src_dirs;
        }

        // include
        if let Some(include_dirs) = project.include {
            include.clear();
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

        // generate makefile for bin

        // CC_%s
        current_makefile.push_str(&gen_fields_solo("CC_%s := ", (project.project.clone(), cc)));

        // CFLAGS_%s
        current_makefile.push_str(&gen_fields_double(
            "CFLAGS_%s := ",
            &[(&project.project, &cc_flags)],
        ));

        // LDLIBS_%s
        current_makefile.push_str(&gen_fields_double(
            "LDLIBS_%s := ",
            &[(&project.project, &libs)],
        ));

        // LDFLAGS_%s
        current_makefile.push_str(&gen_fields_double(
            "LDFLAGS_%s := ",
            &[(&project.project, &ld_flags)],
        ));

        // SRCDIRS_%s
        current_makefile.push_str(&gen_fields_double(
            "SRCDIRS_%s := ",
            &[(&project.project, &src)],
        ));

        // INCLUDE_DIRS_%s
        current_makefile.push_str(&gen_fields_double(
            "INCLUDE_DIRS_%s := ",
            &[(&project.project, &include)],
        ));

        // EXTRA_SOURCES_%s
        current_makefile.push_str(&gen_fields_double(
            "EXTRA_SOURCES_%s := ",
            &[(&project.project, &compile)],
        ));
    }

    current_makefile
}

/// base: will replace '%s'.
/// %s is replaced to projects.
/// example: gen_fields("LIBS_%s := ", ("myproj", "-lm"]);
fn gen_fields_solo(base: &str, projects: (String, String)) -> String {
    format!("{}{}\n", base.replace("%s", &projects.0), projects.1)
}

/// base: will replace '%s'.
/// %s is replaced to projects.
/// example: gen_fields("LIBS_%s := ", &[("myproject", $["-lm", "-lc", "-lpthread"])]);
fn gen_fields_double(base: &str, projects: &[(&str, &[String])]) -> String {
    let mut current = String::new();

    for (proj, double) in projects {
        current = format!("{}{}", base.replace("%s", proj), double.join(" "));
    }

    format!("{}\n", current)
}
