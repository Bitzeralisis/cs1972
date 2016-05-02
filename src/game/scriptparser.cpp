#include "scriptparser.h"
#include <QFile>
#include <QRegularExpression>
#include <QTextStream>
#include <iostream>

using namespace COG;

ScriptParser::ScriptParser(const char *file)
    : m_fileName(file)
{ }

int ScriptParser::nextLine(QStringList &line) {
    QString nextLine;
    do {
        if (!m_stream->readLineInto(&nextLine, 0))
            return 1;
        line = nextLine.trimmed().split(QRegularExpression("\\s+"));
        ++m_lineNumber;
    } while (line[0] == "");
    return 0;
}

std::ostream &ScriptParser::lineErr() {
    return std::cerr << "Line " << m_lineNumber << ": ";
}

int ScriptParser::parseRoot(COGScript *&script) {
    script = new COGScript();

    // Special behavior none
    script->behaviors["none"] = 0;

    QStringList line;
    while (!nextLine(line)) {
        if (line.length() < 2) {
            lineErr() << "Warning: Found something other than behavior definition at root" << std::endl;
            continue;
        }
        if (line[0] == "def") {
            std::string name = line[1].toStdString();
            COGScriptBehavior *behavior = new COGScriptBehavior(script);
            if (parseBehavior(behavior))
                return 1;
            if (script->behaviors.count(name)) {
                lineErr() << "Warning: Redefinition of behavior " << name << std::endl;
                delete behavior;
                continue;
            }
            script->behaviors[name] = behavior;
        }
    }
    return 0;
}

int ScriptParser::parseBehavior(COGScriptBehavior *&behavior) {
    QStringList line;
    while (!nextLine(line)) {
        if (line[0] == QString("end"))
            return 0;
        if (line.length() < 2) {
            lineErr() << "Warning: Expected an action" << std::endl;
            continue;
        }
        COGScriptAction *action;
        if (parseAction(line, action)) {
            delete action;
            continue;
        }
        behavior->actions.push_back(action);
    }
    lineErr() << "Warning: Reached end of file without finding end of behavior" << std::endl;
    return 0;
}

int ScriptParser::parseAction(QStringList &line, COGScriptAction *&action) {
    bool queued = (line[0] == "q");
    if (queued)
        line.pop_front();

    std::string command = line[1].toStdString();

    int retval = 0;
    if      (command == "disappear") action = new COGScriptAction(Action::DISAPPEAR);
    else if (command == "set")       retval = parseActionSetAttribute(line, action);
    else if (command == "spawn")     retval = parseActionSpawn(line, action);
    else if (command == "shoot")     retval = parseActionShoot(line, action);
    else if (command == "play")      retval = parseActionPlay(line, action);
    else {
        lineErr() << "Warning: Unrecognized action-name " << command << std::endl;
        return 1;
    }

    if (retval)
        return retval;

    action->queued = queued;
    action->beat = atof(line[0].toUtf8().constData());
    return 0;
}

int ScriptParser::parseActionShoot(QStringList &line, COGScriptAction *&action) {
    if (line.length() < 15) {
        lineErr() << "Warning: Shoot action missing arguments" << std::endl;
        return 1;
    }

    COGScriptActionShoot *shoot = new COGScriptActionShoot();
    action = shoot;
    shoot->travel = atof(line[2].toUtf8().constData());
    shoot->type = atof(line[3].toUtf8().constData());
    shoot->lane = atof(line[4].toUtf8().constData());

    if (line[5] != "pos") {
        lineErr() << "Warning: Expected pos, got " << line[3].toStdString() << std::endl;
        return 1;
    }
    shoot->pos.attr = Attribute::POSITION;
    COGScriptAttributeSpatial *pos = &shoot->pos;
    if (parseAttributeSpatial(line, 5, pos))
        return 1;

    if (line[10] != "vel") {
        lineErr() << "Warning: Expected vel, got " << line[3].toStdString() << std::endl;
        return 1;
    }
    shoot->vel.attr = Attribute::VELOCITY;
    COGScriptAttributeSpatial *vel = &shoot->vel;
    if (parseAttributeSpatial(line, 10, vel))
        return 1;

    if (line.length() > 15)
        lineErr() << "Warning: Extra arguments at end of shoot action" << std::endl;

    return 0;
}

