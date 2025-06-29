#include "aboutdialog.h"
#include "./ui_aboutdialog.h"

#include <QPixmap>
#include <QPainter>

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    setFixedSize(size());

    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setAttribute(Qt::WA_TranslucentBackground);

    ui->btnClose->setText("");
    ui->btnClose->setFixedSize(25, 25);
    ui->btnClose->setIcon(QIcon(":/images/icons/icons8-close-64.png"));
    ui->btnClose->setIconSize(QSize(10, 10));

    ui->version->setText( version );

    connect(ui->btnClose, &QPushButton::clicked, this, &AboutDialog::close);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QRect r = rect();

    p.setBrush(QColor("#2d283c"));
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(r, 15, 15);

    // Borda sutil
    QColor borderColor = QColor(0, 0, 0, 80);
    QPen pen(borderColor, 1.0);
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);
    p.drawRoundedRect(r.adjusted(0, 0, -1, -1), 15, 15);
}
