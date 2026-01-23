#include <pybind11/pybind11.h>
#include "align.hpp"

PYBIND11_MODULE(align, m) {
    m.doc() = "Alignment helpers";
    m.def("GetBounds", &GetBounds, "bounds getter");
}