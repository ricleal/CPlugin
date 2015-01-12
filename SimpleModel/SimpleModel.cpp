#include "ModelInfo.h"

ParameterInfo param_infos[] = {
    { "radius"    , "radius of sphere", "A"   , 60.0, 0.0, +DBL_INF, PF_None},
    { "background", NULL              , "1/cm",  0.0, 0.0, +DBL_INF, PF_None},
};

ModelInfo model_info(
    "SimpleModel",
    "P(q)= nonsense + bkg",
    GetParameterCount(param_infos),
    param_infos);

// model handling
CExport void* get_model_info() {
    return &model_info;
}
// calculations
CExport void calculate_q(void* ptr, void* p, size_t nq, double iq[], double q[]) {
	Parameters parameters(p);

//	if (!parameters.valid())
//		return;

	if (!parameters.valid(model_info))
		return;

	double radius = parameters[0];
	double bkg    = parameters[1];

    for (size_t i = 0; i != nq; i++)
        iq[i] = q[i] * radius + bkg;
}
CExport double calculate_ER(void* ptr, void* p) {
	Parameters parameters(p);
	if (!parameters.valid())
		return DBL_NAN;

    double radius = parameters[0];
    return radius;
}
CExport double calculate_VR(void* ptr, void* p) {
	Parameters parameters(p);
	if (!parameters.valid())
		return DBL_NAN;

    return 1.0;
}
