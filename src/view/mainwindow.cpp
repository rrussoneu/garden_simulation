//
// Created by Raphael Russo on 1/1/25.
//

#include "mainwindow.h"
#include "plantdragbutton.h"
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QComboBox>
#include <QPushButton>
#include <QGroupBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // Create and set up the central OpenGL widget

    m_controller = std::make_unique<GardenController>(std::make_unique<GardenModel>(10), this);

    m_gardenWidget = new GardenGLWidget(m_controller.get(), this);
    setCentralWidget(m_gardenWidget);


    createDockWindows();
    createMenus();
    createToolbar();
    setupConnections();

    setWindowTitle("Garden Simulation");
    setMinimumSize(800, 600);

    statusBar()->showMessage("Ready");


}

void MainWindow::createMenus() {
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));



    QAction* saveAction = fileMenu->addAction(tr("&Save Garden..."), this, &MainWindow::handleSaveGarden);
    saveAction->setShortcut(QKeySequence::Save);

    QAction* loadAction = fileMenu->addAction(tr("&Load Garden..."), this, &MainWindow::handleLoadGarden);
    loadAction->setShortcut(QKeySequence::Open);

    fileMenu->addSeparator();
    fileMenu->addAction(tr("E&xit"), qApp, &QApplication::closeAllWindows)
            ->setShortcuts(QKeySequence::Quit);

    fileMenu->addSeparator();

    fileMenu->addAction(tr("E&xit"), qApp, &QApplication::closeAllWindows)
            ->setShortcuts(QKeySequence::Quit);

    // View menu
    QMenu* viewMenu = menuBar()->addMenu(tr("&View"));

    // Safety check for dock widgets
    if (m_toolsDock) {
        viewMenu->addAction(m_toolsDock->toggleViewAction());
    }
    if (m_environmentDock) {
        viewMenu->addAction(m_environmentDock->toggleViewAction());
    }

    // Help menu
    QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr("&About"), this, [this]() {
        QMessageBox::about(this, tr("About Garden Simulator"),
                           tr("Garden Simulation :)\n\n"
                              "A 3D Garden Visualizer"));
    });

}

void MainWindow::createDockWindows() {
    // Create plants dock widget
    m_toolsDock = new QDockWidget(tr("Plants"), this);
    m_toolsDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    QWidget* plantPalette = new QWidget(m_toolsDock);
    QVBoxLayout* plantLayout = new QVBoxLayout(plantPalette);

    // Create plant buttons
    auto addPlantButton = [&](Plant::Type type, const QString& name) {
        PlantDragButton* button = new PlantDragButton(type, name, plantPalette);
        button->setMinimumSize(64, 64);
        plantLayout->addWidget(button);
    };

    addPlantButton(Plant::Type::Carrot, tr("Carrot"));
    addPlantButton(Plant::Type::Pumpkin, tr("Pumpkin"));
    addPlantButton(Plant::Type::Tomato, tr("Tomato"));

    // Add spacer at the bottom
    plantLayout->addStretch();

    m_toolsDock->setWidget(plantPalette);
    addDockWidget(Qt::LeftDockWidgetArea, m_toolsDock);

    // Create temperature dock widget
    m_environmentDock = new QDockWidget(tr("Environment"), this);
    m_environmentDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    QWidget* envWidget = new QWidget(m_environmentDock);
    QVBoxLayout* envLayout = new QVBoxLayout(envWidget);
    QGroupBox* tempGroup = new QGroupBox(tr("Temperature"), envWidget);
    QVBoxLayout* tempLayout = new QVBoxLayout(tempGroup);

    // Add temperature slider
    m_tempLabel = new QLabel(tr("Temperature: 60°F"), envWidget);
    m_tempSlider = new QSlider(Qt::Horizontal, envWidget);
    m_tempSlider->setRange(30, 90);  // Temperature range in Fahrenheit
    m_tempSlider->setValue(60);      // Default to 60°F
    m_tempSensorCheck = new QCheckBox(tr("Use Temperature Sensor"));
    handleTemperatureChange(60);

    envLayout->addWidget(m_tempLabel);
    envLayout->addWidget(m_tempSlider);
    tempLayout->addWidget(m_tempSensorCheck);


    // Moisture controls
    QGroupBox* moistureGroup = new QGroupBox(tr("Soil Moisture"), envWidget);
    QVBoxLayout* moistureLayout = new QVBoxLayout(moistureGroup);

    m_moistureLabel = new QLabel(tr("Moisture: 50%"));
    m_moistureSlider = new QSlider(Qt::Horizontal);
    m_moistureSlider->setRange(0, 100);
    m_moistureSlider->setValue(50);
    m_moistureSensorCheck = new QCheckBox(tr("Use Moisture Sensor"));

    moistureLayout->addWidget(m_moistureLabel);
    moistureLayout->addWidget(m_moistureSlider);
    moistureLayout->addWidget(m_moistureSensorCheck);

    envLayout->addWidget(tempGroup);
    envLayout->addWidget(moistureGroup);
    envLayout->addStretch();

    m_environmentDock->setWidget(envWidget);
    addDockWidget(Qt::RightDockWidgetArea, m_environmentDock);
}

