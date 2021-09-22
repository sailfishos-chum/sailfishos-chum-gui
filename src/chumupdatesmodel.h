#pragma once

#include "chumabstractpackagemodel.h"

class ChumUpdatesModel : public ChumAbstractPackageModel {
  Q_OBJECT

public:
  explicit ChumUpdatesModel(QObject *parent = nullptr);

public slots:
  void reset() override;
};
