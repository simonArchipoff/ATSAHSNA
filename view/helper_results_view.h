#pragma once

#include <qcustomplot.h>


class MyQCustomPlot : public QCustomPlot {
    Q_OBJECT
public:
    MyQCustomPlot(QWidget * parent=nullptr);
};

class ResultBase : public QWidget{
    Q_OBJECT
public:
    ResultBase(QString name, QWidget * parent);


    void setConfigureWidget(QWidget * w);


protected:
    void addWidget(QWidget * w);
    QPushButton * button;
private:
    void popWindow(bool b);
    QVBoxLayout * layout;
    QWidget * configureWidget;
};




// Fonction récursive pour supprimer les éléments à un indice donné
template <typename T, typename... Ts>
void erase_at(size_t index, std::vector<T>& first, std::vector<Ts>&... rest) {
    first.erase(first.begin() + index);
    (rest.erase(rest.begin() + index), ...);  // Expansion du pack pour supprimer dans tous les autres vecteurs
}

// Fonction principale
template <typename T, typename... Ts>
void removeInfElements(std::vector<T>& first, std::vector<Ts>&... rest) {
    std::vector<size_t> indices;

    // Trouver les indices où `first[i]` est `-inf`
    for (size_t i = 0; i < first.size(); ++i) {
        if (first[i] == -std::numeric_limits<T>::infinity()) {
            indices.push_back(i);
        }
    }

    // Supprimer les éléments en partant de la fin
    for (auto it = indices.rbegin(); it != indices.rend(); ++it) {
        erase_at(*it, first, rest...);
    }
}



