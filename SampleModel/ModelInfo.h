#ifndef MODELINFO_H
#define MODELINFO_H

#ifdef _MSC_VER
#define CExport extern "C" __declspec(dllexport)
#else
#define CExport extern "C"
#endif

#include <stdint.h>
#include <iostream>
#include <limits>

enum ParameterFlags {
    PF_None         = 0x00,
    PF_Orientation  = 0x01,
    PF_Magnetic     = 0x02,
    PF_Unfittable   = 0x04,
    PF_Integer      = 0x08,
    PF_Polydisperse = 0x10,
    PF_RepeatCount  = 0x20 | PF_Unfittable, // for multiplicity
    PF_Repeated     = 0x40,
};

struct ParameterInfo {
    char*   Name;
    char*   Description;
    char*   Unit;
    double  Default;
    double  DispMin;
    double  DispMax;
    size_t  Flags;
};

#define GetParameterCount(parameters) (sizeof(parameters) / sizeof(ParameterInfo))

struct ModelInfo {
    size_t          Version;
    char*           ModelName;
    char*           ModelDescription;
    size_t          ParameterCount;
    ParameterInfo*  Parameters;
    
    ModelInfo(char* modelName, char* modelDescription, size_t parameterCount, ParameterInfo* parameters) :
        Version(1),
        ModelName(modelName),
        ModelDescription(modelDescription),
        ParameterCount(parameterCount),
        Parameters(parameters) {
    }
};

enum ParameterType {
    PT_End          = 0x00000000,
    PT_Simple       = 0xAAAAAAA1,
    PT_Polydisperse = 0xAAAAAAA2,
};

struct SimpleParameter {
    size_t Type;
    double Value;
};
struct PolydisperseParameter {
    size_t Type;
    size_t Length;
    double *Values;
    double *Weights;
};

inline SimpleParameter* GetSimpleParameter(void* p) {
    if (*(int*)p == PT_Simple)
        return (SimpleParameter*)p;
    else
        return NULL;
}
inline PolydisperseParameter* GetPolydisperseParameter(void* p) {
    if (*(int*)p == PT_Polydisperse)
        return (PolydisperseParameter*)p;
    else
        return NULL;
}

#define AsSimpleParameter(p)        (GetSimpleParameter(p)->Value)
#define AsPolydisperseParameter(p)  (*GetPolydisperseParameter(p))

CExport void* get_model_info();
CExport void* create_model(void* data);
CExport void destroy_model(void* ptr);
CExport void calculate_q(void* ptr, void** p, size_t nq, double iq[], double q[]);
CExport void calculate_qxqy(void* ptr, void** p, size_t nq, double iq[], double qx[], double qy[]);
CExport void calculate_qxqyqz(void* ptr, void** p, size_t nq, double iq[], double qx[], double qy[], double qz[]);
CExport double calculate_ER(void* ptr, void** p);
CExport double calculate_VR(void* ptr, void** p);

#ifndef DBL_INF
#define DBL_INF std::numeric_limits<double>::infinity()
#endif
#ifndef DBL_NAN
#define DBL_NAN std::numeric_limits<double>::quiet_NaN()
#endif

#endif // MODELINFO_H
