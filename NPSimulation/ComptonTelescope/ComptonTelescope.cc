/*****************************************************************************
 * Copyright (C) 2009-2013   this file is part of the NPTool Project         *
 *                                                                           *
 * For the licensing terms see $NPTOOL/Licence/NPTool_Licence                *
 * For the list of contributors see $NPTOOL/Licence/Contributors             *
 *****************************************************************************/

/*****************************************************************************
 * Original Author: N. de Sereville  contact address: deserevi@ipno.in2p3.fr *
 *                                                                           *
 * Creation Date  : 10/06/09                                                 *
 * Last update    : 12/10/09                                                 *
 *---------------------------------------------------------------------------*
 * Decription: Define a module of square shape for the Gaspard tracker       *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Comment:                                                                  *
 *    + 07/09/09: Fix bug for placing module with (r,theta,phi) method.      *
 *                (N. de Sereville)                                          *
 *    + 12/10/09: Change scorer scheme (N. de Sereville)                     *
 *    + 01/10/10: Fix bug with TInteractionCoordinate map size in Read       *
 *                Sensitive (N. de Sereville)                                *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

// C++ headers
#include <sstream>
#include <string>
#include <cmath>

// ROOT headers
#include "TString.h"

// G4 Geometry headers
#include "G4Trd.hh"
#include "G4Box.hh"
#include "G4Trap.hh"

// G4 various headers
#include "G4MaterialTable.hh"
#include "G4Element.hh"
#include "G4ElementTable.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"
#include "G4PVPlacement.hh"
#include "G4PVDivision.hh"

// NPTool headers
#include "ComptonTelescope.hh"
#include "ComptonTelescopeScorers.hh"
#include "GeneralScorers.hh"
#include "RootOutput.h"

// CLHEP
#include "CLHEP/Random/RandGauss.h"

using namespace std;
using namespace CLHEP;

using namespace COMPTONTELESCOPE;




//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
ComptonTelescope::ComptonTelescope() 
   : m_Event(new TComptonTelescopeData()),
     m_ProcessEvent(new TComptonTelescopeProcessData()),
     m_NumberOfDSSSD(1),
     m_SizeOfDSSSD(100),
     m_NumberOfStrips(50),
     m_ThicknessOfDSSSD(2),
     m_DistanceInterDSSSD(7),
     m_ThicknessOfCalorimeter(50),
     m_DistanceTrackerCalorimeter(7)

{
   // calculate tower height
   m_TowerHeight =  (m_NumberOfDSSSD-1) * m_DistanceInterDSSSD + m_ThicknessOfDSSSD
                   + m_DistanceTrackerCalorimeter + m_ThicknessOfCalorimeter;
   
   // initialize materials
   InitializeMaterial();
}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
ComptonTelescope::~ComptonTelescope()
{
   // Materials
   delete m_MaterialSilicon;
   delete m_MaterialAluminium;
   delete m_MaterialLaBr3;
   delete m_MaterialVacuum;

   // Data
   delete m_Event;

   // Scorers
   delete m_TrackerScorer;
   delete m_CalorimeterScorer;
}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void ComptonTelescope::AddModule(G4ThreeVector X1_Y1     ,
                                     G4ThreeVector X128_Y1   ,
                                     G4ThreeVector X1_Y128   ,
                                     G4ThreeVector X128_Y128 ,
                                     bool wTracker        ,
                                     bool wCalorimeter)
{
   m_DefinitionType.push_back(true) ;

   m_X1_Y1.push_back(X1_Y1)               ;
   m_X128_Y1.push_back(X128_Y1)           ;
   m_X1_Y128.push_back(X1_Y128)           ;
   m_X128_Y128.push_back(X128_Y128)       ;
   m_wTracker.push_back(wTracker)   ;
   m_wCalorimeter.push_back(wCalorimeter)   ;

   m_R.push_back(0)      ;
   m_Theta.push_back(0)  ;
   m_Phi.push_back(0)    ;
   m_beta_u.push_back(0) ;
   m_beta_v.push_back(0) ;
   m_beta_w.push_back(0) ;
}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void ComptonTelescope::AddModule(G4double R        ,
                                     G4double Theta    ,
                                     G4double Phi      ,
                                     G4double beta_u   ,
                                     G4double beta_v   ,
                                     G4double beta_w   ,
                                     bool wTracker  ,
                                     bool wCalorimeter)
{
   G4ThreeVector empty = G4ThreeVector(0, 0, 0);

   m_DefinitionType.push_back(false);

   m_R.push_back(R)                       ;
   m_Theta.push_back(Theta)               ;
   m_Phi.push_back(Phi)                   ;
   m_beta_u.push_back(beta_u)             ;
   m_beta_v.push_back(beta_v)             ;
   m_beta_w.push_back(beta_w)             ;
   m_wTracker.push_back(wTracker)   ;
   m_wCalorimeter.push_back(wCalorimeter)   ;

   m_X1_Y1.push_back(empty)     ;
   m_X128_Y1.push_back(empty)   ;
   m_X1_Y128.push_back(empty)   ;
   m_X128_Y128.push_back(empty) ;
}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void ComptonTelescope::VolumeMaker(G4int TelescopeNumber,
                                   G4ThreeVector CTpos,
                                   G4RotationMatrix* CTrot,
                                   bool wTracker,
                                   bool wCalorimeter,
                                   G4LogicalVolume* world)
{
   G4double NbrTelescopes = TelescopeNumber  ;
   G4String DetectorNumber                   ;
   ostringstream Number                      ;
   Number << NbrTelescopes                   ;
   DetectorNumber = Number.str()             ;

   ////////////////////////////////////////////////////////////////
   ////////////// Starting Volume Definition //////////////////////
   ////////////////////////////////////////////////////////////////
   G4String Name = "ComptonTelescopeTower" + DetectorNumber;

   // calculate tower height
   m_TowerHeight =  (m_NumberOfDSSSD-1) * m_DistanceInterDSSSD + m_ThicknessOfDSSSD
                   + m_DistanceTrackerCalorimeter + m_ThicknessOfCalorimeter;
   
   G4Box*           solidComptonTelescope = new G4Box(Name, 0.5*m_SizeOfDSSSD, 0.5*m_SizeOfDSSSD, 0.5*m_TowerHeight);
   G4LogicalVolume* logicComptonTelescope = new G4LogicalVolume(solidComptonTelescope, m_MaterialVacuum, Name, 0, 0, 0);

   new G4PVPlacement(G4Transform3D(*CTrot, CTpos), logicComptonTelescope, Name, world, false, 0);

   logicComptonTelescope->SetVisAttributes(G4VisAttributes::Invisible);
   if (m_non_sensitive_part_visiualisation) logicComptonTelescope->SetVisAttributes(G4VisAttributes(G4Colour(0.90, 0.90, 0.90)));


   ////////////////////////////////////////////////////////////
   //////////////// Tracker Construction //////////////////////
   ////////////////////////////////////////////////////////////
   if (wTracker) {
      // tracker detector itself
      G4Box*           solidTracker = new G4Box("solidTracker", 0.5*m_SizeOfDSSSD, 0.5*m_SizeOfDSSSD, 0.5*m_ThicknessOfDSSSD);
      G4LogicalVolume* logicTracker = new G4LogicalVolume(solidTracker, m_MaterialSilicon, "logicTracker", 0, 0, 0);

      G4ThreeVector positionTracker;
      for (G4int i = 0; i < m_NumberOfDSSSD; ++i) {   // loop on number of DSSSDs
         G4double position = -m_TowerHeight/2 + m_ThicknessOfDSSSD/2 + i*m_DistanceInterDSSSD;
         positionTracker = G4ThreeVector(0, 0, position);
         G4String nameTracker = Name + "_Tracker";
         ostringstream index;
         index << i;
         nameTracker = nameTracker + index.str();
         new G4PVPlacement(0, positionTracker, logicTracker, nameTracker, logicComptonTelescope, false, i);
      } // end loop on number of DSSSDs

      // set tracker sensible
      logicTracker->SetSensitiveDetector(m_TrackerScorer);

      // visualisation of tracker
      G4VisAttributes* TrackerVisAtt = new G4VisAttributes(G4Colour(0.0, 0.0, 0.9));   // blue
      logicTracker->SetVisAttributes(TrackerVisAtt);
   }


   ////////////////////////////////////////////////////////////////
   ///////////////// Calorimeter Construction /////////////////////
   ////////////////////////////////////////////////////////////////
   if (wCalorimeter) {
      // calorimeter detector
      G4Box*           solidCalorimeter = new G4Box("solidCalorimeter", 0.5*m_SizeOfDSSSD, 0.5*m_SizeOfDSSSD, 0.5*m_ThicknessOfCalorimeter);
      G4LogicalVolume* logicCalorimeter = new G4LogicalVolume(solidCalorimeter, m_MaterialSilicon, "logicCalorimeter", 0, 0, 0);

      G4double position = m_TowerHeight/2 - m_ThicknessOfCalorimeter/2; 
      G4ThreeVector positionCalorimeter = G4ThreeVector(0, 0, position);
      new G4PVPlacement(0, positionCalorimeter, logicCalorimeter, Name + "_Calorimeter", logicComptonTelescope, false, 0);

      // Set Third Stage sensible
      logicCalorimeter->SetSensitiveDetector(m_CalorimeterScorer);

      ///Visualisation of Third Stage
      G4VisAttributes* CalorimeterVisAtt = new G4VisAttributes(G4Colour(0.0, 0.9, 0.0));   // green
      logicCalorimeter->SetVisAttributes(CalorimeterVisAtt);
   }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// Virtual Method of VDetector class

// Read stream at Configfile to pick-up parameters of detector (Position,...)
// Called in DetecorConstruction::ReadDetextorConfiguration Method
void ComptonTelescope::ReadConfiguration(string Path)
{
   ifstream ConfigFile           ;
   ConfigFile.open(Path.c_str()) ;
   string LineBuffer          ;
   string DataBuffer          ;

   // A:X1_Y1     --> X:1    Y:1
   // B:X128_Y1   --> X:128  Y:1
   // C:X1_Y128   --> X:1    Y:128
   // D:X128_Y128    --> X:128  Y:128

   G4double Ax , Bx , Cx , Dx , Ay , By , Cy , Dy , Az , Bz , Cz , Dz          ;
   G4ThreeVector A , B , C , D                                                 ;
   G4double Theta = 0 , Phi = 0 , R = 0 , beta_u = 0 , beta_v = 0 , beta_w = 0 ;

   G4int TRACKER = 0, CALORIMETER = 0;

   bool ReadingStatus = false ;

   bool check_A = false ;
   bool check_C = false ;
   bool check_B = false ;
   bool check_D = false ;

   bool check_Theta = false ;
   bool check_Phi   = false ;
   bool check_R     = false ;
   
   while (!ConfigFile.eof()) {
      getline(ConfigFile, LineBuffer);
      if (LineBuffer.compare(0, 16, "ComptonTelescope") == 0) {
         G4cout << "///" << G4endl           ;
         G4cout << "ComptonTelescope found: " << G4endl   ;
         ReadingStatus = true ;
      }
      else ReadingStatus = false;

      while (ReadingStatus) {      

         ConfigFile >> DataBuffer;

         //   Comment Line 
         if (DataBuffer.compare(0, 1, "%") == 0) {
            ConfigFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
         }

         // Finding another telescope (safety), toggle out
         else if (DataBuffer.compare(0, 16, "ComptonTelescope") == 0) {
//            cout << "WARNING: Another Detector is find before standard sequence of Token, Error may occured in Telecope definition" << endl;
//            ReadingStatus = false;
         }

         // Position method
         else if (DataBuffer.compare(0, 6, "X1_Y1=") == 0) {
            check_A = true;
            ConfigFile >> DataBuffer ;
            Ax = atof(DataBuffer.c_str()) ;
            Ax = Ax * mm ;
            ConfigFile >> DataBuffer ;
            Ay = atof(DataBuffer.c_str()) ;
            Ay = Ay * mm ;
            ConfigFile >> DataBuffer ;
            Az = atof(DataBuffer.c_str()) ;
            Az = Az * mm ;

            A = G4ThreeVector(Ax, Ay, Az);
            cout << "X1 Y1 corner position : " << A << endl;
         }

         else if (DataBuffer.compare(0, 8, "X128_Y1=") == 0) {
            check_B = true;
            ConfigFile >> DataBuffer ;
            Bx = atof(DataBuffer.c_str()) ;
            Bx = Bx * mm ;
            ConfigFile >> DataBuffer ;
            By = atof(DataBuffer.c_str()) ;
            By = By * mm ;
            ConfigFile >> DataBuffer ;
            Bz = atof(DataBuffer.c_str()) ;
            Bz = Bz * mm ;

            B = G4ThreeVector(Bx, By, Bz);
            cout << "X128 Y1 corner position : " << B << endl;
         }

         else if (DataBuffer.compare(0, 8, "X1_Y128=") == 0) {
            check_C = true;
            ConfigFile >> DataBuffer ;
            Cx = atof(DataBuffer.c_str()) ;
            Cx = Cx * mm ;
            ConfigFile >> DataBuffer ;
            Cy = atof(DataBuffer.c_str()) ;
            Cy = Cy * mm ;
            ConfigFile >> DataBuffer ;
            Cz = atof(DataBuffer.c_str()) ;
            Cz = Cz * mm ;

            C = G4ThreeVector(Cx, Cy, Cz);
            cout << "X1 Y128 corner position : " << C << endl;
         }

         else if (DataBuffer.compare(0, 10, "X128_Y128=") == 0) {
            check_D = true;
            ConfigFile >> DataBuffer ;
            Dx = atof(DataBuffer.c_str()) ;
            Dx = Dx * mm ;
            ConfigFile >> DataBuffer ;
            Dy = atof(DataBuffer.c_str()) ;
            Dy = Dy * mm ;
            ConfigFile >> DataBuffer ;
            Dz = atof(DataBuffer.c_str()) ;
            Dz = Dz * mm ;

            D = G4ThreeVector(Dx, Dy, Dz);
            cout << "X128 Y128 corner position : " << D << endl;
         }

         // Angle method
         else if (DataBuffer.compare(0, 2, "R=") == 0) {
            check_R = true;
            ConfigFile >> DataBuffer ;
            R = atof(DataBuffer.c_str()) ;
            R = R * mm;
            cout << "R:  " << R / mm << endl;
         }

         else if (DataBuffer.compare(0, 6, "THETA=") == 0) {
            check_Theta = true;
            ConfigFile >> DataBuffer ;
            Theta = atof(DataBuffer.c_str()) ;
            Theta = Theta * deg;
            cout << "Theta:  " << Theta / deg << endl;
         }

         else if (DataBuffer.compare(0, 4, "PHI=") == 0) {
            check_Phi = true;
            ConfigFile >> DataBuffer ;
            Phi = atof(DataBuffer.c_str()) ;
            Phi = Phi * deg;
            cout << "Phi:  " << Phi / deg << endl;
         }

         else if (DataBuffer.compare(0, 5, "BETA=") == 0) {
            ConfigFile >> DataBuffer ;
            beta_u = atof(DataBuffer.c_str()) ;
            beta_u = beta_u * deg   ;
            ConfigFile >> DataBuffer ;
            beta_v = atof(DataBuffer.c_str()) ;
            beta_v = beta_v * deg   ;
            ConfigFile >> DataBuffer ;
            beta_w = atof(DataBuffer.c_str()) ;
            beta_w = beta_w * deg   ;
            G4cout << "Beta:  " << beta_u / deg << " " << beta_v / deg << " " << beta_w / deg << G4endl  ;
         }

         else if (DataBuffer.compare(0, 11, "SIZE_DSSSD=") == 0) {
            ConfigFile >> DataBuffer;
            m_SizeOfDSSSD = atof(DataBuffer.c_str());
            m_SizeOfDSSSD = m_SizeOfDSSSD * mm;
            G4cout << "Size DSSSD: " << m_SizeOfDSSSD / mm << G4endl;
         }

         else if (DataBuffer.compare(0, 13, "NUMBER_DSSSD=") == 0) {
            ConfigFile >> DataBuffer;
            m_NumberOfDSSSD = atoi(DataBuffer.c_str());
            G4cout << "Number of DSSSD: " << m_NumberOfDSSSD << G4endl;
         }

         else if (DataBuffer.compare(0, 21, "DISTANCE_INTER_DSSSD=") == 0) {
            ConfigFile >> DataBuffer;
            m_DistanceInterDSSSD = atof(DataBuffer.c_str());
            m_DistanceInterDSSSD = m_DistanceInterDSSSD * mm;
            G4cout << "Distance Inter DSSSD: " << m_DistanceInterDSSSD / mm << G4endl;
         }

         else if (DataBuffer.compare(0, 16, "THICKNESS_DSSSD=") == 0) {
            ConfigFile >> DataBuffer;
            m_ThicknessOfDSSSD = atof(DataBuffer.c_str());
            m_ThicknessOfDSSSD = m_ThicknessOfDSSSD * mm;
            G4cout << "Thickness DSSSD: " << m_ThicknessOfDSSSD / mm << G4endl;
         }

         else if (DataBuffer.compare(0, 14, "NUMBER_STRIPS=") == 0) {
            ConfigFile >> DataBuffer;
            m_NumberOfStrips = atoi(DataBuffer.c_str());
            G4cout << "Number of STRIPS: " << m_NumberOfStrips << G4endl;
         }

         else if (DataBuffer.compare(0, 29, "DISTANCE_TRACKER_CALORIMETER=") == 0) {
            ConfigFile >> DataBuffer;
            m_DistanceTrackerCalorimeter = atof(DataBuffer.c_str());
            m_DistanceTrackerCalorimeter = m_DistanceTrackerCalorimeter * mm;
            G4cout << "Distance Tracker Calorimeter: " << m_DistanceTrackerCalorimeter / mm << G4endl;
         }

         else if (DataBuffer.compare(0, 22, "THICKNESS_CALORIMETER=") == 0) {
            ConfigFile >> DataBuffer;
            m_ThicknessOfCalorimeter = atof(DataBuffer.c_str());
            m_ThicknessOfCalorimeter = m_ThicknessOfCalorimeter * mm;
            G4cout << "Thickness Calorimeter: " << m_ThicknessOfCalorimeter / mm << G4endl;
         }

         else if (DataBuffer.compare(0, 8, "TRACKER=") == 0) {
            ConfigFile >> DataBuffer;
            TRACKER = atof(DataBuffer.c_str()) ;
            G4cout << "Tracker: " << TRACKER << G4endl;
         }

         else if (DataBuffer.compare(0, 12, "CALORIMETER=") == 0) {
            ConfigFile >> DataBuffer;
            CALORIMETER = atof(DataBuffer.c_str()) ;
            G4cout << "Calorimeter: " << CALORIMETER << G4endl;
         }

         else if (DataBuffer.compare(0, 4, "VIS=") == 0) {
            ConfigFile >> DataBuffer;
            if (DataBuffer.compare(0, 3, "all") == 0) m_non_sensitive_part_visiualisation = true;
         }

         else {
            ReadingStatus = false; 
            G4cout << "other token " << DataBuffer << G4endl;
            G4cout << "WARNING: Wrong Token, ComptonTelescope not added" << G4endl;
         }

         //Add The previously define telescope
         //With position method
         if ((check_A && check_B && check_C && check_D) && !(check_Theta && check_Phi && check_R)) {
            ReadingStatus = false ;
            check_A = false ;
            check_C = false ;
            check_B = false ;
            check_D = false ;

            AddModule(A                ,
                  B                ,
                  C                ,
                  D                ,
                  TRACKER  == 1 ,
                  CALORIMETER  == 1);
         }

         //with angle method
         if ((check_Theta && check_Phi && check_R) && !(check_A && check_B && check_C && check_D)) {
            ReadingStatus = false ;
            check_Theta = false ;
            check_Phi   = false ;
            check_R     = false ;

            AddModule(R                ,
                  Theta            ,
                  Phi              ,
                  beta_u           ,
                  beta_v           ,
                  beta_w           ,
                  TRACKER  == 1 ,
                  CALORIMETER  == 1);
         }

      }
   }
}

// Construct detector and inialise sensitive part.
// Called After DetecorConstruction::AddDetector Method
void ComptonTelescope::ConstructDetector(G4LogicalVolume* world)
{
   G4RotationMatrix* CTrot    = NULL                   ;
   G4ThreeVector     CTpos    = G4ThreeVector(0, 0, 0) ;
   G4ThreeVector     CTu      = G4ThreeVector(0, 0, 0) ;
   G4ThreeVector     CTv      = G4ThreeVector(0, 0, 0) ;
   G4ThreeVector     CTw      = G4ThreeVector(0, 0, 0) ;
   G4ThreeVector     CTCenter = G4ThreeVector(0, 0, 0) ;
   bool FirstStage  = true;
   bool SecondStage = true;

   G4int NumberOfTelescope = m_DefinitionType.size() ;

   for (G4int i = 0; i < NumberOfTelescope; i++) {
      // By Point
      if (m_DefinitionType[i]) {
         // (u,v,w) unitary vector associated to telescope referencial
         // (u,v) // to silicon plan
         // w perpendicular to (u,v) plan and pointing ThirdStage
         CTu = m_X128_Y1[i] - m_X1_Y1[i];
         CTu = CTu.unit();

         CTv = m_X1_Y128[i] - m_X1_Y1[i];
         CTv = CTv.unit();

         CTw = CTu.cross(CTv);
         CTw = CTw.unit();

         CTCenter = (m_X1_Y1[i] + m_X1_Y128[i] + m_X128_Y1[i] + m_X128_Y128[i]) / 4;

         // Passage Matrix from Lab Referential to Telescope Referential
         CTrot = new G4RotationMatrix(CTu, CTv, CTw);
         // translation to place Telescope
         CTpos = CTw * m_TowerHeight * 0.5 + CTCenter;
      }

      // By Angle
      else {
         G4double Theta = m_Theta[i] ;
         G4double Phi   = m_Phi[i]   ;

         // (u,v,w) unitary vector associated to telescope referencial
         // (u,v) // to silicon plan
         // w perpendicular to (u,v) plan and pointing ThirdStage
         // Phi is angle between X axis and projection in (X,Y) plan
         // Theta is angle between  position vector and z axis
         G4double wX = m_R[i] * sin(Theta / rad) * cos(Phi / rad);
         G4double wY = m_R[i] * sin(Theta / rad) * sin(Phi / rad);
         G4double wZ = m_R[i] * cos(Theta / rad);
         CTw = G4ThreeVector(wX, wY, wZ);

         // vector corresponding to the center of the module
         CTCenter = CTw;

         // vector parallel to one axis of silicon plane
         G4double ii = cos(Theta / rad) * cos(Phi / rad);
         G4double jj = cos(Theta / rad) * sin(Phi / rad);
         G4double kk = -sin(Theta / rad);
         G4ThreeVector Y = G4ThreeVector(ii, jj, kk);

         CTw = CTw.unit();
         CTu = CTw.cross(Y);
         CTv = CTw.cross(CTu);
         CTv = CTv.unit();
         CTu = CTu.unit();

         // Passage Matrix from Lab Referential to Telescope Referential
         // MUST2
         CTrot = new G4RotationMatrix(CTu, CTv, CTw);
         // Telescope is rotate of Beta angle around CTv axis.
         CTrot->rotate(m_beta_u[i], CTu);
         CTrot->rotate(m_beta_v[i], CTv);
         CTrot->rotate(m_beta_w[i], CTw);
         // translation to place Telescope
         CTpos = CTw * m_TowerHeight * 0.5 + CTCenter;
      }

      FirstStage  = m_wTracker[i]  ;
      SecondStage = m_wCalorimeter[i] ;

      VolumeMaker(i + 1, CTpos, CTrot, FirstStage, SecondStage, world);
   }

   delete CTrot ;
}



// Connect the GaspardTrackingData class to the output TTree
// of the simulation
void ComptonTelescope::InitializeRootOutput()
{
   RootOutput *pAnalysis = RootOutput::getInstance();
   TTree *pTree = pAnalysis->GetTree();
   pTree->Branch("ComptonTelescope",        "TComptonTelescopeData",        &m_Event);
   pTree->Branch("ComptonTelescopeProcess", "TComptonTelescopeProcessData", &m_ProcessEvent);
}



// Read sensitive part and fill the Root tree.
// Called at in the EventAction::EndOfEventAvtion
void ComptonTelescope::ReadSensitive(const G4Event* event)
{
   m_Event->Clear();

   //////////////////////////////////////////////////////////////////////////////////////
   //////////////////////// Used to Read Event Map of detector //////////////////////////
   //////////////////////////////////////////////////////////////////////////////////////
   // Tracker
   std::map<G4int, G4int*>::iterator    TowerNumber_itr;
   std::map<G4int, G4int*>::iterator    DSSSDNumber_itr;
   std::map<G4int, G4double*>::iterator Energy_itr;
   std::map<G4int, G4double*>::iterator Time_itr;
/*   std::map<G4int, G4int*>::iterator    X_itr;
   std::map<G4int, G4int*>::iterator    Y_itr;
   std::map<G4int, G4double*>::iterator Pos_X_itr;
   std::map<G4int, G4double*>::iterator Pos_Y_itr;
   std::map<G4int, G4double*>::iterator Pos_Z_itr;
   std::map<G4int, G4double*>::iterator Ang_Theta_itr;
   std::map<G4int, G4double*>::iterator Ang_Phi_itr;
*/
   G4THitsMap<G4int>*    TowerNumberHitMap;
   G4THitsMap<G4int>*    DSSSDNumberHitMap;
   G4THitsMap<G4double>* EnergyHitMap;
   G4THitsMap<G4double>* TimeHitMap;
