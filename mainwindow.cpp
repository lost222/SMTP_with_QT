#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    out2line = 0;
    ui->label->setText("Interaction");
    ui->label_2->setText("Mail Content");
//    ui->filelist
    connect(&listenthread, SIGNAL(stringChanged(QString)),
        this, SLOT(changeString(QString)));
    connect(&(listenthread.s), SIGNAL(sig_mail_saved(QString)),
        this, SLOT(on_mail_saved(QString)));

    listenthread.start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::loadFile(const QString &fileName)
{
   QFile file(fileName); // 新建QFile对象
   if (!file.open(QFile::ReadOnly | QFile::Text)) {
       return false; // 只读方式打开文件，出错则提示，并返回false
   }
   QTextStream in(&file); // 新建文本流对象
   QApplication::setOverrideCursor(Qt::WaitCursor);
   // 读取文件的全部文本内容，并添加到编辑器中
   ui->textEdit->setPlainText(in.readAll());      QApplication::restoreOverrideCursor();

   // 设置当前文件
   //curFile = QFileInfo(fileName).canonicalFilePath();
   setWindowTitle("mail content");
   return true;
}

bool MainWindow::decode_File(const QString &fileName){
    QFile file(fileName); // 新建QFile对象
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return false; // 只读方式打开文件，出错则提示，并返回false
    }
    int is_decode = 0;
    int decode_type = 0;
    while(!file.atEnd()) {
            QByteArray line = file.readLine();
            QString str(line);

            if (str[0] == '-'){
                is_decode = 0;
                continue;
            }
            if (str.left(7) == "Content"){
               // is_decode = 1;
                if (str.left(12) == "Content-Type"){
                    QString type = str.mid(14,4);
                    qDebug()<<type;
                    if (type == "imag"){decode_type = 1;is_decode = 1;}
                    else {decode_type = 0;}
                }
                continue;
            }

            if(is_decode == 1){
//                line = file.readLine();
//                line = line.remove(line.size()-2 , 2);
                QByteArray to_de = file.readAll();

                //QByteArray nline = file.readLine();
//                while(nline != QByteArray("\r\n") && !file.atEnd()){
//                    nline = nline.remove(nline.size()-2,2);
//                    line = line + nline;
//                    QByteArray nline = file.readLine();
//                    qDebug()<<nline;
//                }
                //line.replace(QByteArray("\r\n"), QByteArray(""));
                to_de.replace(QByteArray("\n"), QByteArray(""));
                qDebug()<<to_de;
//                if (decode_type == 1)
                QByteArray barray = QByteArray::fromBase64(to_de);
                QImage image;
                image.loadFromData(barray);
                ui->image->clear();
                ui->image->setPixmap(QPixmap::fromImage(image));
            }


            ui->textEdit->append(str);
        }
    return true;
}

void MainWindow::changeString(const QString &str)
{
    ui->interactionlist->insertItem(out2line++,str);
}

void MainWindow::on_mail_saved(const QString &str)
{
    this->decode_File(str);
    return ;
}

