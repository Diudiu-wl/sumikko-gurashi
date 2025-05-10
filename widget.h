#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QLabel>

#include <QLocalServer>
#include <QLocalSocket>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(int instanceCount, QWidget *parent = nullptr);
    ~Widget();

protected:
    void enterEvent(QEnterEvent *event) override {
        Q_UNUSED(event);
        setCursor(Qt::PointingHandCursor);

    }

    void contextMenuEvent(QContextMenuEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    int size = 70;
    int instanceId = 0;

    QLabel* body;
    QLabel* eyes;

    QScreen* screen;
    QRect screenRect;

    QList<int> offset_right = {0, 0, 0};
    QList<int> offset_left = {0, 0, 0};

    //鼠标事件相关
    QPoint m_dragStartPosition;
    bool enabled = true;
    bool firstTime = true;
    bool isOnOther = false;

    //wink

    int i = 0;
    int stopwink = 0;
    int wink = 0;
    int random = 60;
    bool notWink = true;

    //move
    int i1 = 0;
    int count = 0;
    int countRight = 0;
    int countLeft = 0;
    int countStop = 0;
    int randomRight = 70;
    int randomLeft = 70;
    int randomStop = 50;
    bool right = true;
    bool stop = true;
    bool low = false;
    void down();
    int randomdown = 70;
    bool set = false;
    bool canMove = true;


    QAction* action1;
    QAction* action2;

    void showContextMenu(const QPoint &pos);

    void setupServer1();
    void setupServer2();
    void setupSocket1();
    void setupSocket2();

    void checkPosi();
    void checkPositions();


private:
    //:/images/e0.png
    QPixmap e0 = QPixmap(":/images/e0.png");
    QPixmap e1 = QPixmap(":/images/e1.png");
    QPixmap e2 = QPixmap(":/images/e2.png");
    QPixmap e3 = QPixmap(":/images/e3.png");

    QList<QPixmap> ms0 = {e0, e1, e2, e3, e3, e2, e1};

    QPixmap w0 = QPixmap(":/images/w0.png");
    QPixmap w1 = QPixmap(":/images/w1.png");
    QPixmap w2 = QPixmap(":/images/w2.png");
    QPixmap w3 = QPixmap(":/images/w3.png");
    QPixmap w4 = QPixmap(":/images/w4.png");

    QList<QPixmap> body0 = {w0, w1, w2, w1};

    QPixmap ke0 = QPixmap(":/images/ke0.png");
    QPixmap ke1 = QPixmap(":/images/ke1.png");
    QPixmap ke2 = QPixmap(":/images/ke2.png");
    QPixmap ke3 = QPixmap(":/images/ke3.png");

    QList<QPixmap> ms1 = {ke0, ke1, ke2, ke3, ke3, ke2, ke1};

    QPixmap k0 = QPixmap(":/images/k0.png");
    QPixmap k1 = QPixmap(":/images/k1.png");
    QPixmap k2 = QPixmap(":/images/k2.png");
    QPixmap k3 = QPixmap(":/images/k3.png");
    QPixmap k4 = QPixmap(":/images/k4.png");

    QList<QPixmap> body1 = {k0, k1, k2, k1};

    QPixmap ge0 = QPixmap(":/images/ge0.png");
    QPixmap ge1 = QPixmap(":/images/ge1.png");
    QPixmap ge2 = QPixmap(":/images/ge2.png");
    QPixmap ge3 = QPixmap(":/images/ge3.png");

    QList<QPixmap> ms2 = {ge0, ge1, ge2, ge3, ge3, ge2, ge1};

    QPixmap g0 = QPixmap(":/images/g0.png");
    QPixmap g1 = QPixmap(":/images/g1.png");
    QPixmap g2 = QPixmap(":/images/g2.png");
    QPixmap g3 = QPixmap(":/images/g3.png");
    QPixmap g4 = QPixmap(":/images/g4.png");

    QList<QPixmap> body2 = {g0, g1, g2, g1};

    QList<QList<QPixmap>> mss = {ms0, ms1, ms2};
    QList<QList<QPixmap>> bodys = {body0, body1, body2};

    QList<QPixmap> crouch = {w3, k3, g3};

    QList<QPixmap> maps = {w4, k4, g4};

private slots:
    void sendPosition1();
    void onReadyRead1();

    void sendPosition2();
    void onReadyRead2();


private:
    QLocalServer *server;
    QLocalSocket *socket_server;

    QPoint posi;

    //
    QPoint posi1 = QPoint(-1, -1);
    QPoint posi2 = QPoint(-1, -1);

    QPoint posiToSend1 = QPoint(-1, -1);//发给server的
    QPoint posiToSend2 = QPoint(-1, -1);//发给socket的

    bool BCtogether = false;
    bool ACtogether = false;


    int countBC = 0;
    int countAC = 0;

    QLocalSocket *socket;

    //
    bool server1Closed = false;

};
#endif // WIDGET_H
