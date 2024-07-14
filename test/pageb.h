#pragma once

#include <qobject.h>

#include "ui_pageb.h"

class PageB : public QWidget {
    Q_OBJECT

public:
    explicit PageB(QWidget *parent = nullptr);
    ~PageB() override;

private:
    Ui::PageB ui;
};
