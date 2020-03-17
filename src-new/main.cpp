#include <iostream>

int main(int argc, char** argv)
{
  if (argc < 2)
    {
      std::cout << "simFDS <FDS file> -o <output dot file>" << std::endl;
      return 1;
    }
  return 0;
}
