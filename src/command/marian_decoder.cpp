#include "marian.h"
#include "translator/beam_search.h"
#include "translator/translator.h"
#include "common/timer.h"
#ifdef _WIN32
#include <Windows.h>
#endif

int main(int argc, char** argv) {
  using namespace marian;
  auto options = parseOptions(argc, argv, cli::mode::translation);

  std::ifstream input(options->get<std::string>("trie-pruning-path-file"));
  for( std::string line; getline( input, line ); ) {
    boost::trim_right(line);
    size_t pos = line.find('\t');
    std::string srcFile = line.substr(0, pos);
    std::string trgTrieFile = line.substr(pos + 1, line.length() - pos);

    LOG(info, "[trie-ecoding] Currently decoding " + srcFile + " constrained by the trie built from " + trgTrieFile);
    options->set("trie-pruning-path", trgTrieFile);

    auto task = New<TranslateService<BeamSearch>>(options);

    std::ifstream ifs(srcFile);
    std::string inputText( (std::istreambuf_iterator<char>(ifs) ),
                           (std::istreambuf_iterator<char>()    ) );

    auto outputText = task->run(inputText);
    LOG(info, "Best translation: {}", outputText);
  }
  return 0;
}
