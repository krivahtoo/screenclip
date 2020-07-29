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

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
};


#endif // SCREENCLIP_H
