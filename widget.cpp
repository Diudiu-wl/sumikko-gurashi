#include "widget.h"
#include <QMenu>
#include <QContextMenuEvent>
#include <QRandomGenerator>
#include <QWindow>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>


Widget::Widget(int instanceCount, QWidget *parent) : QWidget(parent),instanceId(instanceCount) {
    this->resize(size, size);
    this->setWindowTitle("sumikko");

    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_Hover);



    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QWidget::customContextMenuRequested, this, &Widget::showContextMenu);
    action1 = new QAction("停止移动", this);//可以移动
    action2 = new QAction("关闭", this);


    connect(action1, &QAction::triggered, this, [=]() {
        if(canMove) {
            action1->setText("可以移动");
        }
        else {
            action1->setText("停止移动");
        }
        canMove = !canMove;

    });

    connect(action2, &QAction::triggered, this, [=]() {

        close();

    });


    body = new QLabel(this);
    body->setPixmap(bodys[instanceId].at(1));
    body->setScaledContents(true);
    body->setFixedSize(QSize(size, size));
    body->move(0, 0);


    eyes = new QLabel(this);
    eyes->setPixmap(mss[instanceId].at(0));
    eyes->setScaledContents(true);
    eyes->setFixedSize(QSize(size, size));
    eyes->move(0, 0);

/*
    QLabel* name = new QLabel(this);
    if(instanceId == 0) {
        name->setText("A");
    }
    else if(instanceId == 1) {
        name->setText("B");
    }
    else {
        name->setText("C");
    }

    name->resize(21, 21);
    name->setStyleSheet("color: black; font-size: 20px; font-weight: bold;");
    name->move(0, 0);
*/


    setMouseTracking(true);

    auto wid = this->winId();
    QWindow* window = this->windowHandle();
    if (!window) {
        qWarning("Failed to get the QWindow from QWidget");
        return;
    }

    screen = window->screen();



    if (!screen) {
        qWarning("Failed to get the screen for the QWindow");
        return;
    }

    screenRect = screen->availableGeometry();


    if(instanceId == 0) {
        setupServer1();
    }

    else if(instanceId == 1){
        setupServer2();
        setupSocket1();
    }
    else if(instanceId == 2){
        setupSocket2();
    }


    //眨眼的动态
    QTimer* t = new QTimer(this);

    connect(t, &QTimer::timeout, this, [=]() {
        if(notWink) {
            stopwink ++;
            eyes->setPixmap(mss[instanceId].at(0));
            if(stopwink <= random) {
                return;
            }
            else {

                stopwink = 0;
                notWink = false;
                random = QRandomGenerator::global()->bounded(55, 100);

            }

        }
        wink ++;
        if(wink <= mss[instanceId].size()) {
            i = (i + 1) % mss[instanceId].size();
            eyes->setPixmap(mss[instanceId].at(i));
        }
        else {
            wink = 0;
            notWink = true;
        }

    });
    t->start(30); //25，30


    //走路的动态

    QTimer* t_move = new QTimer(this);

    connect(t_move, &QTimer::timeout, this, [=]() {

        if(!canMove) {
            body->setPixmap(maps[instanceId]);
            return;
        }

        if(isOnOther) {
            body->setPixmap(crouch[instanceId]);
            return;
        }

        if(!enabled) {
            if(low) {
                body->setPixmap(crouch[instanceId]);
            }
            else {
                body->setPixmap(bodys[instanceId].at(1));
            }

            return;
        }


        if(stop) {

            if(!set) {
                randomdown = QRandomGenerator::global()->bounded(1, 100);

                set = true;
            }

            //int randomdown = QRandomGenerator::global()->bounded(1, 100);

            if(randomdown % 2 == 0) {
                down();
            }
            else {
                body->setPixmap(bodys[instanceId].at(1));
            }

            countStop ++;

            if(countStop <= randomStop) {
                return;
            }

            else {

                countStop = 0;
                stop = false;

                body->setPixmap(bodys[instanceId].at(1));
                randomStop = QRandomGenerator::global()->bounded(60, 200);
            }


        }


        if(low) {
            move(pos().x(), pos().y() - 5);
            low = false;
        }

        count = (count + 1) % 3;

        if(right) {

            if(count == 0) {
                i1 = (i1 + 1) % bodys[instanceId].size();
                body->setPixmap(bodys[instanceId].at(i1));

            }

            if (pos().x() + 1 + size >= screenRect.right() + offset_right[instanceId]) {


                right = false;
                //set = false;
                //stop = true;
                firstTime = true;
                randomRight = QRandomGenerator::global()->bounded(50, 80);
                countRight = 0;

                return;
            }

            if(instanceId == 0) {
                if(i1 == 2) move(pos().x() + 1, pos().y() + 1);
                else if(i1 == 3) move(pos().x() + 1, pos().y() - 1);

                else move(pos().x() + 1, pos().y());
            }

            else if(instanceId == 1) {
                if(i1 == 2) move(pos().x() + 1, pos().y() - 1);
                else if(i1 == 3) move(pos().x() + 1, pos().y() + 1);

                else move(pos().x() + 1, pos().y());
            }

            else {
                move(pos().x() + 1, pos().y());
            }

            countRight ++;
            if(countRight >= randomRight && i1 == 3) {
                right = false;
                set = false;
                stop = true;
                firstTime = true;
                randomRight = QRandomGenerator::global()->bounded(50, 80);
                countRight = 0;

            }

        }

        else {

            if(count == 0) {
                i1 = (i1 + 1) % bodys[instanceId].size();
                body->setPixmap(bodys[instanceId].at(i1));

            }

            if (pos().x() - 1 < screenRect.left() + offset_left[instanceId]) {

                //set = false;
                //stop = true;
                firstTime = true;
                right = true;
                countLeft = 0;
                randomLeft = QRandomGenerator::global()->bounded(50, 80);
                return;
            }

            if(instanceId == 0) {
                if(i1 == 2) move(pos().x() - 1, pos().y() + 1);
                else if(i1 == 3) move(pos().x() - 1, pos().y() - 1);

                else move(pos().x() - 1, pos().y());
            }

            else if(instanceId == 1) {
                if(i1 == 2) move(pos().x() - 1, pos().y() - 1);
                else if(i1 == 3) move(pos().x() - 1, pos().y() + 1);

                else move(pos().x() - 1, pos().y());
            }

            else {
                move(pos().x() - 1, pos().y());
            }

            countLeft ++;
            if(countLeft >= randomLeft && i1 == 3) {
                set = false;
                stop = true;
                firstTime = true;
                right = true;
                countLeft = 0;
                randomLeft = QRandomGenerator::global()->bounded(50, 80);
            }
        }
    });

    t_move->start(70);//50


}

