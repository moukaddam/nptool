/*****************************************************************************
 * Copyright (C) 2009-2013   this file is part of the NPTool Project         *
 *                                                                           *
 * For the licensing terms see $NPTOOL/Licence/NPTool_Licence                *
 * For the list of contributors see $NPTOOL/Licence/Contributors             *
 *****************************************************************************/

/*****************************************************************************
 * Original Author: Adrien MATTA  contact address: matta@ipno.in2p3.fr       *
 *                                                                           *
 * Creation Date  : September 2009                                           *
 * Last update    :                                                          *
 *---------------------------------------------------------------------------*
 * Decription:                                                               *
 *  This class describe a Modular cylindrical Plastic Scintillator           *
 *   Few Material are instantiate and user can choose position and dimension    * 
 *  but also the adding of a lead plate on the rear side of the detector     *
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
#include "G4Tubs.hh"
#include "G4Box.hh"

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
#include "Plastic.hh"
#include "GeneralScorers.hh"
#include "RootOutput.h"
using namespace GENERALSCORERS ;
// CLHEP header
#include "CLHEP/Random/RandGauss.h"

using namespace std;
using namespace CLHEP;


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
namespace PLASTIC
{
   // Energy and time Resolution
   const G4double ResoTime    = 4.2         ;// = 10ns of Resolution   //   Unit is MeV/2.35
   const G4double ResoEnergy  = 5.0         ;// Resolution in %

}

using namespace PLASTIC ;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// Plastic Specific Method
Plastic::Plastic()
{
   InitializeMaterial();
   m_Event = new TPlasticData() ;
}

Plastic::~Plastic()
{
   delete m_MaterialPlastic_BC400      ; 
   delete m_MaterialPlastic_BC452_2   ;
   delete m_MaterialPlastic_BC452_5   ;
   delete m_MaterialPlastic_BC452_10   ;
   delete m_MaterialLead                     ;
   delete m_MaterialAl                     ;
   delete m_MaterialKapton                    ;
   delete m_PlasticScorer                  ;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void Plastic::AddPlastic(   G4double  R                       ,
                                   G4double  Theta                   ,
                                     G4double  Phi                  ,
                                     G4double   PlasticThickness   ,
                                     G4double   PlasticRadius         ,
                                     G4String    Scintillator         ,
                                     G4double    LeadThickness         )
   {

     m_R.push_back(R)                                         ;
     m_Theta.push_back(Theta)                                ;
     m_Phi.push_back(Phi)                                     ;
     m_PlasticThickness.push_back(PlasticThickness)   ;
       m_LeadThickness.push_back(LeadThickness)            ;
       m_Scintillator.push_back(Scintillator)               ;
    m_PlasticRadius.push_back(PlasticRadius)            ; // cylindrical shape
       m_PlasticHeight.push_back(-1)                              ; // squared shape
      m_PlasticWidth.push_back(-1)                              ; // squared shape
   }

void Plastic::AddPlastic(   G4double R                      ,
                                    G4double Theta                ,
                                    G4double Phi                     ,
                                    G4double Height                  ,
                                    G4double Width                  ,
                                    G4double PlasticThickness   ,
                                    G4String Scintillator         ,
                                    G4double LeadThickness      )
   {
      m_R.push_back(R)                                         ;
     m_Theta.push_back(Theta)                                ;
     m_Phi.push_back(Phi)                                     ;
     m_PlasticThickness.push_back(PlasticThickness)   ;
       m_LeadThickness.push_back(LeadThickness)            ;
       m_Scintillator.push_back(Scintillator)               ;
    m_PlasticRadius.push_back(-1)            ; // cylindrical shape
       m_PlasticHeight.push_back(Height)                        ; // squared shape
      m_PlasticWidth.push_back(Width)                           ; // squared shape
   
   }


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// Virtual Method of VDetector class


// Read stream at Configfile to pick-up parameters of detector (Position,...)
// Called in DetecorConstruction::ReadDetextorConfiguration Method
void Plastic::ReadConfiguration(string Path)
{
   ifstream ConfigFile           ;
   ConfigFile.open(Path.c_str()) ;
   string LineBuffer          ;
   string DataBuffer          ;

   G4double Theta = 0 , Phi = 0 , R = 0 , Thickness = 0 , Radius = 0 , LeadThickness = 0, X = 0 , Y = 0 , Z = 0 , Width = 0 , Height = 0 ;
   G4String Scintillator, Shape ;

   bool check_Theta = false               ;
   bool check_Phi  = false                 ;
   bool check_R     = false               ;
   bool check_Thickness = false        ;
   bool check_Radius = false              ;
   bool check_LeadThickness = false   ;
   bool check_Scintillator = false      ;
   bool check_Height = false             ;
   bool check_Width = false                ;
   bool check_Shape = false                ;
   bool check_X = false                      ;
   bool check_Y = false                      ;
   bool check_Z = false                      ;      
   bool ReadingStatus = false             ;


 while (!ConfigFile.eof()) 
    {
      
            getline(ConfigFile, LineBuffer);

         //   If line is a Start Up Plastic bloc, Reading toggle to true      
            if (LineBuffer.compare(0, 7, "Plastic") == 0) 
               {
                  G4cout << "///" << G4endl           ;
                     G4cout << "Platic found: " << G4endl   ;        
                  ReadingStatus = true ;
                 
               }
         
         //   Else don't toggle to Reading Block Status
         else ReadingStatus = false ;
         
         //   Reading Block
         while(ReadingStatus)
            {
                  // Pickup Next Word 
               ConfigFile >> DataBuffer ;

               //   Comment Line 
               if (DataBuffer.compare(0, 1, "%") == 0) {   ConfigFile.ignore ( std::numeric_limits<std::streamsize>::max(), '\n' );}

                  //   Finding another telescope (safety), toggle out
               else if (DataBuffer.compare(0, 6, "Plastic") == 0) {
                  cout << "WARNING: Another Telescope is find before standard sequence of Token, Error may occured in Telecope definition" << endl ;
                  ReadingStatus = false ;
               }
                              
               //Angle method
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
                  cout << "R:  " << R/mm << endl;
               }
               
               //Position method
               else if (DataBuffer.compare(0, 2, "X=") == 0) {
                  check_X = true;
                  ConfigFile >> DataBuffer ;
                  X = atof(DataBuffer.c_str()) ;
                  X = X * mm;
                  cout << "X:  " << X / mm << endl;
               }

               else if (DataBuffer.compare(0, 2, "Y=") == 0) {
                  check_Y = true;
                  ConfigFile >> DataBuffer ;
                  Y = atof(DataBuffer.c_str()) ;
                  Y = Y * mm;
                  cout << "Y:  " << Y / mm << endl;
               }

               else if (DataBuffer.compare(0, 2, "Z=") == 0) {
                  check_Z = true;
                  ConfigFile >> DataBuffer ;
                  Z = atof(DataBuffer.c_str()) ;
                  Z = Z * mm;
                  cout << "Z:  " << Z / mm << endl;
               }
               
               
               //General
               else if (DataBuffer.compare(0, 6, "Shape=") == 0) {
                  check_Shape = true;
                  ConfigFile >> DataBuffer ;
                  Shape = DataBuffer ;
                  cout << "Shape:  " << Shape << endl;
               }
               
               // Cylindrical shape
               else if (DataBuffer.compare(0, 7, "Radius=") == 0) {
                  check_Radius = true;
                  ConfigFile >> DataBuffer ;
                  Radius = atof(DataBuffer.c_str()) ;
                  Radius = Radius * mm;
                  cout << "Plastic Radius:  " << Radius/mm << endl;
               }
               
               // Squared shape
               else if (DataBuffer.compare(0, 7, "Width=") == 0) {
                  check_Width = true;
                  ConfigFile >> DataBuffer ;
                  Width = atof(DataBuffer.c_str()) ;
                  Width = Width * mm;
                  cout << "Plastic Width:  " << Width/mm << endl;
               }
               
               else if (DataBuffer.compare(0, 7, "Height=") == 0) {
                  check_Height = true;
                  ConfigFile >> DataBuffer ;
                  Height = atof(DataBuffer.c_str()) ;
                  Height = Height * mm;
                  cout << "Plastic Height:  " << Height/mm << endl;
               }
               
               // Common
               else if (DataBuffer.compare(0, 10, "Thickness=") == 0) {
                  check_Thickness = true;
                  ConfigFile >> DataBuffer ;
                  Thickness = atof(DataBuffer.c_str()) ;
                  Thickness = Thickness * mm;
                  cout << "Plastic Thickness:  " << Thickness/mm << endl;
               }
               
               else if (DataBuffer.compare(0, 13, "Scintillator=") == 0) {
                  check_Scintillator = true ;
                  ConfigFile >> DataBuffer ;
                  Scintillator = DataBuffer ;
                  cout << "Plastic Scintillator type:  " << Scintillator << endl;
               }
               
               else if (DataBuffer.compare(0, 14, "LeadThickness=") == 0) {
                  check_LeadThickness = true;
                  ConfigFile >> DataBuffer ;
                  LeadThickness = atof(DataBuffer.c_str()) ;
                  LeadThickness = LeadThickness * mm;
                  cout << "Lead Thickness :  " << LeadThickness/mm << endl;
               }
               
                     ///////////////////////////////////////////////////
                  //   If no Detector Token and no comment, toggle out
                  else 
                     {ReadingStatus = false; G4cout << "Wrong Token Sequence: Getting out " << DataBuffer << G4endl ;}
                  
                     /////////////////////////////////////////////////
                     //   If All necessary information there, toggle out
                  
                  if (       ( check_Theta && check_Phi && check_R && check_Thickness && check_Radius && check_LeadThickness && check_Scintillator && check_Shape) // Cylindrical case
                           ||    ( check_X && check_Y && check_Z && check_Thickness && check_Radius && check_LeadThickness && check_Scintillator )
                  
                           ||   ( check_Theta && check_Phi && check_R && check_Thickness && check_Width && check_Height && check_LeadThickness && check_Scintillator && check_Shape ) // Squared case
                           ||   ( check_X && check_Y && check_Z && check_Thickness && check_Width && check_Height && check_LeadThickness && check_Scintillator )
                        ) 
                     { 
                     
                        if (check_X && check_Y && check_Z)
                           {
                              R       = sqrt (X*X+Y*Y+Z*Z)         ;
                              Theta   =   acos(Z / (R) )      ;
                              Phi    = atan2(Y,X)    ;
                           }
                     
                        if (Shape == "Cylinder")
                          AddPlastic(   R                ,
                                       Theta             ,
                                       Phi               ,
                                       Thickness         ,
                                       Radius            ,
                                       Scintillator   ,
                                       LeadThickness   );
                                    
                    else if (Shape == "Square")
                       AddPlastic(   R                ,
                                       Theta             ,
                                       Phi               ,
                                       Height            ,
                                       Width               ,
                                       Thickness         ,
                                       Scintillator   ,
                                       LeadThickness   );
                        
                       //   Reinitialisation of Check Boolean 
                       
                     check_Theta = false            ;
                     check_Phi  = false              ;
                     check_R     = false            ;
                     check_Thickness = false        ;
                     check_Radius = false           ;
                     check_LeadThickness = false   ;
                     check_Scintillator = false    ;
                     check_Height = false             ;
                     check_Width = false             ;
                     check_Shape = false             ;
                     check_X = false                   ;
                     check_Y = false                   ;
                     check_Z = false                   ;
                     ReadingStatus = false          ;   
                     cout << "///"<< endl             ;            
                     }
                     
            }
   }

}

// Construct detector and inialise sensitive part.
// Called After DetecorConstruction::AddDetector Method
void Plastic::ConstructDetector(G4LogicalVolume* world)
{
      G4ThreeVector Det_pos = G4ThreeVector(0, 0, 0)  ;
   
   for (unsigned short i = 0 ; i < m_R.size() ; i++) 
      {
         G4double wX = m_R[i] * sin(m_Theta[i] ) * cos(m_Phi[i] )   ;
         G4double wY = m_R[i] * sin(m_Theta[i] ) * sin(m_Phi[i] )   ;
         G4double wZ = m_R[i] * cos(m_Theta[i] )                   ;

         Det_pos = G4ThreeVector(wX, wY, wZ)                 ;
//         G4LogicalVolume* logicPlastic = NULL ;
         
      VolumeMaker(Det_pos , i+1, world) ;
    }

}

void Plastic::VolumeMaker(G4ThreeVector Det_pos, int DetNumber, G4LogicalVolume* world)
   {
      ////////////////////////////////////////////////////////////////
      ////////////// Starting Volume Definition //////////////////////
      ////////////////////////////////////////////////////////////////      
      // Name of the module
      std::ostringstream DetectorNumber                  ;
      DetectorNumber << DetNumber                         ;
      G4String Name = "Plastic" + DetectorNumber.str()   ;
      
      int i = DetNumber-1;

      G4Material* PlasticMaterial ;

      if(m_Scintillator[i] == "BC400"    ) PlasticMaterial = m_MaterialPlastic_BC400    ;
      else if(m_Scintillator[i] == "BC452_2"  ) PlasticMaterial = m_MaterialPlastic_BC452_2    ;
      else if(m_Scintillator[i] == "BC452_5"  ) PlasticMaterial = m_MaterialPlastic_BC452_5   ;
      else if(m_Scintillator[i] == "BC452_10" ) PlasticMaterial = m_MaterialPlastic_BC452_10   ;
      else if(m_Scintillator[i] == "Kapton" ) PlasticMaterial = m_MaterialKapton   ;
      else {   
         G4cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl ;
         G4cout << "WARNING: Material Not found, default material set : BC400" << endl ; 
         G4cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl ;
         PlasticMaterial = m_MaterialPlastic_BC400;
      }


      // Definition of the volume containing the sensitive detector
      

      // Cylindrical Case
      if(m_PlasticRadius[i]!=-1)
      {
         if(m_PlasticThickness[i]>0 && m_PlasticRadius[i]>0)
         { 
            G4Tubs* solidPlastic = new G4Tubs(   Name                              ,    
                  0                                    ,
                  m_PlasticRadius[i]         ,
                  m_PlasticThickness[i]/2   ,
                  0*deg                              , 
                  360*deg                           );

            G4LogicalVolume* logicPlastic = new G4LogicalVolume(solidPlastic, PlasticMaterial, Name+ "_Scintillator", 0, 0, 0);
            logicPlastic->SetSensitiveDetector(m_PlasticScorer);

            G4VisAttributes* PlastVisAtt = new G4VisAttributes(G4Colour(0.0, 0.0, 0.9)) ;
            logicPlastic->SetVisAttributes(PlastVisAtt) ;



            new G4PVPlacement(   0                                       ,
                  Det_pos                              ,
                  logicPlastic                   ,
                  Name  + "_Scintillator"   ,
                  world                          ,
                  false                          ,
                  0                                       );   
         }


         if(m_LeadThickness[i]>0&& m_PlasticRadius[i]>0)
         {
            G4Tubs* solidLead = new G4Tubs(   Name+"_Lead"              ,    
                  0                                 ,
                  m_PlasticRadius[i]      ,
                  m_LeadThickness[i]/2   ,
                  0*deg                           , 
                  360*deg                        );

                     G4LogicalVolume* logicLead = new G4LogicalVolume(solidLead, m_MaterialLead, Name+"_Lead", 0, 0, 0);//AC changed lead to Al
                     G4VisAttributes* LeadVisAtt = new G4VisAttributes(G4Colour(0.1, 0.1, 0.1)) ;
                        logicLead->SetVisAttributes(LeadVisAtt) ;
                        
                     G4PVPlacement(   0                                                                                                         ,
                                                                     Det_pos+(m_PlasticThickness[i]/2+m_LeadThickness[i]/2)*Det_pos.unit()   ,
                                                   logicLead                                                                                           ,
                                                   Name+"_Lead"                                                                                   ,   
                                                   world                                                                                            ,
                                                   false                                                                                            ,
                                                   0                                                                                                         );
                 
                 }   
         }
      
      // Squared case
      if(m_PlasticHeight[i]!=-1)
         {
         
            if(m_PlasticThickness[i]>0 && m_PlasticHeight[i]>0 && m_PlasticWidth[i]>0)
               { 
                  G4Box* solidPlastic = new G4Box(Name, 0.5*m_PlasticWidth[i], 0.5*m_PlasticHeight[i], 0.5*m_PlasticThickness[i]);
                  G4LogicalVolume* logicPlastic = new G4LogicalVolume(solidPlastic, PlasticMaterial, Name+ "_Scintillator", 0, 0, 0);
                  logicPlastic->SetSensitiveDetector(m_PlasticScorer);
                  
                  G4VisAttributes* PlastVisAtt = new G4VisAttributes(G4Colour(0.0, 0.0, 0.9)) ;
                  logicPlastic->SetVisAttributes(PlastVisAtt) ;
                 
                  /*new G4PVPlacement(   0                                    ,
                                                                  Det_pos                           ,
                                                logicPlastic                ,
                                                 Name  + "_Scintillator" ,
                                                world                       ,
                                                false                       ,
                                                0                                    );   */
  		G4RotationMatrix Rot3D;
    	Rot3D.set(0, 0, 0);
    		//Rot3D.rotateX(70*degree);
    		//Rot3D.rotateY(theta*degree);  
    		//Rot3D.rotateZ(phi*degree);  
                  new G4PVPlacement(  G4Transform3D(Rot3D,Det_pos)                   ,
                                                logicPlastic                ,
                                                 Name  + "_Scintillator" ,
                                                world                       ,
                                                false                       ,
                                                0                                    );   

               }
            
              if(m_LeadThickness[i]>0&& m_PlasticHeight[i]>0 && m_PlasticWidth[i]>0)
                 {
                    G4Box* solidLead = new G4Box(Name+"_Lead", 0.5*m_PlasticWidth[i], 0.5*m_PlasticHeight[i], 0.5*m_LeadThickness[i]);
                    
                        G4LogicalVolume* logicLead = new G4LogicalVolume(solidLead, m_MaterialLead, Name+"_Lead", 0, 0, 0);
                        G4VisAttributes* LeadVisAtt = new G4VisAttributes(G4Colour(0.1, 0.1, 0.1)) ;
                        logicLead->SetVisAttributes(LeadVisAtt) ;
                        
                        new G4PVPlacement(   0                                                                                                         ,
                                                                        Det_pos+(m_PlasticThickness[i]/2+m_LeadThickness[i]/2)*Det_pos.unit() +G4ThreeVector(0,0,-10*cm)  ,
                                                              logicLead                                                                                             ,
                                                                 Name+"_Lead"                                                                                   ,   
                                                                 world                                                                                            ,
                                                                 false                                                                                            ,
                                                              0                                                                                                         );
                                         
                 }   
         }
      
   }