/*   G4THitsMap<G4int>*    XHitMap;
   G4THitsMap<G4int>*    YHitMap;
   G4THitsMap<G4double>* PosXHitMap;
   G4THitsMap<G4double>* PosYHitMap;
   G4THitsMap<G4double>* PosZHitMap;
   G4THitsMap<G4double>* AngThetaHitMap;
   G4THitsMap<G4double>* AngPhiHitMap;
*/
   // Calorimeter
   std::map<G4int, G4int*>::iterator    CalorimeterTowerNumber_itr;
   std::map<G4int, G4int*>::iterator    CalorimeterDetectorNumber_itr;
   std::map<G4int, G4double*>::iterator CalorimeterEnergy_itr;
//   std::map<G4int, G4double*>::iterator CalorimeterTime_itr;
   G4THitsMap<G4int>*    CalorimeterTowerNumberHitMap;
   G4THitsMap<G4int>*    CalorimeterDetectorNumberHitMap;
   G4THitsMap<G4double>* CalorimeterEnergyHitMap;
//   G4THitsMap<G4double>* CalorimeterTimeHitMap;


   // Read scorers associated to the tracker part
   // tower number
   G4int TrackTowCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("TrackerScorerComptonTelescope/TowerNumber");
   TowerNumberHitMap = (G4THitsMap<G4int>*)(event->GetHCofThisEvent()->GetHC(TrackTowCollectionID));
   TowerNumber_itr = TowerNumberHitMap->GetMap()->begin();

   // detector number
   G4int TrackDetCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("TrackerScorerComptonTelescope/DSSSDNumber");
   DSSSDNumberHitMap = (G4THitsMap<G4int>*)(event->GetHCofThisEvent()->GetHC(TrackDetCollectionID));
   DSSSDNumber_itr = DSSSDNumberHitMap->GetMap()->begin();

   // energy
   G4int TrackEnergyCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("TrackerScorerComptonTelescope/Energy");
   EnergyHitMap = (G4THitsMap<G4double>*)(event->GetHCofThisEvent()->GetHC(TrackEnergyCollectionID))                    ;
   Energy_itr = EnergyHitMap->GetMap()->begin();

   // time of flight
   G4int TrackTimeCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("TrackerScorerComptonTelescope/Time");
   TimeHitMap = (G4THitsMap<G4double>*)(event->GetHCofThisEvent()->GetHC(TrackTimeCollectionID));
   Time_itr = TimeHitMap->GetMap()->begin();
