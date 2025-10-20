#pragma once

#include "components/app/app.h"

class AppBuilder {
public:
    virtual App create() = 0;
};