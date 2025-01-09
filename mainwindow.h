#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSpinBox>
#include <QGridLayout>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QGroupBox>
#include "GridData.h"  // Tároló osztály importálása
#include "gridelement.h"  // GridElement használatához
#include "solver.h"        // Solver osztály importálása

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateGridSize();
    void startSolver();

private:
    Ui::MainWindow *ui;
    GridData gridData;  // Pálya és csőadatok tárolása

    // GUI elemek
    QGridLayout *gridLayout;
    QSpinBox *widthSpinBox;
    QSpinBox *heightSpinBox;
    QVBoxLayout *pipeSelectionLayout;
    QVBoxLayout *sourceSinkLayout;
    QPushButton *solveButton;

    // Rács tároló
    QVector<QVector<QLabel*>> gridCells;

    // Pálya adatok (Solverhez szükséges)
    QVector<GridElement> elements;  // Csövek, források és elnyelők tárolása
    int gridWidth;                  // Pálya szélessége
    int gridHeight;                 // Pálya magassága

    // Függvények
    void createGrid(int width, int height);
    void setupPipeSelector();
    void setupSourceSinkSelector();
};

#endif // MAINWINDOW_H
