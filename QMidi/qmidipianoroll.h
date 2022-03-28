#ifndef QMIDIPIANOROLL_H
#define QMIDIPIANOROLL_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>

#include "qmidimessage.h"
class QMidiPianoRoll :
        public QGraphicsView
{
    Q_OBJECT
public:
    explicit QMidiPianoRoll(QWidget *parent = 0);
protected:
//    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
private:
    bool isSemiTone(int pitch);
private:
    QGraphicsScene *_scene;
    int _keyWidth;
    int _keyHeight;
    QVector<QGraphicsRectItem*> _keys;
signals:

public slots:
    void onMidiReceive(QMidiMessage *message);
};

#endif // QMIDIPIANOROLL_H
