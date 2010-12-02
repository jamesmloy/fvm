#ifndef _CAPTUREDISCRETIZATION_H_
#define _CAPTUREDISCRETIZATION_H_

#include "PhysicsConstant.h"
#include "ElectricBC.h"
#include "Field.h"
#include "MultiField.h"
#include "MultiFieldMatrix.h"
#include "Mesh.h"
#include "Discretization.h"
#include "StorageSite.h"
#include "DiagonalMatrix.h"
#include "CRMatrix.h"

/**************************************************
Diag type: 2x2Tensor
      | d00,  d01 |
      | d10,  d11 |    

OffDiag type: 2x2Tensor
      | o00,  o01 |
      | o10,  o11 |    

X type: VectorT2
      | x0 |
      | x1 |         

"0" is trapped charge
"1" is band charge

Capture model modifies all diag: d00, d01, d10, d11
and x0, x1

*************************************************/

template <class X, class Diag, class OffDiag>
class CaptureDiscretization : public Discretization
{

 public:
  typedef typename NumTypeTraits<X>::T_Scalar T_Scalar;
  typedef Array<T_Scalar> TArray;
  typedef CRMatrix<Diag,OffDiag,X> CCMatrix;
  typedef typename CCMatrix::DiagArray DiagArray;
  typedef typename CCMatrix::OffDiagArray OffDiagArray;
  typedef Array<X> XArray;

  CaptureDiscretization(const MeshList& meshes,
			const GeomFields& geomFields,
			const Field& varField,
			const Field& totaltrapsField,
			const Field& capturecrossField,
			const ElectricModelConstants<T_Scalar>& constants):
    Discretization(meshes),
    _geomFields(geomFields),
    _varField(varField),
    _totaltrapsField(totaltrapsField),
    _capturecrossField(capturecrossField),
    _constants(constants)
      {}

  void discretize(const Mesh& mesh, MultiFieldMatrix& mfmatrix,
                  MultiField& xField, MultiField& rField)
  {
    const StorageSite& cells = mesh.getCells();
    
    const int nCells = cells.getSelfCount();

    const TArray& cellVolume = dynamic_cast<const TArray&>(_geomFields.volume[cells]);

    const TArray& electron_totaltraps = dynamic_cast<const TArray&> (_totaltrapsField[cells]);

    const TArray& free_electron_capture_cross = dynamic_cast<const TArray&> (_capturecrossField[cells]);

    const MultiField::ArrayIndex cVarIndex(&_varField,&cells);
    
    CCMatrix& matrix = dynamic_cast<CCMatrix&>(mfmatrix.getMatrix(cVarIndex,cVarIndex));
    
    const XArray& xCell = dynamic_cast<const XArray&>(xField[cVarIndex]);

    XArray& rCell = dynamic_cast<XArray&>(rField[cVarIndex]);
       
    DiagArray& diag = matrix.getDiag();

    OffDiagArray& offdiag = matrix.getOffDiag();

    const T_Scalar& electron_effmass = _constants["electron_effmass"];

    const T_Scalar& temperature = _constants["OP_temperature"];
    
    const T_Scalar velocity = sqrt(8.0 * K_SI * temperature / (PI * ME * electron_effmass));
    
    for(int c=0; c<nCells; c++){
      
      T_Scalar fluxCoeff = cellVolume[c] * velocity * free_electron_capture_cross[c];
            
      rCell[c][0] += fluxCoeff * xCell[c][1] * (electron_totaltraps[c] - xCell[c][0]);
      
      diag[c](0,0) -= fluxCoeff * xCell[c][1];  

      diag[c](0,1) += fluxCoeff * (electron_totaltraps[c]-xCell[c][0]);
      
      rCell[c][1] -= fluxCoeff * xCell[c][1] * (electron_totaltraps[c] - xCell[c][0]); 

      diag[c](1,1) -= fluxCoeff * (electron_totaltraps[c]-xCell[c][0]);

      diag[c](1,0) += fluxCoeff * xCell[c][1];

    }    
  }

 private:
  const GeomFields& _geomFields;
  const Field& _varField;
  const Field& _totaltrapsField;
  const Field& _capturecrossField;
  const ElectricModelConstants<T_Scalar>& _constants;
};


#endif
