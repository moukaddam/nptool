/*****************************************************************************
 * Copyright (C) 2009-2013   this file is part of the NPTool Project         *
 *                                                                           *
 * For the licensing terms see $NPTOOL/Licence/NPTool_Licence                *
 * For the list of contributors see $NPTOOL/Licence/Contributors             *
 *****************************************************************************/

/*****************************************************************************
 * Original Author: M. Labiche  contact address: marc.labiche@atfc.ac.uk     *
 *                                                                           *
 * Creation Date  : 26/08/10                                                 *
 * Last update    :                                                          *
 *---------------------------------------------------------------------------*
 * Decription: Define the shiled around a cluster of 9 phoswich modules for  *
 * the Paris detector                                                        *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Comment:                                                                  *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


// C++ headers
#include <sstream>
#include <string>
#include <cmath>

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

// G4 sensitive
#include "G4SDManager.hh"
#include "G4MultiFunctionalDetector.hh"

// NPTool headers
#include "ShieldClParis.hh"
//#include "ParisScorers.hh"
#include "ShieldScorers.hh"
#include "GeneralScorers.hh"
#include "RootOutput.h"

// CLHEP
#include "CLHEP/Random/RandGauss.h"

using namespace std;
using namespace CLHEP;
using namespace PARISCLSHIELD;



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
ShieldClParis::ShieldClParis()
{
   ms_InterCoord = 0;
}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
ShieldClParis::~ShieldClParis()
{
}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void ShieldClParis::AddModule(G4ThreeVector X1_Y1,
                               G4ThreeVector X128_Y1,
                               G4ThreeVector X1_Y128,
                               G4ThreeVector X128_Y128)
{
   m_DefinitionType.push_back(true);

   m_X1_Y1.push_back(X1_Y1);
   m_X128_Y1.push_back(X128_Y1);
   m_X1_Y128.push_back(X1_Y128);
   m_X128_Y128.push_back(X128_Y128);

   m_R.push_back(0);
   m_Theta.push_back(0);
   m_Phi.push_back(0);
   m_beta_u.push_back(0);
   m_beta_v.push_back(0);
   m_beta_w.push_back(0);
}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void ShieldClParis::AddModule(G4double R,
                               G4double Theta,
                               G4double Phi,
                               G4double beta_u,
                               G4double beta_v,
                               G4double beta_w) 
{
   G4ThreeVector empty = G4ThreeVector(0, 0, 0);

   m_DefinitionType.push_back(false);

   m_X1_Y1.push_back(empty);
   m_X128_Y1.push_back(empty);
   m_X1_Y128.push_back(empty);
   m_X128_Y128.push_back(empty);

   m_R.push_back(R);
   m_Theta.push_back(Theta);
   m_Phi.push_back(Phi);
   m_beta_u.push_back(beta_u);
   m_beta_v.push_back(beta_v);
   m_beta_w.push_back(beta_w);
}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void ShieldClParis::VolumeMaker(G4int             DetecNumber,
                                 G4ThreeVector     MMpos,
                                 G4RotationMatrix* MMrot,
                                 G4LogicalVolume*  world)
{
   G4double NbrTelescopes = DetecNumber;
   G4String DetectorNumber;
   ostringstream Number;
   Number << NbrTelescopes;
   DetectorNumber = Number.str();

   /////////////////////////////////////////////////////////////////
   /////////////////Element  Definition ///////////////////////////
   ////////////////////////////////////////////////////////////////
   G4String symbol, name;
   G4double density = 0. , a = 0, z = 0;
   G4int ncomponents = 0;
   G4int nel = 0, natoms = 0;

   ////////////////////////////////////////////////////////////////
   /////////////////Material Definition ///////////////////////////
   ////////////////////////////////////////////////////////////////
   a=137.327*g/mole;
   G4Element* Ba = new G4Element(name="Barium",symbol="Ba",z=56.,a);
   a=18.9984032*g/mole;
   G4Element* F = new G4Element(name="Fluorine",symbol="F",z=9.,a);
   a=22.99*g/mole;
   G4Element* Na = new G4Element(name="Sodium",symbol="Na",z=11.,a);
   a=79.904*g/mole;
   G4Element* Br = new G4Element(name="Bromine",symbol="Br",z=35.,a);
   a=126.90477*g/mole;
   G4Element* I  = new G4Element(name="Iodine",symbol="I",z=53.,a);
   a=132.90545*g/mole;
   G4Element* Cs = new G4Element(name="Cesium",symbol="Cs",z=55.,a);
   a=138.9055*g/mole;
   G4Element* La = new G4Element(name="Lanthanum",symbol="La",z=57.,a);

   //  Vacuum
   G4Element* N   = new G4Element("Nitrogen" , symbol = "N"  , z = 7  , a = 14.01  * g / mole);
   G4Element* O   = new G4Element("Oxigen"   , symbol = "O"  , z = 8  , a = 16.00  * g / mole);

   density = 0.000000001 * mg / cm3;
   G4Material* Vacuum = new G4Material("Vacuum", density, ncomponents = 2);
   Vacuum->AddElement(N, .7);
   Vacuum->AddElement(O, .3);

   // NaI
   density = 3.67*g/cm3, nel = 2; 
   G4Material* NaI = new G4Material(name="NaI",density,nel);
   NaI->AddElement(Na, natoms = 1);
   NaI->AddElement(I,  natoms = 1);

   // CsI
   density  = 4.51*g/cm3, nel = 2; 
   G4Material* CsI = new G4Material(name="CsI", density, nel);
   CsI->AddElement(Cs, natoms = 1);
   CsI->AddElement(I,  natoms = 1);

   // LaBr3
   density = 5.29*g/cm3, nel = 2; 
   G4Material* LaBr3 = new G4Material(name="LaBr3",density,nel);
   LaBr3->AddElement(La, natoms = 1);
   LaBr3->AddElement(Br, natoms = 3);

   // BaF2
   density = 4.89*g/cm3, nel = 2;
   G4Material* BaF2 = new G4Material(name="BaF2", density, nel);
   BaF2->AddElement(Ba, natoms = 1);
   BaF2->AddElement(F,  natoms = 2);

   ////////////////////////////////////////////////////////////////
   ////////////// Starting Volume Definition //////////////////////
   ////////////////////////////////////////////////////////////////
   G4String Name = "ShieldClParis" + DetectorNumber ;

   // Mother Volume
   G4Trap*           solidShieldClParis = new G4Trap(Name, Length/2, 16.3543*deg, 225.*deg, 
                       12.*mm,102.375*mm, 102.375*mm, 0.*deg, 
                       43.125*mm, 133.5*mm, 133.5*mm,0.*deg);
   G4LogicalVolume* logicShieldClParis = new G4LogicalVolume(solidShieldClParis, Vacuum, Name, 0, 0, 0);

   new G4PVPlacement(G4Transform3D(*MMrot, MMpos) ,
                     logicShieldClParis           ,
                     Name                         ,
                     world                        ,
                     false                        ,
                     0);

   logicShieldClParis->SetVisAttributes(G4VisAttributes::Invisible);
   if (m_non_sensitive_part_visiualisation) logicShieldClParis->SetVisAttributes(G4VisAttributes(G4Colour(0.90, 0.90, 0.90)));

   // Daughter Volume
   // CsI or NaI
   G4ThreeVector  positionCsI = G4ThreeVector(0, 0, 0);

   G4Trap*           solidShieldCsI = new G4Trap("solidShieldCsI", Length/2, 16.3543*deg, 225.*deg, 
                       12.*mm,102.375*mm, 102.375*mm, 0.*deg, 
                       43.125*mm, 133.5*mm, 133.5*mm,0.*deg);
   //G4LogicalVolume* logicShieldCsI = new G4LogicalVolume(solidShieldCsI, CsI, "logicShieldCsI", 0, 0, 0);
   G4LogicalVolume* logicShieldCsI = new G4LogicalVolume(solidShieldCsI, NaI, "logicShieldCsI", 0, 0, 0);

   new G4PVPlacement(0,
                     positionCsI              ,
                     logicShieldCsI           ,
                     Name + "_ShieldCsI"      ,
                     logicShieldClParis       ,
                     false                    ,
                     0);

   // Set CsI sensible
   logicShieldCsI->SetSensitiveDetector(m_CsIShieldScorer);

   // Visualisation of CsIStage
   G4VisAttributes* CsIShieldVisAtt = new G4VisAttributes(G4Colour(1., 0., 0.));
   logicShieldCsI->SetVisAttributes(CsIShieldVisAtt);


}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// Virtual Method of VDetector class

// Read stream at Configfile to pick-up parameters of detector (Position,...)
// Called in DetecorConstruction::ReadDetextorConfiguration Method
void ShieldClParis::ReadConfiguration(string Path)
{
   ifstream ConfigFile;
   ConfigFile.open(Path.c_str());
   string LineBuffer, DataBuffer; 

   // A:X1_Y1     --> X:1    Y:1
   // B:X128_Y1   --> X:128  Y:1
   // C:X1_Y128   --> X:1    Y:128
   // D:X128_Y128    --> X:128  Y:128

   G4double Ax , Bx , Cx , Dx , Ay , By , Cy , Dy , Az , Bz , Cz , Dz          ;
   G4ThreeVector A , B , C , D                                                 ;
   G4double Theta = 0 , Phi = 0 , R = 0 , beta_u = 0 , beta_v = 0 , beta_w = 0 ;

   bool ReadingStatus = false;

   bool check_A = false;
   bool check_C = false;
   bool check_B = false;
   bool check_D = false;

   bool check_Theta = false;
   bool check_Phi   = false;
   bool check_R     = false;
   
   bool checkVis = false;

   while (!ConfigFile.eof()) {
      getline(ConfigFile, LineBuffer);
      if (LineBuffer.compare(0, 13, "ShieldClParis") == 0) {
         G4cout << "///" << G4endl           ;
         G4cout << "Cluster Shield element found: " << G4endl   ;
         ReadingStatus = true ;
      }
         
      while (ReadingStatus) {
         ConfigFile >> DataBuffer;
         // Comment Line 
         if (DataBuffer.compare(0, 1, "%") == 0) {/*do nothing */;}
   
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
         else if (DataBuffer.compare(0, 2, "R=") == 0) {
            check_R = true;
            ConfigFile >> DataBuffer ;
            R = atof(DataBuffer.c_str()) ;
            R = R * mm;
            cout << "R:  " << R / mm << endl;
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

         else if (DataBuffer.compare(0, 4, "VIS=") == 0) {
            checkVis = true ;
            ConfigFile >> DataBuffer;
            if (DataBuffer.compare(0, 3, "all") == 0) m_non_sensitive_part_visiualisation = true;
         }
         
         else G4cout << "WARNING: Wrong Token, ShieldClParis: Cluster Element not added" << G4endl;

         // Add The previously define telescope
         // With position method
         if ((check_A && check_B && check_C && check_D && checkVis) && 
             !(check_Theta && check_Phi && check_R)) {
            ReadingStatus = false;
       check_A = false;
       check_C = false;
       check_B = false;
       check_D = false;
       checkVis = false;

            AddModule(A, B, C, D);
         }

         // With angle method
         if ((check_Theta && check_Phi && check_R && checkVis) && 
             !(check_A && check_B && check_C && check_D)) {
            ReadingStatus = false;
            check_Theta = false;
          check_Phi   = false;
          check_R     = false;
       checkVis = false;
           
            AddModule(R, Theta, Phi, beta_u, beta_v, beta_w);
         }
      }
   }
}



