#ifndef UTILITY_STRINGS_HEADER_INCLUDED
#define UTILITY_STRINGS_HEADER_INCLUDED

namespace sd2 {
namespace util {
namespace strings {
extern inline int len(const char* str) { return (str[0] != '\0' ? 1 + len(str + 1) : 0); }
extern inline int size(const char* str) {
  int length = len(str);
  return length > 0 ? length + 1 : 0;
}
extern inline int next_word(char* output_string, const char* input_string, int& current_position, int max_position) {
  int i = 0;
  char current_char = '\0';
  while (current_position <= max_position && (current_char = input_string[current_position++]) != '\0') {
    output_string[i++] = current_char = current_char != ' ' && current_char != '\n' ? current_char : '\0';
    if (current_char == '\0')
      break;
  }
  return i;
}
} // namespace strings
} // namespace util
} // namespace sd2
#endif // !UTILITY_STRINGS_HEADER_INCLUDED
