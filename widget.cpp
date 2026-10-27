
#include "widget.h"
#include "ui_widget.h"

#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QStringConverter>
#include <QStringList>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QDebug>

// 初始化視窗
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // 設定表格欄位
    ui->tableWidget->setColumnCount(4);

    QStringList titles;
    titles << "學號" << "班級" << "姓名" << "電話";

    ui->tableWidget->setHorizontalHeaderLabels(titles);

    // 表格開始時沒有資料
    ui->tableWidget->setRowCount(0);
}

// 解構函式
Widget::~Widget()
{
    delete ui;
}

// ===========================
// 新增資料
// ===========================
void Widget::on_pushButton_clicked()
{
    QString studentID = ui->lineEdit->text();
    QString className = ui->lineEdit_2->text();
    QString name = ui->lineEdit_3->text();
    QString phone = ui->lineEdit_4->text();

    int row = ui->tableWidget->rowCount();

    ui->tableWidget->insertRow(row);

    ui->tableWidget->setItem(
        row, 0, new QTableWidgetItem(studentID));

    ui->tableWidget->setItem(
        row, 1, new QTableWidgetItem(className));

    ui->tableWidget->setItem(
        row, 2, new QTableWidgetItem(name));

    ui->tableWidget->setItem(
        row, 3, new QTableWidgetItem(phone));

    // 清空輸入框
    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    ui->lineEdit_3->clear();
    ui->lineEdit_4->clear();
}

// ===========================
// 匯入 CSV
// ===========================
void Widget::on_pushButton_3_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "選擇 CSV 檔案",
        "",
        "CSV Files (*.csv)"
        );

    if(fileName.isEmpty())
        return;

    QFile file(fileName);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(
            this, "錯誤", "無法開啟 CSV 檔案");
        return;
    }

    QTextStream in(&file);

    // 使用 UTF-8 讀取中文
    in.setEncoding(QStringConverter::Utf8);

    // 清空原本表格
    ui->tableWidget->setRowCount(0);

    while(!in.atEnd())
    {
        QString line = in.readLine();

        if(line.trimmed().isEmpty())
            continue;

        QStringList fields;
        QString field;
        bool inQuotes = false;

        // 解析 CSV
        for(int i = 0; i < line.length(); i++)
        {
            QChar ch = line[i];

            if(ch == '"')
            {
                if(inQuotes &&
                    i + 1 < line.length() &&
                    line[i + 1] == '"')
                {
                    field += '"';
                    i++;
                }
                else
                {
                    inQuotes = !inQuotes;
                }
            }
            else if(ch == ',' && !inQuotes)
            {
                fields << field;
                field.clear();
            }
            else
            {
                field += ch;
            }
        }

        fields << field;

        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);

        for(int j = 0; j < 4 && j < fields.size(); j++)
        {
            ui->tableWidget->setItem(
                row,
                j,
                new QTableWidgetItem(fields[j])
                );
        }
    }

    file.close();

    QMessageBox::information(
        this, "完成", "CSV 匯入成功！");
}

// ===========================
// 匯出 CSV
// ===========================
void Widget::on_pushButton_2_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "儲存 CSV 檔案",
        "myfile.csv",
        "CSV Files (*.csv)"
        );

    if(fileName.isEmpty())
        return;

    if(!fileName.endsWith(".csv", Qt::CaseInsensitive))
        fileName += ".csv";

    QFile file(fileName);

    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(
            this, "錯誤", "無法儲存 CSV 檔案");
        return;
    }

    QTextStream out(&file);

    // UTF-8 編碼
    out.setEncoding(QStringConverter::Utf8);

    // 加入 BOM，避免 Excel 中文亂碼
    out.setGenerateByteOrderMark(true);

    for(int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        QStringList rowData;

        for(int j = 0; j < 4; j++)
        {
            QTableWidgetItem *item =
                ui->tableWidget->item(i, j);

            QString value = "";

            if(item != nullptr)
                value = item->text();

            // 處理 CSV 特殊字元
            if(value.contains(',') ||
                value.contains('"') ||
                value.contains('\n') ||
                value.contains('\r'))
            {
                value.replace("\"", "\"\"");
                value = "\"" + value + "\"";
            }

            rowData << value;
        }

        out << rowData.join(",") << "\n";
    }

    file.close();

    QMessageBox::information(
        this, "完成", "CSV 匯出成功！");
}



