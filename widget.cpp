#include "widget.h"
#include "ui_widget.h"
#include <QSvgWidget>
#include <QTimerEvent>
#include <QPainter>
#include <QPen>
#include <QPainterPath>
#include <QLabel>
#include <QDebug>
#include <QFileDialog>

const int imageSize = 100;

QImage scale(const QString &imageFileName)
{
    QImage image(imageFileName);
    return image.scaled(QSize(imageSize, imageSize), Qt::KeepAspectRatio, Qt::SmoothTransformation);
}


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),PATTERN_LENGTH(4)
{
    ui->setupUi(this);

    this->setMinimumSize(732,552);
    m_value = 100;
//    dialog.setParent(this);
//    dialog = new HS_ImageViewUi(this);
//    dialog->hide();

//    dashes = PATTERN_LENGTH;
//    spaces = PATTERN_LENGTH;

//    for (int i = 0; i < 400; ++i) {
//        dashPattern << PATTERN_LENGTH;
//    }

//    timerId = startTimer(1000);

    m_Model = new QStandardItemModel;
    ui->tableView->setModel(m_Model);

    ui->tableView->horizontalHeader()->setVisible(false);// 水平不可见
    ui->tableView->verticalHeader()->setVisible(false);// 垂直不可见
    ui->tableView->setShowGrid(false);

    m_imageScaling = new QFutureWatcher<QImage>(this);
    connect(m_imageScaling, SIGNAL(resultReadyAt(int)), SLOT(showImage(int)));
    connect(m_imageScaling, SIGNAL(finished()), SLOT(finished()));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == timerId) {
        advanceDashes();
        update();
    }
}

void Widget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
//    painter.fillRect(this->rect(),QColor(0,0,0,80));

//    QPen pen;
//    pen.setDashPattern(dashPattern); // 蚂蚁线的 style

//    painter.translate(20, 20);
//    painter.setPen(Qt::white);
//    painter.drawRect(0, 0, 100, 100);
//    painter.setPen(pen);
//    painter.drawRect(0, 0, 100, 100);

//    QPainterPath path;
//    path.cubicTo(50, 0, 50, 100, 100, 100);
//    path.cubicTo(150, 0, 150, 100, 200, 0);

//    painter.translate(120, 0);
//    painter.setPen(Qt::white);
//    painter.drawPath(path);

//    painter.setPen(pen);
    //    painter.drawPath(path);
}

void Widget::resizeEvent(QResizeEvent *event)
{
    int nWidth = this->width();
    int nCount = nWidth / (imageSize + 100);
    emit sigResize(this->size());
}

//void Widget::wheelEvent(QWheelEvent *event)
//{
//    int numDegrees = event->delta() / 8;
//    int numSteps = numDegrees / 15;

//    if (event->orientation() == Qt::Vertical) {

//        m_value += numSteps * 10;

//        QSize size = ui->label->size();
//        double radio = m_value / 100.0;
//        ui->label->resize(size * radio);

//        qDebug() << QString("size:%1*%2").arg(size.width()).arg(size.height()) << "Zoom:"<< m_value << "%(" << radio << ")";
//    }
//    event->accept();
//}

void Widget::advanceDashes()
{
    if (PATTERN_LENGTH == dashes && PATTERN_LENGTH == spaces) {
        dashes = 0;
        spaces = 0;
    }

    if (0 == dashes && spaces < PATTERN_LENGTH) {
        ++spaces;
    } else if (PATTERN_LENGTH == spaces && dashes < PATTERN_LENGTH) {
        ++dashes;
    }

    dashPattern[0] = dashes;
    dashPattern[1] = spaces;
}

void Widget::on_m_btn_ok_clicked()
{
    // Cancel and wait if we are already loading images.
    if (m_imageScaling->isRunning()) {
        m_imageScaling->cancel();
        m_imageScaling->waitForFinished();
    }

    // Show a file open dialog at QStandardPaths::PicturesLocation.
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Select Images"),
                            QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
                            "*.jpg *.png");

    if (files.count() == 0)
        return;

    // Do a simple layout.
//    qDeleteAll(m_labels);
    m_listFiles.clear();
    m_listFiles = files;
    m_Model->clear();
    m_Model->setRowCount(0);
    m_labels.clear();

    int rowCount = (files.size() / 10) + ((files.size() % 10 > 0)?1:0);
    m_Model->setColumnCount(10);
    m_Model->setRowCount(rowCount);
    int row = 0;
    int colum = 0;
    for(int i = 0;i < files.size();++i)
    {
        if(colum > m_Model->columnCount() - 1) {
            colum = 0;
            row++;
        }

        QLabel *imageLabel = new QLabel;
        imageLabel->setAlignment(Qt::AlignCenter);
        imageLabel->setFixedSize(imageSize,imageSize);
        m_labels.append(imageLabel);
        QModelIndex index = m_Model->index(row,colum);
        qDebug() << "row:" <<row << "colum:"<<colum;
        ui->tableView->setColumnWidth(colum,imageSize + 10);
        ui->tableView->setRowHeight(row,imageSize + 10);
        ui->tableView->setIndexWidget(index,imageLabel);
        colum ++;
    }

    // Use mapped to run the thread safe scale function on the files.
    m_imageScaling->setFuture(QtConcurrent::mapped(files, scale));
}

void Widget::showImage(int index)
{
    m_labels[index]->setPixmap(QPixmap::fromImage(m_imageScaling->resultAt(index)));
}

void Widget::finished()
{

}

void Widget::on_tableView_doubleClicked(const QModelIndex &index)
{
    HS_ImageViewUi dialog(this);
    int n = index.row() * m_Model->columnCount() + index.column();
    if(n > -1 && n < m_listFiles.size()){
        QString strFile = m_listFiles[n];
        dialog.setImage(strFile);
        dialog.exec();
    }
}
