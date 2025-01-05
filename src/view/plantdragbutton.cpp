//
// Created by Raphael Russo on 1/4/25.
//

#include "plantdragbutton.h"
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QPixmap>
#include <QPainter>
#include <QApplication>
PlantDragButton::PlantDragButton(Plant::Type type, const QString& text, QWidget* parent)
        : QPushButton(text, parent)
        , m_plantType(type)
{
    setStyleSheet(
            "PlantDragButton {"
            "    border: 1px solid #999;"
            "    border-radius: 4px;"
            "    padding: 8px;"
            "    background-color: #f0f0f0;"
            "    min-width: 100px;"
            "}"
            "PlantDragButton:hover {"
            "    background-color: #e0e0e0;"
            "    cursor: grab;"
            "}"
    );
}

void PlantDragButton::mouseMoveEvent(QMouseEvent* event) {
    if (!(event->buttons() & Qt::LeftButton)) {
        return;
    }

    if ((event->pos() - m_dragStartPosition).manhattanLength()
        < QApplication::startDragDistance()) {
        return;
    }

    QDrag* drag = new QDrag(this);
    QMimeData* mimeData = new QMimeData;

    mimeData->setData("application/x-plant",
                      QByteArray::number(static_cast<int>(m_plantType)));
    drag->setMimeData(mimeData);

    // Simple text preview for drag operation
    QPixmap pixmap(100, 30);
    pixmap.fill(Qt::white);
    QPainter painter(&pixmap);
    painter.setPen(Qt::black);
    painter.drawText(pixmap.rect(), Qt::AlignCenter, text());
    painter.end();

    drag->setPixmap(pixmap);
    drag->setHotSpot(QPoint(pixmap.width()/2, pixmap.height()/2));

    drag->exec(Qt::CopyAction);
}