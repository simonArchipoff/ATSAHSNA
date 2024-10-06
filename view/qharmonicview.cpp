#include "qharmonicview.h"

THDPlot::THDPlot(QWidget* parent):FrequencyPlot(parent){
    displayPhase(false);
}

void THDPlot::setResult(const ResultHarmonics & r,QString name, QColor c){
        //auto name = QString{r.name.data()};
        setPlot(r.harmonicSpectrum, name, c);
        plots[name]->phaseDisplayed = false;
}


/*
void THDPlot::setResult(const std::vector<ResultHarmonics> &v){
        for(auto & r : v){
            setResult(r);
        }
        replot();
}*/


THDText::THDText(QWidget * parent):QTableWidget(parent){


    // Configurer le QTableWidget
    setRowCount(0);

    QStringList headers;
    headers << "Nom" << "THD (%)" << "THD+N (%)" << "freq Low" << "freq high";

    for (uint i = 1; i <= nb_harmonics; ++i) {
        headers << QString("H%1 (dB)").arg(i);
    }
    setColumnCount(headers.size());
    setHorizontalHeaderLabels(headers);


    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
}



int findRowByName(QTableWidget* tableWidget, const QString& name) {
    for (int row = 0; row < tableWidget->rowCount(); ++row) {
        QTableWidgetItem* item = tableWidget->item(row, 0); // 0 est la colonne "Nom"
        if (item && item->text() == name) {
            return row;
        }
    }
    return -1; // Retourne -1 si l'entrée n'est pas trouvée
}

void THDText::setResult(const ResultHarmonics &r, QString name, QColor c){
    int row = findRowByName(this, name);

    if (row == -1) {
        // Ajouter une nouvelle entrée si elle n'existe pas
        row = rowCount();
        insertRow(row);

        // Ajouter le nom de l'entrée
        setItem(row, 0, new QTableWidgetItem(name));
    }

    // Mettre à jour THD et THD+N
    uint i = 1;
    setItem(row, i++, new QTableWidgetItem(QString::number(r.thdRate * 100,'f',3)));
    setItem(row, i++, new QTableWidgetItem(QString::number(r.thdNoiseRate * 100,'f',3)));
    setItem(row, i++, new QTableWidgetItem(QString::number(r.params.freqMin)));
    setItem(row, i++, new QTableWidgetItem(QString::number(r.params.freqMax)));

    // Mettre à jour les niveaux d'harmoniques
    for (uint j=0; j < nb_harmonics && j < r.harmonicsLevel.size(); ++j) {
        setItem(row, i+j, new QTableWidgetItem(QString::number(20*log10(r.harmonicsLevel[j]),'f',3)));
    }
}

void THDText::removeResult(QString s)
{
    int row = findRowByName(this, s);
    if (row != -1) {
        removeRow(row); // Supprimer la ligne
    }
}




QHarmonicView::QHarmonicView(QWidget * parent)
    :QWidget(parent)
    ,plot(new THDPlot(this))
    ,text(new THDText(this))
    ,measures(new QMeasuresView(this)){
    QHBoxLayout * hLayout = new QHBoxLayout;
    QVBoxLayout* vLayout = new QVBoxLayout;

    setLayout(hLayout);
    vLayout->addWidget(plot);
    vLayout->addWidget(text);
    hLayout->addWidget(measures);
    hLayout->addLayout(vLayout);
    vLayout->setSizeConstraint(QLayout::SetMaximumSize);

    connect(measures,&QMeasuresView::remove, this, & QHarmonicView::removeResult);
}

void QHarmonicView::setResults(const std::variant<const std::vector<ResultHarmonics>, ErrorBackend> &r){
    auto  v = std::get_if<const std::vector<ResultHarmonics>>(&r);
    for(uint i = 0; v && i < v->size(); i++){
        setResult((*v)[i]);
    }
    plot->replot();
}

void QHarmonicView::setResult(const ResultHarmonics &r){
    auto m = measures->addResult(r);
    if(m){
        plot->setResult(m->getResult(), m->getName(), m->getColor());
        text->setResult(m->getResult(), m->getName(), m->getColor());
        //bodePlot->setPlot(m->getResult().response, m->getName(), m->getColor());
        //spectrogramPlot->setResult(m->getResult(), m->getName());
        connect(m, &QResultHarmonicsView::remove,this,&QHarmonicView::removeResult);
    } else {
        plot->updatePlot(m->getResult(),m->getName());
        //temporalPlot->updatePlot(m->getResult().response,m->getName());
        //bodePlot->updatePlot(m->getResult().response,m->getName());
        //spectrogramPlot->setResult(m->getResult(),m->getName());
    }
   // plot->setResult(r);
}

void QHarmonicView::removeResult(QString name){
    plot->removeResult(name);
    text->removeResult(name);
    this->measures->removeResult(name);
}
