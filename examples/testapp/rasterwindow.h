// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
#ifndef RASTERWINDOW_H
#define RASTERWINDOW_H

#include <QtGui>

class RasterWindow : public QRasterWindow
{
public:
    RasterWindow();

    void paintEvent(QPaintEvent * event) override;
    void exposeEvent(QExposeEvent * ev) override;
    void focusInEvent(QFocusEvent * ev) override;
    void focusOutEvent(QFocusEvent * ev) override;
    void hideEvent(QHideEvent * ev) override;
    void keyPressEvent(QKeyEvent * ev) override;
    void keyReleaseEvent(QKeyEvent * ev) override;
    void mouseDoubleClickEvent(QMouseEvent * ev) override;
    void mouseMoveEvent(QMouseEvent * ev) override;
    void mousePressEvent(QMouseEvent * ev) override;
    void mouseReleaseEvent(QMouseEvent * ev) override;
    void moveEvent(QMoveEvent * ev) override;
    void resizeEvent(QResizeEvent * ev) override;
    void showEvent(QShowEvent * ev) override;
    void tabletEvent(QTabletEvent * ev) override;
    void touchEvent(QTouchEvent * ev) override;
    void wheelEvent(QWheelEvent * ev) override;
private:
    void incrementEventCount();
    int m_eventCount;
    int m_timeoutCount;
    int m_frameCount;
    int m_fps;
    QPoint m_offset;
    QPoint m_lastPos;
    bool m_pressed;
};

#endif // RASTERWINDOW_H
