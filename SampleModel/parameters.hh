#pragma once

#include <vector>
#include "ModelInfo.h"

class WeightPoint {
public:
	double value;
	double weight;

	WeightPoint();
	WeightPoint(double, double);
};

class Parameter {
public:
	/// Current value of the parameter
	double value;
	/// True if the parameter has a minimum bound
	bool has_min;
	/// True if the parameter has a maximum bound
	bool has_max;
	/// Minimum bound
	double min;
	/// Maximum bound
	double max;
	/// True if the parameter can be dispersed or averaged
	bool has_dispersion;
	/// Pointer to the dispersion model object for this parameter
	PolydisperseParameter* dispersion;

    Parameter();
	Parameter(double);
	Parameter(double, bool);

	/// Method to set a minimum value
	void set_min(double);
	/// Method to set a maximum value
	void set_max(double);
	/// Method to get weight points for this parameter
	void get_weights(std::vector<WeightPoint>&);
	/// Returns the value of the parameter
	double operator()();
	/// Sets the value of the parameter
	double operator=(double);
    
    void update(const ParameterInfo&, void* = NULL);
};
