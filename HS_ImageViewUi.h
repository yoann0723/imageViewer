#ifndef HS_IMAGEVIEWUI_H
#define HS_IMAGEVIEWUI_H

#include <QDialog>
#include <QLabel>
#include <QMouseEvent>
#include <QParallelAnimationGroup>
#include <QPushButton>
#include "HS_FadeOutWidget.h"
#include <QFutureWatcher>
#include <QThread>
#include <QImageReader>

namespace Ui {
class HS_ImageViewUi;
}

//图片浏览的控制面板，可操作放大，缩小，另存图片等
class controlPanel:public HS_FadeOutWidget
{
    Q_OBJECT
public:
    explicit controlPanel(QWidget *parent = 0);
    ~controlPanel();

public:
    void setText(const QString &str);

private:
    void init();

private:
    QPushButton *m_btn_zoomIn;
    QPushButton *m_btn_zoomOut;
    QPushButton *m_btn_save;
    QLabel      *m_label_zoom;
};

class ThreadScaleImage: public QThread
{
    Q_OBJECT
public:
    ThreadScaleImage(QObject *object = 0);
    ~ThreadScaleImage();

public:
    void setImage(const QImage &image);
    void setImage(const QString &file);
    void setScale(double radio = 1.0);
    void setScale(const QSizeF &size);
    void suspend();
    void resum();
    void stop();

signals:
    void updateImage(const QImage &image,double radio);

protected:
    void run();
private:
    QImage m_image;
    QString m_strfile;
    QSizeF  m_scaledSize;
    bool   m_suspend;
    bool   m_stop;
    double m_radio;
    QMutex m_mutex;
    QImageReader m_reader;
};

class HS_ImageViewUi : public QDialog
{
    Q_OBJECT

public:
    explicit HS_ImageViewUi(QWidget *parent = 0);
    ~HS_ImageViewUi();

    void setPixmap(const QPixmap &pixmap);
    void setImage(const QImage &image);
    void setImage(const QString &file);
    void setPixmaps(const QStringList &files);

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void showEvent(QShowEvent *event);
    virtual void leaveEvent(QEvent *event);

private slots:
    void updateImage(const QImage &image,double radio);

private:
    void init();
    void initActiveArea();
    void moveToCenter();
    void initSize();
    void deleteAnimation();
    void showCloseAnimation();

    void setWidgetVisible(const QPoint &pos);
    void setCurrentIndex();

    //设置widget的动画，visible为ture时，widget显示，为false时，widget隐藏
    void showWidgetAnimation(QWidget *widget,bool visible);

private:
    QPixmap m_pixmap;
    QImage  m_imageSrc;
    QImage  m_imageTemp;

    QRect   m_rectCloseBtn;
    QRect   m_rectPanel;
    QRect   m_rectPrevious;
    QRect   m_rectNext;

    double  m_radio;
    int     m_nScaleIndex;

    bool    m_bPressed;
    bool    m_moved;
    bool    m_inImage;
    bool    m_bZoomIn; //放大

    QRectF  m_rectImage;
    QSizeF  m_sizeImage;
    QPointF m_point;
    QPoint  m_posWidget;

    QParallelAnimationGroup *m_pGroup;
    controlPanel            *m_panel;
    HS_FadeOutButton        *m_btnClose;
    HS_FadeOutButton        *m_btnPrevious;
    HS_FadeOutButton        *m_btnNext;
    QFutureWatcher<QImage>  *m_imageScaling;
    ThreadScaleImage         m_thread;

};

#endif // HS_IMAGEVIEWUI_H
