#include "cogscript.h"

using namespace COG;

COGScriptActionSpawn::~COGScriptActionSpawn() {
    delete init;
}

COGScriptBehavior::~COGScriptBehavior() {
    for (std::deque<COGScriptAction *>::iterator it = actions.begin(); it != actions.end(); ++it)
        delete *it;
}

COGScript::~COGScript() {
    for (std::unordered_map<std::string, COGScriptBehavior *>::iterator it = behaviors.begin(); it != behaviors.end(); ++it)
        delete it->second;
}
