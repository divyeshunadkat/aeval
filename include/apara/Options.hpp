#ifndef __OPTIONS_HPP
#define __OPTIONS_HPP

#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

namespace apara {

  using namespace std;
  using namespace boost::program_options;
  using namespace boost::filesystem;

  class Options
  {
  protected:
    string inputFileName;
    string inputFilePath;
    string outputFileName;
    string outputFilePath;

    int verbosity;
    string version = "1.0";
    string toolName = "apara";

  public:
    inline string getInputFile() { return inputFilePath; }
    inline string getInputFileName() { return inputFileName; }
    inline string getOutputFile() { return outputFilePath; }
    inline string getOutputFileName() { return outputFileName; }
    inline int getVerbosity() { return verbosity; }

    bool parse_config(path configFileName) {
      boost::filesystem::ifstream cfg_file(configFileName);
      variables_map vm;
      options_description config;
      options_description cmdline;
      options_description hlp;
      positional_options_description pd;

      get_description_cmd(config, cmdline, hlp, pd);
      notify(vm);
      try {
        store(parse_config_file(cfg_file, config, false), vm);
        notify(vm);
        interpret_advanced_options(vm);
      } catch ( const error& e ) {
        cout << e.what();
        return false;
      }
    }

    bool parse_cmdline(int argc, char** argv) {
      variables_map vm;
      options_description config;
      options_description cmdline;
      options_description hlp;
      positional_options_description pd;

      get_description_cmd(config, cmdline, hlp, pd);
      try {
        store(command_line_parser(argc, argv).options(cmdline).positional(pd).run(), vm);
        notify(vm);
        if (vm.count("version")) {
          cout << "\nVersion : " << version << "\n\n";
          return false;
        }
        if (vm.count("help")) {
          show_help(hlp);
          return false;
        }
        if (!vm.count("input")) {
          cout << "\n  No input file specified\n\n";
          return false;
        }
        if (vm.count("config")) {
          path path(vm["config"].as<string>());
          if(!parse_config(path)) return false;
        }
        interpret_advanced_options(vm);
      } catch ( const error& e ) {
        cout << e.what();
        return false;
      }
      return true;
    }

  private:
    void get_description_cmd(options_description& config,
                             options_description& cmdline,
                             options_description& hlp,
                             positional_options_description& pd) {
      options_description core("Core Options");
      options_description generic("Generic Options");
      options_description hidden("Hidden Options");

      core.add_options()
        ("output,o", value(&outputFilePath)->default_value("/tmp/result.smt2"), "Set output file")
        ("config,c", value<string>(), "Set config file")
        ;
      hidden.add_options()
        ("input,i", value(&inputFilePath), "Set source files")
        ;
      generic.add_options()
        ("version", "Print version string")
        ("verbose,v", value<int>(&verbosity)->default_value(0), "Set verbosity level")
        ("help,h", "Print help")
        ;
      pd.add("input", -1);
      config.add(core).add(hidden);
      cmdline.add(config).add(generic);
      hlp.add(core).add(generic);
    }

    void interpret_advanced_options(variables_map& vm) {
      if (vm.count("input")) {
        path cf( inputFilePath );
        inputFileName = cf.filename().string();
        if (!exists(cf))
          throw error("\n  Input file " + inputFileName + " does not exist\n\n");
      }
      if (vm.count("output")) {
        path cf( outputFilePath );
        outputFileName = cf.filename().string();
      }
    }

    void show_help(options_description& desc) {
      cout << "\n\t" << toolName << " - Automatic Parallelization using CHCs\n";
      cout << "\t    Divyesh Unadkat and Grigory Fedyukovich \n\n";
      cout << "apara [Options] file.smt2 ...\n";
      cout << desc << "\n";
    }

  };

}

#endif
