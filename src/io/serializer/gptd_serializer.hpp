#pragma once

#include "io/serializer/serializer.hpp"

namespace paint::io {

class GptdSerializer : public Serializer {
   public:
    void serialize(std::ostream& output, const app::DocumentData& document) const override;
    app::DocumentData deserialize(std::istream& input) const override;
};

}  // namespace paint::io
