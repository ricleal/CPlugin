#include "stdafx.h"
#include "parameters.hh"

using namespace std;

#ifndef DBL_INF
#define DBL_INF numeric_limits<double>::infinity()
#endif

WeightPoint::WeightPoint() {
	value = 0.0;
	weight = 0.0;
}
WeightPoint::WeightPoint(double v, double w) {
	value = v;
	weight = w;
}

Parameter::Parameter() {
	value = 0;
	min   = 0.0;
	max   = 0.0;
	has_min = false;
	has_max = false;
	has_dispersion = false;
	dispersion = NULL;
}
Parameter::Parameter(double _value) {
	value = _value;
	min   = 0.0;
	max   = 0.0;
	has_min = false;
	has_max = false;
	has_dispersion = false;
	dispersion = NULL;
}
Parameter::Parameter(double _value, bool disp) {
	value = _value;
	min   = 0.0;
	max   = 0.0;
	has_min = false;
	has_max = false;
	has_dispersion = disp;
}

void Parameter::get_weights(vector<WeightPoint> &weights) {
    weights.clear();
    weights.reserve(dispersion->Length);
    
    double* pValue  = dispersion->Values;
    double* pWeight = dispersion->Weights;
    for (int i = 0, n = dispersion->Length; i != n; i++)
        weights.push_back(WeightPoint(*pValue++, *pWeight++));
}
void Parameter::set_min(double value) {
	min = value;
	has_min = min != -DBL_INF;
}
void Parameter::set_max(double value) {
	max = value;
	has_max = max != +DBL_INF;
}

double Parameter::operator()() {
	return value;
}
double Parameter::operator=(double _value){
	value = _value;
	return value;
}

void Parameter::update(const ParameterInfo& info, void* p) {
    value = info.Default;
    min   = info.DispMin;
	max   = info.DispMax;
	has_min = min != -DBL_INF;
	has_max = max != +DBL_INF;
	has_dispersion = (info.Flags & PF_Polydisperse) != 0;

    if (has_dispersion) {
        dispersion = &AsPolydisperseParameter(p);
    } else {
        value = AsSimpleParameter(p);
	    dispersion = NULL;
    }
}
