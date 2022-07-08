#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QAction>
#include <QTcpSocket>
#include <QHostAddress>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // 0, 1, 2
    enum srv_packets {
        srv_Invalid,
        srv_GetMemSize,
        srv_BlastMem
    };

private slots:
    void on_BlastRadiusSlider_valueChanged(int value);
    void on_Action_ConnectToServer();
    void on_Socket_Disconnected();
    void on_Socket_Error();
    void on_corruptButton_clicked();

    void on_BlastRadiusNumberBox_valueChanged(int arg1);

    void on_BlastRangeEnabled_stateChanged(int arg1);

    void on_AddressRangeStart_valueChanged(int arg1);

    void on_AddressRangeEnd_valueChanged(int arg1);

private:
    Ui::MainWindow *ui;
    int BlastRadius = 0;
    int MemSize = 0;
    int StartingAddress = 0;
    int EndingAddress = 0;

    bool BlastByRange = false;
    int AddressRangeStart = 0;
    int AddressRangeEnd = 0;
    QTcpSocket* socket;

    void toggle_ui_elements(bool visible);
    void UpdateBlastRadius(int val);
    char* generate_random(int memSize, int blastRadius);
};
#endif // MAINWINDOW_H