// Add Detector branch to the EventTree.
// Called After DetecorConstruction::AddDetector Method
void Plastic::InitializeRootOutput()
{
   RootOutput *pAnalysis = RootOutput::getInstance();
   TTree *pTree = pAnalysis->GetTree();
   pTree->Branch("Plastic", "TPlasticData", &m_Event) ;
}

// Read sensitive part and fill the Root tree.
// Called at in the EventAction::EndOfEventAvtion
void Plastic::ReadSensitive(const G4Event* event)
{
   G4String DetectorNumber    ;
   m_Event->Clear()         ;
   
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////// Used to Read Event Map of detector //////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
  
   std::map<G4int, G4int*>::iterator DetectorNumber_itr    ;
   std::map<G4int, G4double*>::iterator Energy_itr        ;
   std::map<G4int, G4double*>::iterator Time_itr          ;
   std::map<G4int, G4double*>::iterator Pos_Z_itr         ;
   
   G4THitsMap<G4int>*     DetectorNumberHitMap            ;      
   G4THitsMap<G4double>* EnergyHitMap                    ;
   G4THitsMap<G4double>* TimeHitMap                      ;
   G4THitsMap<G4double>* PosZHitMap                 ;
   
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

    // Read the Scorer associate to the Silicon Strip
    
   //Detector Number
   G4int StripDetCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("PlasticScorer/PlasticNumber")     ;
   DetectorNumberHitMap = (G4THitsMap<G4int>*)(event->GetHCofThisEvent()->GetHC(StripDetCollectionID))          ;
   DetectorNumber_itr =  DetectorNumberHitMap->GetMap()->begin()                                               ;

   //Energy
   G4int StripEnergyCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("PlasticScorer/Energy")      ;
   EnergyHitMap = (G4THitsMap<G4double>*)(event->GetHCofThisEvent()->GetHC(StripEnergyCollectionID))           ;
   Energy_itr = EnergyHitMap->GetMap()->begin()                                                                ;

   //Time of Flight
   G4int StripTimeCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("PlasticScorer/Time")          ;
   TimeHitMap = (G4THitsMap<G4double>*)(event->GetHCofThisEvent()->GetHC(StripTimeCollectionID))               ;
   Time_itr = TimeHitMap->GetMap()->begin()                                                                    ;

	    //Interaction Coordinate Z
	    G4int InterCoordZCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("PlasticScorer/InterCoordZ");
	    PosZHitMap = (G4THitsMap<G4double>*)(event->GetHCofThisEvent()->GetHC(InterCoordZCollectionID));
	    Pos_Z_itr = PosZHitMap->GetMap()->begin();

    G4int sizeN = DetectorNumberHitMap->entries()    ;
    G4int sizeE = EnergyHitMap->entries()          ;
    G4int sizeT = TimeHitMap->entries()          ;
    G4int sizeZ = PosZHitMap->entries();

    // Loop on Plastic Number
    for (G4int l = 0 ; l < sizeN ; l++) {
        G4int N     =      *(DetectorNumber_itr->second)    ;
        G4int NTrackID  =   DetectorNumber_itr->first - N  ;
        
      
        if (N > 0) {
        
           m_Event->SetPlasticNumber(N) ;

         //  Energy
           Energy_itr = EnergyHitMap->GetMap()->begin();
           for (G4int h = 0 ; h < sizeE ; h++) {
               G4int ETrackID  =   Energy_itr->first  - N      ;
               G4double E     = *(Energy_itr->second)         ;

               if (ETrackID == NTrackID) {
                   m_Event->SetEnergy(RandGauss::shoot(E, E*ResoEnergy/100./2.35))    ;
               }
               
               Energy_itr++;
              }


           //  Time
           Time_itr = TimeHitMap->GetMap()->begin();
           for (G4int h = 0 ; h < sizeT ; h++) {
               G4int TTrackID  =   Time_itr->first   - N    ;
               G4double T     = *(Time_itr->second)      ;

               if (TTrackID == NTrackID) {
                   m_Event->SetTime(RandGauss::shoot(T, ResoTime)) ;
               }
               
               Time_itr++;
           }

	        // Pos Z
	        Pos_Z_itr = PosZHitMap->GetMap()->begin();
	        for (G4int h = 0 ; h < PosZHitMap->entries() ; h++) {
	            G4int PosZTrackID =   Pos_Z_itr->first   - N  ;
	            G4double PosZ     = *(Pos_Z_itr->second)      ;
	            if (PosZTrackID == NTrackID) {
	                ms_InterCoord->SetDetectedPositionZ(PosZ) ;
	            }
	            Pos_Z_itr++;
	        
           }
        }

        DetectorNumber_itr++;
    }
    
    // clear map for next event
    TimeHitMap            ->clear()   ;    
    DetectorNumberHitMap    ->clear()   ;
    EnergyHitMap            ->clear()    ; 
    PosZHitMap					->clear() ;
   
}

