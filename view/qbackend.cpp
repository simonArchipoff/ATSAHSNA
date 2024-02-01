#include "qbackend.h"
#include "qboxlayout.h"
#include "qnamespace.h"
#include "qpushbutton.h"

#include <faust/gui/QTUI.h>
#include <QScrollArea>


std::list<GUI*> GUI::fGuiList;
QFaustView::QFaustView(QWidget *parent)
    : QWidget{parent}
    ,dspUi{nullptr}
    ,codeEdit{new QTextEdit{this}}
    ,errorLabel{new QLabel{this}}
    ,layout{new QVBoxLayout{this}}
{
    this->setLayout(layout);
    layout->addWidget(codeEdit,5);
    sr = new QLineEdit(this);
    layout->addWidget(sr,1);
    sr->setValidator(new QIntValidator(1,1000000,this));
    sr->setText("44100");

    compile_button = new QPushButton(tr("valider"),this);
    layout->addWidget(compile_button,0);
    layout->addWidget(errorLabel,1);

    setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
    codeEdit->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
    errorLabel->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);

    codeEdit->setText(
        "import(\"filters.lib\");\n" \
        "fc = hslider(\"frequency\",100,100,20000,1);\n" \
        "q = hslider(\"quality\",1,0.1,10,0.1);\n" \
        "process=fi.resonlp(fc,q,1);\n");
    connect(compile_button,&QPushButton::clicked,this, &QFaustView::compile);
    connect(codeEdit,&QTextEdit::textChanged, this, [this](){compile_button->setDisabled(false);});
    connect(sr,&QLineEdit::textChanged, this, [this](){compile_button->setDisabled(false);});
}

void QFaustView::compile(){
    emit setFaustCode(codeEdit->toPlainText(),sr->text().toInt());
}

void QFaustView::setFile(QString path){
    QFile file(path);
    if(file.exists()){
        emit setFaustCode(path,sr->text().toInt());
        if(file.open(QIODevice::ReadOnly|QIODevice::Text)) {
            QTextStream in(&file);
            QString c = in.readAll();
            codeEdit->setText(c);
            codeEdit->setReadOnly(true);
        }
    }
}

void QFaustView::setDSPUI(QWidget * ui)
{
    if(dspUi == nullptr) {
        dspUi = ui;
    } else {
        layout->removeWidget(dspUi);
        delete dspUi;
        dspUi = ui;
        ui->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
    }
        layout->addWidget(dspUi,5);
    compile_button->setDisabled(true);
    errorLabel->setText("");
}

void QFaustView::setErrorMessage(QString s)
{
    compile_button->setDisabled(false);
    errorLabel->setText(s);
}


QFaustView * QBackendsView::addFaust(QString name){
    auto * f{new QFaustView};
    QScrollArea * s = new QScrollArea; 

    s->setWidget(f);
    s->setWidgetResizable(true); 
    addTab(s,name);
    fausts.push_back(f);
    setCurrentIndex(count()-1);
    return f;
}

#if ENABLE_JACK
QJackView * QBackendsView::addJack(){
    auto f = new QJackView;
    QScrollArea * s = new QScrollArea;
    s->setWidget(f);
    s->setWidgetResizable(true); 
    addTab(s,"jack"+QString::number(jacks.size()));
    jacks.push_back(f);
    return f;
}
#endif