/*
   //Track Number X
   G4int TrackXCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("TrackerScorerComptonTelescope/NumberX");
   XHitMap = (G4THitsMap<G4int>*)(event->GetHCofThisEvent()->GetHC(TrackXCollectionID));
   X_itr = XHitMap->GetMap()->begin();

   //Track Number Y
   G4int TrackYCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("TrackerScorerComptonTelescope/TrackNumberY")    ;
   YHitMap = (G4THitsMap<G4int>*)(event->GetHCofThisEvent()->GetHC(TrackYCollectionID));
   Y_itr = YHitMap->GetMap()->begin();

   //Interaction Coordinate X
   G4int InterCoordXCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("TrackerScorerComptonTelescope/InterCoordX")    ;
   PosXHitMap = (G4THitsMap<G4double>*)(event->GetHCofThisEvent()->GetHC(InterCoordXCollectionID))                              ;
   Pos_X_itr = PosXHitMap->GetMap()->begin()                                                                    ;

   //Interaction Coordinate Y
   G4int InterCoordYCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("TrackerScorerComptonTelescope/InterCoordY")    ;
   PosYHitMap = (G4THitsMap<G4double>*)(event->GetHCofThisEvent()->GetHC(InterCoordYCollectionID))                              ;
   Pos_Y_itr = PosYHitMap->GetMap()->begin()                                                                    ;

   //Interaction Coordinate Z
   G4int InterCoordZCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("TrackerScorerComptonTelescope/InterCoordZ")    ;
   PosZHitMap = (G4THitsMap<G4double>*)(event->GetHCofThisEvent()->GetHC(InterCoordZCollectionID))                              ;
   Pos_Z_itr = PosXHitMap->GetMap()->begin()                                                                    ;

   //Interaction Coordinate Angle Theta
   G4int InterCoordAngThetaCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("TrackerScorerComptonTelescope/InterCoordAngTheta")    ;
   AngThetaHitMap = (G4THitsMap<G4double>*)(event->GetHCofThisEvent()->GetHC(InterCoordAngThetaCollectionID))                              ;
   Ang_Theta_itr = AngThetaHitMap->GetMap()->begin()                                                                    ;

   //Interaction Coordinate Angle Phi
   G4int InterCoordAngPhiCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("TrackerScorerComptonTelescope/InterCoordAngPhi")    ;
   AngPhiHitMap = (G4THitsMap<G4double>*)(event->GetHCofThisEvent()->GetHC(InterCoordAngPhiCollectionID))                              ;
   Ang_Phi_itr = AngPhiHitMap->GetMap()->begin()                                                                    ;
*/

   // read the scorers associated to the calorimeter
   // tower number
   G4int CaloTowCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("CalorimeterScorerComptonTelescope/TowerNumber");
   CalorimeterTowerNumberHitMap = (G4THitsMap<G4int>*)(event->GetHCofThisEvent()->GetHC(CaloTowCollectionID));
   CalorimeterTowerNumber_itr = CalorimeterTowerNumberHitMap->GetMap()->begin();

   // detector number
   G4int CaloDetCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("CalorimeterScorerComptonTelescope/DetectorNumber");
   CalorimeterDetectorNumberHitMap = (G4THitsMap<G4int>*)(event->GetHCofThisEvent()->GetHC(CaloDetCollectionID));
   CalorimeterDetectorNumber_itr = CalorimeterDetectorNumberHitMap->GetMap()->begin();

   // energy
   G4int CaloEnergyCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("CalorimeterScorerComptonTelescope/Energy");
   CalorimeterEnergyHitMap = (G4THitsMap<G4double>*)(event->GetHCofThisEvent()->GetHC(CaloEnergyCollectionID))                    ;
   CalorimeterEnergy_itr = CalorimeterEnergyHitMap->GetMap()->begin();
