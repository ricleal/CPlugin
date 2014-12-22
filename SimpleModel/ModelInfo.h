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

#ifndef DBL_INF
#define DBL_INF std::numeric_limits<double>::infinity()
#endif
#ifndef DBL_NAN
#define DBL_NAN std::numeric_limits<double>::quiet_NaN()
#endif

// model information

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
	// fields
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
	// fields
    size_t          Version;
    char*           ModelName;
    char*           ModelDescription;
    size_t          ParameterCount;
    ParameterInfo*  Parameters;
    
	// constructor
    ModelInfo(char* modelName, char* modelDescription, size_t parameterCount, ParameterInfo* parameters) :
        Version(1),
        ModelName(modelName),
        ModelDescription(modelDescription),
        ParameterCount(parameterCount),
        Parameters(parameters) {
    }
};

// concrete parameters

enum ParameterType {
    PT_End          = 0xAAAAAAA0,
    PT_Simple       = 0xAAAAAAA1,
    PT_Polydisperse = 0xAAAAAAA2,
};

#pragma pack(push, 4) // 4-byte boundaries 
struct EndParameter {
	// fields
    const size_t type;

	// properties
    bool valid() const {
		return type == PT_End;
	}
};
struct SimpleParameter {
	// fields
    const size_t type;
    double value;

	// properties
    bool valid() const {
		return type == PT_Simple;
	}
};
struct PolydisperseParameter {

	class Values {
	private:
		// fields
		size_t _type;
		size_t _npoints;
		double _base;

	public:
		// opperators
		operator double*() {
			return &_base;
		}
		operator const double*() const {
			return &_base;
		}
		double& operator[](size_t index) {
			if (index < _npoints)
				return (&_base)[index];
			return *((double*)NULL);
		}
		const double& operator[](size_t index) const {
			if (index < _npoints)
				return (&_base)[index];
			return *((double*)NULL);
		}
	};
	
	class Weights {
	private:
		// fields
		size_t _type;
		size_t _npoints;
		double _base;

	public:		
		// opperators
		operator double*() {
			return &_base + _npoints;
		}
		operator const double*() const {
			return &_base + _npoints;
		}
		double& operator[](size_t index) {
			if (index < _npoints)
				return (&_base)[_npoints + index];
			return *((double*)NULL);
		}
		const double& operator[](size_t index) const {
			if (index < _npoints)
				return (&_base)[_npoints + index];
			return *((double*)NULL);
		}
	};
	
	// not supported
	PolydisperseParameter();
	PolydisperseParameter(const PolydisperseParameter&);
	PolydisperseParameter& operator=(const PolydisperseParameter&);

public:
	// fields
    union {
		struct {
			const size_t type;
			const size_t npoints;
		};
		Values  values;
		Weights weights;
	};

	// properties
    bool valid() const {
		return type == PT_Polydisperse;
	}
};
#pragma pack(pop)

class ParameterValue {
private:
	// fields
	void* _ptr;
	
	// helper
	SimpleParameter* GetSimpleParameter(void* p) {
		if (((SimpleParameter*)p)->valid())
			return (SimpleParameter*)p;
		return NULL;
	}
	PolydisperseParameter* GetPolydisperseParameter(void* p) {
		if (((PolydisperseParameter*)p)->valid())
			return (PolydisperseParameter*)p;
		return NULL;
	}

public:
	// constructor
	ParameterValue(void* ptr) : _ptr(ptr) {
	}

	// casts
	operator double&() {
		return GetSimpleParameter(_ptr)->value;
	}
	operator SimpleParameter&() {
		return *GetSimpleParameter(_ptr);
	}
	operator PolydisperseParameter&() {
		return *GetPolydisperseParameter(_ptr);
	}
};

class Parameters {
private:
	// fields
	size_t  _count;		// number of valid entries
	size_t* _offsets;	// relative offsets (in bytes) in reference to base address
	char*   _base;		// base address

public:
	// constructor
	Parameters(void* p) :
		_count(0), _offsets(NULL), _base(NULL) {

		if (p == NULL)
			return;
		
		// check format
		size_t  count   = *(size_t*)p;
		size_t* offsets =  (size_t*)p + 1;
		char*   base    = (char*)&offsets[count];
		
		// count is expected to be greater than zero
		if (count == 0)
			return;

		count--; // "EndParameter" is not a valid simple or polydisperse parameter

		// check if parameters are either simple or polydisperse
		for (size_t i = 0; i != count; i++) {
			size_t type = *(size_t*)(base + offsets[i]);
			if ((type != PT_Simple) && (type != PT_Polydisperse))
				return;
		}

		// check if data has correct end flag
		if (*(size_t*)(base + offsets[count]) != PT_End)
			return;

		// format appears to be correct
		_count   = count;
		_offsets = offsets;
		_base    = base;
	}

	// properties
	bool valid() const {
		return (_count != 0) && (_offsets != NULL) && (_base != NULL);
	}
	bool valid(const ModelInfo& model_info) const {
		if (!valid())
			return false;

		if (model_info.ParameterCount != _count)
			return false;

		for (size_t i = 0; i != _count; i++) {
			size_t type = *(size_t*)(_base + _offsets[i]);
			
			if ((model_info.Parameters[i].Flags & PF_Polydisperse) == PF_Polydisperse) {
				if (type != PT_Polydisperse)
					return false;
			} else {
				if (type != PT_Simple)
					return false;
			}
		}

		return true;
	}
	size_t count() const {
		return _count;
	}
	ParameterValue operator[](size_t index) {
		if (index < _count)
			return ParameterValue(_base + _offsets[index]);
		return ParameterValue(NULL);
	}
};

// exports

CExport void* get_model_info();
CExport void* create_model(void* data);
CExport void destroy_model(void* ptr);
CExport void calculate_q(void* ptr, void* p, size_t nq, double iq[], double q[]);
CExport void calculate_qxqy(void* ptr, void* p, size_t nq, double iq[], double qx[], double qy[]);
CExport void calculate_qxqyqz(void* ptr, void* p, size_t nq, double iq[], double qx[], double qy[], double qz[]);
CExport double calculate_ER(void* ptr, void* p);
CExport double calculate_VR(void* ptr, void* p);

#endif // MODELINFO_H
