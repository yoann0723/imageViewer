#ifndef HS_FADEOUTWIDGET_H
#define HS_FADEOUTWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QShowEvent>
#include <QHideEvent>
#include <QGraphicsOpacityEffect>

class HS_FadeOutWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HS_FadeOutWidget(QWidget *parent = 0);
    ~HS_FadeOutWidget();

public Q_SLOTS:
    void setShow();
    void setHide();

private:
    QPropertyAnimation *m_animationShow;
    QPropertyAnimation *m_animationHide;
    QGraphicsOpacityEffect *m_pOpacityEffect;
};

class HS_FadeOutButton:public QPushButton
{
    Q_OBJECT
public:
    explicit HS_FadeOutButton(QWidget *parent = 0);
    ~HS_FadeOutButton();

public Q_SLOTS:
    void setShow();
    void setHide();

private:
    QPropertyAnimation *m_animationShow;
    QPropertyAnimation *m_animationHide;
    QGraphicsOpacityEffect *m_pOpacityEffect;
};

#endif // HS_FADEOUTWIDGET_H
