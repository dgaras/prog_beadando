#include "solver.h"
#include <QDebug>
#include "gridelement.h"
#include "griddata.h"


// Konstruktor
#include "solver.h"

Solver::Solver(const QList<GridElement> &inputElements, int width, int height, GridData &gridData)
    : elements(inputElements.toVector()), gridWidth(width), gridHeight(height), gridData(gridData) {
    grid.resize(height);
    visited.resize(height, QVector<bool>(width, false));

    for (int y = 0; y < height; ++y) {
        grid[y].resize(width);
        for (int x = 0; x < width; ++x) {
            grid[y][x] = GridElement();
        }
    }

    for (const auto &element : elements) {
        grid[element.getY()][element.getX()] = element;
    }
}




// Solver futtatása
bool Solver::solve() {
    qDebug() << "Útkeresés indítása...";

    // Forrásokból indulunk
    for (const auto &element : elements) {
        if (element.getType() == ElementType::Source && element.isActive()) {
            qDebug() << "Forrás keresése a pozíción: " << element.getX() << "," << element.getY();
            if (findPath(element.getX(), element.getY())) {
                qDebug() << "Megoldás megtalálva!";
                return true;
            }
        }
    }

    qDebug() << "Nem sikerült megoldást találni.";
    return false;
}

// Útkereső algoritmus (rekurzív DFS)
bool Solver::findPath(int x, int y) {
    if (x < 0 || y < 0 || x >= gridWidth || y >= gridHeight) {
        qDebug() << "Pozíció kívül esik az érvényes határokon! (" << x << "," << y << ")";
        return false;
    }

    if (visited[y][x]) {
        qDebug() << "Már meglátogatott hely: (" << x << "," << y << ")";
        return false;

    }

    auto element = grid[y][x];
    qDebug() << "Pozíció vizsgálata: (" << x << "," << y << ") Típus: "
             << static_cast<int>(element.getType()) << "    Aktív: " << element.isActive();

    if (element.getType() == ElementType::Sink && element.isActive()) {
        qDebug() << "Elnyelő megtalálva a pozíción: (" << x << "," << y << ")";
        return true;
    }

    visited[y][x] = true;

    // Forrás pozíción ne helyezzünk el új csövet, csak haladjunk tovább
    if (element.getType() == ElementType::Source) {
        qDebug() << "Forrás pozíció. Cső elhelyezés kihagyva: (" << x << "," << y << ")";
        if (findPath(x + 1, y) || findPath(x - 1, y) || findPath(x, y + 1) || findPath(x, y - 1)) {
            return true;
        }
        visited[y][x] = false;
        return false;
    }

    QVector<bool> connections = element.getConnections();

    // Először próbáljunk kanyar csöveket elhelyezni
    for (int rotation = 0; rotation < 4; ++rotation) {
        QVector<bool> rotatedConnections = rotateConnections(connections, rotation);
        if (placePipe(x, y, ElementType::Bend, rotatedConnections, rotation)) {
            if (findPath(x + 1, y) || findPath(x - 1, y) ||
                findPath(x, y + 1) || findPath(x, y - 1)) {
                return true;
            }
            backtrack(x, y);  // Visszalépés, ha nem vezet megoldáshoz
        }
    }

    // Ha kanyar nem működött, próbáljunk egyenes csövet elhelyezni
    if (placePipe(x, y, ElementType::Pipe, connections, 0)) {
        if (findPath(x + 1, y) || findPath(x - 1, y) ||
            findPath(x, y + 1) || findPath(x, y - 1)) {
            return true;
        }
        backtrack(x, y);
    }

    visited[y][x] = false;
    qDebug() << "Nem sikerült útvonalat találni ezen az úton: (" << x << "," << y << ")";
    return false;
}




// Cső elhelyezésének módosítása (kanyar és egyenes figyelembe vétele)
bool Solver::placePipe(int x, int y, ElementType type, QVector<bool> &connections, int rotation) {
    PipeType pipeType = elementTypeToPipeType(type);

    if (gridData.getPipeCount(pipeType) > 0 && isValid(x, y, connections)) {
        grid[y][x] = GridElement(type, true, x, y, connections);
        gridData.setPipeCount(pipeType, gridData.getPipeCount(pipeType) - 1);
        qDebug() << "Cső elhelyezve: (" << x << "," << y << ") Típus:" << static_cast<int>(pipeType)
                 << " Maradék cső:" << gridData.getPipeCount(pipeType);
        return true;
    } else {
        qDebug() << "Nincs elegendő cső készleten! (" << x << "," << y << ")";
    }
    return false;
}






// Visszalépés (cső eltávolítása)
void Solver::backtrack(int x, int y) {
    ElementType type = grid[y][x].getType();
    grid[y][x].setActive(false);

    if (type != ElementType::Source && type != ElementType::Sink) {
        PipeType pipeType = elementTypeToPipeType(type);  // Típuskonverzió
        gridData.setPipeCount(pipeType, gridData.getPipeCount(pipeType) + 1);  // Visszaadjuk a csövet
        qDebug() << "Cső eltávolítva a pozícióról: (" << x << "," << y
                 << ") Visszaadott cső:" << gridData.getPipeCount(pipeType);
    }
}


bool Solver::isValid(int x, int y, const QVector<bool> &connections) {
    if (x < 0 || y < 0 || x >= gridWidth || y >= gridHeight) return false;

    // Ellenőrizzük minden irányban a csatlakozást
    bool valid = false;
    if (y > 0 && (grid[y - 1][x].getConnections()[1] || connections[0])) valid = true;  // Észak
    if (y < gridHeight - 1 && (grid[y + 1][x].getConnections()[0] || connections[1])) valid = true;  // Dél
    if (x > 0 && (grid[y][x - 1].getConnections()[2] || connections[3])) valid = true;  // Nyugat
    if (x < gridWidth - 1 && (grid[y][x + 1].getConnections()[3] || connections[2])) valid = true;  // Kelet

    if (!valid) {
        qDebug() << "Nincs megfelelő csatlakozás! (" << x << "," << y << ")";
    }

    return valid;
}



PipeType Solver::elementTypeToPipeType(ElementType type) {
    switch (type) {
    case ElementType::Pipe: return PipeType::cső;
    case ElementType::Bend: return PipeType::kanyar;
    case ElementType::Tjunction: return PipeType::Tjunction;
    case ElementType::Cross: return PipeType::Cross;
    case ElementType::PipeValve: return PipeType::csőcap;
    case ElementType::BendValve: return PipeType::kanyarcsap;
    case ElementType::TjunctionValve: return PipeType::Tjunctioncsap;
    case ElementType::CrossValve: return PipeType::Crosscsap;
    default: return PipeType::cső;  // Alapértelmezett visszatérés
    }
}

QVector<bool> Solver::rotateConnections(const QVector<bool> &connections, int rotation) {
    QVector<bool> rotated = connections;
    for (int i = 0; i < rotation; ++i) {
        bool temp = rotated[3];
        rotated[3] = rotated[2];
        rotated[2] = rotated[1];
        rotated[1] = rotated[0];
        rotated[0] = temp;
    }
    return rotated;
}

