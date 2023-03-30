#include "dialog.h"
#include "ui_dialog.h"
#include <QMovie>
#include <QScreen>
#include <QDebug>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);

    /* Set transparent */
    setStyleSheet("background: transparent");
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    /* Set UI position */
    QRect rcScreen = QGuiApplication::primaryScreen()->availableGeometry();

    this->move(rcScreen.width() - this->width(), rcScreen.height() - this->height());

    /* Animated button */
    auto movie = new QMovie(this);

    movie->setFileName(":/rc/calling.gif");
    connect(movie, &QMovie::frameChanged, [=]{
        ui->pushButton->setIcon(movie->currentPixmap());
    });
    movie->start();

    m_sdk.setPrintUnprocessedDataEnabled(false); //to reduce debug output

    connect(&m_sdk, &VideoSDK::stateChanged, [=](State state) {
        m_state = state;

        ui->pushButton->setEnabled(m_state != State::connect);
        if(m_state == State::conference || m_state == State::wait) {
            m_sdk.changeWindowState(WindowState::maximized, true);
        }
        else if(m_state == State::none || m_state == State::connect) {
            //No sense in trying to change window state of app that is disconnected.
            //Just to keep requests queue clearer and reduce debug output.
        }
        else {
            m_sdk.changeWindowState(WindowState::minimized, false);
        }
    });

    /* When Room or VideoSDK have been started on local machine */
    m_room_ip = "127.0.0.1";
    m_room_port = 80;
    m_pin = "123";

    m_sdk.open_session(m_room_ip, m_room_port, m_pin);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_clicked()
{
    if(!m_sdk.started()) {
        m_sdk.open_session(m_room_ip, m_room_port, m_pin);
    }

    m_sdk.call("echotest@trueconf.com");
}
