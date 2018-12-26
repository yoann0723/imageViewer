#include "HS_ImageViewUi.h"
#include <QPainter>
#include <QDesktopWidget>
#include <QApplication>
#include <QDebug>
#include <QPropertyAnimation>
#include <QtConcurrent>

const int duration = 200;

const double zoomIn[] = {
                         0.05,   0.06,   0.07,   0.08,  0.10,  0.12,  0.14,
                         0.17,  0.20,  0.24,  0.29,  0.35,  0.42,  0.50,
                         0.60,  0.72,  0.86, 1.00, 1.20, 1.44, 1.73,
                         2.08, 2.50, 3.00, 3.60, 4.32, 5.18, 6.22,
                         7.46, 8.95,10.74,12.89,15.47,16.00
                     };

const double zoomOut[] = {
                         16.00,15.47,12.89,10.74,8.95,7.46,
                         6.22, 5.18, 4.32, 3.60, 3.00, 2.50, 2.08,
                         1.73, 1.44, 1.20, 1.00,  0.86,  0.72,  0.60,
                         0.50,   0.42,  0.35,  0.29,  20.4,  0.20,  0.17,
                         0.14,   0.12,   0.10,   0.08,   0.07,   0.06,   0.05
                      };

//最小大小
const QSize &minSize = QSize(732,552);

//最小宽度
const int minWidth = minSize.width();

//最小高度
const int minHeight = minSize.height();

HS_ImageViewUi::HS_ImageViewUi(QWidget *parent) :
    QDialog(parent,Qt::FramelessWindowHint | Qt::Dialog)
{
    m_inImage = false;
    m_moved = false;
    m_bPressed = false;
    m_bZoomIn = false;
    this->init();
}

HS_ImageViewUi::~HS_ImageViewUi()
{
    m_thread.stop();
    m_thread.quit();
    m_thread.wait();
}

void HS_ImageViewUi::setPixmap(const QPixmap &pixmap)
{
    m_pixmap = pixmap;
    this->initSize();
    this->update();
}

void HS_ImageViewUi::setImage(const QImage &image)
{
    m_imageSrc = image;
    m_thread.setImage(image);
    this->initSize();
//    double radio = m_rectImage.width() / (double)m_imageSrc.width();
    m_thread.setScale(m_rectImage.size());
    m_thread.start();
}

void HS_ImageViewUi::setImage(const QString &file)
{
    m_imageSrc = QImage(file);
    m_thread.setImage(file);
    this->initSize();
//    double radio = m_rectImage.width() / (double)m_imageSrc.width();
    m_thread.setScale(m_rectImage.size());
    m_thread.start();
    m_nScaleIndex = 17;
}

void HS_ImageViewUi::setPixmaps(const QStringList &files)
{

}

void HS_ImageViewUi::paintEvent(QPaintEvent *event)
{
    QPainter dc(this);
    dc.setRenderHint(QPainter::SmoothPixmapTransform);
    dc.fillRect(this->rect(),QColor(0,0,0,215));
    if(!m_imageTemp.isNull())
    {
        dc.drawImage(m_rectImage,m_imageTemp);
    }
}

void HS_ImageViewUi::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_moved = false;
        m_bPressed = true;
        if(m_rectImage.contains(event->pos())){
            this->setCursor(Qt::ClosedHandCursor);
            m_inImage = true;
            m_point = event->pos() - m_rectImage.topLeft();
        }
        else
            m_inImage = false;
        m_posWidget = event->pos();
    }
}

void HS_ImageViewUi::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if(event->button() == Qt::LeftButton){
        m_bPressed = false;
        m_inImage = false;
        this->setCursor(Qt::ArrowCursor);
        if(!m_moved && !QRect(0,0,this->width(),40).contains(event->pos())){
            this->showCloseAnimation();
        }
    }
}

void HS_ImageViewUi::mouseMoveEvent(QMouseEvent *event)
{
    this->setCursor(QRect(0,0,this->width(),40).contains(event->pos())?(m_bPressed?Qt::ClosedHandCursor:Qt::ArrowCursor):Qt::ClosedHandCursor);

    QPoint pos = event->pos();
    this->setWidgetVisible(pos);

    bool moveH = (m_rectImage.width() > QSizeF(this->size()).width());
    bool moveV = (m_rectImage.height() > QSizeF(this->size()).height());
    QSizeF size = m_rectImage.size();
    if(m_bPressed)
    {
        if(m_inImage){
            QPointF pos = event->localPos() - m_point;
            if(moveV)
                m_rectImage.setY(pos.y());
            else
                m_rectImage.setY(m_rectImage.y());

            if(moveH)
                m_rectImage.setX(pos.x());
            else
                m_rectImage.setX(m_rectImage.x());

            if(!moveH && !moveV){
                QPoint posWidget = event->pos() - m_posWidget + this->pos();
                this->move(posWidget);
            }
        }
        else{
            QPoint posWidget = event->pos() - m_posWidget + this->pos();
            this->move(posWidget);
        }

        m_moved = true;
        m_rectImage.setWidth(size.width());
        m_rectImage.setHeight(size.height());
        this->update();
    }
}

