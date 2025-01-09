#ifndef GRIDELEMENT_H
#define GRIDELEMENT_H

#include <QVector>
#include <QString>

// Elemtípusok
enum class ElementType {
    None,
    Pipe,            // Egyenes cső
    Bend,            // Kanyar cső
    Tjunction,       // T-elágazás
    Cross,           // Kereszt cső
    PipeValve,       // Egyenes cső csap
    BendValve,       // Kanyar csap
    TjunctionValve,  // T-elágazás csap
    CrossValve,      // Kereszt csap
    Source,          // Forrás
    Sink             // Elnyelő
};



// Cső, csap, forrás és elnyelő adatait tároló struktúra
class GridElement
{
public:
    GridElement(ElementType type = ElementType::Pipe, bool active = false, int x = 0, int y = 0,
                const QVector<bool> &connections = {false, false, false, false},
                const QString &color = "N/A");

    // Getter és setter függvények
    ElementType getType() const;
    bool isActive() const;
    void setActive(bool state);

    bool placePipe(int x, int y, ElementType type);

    int getX() const;
    int getY() const;
    void setPosition(int x, int y);

    QVector<bool> getConnections() const;
    void setConnections(const QVector<bool> &connections);

    QString getColor() const;
    void setColor(const QString &color);

private:
    ElementType type;           // Elem típusa (cső, csap, forrás, elnyelő)
    bool active;                // Aktív vagy inaktív állapot
    int x;                      // X koordináta
    int y;                      // Y koordináta
    QVector<bool> connections;  // Átjárhatóság irányai (É, D, K, Ny)
    QString color;              // Szín (forrás/elnyelő esetén)
};

#endif // GRIDELEMENT_H
