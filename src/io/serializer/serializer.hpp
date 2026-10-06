#pragma once

#include <istream>
#include <ostream>

#include "app/document.hpp"

namespace paint::io {

class Serializer {
   public:
    virtual ~Serializer() = default;

    virtual void serialize(std::ostream& output, const app::DocumentData& document) const = 0;
    virtual app::DocumentData deserialize(std::istream& input) const = 0;
};

}  // namespace paint::io