/*
   // time of flight
   G4int CaloTimeCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("CalorimeterScorerComptonTelescope/Time");
   CalorimeterTimeHitMap = (G4THitsMap<G4double>*)(event->GetHCofThisEvent()->GetHC(CaloTimeCollectionID));
   CalorimeterTime_itr = CalorimeterTimeHitMap->GetMap()->begin();
*/

   // Calorimeter
//   G4int sizeCaloTower = ->entries();

   // Check the size of different map
   G4int sizeTrackerNtower = TowerNumberHitMap->entries();
   G4int sizeTrackerNdsssd = DSSSDNumberHitMap->entries(); sizeTrackerNdsssd *= 1;
   G4int sizeTrackerE      = EnergyHitMap->entries();
   G4int sizeTrackerT      = TimeHitMap->entries();
//   G4cout << sizeTrackerNtower << "\t" << sizeTrackerNdsssd << "\t" << sizeTrackerE << "\t" << sizeTrackerT << G4endl;
//   G4int sizeTrackerX = XHitMap->entries();
//   G4int sizeTrackerY = YHitMap->entries();

//   if (sizeE != sizeT || sizeT != sizeX || sizeX != sizeY) {
   if (sizeTrackerE != sizeTrackerT) {
      G4cout << "No match size Si Event Map: sE:" << sizeTrackerE << " sT:" << sizeTrackerT << G4endl ;
      return;
   }

   G4double Etot = 0;

   // Loop on FirstStage number
   for (G4int l = 0; l < sizeTrackerNtower; l++) {
      G4double N     = *(TowerNumber_itr->second);
      G4int NTrackID =   TowerNumber_itr->first - N;
//      G4cout << N << "\t" << NTrackID << G4endl;

      if (N > 0) {
         // Fill tower number
         m_Event->SetCTTrackerFrontETowerNbr(N);
         m_Event->SetCTTrackerFrontTTowerNbr(N);
         m_Event->SetCTTrackerBackEDetectorNbr(N);
         m_Event->SetCTTrackerBackTDetectorNbr(N);

         // Energy
         Energy_itr = EnergyHitMap->GetMap()->begin();
         for (G4int l = 0 ; l < sizeTrackerE ; l++) {
            G4int ETrackID  =   Energy_itr->first - N;
            G4double E     = *(Energy_itr->second);
            if (ETrackID == NTrackID) {
               Etot += E;
               m_Event->SetCTTrackerFrontEEnergy(RandGauss::shoot(E, EnergyResolutionTracker));
//               m_Event->SetCTTrackerBackEEnergy(RandGauss::shoot(E, EnergyResolutionTracker));
            }
            Energy_itr++;
         }

         //  Time
         Time_itr = TimeHitMap->GetMap()->begin();
         for (G4int h = 0 ; h < sizeTrackerT ; h++) {
            G4int TTrackID  =   Time_itr->first - N;
            G4double T     = *(Time_itr->second);
            if (TTrackID == NTrackID) {
               m_Event->SetCTTrackerFrontTTime(RandGauss::shoot(T, TimeResolutionTracker));
               m_Event->SetCTTrackerBackTTime(RandGauss::shoot(T, TimeResolutionTracker));
            }
            Time_itr++;
         }
/*
         // X
         X_itr = XHitMap->GetMap()->begin();
         for (G4int h = 0 ; h < sizeTrackerX ; h++) {
            G4int XTrackID  =   X_itr->first - N;
            G4int X         = *(X_itr->second);
            if (XTrackID == NTrackID) {
               m_Event->SetCTFrontEStripNbr(X);
               m_Event->SetCTFrontTStripNbr(X);
            }

            X_itr++;
         }

         // Y
         Y_itr = YHitMap->GetMap()->begin()  ;
         for (G4int h = 0 ; h < sizeTrackerY ; h++) {
            G4int YTrackID  =   Y_itr->first - N;
            G4int Y         = *(Y_itr->second);
            if (YTrackID == NTrackID) {
               m_Event->SetCTBackEStripNbr(Y);
               m_Event->SetCTBackTStripNbr(Y);
            }

            Y_itr++;
         }

         // Pos X
         Pos_X_itr = PosXHitMap->GetMap()->begin();
         for (G4int h = 0; h < PosXHitMap->entries(); h++) {
            G4int PosXTrackID =   Pos_X_itr->first - N    ;
            G4double PosX     = *(Pos_X_itr->second)      ;
            if (PosXTrackID == NTrackID) {
               ms_InterCoord->SetDetectedPositionX(PosX) ;
            }
            Pos_X_itr++;
         }

         // Pos Y
         Pos_Y_itr = PosYHitMap->GetMap()->begin();
         for (G4int h = 0; h < PosYHitMap->entries(); h++) {
            G4int PosYTrackID =   Pos_Y_itr->first - N    ;
            G4double PosY     = *(Pos_Y_itr->second)      ;
            if (PosYTrackID == NTrackID) {
               ms_InterCoord->SetDetectedPositionY(PosY) ;
            }
            Pos_Y_itr++;
         }

         // Pos Z
         Pos_Z_itr = PosZHitMap->GetMap()->begin();
         for (G4int h = 0; h < PosZHitMap->entries(); h++) {
            G4int PosZTrackID =   Pos_Z_itr->first - N    ;
            G4double PosZ     = *(Pos_Z_itr->second)      ;
            if (PosZTrackID == NTrackID) {
               ms_InterCoord->SetDetectedPositionZ(PosZ) ;
            }
            Pos_Z_itr++;
         }

         // Angle Theta
         Ang_Theta_itr = AngThetaHitMap->GetMap()->begin();
         for (G4int h = 0; h < AngThetaHitMap->entries(); h++) {
            G4int AngThetaTrackID =   Ang_Theta_itr->first - N    ;
            G4double AngTheta     = *(Ang_Theta_itr->second)      ;
            if (AngThetaTrackID == NTrackID) {
               ms_InterCoord->SetDetectedAngleTheta(AngTheta) ;
            }
            Ang_Theta_itr++;
         }

         // Angle Phi
         Ang_Phi_itr = AngPhiHitMap->GetMap()->begin();
         for (G4int h = 0; h < AngPhiHitMap->entries(); h++) {
            G4int AngPhiTrackID =   Ang_Phi_itr->first - N    ;
            G4double AngPhi     = *(Ang_Phi_itr->second)      ;
            if (AngPhiTrackID == NTrackID) {
               ms_InterCoord->SetDetectedAnglePhi(AngPhi) ;
            }
            Ang_Phi_itr++;
         }
*/
/*
         // Third Stage
         ThirdStageEnergy_itr = ThirdStageEnergyHitMap->GetMap()->begin()  ;
         for (G4int h = 0 ; h < ThirdStageEnergyHitMap->entries() ; h++) {
            G4int ThirdStageEnergyTrackID  =   ThirdStageEnergy_itr->first - N;
            G4double ThirdStageEnergy      = *(ThirdStageEnergy_itr->second)    ;

            if (ThirdStageEnergyTrackID == NTrackID) {
               m_Event->SetGPDTrkThirdStageEEnergy(RandGauss::shoot(ThirdStageEnergy, ResoThirdStage));
               m_Event->SetGPDTrkThirdStageEPadNbr(1);
               m_Event->SetGPDTrkThirdStageTPadNbr(1);
               m_Event->SetGPDTrkThirdStageTTime(1);
               m_Event->SetGPDTrkThirdStageTDetectorNbr(m_index["Square"] + N);
               m_Event->SetGPDTrkThirdStageEDetectorNbr(m_index["Square"] + N);
            }

            ThirdStageEnergy_itr++;
         }
*/
      }
      TowerNumber_itr++;

   }

   // Fill total energy here
   if (Etot > 0) m_Event->SetCTTrackerBackEEnergy(RandGauss::shoot(Etot, EnergyResolutionTracker));

   // clear map for next event
   TowerNumberHitMap -> clear();
   DSSSDNumberHitMap -> clear();
   EnergyHitMap      -> clear();
   TimeHitMap        -> clear();
