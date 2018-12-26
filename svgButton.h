#ifndef SVGBUTTON_H
#define SVGBUTTON_H

class QPaintEvent;
class QSvgRenderer;
#include <QPushButton>


class svgButton : public QPushButton
{
    Q_OBJECT
public:
    svgButton(QWidget *parent = 0);
    svgButton(const QString &file, QWidget *parent=0);
    ~svgButton();

public Q_SLOTS:
    void load(const QString &file);
    void load(const QByteArray &contents);
protected:
    void paintEvent(QPaintEvent *event);

private:
    QSvgRenderer *renderer;
};

#endif // SVGBUTTON_H
