#ifndef SharcScorers_h
#define SharcScorers_h 1
/*****************************************************************************
 * Copyright (C) 2009-2013   this file is part of the NPTool Project         *
 *                                                                           *
 * For the licensing terms see $NPTOOL/Licence/NPTool_Licence                *
 * For the list of contributors see $NPTOOL/Licence/Contributors             *
 *****************************************************************************/

/*****************************************************************************
 * Original Author: Adrien MATTA  contact address: matta@ipno.in2p3.fr       *
 *                                                                           *
 * Creation Date  : February 2013                                            *
 * Last update    :                                                          *
 *---------------------------------------------------------------------------*
 * Decription:                                                               *
 *  File old the scorer specific to the Sharc Detector                       *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Comment:                                                                  *
 * This new style of scorer is aim to become the standard way of doing scorer*
 * in NPTool.                                                                *
 *The index is build using the TrackID, Detector Number and Strip Number.    *
 *The scorer Hold Energy and time together                                   *
 *Only one scorer is needed for a detector                                   *
 *****************************************************************************/
#include "G4VPrimitiveScorer.hh"
#include "G4THitsMap.hh"

#include <map>
using namespace std;

namespace SHARC {
  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
  
  class PS_Silicon_Rectangle : public G4VPrimitiveScorer{
    
  public: // with description
    PS_Silicon_Rectangle(G4String name, G4double StripPlaneLength, G4double StripPlaneWidth, G4int NumberOfStripLength,G4int NumberOfStripWidth,G4int depth=0);
     ~PS_Silicon_Rectangle();
    
  protected: // with description
     G4bool ProcessHits(G4Step*, G4TouchableHistory*);
    
  public:
    void Initialize(G4HCofThisEvent*);
    void EndOfEvent(G4HCofThisEvent*);
    void clear();
    void DrawAll();
    void PrintAll();
  
  private: // Threshold
    G4double m_TriggerThreshold;
    
  private: // Geometry of the detector
    G4double m_StripPlaneLength;
    G4double m_StripPlaneWidth;
    G4int    m_NumberOfStripLength;
    G4int    m_NumberOfStripWidth;
    G4double m_StripPitchLength;
    G4double m_StripPitchWidth;
    
  private: // inherited from G4VPrimitiveScorer
    G4int HCID;
    G4THitsMap<G4double*>* EvtMap;
    
  private: // Needed for intermediate calculation (avoid multiple instantiation in Processing Hit)
    G4ThreeVector m_Position  ;
    G4int m_DetectorNumber    ;
    G4int m_StripLengthNumber ;
    G4int m_StripWidthNumber  ;
    G4int m_Index             ;
    
  };
  
  class PS_Silicon_Annular : public G4VPrimitiveScorer{
    
  public: // with description
    PS_Silicon_Annular(G4String name, G4double StripPlaneInnerRadius, G4double StripPlaneOuterRadius, G4double PhiStart,G4double PhiStop, G4int NumberOfStripRadial,G4int NumberOfStripTheta,G4int depth=0);
    ~PS_Silicon_Annular();
    
  protected: // with description
    G4bool ProcessHits(G4Step*, G4TouchableHistory*);
    
  public:
    void Initialize(G4HCofThisEvent*);
    void EndOfEvent(G4HCofThisEvent*);
    void clear();
    void DrawAll();
    void PrintAll();
    
  private: // Threshold
    G4double m_TriggerThreshold;
    
  private: // Geometry of the detector
    G4double m_StripPlaneInnerRadius;
    G4double m_StripPlaneOuterRadius;
    G4double m_PhiStart;
    G4double m_PhiStop;
    G4int m_NumberOfStripRadial;
    G4int m_NumberOfStripTheta;
    G4double m_StripPitchRadial;
    G4double m_StripPitchTheta;
    
  private: // inherited from G4VPrimitiveScorer
    G4int HCID;
    G4THitsMap<G4double*>* EvtMap;

  private: // Needed for intermediate calculation (avoid multiple instantiation in Processing Hit)
    G4ThreeVector m_Position  ;
    G4ThreeVector m_uz        ;
    G4int m_DetectorNumber    ;
    G4int m_StripRadialNumber ;
    G4int m_StripThetaNumber  ;
    G4int m_Index             ;
    
  };
}

#endif