/*   XHitMap        ->clear()   ;
   YHitMap        ->clear()   ;
   PosXHitMap     ->clear();
   PosYHitMap     ->clear();
   PosZHitMap     ->clear();
   AngThetaHitMap ->clear();
   AngPhiHitMap   ->clear();*/
//   ThirdStageEnergyHitMap ->clear();

   ///////////////////////////////////////////////////
   // counting processes (compton, .....)           //
   ///////////////////////////////////////////////////
   // clear root object
   m_ProcessEvent->Clear();

   // scorers for processes....
   std::map<G4int, G4double*>::iterator TrkProcC_itr;
   G4THitsMap<G4double>* TrkProcCHitMap;
   G4int TrkProcCCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("TrackerScorerComptonTelescope/Process");
   TrkProcCHitMap = (G4THitsMap<G4double>*)(event->GetHCofThisEvent()->GetHC(TrkProcCCollectionID))                    ;
   TrkProcC_itr = TrkProcCHitMap->GetMap()->begin();

   for (G4int l = 0; l < TrkProcCHitMap->entries(); l++) {
      // get electron energy
      G4double ElectronKineticEnergy = *(TrkProcC_itr->second);
      // get tower, dssd number & trackID
      G4int index = TrkProcC_itr->first;
//      G4cout << "Read Sensitive : " << l << "\t" << index << G4endl;
      G4int towerID = index / (G4int)pow(100.,2.);
      index -= towerID * (G4int)pow(100.,2.);
      G4int dsssdID = index / (G4int)pow(100.,1.);
      G4int trackID = index - dsssdID * (G4int)pow(100.,1.);
      trackID *= 1;
//      G4cout << "IDs : " << towerID << "\t" << dsssdID << "\t" << trackID << G4endl;
//      G4cout << "Energy : " << ElectronKineticEnergy << G4endl;
      // fill process data class
      m_ProcessEvent->SetCTTrackerComptonTowerNbr(towerID);
      m_ProcessEvent->SetCTTrackerComptonDetectorNbr(dsssdID);
      m_ProcessEvent->SetCTTrackerComptonEnergy(ElectronKineticEnergy);

      TrkProcC_itr++;
   }
   TrkProcCHitMap->clear();

}


