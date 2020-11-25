#include "marian.h"
#include "translator/beam_search.h"
#include "translator/translator.h"
#include "common/timer.h"
#include <stdio.h>
#ifdef _WIN32
#include <Windows.h>
#endif

int main(int argc, char** argv) {
  using namespace marian;
  auto options = parseOptions(argc, argv, cli::mode::translation);

/*
  for (auto &&path : options->get<std::vector<std::string>>("input")) {
    try {
      std::ifstream ifs(path);

      std::string input(
        (std::istreambuf_iterator<char>(ifs)),
        (std::istreambuf_iterator<char>()));

      std::cerr << "Input: <<<END\n" << input << "\nEND" << std::endl;

      auto task = New<TranslateService<BeamSearch>>(options);

      std::cerr << "Set-up complete about to run" << std::endl;

      std::cout << task->run(input) << std::endl;


    } catch (std::exception const &e) {
      std::cerr << "Exception in main: " << e.what() << std::endl;
    } catch(std::string const &e) {
      std::cerr << "Exceptino in main: " << e << std::endl; 
    }
  }
  */

  auto task = New<Translate<BeamSearch>>(options);
  task->run();

  return 0;
}