void HS_ImageViewUi::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;

    qDebug() << "numSteps:" << numSteps;

    double radio = zoomIn[m_nScaleIndex];
    if(numSteps > 0){
        if(m_nScaleIndex + 1 > -1 && (m_nScaleIndex + 1) < (sizeof(zoomIn) / sizeof(zoomIn[0]))){
            radio = zoomIn[++m_nScaleIndex];
        }
    }
    else{
        if(m_nScaleIndex - 1 > -1 && (m_nScaleIndex - 1) < (sizeof(zoomIn) / sizeof(zoomIn[0]))){
            radio = zoomIn[--m_nScaleIndex];
        }
    }

    QSizeF size = m_imageSrc.size();
    m_rectImage.setWidth(size.width() * radio);
    m_rectImage.setHeight(size.height() * radio);
    m_rectImage.setX(this->rect().center().x() - (m_rectImage.width() / 2));
    m_rectImage.setY(this->rect().center().y() - (m_rectImage.height() / 2));
    m_imageTemp = m_imageSrc.scaled(m_rectImage.width(),m_rectImage.height(),Qt::KeepAspectRatio);
    this->update();

    m_thread.setScale(radio);

    event->accept();
}

void HS_ImageViewUi::resizeEvent(QResizeEvent *event)
{
    this->initActiveArea();
    m_panel->move((this->rect().width() - m_panel->width()) / 2,this->rect().bottom() - m_panel->height());
    m_btnClose->move(this->rect().topRight().x() - m_btnClose->width() + 1,0);
    m_btnPrevious->move(20,(this->height() - m_btnPrevious->height()) / 2);
    m_btnNext->move(this->width() - m_btnNext->width() - 20,(this->height() - m_btnPrevious->height()) / 2);
}

void HS_ImageViewUi::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape){
        this->showCloseAnimation();
    }
}

void HS_ImageViewUi::showEvent(QShowEvent *event)
{

}

void HS_ImageViewUi::leaveEvent(QEvent *event)
{
    this->setWidgetVisible(QPoint(-1,-1));
}

void HS_ImageViewUi::updateImage(const QImage &image, double radio)
{
    m_imageTemp = image;
    m_panel->setText(QString::number((int)(radio * 100)).append("%"));
    this->update();
}

void HS_ImageViewUi::init()
{
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setWindowTitle("ImageViewer");
    this->setMinimumSize(minSize);
    this->moveToCenter();
    //动画组
    m_pGroup = new QParallelAnimationGroup(this);
    connect(m_pGroup,&QParallelAnimationGroup::finished,this,&HS_ImageViewUi::hide);

    m_imageScaling = new QFutureWatcher<QImage>(this);
//    connect(m_imageScaling, SIGNAL(resultReadyAt(int)), SLOT(updateImage(int)));
//    connect(m_imageScaling, &QFutureWatcher::finished, this,SLOT(finished()));

    connect(&m_thread,&ThreadScaleImage::updateImage,this,&HS_ImageViewUi::updateImage,Qt::QueuedConnection);

    //初始化按钮和控制面板
    {
        {
            m_panel = new controlPanel(this);
        }

        {
            m_btnClose = new HS_FadeOutButton(this);
            connect(m_btnClose,&HS_FadeOutButton::clicked,[=](){
                this->showCloseAnimation();
            });
            m_btnClose->setFixedSize(42,40);
            m_btnClose->setObjectName("m_btnClose");
            m_btnClose->setStyleSheet("#m_btnClose{border:0;background-color:rgba(0,0,0,60);}"
                                      "#m_btnClose::hover{border:0;background-color:rgb(205,84,74);}"
                                      "#m_btnClose::pressed{border:0;background-color:rgb(168,48,39);}");
            m_btnClose->setIcon(QIcon(":/images/close.png"));
            m_btnClose->setCursor(Qt::ArrowCursor);
        }

        {
            m_btnPrevious = new HS_FadeOutButton(this);
            m_btnPrevious->setObjectName("m_btnPrevious");
            m_btnPrevious->setStyleSheet("#m_btnPrevious{border:0;border-image:url(:/images/back_gray.png);}"
                                      "#m_btnPrevious::hover{border:0;border-image:url(:/images/back.png);}"
                                      "#m_btnPrevious::pressed{border:0;border-image:url(:/images/back.png);}");
            m_btnPrevious->setCursor(Qt::ArrowCursor);
            m_btnPrevious->setFixedSize(55,55);
            connect(m_btnPrevious,&HS_FadeOutButton::clicked,[=](){
                //todo
            });
        }

        {
            m_btnNext = new HS_FadeOutButton(this);
            m_btnNext->setObjectName("m_btnNext");
            m_btnNext->setStyleSheet("#m_btnNext{border:0;border-image:url(:/images/next_gray.png);}"
                                      "#m_btnNext::hover{border:0;border-image:url(:/images/next.png);}"
                                      "#m_btnNext::pressed{border:0;border-image:url(:/images/next.png);}");
            m_btnNext->setCursor(Qt::ArrowCursor);
            m_btnNext->setFixedSize(55,55);
            connect(m_btnNext,&HS_FadeOutButton::clicked,[=](){
                //todo
            });
        }

    }


    this->setCursor(Qt::ClosedHandCursor);
    this->setMouseTracking(true);
}

