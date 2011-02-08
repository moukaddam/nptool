/*****************************************************************************
 * Copyright (C) 2009   this file is part of the NPTool Project              *
 *                                                                           *
 * For the licensing terms see $NPTOOL/Licence/NPTool_Licence                *
 * For the list of contributors see $NPTOOL/Licence/Contributors             *
 *****************************************************************************/

/*****************************************************************************
 * Original Author: N. de Sereville  contact address: deserevi@ipno.in2p3.fr *
 *                                                                           *
 * Creation Date  : 10/06/09                                                 *
 * Last update    :                                                          *
 *---------------------------------------------------------------------------*
 * Decription: This class is an Abstract Base Class (ABC) from which should  *
 *             derive all different modules from the Hyde tracker.        *
 *---------------------------------------------------------------------------*
 * Comment:                                                                  *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#ifndef HydeTrackerModule_h
#define HydeTrackerModule_h 1

// C++ headers
#include <string>
#include <vector>

// G4 headers
#include "G4LogicalVolume.hh"
#include "G4Event.hh"
#include "G4MultiFunctionalDetector.hh"

// NPTool - ROOT headers
#include "TInteractionCoordinates.h"
#include "THydeTrackerData.h"

using namespace std;



class HydeTrackerModule
{
public:
   HydeTrackerModule();
   virtual ~HydeTrackerModule();

public:
   // Read stream at Configfile to pick-up parameters of detector (Position,...)
   virtual void ReadConfiguration(string Path) = 0;

   // Construct detector and inialise sensitive part.
   virtual void ConstructDetector(G4LogicalVolume* world) = 0;

   // Read sensitive part of a the HydeTrackerModule detector and fill the Root tree.
   virtual void ReadSensitive(const G4Event* event) = 0;

   // Add Detector branch to the ROOT output tree
   virtual void InitializeRootOutput();

   // Initialize all scorers necessary for each detector
   virtual void InitializeScorers() = 0;

   // Give the static TInteractionCoordinates from VDetector to the classes
   // deriving from HydeTrackerModule
   // This is mandatory since the HydeTracker*** does not derive from VDetector
   virtual void SetInterCoordPointer(TInteractionCoordinates* interCoord) = 0;
   virtual TInteractionCoordinates* GetInterCoordPointer() = 0;

   // Initialize the Index map for the different modules of Hyde
   void InitializeIndex();

public:
   THydeTrackerData*      GetEventPointer()    {return ms_Event;};

protected:
   // Class to store the result of simulation
   static THydeTrackerData*      ms_Event;

   // Set to true if you want this stage on you telescope
   vector<bool>      m_wFirstStage;
   vector<bool>      m_wSecondStage;
   vector<bool>      m_wThirdStage;

   // Set to true if you want to see Telescope Frame in your visualisation
   bool              m_non_sensitive_part_visiualisation;

protected:
   // First stage Associate Scorer
   G4MultiFunctionalDetector* m_FirstStageScorer;

   // Second stage Associate Scorer
   G4MultiFunctionalDetector* m_SecondStageScorer;

   // Third stage Associate Scorer 
   G4MultiFunctionalDetector* m_ThirdStageScorer;

protected:
   map<string, int> m_index;
};

#endif