void ComptonTelescope::InitializeMaterial()
{

   ////////////////////////////////////////////////////////////////
   /////////////////Element  Definition ///////////////////////////
   ////////////////////////////////////////////////////////////////
   G4String symbol;
   G4double density = 0, a = 0, z = 0;
   G4int ncomponents = 0, natoms = 0;

   G4Element* N   = new G4Element("Nitrogen" , symbol = "N"  , z = 7  , a = 14.01  * g / mole);
   G4Element* O   = new G4Element("Oxigen"   , symbol = "O"  , z = 8  , a = 16.00  * g / mole);
   G4Element* Br  = new G4Element("Bromine"  , symbol = "Br" , z = 35 , a =  79.9  * g / mole);
   G4Element* La  = new G4Element("Lanthanum", symbol = "La" , z = 57 , a = 138.9  * g / mole);

   ////////////////////////////////////////////////////////////////
   /////////////////Material Definition ///////////////////////////
   ////////////////////////////////////////////////////////////////

   // Si
   a = 28.0855 * g / mole;
   density = 2.321 * g / cm3;
   m_MaterialSilicon = new G4Material("Si", z = 14., a, density);

   // Al
   density = 2.702 * g / cm3;
   a = 26.98 * g / mole;
   m_MaterialAluminium = new G4Material("Aluminium", z = 13., a, density);

   // LaBr3
   density = 5.06 * g / cm3;
   m_MaterialLaBr3 = new G4Material("LaBr3", density, ncomponents = 2);
   m_MaterialLaBr3->AddElement(La , natoms = 1);
   m_MaterialLaBr3->AddElement(Br , natoms = 3);

   //  Vacuum
   density = 0.000000001 * mg / cm3;
   m_MaterialVacuum = new G4Material("Vacuum", density, ncomponents = 2);
   m_MaterialVacuum->AddElement(N, .7);
   m_MaterialVacuum->AddElement(O, .3);
}