// Construct detector and inialise sensitive part.
// Called After DetecorConstruction::AddDetector Method
void ShieldClParis::ConstructDetector(G4LogicalVolume* world)
{
   G4RotationMatrix* MMrot    = NULL                   ;
   G4ThreeVector     MMpos    = G4ThreeVector(0, 0, 0) ;
   G4ThreeVector     MMu      = G4ThreeVector(0, 0, 0) ;
   G4ThreeVector     MMv      = G4ThreeVector(0, 0, 0) ;
   G4ThreeVector     MMw      = G4ThreeVector(0, 0, 0) ;
   G4ThreeVector     MMCenter = G4ThreeVector(0, 0, 0) ;

   G4int NumberOfTelescope = m_DefinitionType.size() ;

   for (G4int i = 0; i < NumberOfTelescope; i++) {
      // By Point
      if (m_DefinitionType[i]) {
         // (u,v,w) unitary vector associated to telescope referencial
         // (u,v) // to silicon plan
         // w perpendicular to (u,v) plan and pointing ThirdStage
         MMu = m_X128_Y1[i] - m_X1_Y1[i]; 
         MMu = MMu.unit();

         MMv = m_X1_Y128[i] - m_X1_Y1[i];
         //MMv = -0.5 * (m_X1_Y1[i] + m_X128_Y128[i] - m_X1_Y128[i] - m_X128_Y1[i]);
         MMv = MMv.unit();

         MMw = MMu.cross(MMv);
//         if (MMw.z() > 0) MMw = MMv.cross(MMu) ;
         MMw = MMw.unit();

         MMCenter = (m_X1_Y1[i] + m_X1_Y128[i] + m_X128_Y1[i] + m_X128_Y128[i]) / 4;

         // Passage Matrix from Lab Referential to Telescope Referential
         // MUST2
         MMrot = new G4RotationMatrix(MMu, MMv, MMw);
         // translation to place Telescope
         //MMpos = MMw * Length * 0.5 + MMCenter;
         MMpos = MMCenter;
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
         MMw = G4ThreeVector(wX, wY, wZ);

         // vector corresponding to the center of the module
         G4ThreeVector CT = MMw;

         // vector parallel to one axis of silicon plane
         G4double ii = cos(Theta / rad) * cos(Phi / rad);
         G4double jj = cos(Theta / rad) * sin(Phi / rad);
         G4double kk = -sin(Theta / rad);
         G4ThreeVector Y = G4ThreeVector(ii, jj, kk);

         MMw = MMw.unit();
         MMu = MMw.cross(Y);
         MMv = MMw.cross(MMu);
         MMv = MMv.unit();
         MMu = MMu.unit();

         // Passage Matrix from Lab Referential to Telescope Referential
         // MUST2
         MMrot = new G4RotationMatrix(MMu, MMv, MMw);
         // Telescope is rotate of Beta angle around MMv axis.
         MMrot->rotate(m_beta_u[i], MMu);
         MMrot->rotate(m_beta_v[i], MMv);
         MMrot->rotate(m_beta_w[i], MMw);
         // translation to place Telescope
         MMpos = MMw * Length * 0.5 + CT ;
      }

      VolumeMaker(i + 1, MMpos, MMrot, world);
   }

   delete MMrot ;
}



