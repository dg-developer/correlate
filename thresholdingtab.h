#ifndef THRESHOLDINGTAB_H
#define THRESHOLDINGTAB_H

#include <QTabWidget>

namespace Ui {
    class ThresholdingTab;
}

class ThresholdingTab : public QTabWidget
{
    Q_OBJECT

public:
    explicit ThresholdingTab(QWidget *parent = 0);
    ~ThresholdingTab();

private:
    Ui::ThresholdingTab *ui;
};

#endif // THRESHOLDINGTAB_H
