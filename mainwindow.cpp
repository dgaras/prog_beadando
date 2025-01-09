#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "solver.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // GUI alapok
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    setCentralWidget(centralWidget);

    // Rács terület
    gridLayout = new QGridLayout();
    mainLayout->addLayout(gridLayout);

    // Pályaméret beállítás
    QHBoxLayout *sizeLayout = new QHBoxLayout();
    widthSpinBox = new QSpinBox();
    heightSpinBox = new QSpinBox();

    widthSpinBox->setRange(3, 8);
    heightSpinBox->setRange(2, 8);

    QLabel *widthLabel = new QLabel("Szélesség:");
    QLabel *heightLabel = new QLabel("Magasság:");

    sizeLayout->addWidget(widthLabel);
    sizeLayout->addWidget(widthSpinBox);
    sizeLayout->addWidget(heightLabel);
    sizeLayout->addWidget(heightSpinBox);

    mainLayout->addLayout(sizeLayout);

    // Csőelemek és csapok
    pipeSelectionLayout = new QVBoxLayout();
    QGroupBox *pipeGroupBox = new QGroupBox("Csőelemek és csapok");
    pipeGroupBox->setLayout(pipeSelectionLayout);
    mainLayout->addWidget(pipeGroupBox);

    setupPipeSelector();

    // Források és elnyelők
    sourceSinkLayout = new QVBoxLayout();
    QGroupBox *sourceSinkGroupBox = new QGroupBox("Források és elnyelők");
    sourceSinkGroupBox->setLayout(sourceSinkLayout);
    mainLayout->addWidget(sourceSinkGroupBox);

    setupSourceSinkSelector();

    // Megoldás gomb
    solveButton = new QPushButton("Megoldás keresése");
    mainLayout->addWidget(solveButton);

    // Események
    connect(widthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateGridSize()));
    connect(heightSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateGridSize()));
    connect(solveButton, &QPushButton::clicked, this, &MainWindow::startSolver);

    createGrid(widthSpinBox->value(), heightSpinBox->value());
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Rács létrehozása
void MainWindow::createGrid(int width, int height)
{
    // Törli az előző rácsot
    QLayoutItem *item;
    while ((item = gridLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    gridCells.clear();
    elements.clear();  // Korábbi elemek törlése
    gridCells.resize(height, QVector<QLabel*>(width, nullptr));

    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            QLabel *cell = new QLabel(this);
            cell->setFrameShape(QFrame::Box);
            cell->setMinimumSize(40, 40);
            cell->setAlignment(Qt::AlignCenter);
            gridLayout->addWidget(cell, row, col);
            gridCells[row][col] = cell;

            // GridElement létrehozása és tárolása
            GridElement element(ElementType::Pipe, false, col, row);
            elements.append(element);
        }
    }
}


void MainWindow::setupPipeSelector()
{
    QMap<PipeType, QString> pipeNames = {
        {PipeType::cső, "Cső"},
        {PipeType::kanyar, "Kanyar"},
        {PipeType::Tjunction, "Tjunction"},
        {PipeType::Cross, "Cross"},
        {PipeType::csőcap, "Csőcap"},
        {PipeType::kanyarcsap, "Kanyarcsap"},
        {PipeType::Tjunctioncsap, "Tjunctioncsap"},
        {PipeType::Crosscsap, "Crosscsap"}
    };

    for (auto type : {PipeType::cső, PipeType::kanyar, PipeType::Tjunction, PipeType::Cross}) {
        QHBoxLayout *rowLayout = new QHBoxLayout();

        // Cső típus címke
        QLabel *pipeLabel = new QLabel(pipeNames[type] + ":");
        QSpinBox *pipeSpinBox = new QSpinBox();
        pipeSpinBox->setRange(0, 10);

        // Csap típus címke
        PipeType valveType = static_cast<PipeType>(static_cast<int>(type) + 4);  // Csap típus
        QLabel *valveLabel = new QLabel(pipeNames[valveType] + ":");
        QSpinBox *valveSpinBox = new QSpinBox();
        valveSpinBox->setRange(0, 10);

        // Értékek tárolása a tárolóban
        connect(pipeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                [this, type](int value) { this->gridData.setPipeCount(type, value); });

        connect(valveSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                [this, valveType](int value) { this->gridData.setValveCount(valveType, value); });

        // Elhelyezés
        rowLayout->addWidget(pipeLabel);
        rowLayout->addWidget(pipeSpinBox);
        rowLayout->addWidget(valveLabel);
        rowLayout->addWidget(valveSpinBox);

        pipeSelectionLayout->addLayout(rowLayout);
    }
}




