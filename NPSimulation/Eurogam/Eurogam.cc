/*****************************************************************************
 * Copyright (C) 2009-2013   this file is part of the NPTool Project         *
 *                                                                           *
 * For the licensing terms see $NPTOOL/Licence/NPTool_Licence                *
 * For the list of contributors see $NPTOOL/Licence/Contributors             *
 *****************************************************************************/

/*****************************************************************************
 * Original Author: N. de Sereville  contact address: deserevi@ipno.in2p3.fr *
 *                                                                           *
 * Creation Date  : December 2010                                            *
 * Last update    :                                                          *
 *---------------------------------------------------------------------------*
 * Decription:                                                               *
 *     This class describes the Eurogam germanium detector                   *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Comment:                                                                  *
 *                                                                           *
 *****************************************************************************/

// C++ headers
#include <sstream>
#include <cmath>
#include <limits>

//G4 Geometry object
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"

//G4 sensitive
#include "G4SDManager.hh"
#include "G4MultiFunctionalDetector.hh"

//G4 various object
#include "G4MaterialTable.hh"
#include "G4Element.hh"
#include "G4ElementTable.hh"
#include "G4Transform3D.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

// NPTool header
#include "Eurogam.hh"
#include "GeneralScorers.hh"
#include "RootOutput.h"
using namespace GENERALSCORERS;

// CLHEP header
#include "CLHEP/Random/RandGauss.h"

using namespace std;
using namespace CLHEP;
using namespace EUROGAMDETECTOR;



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// Eurogam Specific Method
Eurogam::Eurogam()
{
   InitializeMaterial();
   m_Event = new TEurogamData();
}



Eurogam::~Eurogam()
{
   // Materials
   delete m_Material_Vacuum;
   delete m_Material_Aluminium;
   delete m_Material_Silicon;
   delete m_Material_Germanium;

   delete m_Event;
   delete m_EurogamScorer;
}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void Eurogam::AddEurogamModule(G4double R, G4double Theta, G4double Phi,
                               G4double beta_u, G4double beta_v, G4double beta_w)
{
   m_R.push_back(R);
   m_Theta.push_back(Theta);
   m_Phi.push_back(Phi);
   m_beta_u.push_back(beta_u);
   m_beta_v.push_back(beta_v);
   m_beta_w.push_back(beta_w);
}