void ComptonTelescope::InitializeScorers()
{
   // First stage Associate Scorer
   m_TrackerScorer = new G4MultiFunctionalDetector("TrackerScorerComptonTelescope");
   G4VPrimitiveScorer* TowerNbr    = new ComptonTelescopeScorerTrackerTowerNumber("TowerNumber", "ComptonTelescopeTower", 0);
   G4VPrimitiveScorer* DSSSDNbr    = new ComptonTelescopeScorerTrackerDSSSDNumber("DSSSDNumber", "ComptonTelescopeTower", 0);
   G4VPrimitiveScorer* Energy      = new ComptonTelescopeScorerTrackerEnergy("Energy", "ComptonTelescopeTower", 0);
   G4VPrimitiveScorer* TOF         = new ComptonTelescopeScorerTrackerTOF("Time", "ComptonTelescopeTower", 0);
   G4VPrimitiveScorer* ProcessTypeCompt = new ComptonTelescopeScorerProcess("Process", "ComptonTelescopeTower", "compt", 0);
/*   G4VPrimitiveScorer* StripPositionX                   = new GPDScorerFirstStageFrontStripSquare("StripNumberX", 0, m_NumberOfStrips);
   G4VPrimitiveScorer* StripPositionY                   = new GPDScorerFirstStageBackStripSquare("StripNumberY", 0, m_NumberOfStrips);
   G4VPrimitiveScorer* InteractionCoordinatesX          = new GENERALSCORERS::PSInteractionCoordinatesX("InterCoordX","", 0);
   G4VPrimitiveScorer* InteractionCoordinatesY          = new GENERALSCORERS::PSInteractionCoordinatesY("InterCoordY","", 0);
   G4VPrimitiveScorer* InteractionCoordinatesZ          = new GENERALSCORERS::PSInteractionCoordinatesZ("InterCoordZ","", 0);
   G4VPrimitiveScorer* InteractionCoordinatesAngleTheta = new GENERALSCORERS::PSInteractionCoordinatesAngleTheta("InterCoordAngTheta","", 0);
   G4VPrimitiveScorer* InteractionCoordinatesAnglePhi   = new GENERALSCORERS::PSInteractionCoordinatesAnglePhi("InterCoordAngPhi","", 0);
*/
   //and register it to the multifunctionnal detector
   m_TrackerScorer->RegisterPrimitive(TowerNbr);
   m_TrackerScorer->RegisterPrimitive(DSSSDNbr);
   m_TrackerScorer->RegisterPrimitive(Energy);
   m_TrackerScorer->RegisterPrimitive(TOF);
   m_TrackerScorer->RegisterPrimitive(ProcessTypeCompt);
/*   m_TrackerScorer->RegisterPrimitive(StripPositionX);
   m_TrackerScorer->RegisterPrimitive(StripPositionY);
   m_TrackerScorer->RegisterPrimitive(InteractionCoordinatesX);
   m_TrackerScorer->RegisterPrimitive(InteractionCoordinatesY);
   m_TrackerScorer->RegisterPrimitive(InteractionCoordinatesZ);
   m_TrackerScorer->RegisterPrimitive(InteractionCoordinatesAngleTheta);
   m_TrackerScorer->RegisterPrimitive(InteractionCoordinatesAnglePhi);
*/
   //  Third stage Associate Scorer 
   m_CalorimeterScorer = new G4MultiFunctionalDetector("CalorimeterScorerComptonTelescope");
   G4VPrimitiveScorer* CaloTower    = new ComptonTelescopeScorerCalorimeterTowerNumber("TowerNumber", "ComptonTelescopeTower", 0);
   G4VPrimitiveScorer* CaloDetector = new ComptonTelescopeScorerCalorimeterDetectorNumber("DetectorNumber", "ComptonTelescopeTower", 0);
   G4VPrimitiveScorer* CaloEnergy   = new ComptonTelescopeScorerCalorimeterEnergy("Energy", "ComptonTelescopeTower", 0);
//   G4VPrimitiveScorer* CaloTime     = new ComptonTelescopeScorerCalorimeterTOF("Time", "ComptonTelescopeTower", 0);
   m_CalorimeterScorer->RegisterPrimitive(CaloTower);
   m_CalorimeterScorer->RegisterPrimitive(CaloDetector);
   m_CalorimeterScorer->RegisterPrimitive(CaloEnergy);
//   m_CalorimeterScorer->RegisterPrimitive(CaloTime);

   //  Add All Scorer to the Global Scorer Manager
   G4SDManager::GetSDMpointer()->AddNewDetector(m_TrackerScorer);
   G4SDManager::GetSDMpointer()->AddNewDetector(m_CalorimeterScorer);
}
