/*****************************************************************************
 * Copyright (C) 2009-2013   this file is part of the NPTool Project         *
 *                                                                           *
 * For the licensing terms see $NPTOOL/Licence/NPTool_Licence                *
 * For the list of contributors see $NPTOOL/Licence/Contributors             *
 *****************************************************************************/

/*****************************************************************************
 * Original Author: N. de Sereville  contact address: deserevi@ipno.in2p3.fr *
 *                                                                           *
 * Creation Date  : march 2011                                               *
 * Last update    :                                                          *
 *---------------------------------------------------------------------------*
 * Decription:                                                               *
 *  This class holds all the online spectra needed for Exogam                 *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Comment:                                                                  *
 *    + first version (not complete yet)                                     *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

// STL
#include <iostream>
#include <cstdlib>
#include <stdexcept>


// NPL
#include "TExogamSpectra.h"
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
TExogamSpectra::TExogamSpectra(){
  fnbinsRaw=16384;
  fbinRawMin=0;
  fbinRawMax=16384;
  fnbinsCal=5000;
  fbinCalMin=0;
  fbinCalMax=5000;

}

////////////////////////////////////////////////////////////////////////////////
TExogamSpectra::TExogamSpectra(unsigned int NumberOfClover){
  if(NPOptionManager::getInstance()->GetVerboseLevel()>0)
    cout << "************************************************" << endl
      << "TExogamSpectra : Initalising control spectra for " 
      << NumberOfClover << " Clover" << endl
      << "************************************************" << endl ;
  fNumberOfClover = NumberOfClover;
  fNumberOfSegments=4;
  fNumberOfCores=4;
  InitRawSpectra();
  InitPreTreatedSpectra();
  InitPhysicsSpectra();
  fnbinsRaw=4096;
  fbinRawMin=0;
  fbinRawMax=16384;
  fnbinsCal=5000;
  fbinCalMin=0;
  fbinCalMax=5000;

}

////////////////////////////////////////////////////////////////////////////////
TExogamSpectra::~TExogamSpectra(){
}

////////////////////////////////////////////////////////////////////////////////
void TExogamSpectra::InitRawSpectra(){
  string name;
  for (unsigned int i = 0; i < fNumberOfClover; i++) { // loop on number of detectors
   for (unsigned int j = 0; j < fNumberOfCores; j++) { // loop on number of cores

    	name = Form("ExogamEnergyRaw_Clover%d_ECC%d", i+1, j+1);
    	AddHisto1D(name, name, 16384, 0, 16384, "Exogam/ERAW/ECC");
    	name = Form("ExogamTimeRaw_Clover%d_ECC%d", i+1, j+1);
    	AddHisto1D(name, name, 16384, 0, 16384, "Exogam/TRAW/ECC");

  	for (unsigned int k = 0; k < fNumberOfSegments; k++) { // loop on number of segments
    	name = Form("ExogamEnergyRaw_Clover%d_ECC%d_GOCCE%d", i+1, j+1, k+1);
    	AddHisto1D(name, name, 16384, 0, 16384, "Exogam/ERAW/GOCCE");
    }
   }
  } // end loop on number of detectors
  
}

////////////////////////////////////////////////////////////////////////////////
void TExogamSpectra::InitPreTreatedSpectra()
{
  string name;
  for (unsigned int i = 0; i < fNumberOfClover; i++) { // loop on number of detectors
   for (unsigned int j = 0; j < fNumberOfCores; j++) { // loop on number of cores
    	name = Form("ExogamEnergyCal_Clover%d_ECC%d", i+1, j+1);
    	AddHisto1D(name, name, 5000, 0, 5000, "Exogam/ECal/ECC");
    	name = Form("ExogamTimeCal_Clover%d_ECC%d", i+1, j+1);
    	AddHisto1D(name, name, 5000, 0, 5000, "Exogam/TCal/ECC");

  	for (unsigned int k = 0; k < fNumberOfSegments; k++) { // loop on number of segments
    	name = Form("ExogamEnergyCal_Clover%d_ECC%d_GOCCE%d", i+1, j+1, k+1);
    	AddHisto1D(name, name, 5000, 0, 5000, "Exogam/ECal/GOCCE");

    }
   }
  } // end loop on number of detectors
}

////////////////////////////////////////////////////////////////////////////////
void TExogamSpectra::InitPhysicsSpectra(){
  string name;
  name = "ExogamEnergyAddBack";
  AddHisto1D(name, name, 5000, 0, 5000, "Exogam/DC");

}



////////////////////////////////////////////////////////////////////////////////
void TExogamSpectra::FillRawSpectra(TExogamData* RawData){
<<<<<<< HEAD
  TString name;
  TString family;
  // Energy and Time RAw 
=======
  string name;
  string family;

  // Energy 
>>>>>>> a51372fab418d17126a214991f612e6da2c5c9be
  for (unsigned int i = 0; i < RawData->GetECCEMult(); i++) {

    name = Form("ExogamEnergyRaw_Clover%d_ECC%d", RawData->GetECCEClover(i)+1,RawData->GetECCECristal(i)+1);
    family = "Exogam/ERAW/ECC";
    GetHisto(family,name)
      -> Fill(RawData->GetECCEEnergy(i));
<<<<<<< HEAD
   }

  for (unsigned int i = 0; i < RawData->GetGOCCEEMult(); i++) {
    name = Form("ExogamEnergyRaw_Clover%d_ECC%d_GOCCE%d", RawData->GetGOCCEEClover(i)+1,RawData->GetGOCCEECristal(i)+1,RawData->GetGOCCEESegment(i)+1);
    family = "Exogam/ERAW/GOCCE";
=======
   
    name = Form("ExogamEnergyRaw_Clover%d_ECC%d_GOCCE%d", RawData->GetECCEClover(i)+1,RawData->GetECCECristal(i)+1,RawData->GetGOCCEESegment(i)+1);
    family = "Exogam/RAW";
>>>>>>> a51372fab418d17126a214991f612e6da2c5c9be

    GetHisto(family,name)
      -> Fill(RawData->GetGOCCEEEnergy(i));
    }

<<<<<<< HEAD
  for (unsigned int i = 0; i < RawData->GetECCTMult(); i++) {
    name = Form("ExogamTimeRaw_Clover%d_ECC%d", RawData->GetECCTClover(i)+1,RawData->GetECCTCristal(i)+1);
    family = "Exogam/TRAW/ECC";

    GetHisto(family,name)
      -> Fill(RawData->GetECCTTime(i));
   }
=======
  // Time
  for (unsigned int i = 0; i < RawData->GetECCTMult(); i++) {
   name = Form("ExogamTimeRaw_Clover%d_ECC%d", RawData->GetECCTClover(i)+1,RawData->GetECCTCristal(i)+1);
    family = "Exogam/RAW";
>>>>>>> a51372fab418d17126a214991f612e6da2c5c9be

    GetHisto(family,name)
      -> Fill(RawData->GetECCTTime(i));
  }
}

////////////////////////////////////////////////////////////////////////////////
void TExogamSpectra::FillPreTreatedSpectra(TExogamData* PreTreatedData){
<<<<<<< HEAD
  TString name ;
  TString family;
  // Energy and Time Cal
=======
  string name ;
  string family;
  // Energy 
>>>>>>> a51372fab418d17126a214991f612e6da2c5c9be
  for (unsigned int i = 0; i < PreTreatedData->GetECCEMult(); i++) {
    name = Form("ExogamEnergyCal_Clover%d_ECC%d", PreTreatedData->GetECCEClover(i)+1,PreTreatedData->GetECCECristal(i)+1);
    family = "Exogam/ECal/ECC";

    GetHisto(family,name)
      -> Fill(PreTreatedData->GetECCEEnergy(i));
  }

  for (unsigned int i = 0; i < PreTreatedData->GetGOCCEEMult(); i++) {   
    name = Form("ExogamEnergyCal_Clover%d_ECC%d_GOCCE%d", PreTreatedData->GetGOCCEEClover(i)+1,PreTreatedData->GetGOCCEECristal(i)+1,PreTreatedData->GetGOCCEESegment(i)+1);
    family = "Exogam/ECal/GOCCE";

    GetHisto(family,name)
      -> Fill(PreTreatedData->GetGOCCEEEnergy(i));
    }

<<<<<<< HEAD
  for (unsigned int i = 0; i < PreTreatedData->GetECCTMult(); i++) {
    name = Form("ExogamTimeCal_Clover%d_ECC%d", PreTreatedData->GetECCTClover(i)+1,PreTreatedData->GetECCTCristal(i)+1);
    family = "Exogam/TCal/ECC";

    GetHisto(family,name)
      -> Fill(PreTreatedData->GetECCTTime(i));

  }
=======
   
    name = Form("ExogamEnergyCal_Clover%d_ECC%d_GOCCE%d", PreTreatedData->GetECCEClover(i)+1,PreTreatedData->GetECCECristal(i)+1,PreTreatedData->GetGOCCEESegment(i)+1);
    family = "Exogam/Cal";

    GetHisto(family,name)
      -> Fill(PreTreatedData->GetGOCCEEEnergy(i));
    }

  // Time
  for (unsigned int i = 0; i < PreTreatedData->GetECCTMult(); i++) {
  name = Form("ExogamTimeCal_Clover%d_ECC%d", PreTreatedData->GetECCTClover(i)+1,PreTreatedData->GetECCTCristal(i)+1);
    family = "Exogam/Cal";

    GetHisto(family,name)
      -> Fill(PreTreatedData->GetECCTTime(i));
   }
>>>>>>> a51372fab418d17126a214991f612e6da2c5c9be

}

////////////////////////////////////////////////////////////////////////////////
void TExogamSpectra::FillPhysicsSpectra(TExogamPhysics* Physics){
  string name;
  string family= "Exogam/PHY";
  // Doppler Correct and Add Back
  name = "ExogamEnergyAddBack";
  family = "Exogam/DC";
  for (unsigned int i = 0; i < Physics->DopplerCorrectedEnergy.size(); i++) {
    GetHisto(family,name)
      -> Fill(Physics->DopplerCorrectedEnergy[i]);
  }

}
////////////////////////////////////////////////////////////////////////////////
TH1* TExogamSpectra::AddHisto1D(string name, string title, Int_t nbinsx, Double_t xlow, Double_t xup, string family){
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
TH1* TExogamSpectra::AddHisto2D(string name, string title, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup, string family){
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
TH1* TExogamSpectra::GetHisto(string family, string name){
vector<string> index;
  index.reserve(2);
  index.push_back(family);
  index.push_back(name);
  TH1* histo ; 

  try{
    histo = fMapHisto.at(index); 
  }

  catch(const std::out_of_range& oor){
    cout << "ERROR : the folowing Histo has been requested by TCATSSpectra and does not exist: family:" << family << " name: "  << name << endl ;
    exit(1);
  }

  return histo;
}


////////////////////////////////////////////////////////////////////////////////
void TExogamSpectra::WriteHisto(string filename){
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
void TExogamSpectra::CheckSpectra(){
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
