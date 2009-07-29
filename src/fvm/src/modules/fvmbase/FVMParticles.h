//
// C++ Interface: fvmparticles
//
// Description: 
//
//
// Author: yildirim,,, <yildirim@prism>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef FVMPARTICLES_H
#define FVMPARTICLES_H

#include "Mesh.h"
#include "Array.h"
#include <vector>
#include <set>

/**
	@author yildirim,,, <yildirim@prism>
*/
class FVMParticles{

    public:
       typedef   shared_ptr< Array<int> >     ArrayIntPtr;

       FVMParticles( const MeshList& meshList );
      ~FVMParticles();

       void   setParticles( int sweep );
       void   setSweepIter( int sweep) { _sweepIter = sweep; };

       const ArrayBase&  getCellIDs( int mesh_id ) const { return *_cellID.at(mesh_id); }
       int  getNumOfFluidParticles ( int mesh_id ) const { return _cellID.at(mesh_id)->getLength(); }

    private:

       const MeshList _meshList;
       int   _sweepIter;

       int _nmesh;
       vector< set<int> >  _cellIDSet;
       vector< ArrayIntPtr >  _cellID;
};

#endif