void Widget::mousePressEvent(QMouseEvent *event) {

    if (event->button() == Qt::LeftButton) {

        m_dragStartPosition = event->pos();
        firstTime = false;
        enabled = false;

    }

}

void Widget::mouseMoveEvent(QMouseEvent *event) {

    if (event->buttons() & Qt::LeftButton) {
        QPoint delta = event->pos() - m_dragStartPosition;
        move(pos() + delta);
    }
}

void Widget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isOnOther = false;
        enabled = true;


    }


}

void Widget::contextMenuEvent(QContextMenuEvent *event) {

    showContextMenu(event->pos());
}

void Widget::showContextMenu(const QPoint &pos) {

    QMenu contextMenu(this);
    contextMenu.setFont(QFont("微软雅黑", 10));
    contextMenu.addAction(action1);
    contextMenu.addAction(action2);


    contextMenu.exec(mapToGlobal(pos));
}

void Widget::setupServer1() {

    server = new QLocalServer(this);

    connect(server, &QLocalServer::newConnection, this, &Widget::sendPosition1);

    QLocalServer::removeServer("wasabi");

    if (!server->listen("wasabi")) {
        QMessageBox::warning(this, "提示", "创建服务器失败");
        close();
        return;
    }
}

void Widget::setupServer2() {

    server = new QLocalServer(this);

    connect(server, &QLocalServer::newConnection, this, &Widget::sendPosition2);

    QLocalServer::removeServer("kome");

    if (!server->listen("kome")) {
        QMessageBox::warning(this, "提示", "创建服务器失败");
        close();
        return;
    }
}

