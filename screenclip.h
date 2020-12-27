/*
 * Copyright (c) 2020 Noah Too
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef SCREENCLIP_H
#define SCREENCLIP_H


#include <QPixmap>
#include <QWidget>
#include <QSystemTrayIcon>


QT_BEGIN_NAMESPACE
class QCheckBox;
class QAction;
class QGridLayout;
class QGroupBox;
class QHBoxLayout;
class QLabel;
class QMenu;
class QPushButton;
class QSpinBox;
class QVBoxLayout;
QT_END_NAMESPACE

class Screenclip : public QWidget
{
    Q_OBJECT

public:
    Screenclip();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void newScreenshot();
    void saveScreenshot();
    void shootScreen();
    void updateCheckBox();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    void updateScreenshotLabel();
    void createActions();
    void createTrayIcon();

    QPixmap originalPixmap;

    QLabel *screenshotLabel;
    QSpinBox *delaySpinBox;
    QCheckBox *hideThisWindowCheckBox;
    QPushButton *newScreenshotButton;

    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *saveAction;
    QAction *quitAction;
    QAction *autoSave;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
};


#endif // SCREENCLIP_H
