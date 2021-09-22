#pragma once

#include "chumabstractpackagemodel.h"

class ChumAvailablePackagesModel : public ChumAbstractPackageModel {
  Q_OBJECT

public:
  explicit ChumAvailablePackagesModel(QObject *parent = nullptr);

public slots:
  void reset() override;
};
