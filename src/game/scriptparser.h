#pragma once

#include "cogscript.h"
#include <QStringList>

class QTextStream;

namespace COG {

class ScriptParser {
public:
    ScriptParser(const char *file);

private:
    const char *m_fileName;
    QTextStream *m_stream;
    int m_lineNumber;
    COGScript *m_script;

private:
    int nextLine(QStringList &line);
    std::ostream &lineErr();
    int parseRoot(COGScript *&script);
    int parseBehavior(COGScriptBehavior *&behavior);
    int parseAction(QStringList &line, COGScriptAction *&action);
    int parseActionSpawn(QStringList &line, COGScriptAction *&action);
    int parseActionShoot(QStringList &line, COGScriptAction *&action);
    int parseActionPlay(QStringList &line, COGScriptAction *&action);
    int parseActionSetAttribute(QStringList &line, COGScriptAction *&action);

    Attribute peekAttribute(QStringList &line, int pos, int &length);
    int parseAttribute(QStringList &line, int pos, COGScriptActionSetAttribute *&set);
    int parseAttributeSpatial(QStringList &line, int pos, COGScriptAttributeSpatial *&attr);

public:
    COGScript *parse();
};

}
