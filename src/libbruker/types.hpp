#ifndef DATA_TYPES_HPP_
#define DATA_TYPES_HPP_

#define PI 3.14159265

#include <complex>

#include "ndarray.hpp"

namespace mr_recon
{
	typedef std::complex<float> ComplexFloat;
	typedef std::complex<double> ComplexDouble;

	typedef NDArray<ComplexFloat> ComplexFloatArray;
	typedef NDArray<ComplexDouble> ComplexDoubleArray;

}

#endif //DATA_TYPES_HPP
