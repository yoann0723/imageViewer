#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QVector>
#include <QPushButton>
#include <QStandardItemModel>
#include <QWheelEvent>
#include <QtConcurrent>
#include <QLabel>
#include "HS_ImageViewUi.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

protected:
    void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
//    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private slots:
    void on_m_btn_ok_clicked();
    void showImage(int index);
    void finished();

    void on_tableView_doubleClicked(const QModelIndex &index);

signals:
    void sigResize(const QSize &size);

private:
    void advanceDashes();

    int timerId;
    int dashes;
    int spaces;
    const int PATTERN_LENGTH;
    QVector<qreal> dashPattern;

private:
    Ui::Widget *ui;
    QStandardItemModel     *m_Model;
    int                     m_value;
    QList<QLabel *>         m_labels;
    QFutureWatcher<QImage> *m_imageScaling;
    QStringList             m_listFiles;
//    HS_ImageViewUi          *dialog;
};

#endif // WIDGET_H
