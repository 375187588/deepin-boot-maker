/*
 * Copyright (C) 2017 ~ 2018 Wuhan Deepin Technology Co., Ltd.
 *
 * Author:     Iceyer <me@iceyer.net>
 *
 * Maintainer: Iceyer <me@iceyer.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "progressview.h"
#include "widgetutil.h"
#include "deviceinfoitem.h"
#include "devicelistwidget.h"
#include <bminterface.h>

#include <DCheckBox>
#include <DLabel>
#include <DListWidget>
#include <DWaterProgress>
#include <DPushButton>
#include <DApplicationHelper>
#include <DTipLabel>

#include <QDebug>
#include <QVBoxLayout>
#include <QFontDatabase>

DWIDGET_USE_NAMESPACE

ProgressView::ProgressView(DWidget *parent) : DWidget(parent)
{
    setObjectName("ProgressView");
    setAutoFillBackground(true);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 1, 0, 0);

    int lcdFontId = QFontDatabase::addApplicationFont(":/theme/SourceHanSansSC-Medium.ttf");
    int lcdFontId1 = QFontDatabase::addApplicationFont(":/theme/SourceHanSansSC-Bold.ttf");
    QStringList m_fontList;
    m_fontList.clear();
    if (lcdFontId != -1) {
        m_fontList << QFontDatabase::applicationFontFamilies(lcdFontId);
    }
    if (lcdFontId != -1) {
        m_fontList << QFontDatabase::applicationFontFamilies(lcdFontId1);
    }

    DLabel *m_title = new DLabel(tr("Burning"));
    m_title->setFixedHeight(36);
    QFont qf = m_title->font();
    if (m_fontList.size() > 0)
        qf.setFamily(m_fontList.at(0));
    qf.setPixelSize(24);
    m_title->setFont(qf);
//    m_title->setStyleSheet("font-size: 26px;");

    auto waterProgress = new Dtk::Widget::DWaterProgress;
    waterProgress->setFixedSize(100, 100);

    DLabel *m_hitsTitle = new DLabel(tr("Burning, please wait..."));
    m_hitsTitle->setObjectName("ProgressHitsTitle");
    m_hitsTitle->setFixedHeight(25);
    qf = m_hitsTitle->font();
    qf.setPixelSize(17);
    if (m_fontList.size() > 1)
        qf.setFamily(m_fontList.at(1));
//    qf.setBold(true);
    m_hitsTitle->setFont(qf);
    m_hitsTitle->setAlignment(Qt::AlignCenter);

//    DTipLabel *m_hits = new DTipLabel(tr("Do not remove the disk or shut down the computer during the process"));
    DLabel *m_hits = new DLabel(tr("Do not remove the disk or shut down the computer during the process"));
    m_hits->setObjectName("ProgressHits");
    m_hits->setFixedSize(213, 17);
    m_hits->setWordWrap(true);
    qf = m_hits->font();
    qf.setPixelSize(12);
    m_hits->setFont(qf);
    m_hits->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    DPalette pa = DApplicationHelper::instance()->palette(m_hits);
    QBrush brush = DApplicationHelper::instance()->palette(m_hits).textTips();
    pa.setBrush(DPalette::WindowText, brush);
    m_hits->setPalette(pa);

//    SuggestButton *start = new SuggestButton();
    DPushButton *start = new DPushButton();
    start->setFocusPolicy(Qt::NoFocus);
    start->setFixedSize(310, 36);
    start->setObjectName("ProgressCancel");
    start->setText(tr("Cancel"));
    qf = start->font();
    if (m_fontList.size() > 0)
        qf.setFamily(m_fontList.at(0));
    qf.setPixelSize(14);
    start->setFont(qf);

    mainLayout->addWidget(m_title, 0, Qt::AlignHCenter);
    mainLayout->addSpacing(95);
    mainLayout->addWidget(waterProgress, 0, Qt::AlignHCenter);
    mainLayout->addSpacing(26);
    mainLayout->addWidget(m_hitsTitle, 0, Qt::AlignHCenter);
    mainLayout->addSpacing(1);
    mainLayout->addWidget(m_hits, 0, Qt::AlignHCenter);
    mainLayout->addSpacing(20);
    mainLayout->addWidget(start, 0, Qt::AlignHCenter);
    mainLayout->addStretch();

    waterProgress->setValue(0);
    waterProgress->start();

//    this->setStyleSheet(WidgetUtil::getQss(":/theme/light/ProgressView.theme"));
//    this->setStyleSheet("#ProgressHits{line-height: 1.67;}");

//    connect(start, &SuggestButton::clicked, this, &ProgressView::testCancel);
    start->hide();


    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged,
    this, [ = ] {
        DPalette pa;
        DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
        if (themeType == DGuiApplicationHelper::LightType)
        {
            pa = palette();
            pa.setColor(DPalette::Background, QColor(255, 255, 255));
            setPalette(pa);
            pa = m_title->palette();
            pa.setColor(DPalette::WindowText, QColor("#001A2E"));
            m_title->setPalette(pa);
            pa = m_hitsTitle->palette();
            pa.setColor(DPalette::WindowText, QColor("#001A2E"));
            m_hitsTitle->setPalette(pa);
        } else if (themeType == DGuiApplicationHelper::DarkType)
        {
            pa = palette();
            pa.setColor(DPalette::Background, QColor("#292929"));
            setPalette(pa);
            pa = m_title->palette();
            pa.setColor(DPalette::WindowText, QColor("#C0C6D4"));
            m_title->setPalette(pa);
            pa = m_hitsTitle->palette();
            pa.setColor(DPalette::WindowText, QColor("#C0C6D4"));
            m_hitsTitle->setPalette(pa);
        }
    });

    emit DGuiApplicationHelper::instance()->themeTypeChanged(DGuiApplicationHelper::instance()->themeType());

    connect(BMInterface::instance(), &BMInterface::reportProgress,
    this, [ = ](quint32 current, quint32 error, const QString & title, const QString & description) {
        qDebug() << error << current << title << description;
        waterProgress->setValue(static_cast<int>(current));
        if (current >= 100) {
            qDebug() << "finish" << current << error;
            emit finish(error, title, description);
        }
    });
}
