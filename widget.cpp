#include "widget.h"
#include "ui_widget.h"
#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <QFileDialog> // 1. 引入 QFileDialog 標頭檔以支援選擇資料夾與檔案對話框

void Write(QString Filename, QString str)
{
    QFile mFile(Filename);

    // 寫入檔案判斷
    if(!mFile.open(QFile::WriteOnly | QFile::Text)){
        qDebug() << "could not open file for write";
        return;
    }

    QTextStream out(&mFile);

    // 設定編碼為 UTF-8，並加入 BOM，這樣 Excel 才能正確顯示中文繁體
    out.setEncoding(QStringConverter::Utf8);
    out.setGenerateByteOrderMark(true); // 支援 Qt 6 (如果是 Qt 5，請用 out.setCodec("UTF-8");)

    out << str;  // 將內容輸出至 csv 檔

    mFile.flush();
    mFile.close();
}

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    QStringList ColTotle;

    ui->tableWidget->setColumnCount(4);
    ColTotle << QStringLiteral("學號") << QStringLiteral("班級") << QStringLiteral("姓名") << QStringLiteral("電話");
    ui->tableWidget->setHorizontalHeaderLabels(ColTotle);
}

Widget::~Widget()
{
    delete ui;
}

// 按下按鈕新增資料到 TableWidget
void Widget::on_pushButton_clicked()
{
    int row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);

    ui->tableWidget->setItem(row, 0, new QTableWidgetItem(ui->lineEdit->text()));
    ui->tableWidget->setItem(row, 1, new QTableWidgetItem(ui->lineEdit_2->text()));
    ui->tableWidget->setItem(row, 2, new QTableWidgetItem(ui->lineEdit_3->text()));
    ui->tableWidget->setItem(row, 3, new QTableWidgetItem(ui->lineEdit_4->text()));
}

// 按下按鈕匯出到 CSV（彈出視窗讓使用者選擇儲存路徑）
void Widget::on_pushButton_2_clicked()
{
    // 2. 彈出存檔對話框讓使用者選擇資料夾與輸入檔名
    QString mFilename = QFileDialog::getSaveFileName(this,
                                                     QStringLiteral("選擇存檔路徑"),
                                                     "C:/",
                                                     QStringLiteral("CSV 檔案 (*.csv);;所有檔案 (*.*)"));

    // 如果使用者按了「取消」，則直接返回，不執行後續動作
    if (mFilename.isEmpty()) {
        return;
    }

    QString saveFile = ""; // 存檔內容

    // 寫入表頭標題
    for(int j = 0; j < ui->tableWidget->columnCount(); j++) {
        saveFile += ui->tableWidget->horizontalHeaderItem(j)->text() + ",";
    }
    saveFile += "\n";

    // 讀取表格內容
    for(int i = 0; i < ui->tableWidget->rowCount(); i++){
        for(int j = 0; j < ui->tableWidget->columnCount(); j++){
            QTableWidgetItem *item = ui->tableWidget->item(i, j);
            if(item) {
                saveFile += item->text() + ",";
            } else {
                saveFile += ","; // 避免格子是空的程式崩潰
            }
        }
        saveFile += "\n";
    }

    Write(mFilename, saveFile); // 呼叫 Write 將資料寫入使用者選擇的路徑
    qDebug() << "匯出成功！";
}