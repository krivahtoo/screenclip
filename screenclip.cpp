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

#include "screenclip.h"

#include <QtWidgets>
#include <QtCore>

Screenclip::Screenclip()
    :  screenshotLabel(new QLabel(this))
{
    createActions();
    createTrayIcon();

    screenshotLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    screenshotLabel->setAlignment(Qt::AlignCenter);

    const QRect screenGeometry = screen()->geometry();
    screenshotLabel->setMinimumSize(screenGeometry.width() / 8, screenGeometry.height() / 8);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(screenshotLabel);

    QGroupBox *optionsGroupBox = new QGroupBox(tr("Options"), this);
    delaySpinBox = new QSpinBox(optionsGroupBox);
    delaySpinBox->setSuffix(tr(" s"));
    delaySpinBox->setMaximum(60);

    connect(delaySpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &Screenclip::updateCheckBox);

    hideThisWindowCheckBox = new QCheckBox(tr("Hide This Window"), optionsGroupBox);

    QGridLayout *optionsGroupBoxLayout = new QGridLayout(optionsGroupBox);
    optionsGroupBoxLayout->addWidget(new QLabel(tr("Screenshot Delay:"), this), 0, 0);
    optionsGroupBoxLayout->addWidget(delaySpinBox, 0, 1);
    optionsGroupBoxLayout->addWidget(hideThisWindowCheckBox, 1, 0, 1, 2);

    mainLayout->addWidget(optionsGroupBox);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    newScreenshotButton = new QPushButton(tr("New Screenshot"), this);
    connect(newScreenshotButton, &QPushButton::clicked, this, &Screenclip::newScreenshot);
    buttonsLayout->addWidget(newScreenshotButton);
    QPushButton *saveScreenshotButton = new QPushButton(tr("Save Screenshot"), this);
    connect(saveScreenshotButton, &QPushButton::clicked, this, &Screenclip::saveScreenshot);
    buttonsLayout->addWidget(saveScreenshotButton);
    QPushButton *quitScreenshotButton = new QPushButton(tr("Quit"), this);
    quitScreenshotButton->setShortcut(Qt::CTRL + Qt::Key_Q);
    connect(quitScreenshotButton, &QPushButton::clicked, this, &QWidget::close);
    connect(trayIcon, &QSystemTrayIcon::activated, this, &Screenclip::iconActivated);
    buttonsLayout->addWidget(quitScreenshotButton);
    buttonsLayout->addStretch();
    mainLayout->addLayout(buttonsLayout);

    trayIcon->show();

    shootScreen();
    delaySpinBox->setValue(5);

    setWindowTitle(tr("Screenclip"));
    resize(300, 200);
}


void Screenclip::resizeEvent(QResizeEvent * /* event */)
{
    QSize scaledSize = originalPixmap.size();
    scaledSize.scale(screenshotLabel->size(), Qt::KeepAspectRatio);
    if (scaledSize != screenshotLabel->pixmap(Qt::ReturnByValue).size())
        updateScreenshotLabel();
}


void Screenclip::closeEvent(QCloseEvent *event)
{
#ifdef Q_OS_MACOS
    if (!event->spontaneous() || !isVisible()) {
        return;
    }
#endif
    if (trayIcon->isVisible()) {
        QMessageBox::information(this, tr("Screenclip"),
                                 tr("The program will keep running in the "
                                    "system tray. To terminate the program, "
                                    "choose <b>Quit</b> in the context menu "
                                    "of the system tray entry."));
        hide();
        event->ignore();
    }
}


void Screenclip::newScreenshot()
{
    if (hideThisWindowCheckBox->isChecked())
        hide();
    newScreenshotButton->setDisabled(true);

    QTimer::singleShot(delaySpinBox->value() * 1000, this, &Screenclip::shootScreen);
}


void Screenclip::saveScreenshot()
{
    QDateTime currentTime = QDateTime::currentDateTime();
    const QString dateFormat = "yyyyMMdd_hhmmss";
    QString name = currentTime.toString(dateFormat);
    const QString format = "png";
    QString initialPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    if (initialPath.isEmpty())
        initialPath = QDir::currentPath();
    initialPath += "/Screenshot_"+ name + "." + format;
    
    QString fileName;
    
    if (!autoSave->isChecked()) {
        QFileDialog fileDialog(this, tr("Save As"), initialPath);
        fileDialog.setAcceptMode(QFileDialog::AcceptSave);
        fileDialog.setFileMode(QFileDialog::AnyFile);
        fileDialog.setDirectory(initialPath);
        QStringList mimeTypes;
        const QList<QByteArray> baMimeTypes = QImageWriter::supportedMimeTypes();
        for (const QByteArray &bf : baMimeTypes)
            mimeTypes.append(QLatin1String(bf));
        fileDialog.setMimeTypeFilters(mimeTypes);
        fileDialog.selectMimeTypeFilter("image/" + format);
        fileDialog.setDefaultSuffix(format);
        if (fileDialog.exec() != QDialog::Accepted)
            return;
        fileName = fileDialog.selectedFiles().first();
    } else {
        fileName = initialPath;
    }

    if (!originalPixmap.save(fileName)) {
        QMessageBox::warning(this, tr("Save Error"), tr("The image could not be saved to \"%1\".")
                             .arg(QDir::toNativeSeparators(fileName)));
    }
}


void Screenclip::shootScreen()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (const QWindow *window = windowHandle())
        screen = window->screen();
    if (!screen)
        return;

    if (delaySpinBox->value() != 0)
        QApplication::beep();

    originalPixmap = screen->grabWindow(0);
    updateScreenshotLabel();

    newScreenshotButton->setDisabled(false);
    if (hideThisWindowCheckBox->isChecked())
        show();
}


void Screenclip::updateCheckBox()
{
    if (delaySpinBox->value() == 0) {
        hideThisWindowCheckBox->setDisabled(true);
        hideThisWindowCheckBox->setChecked(false);
    } else {
        hideThisWindowCheckBox->setDisabled(false);
    }
}

void Screenclip::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        shootScreen();
        break;
    case QSystemTrayIcon::MiddleClick:
        // showMessage();
        // break;
    default:
        ;
    }
}


void Screenclip::updateScreenshotLabel()
{
    screenshotLabel->setPixmap(originalPixmap.scaled(screenshotLabel->size(),
                                                     Qt::KeepAspectRatio,
                                                     Qt::SmoothTransformation));
}

void Screenclip::createActions()
{
    saveAction = new QAction(tr("&Save"), this);
    connect(saveAction, &QAction::triggered, this, &Screenclip::saveScreenshot);

    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, &QAction::triggered, this, &QWidget::hide);

    maximizeAction = new QAction(tr("Ma&ximize"), this);
    connect(maximizeAction, &QAction::triggered, this, &QWidget::showMaximized);

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, &QAction::triggered, this, &QWidget::showNormal);
    
    autoSave = new QAction(tr("Auto Save"), this);
    autoSave->setCheckable(true);

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}


void Screenclip::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(saveAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(autoSave);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(QIcon(":/images/icon.png"), this);
    trayIcon->setContextMenu(trayIconMenu);
}
