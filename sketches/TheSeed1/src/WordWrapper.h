#pragma once

#include "common/xf/Font.h"

class WordWrapper
{
public:
    size_t size;
    std::vector<int> offsets;
    std::vector<int> lengths;

    WordWrapper() = default;

    void wrap(const chr::XFont &font, const std::u16string &text, const std::vector<float> &widths);

protected:
    void addLine(const std::u16string &text, int offset, int length);
};
