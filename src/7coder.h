#pragma once

#include "bit_io.h"

#include <iostream>

// Encodes the ASCII input stream from `in` onto `out`.
void encode(std::istream& in, ipd::bostream& out);

// Decodes the 7-bit-packed input stream from `in` onto `out`.
void decode(ipd::bistream& in, std::ostream& out);
