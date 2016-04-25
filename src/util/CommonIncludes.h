#ifndef COMMONINCLUDES_H
#define COMMONINCLUDES_H

/*A file for any includes or structs that need to be used in many different places. */

#include "engine/enginedef.h"

#include "GL/glew.h"
#include <iostream>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

#include "fmod_common.h"
#include "fmod.hpp"
#include "fmod_errors.h"

#define RAND2_MAX USHRT_MAX
unsigned rand2();

#endif // COMMONINCLUDES_H
