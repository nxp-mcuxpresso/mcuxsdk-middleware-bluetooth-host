%module ranging

%{
#include "../src/PythonWrapper.h"
#include "../../../libs/common/include/RangingSettings.h"
%}


%include <std_vector.i>
%include <std_complex.i>

%template(Integer) std::vector<int>;
%template(Float) std::vector<float>;
%template(Complex) std::vector<std::complex<float> >;


%include "../src/PythonWrapper.h"
%include "../../../libs/common/include/RangingSettings.h"

