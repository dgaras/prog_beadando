#include "GridData.h"

// Konstruktor - pályaméret és forrás/elnyelő inicializálása
GridData::GridData(int width, int height)
    : gridWidth(width), gridHeight(height), sources(5), sinks(5)
{
    // Források és elnyelők alapértelmezése (inaktív, 0,0)
    for (int i = 0; i < 5; ++i) {
        sources[i] = {false, 0, 0};
        sinks[i] = {false, 0, 0};
    }

    // Csövek és csapok alapértelmezett száma 0
    for (auto type : {PipeType::cső, PipeType::kanyar, PipeType::Tjunction, PipeType::Cross,
                      PipeType::csőcap, PipeType::kanyarcsap, PipeType::Tjunctioncsap, PipeType::Crosscsap}) {
        pipeCounts[type] = 0;
        valveCounts[type] = 0;
    }
}

// Forrás beállítása (koordináta validációval)
void GridData::setSource(int index, bool active, int x, int y) {
    if (index >= 0 && index < sources.size()) {
        // Koordináták validálása a pálya méretéhez
        if (x >= 0 && x < gridWidth && y >= 0 && y < gridHeight) {
            sources[index] = {active, x, y};
        }
    }
}

// Elnyelő beállítása (koordináta validációval)
void GridData::setSink(int index, bool active, int x, int y) {
    if (index >= 0 && index < sinks.size()) {
        // Koordináták validálása a pálya méretéhez
        if (x >= 0 && x < gridWidth && y >= 0 && y < gridHeight) {
            sinks[index] = {active, x, y};
        }
    }
}

// Forrás lekérdezése
SourceSink GridData::getSource(int index) const {
    if (index >= 0 && index < sources.size()) {
        return sources[index];
    }
    return {false, 0, 0};
}

// Elnyelő lekérdezése
SourceSink GridData::getSink(int index) const {
    if (index >= 0 && index < sinks.size()) {
        return sinks[index];
    }
    return {false, 0, 0};
}

// Pályaméret beállítása (frissíti a koordinátákat, ha szükséges)
void GridData::setGridSize(int width, int height) {
    gridWidth = width;
    gridHeight = height;

    // Források és elnyelők koordinátáinak ellenőrzése
    for (int i = 0; i < 5; ++i) {
        if (sources[i].x >= gridWidth) {
            sources[i].x = gridWidth - 1;
        }
        if (sources[i].y >= gridHeight) {
            sources[i].y = gridHeight - 1;
        }

        if (sinks[i].x >= gridWidth) {
            sinks[i].x = gridWidth - 1;
        }
        if (sinks[i].y >= gridHeight) {
            sinks[i].y = gridHeight - 1;
        }
    }
}

// Getter - pálya szélesség és magasság
int GridData::getGridWidth() const {
    return gridWidth;
}

int GridData::getGridHeight() const {
    return gridHeight;
}

// Cső és csap mennyiségének beállítása
void GridData::setPipeCount(PipeType type, int count) {
    pipeCounts[type] = count;
}

void GridData::setValveCount(PipeType type, int count) {
    valveCounts[type] = count;
}

// Cső és csap mennyiségének lekérdezése
int GridData::getPipeCount(PipeType type) const {
    return pipeCounts.value(type, 0);
}

int GridData::getValveCount(PipeType type) const {
    return valveCounts.value(type, 0);
}
