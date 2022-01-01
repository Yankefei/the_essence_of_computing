#include <iostream>
#include <fstream>
#include "stream.h"

namespace tools
{

//std::ofstream of("text.txt", std::ofstream::app);

Stream stream{/*&of,*/ &std::cout};

void Boolalpha(Stream& os)   { Stream::set_boolalpha(os, true); }
void NoBoolalpha(Stream& os) { Stream::set_boolalpha(os, false); }

}