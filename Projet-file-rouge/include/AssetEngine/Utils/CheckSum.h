#pragma once

#include <cstdint>
#include "AssetEngine/Binary/BinaryTypes.h"

std::uint32_t CalculateChecksum(const Bytes& data);