// Források és elnyelők
void MainWindow::setupSourceSinkSelector()
{
    QGridLayout *grid = new QGridLayout();

    QStringList colors = {"Kék", "Zöld", "Piros", "Sárga", "Lila"};

    for (int i = 0; i < 5; ++i) {
        // Forrás aktív checkbox
        QCheckBox *sourceCheckBox = new QCheckBox(colors[i] + " forrás");
        QSpinBox *sourceX = new QSpinBox();
        QSpinBox *sourceY = new QSpinBox();
        sourceX->setRange(0, gridData.getGridWidth() - 1);
        sourceY->setRange(0, gridData.getGridHeight() - 1);

        // Elnyelő aktív checkbox
        QCheckBox *sinkCheckBox = new QCheckBox(colors[i] + " elnyelő");
        QSpinBox *sinkX = new QSpinBox();
        QSpinBox *sinkY = new QSpinBox();
        sinkX->setRange(0, gridData.getGridWidth() - 1);
        sinkY->setRange(0, gridData.getGridHeight() - 1);

        // Forrás frissítése vagy hozzáadása
        connect(sourceCheckBox, &QCheckBox::toggled, [=](bool checked) {
            gridData.setSource(i, checked, sourceX->value(), sourceY->value());

            // Meglévő forrás frissítése
            bool updated = false;
            for (auto &element : elements) {
                if (element.getX() == sourceX->value() && element.getY() == sourceY->value() &&
                    element.getType() == ElementType::Source) {
                    element.setActive(checked);
                    updated = true;
                    break;
                }
            }

            // Ha nem létezik, akkor új forrást adunk hozzá
            if (!updated && checked) {
                GridElement element(ElementType::Source, true, sourceX->value(), sourceY->value());
                elements.append(element);
            }
        });

        // Elnyelő frissítése vagy hozzáadása
        connect(sinkCheckBox, &QCheckBox::toggled, [=](bool checked) {
            gridData.setSink(i, checked, sinkX->value(), sinkY->value());

            // Meglévő elnyelő frissítése
            bool updated = false;
            for (auto &element : elements) {
                if (element.getX() == sinkX->value() && element.getY() == sinkY->value() &&
                    element.getType() == ElementType::Sink) {
                    element.setActive(checked);
                    updated = true;
                    break;
                }
            }

            // Ha nem létezik, akkor új elnyelőt adunk hozzá
            if (!updated && checked) {
                GridElement element(ElementType::Sink, true, sinkX->value(), sinkY->value());
                elements.append(element);
            }
        });

        // Forrás koordináta frissítése mozgatáskor
        connect(sourceX, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
            gridData.setSource(i, sourceCheckBox->isChecked(), value, sourceY->value());

            // Pozíció frissítése az elements listában
            for (auto &element : elements) {
                if (element.getType() == ElementType::Source && element.isActive()) {
                    element.setPosition(value, sourceY->value());
                }
            }
        });

        connect(sourceY, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
            gridData.setSource(i, sourceCheckBox->isChecked(), sourceX->value(), value);

            // Pozíció frissítése az elements listában
            for (auto &element : elements) {
                if (element.getType() == ElementType::Source && element.isActive()) {
                    element.setPosition(sourceX->value(), value);
                }
            }
        });

        // Elnyelő koordináta frissítése mozgatáskor
        connect(sinkX, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
            gridData.setSink(i, sinkCheckBox->isChecked(), value, sinkY->value());

            // Pozíció frissítése az elements listában
            for (auto &element : elements) {
                if (element.getType() == ElementType::Sink && element.isActive()) {
                    element.setPosition(value, sinkY->value());
                }
            }
        });

        connect(sinkY, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
            gridData.setSink(i, sinkCheckBox->isChecked(), sinkX->value(), value);

            // Pozíció frissítése az elements listában
            for (auto &element : elements) {
                if (element.getType() == ElementType::Sink && element.isActive()) {
                    element.setPosition(sinkX->value(), value);
                }
            }
        });

        // Elhelyezés a layoutban
        grid->addWidget(sourceCheckBox, i, 0);
        grid->addWidget(sourceX, i, 1);
        grid->addWidget(sourceY, i, 2);

        grid->addWidget(sinkCheckBox, i, 3);
        grid->addWidget(sinkX, i, 4);
        grid->addWidget(sinkY, i, 5);
    }
    sourceSinkLayout->addLayout(grid);
}

void MainWindow::updateGridSize()
{
    int width = widthSpinBox->value();
    int height = heightSpinBox->value();
    gridData.setGridSize(width, height);
    createGrid(width, height);
}
void MainWindow::startSolver() {
    gridWidth = widthSpinBox->value();
    gridHeight = heightSpinBox->value();
    createGrid(gridWidth, gridHeight);

    // Források és elnyelők manuális hozzáadása
    elements.clear();
    for (int i = 0; i < 5; ++i) {
        auto source = gridData.getSource(i);
        if (source.active) {
            GridElement sourceElement(ElementType::Source, true, source.x, source.y);
            elements.append(sourceElement);
        }

        auto sink = gridData.getSink(i);
        if (sink.active) {
            GridElement sinkElement(ElementType::Sink, true, sink.x, sink.y);
            elements.append(sinkElement);
        }
    }

    // Csövek aktiválása induláskor
    for (auto &element : elements) {
        if (element.getType() == ElementType::Pipe) {
            element.setActive(true);
        }
    }

    qDebug() << "GridElement lista (frissítve):";
    for (const auto &element : elements) {
        qDebug() << "Elem típus:" << static_cast<int>(element.getType())
                 << " Pozíció: (" << element.getX() << "," << element.getY() << ")"
                 << " Aktív:" << element.isActive();
    }

    Solver solver(elements, gridWidth, gridHeight, gridData);

    bool success = solver.solve();

    if (success) {
        for (const auto &element : solver.getSolution()) {
            int x = element.getX();
            int y = element.getY();
            gridCells[y][x]->setStyleSheet("background-color: green;");
        }
    } else {
        qDebug() << "Nem sikerült megoldást találni.";
    }
}

QVector<GridElement> Solver::getSolution() const {
    QVector<GridElement> solution;

    qDebug() << "Megoldás lekérdezése...";
    for (int y = 0; y < gridHeight; ++y) {
        for (int x = 0; x < gridWidth; ++x) {
            auto element = grid[y][x];
            if (element.isActive()) {
                solution.append(element);
                qDebug() << "Megoldás része: (" << x << "," << y << ") Típus: "
                         << static_cast<int>(element.getType());
            }
        }
    }
    return solution;
}
