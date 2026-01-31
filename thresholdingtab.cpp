#include "thresholdingtab.h"
#include "ui_thresholdingtab.h"

ThresholdingTab::ThresholdingTab(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::ThresholdingTab)
{
    ui->setupUi(this);
}

ThresholdingTab::~ThresholdingTab()
{
    delete ui;
}