void HS_ImageViewUi::initActiveArea()
{
    m_rectCloseBtn.setRect(0,0,this->width(),160);
    m_rectPanel.setRect(0,this->height() - 150,this->width(),150);
    m_rectPrevious.setRect(0,0,120,this->height());
    m_rectNext.setRect(this->width() - 120,0,120,this->height());
}

void HS_ImageViewUi::moveToCenter()
{
    QPoint posCenter = QApplication::desktop()->availableGeometry().center();
    this->move(posCenter.x() - (this->width() / 2),posCenter.y() - (this->height() / 2));
}

void HS_ImageViewUi::initSize()
{
    QSize imageSize = m_imageSrc.size(); //图像原始大小
    int width = imageSize.width();
    int height = imageSize.height();

    //屏幕大小
    const QSize &screenSize = QApplication::desktop()->availableGeometry().size();

    //屏幕宽度
    const int screenWidth = screenSize.width();

    //屏幕高度
    const int screenHeight = screenSize.height();

    QSize result(width,height);

    auto heightFitWidth = [=](QSize &size){
        size.setWidth(screenWidth);
        double radio = height / (double)width;
        size.setHeight(radio * screenWidth);
    };

    auto widthFitHeight = [=](QSize &size){
        size.setHeight(screenHeight);
        double radio = width / (double)height;
        size.setWidth(radio * screenHeight);
    };

    //图片大小 > 最小窗口尺寸
    if((width > minWidth) || (height > minHeight)){
        //pixmap width > height
        if(width > height)
        {
            if(width > screenWidth){
                heightFitWidth(result);
            }
            else if(height > screenHeight){
                widthFitHeight(result);
            }
        }
        else
        {
            if(height > screenHeight){
                widthFitHeight(result);
            }
            else if(width > screenWidth){
                heightFitWidth(result);
            }
        }
    }

    this->resize(qMax(result.width(),minWidth),qMax(result.height(),minHeight));

    m_rectImage.setX(this->rect().center().x() - (result.width() / 2));
    m_rectImage.setY(this->rect().center().y() - (result.height() / 2));
    m_rectImage.setWidth(result.width());
    m_rectImage.setHeight(result.height());

    this->moveToCenter();
}

void HS_ImageViewUi::deleteAnimation()
{
    int n = m_pGroup->animationCount();
    for(int i = 0;i < n;++i)
    {
        QAbstractAnimation *p = m_pGroup->takeAnimation(i);
        if(p){
            delete p;
            p = NULL;
        }
    }
    m_pGroup->clear();
}

void HS_ImageViewUi::showCloseAnimation()
{
    this->deleteAnimation();

    QRect rect = this->geometry();
    QPropertyAnimation *animation1 = new QPropertyAnimation(this,"geometry");
    animation1->setDuration(duration);
    animation1->setStartValue(rect);
    animation1->setEndValue(QRect(rect.x(), rect.y() - 50, rect.width(), rect.height()));
    animation1->setEasingCurve(QEasingCurve::Linear);

    QPropertyAnimation *animation2 = new QPropertyAnimation(this,"windowOpacity");
    animation2->setDuration(duration);
    animation2->setKeyValueAt(0,1);
    animation2->setKeyValueAt(1,0);
    animation2->setEasingCurve(QEasingCurve::Linear);

    m_pGroup->addAnimation(animation1);
    m_pGroup->addAnimation(animation2);
    m_pGroup->start();
}

