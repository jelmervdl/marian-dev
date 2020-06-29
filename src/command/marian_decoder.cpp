#include "marian.h"
#include "translator/beam_search.h"
#include "translator/translator.h"
#include "common/timer.h"
#include <stdio.h>
#ifdef _WIN32
#include <Windows.h>
#endif

// https://stackoverflow.com/questions/180947/base64-decode-snippet-in-c
static const std::string base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";

static inline bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_decode(std::string const& encoded_string) {
  int in_len = encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  std::string ret;
  while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_]; in_++;
    if (i ==4) {
      for (i = 0; i <4; i++)
        char_array_4[i] = base64_chars.find(char_array_4[i]);
      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
      for (i = 0; (i < 3); i++)
        ret += char_array_3[i];
      i = 0;
    }
  }
  if (i) {
    for (j = i; j <4; j++)
      char_array_4[j] = 0;
    for (j = 0; j <4; j++)
      char_array_4[j] = base64_chars.find(char_array_4[j]);
    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
    for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
  }
  return ret;
}

int main(int argc, char** argv) {
  using namespace marian;
  auto options = parseOptions(argc, argv, cli::mode::translation);

  std::ifstream input(options->get<std::string>("trie-pruning-path-file"));
  // each line contains a pair of domains in source and target, separated by a tab.

  for( std::string line; getline( input, line ); ) {
    boost::trim_right(line);
    size_t pos = line.find('\t');
    std::string srcBase64Encoded = line.substr(0, pos);
    std::string trgBase64Encoded = line.substr(pos + 1, line.length() - pos);

    std::ofstream source_file;
    source_file.open("temp_source");
    source_file << base64_decode(srcBase64Encoded);
    source_file.close();
    std::ofstream target_file;
    target_file.open("temp_target");
    target_file << base64_decode(trgBase64Encoded);
    target_file.close();

    options->set("trie-pruning-path", "temp_target");
    auto task = New<TranslateService<BeamSearch>>(options);

    std::ifstream ifs("temp_source");
    std::string inputText( (std::istreambuf_iterator<char>(ifs) ),
                           (std::istreambuf_iterator<char>()    ) );

    auto outputText = task->run(inputText);
    LOG(info, "{}\n", outputText);
    remove("temp_source");
    remove("temp_target");
  }
  return 0;
}
