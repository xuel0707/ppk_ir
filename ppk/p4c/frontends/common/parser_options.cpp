/*
Copyright 2013-present Barefoot Networks, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <getopt.h>
#include <regex>
#include <unordered_set>

#include "frontends/p4/toP4/toP4.h"
#include "ir/json_generator.h"
#include "lib/exceptions.h"
#include "lib/exename.h"
#include "lib/log.h"
#include "lib/nullstream.h"
#include "lib/path.h"
#include "parser_options.h"

const char* p4includePath = CONFIG_PKGDATADIR "/p4include";
const char* p4_14includePath = CONFIG_PKGDATADIR "/p4_14include";

const char* ParserOptions::defaultMessage = "Compile a P4 program";

ParserOptions::ParserOptions() : Util::Options(defaultMessage) {
    registerOption(
        "--help", nullptr,
        [this](const char* ) {
            usage();
            exit(0);
            return false;
        },
        "Print this help message");
    registerOption(
        "--version", nullptr,
        [this](const char* ) {
            std::cerr << binaryName << std::endl;
            std::cerr << "Version " << compilerVersion << std::endl;
            exit(0);
            return false;
        },
        "Print compiler version");
    registerOption(
        "-I", "path",
        [this](const char* arg) {
            preprocessor_options += std::string(" -I") + arg;
            return true;
        },
        "Specify include path (passed to preprocessor)");
    registerOption(
        "-D", "arg=value",
        [this](const char* arg) {
            preprocessor_options += std::string(" -D") + arg;
            return true;
        },
        "Define macro (passed to preprocessor)");
    registerOption(
        "-U", "arg",
        [this](const char* arg) {
            preprocessor_options += std::string(" -U") + arg;
            return true;
        },
        "Undefine macro (passed to preprocessor)");
    registerOption(
        "-E", nullptr,
        [this](const char* ) {
            doNotCompile = true;
            return true;
        },
        "Preprocess only, do not compile (prints program on stdout)");
    registerOption(
        "--p4v", "{14|16}",
        [this](const char* arg) {
            if (!strcmp(arg, "1.0") || !strcmp(arg, "14")) {
                langVersion = ParserOptions::FrontendVersion::P4_14;
            } else if (!strcmp(arg, "1.2") || !strcmp(arg, "16")) {
                langVersion = ParserOptions::FrontendVersion::P4_16;
            } else {
                ::error(ErrorType::ERR_INVALID, "Illegal language version %1%",
                        arg);
                return false;
            }
            return true;
        },
        "[Deprecated; use --std instead] Specify language version to compile.",
        OptionFlags::Hide);
    registerOption(
        "--std", "{p4-14|p4-16}",
        [this](const char* arg) {
            if (!strcmp(arg, "14") || !strcmp(arg, "p4-14")) {
                langVersion = ParserOptions::FrontendVersion::P4_14;
            } else if (!strcmp(arg, "16") || !strcmp(arg, "p4-16")) {
                langVersion = ParserOptions::FrontendVersion::P4_16;
            } else {
                ::error(ErrorType::ERR_INVALID, "Illegal language version %1%",
                        arg);
                return false;
            }
            return true;
        },
        "Specify language version to compile.");
    registerOption(
        "--nocpp", nullptr,
        [this](const char* ) {
            doNotPreprocess = true;
            return true;
        },
        "Skip preprocess, assume input file is already preprocessed.");
    registerOption(
        "--disable-annotations", "annotations",
        [this](const char* arg) {
            auto copy = strdup(arg);
            while (auto name = strsep(&copy, ","))
                disabledAnnotations.insert(name);
            return true;
        },
        "Specify a (comma separated) list of annotations that should be "
        "ignored by\n"
        "the compiler. A warning will be printed that the annotation is "
        "ignored",
        OptionFlags::OptionalArgument);
    registerOption(
        "--Wdisable", "diagnostic",
        [](const char* diagnostic) {
            if (diagnostic) {
                P4CContext::get().setDiagnosticAction(diagnostic,
                                                      DiagnosticAction::Ignore);
            } else {
                auto action = DiagnosticAction::Ignore;
                P4CContext::get().setDefaultWarningDiagnosticAction(action);
            }
            return true;
        },
        "Disable a compiler diagnostic, or disable all warnings if no "
        "diagnostic is specified.",
        OptionFlags::OptionalArgument);
    registerOption(
        "--Wwarn", "diagnostic",
        [](const char* diagnostic) {
            if (diagnostic) {
                P4CContext::get().setDiagnosticAction(diagnostic,
                                                      DiagnosticAction::Warn);
            } else {
                auto action = DiagnosticAction::Warn;
                P4CContext::get().setDefaultWarningDiagnosticAction(action);
            }
            return true;
        },
        "Report a warning for a compiler diagnostic, or treat all warnings "
        "as warnings (the default) if no diagnostic is specified.",
        OptionFlags::OptionalArgument);
    registerOption(
        "--Werror", "diagnostic",
        [](const char* diagnostic) {
            if (diagnostic) {
                P4CContext::get().setDiagnosticAction(diagnostic,
                                                      DiagnosticAction::Error);
            } else {
                auto action = DiagnosticAction::Error;
                P4CContext::get().setDefaultWarningDiagnosticAction(action);
            }
            return true;
        },
        "Report an error for a compiler diagnostic, or treat all warnings as "
        "errors if no diagnostic is specified.",
        OptionFlags::OptionalArgument);
    registerOption(
        "--maxErrorCount", "errorCount",
        [](const char* arg) {
            auto maxError = strtoul(arg, nullptr, 10);
            P4CContext::get().errorReporter().setMaxErrorCount(maxError);
            return true;
        },
        "Set the maximum number of errors to display before failing.");
    registerOption(
        "-T", "loglevel",
        [](const char* arg) {
            Log::addDebugSpec(arg);
            return true;
        },
        "[Compiler debugging] Adjust logging level per file (see below)");
    registerOption(
        "-v", nullptr,
        [](const char* ) {
            Log::increaseVerbosity();
            return true;
        },
        "[Compiler debugging] Increase verbosity level (can be repeated)");
    registerOption(
        "--top4", "pass1[,pass2]",
        [this](const char* arg) {
            auto copy = strdup(arg);
            while (auto pass = strsep(&copy, ","))
                top4.push_back(pass);
            return true;
        },
        "[Compiler debugging] Dump the P4 representation after\n"
        "passes whose name contains one of `passX' substrings.\n"
        "When '-v' is used this will include the compiler IR.\n");
    registerOption(
        "--dump", "folder",
        [this](const char* arg) {
            dumpFolder = arg;
            return true;
        },
        "[Compiler debugging] Folder where P4 programs are dumped\n");
    registerUsage(
        "loglevel format is: \"sourceFile:level,...,sourceFile:level\"\n"
        "where 'sourceFile' is a compiler source file and "
        "'level' is the verbosity level for LOG messages in that file");
}

void ParserOptions::setInputFile() {
    if (remainingOptions.size() > 1) {
        ::error(ErrorType::ERR_OVERLIMIT,
                "Only one input file must be specified: %s",
                cstring::join(remainingOptions.begin(), remainingOptions.end(),
                              ","));
        usage();
        exit(1);
    } else if (remainingOptions.size() == 0) {
        ::error(ErrorType::ERR_EXPECTED, "No input files specified");
        usage();
        exit(1);
    } else {
        file = remainingOptions.at(0);
    }
}

namespace {

bool setIncludePathIfExists(const char*& includePathOut,
                            const char* possiblePath) {
    struct stat st;
    if (!(stat(possiblePath,& st) >= 0 && S_ISDIR(st.st_mode)))
        return false;
    if (auto path = realpath(possiblePath, NULL))
        includePathOut = path;
    else
        includePathOut = strdup(possiblePath);
    return true;
}

}  // namespace

std::vector<const char*>* ParserOptions::process(int argc,
                                                  char* const argv[]) {
    char buffer[PATH_MAX];

    snprintf(buffer, sizeof(buffer), "%s", exename(argv[0]));
    if (char* p = strrchr(buffer, '/')) {
        ++p;
        exe_name = p;
        snprintf(p, buffer + sizeof(buffer) - p, "p4include");
        if (!setIncludePathIfExists(p4includePath, buffer)) {
            snprintf(p, buffer + sizeof(buffer) - p, "../p4include");
            setIncludePathIfExists(p4includePath, buffer);
        }
        snprintf(p, buffer + sizeof(buffer) - p, "p4_14include");
        if (!setIncludePathIfExists(p4_14includePath, buffer)) {
            snprintf(p, buffer + sizeof(buffer) - p, "../p4_14include");
            setIncludePathIfExists(p4_14includePath, buffer);
        }
    }

    auto remainingOptions = Util::Options::process(argc, argv);
    validateOptions();
    return remainingOptions;
}

void ParserOptions::validateOptions() const {}

FILE* ParserOptions::preprocess() {
    FILE* in = nullptr;

    if (file == "-") {
        file = "<stdin>";
        in = stdin;
    } else {
#ifdef __clang__
        std::string cmd("cc -E -x c -Wno-comment");
#else
        std::string cmd("cpp");
#endif
        // the p4c driver sets environment variables for include
        // paths.  check the environment and add these to the command
        // line for the preprocessor
        char* driverP4IncludePath = isv1() ? getenv("P4C_14_INCLUDE_PATH")
                                           : getenv("P4C_16_INCLUDE_PATH");
        cmd +=
            cstring(" -C -undef -nostdinc -x assembler-with-cpp") + " " +
            preprocessor_options +
            (driverP4IncludePath ? " -I" + cstring(driverP4IncludePath) : "") +
            " -I" + (isv1() ? p4_14includePath : p4includePath) + " " +
            (file != nullptr ? file : "");

        if (Log::verbose())
            std::cerr << "Invoking preprocessor " << std::endl
                      << cmd << std::endl;
        in = popen(cmd.c_str(), "r");
        if (in == nullptr) {
            ::error(ErrorType::ERR_IO, "Error invoking preprocessor");
            perror("");
            return nullptr;
        }
        close_input = true;
    }

    if (doNotCompile) {
        char* line = NULL;
        size_t len = 0;
        ssize_t read;

        while ((read = getline(&line,& len, in)) != -1)
            printf("%s", line);
        closeInput(in);
        return nullptr;
    }
    return in;
}

void ParserOptions::closeInput(FILE* inputStream) const {
    if (close_input) {
        int exitCode = pclose(inputStream);
        if (WIFEXITED(exitCode) && WEXITSTATUS(exitCode) == 4)
            ::error(ErrorType::ERR_IO, "input file %s does not exist", file);
        else if (exitCode != 0)
            ::error(ErrorType::ERR_IO,
                    "Preprocessor returned exit code %d; aborting compilation",
                    exitCode);
    }
}

// From (folder, file.ext, suffix)  returns
// folder/file-suffix.ext
static cstring makeFileName(cstring folder, cstring name, cstring baseSuffix) {
    Util::PathName filename(name);
    Util::PathName newName(filename.getBasename() + baseSuffix + "." +
                           filename.getExtension());
    auto result = Util::PathName(folder).join(newName.toString());
    return result.toString();
}

bool ParserOptions::isv1() const {
    return langVersion == ParserOptions::FrontendVersion::P4_14;
}

void ParserOptions::dumpPass(const char* manager, unsigned seq,
                             const char* pass, const IR::Node* node) const {
    if (strncmp(pass, "P4::", 4) == 0)
        pass += 4;
    cstring name = cstring(manager) + "_" + Util::toString(seq) + "_" + pass;
    if (Log::verbose())
        std::cerr << name << std::endl;

    for (auto s : top4) {
        bool match = false;
        try {
            auto s_regex = std::regex(s, std::regex_constants::ECMAScript);
            // we use regex_search instead of regex_match
            // regex_match compares the regex against the entire string
            // regex_search checks if the regex is contained as substring
            match = std::regex_search(name.begin(), name.end(), s_regex);
        } catch (const std::regex_error& e) {
            ::error(ErrorType::ERR_INVALID,
                    "Malformed toP4 regex string \"%s\".\n"
                    "The regex matcher follows ECMAScript syntax.",
                    s);
            exit(1);
        }
        if (match) {
            cstring suffix = cstring("-") + name;
            cstring filename = file;
            if (filename == "-")
                filename = "tmp.p4";

            cstring fileName = makeFileName(dumpFolder, filename, suffix);
            auto stream = openFile(fileName, true);
            if (stream != nullptr) {
                if (Log::verbose())
                    std::cerr << "Writing program to " << fileName << std::endl;
                P4::ToP4 toP4(stream, Log::verbose(), file);
                node->apply(toP4);
                delete stream;  // close the file
            }
            break;
        }
    }
}

bool ParserOptions::isAnnotationDisabled(const IR::Annotation* a) const {
    if (disabledAnnotations.count(a->name.name) > 0) {
        ::warning(ErrorType::WARN_IGNORE,
                  "%1% is ignored because it was explicitly disabled", a);
        return true;
    }
    return false;
}

DebugHook ParserOptions::getDebugHook() const {
    using namespace std::placeholders;
    auto dp = std::bind(&ParserOptions::dumpPass, this, _1, _2, _3, _4);
    return dp;
}

/* static */ P4CContext& P4CContext::get() {
    return CompileContextStack::top<P4CContext>();
}

const P4CConfiguration& P4CContext::getConfig() {
    if (CompileContextStack::isEmpty())
        return DefaultP4CConfiguration::get();
    return get().getConfigImpl();
}

bool P4CContext::isRecognizedDiagnostic(cstring diagnostic) {
    static const std::unordered_set<cstring> recognizedDiagnostics = {
        "uninitialized_out_param", "uninitialized_use", "unknown_diagnostic"};

    return recognizedDiagnostics.count(diagnostic);
}

const P4CConfiguration& P4CContext::getConfigImpl() {
    return DefaultP4CConfiguration::get();
}