// Connect the ParisData class to the output TTree
// of the simulation
void ShieldClParis::InitializeRootOutput()
{
}



// Set the TinteractionCoordinates object from VDetector to the present class
void ShieldClParis::SetInterCoordPointer(TInteractionCoordinates* interCoord)
{
   ms_InterCoord = interCoord;
}



// Read sensitive part and fill the Root tree.
// Called at in the EventAction::EndOfEventAvtion
void ShieldClParis::ReadSensitive(const G4Event* event)
{
   //////////////////////////////////////////////////////////////////////////////////////
   //////////////////////// Used to Read Event Map of detector //////////////////////////
   //////////////////////////////////////////////////////////////////////////////////////

      momentum = event->GetPrimaryVertex()->GetPrimary()->GetMomentum(); 
      G4double EGamma = momentum.getR(); // for photon E=p
      G4double EGammaMin = EGamma-4*ResoFirstStage; 
      G4double EGammaMax = EGamma+4*ResoFirstStage;
  
   // CsI Shield
   std::map<G4int, G4int*>::iterator    CsIShieldDetectorNumber_itr;
   std::map<G4int, G4double*>::iterator CsIShieldEnergy_itr;
   std::map<G4int, G4double*>::iterator CsIShieldTime_itr;
   //std::map<G4int, G4double*>::iterator X_itr;
   //std::map<G4int, G4double*>::iterator Y_itr;
   //std::map<G4int, G4double*>::iterator Pos_X_itr;
   //std::map<G4int, G4double*>::iterator Pos_Y_itr;
   //std::map<G4int, G4double*>::iterator Pos_Z_itr;
   //std::map<G4int, G4double*>::iterator Ang_Theta_itr;
   //std::map<G4int, G4double*>::iterator Ang_Phi_itr;

   G4THitsMap<G4int>*    CsIShieldDetectorNumberHitMap;
   G4THitsMap<G4double>* CsIShieldEnergyHitMap;
   G4THitsMap<G4double>* CsIShieldTimeHitMap;
   //   G4THitsMap<G4double>* XHitMap;
   //  G4THitsMap<G4double>* YHitMap;
   //G4THitsMap<G4double>* PosXHitMap;
   //G4THitsMap<G4double>* PosYHitMap;
   //G4THitsMap<G4double>* PosZHitMap;
   //G4THitsMap<G4double>* AngThetaHitMap;
   //G4THitsMap<G4double>* AngPhiHitMap;

   // NULL pointer are given to avoid warning at compilation

   // Read the Scorer associate to the LaBr

   //Detector Number
   G4int CsIShieldDetCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("CsIScorerShieldClParis/CsIShieldDetectorNumber")    ;
   CsIShieldDetectorNumberHitMap = (G4THitsMap<G4int>*)(event->GetHCofThisEvent()->GetHC(CsIShieldDetCollectionID))         ;
   CsIShieldDetectorNumber_itr =  CsIShieldDetectorNumberHitMap->GetMap()->begin()                                               ;

   //Energy
   G4int CsIShieldEnergyCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("CsIScorerShieldClParis/CsIShieldEnergy")   ;
   CsIShieldEnergyHitMap = (G4THitsMap<G4double>*)(event->GetHCofThisEvent()->GetHC(CsIShieldEnergyCollectionID))                    ;
   CsIShieldEnergy_itr = CsIShieldEnergyHitMap->GetMap()->begin()                                                          ;

   //Time of Flight
   G4int CsIShieldTimeCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("CsIScorerShieldClParis/CsIShieldTime")    ;
   CsIShieldTimeHitMap = (G4THitsMap<G4double>*)(event->GetHCofThisEvent()->GetHC(CsIShieldTimeCollectionID))                        ;
   CsIShieldTime_itr = CsIShieldTimeHitMap->GetMap()->begin()                                                              ;


   // Check the size of different map
   G4int sizeN = CsIShieldDetectorNumberHitMap->entries();  // number of objects hit by trackID=1 (can be the same object hit several time) 
   G4int sizeE = CsIShieldEnergyHitMap->entries();          // = number of steps with edep non null 
   G4int sizeT = CsIShieldTimeHitMap->entries();
   //G4int sizeX = PosXHitMap->entries();
   //G4int sizeY = PosYHitMap->entries();
   //G4int sizeX = XHitMap->entries();
   //G4int sizeY = YHitMap->entries();

   //G4cout <<"SizeN=" << sizeN << endl;
   //G4cout <<"SizeC=" << sizeC << endl;
   //G4cout <<"SizeN CsI =" << sizeNCsI << endl;
   //G4cout <<"SizeE CsI =" << sizeECsI << endl;

   //DetectorNumberHitMap->PrintAllHits();


    if (sizeE != sizeT) {
      G4cout << "No match size PARIS Event Map: sE:"
      << sizeE << " sT:" << sizeT << endl ;

      //   if (sizeE != sizeX) {
      //G4cout << "No match size PARIS Event Map: sE:"
      //<< sizeE << " sT:" << sizeT << " sX:" << sizeX << " sY:" << sizeY << endl ;
      return;
     }


    //G4cout <<"SizeN=" << sizeN << G4endl;


   if(sizeN>0)
     {

       // Deal with trackID=1:
       G4int N_first= *(CsIShieldDetectorNumber_itr->second);                  // ID of first det hit
       G4int NTrackID =   CsIShieldDetectorNumber_itr->first - N_first;           // first trackID dealt with (not always =1)
       NTrackID *= 1;
       G4double E = *(CsIShieldEnergy_itr->second);
       G4double T = *(CsIShieldTime_itr->second);
      

       //G4cout <<"NTrackID=" << NTrackID << G4endl;
       //G4cout <<"N_first=" << N_first << G4endl;
       //G4cout <<"CrystalNumber_first=" << NCryst << G4endl;
       //G4cout <<"Energy first=" << E << G4endl;
       //G4cout <<"Time first =" << T << G4endl;


       if(sizeN>1)
    {
      CsIShieldEnergy_itr++;
      CsIShieldTime_itr++;
      CsIShieldDetectorNumber_itr++;

      for (G4int l = 1; l < sizeN ; l++) {                    // loop on all the other tracks

 
        G4int N= *(CsIShieldDetectorNumber_itr->second);            // ID of det hit
        NTrackID =   CsIShieldDetectorNumber_itr->first - N;           // ID of the track

        //G4cout <<"l=" << l << G4endl;
        //G4cout <<"N=" << N << G4endl;
        //G4cout <<"DetectorNumber_itr->first =" << DetectorNumber_itr->first << G4endl;
        //G4cout <<"NTrackID=" << NTrackID << G4endl;

        if(N==N_first)
          {
       E += *(CsIShieldEnergy_itr->second);

          }else  // we fill the tree for the first detector hit and move to the next detector hit
       {
         if(E!=0)
           {
             // Fill detector number
             ms_Event->SetPARISCsIShieldEDetectorNbr(m_index["ShieldCl"] + N_first);
             ms_Event->SetPARISCsIShieldTDetectorNbr(m_index["ShieldCl"] + N_first);
             // Fill Energy
             // ms_Event->SetPARISCsIShieldEEnergy(RandGauss::shoot(E, ResoFirstStage));
                 E=RandGauss::shoot(E, ResoFirstStage);
                 ms_Event->SetPARISCsIShieldEEnergy(E); // Fill the tree
                 if(E>EGammaMin && E<EGammaMax) ms_Event->SetPARISCsIShieldEffphpeak(EGamma);

             // Fill Time
             ms_Event->SetPARISCsIShieldTTime(RandGauss::shoot(T, ResoTimeGpd));

           }

         N_first=N;
         E=*(CsIShieldEnergy_itr->second);

       }


        //G4cout <<"Energy=" << E << G4endl;
        //G4cout <<"Time =" << T << G4endl;
       
        // Always fill the tree at the end of the loop:
      if(l==(sizeN-1) && E!=0)
        {
          // Fill detector number
          ms_Event->SetPARISCsIShieldEDetectorNbr(m_index["ShieldCl"] + N_first);
          ms_Event->SetPARISCsIShieldTDetectorNbr(m_index["ShieldCl"] + N_first);
          // Fill Energy
          // ms_Event->SetPARISCsIShieldEEnergy(RandGauss::shoot(E, ResoFirstStage));
                 E=RandGauss::shoot(E, ResoFirstStage);
                 ms_Event->SetPARISCsIShieldEEnergy(E); // Fill the tree
                 if(E>EGammaMin && E<EGammaMax) ms_Event->SetPARISCsIShieldEffphpeak(EGamma);
          // Fill Time
          ms_Event->SetPARISCsIShieldTTime(RandGauss::shoot(T, ResoTimeGpd));          
        }

        CsIShieldEnergy_itr++;
        CsIShieldDetectorNumber_itr++;
      }
    }else
      {
        // Fill the tree if sizeN=1:
        if(E!=0)
          {
          // Fill detector number
          ms_Event->SetPARISCsIShieldEDetectorNbr(m_index["ShieldCl"] + N_first);
          ms_Event->SetPARISCsIShieldTDetectorNbr(m_index["ShieldCl"] + N_first);
          // Fill Energy
          // ms_Event->SetPARISCsIShieldEEnergy(RandGauss::shoot(E, ResoFirstStage));
                 E=RandGauss::shoot(E, ResoFirstStage);
                 ms_Event->SetPARISCsIShieldEEnergy(E); // Fill the tree
                 if(E>EGammaMin && E<EGammaMax) ms_Event->SetPARISCsIShieldEffphpeak(EGamma);

          // Fill Time
          ms_Event->SetPARISCsIShieldTTime(RandGauss::shoot(T, ResoTimeGpd));
          }
      }
   
 
     }
  


      // clear map for next event
      CsIShieldDetectorNumberHitMap    -> clear();
      CsIShieldEnergyHitMap            -> clear();
      CsIShieldTimeHitMap              -> clear();
      //XHitMap                 -> clear();
      //YHitMap                 -> clear();
      //PosXHitMap              -> clear();
      //PosYHitMap              -> clear();
      //PosZHitMap              -> clear();
      //AngThetaHitMap          -> clear();
      //AngPhiHitMap            -> clear();

}



