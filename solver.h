#ifndef SOLVER_H
#define SOLVER_H

#include "griddata.h"
#include "gridelement.h"
#include <QVector>

// Solver osztály a források és elnyelők összekapcsolására
class Solver
{
public:
    // Konstruktor GridData referencia nélkül (alapértelmezett GridData példányt hoz létre)
    Solver(const QList<GridElement> &inputElements, int width, int height, GridData &gridData);

    // Útvonal keresés indítása
    bool solve();

    // Megoldás visszaadása
    QVector<GridElement> getSolution() const;

    // Cső elhelyezése, forgatással és csatlakozásokkal
    bool placePipe(int x, int y, ElementType type, QVector<bool> &connections, int rotation);

    // ElementType konvertálása PipeType-ra
    PipeType elementTypeToPipeType(ElementType type);

private:
    QVector<GridElement> elements;  // Elemeket saját példányként tároljuk (NEM referencia)
    int gridWidth;                  // Pálya szélessége
    int gridHeight;                 // Pálya magassága
    QVector<QVector<GridElement>> grid;  // A pálya rács elemei
    GridData &gridData;             // Hivatkozás a csövek és csapok tárolójára

    QVector<QVector<bool>> visited; // Látogatási mátrix (rekurzív kereséshez)

    // Útvonal keresése adott pozícióból
    bool findPath(int x, int y);

    // Cső elhelyezésének érvényességének ellenőrzése
    bool isValid(int x, int y, const QVector<bool> &connections);

    // Csatlakozások forgatása (pl. kanyar cső esetén)
    QVector<bool> rotateConnections(const QVector<bool> &connections, int rotation);

    // Visszalépés cső eltávolításához (backtracking)
    void backtrack(int x, int y);
};

#endif // SOLVER_H