// Read stream at Configfile to pick-up parameters of detector (Position,...)
// Called in DetecorConstruction::ReadDetextorConfiguration Method
void Eurogam::ReadConfiguration(string Path)
{
   ifstream ConfigFile;
   ConfigFile.open(Path.c_str());
   string LineBuffer, DataBuffer;

   G4double R = 0, Theta = 0, Phi = 0;
   G4double beta_u = 0, beta_v = 0, beta_w = 0;

   bool check_Theta   = false;
   bool check_Phi     = false;
   bool check_R       = false;
   bool ReadingStatus = false;


   while (!ConfigFile.eof()) {
      getline(ConfigFile, LineBuffer);

      // If line is a Start Up Eurogam bloc, Reading toggle to true
      if (LineBuffer.compare(0, 7, "Eurogam") == 0) {
         G4cout << "///" << G4endl;
         G4cout << "Eurogam Module found: " << G4endl;
         ReadingStatus = true;
      }
      // Else don't toggle to Reading Block Status
      else ReadingStatus = false;

      // Reading Block
      while (ReadingStatus) {
         // Pickup Next Word 
         ConfigFile >> DataBuffer;

         // Comment Line 
         if (DataBuffer.compare(0, 1, "%") == 0) {
            ConfigFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
         }

         // Finding another telescope (safety), toggle out
         else if (DataBuffer.compare(0, 7, "Eurogam") == 0) {
            cout << "WARNING: Another Detector is find before standard sequence of Token, Error may occured in Telecope definition" << endl;
            ReadingStatus = false;
         }

         // Angle method
         else if (DataBuffer.compare(0, 2, "R=") == 0) {
            check_R = true;
            ConfigFile >> DataBuffer ;
            R = atof(DataBuffer.c_str()) ;
            R = R * mm;
            cout << "R:  " << R/mm << endl;
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
         
         ///////////////////////////////////////////////////
         // If no Detector Token and no comment, toggle out
         else {
            ReadingStatus = false; 
            G4cout << "Wrong Token Sequence: Getting out " << DataBuffer << G4endl;
         }

         /////////////////////////////////////////////////
         // If All necessary information there, toggle out
         if (check_Theta && check_Phi && check_R) { 
            AddEurogamModule(R, Theta, Phi, beta_u, beta_v, beta_w);

            // Reinitialisation of Check Boolean 
            check_R       = false;
            check_Theta   = false;
            check_Phi     = false;
            ReadingStatus = false;
            cout << "///"<< endl;
      	 }
      }
   }
}



// Construct detector and inialise sensitive part.
// Called After DetecorConstruction::AddDetector Method
void Eurogam::ConstructDetector(G4LogicalVolume* world)
{
   G4ThreeVector     Det_pos = G4ThreeVector(0, 0, 0);
   G4RotationMatrix* Det_rot = new G4RotationMatrix();

   for (unsigned short i = 0; i < m_R.size(); i++) {
      G4double Theta = m_Theta[i];
      G4double Phi   = m_Phi[i];

      // (u,v,w) unitary vector associated to the detector referencial
      // (u,v) // to detector front
      // w perpendicular to (u,v) plan and pointing to the detector back
      // Phi is angle between X axis and projection in (X,Y) plan
      // Theta is angle between  position vector and z axis
      G4double wX = m_R[i] * sin(Theta / rad) * cos(Phi / rad);
      G4double wY = m_R[i] * sin(Theta / rad) * sin(Phi / rad);
      G4double wZ = m_R[i] * cos(Theta / rad);
      G4ThreeVector dirw = G4ThreeVector(wX, wY, wZ);

      // vector corresponding to the center of the module
      Det_pos = dirw;
      dirw = dirw.unit();

      // vector parallel to one axis of silicon plane
      G4double ii = cos(Theta / rad) * cos(Phi / rad);
      G4double jj = cos(Theta / rad) * sin(Phi / rad);
      G4double kk = -sin(Theta / rad);
      G4ThreeVector Y = G4ThreeVector(ii, jj, kk);

      // build rotation matrix to go from the lab referential 
      // to the telescope referential
      G4ThreeVector diru = dirw.cross(Y);
      G4ThreeVector dirv = dirw.cross(diru);
      dirv = dirv.unit();
      diru = diru.unit();
      Det_rot = new G4RotationMatrix(diru, dirv, dirw);

      // Detector is rotated by Beta angles around the v axis.
      Det_rot->rotate(m_beta_u[i], diru);
      Det_rot->rotate(m_beta_v[i], dirv);
      Det_rot->rotate(m_beta_w[i], dirw);

      // translation to place the detector
      Det_pos += dirw * EurogamDepth * 0.5;

      // Build geometry
      VolumeMaker(i + 1, Det_pos, Det_rot, world);
   }
}



void Eurogam::VolumeMaker(G4int             DetNumber,
                          G4ThreeVector     DetPos, 
                          G4RotationMatrix* DetRot,
                          G4LogicalVolume*  world)
{

   ////////////////////////////////////////////////////////////////
   ////////////// Starting Volume Definition //////////////////////
   ////////////////////////////////////////////////////////////////		
   // Name of the module
   std::ostringstream DetectorNumber;
   DetectorNumber << DetNumber;
   G4String Name = "Eurogam" + DetectorNumber.str();

   // EUROGAM box
   G4Box* solidEurogam = new G4Box(Name, EurogamSize/2, EurogamSize/2, EurogamDepth/2);

   G4LogicalVolume* logicEurogam = new G4LogicalVolume(solidEurogam, m_Material_Vacuum, Name, 0, 0, 0); 

   new G4PVPlacement(G4Transform3D(*DetRot, DetPos), logicEurogam, Name, world, false, 0);

   logicEurogam->SetVisAttributes(G4VisAttributes::Invisible);

   // Germanium detector itself
   // Capot en Aluminium
   // Epaisseur du capot
   G4double AluThickness = 1*mm;
   // Fermeture du capot par 2 disques pleins
   G4double RMinFront = 0, RMaxFront = 66.2/2*mm, DzFront = AluThickness;
   G4double RMinBack  = 0, RMaxBack  = 86.2/2*mm, DzBack  = AluThickness;
   // Composante conique du capot
   G4double RMin1Cone = RMaxFront - DzFront, RMax1Cone = RMaxFront;
   G4double RMin2Cone = RMaxBack  - DzBack,  RMax2Cone = RMaxBack;
   G4double DzCone = 101.5*mm;
   // Composante cylindrique du capot
   G4double RMinCylin = RMaxBack - DzBack, RMaxCylin = RMaxBack;
   G4double DzCylin = 163.1*mm;

   // Capot avant
   G4VSolid* solidCapotFront = new G4Tubs("solidCapotFront", RMinFront, RMaxFront, DzFront/2, 0*degree, 360*degree);

   G4LogicalVolume* logicCapotFront = new G4LogicalVolume(solidCapotFront, // its solid
                                                          m_Material_Aluminium,       // its material
                                                          "logicCapotFront");   // its name

   G4double OffsetZCapotFront = -(EurogamDepth - DzFront)/2;
   G4ThreeVector ZposCapotFront = G4ThreeVector(0, 0, OffsetZCapotFront);
   new G4PVPlacement(0,     // no rotation
                     ZposCapotFront,   // at (0,0,Dz)
                     logicCapotFront,  // its logical name
                     Name + "_CapotFront",  // its name
                     logicEurogam,  // its mother name
                     false,      // no boolean operation
                     0);      // copy number

   // Partie conique
   G4VSolid* solidCapotCone = new G4Cons("solidCapotCone", RMin1Cone, RMax1Cone, RMin2Cone, RMax2Cone, DzCone/2, 0*degree, 360*degree);

   G4LogicalVolume* logicCapotCone = new G4LogicalVolume(solidCapotCone,      //its solid
                                                         m_Material_Aluminium,    //its material
                                                        "logicCapotCone");    //its name

   G4double OffsetZCapotCone = -(EurogamDepth/2 - DzFront - DzCone/2);
   G4ThreeVector ZposCapotCone = G4ThreeVector(0, 0, OffsetZCapotCone);
   new G4PVPlacement(0,      // no rotation
                     ZposCapotCone,  // at (0,0,Dz)
                     logicCapotCone, // its logical name
                     Name + "_CapotCone", // its name
                     logicEurogam,   // its mother name
                     false,    // no boolean operation
                     0);    // copy number

   // Partie cylindrique
   G4VSolid* solidCapotCylin = new G4Tubs("solidCapotCylin", RMinCylin, RMaxCylin, DzCylin/2, 0*degree, 360*degree);

   G4LogicalVolume* logicCapotCylin = new G4LogicalVolume(solidCapotCylin, //its solid
                                                          m_Material_Aluminium,      //its material
                                                          "logicCapotCylin");     //its name

   G4double OffsetZCapotCylin = -(EurogamDepth/2 - DzFront - DzCone - DzCylin/2);
   G4ThreeVector ZposCapotCylin = G4ThreeVector(0, 0, OffsetZCapotCylin);
   new G4PVPlacement(0,     // no rotation
                     ZposCapotCylin,   // at (0,0,Dz)
                     logicCapotCylin,  // its logical name
                     Name + "_CapotCylin",  // its name
                     logicEurogam,  // its mother name
                     false,      // no boolean operation
                     0);      // copy number

  // Capot arriere
  G4VSolid* solidCapotBack = new G4Tubs("solidCapotBack", RMinBack, RMaxBack, DzBack/2, 0*degree, 360*degree);

  G4LogicalVolume* logicCapotBack = new G4LogicalVolume(solidCapotBack,      //its solid
                                                        m_Material_Aluminium,     //its material
                                                        "logicCapotBack");     //its name

   G4double OffsetZCapotBack = -(EurogamDepth/2 - DzFront - DzCone - DzCylin - DzBack/2);
   G4ThreeVector ZposCapotBack = G4ThreeVector(0, 0, OffsetZCapotBack);
   new G4PVPlacement(0,         // no rotation
                     ZposCapotBack,  // at (0,0,Dz)
                     logicCapotBack, // its logical name
                     Name + "_CapotBack", // its name
                     logicEurogam,   // its mother name
                     false,    // no boolean operation
                     0);    // copy number

   // cristal de Ge et doigt froid
   G4double RMinGe = 0, RMaxGe = 63.1/2*mm, DzGe = 78.5*mm;
   G4double SPhiGe = 0, DPhiGe = 360*degree;
   G4double distCapotGe = 20*mm;
   G4double RMinDoigt = 0, RMaxDoigt = 11.2/2*mm, DzDoigt = 70*mm;
   G4double SPhiDoigt = 0*degree, DPhiDoigt = 360*degree;

   G4VSolid* solidDetec = new G4Tubs("solidGermanium",           //its name
                                     RMinGe, RMaxGe, DzGe/2,      //its size
                                     SPhiGe, DPhiGe);        //its size

   G4VSolid* solidDoigt = new G4Tubs("solidDoigt",
                                     RMinDoigt, RMaxDoigt, DzDoigt/2,
                                     SPhiDoigt, DPhiDoigt);

   // !!! pour soustraire le doigt froid du cristal initial de Germainum
   // cela se fait dans le repere du cristal de Germanium
   // *************** changer signe - en signe + ???????????
   G4double OffsetZDoigt = - (DzGe/2 - DzDoigt/2);
   G4ThreeVector ZposDoigt = G4ThreeVector(0, 0, OffsetZDoigt);
   G4VSolid* solidSubtrac = new G4SubtractionSolid("solidCristalGe", solidDetec, solidDoigt, 0, ZposDoigt);

   G4LogicalVolume* logicDetec= new G4LogicalVolume(solidSubtrac,    //its solid
//                                                    m_Material_Germanium,    //its material
                                                    m_Material_Silicon,    //its material
                                                    "logicCristalGe");     //its name

//   G4LogicalVolume* logicDetec= new G4LogicalVolume(solidDetec,      //its solid
//                                                    m_Material_Germanium,    //its material
//                                                    "logicCristalGe");     //its name
                                                                                     

   G4double OffsetZGe = -(EurogamDepth/2 - distCapotGe - DzGe/2);
   G4ThreeVector ZposGe = G4ThreeVector(0, 0, OffsetZGe);
   new G4PVPlacement(0,    //no rotation
                     ZposGe, //at (0,0,0)
                     logicDetec, //its logical volume
                     Name + "_CristalGe",    //its name
                     logicEurogam,  //its mother  volume
                     false,      //no boolean operation
                     0);      //copy number

   // Set Ge detector sensible
   logicDetec->SetSensitiveDetector(m_EurogamScorer);

   // Visualisation of FirstStage Strip
   G4VisAttributes* DetectorVisAtt = new G4VisAttributes(G4Colour(0.0, 0.0, 0.9));   // blue
   logicDetec->SetVisAttributes(DetectorVisAtt);
}



// Add Detector branch to the EventTree.
// Called After DetecorConstruction::AddDetector Method
void Eurogam::InitializeRootOutput()
{
   RootOutput *pAnalysis = RootOutput::getInstance();
   TTree *pTree = pAnalysis->GetTree();
   pTree->Branch("Eurogam", "TEurogamData", &m_Event) ;
}



// Read sensitive part and fill the Root tree.
// Called at in the EventAction::EndOfEventAvtion
void Eurogam::ReadSensitive(const G4Event* event)
{
   // Clear the data of the TEurogamData object
   m_Event->Clear();

   //////////////////////////////////////////////////////////////////////////////////////
   //////////////////////// Used to Read Event Map of detector //////////////////////////
   //////////////////////////////////////////////////////////////////////////////////////
   std::map<G4int, G4int*>::iterator    DetectorNumber_itr;
   std::map<G4int, G4double*>::iterator Energy_itr;
   std::map<G4int, G4double*>::iterator Time_itr;
   std::map<G4int, G4double*>::iterator Pos_X_itr;
   std::map<G4int, G4double*>::iterator Pos_Y_itr;
   std::map<G4int, G4double*>::iterator Pos_Z_itr;
   std::map<G4int, G4double*>::iterator Ang_Theta_itr;
   std::map<G4int, G4double*>::iterator Ang_Phi_itr;

   G4THitsMap<G4int>*    DetectorNumberHitMap;
   G4THitsMap<G4double>* EnergyHitMap;
   G4THitsMap<G4double>* TimeHitMap;
   G4THitsMap<G4double>* PosXHitMap;
   G4THitsMap<G4double>* PosYHitMap;
   G4THitsMap<G4double>* PosZHitMap;
   G4THitsMap<G4double>* AngThetaHitMap;
   G4THitsMap<G4double>* AngPhiHitMap;

   // Read the Scorer associate to the Silicon Strip
   //Detector Number
   G4int DetCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("EurogamScorer/DetectorNumber");
   DetectorNumberHitMap = (G4THitsMap<G4int>*)(event->GetHCofThisEvent()->GetHC(DetCollectionID));
   DetectorNumber_itr =  DetectorNumberHitMap->GetMap()->begin();

   // Energy
   G4int EnergyCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("EurogamScorer/Energy");
   EnergyHitMap = (G4THitsMap<G4double>*)(event->GetHCofThisEvent()->GetHC(EnergyCollectionID));
   Energy_itr = EnergyHitMap->GetMap()->begin();

   //Time of Flight
   G4int TimeCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("EurogamScorer/Time");
   TimeHitMap = (G4THitsMap<G4double>*)(event->GetHCofThisEvent()->GetHC(TimeCollectionID));
   Time_itr = TimeHitMap->GetMap()->begin();

   //Interaction Coordinate X
   G4int InterCoordXCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("EurogamScorer/InterCoordX");
   PosXHitMap = (G4THitsMap<G4double>*)(event->GetHCofThisEvent()->GetHC(InterCoordXCollectionID));
   Pos_X_itr = PosXHitMap->GetMap()->begin();

   //Interaction Coordinate Y
   G4int InterCoordYCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("EurogamScorer/InterCoordY");
   PosYHitMap = (G4THitsMap<G4double>*)(event->GetHCofThisEvent()->GetHC(InterCoordYCollectionID));
   Pos_Y_itr = PosYHitMap->GetMap()->begin();

   //Interaction Coordinate Z
   G4int InterCoordZCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("EurogamScorer/InterCoordZ");
   PosZHitMap = (G4THitsMap<G4double>*)(event->GetHCofThisEvent()->GetHC(InterCoordZCollectionID));
   Pos_Z_itr = PosXHitMap->GetMap()->begin();

   //Interaction Coordinate Angle Theta
   G4int InterCoordAngThetaCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("EurogamScorer/InterCoordAngTheta");
   AngThetaHitMap = (G4THitsMap<G4double>*)(event->GetHCofThisEvent()->GetHC(InterCoordAngThetaCollectionID));
   Ang_Theta_itr = AngThetaHitMap->GetMap()->begin();

   //Interaction Coordinate Angle Phi
   G4int InterCoordAngPhiCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("EurogamScorer/InterCoordAngPhi");
   AngPhiHitMap = (G4THitsMap<G4double>*)(event->GetHCofThisEvent()->GetHC(InterCoordAngPhiCollectionID));
   Ang_Phi_itr = AngPhiHitMap->GetMap()->begin();

   // Check the size of different map
   G4int sizeN = DetectorNumberHitMap->entries();
   G4int sizeE = EnergyHitMap->entries();
   G4int sizeT = TimeHitMap->entries();
   G4int sizeX = PosXHitMap->entries();

/*   if (sizeN != sizeE || sizeE != sizeT) {
      G4cout << "No match size Eurogam Event Map: sE:" << sizeE << " sT:" << sizeT << endl;
      return;
   }*/
   G4cout << "*******SIZE********: " << sizeN << "  " << sizeE << "  " << sizeT << "  " << sizeX << G4endl;

   G4double Etot = 0;

   // Loop on detector number
   for (G4int l = 0; l < sizeN; l++) {
      G4double N     = *(DetectorNumber_itr->second);
      G4int NTrackID =   DetectorNumber_itr->first - N;
      G4cout << "NTrackID: " << NTrackID << "  " << N << G4endl;

      if (N > 0) {
         // Fill detector number
         m_Event->SetEurogamDetectorNumber(N);

         // Energy
         Energy_itr = EnergyHitMap->GetMap()->begin();
         for (G4int l = 0; l < sizeE; l++) {
            G4int ETrackID =   Energy_itr->first - N;
            G4double E     = *(Energy_itr->second);
            G4cout << "ETrackID, E: " << ETrackID << "   " << E << G4endl;
            if (ETrackID == NTrackID) {
               Etot += E;
//               m_Event->SetEurogamEnergy(RandGauss::shoot(E, 1));
               // Fill energy loss for each step
//               m_Event->SetEurogamEnergy(E*1e3);
            }
            Energy_itr++;
         }

         //  Time
         Time_itr = TimeHitMap->GetMap()->begin();
         for (G4int h = 0; h < sizeT; h++) {
            G4int TTrackID =   Time_itr->first - N;
            G4double T     = *(Time_itr->second);
            if (TTrackID == NTrackID) {
               m_Event->SetEurogamTime(RandGauss::shoot(T, 1));
            }
            Time_itr++;
         }

         // Pos X
         Pos_X_itr = PosXHitMap->GetMap()->begin();
         for (G4int h = 0; h < PosXHitMap->entries(); h++) {
            G4int PosXTrackID =   Pos_X_itr->first - N;
            G4double PosX     = *(Pos_X_itr->second);
            if (PosXTrackID == NTrackID) {
               ms_InterCoord->SetDetectedPositionX(PosX);
            }
            Pos_X_itr++;
         }

         // Pos Y
         Pos_Y_itr = PosYHitMap->GetMap()->begin();
         for (G4int h = 0; h < PosYHitMap->entries(); h++) {
            G4int PosYTrackID =   Pos_Y_itr->first - N;
            G4double PosY     = *(Pos_Y_itr->second);
            if (PosYTrackID == NTrackID) {
               ms_InterCoord->SetDetectedPositionY(PosY);
            }
            Pos_Y_itr++;
         }

         // Pos Z
         Pos_Z_itr = PosZHitMap->GetMap()->begin();
         for (G4int h = 0; h < PosZHitMap->entries(); h++) {
            G4int PosZTrackID =   Pos_Z_itr->first - N;
            G4double PosZ     = *(Pos_Z_itr->second);
            if (PosZTrackID == NTrackID) {
               ms_InterCoord->SetDetectedPositionZ(PosZ);
            }
            Pos_Z_itr++;
         }

         // Angle Theta
         Ang_Theta_itr = AngThetaHitMap->GetMap()->begin();
         for (G4int h = 0; h < AngThetaHitMap->entries(); h++) {
            G4int AngThetaTrackID =   Ang_Theta_itr->first - N;
            G4double AngTheta     = *(Ang_Theta_itr->second);
            if (AngThetaTrackID == NTrackID) {
               ms_InterCoord->SetDetectedAngleTheta(AngTheta);
            }
            Ang_Theta_itr++;
         }

         // Angle Phi
         Ang_Phi_itr = AngPhiHitMap->GetMap()->begin();
         for (G4int h = 0; h < AngPhiHitMap->entries(); h++) {
            G4int AngPhiTrackID =   Ang_Phi_itr->first - N;
            G4double AngPhi     = *(Ang_Phi_itr->second);
            if (AngPhiTrackID == NTrackID) {
               ms_InterCoord->SetDetectedAnglePhi(AngPhi);
            }
            Ang_Phi_itr++;
         }

      }
      DetectorNumber_itr++;
   }

   // Fill total energy here
   if (Etot > 0) m_Event->SetEurogamEnergy(RandGauss::shoot(Etot*1e3, 1));

   // clear map for next event
   DetectorNumberHitMap    -> clear();
   EnergyHitMap            -> clear();
   TimeHitMap              -> clear();
   PosXHitMap              -> clear();
   PosYHitMap              -> clear();
   PosZHitMap              -> clear();
   AngThetaHitMap          -> clear();
   AngPhiHitMap            -> clear();
}



////////////////////////////////////////////////////////////////
void Eurogam::InitializeMaterial()
{
   ////////////////////////////////////////////////////////////////
   /////////////////Element  Definition ///////////////////////////
   ////////////////////////////////////////////////////////////////
   G4String symbol;
   G4double density = 0, a = 0, z = 0;
   G4int ncomponents = 0;

   // Elements
   G4Element* N  = new G4Element("Nitrogen" , symbol = "N",  z = 7, a = 14.01  * g / mole);
   G4Element* O  = new G4Element("Oxigen"   , symbol = "O",  z = 8, a = 16.00  * g / mole);
   G4Element* Al = new G4Element("Aluminium", symbol = "Al", z= 13, a = 26.98  * g / mole);
   G4Element* Si = new G4Element("Silicon"  , symbol = "Si", z= 14, a = 28.09  * g / mole);
   G4Element* Ge = new G4Element("Germanium", symbol = "Ge", z= 32, a = 72.61  * g / mole);


   ////////////////////////////////////////////////////////////////
   /////////////////Material Definition ///////////////////////////
   ////////////////////////////////////////////////////////////////
   //  Vacuum
   density = 0.000000001 * mg / cm3;
   m_Material_Vacuum = new G4Material("Vacuum", density, ncomponents = 2);
   m_Material_Vacuum->AddElement(N, .7);
   m_Material_Vacuum->AddElement(O, .3);

   // Aluminium
   m_Material_Aluminium = new G4Material("Aluminium", density= 2.699*g/cm3, ncomponents=1);
   m_Material_Aluminium->AddElement(Al, 1);

   // Siliclum
   m_Material_Silicon = new G4Material("Silicium", density = 2.321*g/cm3, ncomponents=1);
   m_Material_Silicon->AddElement(Si, 1);

   // Germanium
   m_Material_Germanium = new G4Material("Germanium", density= 5.323*g/cm3, ncomponents=1);
   m_Material_Germanium->AddElement(Ge, 1);
}



////////////////////////////////////////////////////////////////	
void Eurogam::InitializeScorers() 
{ 
   // Eurogam associated scorer
   m_EurogamScorer                                      = new G4MultiFunctionalDetector("EurogamScorer");
   G4VPrimitiveScorer* DetNbr                           = new GENERALSCORERS::PSDetectorNumber("DetectorNumber", "Eurogam", 0);
   G4VPrimitiveScorer* Energy                           = new GENERALSCORERS::PSEnergy("Energy","Eurogam", 0);
   G4VPrimitiveScorer* TOF                              = new GENERALSCORERS::PSTOF("Time", "Eurogam", 0);
   G4VPrimitiveScorer* InteractionCoordinatesX          = new GENERALSCORERS::PSInteractionCoordinatesX("InterCoordX", "Eurogam", 0);
   G4VPrimitiveScorer* InteractionCoordinatesY          = new GENERALSCORERS::PSInteractionCoordinatesY("InterCoordY", "Eurogam", 0);
   G4VPrimitiveScorer* InteractionCoordinatesZ          = new GENERALSCORERS::PSInteractionCoordinatesZ("InterCoordZ", "Eurogam", 0);
   G4VPrimitiveScorer* InteractionCoordinatesAngleTheta = new GENERALSCORERS::PSInteractionCoordinatesAngleTheta("InterCoordAngTheta", "Eurogam", 0);
   G4VPrimitiveScorer* InteractionCoordinatesAnglePhi   = new GENERALSCORERS::PSInteractionCoordinatesAnglePhi("InterCoordAngPhi", "Eurogam", 0);

   //and register it to the multifunctionnal detector
   m_EurogamScorer->RegisterPrimitive(DetNbr);
   m_EurogamScorer->RegisterPrimitive(Energy);
   m_EurogamScorer->RegisterPrimitive(TOF);
   m_EurogamScorer->RegisterPrimitive(InteractionCoordinatesX);
   m_EurogamScorer->RegisterPrimitive(InteractionCoordinatesY);
   m_EurogamScorer->RegisterPrimitive(InteractionCoordinatesZ);
   m_EurogamScorer->RegisterPrimitive(InteractionCoordinatesAngleTheta);
   m_EurogamScorer->RegisterPrimitive(InteractionCoordinatesAnglePhi);

   // Add All Scorer to the Global Scorer Manager
   G4SDManager::GetSDMpointer()->AddNewDetector(m_EurogamScorer);
}
