//
// Created by Raphael Russo on 1/1/25.
//

#include "mainwindow.h"
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QComboBox>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // Create and set up the central OpenGL widget
    m_gardenWidget = new GardenGLWidget(this);
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

    QAction* newAction = fileMenu->addAction(tr("&New Garden"), this, [this]() {

    });

    newAction->setShortcut(QKeySequence::New);



    fileMenu->addSeparator();

    fileMenu->addAction(tr("E&xit"), qApp, &QApplication::closeAllWindows)
            ->setShortcuts(QKeySequence::Quit);

    // View menu
    QMenu* viewMenu = menuBar()->addMenu(tr("&View"));

    // Safety check for dock widgets
    if (m_toolsDock) {
        viewMenu->addAction(m_toolsDock->toggleViewAction());
    }
    if (m_tempDock) {
        viewMenu->addAction(m_tempDock->toggleViewAction());
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


    plantLayout->addStretch();
    m_toolsDock->setWidget(plantPalette);
    addDockWidget(Qt::LeftDockWidgetArea, m_toolsDock);


    plantLayout->addStretch();
    m_toolsDock->setWidget(plantPalette);
    addDockWidget(Qt::LeftDockWidgetArea, m_toolsDock);

    // Create temperature dock widget
    m_tempDock = new QDockWidget(tr("Environment"), this);
    m_tempDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    QWidget* tempWidget = new QWidget(m_tempDock);
    QVBoxLayout* tempLayout = new QVBoxLayout(tempWidget);

    // Add temperature slider
    m_tempLabel = new QLabel(tr("Temperature: 60°F"), tempWidget);
    m_tempSlider = new QSlider(Qt::Horizontal, tempWidget);
    m_tempSlider->setRange(30, 90);  // Temperature range in Fahrenheit
    m_tempSlider->setValue(60);      // Default to 60°F

    handleTemperatureChange(60);

    tempLayout->addWidget(m_tempLabel);
    tempLayout->addWidget(m_tempSlider);

    // Moisture display (for future use)
    QLabel* moistureLabel = new QLabel(tr("Soil Moisture: N/A"), tempWidget);
    tempLayout->addWidget(moistureLabel);

    tempLayout->addStretch();
    m_tempDock->setWidget(tempWidget);
    addDockWidget(Qt::RightDockWidgetArea, m_tempDock);
}

void MainWindow::createToolbar() {
    QToolBar* toolbar = addToolBar(tr("Garden Tools"));
    toolbar->setMovable(false);

    // Common actions
    toolbar->addAction(QIcon(":/icons/new.png"), tr("New Garden"), [this]() {

    });


    toolbar->addSeparator();

    // Add view control actions
    toolbar->addAction(QIcon(":/icons/reset.png"), tr("Reset View"), [this]() {
        // Implement view reset
    });
}

void MainWindow::setupConnections() {
}

void MainWindow::handleTemperatureChange(int value) {
    // Update the temperature label
    m_tempLabel->setText(tr("Temperature: %1°F").arg(value));

    // Force a redraw of the garden scene
    //m_gardenWidget->update();

    // Debug output to verify temperature changes
    qDebug() << "Temperature updated to:" << value << "°F";
}

