#pragma once

#include <qwidget.h>

#include "ui_pagea.h"

class PageA : public QWidget {
    Q_OBJECT

public:
    explicit PageA(QWidget *parent = nullptr);
    ~PageA() override;

private:
    Ui::PageA ui;

private slots:
    void on_update_cb_clicked();
    void on_update_thr_clicked();
};
