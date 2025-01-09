#include "gridelement.h"

// Konstruktor
GridElement::GridElement(ElementType type, bool active, int x, int y,
                         const QVector<bool> &connections, const QString &color)
    : type(type), active(active), x(x), y(y), connections(connections), color(color) {}

// Elem típusának lekérdezése
ElementType GridElement::getType() const {
    return type;
}

// Aktív állapot lekérdezése és beállítása
bool GridElement::isActive() const {
    return active;
}

void GridElement::setActive(bool state) {
    active = state;
}

// Pozíció lekérdezése és beállítása
int GridElement::getX() const {
    return x;
}

int GridElement::getY() const {
    return y;
}

void GridElement::setPosition(int newX, int newY) {
    x = newX;
    y = newY;
}

// Irányok lekérdezése és beállítása
QVector<bool> GridElement::getConnections() const {
    return connections;
}

void GridElement::setConnections(const QVector<bool> &newConnections) {
    connections = newConnections;
}

// Szín lekérdezése és beállítása
QString GridElement::getColor() const {
    return color;
}

void GridElement::setColor(const QString &newColor) {
    color = newColor;
}
