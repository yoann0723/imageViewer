#include "svgButton.h"
#include "qsvgrenderer.h"

#include "qpainter.h"


svgButton::svgButton(QWidget *parent)
    :QPushButton(parent)
{
    renderer = new QSvgRenderer(this);
    QObject::connect(renderer, SIGNAL(repaintNeeded()),
                     this, SLOT(update()));
}

svgButton::~svgButton()
{

}

void svgButton::load(const QString &file)
{
    renderer->load(file);
}

void svgButton::load(const QByteArray &contents)
{
   renderer->load(contents);
}

void svgButton::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    renderer->render(&p);
}
