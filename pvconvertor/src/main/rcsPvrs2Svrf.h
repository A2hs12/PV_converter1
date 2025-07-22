#ifndef RCS_PVRS2SVRF_H
#define RCS_PVRS2SVRF_H

#include <string>
#include <iostream>

class rcsPvrs2Svrf {
public:
    rcsPvrs2Svrf() {}
    ~rcsPvrs2Svrf() {}
    bool execute(const std::string &pvsFile, const std::string &svrfFile, std::ostream &out);
};

#endif // RCS_PVRS2SVRF_H