int ScriptParser::parseActionSpawn(QStringList &line, COGScriptAction *&action) {
    if (line.length() < 4) {
        lineErr() << "Warning: Spawn action missing arguments" << std::endl;
        return 1;
    }

    COGScriptActionSpawn *spawn = new COGScriptActionSpawn();
    action = spawn;
    spawn->type = line[2].toStdString();
    spawn->behavior = line[3].toStdString();

    spawn->init = new COGScriptBehavior(m_script);
    int pos = 4;
    while (pos < line.length()) {
        int length;
        peekAttribute(line, pos, length);
        COGScriptActionSetAttribute *set;
        parseAttribute(line, pos, set);
        set->beat = 0.f;
        spawn->init->actions.push_back(set);
        pos += length;
    }

    return 0;
}

int ScriptParser::parseActionPlay(QStringList &line, COGScriptAction *&action) {
    if (line.length() < 7) {
        lineErr() << "Warning: Play action missing arguments" << std::endl;
        return 1;
    }

    COGScriptActionPlay *play = new COGScriptActionPlay();
    action = play;
    play->sound = line[2].toStdString();
    play->bpm = atof(line[3].toUtf8().constData());
    play->offset = atof(line[4].toUtf8().constData());
    play->loopStart = atof(line[5].toUtf8().constData());
    play->loopEnd = atof(line[6].toUtf8().constData());

    if (line.length() > 7)
        lineErr() << "Warning: Extra arguments at end of play action" << std::endl;

    return 0;
}

int ScriptParser::parseActionSetAttribute(QStringList &line, COGScriptAction *&action) {
    if (line.length() < 2) {
        lineErr() << "Warning: Set attribute action missing arguments" << std::endl;
        return 1;
    }
    COGScriptActionSetAttribute *set;
    int retval = parseAttribute(line, 2, set);
    action = set;
    return retval;
}

Attribute ScriptParser::peekAttribute(QStringList &line, int pos, int &length) {
    QString name = line[pos];
    if      (name == "pos")        { length = 5; return Attribute::POSITION; }
    else if (name == "vel")        { length = 5; return Attribute::VELOCITY; }
    else if (name == "acc")        { length = 5; return Attribute::ACCELERATION; }
    else if (name == "targetable") { length = 2; return Attribute::TARGETABLE; }
    else                           { length = 1; return Attribute::UNDEFINED; }
}

int ScriptParser::parseAttribute(QStringList &line, int pos, COGScriptActionSetAttribute *&set) {
    int length;
    Attribute type = peekAttribute(line, pos, length);

    switch (type) {
    case Attribute::POSITION:
    case Attribute::VELOCITY:
    case Attribute::ACCELERATION: {
        COGScriptAttributeSpatial *attr = new COGScriptAttributeSpatial();
        set = attr;
        parseAttributeSpatial(line, pos, attr);
        break;
    }

    case Attribute::TARGETABLE: {
        COGScriptAttributeFloat *attr = new COGScriptAttributeFloat();
        set = attr;
        attr->value = atof(line[pos+1].toUtf8().constData());
        break;
    }

    default:
        lineErr() << "Warning: Unrecognized attribute-name " << line[pos].toStdString() << std::endl;
        return 1;
    }

    set->attr = type;
    return 0;
}

int ScriptParser::parseAttributeSpatial(QStringList &line, int pos, COGScriptAttributeSpatial *&attr) {
    if (line.length()-pos < 5) {
        lineErr() << "Warning: Incomplete spatial attribute" << std::endl;
        return 1;
    }

    if      (line[pos+1] == "abs") attr->relative = false;
    else if (line[pos+1] == "rel") attr->relative = true;
    else {
        lineErr() << "Warning: Unrecognized relative-type " << line[pos+1].toStdString() << std::endl;
        return 1;
    }

    attr->coord = glm::vec3(atof(line[pos+2].toUtf8().constData()), atof(line[pos+3].toUtf8().constData()), atof(line[pos+4].toUtf8().constData()));

    if (line[pos+2] == ".") attr->coord.x = NAN;
    if (line[pos+3] == ".") attr->coord.y = NAN;
    if (line[pos+4] == ".") attr->coord.z = NAN;

    return 0;
}

COGScript *ScriptParser::parse() {
    QFile file(m_fileName);
    file.open(QIODevice::ReadOnly);
    m_stream = new QTextStream(&file);
    m_lineNumber = 0;

    parseRoot(m_script);
    std::cerr.flush();

    delete m_stream;
    file.close();

    return m_script;
}