void HS_ImageViewUi::setWidgetVisible(const QPoint &pos)
{
    auto func_showWidget = [=](const QRect &rect,HS_FadeOutWidget *widget){
        if(rect.contains(pos)){
            if(!widget->isVisible()){
                widget->setShow();
            }
        }
        else{
            if(widget->isVisible()){
                widget->setHide();
            }
        }
    };

    auto func_showBtn = [=](const QRect &rect,HS_FadeOutButton *widget){
        if(rect.contains(pos)){
            if(!widget->isVisible()){
                widget->setShow();
            }
        }
        else{
            if(widget->isVisible()){
                widget->setHide();
            }
        }
    };

    func_showBtn(m_rectCloseBtn,m_btnClose);
    func_showBtn(m_rectPrevious,m_btnPrevious);
    func_showBtn(m_rectNext,m_btnNext);
    func_showWidget(m_rectPanel,m_panel);
}

void HS_ImageViewUi::setCurrentIndex()
{
    int index = 0;

    m_nScaleIndex = index;
}

void HS_ImageViewUi::showWidgetAnimation(QWidget *widget, bool visible)
{
    if(!widget) return;
    QPropertyAnimation *animation = new QPropertyAnimation(widget,"windowOpacity");
    connect(animation,&QPropertyAnimation::finished,[=](){
        widget->setVisible(visible);
    });
    animation->setDuration(500);
    if(visible){
        widget->setWindowOpacity(0.0);
        widget->show();
        animation->setStartValue(0.0);
        animation->setEndValue(1.0);
    }
    else{
        animation->setStartValue(1.0);
        animation->setEndValue(0.0);
    }
    animation->setEasingCurve(QEasingCurve::Linear);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

//图片浏览的控制面板，可操作放大，缩小，另存图片等
#include <QHBoxLayout>

controlPanel::controlPanel(QWidget *parent)
    :HS_FadeOutWidget(parent)
{
    this->init();
}

controlPanel::~controlPanel()
{

}

void controlPanel::setText(const QString &str)
{
    if(m_label_zoom)
        m_label_zoom->setText(str);
}

void controlPanel::init()
{
    this->setFixedSize(270,50);
    m_btn_zoomIn  = new QPushButton("Zoom In",this);
    m_btn_zoomOut = new QPushButton("Zoom Out",this);
    m_btn_save    = new QPushButton("Save As...",this);
    m_label_zoom  = new QLabel(this);
    m_label_zoom->setAlignment(Qt::AlignCenter);

    m_btn_zoomIn-> setCursor(Qt::PointingHandCursor);
    m_btn_zoomOut->setCursor(Qt::PointingHandCursor);
    m_btn_save->   setCursor(Qt::PointingHandCursor);

    m_btn_zoomIn-> setFixedSize(70,30);
    m_btn_zoomOut->setFixedSize(70,30);
    m_btn_save->   setFixedSize(70,30);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setSpacing(10);
    layout->setContentsMargins(0,10,0,10);
    this->setLayout(layout);

    layout->addStretch();
    layout->addWidget(m_btn_zoomIn);
    layout->addWidget(m_btn_zoomOut);
    layout->addWidget(m_btn_save);
    layout->addWidget(m_label_zoom);
    layout->addStretch();

}

ThreadScaleImage::ThreadScaleImage(QObject *object)
{
    m_suspend = false;
    m_stop = false;
}

ThreadScaleImage::~ThreadScaleImage()
{

}

void ThreadScaleImage::setImage(const QImage &image)
{
    QMutexLocker locker(&m_mutex);
    m_image = image;
}

void ThreadScaleImage::setImage(const QString &file)
{
    QMutexLocker locker(&m_mutex);
    m_strfile = file;
}

void ThreadScaleImage::setScale(double radio)
{
    QMutexLocker locker(&m_mutex);
    m_radio = radio;
    m_suspend = false;
}

void ThreadScaleImage::setScale(const QSizeF &size)
{
    QMutexLocker locker(&m_mutex);
    m_scaledSize = size;
    m_suspend = false;
}

void ThreadScaleImage::suspend()
{
    QMutexLocker locker(&m_mutex);
    m_suspend = true;
}

void ThreadScaleImage::resum()
{
    QMutexLocker locker(&m_mutex);
    m_suspend = false;
}

void ThreadScaleImage::stop()
{
    QMutexLocker locker(&m_mutex);
    m_stop = true;
}

void ThreadScaleImage::run()
{
    while (1) {
        QMutexLocker locker(&m_mutex);
        if(!m_suspend){
            int width = m_radio * m_image.width();
            int height = m_radio * m_image.height();
            m_reader.setFileName(m_strfile);
            m_reader.setQuality(100);
            if(m_reader.canRead()){
                m_reader.setScaledSize(QSize(m_scaledSize.width(),m_scaledSize.height()));
                emit updateImage(m_reader.read(),m_radio);
            }
//            m_reader.setScaledSize(QSize(width,height));
        }

        m_suspend = true;

        this->msleep(5);

        if(m_stop)
            break;
    }
}
