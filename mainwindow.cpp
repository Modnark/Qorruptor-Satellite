#include "mainwindow.h"
#include "./ui_mainwindow.h"

static inline QByteArray IntToArray(qint32 source);

// Constructor
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    this->setFixedSize(QSize(530, 375));
    ui->setupUi(this);
    ui->statusBar->showMessage(QString("Not connected to server"));
    ui->menuFile->addAction("Connect To Server", this, SLOT(on_Action_ConnectToServer()));

    // Toggle the state of UI elements
    toggle_ui_elements(false);
}

// Destructor
MainWindow::~MainWindow()
{
    if(socket->isOpen())
        socket->close();
    delete ui;
}

// Toggles whether or not UI elements are enabled
void MainWindow::toggle_ui_elements(bool visible) {
    ui->corruptButton->setEnabled(visible);
    ui->BlastRadiusSlider->setEnabled(visible);
    ui->BlastRadiusNumberBox->setEnabled(visible);

    if(this->BlastByRange == true) {
        ui->AddressRangeStart->setEnabled(visible);
        ui->AddressRangeEnd->setEnabled(visible);
    }

    ui->BlastRangeEnabled->setEnabled(visible);

    // Temporary limitation
    int TempMemSize = this->MemSize;
    if(TempMemSize > 1000)
        TempMemSize = 1000;

    ui->BlastRadiusSlider->setMaximum(TempMemSize);
    ui->BlastRadiusNumberBox->setMaximum(TempMemSize);
}

// Change the values of the blast radius
void MainWindow::on_BlastRadiusSlider_valueChanged(int value)
{
    this->BlastRadius = value;
    ui->BlastRadiusNumberBox->setValue(value);
}

// Connects to the server
void MainWindow::on_Action_ConnectToServer() {
    ui->statusBar->showMessage(QString("Connecting to server..."));

    // Setup the socket
    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::disconnected, this, &MainWindow::on_Socket_Disconnected);
    connect(socket, &QAbstractSocket::errorOccurred, this, &MainWindow::on_Socket_Error);

    socket->connectToHost(QHostAddress::LocalHost, 59891);

    if(socket->waitForConnected()) {
        ui->statusBar->showMessage(QString("Getting memsize..."));
        socket->write("1");
        socket->waitForBytesWritten();
        socket->waitForReadyRead(1000);
        this->MemSize = socket->readAll().toInt();
        ui->AddressRangeStart->setRange(0, this->MemSize);
        ui->AddressRangeEnd->setRange(0, this->MemSize);

        ui->statusBar->showMessage(QString("Connected to server"));
        toggle_ui_elements(true);
    } else {
        QMessageBox::critical(this, "Qorruptor Satellite", QString("Failed to connect to server because %1").arg(socket->errorString()));
        ui->statusBar->showMessage(socket->errorString());
        if(socket->isOpen())
            socket->close();
    }
}

// Handles disconnection signals
void MainWindow::on_Socket_Disconnected() {
    socket->deleteLater();
    socket=nullptr;

    QMessageBox::information(this, "Qorruptor Satellite", QString("Connection to server was lost."));
    toggle_ui_elements(false);

    ui->statusBar->showMessage(QString("Not connected to server"));
}

// TODO: handle socket errors
void MainWindow::on_Socket_Error() {

}

// Generates the blast packet to send to the server
char* MainWindow::generate_random(int memSize, int blastRadius) {
    srand(time(NULL));
    char* buffer = new char[5005];
    unsigned int blast_repeated = 0;
    buffer[0] = 0x33; //blastman
    *(int*)&buffer[1] = blastRadius; //how many times to repeat the corruption

    for (int i=5; blast_repeated != blastRadius; i += 5){
        if(this->BlastByRange)
            *(int*)&buffer[i] = rand() % (this->AddressRangeEnd - this->AddressRangeStart) + this->AddressRangeStart;
        else
            *(int*)&buffer[i] = rand() % memSize;
        buffer[i+4] = rand() % 0xFF; //byte
        ++blast_repeated;
    }

    return buffer;
}

// Send the corruption packet
void MainWindow::on_corruptButton_clicked()
{
    if(socket->state() == QAbstractSocket::ConnectedState) {
        // Connected
        char* packed = generate_random(this->MemSize, this->BlastRadius);

        socket->write(packed, (this->BlastRadius*5)+5);
        socket->waitForBytesWritten();

        delete[] packed;
    } else {
        // Not connected
        QMessageBox::critical(this, "Qorruptor Satellite", QString("You are not connected to the server."));
    }
}

void MainWindow::on_BlastRadiusNumberBox_valueChanged(int value)
{
    this->BlastRadius = value;
    ui->BlastRadiusSlider->setValue(value);
}


// Toggle blast range on or off
void MainWindow::on_BlastRangeEnabled_stateChanged(int value)
{
    this->BlastByRange = (bool)value;
    ui->AddressRangeStart->setEnabled(this->BlastByRange);
    ui->AddressRangeEnd->setEnabled(this->BlastByRange);
}

void MainWindow::on_AddressRangeStart_valueChanged(int value)
{
    this->AddressRangeStart = value;
    ui->AddressRangeEnd->setRange(this->AddressRangeStart, this->MemSize);
}


void MainWindow::on_AddressRangeEnd_valueChanged(int value)
{
    this->AddressRangeEnd = value;
}

