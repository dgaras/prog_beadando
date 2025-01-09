#ifndef GRIDDATA_H
#define GRIDDATA_H

#include <QMap>
#include <QString>
#include <QVector>

// Csőelemek típusai
enum class PipeType {
    cső,
    kanyar,
    Tjunction,
    Cross,
    csőcap,
    kanyarcsap,
    Tjunctioncsap,
    Crosscsap
};

// Forrás és elnyelő adatok tárolására szolgáló struktúra
struct SourceSink {
    bool active;  // Aktív vagy inaktív
    int x;        // X koordináta
    int y;        // Y koordináta
};

// Tároló osztály
class GridData
{
public:
    GridData(int width = 3, int height = 2);  // Alapértelmezett pályaméret

    // Csövek és csapok számainak beállítása
    void setPipeCount(PipeType type, int count);
    void setValveCount(PipeType type, int count);

    int getPipeCount(PipeType type) const;
    int getValveCount(PipeType type) const;

    // Pályaméret beállítása
    void setGridSize(int width, int height);
    int getGridWidth() const;
    int getGridHeight() const;

    // Források és elnyelők kezelése
    void setSource(int index, bool active, int x, int y);
    void setSink(int index, bool active, int x, int y);

    SourceSink getSource(int index) const;
    SourceSink getSink(int index) const;

private:
    int gridWidth;
    int gridHeight;

    QMap<PipeType, int> pipeCounts;
    QMap<PipeType, int> valveCounts;

    QVector<SourceSink> sources;  // Források tárolása
    QVector<SourceSink> sinks;    // Elnyelők tárolása
};

#endif // GRIDDATA_H
