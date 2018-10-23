#include"tests.h"
int main(int argc, char* argv[])
{
  try {
    check_map(argc, argv);
    return object_loader(argc, argv);
  } catch (std::runtime_error& error) {
    std::cout << error.what();
  } catch (...) {
    std::cout << "unknown error occurred\n";
  }
  return 1;

}