void ShieldClParis::InitializeScorers()
{
 
   // CsIShield Associate Scorer
   m_CsIShieldScorer = new G4MultiFunctionalDetector("CsIScorerShieldClParis");
   
   //   G4VPrimitiveScorer* DetNbr                           = new GENERALSCORERS::PSDetectorNumber("DetectorNumber", "ShieldClParis", 0);
   G4VPrimitiveScorer* DetNbr                           = new SHIELDScorerCsIDetectorNumber("CsIShieldDetectorNumber", "ShieldClParis", 0);
   //   G4VPrimitiveScorer* TOF                              = new GENERALSCORERS::PSTOF("StripTime","ShieldClParis", 0);
   G4VPrimitiveScorer* TOF                              = new SHIELDScorerCsITOF("CsIShieldTime","ShieldClParis", 0);
   //G4VPrimitiveScorer* InteractionCoordinatesX          = new GENERALSCORERS::PSInteractionCoordinatesX("InterCoordX","ShieldClParis", 0);
   //G4VPrimitiveScorer* InteractionCoordinatesY          = new GENERALSCORERS::PSInteractionCoordinatesY("InterCoordY","ShieldClParis", 0);
   //G4VPrimitiveScorer* InteractionCoordinatesZ          = new GENERALSCORERS::PSInteractionCoordinatesZ("InterCoordZ","ShieldClParis", 0);
   //G4VPrimitiveScorer* InteractionCoordinatesAngleTheta = new GENERALSCORERS::PSInteractionCoordinatesAngleTheta("InterCoordAngTheta","ShieldClParis", 0);
   //G4VPrimitiveScorer* InteractionCoordinatesAnglePhi   = new GENERALSCORERS::PSInteractionCoordinatesAnglePhi("InterCoordAngPhi","ShieldClParis", 0);

   G4VPrimitiveScorer* Energy                           = new SHIELDScorerCsIEnergy("CsIShieldEnergy", "ShieldClParis", 0);

   //   G4VPrimitiveScorer* StripPositionX                   = new PARIScorerLaBr3StageFrontStripDummyShape("StripIDFront", 0, NumberOfStrips);
   //  G4VPrimitiveScorer* StripPositionY                   = new PARISScorerLaBr3StageBackStripDummyShape("StripIDBack", 0, NumberOfStrips);

   //and register it to the multifunctionnal detector
   m_CsIShieldScorer->RegisterPrimitive(DetNbr);
   m_CsIShieldScorer->RegisterPrimitive(Energy);
   m_CsIShieldScorer->RegisterPrimitive(TOF);
   //m_LaBr3StageScorer->RegisterPrimitive(StripPositionX);
   //m_LaBr3StageScorer->RegisterPrimitive(StripPositionY);
   //m_LaBr3StageScorer->RegisterPrimitive(InteractionCoordinatesX);
   //m_LaBr3StageScorer->RegisterPrimitive(InteractionCoordinatesY);
   //m_LaBr3StageScorer->RegisterPrimitive(InteractionCoordinatesZ);
   //m_LaBr3StageScorer->RegisterPrimitive(InteractionCoordinatesAngleTheta);
   //m_LaBr3StageScorer->RegisterPrimitive(InteractionCoordinatesAnglePhi);



   //  Add All Scorer to the Global Scorer Manager
   G4SDManager::GetSDMpointer()->AddNewDetector(m_CsIShieldScorer);
  
 
}