////////////////////////////////////////////////////////////////
void Plastic::InitializeMaterial()
   {
   
      ////////////////////////////////////////////////////////////////
      /////////////////Element  Definition ///////////////////////////
      ////////////////////////////////////////////////////////////////
         G4String symbol                                 ;
         G4double density = 0. , a = 0, z = 0               ;
         G4int ncomponents = 0, natoms = 0, fractionmass = 0   ;

         // for Plastic
            G4Element* H   = new G4Element("Hydrogen"    , symbol = "H"     , z = 1  , a = 1.01   * g / mole);
            G4Element* C   = new G4Element("Carbon"      , symbol = "C"     , z = 6  , a = 12.011 * g / mole);
            G4Element* O   = new G4Element("Oxygen"    , symbol = "O"     , z = 8  , a = 16   * g / mole);
          G4Element* Pb  = new G4Element("Lead"      , symbol = "Pb"     , z = 82 , a = 207.2  * g / mole);
      ////////////////////////////////////////////////////////////////
      /////////////////Material Definition ///////////////////////////
      ////////////////////////////////////////////////////////////////

         // Pb
         a = 207.2 * g / mole;
         density = 11.34 * g / cm3;
         m_MaterialLead = new G4Material("Lead", z = 82 , a, density);

         // Al
         a = 13 * g / mole;
         density = 2.7 * g / cm3;
         m_MaterialAl = new G4Material("Al", z = 13 , a, density);

         // Plastic BC-400
         density = 1.032 * g / cm3;
         m_MaterialPlastic_BC400 = new G4Material("Plastic_BC400", density, ncomponents = 2);
         m_MaterialPlastic_BC400->AddElement(H , natoms = 10);
         m_MaterialPlastic_BC400->AddElement(C  , natoms = 9);
         
         // Plastic BC-452 Pb 2%
         density = 1.05 * g / cm3;
         m_MaterialPlastic_BC452_2 = new G4Material("Plastic_BC452_2", density, ncomponents = 3);
         m_MaterialPlastic_BC452_2->AddElement(H  , natoms = 10);
         m_MaterialPlastic_BC452_2->AddElement(C  , natoms = 9);
         m_MaterialPlastic_BC452_2->AddElement(Pb , fractionmass=2*perCent);

         // Plastic BC-452 Pb 5%
         density = 1.08 * g / cm3;
         m_MaterialPlastic_BC452_5 = new G4Material("Plastic_BC452_5", density, ncomponents = 3);
         m_MaterialPlastic_BC452_5->AddElement(H  , natoms = 10);
         m_MaterialPlastic_BC452_5->AddElement(C  , natoms = 9);
         m_MaterialPlastic_BC452_5->AddElement(Pb , fractionmass=5*perCent);

         // Plastic BC-452 Pb 10%
         density = 1.17 * g / cm3;
         m_MaterialPlastic_BC452_10 = new G4Material("Plastic_BC452_10", density, ncomponents = 3);
         m_MaterialPlastic_BC452_10->AddElement(H  , natoms = 10);
         m_MaterialPlastic_BC452_10->AddElement(C  , natoms = 9);
         m_MaterialPlastic_BC452_10->AddElement(Pb , fractionmass=10*perCent);         

         // Kapton    
	 density = 1.39*g/cm3;
	 m_MaterialKapton = new G4Material("Kapton", density, ncomponents=3);
	 m_MaterialKapton->AddElement(O,2);
	 m_MaterialKapton->AddElement(C,5);
	 m_MaterialKapton->AddElement(H,4);
   
   }