void MainWindow::createToolbar() {
    QToolBar* toolbar = addToolBar(tr("Garden Tools"));
    toolbar->setMovable(false);


    // Create Save Garden action
    QAction* saveAction = toolbar->addAction(QIcon(":/icons/save.png"), tr("Save Garden"),
                                             this, &MainWindow::handleSaveGarden);
    saveAction->setStatusTip(tr("Save the current garden to a file"));

    // Create Load Garden action
    QAction* loadAction = toolbar->addAction(QIcon(":/icons/load.png"), tr("Load Garden"),
                                             this, &MainWindow::handleLoadGarden);
    loadAction->setStatusTip(tr("Load a garden from a file"));

    toolbar->addSeparator();

}

void MainWindow::setupConnections() {
    // Environment condition connections
    connect(m_tempSlider, &QSlider::valueChanged, this, &MainWindow::handleTemperatureChange);
    connect(m_tempSensorCheck, &QCheckBox::toggled, this, &MainWindow::handleTemperatureSensorToggle);

    connect(m_moistureSlider, &QSlider::valueChanged, this, &MainWindow::handleMoistureChange);
    connect(m_moistureSensorCheck, &QCheckBox::toggled, this, &MainWindow::handleMoistureSensorToggle);

    // Connect controller signals to update UI
    connect(m_controller.get(), &GardenController::temperatureChanged,
            this, [this](float temp) {
                m_tempLabel->setText(tr("Temperature: %1°F").arg(temp));
                if (!m_tempSensorCheck->isChecked()) {
                    m_tempSlider->setValue(static_cast<int>(temp));
                }
            });

    connect(m_controller.get(), &GardenController::moistureChanged,
            this, [this](float moisture) {
                m_moistureLabel->setText(tr("Moisture: %1%").arg(moisture * 100));
                if (!m_moistureSensorCheck->isChecked()) {
                    m_moistureSlider->setValue(static_cast<int>(moisture * 100));
                }
            });

    connect(m_controller.get(), &GardenController::gardenSaved,
            this, [this]() {
            });

    connect(m_controller.get(), &GardenController::gardenLoaded,
            this, [this]() {
                // Update UI elements after loading
                float temp = m_controller->getModel()->getCurrentTemperature();
                float moisture = m_controller->getModel()->getCurrentMoisture();

                // Update temperature controls
                m_tempSlider->setValue(static_cast<int>(temp));
                m_tempLabel->setText(tr("Temperature: %1°F").arg(temp));

                // Update moisture controls
                m_moistureSlider->setValue(static_cast<int>(moisture * 100));
                m_moistureLabel->setText(tr("Moisture: %1%").arg(moisture * 100));
            });
}

void MainWindow::handleTemperatureChange(int value) {
    // Update the temperature label
    float temperature = static_cast<float>(value);
    m_tempLabel->setText(tr("Temperature: %1°F").arg(temperature));

    //emit temperatureChanged(temperature);
    // Force a redraw of the garden scene
    //m_gardenWidget->update();

    m_controller->setTemperature(temperature);

    // Debug output to verify temperature changes
    qDebug() << "Temperature updated to:" << value << "°F";
}


void MainWindow::handleMoistureChange(int value) {
    if (!m_moistureSensorCheck->isChecked()) {
        m_controller->setMoisture(static_cast<float>(value) / 100.0f);
    }
}

void MainWindow::handleTemperatureSensorToggle(bool enabled) {
    m_tempSlider->setEnabled(!enabled);
    m_controller->toggleTemperatureSensor(enabled);
}

void MainWindow::handleMoistureSensorToggle(bool enabled) {
    m_moistureSlider->setEnabled(!enabled);
    m_controller->toggleMoistureSensor(enabled);
}


void MainWindow::handleSaveGarden() {
    QString filename = QFileDialog::getSaveFileName(
            this,
            tr("Save Garden"),
            QString(),
            tr("Garden Files (*.garden);;All Files (*.*)")
    );

    if (!filename.isEmpty()) {
        if (!filename.endsWith(".garden")) {
            filename += ".garden";
        }

        if (m_controller->saveGarden(filename)) {
            statusBar()->showMessage(tr("Garden saved successfully"), 3000);
        } else {
            QMessageBox::warning(
                    this,
                    tr("Save Garden"),
                    tr("Failed to save garden to %1").arg(filename)
            );
        }
    }
}

void MainWindow::handleLoadGarden() {
    QString filename = QFileDialog::getOpenFileName(
            this,
            tr("Load Garden"),
            QString(),
            tr("Garden Files (*.garden);;All Files (*.*)")
    );

    if (!filename.isEmpty()) {
        if (m_controller->loadGarden(filename)) {
            statusBar()->showMessage(tr("Garden loaded successfully"), 3000);
        } else {
            QMessageBox::warning(
                    this,
                    tr("Load Garden"),
                    tr("Failed to load garden from %1").arg(filename)
            );
        }
    }
}