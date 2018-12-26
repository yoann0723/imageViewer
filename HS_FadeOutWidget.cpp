#include "HS_FadeOutWidget.h"
const int duration = 150;

HS_FadeOutWidget::HS_FadeOutWidget(QWidget *parent)
    : QWidget(parent)
{
    m_pOpacityEffect = new QGraphicsOpacityEffect(this);
    m_pOpacityEffect->setOpacity(1.0);
    this->setGraphicsEffect(m_pOpacityEffect);

    m_animationHide = new QPropertyAnimation(m_pOpacityEffect,"opacity",this);
    m_animationShow = new QPropertyAnimation(m_pOpacityEffect,"opacity",this);
}

HS_FadeOutWidget::~HS_FadeOutWidget()
{

}

void HS_FadeOutWidget::setShow()
{
    m_animationHide->stop();
    m_animationShow->setDuration(duration);
    m_animationShow->setStartValue(0.0);
    m_animationShow->setEndValue(1.0);
    m_animationShow->start();

    connect(m_animationShow,&QPropertyAnimation::finished,[=]{
        QWidget::show();
    });
}

void HS_FadeOutWidget::setHide()
{
    m_animationShow->stop();
    m_animationHide->setDuration(duration);
    m_animationHide->setStartValue(1.0);
    m_animationHide->setEndValue(0.0);
    m_animationHide->start();

    connect(m_animationHide,&QPropertyAnimation::finished,[=]{
        QWidget::hide();
    });
}

HS_FadeOutButton::HS_FadeOutButton(QWidget *parent)
    :QPushButton(parent)
{
    m_pOpacityEffect = new QGraphicsOpacityEffect(this);
    m_pOpacityEffect->setOpacity(1.0);
    this->setGraphicsEffect(m_pOpacityEffect);

    m_animationHide = new QPropertyAnimation(m_pOpacityEffect,"opacity",this);
    m_animationShow = new QPropertyAnimation(m_pOpacityEffect,"opacity",this);
}

HS_FadeOutButton::~HS_FadeOutButton()
{

}

void HS_FadeOutButton::setShow()
{
    m_animationHide->stop();
    m_animationShow->setDuration(duration);
    m_animationShow->setStartValue(0.0);
    m_animationShow->setEndValue(1.0);
    m_animationShow->start();

    connect(m_animationShow,&QPropertyAnimation::finished,[=]{
        QWidget::show();
    });
}

void HS_FadeOutButton::setHide()
{
    m_animationShow->stop();
    m_animationHide->setDuration(duration);
    m_animationHide->setStartValue(1.0);
    m_animationHide->setEndValue(0.0);
    m_animationHide->start();

    connect(m_animationHide,&QPropertyAnimation::finished,[=]{
        QWidget::hide();
    });
}