void Widget::sendPosition1() {


    if(server->hasPendingConnections()) {
        socket_server = server->nextPendingConnection();
        connect(socket_server, &QLocalSocket::readyRead, this, &Widget::onReadyRead1);
        connect(socket_server, &QLocalSocket::disconnected, socket_server, &QLocalSocket::deleteLater);


        QDataStream out(socket_server);
        out.setVersion(QDataStream::Qt_5_10);
        QPoint posit = pos();

        out << posit;

        socket_server->flush();


    }

}

void Widget::sendPosition2() {

    if(server->hasPendingConnections()) {
        socket_server = server->nextPendingConnection();
        connect(socket_server, &QLocalSocket::readyRead, this, &Widget::onReadyRead2);
        connect(socket_server, &QLocalSocket::disconnected, socket_server, &QLocalSocket::deleteLater);

        connect(socket_server, &QLocalSocket::errorOccurred, this, [=](QLocalSocket::LocalSocketError error) {
            posi2 = QPoint(-1, -1);
        });

        QDataStream out(socket_server);
        out.setVersion(QDataStream::Qt_5_10);


        out << posiToSend2;

        socket_server->flush();

    }

}

void Widget::onReadyRead1() {


    QDataStream read(socket_server);
    read.setVersion(QDataStream::Qt_5_10);

    read >> posi;

    checkPosi();

}

void Widget::onReadyRead2() {

    QDataStream read(socket_server);
    read.setVersion(QDataStream::Qt_5_10);

    read >> posi2;//来自C的位置

    checkPositions();

}

void Widget::checkPosi() {
    if(posi == QPoint(-1, -1)) return;

    double deltaX = std::abs(pos().x() + size / 2 - (posi.x() + size / 2));
    double deltaY = std::abs(pos().y() + size / 2 - (posi.y() + size / 2));

    double distance = std::sqrt(deltaX * deltaX + deltaY * deltaY); // 总距离

    if(distance <= 65 && enabled) {

        if(pos().y() < posi.y()) {
            move(posi.x(), posi.y() - 40);
            isOnOther = true;

        }

    }

}


Widget::~Widget()
{


    if (server) {
        server->close();
        delete server;
    }

    if (socket) {
        socket->abort();
        delete socket;
    }


    if (socket_server) {
        socket_server->abort();
        delete socket_server;
    }

}


void Widget::setupSocket1() { //1作为socket读取来自server0的位置，并将2个位置传回server0
    //Socket
    socket = new QLocalSocket(this);

    QTimer* timer = new QTimer(this);


    connect(timer, &QTimer::timeout, this, [=]() {

        socket->abort();

        socket->connectToServer("wasabi");

    });

    timer->start(10);


    connect(socket, &QLocalSocket::errorOccurred, this, [=](QLocalSocket::LocalSocketError error) {
        posi1 = QPoint(-1, -1);
    });


    connect(socket, &QLocalSocket::readyRead, this, [=]() {

        QDataStream tempReadDataStream(socket);
        tempReadDataStream.setVersion(QDataStream::Qt_5_10);


        tempReadDataStream >> posi1; //来自A的位置

        checkPositions();


        QDataStream tempWriteDataStream(socket);
        tempWriteDataStream.setVersion(QDataStream::Qt_5_10);



        tempWriteDataStream << posiToSend1;//发给A位置

        socket->flush();



    });
}

void Widget::setupSocket2() {
    //Socket
    socket = new QLocalSocket(this);

    QTimer* timer = new QTimer(this);


    connect(timer, &QTimer::timeout, this, [=]() {
        socket->abort();

        socket->connectToServer("kome");

        if(server1Closed) {
            socket->connectToServer("wasabi");
        }


    });

    timer->start(10);

    connect(socket, &QLocalSocket::errorOccurred, this, [=](QLocalSocket::LocalSocketError error) {

        server1Closed = !server1Closed;

    });

    connect(socket, &QLocalSocket::readyRead, this, [=]() {



        QDataStream tempReadDataStream(socket);
        tempReadDataStream.setVersion(QDataStream::Qt_5_10);


        tempReadDataStream >> posi;


        checkPosi();

        QDataStream tempWriteDataStream(socket);
        tempWriteDataStream.setVersion(QDataStream::Qt_5_10);
        QPoint thisposit = pos();

        tempWriteDataStream << thisposit;

        socket->flush();


    });
}

