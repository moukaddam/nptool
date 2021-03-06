/*****************************************************************************
 * Copyright (C) 2009-2013   this file is part of the NPTool Project         *
 *                                                                           *
 * For the licensing terms see $NPTOOL/Licence/NPTool_Licence                *
 * For the list of contributors see $NPTOOL/Licence/Contributors             *
 *****************************************************************************/

/*****************************************************************************
 * Original Author: A. MATTA  contact address: a.matta@surrey.ac.uk          *
 *                                                                           *
 * Creation Date  : dec 2013                                                 *
 * Last update    :                                                          *
 *---------------------------------------------------------------------------*
 * Decription:                                                               *
 *  This class holds all the online spectra needed for TiaraBarrel           *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Comment:                                                                  *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

// STL
#include <iostream>
#include <cstdlib>
#include <stdexcept>

// NPL
#include "TTiaraBarrelSpectra.h"
#include "NPOptionManager.h"
#include "NPGlobalSystemOfUnits.h"
#include "NPPhysicalConstants.h"
#ifdef NP_SYSTEM_OF_UNITS_H
using namespace NPUNITS;
#endif


// ROOT
#include "string.h"
#include "TDirectory.h"
#include "TFile.h"


////////////////////////////////////////////////////////////////////////////////
TTiaraBarrelSpectra::TTiaraBarrelSpectra(){
  fNumberOfDetector= 0;
  fInnerBarrelStrip=4;
  fOuterBarrelStrip=4;
}

////////////////////////////////////////////////////////////////////////////////
TTiaraBarrelSpectra::TTiaraBarrelSpectra(unsigned int NumberOfDetector){
  if(NPOptionManager::getInstance()->GetVerboseLevel()>0)
    cout << "************************************************" << endl
      << "TTiaraBarrelSpectra : Initalising control spectra for " 
      << NumberOfDetector << " Detectors" << endl
      << "************************************************" << endl ;

  fNumberOfDetector= NumberOfDetector;
  fInnerBarrelStrip=4;
  fOuterBarrelStrip=4;

  InitRawSpectra();
  InitPreTreatedSpectra();
  InitPhysicsSpectra();
}

////////////////////////////////////////////////////////////////////////////////
TTiaraBarrelSpectra::~TTiaraBarrelSpectra(){
}

////////////////////////////////////////////////////////////////////////////////
void TTiaraBarrelSpectra::InitRawSpectra(){
  string name;
  string BaseFamily = "TIARA/BARREL/RAW/";

  //// HIT ////
  // Inner Barrel
  // Hit Up Stream
  name = "INNER_BARREL_US_HIT_RAW";
  AddHisto1D(name, name, fNumberOfDetector*fInnerBarrelStrip, 1, fNumberOfDetector*fInnerBarrelStrip+1, BaseFamily+"HIT");
  //Hit Down Stream
  name = "INNER_BARREL_DS_HIT_RAW";
  AddHisto1D(name, name, fNumberOfDetector*fInnerBarrelStrip, 1, fNumberOfDetector*fInnerBarrelStrip+1, BaseFamily+"HIT");
  // Hit Back
  name = "INNER_BARREL_BACK_HIT_RAW";
  AddHisto1D(name, name, fNumberOfDetector, 1, fNumberOfDetector+1, BaseFamily+"HIT");

  // Outer Barrel
  //Hit Strip
  name = "OUTER_BARREL_STRIP_HIT_RAW";
  AddHisto1D(name, name, fNumberOfDetector*fOuterBarrelStrip, 1, fNumberOfDetector*fOuterBarrelStrip+1, BaseFamily+"HIT");

  //// E ////
  // Inner Barrel
  // E Up Stream
  name = "INNER_BARREL_US_E_RAW";
  AddHisto2D(name, name, fNumberOfDetector*fInnerBarrelStrip, 1, fNumberOfDetector*fInnerBarrelStrip+1, 512,0,16384,BaseFamily+"E");
  //E Down Stream
  name = "INNER_BARREL_DS_E_RAW";
  AddHisto2D(name, name, fNumberOfDetector*fInnerBarrelStrip, 1, fNumberOfDetector*fInnerBarrelStrip+1,512,0,16384, BaseFamily+"E");
  //E Back
  name = "INNER_BARREL_BACK_E_RAW";
  AddHisto2D(name, name, fNumberOfDetector, 1, fNumberOfDetector+1,512,0,16384, BaseFamily+"E");

  // Outer Barrel
  //E Strip
  name = "OUTER_BARREL_STRIP_E_RAW";
  AddHisto2D(name, name, fNumberOfDetector*fOuterBarrelStrip, 1, fNumberOfDetector*fOuterBarrelStrip+1,512,0,16384, BaseFamily+"E");

  //// VS ////
  // Inner Barrel
  for(unsigned int i  = 0 ; i < fNumberOfDetector ; i++){
    for(unsigned int j = 0 ; j < fInnerBarrelStrip;j++){
      name = "IB"+TiaraBarrel_LOCAL::itoa(i+1)+"_VS"+TiaraBarrel_LOCAL::itoa(j+1)+"_RAW";
      AddHisto2D(name, name,1024,0,0,1024,0,0,BaseFamily+"VS");
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
void TTiaraBarrelSpectra::InitPreTreatedSpectra(){
  string BaseFamily = "TIARA/BARREL/CAL/";
  string name ;
  //// VS ////
  // Inner Barrel
  for(unsigned int i  = 0 ; i < fNumberOfDetector ; i++){
    for(unsigned int j = 0 ; j < fInnerBarrelStrip;j++){
      name = "IB"+TiaraBarrel_LOCAL::itoa(i+1)+"_VS"+TiaraBarrel_LOCAL::itoa(j+1)+"_CAL";
      AddHisto2D(name,name,2048,0,0,2048,0,0,BaseFamily+"VS");
    }
    name = "IB"+TiaraBarrel_LOCAL::itoa(i+1)+"_VS_BACK_CAL"; 
    AddHisto2D(name,name,2048,0,0,2048,0,0,BaseFamily+"VS");
  }
}

////////////////////////////////////////////////////////////////////////////////
void TTiaraBarrelSpectra::InitPhysicsSpectra(){
string BaseFamily = "TIARA/BARREL/PHY";
string name ;
  //// E POS ////
  // Inner Barrel
  for(unsigned int i  = 0 ; i < fNumberOfDetector ; i++){
    for(unsigned int j = 0 ; j < fInnerBarrelStrip;j++){
      name = "IB"+TiaraBarrel_LOCAL::itoa(i+1)+"_EPOS"+TiaraBarrel_LOCAL::itoa(j+1)+"_CAL";
      AddHisto2D(name, name,1000,-0.5,1.5,1000,0,30,BaseFamily);
    }
  }
  //// E Theta ////
  // Inner Barrel
  for(unsigned int i  = 0 ; i < fNumberOfDetector ; i++){
    for(unsigned int j = 0 ; j < fInnerBarrelStrip;j++){
      name = "IB"+TiaraBarrel_LOCAL::itoa(i+1)+"_ETHETA"+TiaraBarrel_LOCAL::itoa(j+1)+"_CAL";
      AddHisto2D(name, name,360,0,180,1000,0,30,BaseFamily);
    }
  }
  //// E Theta ////
  // Inner Barrel
 name = "IB_ETHETA_CAL";
 AddHisto2D(name, name,360,0,180,1000,0,30,BaseFamily);

}

////////////////////////////////////////////////////////////////////////////////
void TTiaraBarrelSpectra::FillRawSpectra(TTiaraBarrelData* RawData){
  string name;
  string family;
  string BaseFamily = "TIARA/BARREL/RAW/";

  // INNER_BARREL_US_HIT_RAW
  for (unsigned int i = 0; i < RawData->GetFrontUpstreamEMult(); i++) {
    name = "INNER_BARREL_US_HIT_RAW";
    family = BaseFamily+"HIT";

    GetHisto(family,name)
      ->Fill((RawData->GetFrontUpstreamEDetectorNbr(i)-1)*fInnerBarrelStrip
          +RawData->GetFrontUpstreamEStripNbr(i));

    family = BaseFamily+"E";
    name = "INNER_BARREL_US_E_RAW";
    GetHisto(family,name)
      ->Fill((RawData->GetFrontUpstreamEDetectorNbr(i)-1)*fInnerBarrelStrip
          +RawData->GetFrontUpstreamEStripNbr(i),RawData->GetFrontUpstreamEEnergy(i));
  }
  // INNER_BARREL_DS_HIT_RAW
  for (unsigned int i = 0; i < RawData->GetFrontDownstreamEMult(); i++) {
    name = "INNER_BARREL_DS_HIT_RAW";
    family = BaseFamily+"HIT";

    GetHisto(family,name)
      ->Fill((RawData->GetFrontDownstreamEDetectorNbr(i)-1)*fInnerBarrelStrip
          +RawData->GetFrontDownstreamEStripNbr(i));

    name = "INNER_BARREL_DS_E_RAW";
    family = BaseFamily+"E";
    GetHisto(family,name)
      ->Fill((RawData->GetFrontDownstreamEDetectorNbr(i)-1)*fInnerBarrelStrip
          +RawData->GetFrontDownstreamEStripNbr(i),RawData->GetFrontDownstreamEEnergy(i));
  }

  // INNER_BARREL_BACK_HIT_RAW
  for (unsigned int i = 0; i < RawData->GetBackEMult(); i++) {
    name = "INNER_BARREL_BACK_HIT_RAW";
    family = BaseFamily+"HIT";

    GetHisto(family,name)
      ->Fill((RawData->GetBackEDetectorNbr(i)-1));

    name = "INNER_BARREL_BACK_E_RAW";
    family = BaseFamily+"E";
    GetHisto(family,name)
      ->Fill((RawData->GetBackEDetectorNbr(i)-1), RawData->GetBackEEnergy(i));
  }

  // OUTER_BARREL_STRIP_HIT_RAW
  for (unsigned int i = 0; i < RawData->GetOuterEMult(); i++) {
    name = "OUTER_BARREL_STRIP_HIT_RAW";
    family = BaseFamily+"HIT";

    GetHisto(family,name)
      ->Fill((RawData->GetOuterEDetectorNbr(i)-1)*fInnerBarrelStrip
          +RawData->GetOuterEStripNbr(i));
    
    family = BaseFamily+"E";
    name = "OUTER_BARREL_STRIP_E_RAW";
    GetHisto(family,name)
      ->Fill((RawData->GetOuterEDetectorNbr(i)-1)*fInnerBarrelStrip
          +RawData->GetOuterEStripNbr(i),RawData->GetOuterEEnergy(i));
  }
  
  // INNER_BARREL_VS_RAW                    
  family = BaseFamily+"VS";
  for (unsigned int i = 0; i < RawData->GetFrontUpstreamEMult(); i++) {
    int UpStreamDetNbr = RawData->GetFrontUpstreamEDetectorNbr(i);
    int UpStreamStrNbr = RawData->GetFrontUpstreamEStripNbr(i);
    
    for (unsigned int j = 0; j < RawData->GetFrontDownstreamEMult(); j++) {
      int DoStreamDetNbr = RawData->GetFrontDownstreamEDetectorNbr(j);
      int DoStreamStrNbr = RawData->GetFrontDownstreamEStripNbr(j);
     if(UpStreamDetNbr==DoStreamDetNbr && UpStreamStrNbr==DoStreamStrNbr){
       name = "IB"+TiaraBarrel_LOCAL::itoa(UpStreamDetNbr)+"_VS"+TiaraBarrel_LOCAL::itoa(UpStreamStrNbr)+"_RAW"; 
       GetHisto(family,name)
        ->Fill(RawData->GetFrontUpstreamEEnergy(i),RawData->GetFrontDownstreamEEnergy(j));
      } 
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
void TTiaraBarrelSpectra::FillPreTreatedSpectra(TTiaraBarrelData* PreTreatedData){
string BaseFamily = "TIARA/BARREL/CAL/";
// INNER_BARREL_VS_CAL                 
  string family = BaseFamily+"VS";
  string name ;

  unsigned int sizeU = PreTreatedData->GetFrontUpstreamEMult();
  unsigned int sizeD = PreTreatedData->GetFrontDownstreamEMult();
  unsigned int sizeB = PreTreatedData->GetBackEMult(); 
  
  for (unsigned int i = 0; i < sizeU ; i++) {
    int UpStreamDetNbr = PreTreatedData->GetFrontUpstreamEDetectorNbr(i);
    int UpStreamStrNbr = PreTreatedData->GetFrontUpstreamEStripNbr(i);
    
    for (unsigned int j = 0; j < sizeD ; j++) {
      int DoStreamDetNbr = PreTreatedData->GetFrontDownstreamEDetectorNbr(j);
      int DoStreamStrNbr = PreTreatedData->GetFrontDownstreamEStripNbr(j);
     if(UpStreamDetNbr==DoStreamDetNbr && UpStreamStrNbr==DoStreamStrNbr){
      name = "IB"+TiaraBarrel_LOCAL::itoa(UpStreamDetNbr)+"_VS"+TiaraBarrel_LOCAL::itoa(UpStreamStrNbr)+"_CAL"; 
       GetHisto(family,name)
         ->Fill(PreTreatedData->GetFrontUpstreamEEnergy(i),PreTreatedData->GetFrontDownstreamEEnergy(j));
     
        for (unsigned int k = 0; k < sizeB; k++) {
          if(UpStreamDetNbr == PreTreatedData->GetBackEDetectorNbr(k)){     
            name = "IB"+TiaraBarrel_LOCAL::itoa(UpStreamDetNbr)+"_VS_BACK_CAL"; 
            GetHisto(family,name)
              ->Fill(PreTreatedData->GetFrontUpstreamEEnergy(i)
                      +PreTreatedData->GetFrontDownstreamEEnergy(j),
                      PreTreatedData->GetBackEEnergy(k));
          }
        }
      } 
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
void TTiaraBarrelSpectra::FillPhysicsSpectra(TTiaraBarrelPhysics* Physics){
  string family = "TIARA/BARREL/PHY";
  string name ;
  //// E POS ////
  // Inner Barrel
  //unsigned int size = Physics->Strip_E.size();
  for(unsigned int i  = 0 ; i < Physics->Strip_E.size() ; i++){
    name ="IB"+TiaraBarrel_LOCAL::itoa(Physics->DetectorNumber[i])+"_EPOS"+TiaraBarrel_LOCAL::itoa(Physics->Strip_N[i])+"_CAL";
    GetHisto(family,name)
      ->Fill(Physics->Strip_Pos[i],Physics->Strip_E[i]);

    name = "IB"+TiaraBarrel_LOCAL::itoa(Physics->DetectorNumber[i])+"_ETHETA"+TiaraBarrel_LOCAL::itoa(Physics->Strip_N[i])+"_CAL";
    double Theta = Physics->GetPositionOfInteraction(i).Angle(TVector3(0,0,1));

    GetHisto(family,name)
      ->Fill(Theta*rad/deg,Physics->Strip_E[i]);

    name = "IB_ETHETA_CAL";
    GetHisto(family,name)
      ->Fill(Theta*rad/deg,Physics->Strip_E[i]);
  }
}

////////////////////////////////////////////////////////////////////////////////
TH1* TTiaraBarrelSpectra::AddHisto1D(string name, string title, Int_t nbinsx, Double_t xlow, Double_t xup, string family){
  // create histo
  TH1 *hist = new TH1D(name.c_str(), title.c_str(), nbinsx, xlow, xup);

  vector<string> index ;
  index.push_back(family);
  index.push_back(name);

  // fill map
  fMapHisto[index]=hist;

  return hist;
}

////////////////////////////////////////////////////////////////////////////////
TH1* TTiaraBarrelSpectra::AddHisto2D(string name, string title, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup, string family){
  // create histo
  TH1 *hist = new TH2D(name.c_str(), title.c_str(), nbinsx, xlow, xup, nbinsy, ylow, yup);

  vector<string> index ;
  index.push_back(family);
  index.push_back(name);

  // fill map
  fMapHisto[index]=hist;

  return hist;
}

////////////////////////////////////////////////////////////////////////////////
TH1* TTiaraBarrelSpectra::GetHisto(string& family, string& name){
  vector<string> index;
  index.reserve(2);
  index.push_back(family);
  index.push_back(name);
  TH1* histo ; 

  try{
    histo = fMapHisto.at(index); 
  }

  catch(const std::out_of_range& oor){
    cout << "ERROR : the folowing Histo has been requested by TTiaraBarrelSpectra and does not exist: family:" << family << " name: "  << name << endl ;
    exit(1);
  }

  return histo;
}

////////////////////////////////////////////////////////////////////////////////
void TTiaraBarrelSpectra::WriteHisto(string filename){
  TFile* f=NULL; 

  if(filename!="VOID"){
    f = new TFile(filename.c_str(),"RECREATE");
  }

  map< vector<string>, TH1* >::iterator it;

  for (it=fMapHisto.begin(); it!=fMapHisto.end(); ++it){
    it->second->Write();
  }

  if(filename!="VOID"){
    f->Close();
    delete f;
  }

}
///////////////////////////////////////////////////////////////////////////////
void TTiaraBarrelSpectra::CheckSpectra(){
  map< vector<string>, TH1* >::iterator it;
  Color_t ok_color = kTeal+9;
  Color_t warning_color = kOrange+8;   warning_color *= 1;
  Color_t bad_color = kRed;            bad_color *= 1;

  for (it=fMapHisto.begin(); it!=fMapHisto.end(); ++it){
    it->second->SetLineColor(ok_color);
    it->second->SetFillStyle(1001);
    it->second->SetFillColor(ok_color);
  }

}