////////////////////////////////////////////////////////////////   
void Plastic::InitializeScorers() 
   { 
      m_PlasticScorer = new G4MultiFunctionalDetector("PlasticScorer") ;
      G4SDManager::GetSDMpointer()->AddNewDetector(m_PlasticScorer);
      
      G4VPrimitiveScorer* DetNbr = new PSDetectorNumber("PlasticNumber","Plastic", 0) ;
      G4VPrimitiveScorer* Energy = new PSEnergy("Energy","Plastic", 0)                   ;
      G4VPrimitiveScorer* Time   = new PSTOF("Time","Plastic", 0)                         ;
      G4VPrimitiveScorer* InteractionCoordinatesZ  			= new GENERALSCORERS::PSInteractionCoordinatesZ("InterCoordZ","Plastic", 0);       
      //and register it to the multifunctionnal detector
      m_PlasticScorer->RegisterPrimitive(DetNbr)                         ;
      m_PlasticScorer->RegisterPrimitive(Energy)                         ;
      m_PlasticScorer->RegisterPrimitive(Time)                            ;      
      m_PlasticScorer->RegisterPrimitive(InteractionCoordinatesZ);      
      G4SDManager::GetSDMpointer()->AddNewDetector(m_PlasticScorer) ;
      
   }
////////////////////////////////////////////////////////////////