void Widget::checkPositions() {

    if(posi1 == QPoint(-1, -1) && posi2 == QPoint(-1, -1)) return;

    double deltaXAB = std::abs(pos().x() + size / 2 - (posi1.x() + size / 2));
    double deltaYAB = std::abs(pos().y() + size / 2 - (posi1.y() + size / 2));

    double distanceAB = std::sqrt(deltaXAB * deltaXAB + deltaYAB * deltaYAB); // 总距离

    double deltaXAC = std::abs(posi1.x() + size / 2 - (posi2.x() + size / 2));
    double deltaYAC = std::abs(posi1.y() + size / 2 - (posi2.y() + size / 2));

    double distanceAC = std::sqrt(deltaXAC * deltaXAC + deltaYAC * deltaYAC); // 总距离

    double deltaXBC = std::abs(pos().x() + size / 2 - (posi2.x() + size / 2));
    double deltaYBC = std::abs(pos().y() + size / 2 - (posi2.y() + size / 2));

    double distanceBC = std::sqrt(deltaXBC * deltaXBC + deltaYBC * deltaYBC); // 总距离


    if(posi1 == QPoint(-1, -1)) {//A不存在
        //只判断自己(B)和C
        posiToSend2 = QPoint(pos().x(), pos().y() - 5);
        if(distanceBC <= 65 && enabled) {

            if(pos().y() < posi2.y()) {
                //自己(B)在C的上面
                move(posi2.x(), posi2.y() - 45);
                isOnOther = true;

                return;

            }

            return;

        }
        return;
    }

    if(posi2 == QPoint(-1, -1)) {//C不存在
        //只判断自己(B)和A
        posiToSend1 = QPoint(pos().x(), pos().y() - 5);
        if(distanceAB <= 65 && enabled) {
            if(pos().y() < posi1.y()) {//B在A上面
                //自己(B)在A的上面
                move(posi1.x(), posi1.y() - 45);
                isOnOther = true;

                return;

            }

        }
        return;
    }


    if(!BCtogether && !ACtogether && distanceAB <= 65) {

        if(pos().y() < posi1.y() && enabled) {//B在A上面
            //自己(B)在A的上面
            move(posi1.x(), posi1.y() - 45);
            isOnOther = true;
            posiToSend1 = posi2;//A得到C的位置（A可以判断是否叠上C）
            posiToSend2 = QPoint(pos().x(), pos().y() - 5);//C得到B的位置（C可以判断是否叠上B)
            return;

        }

        else if(pos().y() >= posi1.y()){//A在B的上面
            //A在B的上面, B可以判断是否叠上C, C可以判断是否叠上A
            posiToSend1 = QPoint(pos().x(), pos().y() - 5);//将自己(B)的位置发给A(A判断要叠上B)

            if(distanceBC <= 65) {//BC靠近
                if(pos().y() < posi2.y() && enabled) {//B叠上C
                    move(posi2.x(), posi2.y() - 45);
                    isOnOther = true;
                    posiToSend2 = QPoint(-1, -1);//C头上有两个了，不能叠上任何东西
                    return;

                }
                else if(pos().y() >= posi2.y()){//C叠上B(不行，只能叠上A）
                    posiToSend2 = posi1;//C得到A的位置（C可以判断是否叠上A）
                    return;
                }

            }
            else {//BC不靠近，C得到A的位置（C可以判断是否叠上A）
                posiToSend2 = posi1;
                return;
            }

            return;
        }

        return;

    }



    if(!ACtogether && distanceBC <= 65) {

        BCtogether = true;
        countBC = 0;

        if(pos().y() < posi2.y() && enabled) {
            //自己(B)在C的上面
            move(posi2.x(), posi2.y() - 45);
            isOnOther = true;
            posiToSend1 = QPoint(pos().x(), pos().y() - 5);//A得到B的位置(A判断是否叠上B)
            posiToSend2 = posi1;//C得到A的位置(C可以判断是否叠上A)

            return;

        }

        else if(pos().y() >= posi2.y()){
            //C在B的上面, B可以判断是否叠上A, A可以判断是否叠上C
            posiToSend2 = QPoint(pos().x(), pos().y() - 5); //将自己(B)的位置发给C，使得C可以叠上B


            if(distanceAB <= 65) {//AB靠近
                if(pos().y() < posi1.y() && enabled) {//B叠上A
                    move(posi1.x(), posi1.y() - 45);
                    isOnOther = true;
                    posiToSend1 = QPoint(-1, -1);//A头上有两个了，不能叠上任何东西
                    return;

                }
                else if(pos().y() >= posi1.y()){//A叠上B(不行，只能叠上C）
                    posiToSend1 = posi2;//A得到C的位置（A可以判断是否叠上C）
                    return;
                }

            }
            else {//AB不靠近，A得到C的位置（A可以判断是否叠上C）
                posiToSend1 = posi2;
                return;
            }

        }


        return;

    }

    else if(distanceBC > 65){
        countBC ++;
        if(countBC >= 5) {
            BCtogether = false;
            countBC = 0;
        }

    }


    if(distanceAC <= 65) {//AC靠近

        //posiToSend1 = posi2;//将C的位置发回A
        //posiToSend2 = posi1;//将A的位置发回C

        ACtogether = true;
        countAC = 0;

        if(posi1.y() < posi2.y()) {//A叠在C上面

            posiToSend1 = posi2;//将C的位置发回A

            if(distanceBC <= 65) {//BC靠近

                if(posi2.y() < pos().y()) {//C要叠上B
                    posiToSend2 = QPoint(pos().x(), pos().y() - 5); //C得到B的位置

                    return;//B头上有两个了，不能再叠了
                }
                else {//B要叠上C(但是不行，只能叠上A)
                    //判断AB距离
                    posiToSend2 = QPoint(-1, -1);//C在最下面，不能再叠了
                    if(distanceAB <= 65) {
                        if(pos().y() < posi1.y() && enabled) {//B在A上面
                            move(posi1.x(), posi1.y() - 45);
                            isOnOther = true;
                            //posiToSend1 = QPoint(-1, -1);//A头上有两个了，不能叠上任何东西
                            return;
                        }
                        else {//A在B上面（不行）


                        }
                    }
                    else { //A不靠近B

                    }
                }
            }

            else {//BC不靠近，B可以判断是否叠上A
                posiToSend2 = QPoint(-1, -1);
                //判断AB距离
                if(distanceAB <= 65) {
                    if(pos().y() < posi1.y() && enabled) {//B在A上面
                        move(posi1.x(), posi1.y() - 45);
                        isOnOther = true;
                        //posiToSend1 = QPoint(-1, -1);//A头上有两个了，不能叠上任何东西
                        return;
                    }
                    else {//A在B上面（不行）

                    }
                }
                else {//AB不靠近

                }
            }


        }

        else {//C在A上面


            //将A的位置发给C
            posiToSend2 = posi1;


            if(distanceAB <= 65) { //AB靠近
                if(posi1.y() < pos().y()) { //A在B上
                    posiToSend1 = QPoint(pos().x(), pos().y() - 5); //A获得B的位置
                    return; //B头上有两个了，不能再叠了
                }
                else { //B在A上（但是不行，B只能在C上)

                    //判断BC距离
                    posiToSend1 = QPoint(-1, -1); //A上有两个了，不能再叠了

                    if(distanceBC <= 65) {//BC靠近
                        if(pos().y() < posi2.y() && enabled) { //B在C上
                            move(posi2.x(), posi2.y() - 45);
                            isOnOther = true;

                            //posiToSend1 = QPoint(-1, -1);//A头上有两个了，不能叠上任何东西
                            return;
                        }
                        else {//C在B上(不行)

                        }
                    }
                    else {//BC不靠近

                    }
                }
            }

            else {//AB不靠近， B可以判断是否叠上C
                //判断BC距离

                if(distanceBC <= 65) {//BC靠近

                    if(pos().y() < posi2.y() && enabled) { //B在C上

                        move(posi2.x(), posi2.y() - 45);
                        isOnOther = true;
                        //posiToSend1 = QPoint(-1, -1);//A头上有两个了，不能叠上任何东西
                        return;
                    }
                    else {//C在B上（不行）

                    }
                }
                else {//BC不靠近

                }

            }

        }


        return;

    }

    else if(distanceAC > 65){
        countAC ++;
        if(countAC >= 5) {
            ACtogether = false;
            countAC = 0;
        }

    }



}

void Widget::down() {

    if(firstTime) {
        move(pos().x(), pos().y() + 5);
        firstTime = false;
        low = true;
    }

    body->setPixmap(crouch.at(instanceId));

}
