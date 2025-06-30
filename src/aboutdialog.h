#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QPoint>
#include <QShowEvent>
#include <QPaintEvent>
#include <QGraphicsDropShadowEffect>

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

    public:
        explicit AboutDialog(QWidget *parent = nullptr);
        ~AboutDialog();

    protected:
        void paintEvent(QPaintEvent *) override;

    private:
        Ui::AboutDialog *ui;

        QString version = "0.1.2";
};

#endif // ABOUTDIALOG_H
