#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include "TProfile.h"
#include "TMath.h"
#include "TH1.h"
#include <TLegend.h>
#include <iostream>
#include <fstream>
#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TComplex.h>
#include <TString.h>

using std::cout;
using std::endl;
using std::cerr;
#define sqr(x) ((x)*(x))
double CentB(double bimp)
{
  // Hard coded centrality defenition
  // based on the impact parameter
  double fcent;
  if (bimp < 2.91)
    fcent = 2.5; // 0-5%
  
  else if (bimp < 4.17)
    fcent = 7.5; // 5-10%
    
  else if (bimp < 5.18)
    fcent = 12.5; // 10-15%
  else if (bimp < 6.02)
    fcent = 17.5; // 15-20%
  else if (bimp < 6.74)
    fcent = 22.5; // 20-25%
  else if (bimp < 7.38)
    fcent = 27.5; // 25-30%
  else if (bimp < 7.97)
    fcent = 32.5; // 30-35%
  else if (bimp < 8.53)
    fcent = 37.5; // 35-40%
  else if (bimp < 9.06)
    fcent = 42.5; // 40-45%
  else if (bimp < 9.56)
    fcent = 47.5; // 45-50%
  else if (bimp < 10.05)
    fcent = 52.5; // 50-55%
  else if (bimp < 10.50)
    fcent = 57.5; // 55-60%
  else if (bimp < 10.94)
    fcent = 62.5; // 60-65%
  else if (bimp < 11.35)
    fcent = 67.5; // 65-70%
  else if (bimp < 11.76)
    fcent = 72.5; // 70-75%
  else if (bimp < 12.19)
    fcent = 77.5; // 75-80%
  else
    fcent = -1;
  return fcent;
}

int GetCentBin(double cent)
{
  if (cent == -1)
    return -1;
  if (cent == 2.5)
    return 0;
  if (cent == 7.5)
    return 1;
  if (cent == 12.5)
    return 2;
  if (cent == 17.5)
    return 3;
  if (cent == 22.5)
    return 4;
  if (cent == 27.5)
    return 5;
  if (cent == 32.5)
    return 6;
  if (cent == 37.5)
    return 7;
  if (cent == 42.5)
    return 8;
  if (cent == 47.5)
    return 9;
  if (cent == 52.5)
    return 10;
  if (cent == 57.5)
    return 11;
  if (cent == 62.5)
    return 12;
  if (cent == 67.5)
    return 13;
  if (cent == 72.5)
    return 14;
  if (cent == 77.5)
    return 15;            
  return -1;
}

void GetRes(TProfile *const &pr)
{
  cout << "const double res2[" << pr->GetNbinsX() <<"] = {";
  for (int i=0; i<pr->GetNbinsX()-1; i++)
  {
    cout << TMath::Sqrt(pr->GetBinContent(i+1)) <<", ";
  }
  cout << TMath::Sqrt(pr->GetBinContent(pr->GetNbinsX())) <<"};" << endl;
}

void GetMultMean(TProfile *const &pr)
{
  cout << "const double dMultMean[" << pr->GetNbinsX() <<"] = {";
  for (int i=0; i<pr->GetNbinsX()-1; i++)
  {
    cout << (pr->GetBinContent(i+1)) <<", ";
  }
  cout <<(pr->GetBinContent(pr->GetNbinsX()))<<"};" << endl;
}

double GetR0(TH1F *const &hist)
{
  //find the first minimum of the square of the modulus of Gtheta 

  int iNbins = hist->GetNbinsX();
  double dR0 = 0.; 

  for (int b=2;b<iNbins;b++)
  {
    double dG0 = hist->GetBinContent(b);
    double dGnext = hist->GetBinContent(b+1);
    double dGnextnext = hist->GetBinContent(b+2);
    // cout << hist->GetBinCenter(b);
    if (dGnext > dG0 && dGnextnext > dG0 && dG0<1.)
    {
      double dGlast = hist->GetBinContent(b-1);
      double dXlast = hist->GetBinCenter(b-1);
      double dX0 = hist->GetBinCenter(b);
      double dXnext = hist->GetBinCenter(b+1);

      dR0 = dX0 - ((dX0-dXlast)*(dX0-dXlast)*(dG0-dGnext) - (dX0-dXnext)*(dX0-dXnext)*(dG0-dGlast))/
        (2.*((dX0-dXlast)*(dG0-dGnext) - (dX0-dXnext)*(dG0-dGlast))); //parabolic interpolated minimum
      break; //stop loop if minimum is found
    } //if

  }//b

      
  return dR0;
}

TH1F* FillHistGtheta(TProfile *const &prReGtheta, TProfile *const &prImGtheta)
{
  Int_t iNbins = prReGtheta->GetNbinsX();
  Double_t xMin = prReGtheta->GetXaxis()->GetBinLowEdge(1);
  Double_t xMax = prReGtheta->GetXaxis()->GetBinLowEdge(iNbins) + prReGtheta->GetXaxis()->GetBinWidth(iNbins);
  TH1F* hGtheta = new TH1F("","",iNbins,xMin,xMax);
  for (int rbin = 0; rbin < iNbins; rbin++)
  {
    // get bincentre of bins in histogram
    Double_t dRe = prReGtheta->GetBinContent(rbin+1);
    Double_t dIm = prImGtheta->GetBinContent(rbin+1);
    TComplex cGtheta(dRe,dIm);
    //fill fHistGtheta with the modulus squared of cGtheta
    //to avoid errors when using a merged outputfile use SetBinContent() and not Fill()
    if (cGtheta.Rho2()>3.) hGtheta->SetBinContent(rbin+1,0);
    else hGtheta->SetBinContent(rbin+1,cGtheta.Rho2());
    // hGtheta->SetBinContent(rbin+1,cGtheta.Rho2());
    hGtheta->SetBinError(rbin+1,0.0);
  }
  return hGtheta;
}

double BesselJ0(double x)
{
  double temp=1., xn=1.;
  long n, Nmax;

  Nmax=int(floor(2.*x)+4);
  for (n=1;n<Nmax;n++)
  {
    xn*=(-sqr(x/2./((float) n)));
    temp+=xn;
  }
  return temp;
}

TComplex Qstar(TComplex Q){
   TComplex QStar   = TComplex::Conjugate(Q);
   return QStar;
}

Double_t CalCor22(TComplex Q2, Double_t M, Double_t w2){
   // single-event average 2-particle azimuthal correlation <2>

  Double_t Q2Square = Q2.Rho2();
  Double_t coor22   = Q2Square - M;                                          
  
  return coor22/w2;
}

Double_t CalCor24(TComplex Q2, TComplex Q4, Double_t M, Double_t w4){
   // single-event average 4-particle azimuthal correlation <4>

   TComplex Q2Star   = Qstar(Q2);
   TComplex Q4Star   = Qstar(Q4);
   
   Double_t Q2Square = Q2.Rho2();
   Double_t Q4Square = Q4.Rho2();
   Double_t ReQQQ    = (Q4 * Q2Star * Q2Star).Re();

   Double_t coor24   = (Q2Square*Q2Square + Q4Square - 2.*ReQQQ
                        - 4.*(M-2.)*Q2Square + 2.*M*(M-3.));

   return coor24/w4;
}

Double_t CalRedCor22(TComplex Q2, TComplex p2, Double_t M, Double_t mp, 
                     Double_t mq, Double_t wred2){

   // Calculate the average reduced single-event 2-particle correlations                      
   TComplex Q2Star = TComplex::Conjugate(Q2);
   Double_t coor22 = (p2*Q2Star-mq).Re();

   return coor22/wred2;
}

Double_t CalRedCor24(TComplex Q2, TComplex Q4, TComplex p2, TComplex q2,
                            TComplex q4, Double_t M, Double_t mp, Double_t mq, Double_t wred4){

   // Calculate the average reduced single-event 2-particle correlations                      
   TComplex Q2Star = TComplex::Conjugate(Q2);
   TComplex Q4Star = TComplex::Conjugate(Q4);
   TComplex q2Star = TComplex::Conjugate(q2);
   Double_t Q2Square = Q2.Rho2();
   TComplex coorc = p2*Q2*Q2Star*Q2Star-q4*Q2Star*Q2Star-p2*Q2*Q4Star
                  - 2.0*M*p2*Q2Star-2.0*mq*Q2Square+7.0*q2*Q2Star
                  - Q2*q2Star+q4*Q4Star+2.0*p2*Q2Star
                  + 2.0*mq*M-6.0*mq;
             
   Double_t coor24 = coorc.Re(); 
   return coor24/wred4;
}

void ToyModelTreeReader(TString inputFileName = "ToyModel.root", TString outputFileName = "test.root", TString inputFileHist="", Bool_t bFirstRun = 1, TString inputFileHist2="" , bool bTemporaryFlagForLYZEP = 0)
{
  // bool bTemporaryFlagForLYZEP = 0;
  const int ncent = 16; // 0-80%
  const double bin_cent[ncent + 1] = {0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80};
  const int npt = 12; // 0.2 - 3.5 GeV/c
  const double bin_pT[npt + 1] = {0.2, 0.4, 0.6, 0.8, 1.0, 1.2, 1.4, 1.6, 1.8, 2.2, 2.6, 3.0, 3.5};
  const double maxpt = 3.5; // max pt
  const double minpt = 0.2; // min pt
  const double eta_cut = 2.0;
  const double eta_gap = 0;

  const int neta = 2; // [eta-,eta+]
  const int max_nh = 6000;

  // LYZ
  bool bUseProduct = 1;
  const int rbins = 1000;
  const double rMax = 0.5;
  const double rMin = 0.005;

  const double rMaxSum = rMax;
  const double rMinSum = rMin;

  // const double rMaxSum = 250;
  // const double rMinSum = 0;
  const int thetabins = 5;
  const double rootJ0 = 2.4048256;
  // const double J1rootJ0 = 0.519147;
  double theta[thetabins];
  double multPOI[npt];
  for (int thetabin = 0; thetabin < thetabins; ++thetabin)
  {
    theta[thetabin] = thetabin * TMath::Pi() / (2.0 * thetabins);
  }
  double Qtheta[thetabins];
  TComplex genfunS[rbins][thetabins]; // sum
  TComplex genfunP[rbins][thetabins]; // product
  TComplex genfunPr0[thetabins]; // product
  TComplex cdGr0[thetabins];
  TComplex cExpo;
  double Q2x, Q2y;
  int mult;
  
  // Differential LYZ
  TComplex cDenominator, cTemporary, cRatio, cDtheta;
  TComplex cExponent[thetabins];

  TFile *d_outfile = new TFile(outputFileName.Data(), "recreate");

  TH1I *hMult = new TH1I("hMult", "Multiplicity distr;M;dN/dM", max_nh, 0, max_nh);
  TH2F *hBimpvsMult = new TH2F("hBimpvsMult", "Impact parameter vs multiplicity;N_{ch};b (fm)", max_nh, 0, max_nh, 200, 0., 20.);
  TH1F *hBimp = new TH1F("hBimp", "Impact parameter;b (fm);dN/db", 200, 0., 20.);
  TH1F *hPt = new TH1F("hPt", "Pt-distr;p_{T} (GeV/c); dN/dP_{T}", 500, 0., 6.);
  TH1F *hRP = new TH1F("hRP", "Event Plane; #phi-#Psi_{RP}; dN/d#Psi_{RP}", 300, 0., 7.);
  TH1F *hPhi = new TH1F("hPhi", "Particle azimuthal angle distr with respect to RP; #phi-#Psi_{RP}; dN/d(#phi-#Psi_{RP})", 300, 0., 7.);
  TH1F *hPhil = new TH1F("hPhil", "Azimuthal angle distr in laboratory coordinate system; #phi; dN/d#phi", 300, 0., 7.);
  TH1F *hEta = new TH1F("hEta", "Pseudorapidity distr; #eta; dN/d#eta", 300, -2.2, 2.2);

  TProfile *hv2MC = new TProfile("hv2MC", "MC flow", ncent, &bin_cent[0]);
  TProfile *hv2EP = new TProfile("hv2EP", "Ref. v_{2}{EP}", ncent, &bin_cent[0]);
  TProfile *HRes = new TProfile("HRes", "EP resolution", ncent, &bin_cent[0]);

  double res2[ncent];
  TFile *fiHist = NULL;
  if (!bFirstRun){
    if (!inputFileHist) cerr << "inputFileHist=NULL!!" << endl;
    fiHist = new TFile(inputFileHist.Data(),"read");
    HRes =  (TProfile*) fiHist->Get("HRes");
    for (int ic = 0; ic < ncent; ic++){
      res2[ic] = TMath::Sqrt(HRes->GetBinContent(ic+1));
    }
  }
  
  TProfile *hv2MCpt[ncent];
  TProfile *hv2EPpt[ncent];

  for (int icent = 0; icent < ncent; icent++)
  { // loop over centrality classes
    hv2EPpt[icent] = new TProfile(Form("hv2EPpt_%i", icent), "", npt, &bin_pT[0]);
    hv2MCpt[icent] = new TProfile(Form("hv2MCpt_%i", icent), "", npt, &bin_pT[0]);
  } // end of loop over centrality classes

  TProfile *prReGthetaSum[ncent][thetabins];
  TProfile *prImGthetaSum[ncent][thetabins];
  TH1F *hGthetaSum[ncent][thetabins];
  double r02[ncent][thetabins] = {{0.}};
  if (!bFirstRun){
    if (!inputFileHist) cerr << "inputFileHist=NULL!!" << endl;
    fiHist = new TFile(inputFileHist.Data(),"read");
    for (int i = 0; i < ncent; ++i)
    {
      for (int j = 0; j < thetabins; ++j)
      {
        prReGthetaSum[i][j] = (TProfile*) fiHist->Get(Form("prReGthetaSum_mult%d_theta%d", i, j));
        prImGthetaSum[i][j] = (TProfile*) fiHist->Get(Form("prImGthetaSum_mult%d_theta%d", i, j));
      }
    }

    
    for (int ic = 0; ic < ncent; ic++)
    {
      cout <<"{";
      for (int it = 0; it < thetabins; it++)
      {
        TH1F *hGtheta = FillHistGtheta(prReGthetaSum[ic][it], prImGthetaSum[ic][it]);
        double r0theta = GetR0(hGtheta);
        r02[ic][it] = r0theta;
        cout << r0theta << ", ";
      }
      cout << "}," << endl;
    }
  }

  TProfile *prReGthetaProduct[ncent][thetabins];
  TProfile *prImGthetaProduct[ncent][thetabins];
  TH1F *hGthetaProduct[ncent][thetabins];
  double r02Pro[ncent][thetabins];
  if (!bFirstRun && bUseProduct){
    if (!inputFileHist) cerr << "inputFileHist=NULL!!" << endl;
    fiHist = new TFile(inputFileHist.Data(),"read");
    for (int i = 0; i < ncent; ++i)
    {
      for (int j = 0; j < thetabins; ++j)
      {
        prReGthetaProduct[i][j] = (TProfile*) fiHist->Get(Form("prReGthetaProduct_mult%d_theta%d", i, j));
        prImGthetaProduct[i][j] = (TProfile*) fiHist->Get(Form("prImGthetaProduct_mult%d_theta%d", i, j));
      }
    }

    cout << "r02Pro" << endl;
    for (int ic = 0; ic < ncent; ic++)
    {
      cout <<"{";
      for (int it = 0; it < thetabins; it++)
      {
        TH1F *hGtheta = FillHistGtheta(prReGthetaProduct[ic][it], prImGthetaProduct[ic][it]);
        double r0theta = GetR0(hGtheta);
        r02Pro[ic][it] = r0theta;
        cout << r0theta << ", ";
      }
      cout << "}," << endl;
    }
  }
  TProfile *prRefMult = new TProfile("prRefMult","",ncent, &bin_cent[0]);
  TProfile *prQ2x = new TProfile("prQ2x","",ncent, &bin_cent[0]);
  TProfile *prQ2y = new TProfile("prQ2y","",ncent, &bin_cent[0]);
  TProfile *prQ2ModSq = new TProfile("prQ2ModSq","",ncent, &bin_cent[0]);

  // Differential flow
  TProfile *prReDenom[thetabins];
  TProfile *prImDenom[thetabins];
  TProfile *prReNumer[thetabins][ncent];
  TProfile *prImNumer[thetabins][ncent];

  TProfile *prReDenomPro[thetabins];
  TProfile *prImDenomPro[thetabins];
  TProfile *prReNumerPro[thetabins][ncent];
  TProfile *prImNumerPro[thetabins][ncent];

  TProfile *prMultPOI[ncent];
  if (bFirstRun){
  for (int i = 0; i < ncent; ++i)
  {
    for (int j = 0; j < thetabins; ++j)
    {
      prReGthetaSum[i][j] = new TProfile(Form("prReGthetaSum_mult%d_theta%d", i, j), "", rbins, rMinSum, rMaxSum);
      prImGthetaSum[i][j] = new TProfile(Form("prImGthetaSum_mult%d_theta%d", i, j), "", rbins, rMinSum, rMaxSum);
      hGthetaSum[i][j] = new TH1F(Form("hGthetaSum_mult%d_theta%d", i, j), "", rbins, rMinSum, rMaxSum);
      if (bUseProduct)
      {
        prReGthetaProduct[i][j] = new TProfile(Form("prReGthetaProduct_mult%d_theta%d", i, j), "", rbins, rMin, rMax);
        prImGthetaProduct[i][j] = new TProfile(Form("prImGthetaProduct_mult%d_theta%d", i, j), "", rbins, rMin, rMax);
        hGthetaProduct[i][j] = new TH1F(Form("hGthetaProduct_mult%d_theta%d", i, j), "", rbins, rMin, rMax);
      }
    }
  }}

  else{
  for (int i = 0; i < thetabins; i++)
  {
    prReDenom[i] = new TProfile(Form("prReDenom_theta%i",i),"", ncent, &bin_cent[0]);
    prImDenom[i] = new TProfile(Form("prImDenom_theta%i",i),"", ncent, &bin_cent[0]);

    for (int j = 0; j < ncent; j++)
    {
      prReNumer[i][j] = new TProfile(Form("prReNumer_theta%i_cent%i", i, j),"", npt, &bin_pT[0]);
      prImNumer[i][j] = new TProfile(Form("prImNumer_theta%i_cent%i", i, j),"", npt, &bin_pT[0]);
    }
    if (bUseProduct){
    prReDenomPro[i] = new TProfile(Form("prReDenomPro_theta%i",i),"", ncent, &bin_cent[0]);
    prImDenomPro[i] = new TProfile(Form("prImDenomPro_theta%i",i),"", ncent, &bin_cent[0]);

    for (int j = 0; j < ncent; j++)
    {
      prReNumerPro[i][j] = new TProfile(Form("prReNumerPro_theta%i_cent%i", i, j),"", npt, &bin_pT[0]);
      prImNumerPro[i][j] = new TProfile(Form("prImNumerPro_theta%i_cent%i", i, j),"", npt, &bin_pT[0]);
    }
    }

  }

  for (int ic = 0; ic < ncent; ic++)
  {
    prMultPOI[ic] = new TProfile(Form("prMultPOI_cent%i",ic),"", npt, 0, npt);
  }}
  double r[rbins], rSum[rbins];
  if (bFirstRun){
  for (int rbin = 0; rbin < rbins; ++rbin)
  {
    if (bUseProduct) {
      r[rbin] = (double) hGthetaProduct[0][0]->GetBinCenter(rbin+1);
      rSum[rbin] = (double) hGthetaSum[0][0]->GetBinCenter(rbin+1);
    }
    else{rSum[rbin] = (double) hGthetaSum[0][0]->GetBinCenter(rbin+1);}
  }}

  // Lee Yang Zero RP
  TFile *fiLYZEP;
  TProfile *prReDtheta[thetabins];
  TProfile *prImDtheta[thetabins];
  if (!bFirstRun){
    if (!bTemporaryFlagForLYZEP){
      for (int i = 0; i < thetabins; i++)
      {
        prReDtheta[i] = new TProfile(Form("prReDtheta_theta%i",i),"", ncent, &bin_cent[0]);
        prImDtheta[i] = new TProfile(Form("prImDtheta_theta%i",i),"", ncent, &bin_cent[0]);
      }
    }
    else
    {
      if (inputFileHist2 == "") { cout << "inputFileHist2==NULL" << endl; return; }
      fiLYZEP = new TFile(inputFileHist2.Data(),"read");
      // fiLYZEP = new TFile("test.root","read");
      for (int i = 0; i < thetabins; i++)
      {
        prReDtheta[i] = (TProfile*)fiLYZEP->Get(Form("prReDtheta_theta%i",i));
        prImDtheta[i] = (TProfile*)fiLYZEP->Get(Form("prImDtheta_theta%i",i));
      }
    }
  }
  TProfile *hv2LYZEP = new TProfile("hv2LYZEP", "Ref. v_{2}{LYZ, EP}", ncent, &bin_cent[0]);
  TProfile *hv2LYZEPpt[ncent];
  for (int icent = 0; icent < ncent; icent++)
  { // loop over centrality classes
    hv2LYZEPpt[icent] = new TProfile(Form("hv2LYZEPpt_%i", icent), "", npt, &bin_pT[0]);
  } // end of loop over centrality classes
  // QC

  // TProfile for reference flow (RF)
  TProfile *hv22[ncent];  // profile <<2>> from 2nd Q-Cumulants
  TProfile *hv24[ncent];  // profile <<4>> from 4th Q-Cumulants
  // TProfile for differential flow (DF)
  TProfile *hv22pt[ncent][npt];  // profile <<2'>> from 2nd Q-Cumulants
  TProfile *hv24pt[ncent][npt];  // profile <<4'>> from 4th Q-Cumulants

  // TProfile for covariance calculation according to (C.12), Appendix C
  // in Bilandzic, A. (2012). Anisotropic flow measurements in ALICE at the large hadron collider.
  TProfile *hcov24[ncent];                // <2>*<4>
  TProfile *hcov22prime[ncent][npt];      // <2>*<2'>
  TProfile *hcov24prime[ncent][npt];      // <2>*<4'>
  TProfile *hcov42prime[ncent][npt];      // <2>*<4'>
  TProfile *hcov44prime[ncent][npt];      // <4>*<4'>
  TProfile *hcov2prime4prime[ncent][npt]; // <2'>*<4'>
  for (int icent = 0; icent < ncent; icent++)
  { // loop over centrality classes
    hv22[icent] = new TProfile(Form("hv22_%i", icent), "", 1, 0., 1.);
    hv24[icent] = new TProfile(Form("hv24_%i", icent), "", 1, 0., 1.);
    hcov24[icent] = new TProfile(Form("hcov24_%i", icent), "", 1, 0., 1.);
    for (int kpt = 0; kpt < npt; kpt++)
    { // loop over pt bin
      hv22pt[icent][kpt] = new TProfile(Form("hv22pt_%i_%i", icent, kpt), "", 1, 0., 1.);
      hv24pt[icent][kpt] = new TProfile(Form("hv24pt_%i_%i", icent, kpt), "", 1, 0., 1.);
      hcov22prime[icent][kpt] = new TProfile(Form("hcov22prime_%i_%i", icent, kpt), "", 1, 0., 1.);
      hcov24prime[icent][kpt] = new TProfile(Form("hcov24prime_%i_%i", icent, kpt), "", 1, 0., 1.);
      hcov42prime[icent][kpt] = new TProfile(Form("hcov42prime_%i_%i", icent, kpt), "", 1, 0., 1.);
      hcov44prime[icent][kpt] = new TProfile(Form("hcov44prime_%i_%i", icent, kpt), "", 1, 0., 1.);
      hcov2prime4prime[icent][kpt] = new TProfile(Form("hcov2prime4prime_%i_%i", icent, kpt), "", 1, 0., 1.);
    } // end of loop over pt bin
  } // end of loop over centrality classes
  // cout << "Here?!" << endl;  
  // cout << "Histograms have been initialized" << endl;

  // Declaration of leaf types
  Int_t nh;
  Float_t b;
  Float_t rp;
  Float_t phi0[max_nh]; //[nh]
  Bool_t bFlow[max_nh]; //[nh]
  Float_t eta[max_nh];  //[nh]
  Float_t pt[max_nh];   //[nh]

  // List of branches
  TBranch *b_nh;    //!
  TBranch *b_b;     //!
  TBranch *b_rp;    //!
  TBranch *b_phi0;  //!
  TBranch *b_bFlow; //!
  TBranch *b_eta;   //!
  TBranch *b_pt;    //!

  TChain *fChain = new TChain("tree");
  if (inputFileName.Contains(".root"))
  {fChain->Add(inputFileName.Data());}
  else
  {
    std::ifstream file(inputFileName.Data());
    std::string line;
    while (std::getline(file, line))
    {
      fChain->Add(line.c_str());
    }
  }
  
  if (!fChain)
    return;
  fChain->SetBranchAddress("nh", &nh, &b_nh);
  fChain->SetBranchAddress("b", &b, &b_b);
  fChain->SetBranchAddress("rp", &rp, &b_rp);
  fChain->SetBranchAddress("phi0", phi0, &b_phi0);
  fChain->SetBranchAddress("bFlow", bFlow, &b_bFlow);
  fChain->SetBranchAddress("eta", eta, &b_eta);
  fChain->SetBranchAddress("pt", pt, &b_pt);

  if (fChain == 0)
    return;

  Long64_t nentries = fChain->GetEntries();
  cout << "Calculating flow..." << endl;
  for (Long64_t jentry = 0; jentry < nentries; jentry++)
  {
    if (jentry % 10000 == 0)
      cout << "[" << jentry << "/" << nentries << "]" << endl;
    // if (jentry == 100000) break;  
    fChain->GetEntry(jentry);

    double dCent = CentB(b);
    if (dCent < 0)
      continue;
    int icent = GetCentBin(dCent);
    mult = 0;
    hMult->Fill(nh);
    hRP->Fill(rp);
    hBimp->Fill(b);
    hBimpvsMult->Fill(nh, b);
    double sumQxy[neta][2] = {{0}}; // [eta-,eta+][x,y]
    double multQv[neta] = {0};      // [eta+,eta-]

    for (int i = 0; i < thetabins; ++i)
    {
      Qtheta[i] = 0.;
      cExponent[i] = TComplex(0.0,0.0);
    }
    Q2x = 0.;
    Q2y = 0.;
    if (bFirstRun)
    {
      for (int i = 0; i < rbins; ++i)
      {
        for (int j = 0; j < thetabins; ++j)
        {
          genfunS[i][j] = TComplex(0.0, 0.0); // initialize to 0, calculate directly
          if (bUseProduct) genfunP[i][j] = TComplex::One(); // initialize to 1, calcualte via product
        }
      }
    }
    else{
      
      for (int it = 0; it < thetabins; it++){
        genfunPr0[it] = TComplex::One();
        cdGr0[it] = TComplex(0.0,0.0);
      }
      for (int ipt = 0; ipt < npt; ipt++) multPOI[ipt] = 0.;
    } 

    // Q-vector of RFP
    Double_t Qx2 = 0., Qy2 = 0., Qx4 = 0., Qy4 = 0.;
    TComplex Q2 = 0., Q4 = 0.;
    // p-vector of POI
    Double_t px2[npt] = {0.}, py2[npt] = {0.};
    TComplex p2[npt] = {0.}, q2[npt] = {0.}, q4[npt] = {0.};
    // q-vector of particles marked as POI and RFP, which is used for
    // autocorrelation substraction
    Double_t qx2[npt] = {0.}, qy2[npt] = {0.}, qx4[npt] = {0.}, qy4[npt] = {0.};
    // Total number of RFP in given event
    Double_t M = 0.;
    // numbers of POI (mp) and particles marked both POI and RFP (mq)
    Double_t mq[npt] = {0.}, mp[npt] = {0.};
    // average reduced single-event 2- and 4-particle correlations : <2'> & <4'>
    Double_t redCor22[npt] = {0.}, redCor24[npt] = {0.};
    // event weights for correlation calculation
    Double_t w2 = 0., w4 = 0.;
    // event weights for reduced correlation calculation
    Double_t wred2[npt] = {0.}, wred4[npt] = {0.};
    // Average single-event 2- and 4- particle correlations : <2> & <4>
    Double_t cor22 = 0., cor24 = 0.;

    double dWR = 0., dPsiR = 0.;

    for (int i = 0; i < nh; i++)
    { // track loop
      double pT = pt[i];
      if (pT < minpt || pT > maxpt || eta[i] > eta_cut)
        continue;
      hPt->Fill(pT);
      double phi = phi0[i];
      if (phi < 0)
        phi += 2. * TMath::Pi();
      hPhi->Fill(phi-rp);
      hPhil->Fill(phi);
      hEta->Fill(eta[i]);

      Q2x += TMath::Cos(2.0 * phi);
      Q2y += TMath::Sin(2.0 * phi);
        Int_t ipt = -1;
        for (int j = 0; j < npt; j++) if (pT >= bin_pT[j] && pT < bin_pT[j + 1]) ipt = j;
      if (!bFirstRun)
      {

        multPOI[ipt]++;
      }
      Double_t v2 = TMath::Cos(2 * (phi - rp));

      Double_t cos4phi = TMath::Cos(4.*phi);
      Double_t sin4phi = TMath::Sin(4.*phi);
      Double_t cos2phi = TMath::Cos(2.*phi);
      Double_t sin2phi = TMath::Sin(2.*phi);

      Qx2 += cos2phi;
      Qy2 += sin2phi;
      Qx4 += cos4phi;
      Qy4 += sin4phi; 
      M++;

      px2[ipt] += cos2phi;
      py2[ipt] += sin2phi;
      mp[ipt]++;

      qx2[ipt] += cos2phi;
      qy2[ipt] += sin2phi;
      qx4[ipt] += cos4phi;
      qy4[ipt] += sin4phi;
      mq[ipt]++;
      
      if (bFlow[i])
      {
        hv2MC->Fill(dCent, v2);        // calculate reference v2 from MC toy
        hv2MCpt[icent]->Fill(pT, v2); // Calculate differential v2 from MC toy
      }
      // Sub eta event method
      int fEta = -1;
      if (eta[i] < -eta_gap && eta[i] > -eta_cut)
        fEta = 0;
      if (eta[i] > eta_gap && eta[i] < eta_cut)
        fEta = 1;

      if (fEta > -1)
      {
        sumQxy[fEta][0] += pT * TMath::Cos(2.0 * phi);
        sumQxy[fEta][1] += pT * TMath::Sin(2.0 * phi);
        multQv[fEta]++;
      } // end of eta selection
      if (bUseProduct){
        
        if (bFirstRun){
          for (int thetabin = 0; thetabin < thetabins; ++thetabin)
          {
            double dCosTerm = TMath::Cos(2. * (phi - theta[thetabin]));
            for (int rbin = 0; rbin < rbins; ++rbin)
            {
              genfunP[rbin][thetabin] *= TComplex(1.0, r[rbin] * dCosTerm);
              if (genfunP[rbin][thetabin].Rho2() > 100.) break;
            }
          }
        }
        else{
          for (int thetabin = 0; thetabin < thetabins; ++thetabin)
          {
            double dCosTerm = TMath::Cos(2. * (phi - theta[thetabin]));
            genfunPr0[thetabin] *= TComplex(1.0, r02Pro[icent][thetabin] * dCosTerm);
            TComplex cCosTermComplex(1., r02Pro[icent][thetabin] * dCosTerm);
            cdGr0[thetabin] += (dCosTerm/cCosTermComplex); 
          }
        }
      }
      mult++;
    } // end of track loop

    if (M >= 4.){
      Q2 = TComplex(Qx2, Qy2);
      w2 = M * (M - 1.);                 // w(<2>)
      cor22 = CalCor22(Q2, M, w2);       // <2>
      hv22[icent]->Fill(0.5, cor22, w2); // <<2>>
      Q4 = TComplex(Qx4, Qy4);
      w4 = M * (M - 1.) * (M - 2.) * (M - 3.); // w(<4>)
      cor24 = CalCor24(Q2, Q4, M, w4);      // <4>
      hv24[icent]->Fill(0.5, cor24, w4);    // <<4>>
      hcov24[icent]->Fill(0.5, cor22 * cor24, w2 * w4); // <2>*<4>
      if (w2 == 0 || w4 == 0) cerr << "WARNING: w2,4=0" << endl;
      for (int ipt = 0; ipt < npt; ipt++){
        if (mp[ipt] == 0) continue;
        p2[ipt] = TComplex(px2[ipt], py2[ipt]);
        q2[ipt] = TComplex(qx2[ipt], qy2[ipt]);
        wred2[ipt] = mp[ipt] * M - mq[ipt];                                        // w(<2'>)
        if (wred2[ipt]==0) continue;
        redCor22[ipt] = CalRedCor22(Q2, p2[ipt], M, mp[ipt], mq[ipt], wred2[ipt]); // <2'>
        hv22pt[icent][ipt]->Fill(0.5, redCor22[ipt], wred2[ipt]);                  // <<2'>>

        hcov22prime[icent][ipt]->Fill(0.5, cor22 * redCor22[ipt], w2 * wred2[ipt]); // <2>*<2'>

        q4[ipt] = TComplex(qx4[ipt], qy4[ipt]);
        wred4[ipt] = (mp[ipt] * M - 3. * mq[ipt]) * (M - 1.) * (M - 2.);                                 // w(<4'>)
        if (wred4[ipt]==0) continue;
        redCor24[ipt] = CalRedCor24(Q2, Q4, p2[ipt], q2[ipt], q4[ipt], M, mp[ipt], mq[ipt], wred4[ipt]); // <4'>
        hv24pt[icent][ipt]->Fill(0.5, redCor24[ipt], wred4[ipt]);                                        // <<4'>>

        // TProfile for covariance calculation in statistic error
        hcov24prime[icent][ipt]->Fill(0.5, cor22 * redCor24[ipt], w2 * wred4[ipt]);
        hcov42prime[icent][ipt]->Fill(0.5, cor24 * redCor22[ipt], w4 * wred2[ipt]);
        hcov44prime[icent][ipt]->Fill(0.5, cor24 * redCor24[ipt], w4 * wred4[ipt]);
        hcov2prime4prime[icent][ipt]->Fill(0.5, redCor22[ipt] * redCor24[ipt], wred2[ipt] * wred4[ipt]);
      }
    }

    if (mult != 0) 
    {
      hMult->Fill(mult);
      if (bFirstRun) {prRefMult->Fill(dCent, mult);}
      else{
        for (int ipt = 0; ipt < npt; ipt++) 
        {prMultPOI[icent]->Fill(ipt+0.5,multPOI[ipt]);}
      }

      // double Q2xMean = Q2x / mult;
      // double Q2yMean = Q2y / mult;
      for (int thetabin = 0; thetabin < thetabins; ++thetabin)
      {
        // Qtheta[thetabin] = Q2xMean * TMath::Cos(2.0 * theta[thetabin]) + Q2yMean * TMath::Sin(2.0 * theta[thetabin]);
        Qtheta[thetabin] = Q2x * TMath::Cos(2.0 * theta[thetabin]) + Q2y * TMath::Sin(2.0 * theta[thetabin]);
      }

      if (bFirstRun)
      {
        prQ2x->Fill(dCent, Q2x);
        prQ2y->Fill(dCent, Q2y);
        prQ2ModSq->Fill(dCent, Q2x*Q2x+Q2y*Q2y);

        for (int rbin = 0; rbin < rbins; rbin++)
        {
          for (int thetabin = 0; thetabin < thetabins; thetabin++)
          {
            cExpo = TComplex(0., rSum[rbin] * Qtheta[thetabin]);
            genfunS[rbin][thetabin] = TComplex::Exp(cExpo); // generating function from Q-vectors
            // prReGthetaSum[icent][thetabin]->Fill(rSum[rbin], genfunS[rbin][thetabin].Re());
            // prImGthetaSum[icent][thetabin]->Fill(rSum[rbin], genfunS[rbin][thetabin].Im());
            prReGthetaSum[icent][thetabin]->Fill(rSum[rbin], genfunS[rbin][thetabin].Re(), mult);
            prImGthetaSum[icent][thetabin]->Fill(rSum[rbin], genfunS[rbin][thetabin].Im(), mult);
            if (bUseProduct)
            {
              prReGthetaProduct[icent][thetabin]->Fill(r[rbin], genfunP[rbin][thetabin].Re());
              prImGthetaProduct[icent][thetabin]->Fill(r[rbin], genfunP[rbin][thetabin].Im());
              // prReGthetaProduct[icent][thetabin]->Fill(r[rbin], genfunP[rbin][thetabin].Re(), mult);
              // prImGthetaProduct[icent][thetabin]->Fill(r[rbin], genfunP[rbin][thetabin].Im(), mult);              
            }
          }
        }


      }
      else
      {
        // cout << "Here?!" << endl;
        double dWRcos2Psi = 0., dWRsin2Psi = 0.;
        // Differential LYZ
        for (int thetabin = 0; thetabin < thetabins; thetabin++)
        {
          cExponent[thetabin] = TComplex(0., r02[icent][thetabin] * Qtheta[thetabin]);
          cDenominator = Qtheta[thetabin]*(TComplex::Exp(cExponent[thetabin])); // BP eq 12
          prReDenom[thetabin]->Fill(dCent, cDenominator.Re());
          prImDenom[thetabin]->Fill(dCent, cDenominator.Im());
          if (!bTemporaryFlagForLYZEP){
          cTemporary = r02[icent][thetabin]*Qtheta[thetabin]*(TComplex::Exp(cExponent[thetabin]));
          prReDtheta[thetabin]->Fill(dCent, cTemporary.Re());
          prImDtheta[thetabin]->Fill(dCent, cTemporary.Im());
          }
          else{
            cDtheta = TComplex(prReDtheta[thetabin]->GetBinContent(icent+1)/rootJ0, prImDtheta[thetabin]->GetBinContent(icent+1)/rootJ0);
            if (cDtheta.Rho()!=0){ cRatio = TComplex::Exp(cExponent[thetabin]) / cDtheta;}
            else{cRatio(0.,0.);}
            dWRcos2Psi += cRatio.Re()*TMath::Cos(2.*theta[thetabin]);
            dWRsin2Psi += cRatio.Re()*TMath::Sin(2.*theta[thetabin]);  
          }
        }
        if (bTemporaryFlagForLYZEP){
        dWRcos2Psi /= thetabins;
        dWRsin2Psi /= thetabins;
        dWR = TMath::Sqrt(dWRcos2Psi*dWRcos2Psi + dWRsin2Psi*dWRsin2Psi);
        
        // calculate dPsiR
        dPsiR = 0.5*TMath::ATan2(dWRsin2Psi,dWRcos2Psi);   // takes care of the signs correctly!
        if (dPsiR < 0.) { dPsiR += TMath::Pi(); }          // to shift distribution from (-pi/2 to pi/2) to (0 to pi)
        }
        if (bUseProduct)
        {
          for (int thetabin = 0; thetabin < thetabins; thetabin++)
          {
            prReDenomPro[thetabin]->Fill(dCent, (genfunPr0[thetabin] * cdGr0[thetabin]).Re());
            prImDenomPro[thetabin]->Fill(dCent, (genfunPr0[thetabin] * cdGr0[thetabin]).Im());
          }
        }
      }
    } // end of if (mult!=0)

    // Eta sub-event method
    double fEP[2] = {0.}; // [eta-,eta+]
    for (int ieta = 0; ieta < neta; ieta++)
    {
      if (multQv[ieta] > 5)
      { // multiplicity > 5
        fEP[ieta] = TMath::ATan2(sumQxy[ieta][1], sumQxy[ieta][0]) / 2.0;
        fEP[ieta] = TMath::ATan2(sin(2.0 * fEP[ieta]), cos(2.0 * fEP[ieta])); // what for?
        fEP[ieta] /= 2.0;
      }
      else
      {
        fEP[ieta] = -9999.;
      }
    }
    if (fEP[0] != -9999. && fEP[1] != -9999.)
    {
      HRes->Fill(dCent, TMath::Cos(2.0 * (fEP[0] - fEP[1])));
    }
    // Estimate the event plane resolution of 2nd harmonic by the correlation between the azimuthal
    // angles of two subset groups of tracks, called sub-events \eta- and \eta+

    if (!bFirstRun){
    for (int itrk = 0; itrk < nh; itrk++)
    { //track loop
      double pT = pt[itrk];
      if (pT < minpt || pT > maxpt || eta[itrk] > eta_cut)
        continue;
      double phi = phi0[itrk];
  
      if (fEP[0] != -9999. && fEP[1] != -9999.)
      {
        float v2 = -999.0;
        if (eta[itrk] > 0)
        { // eta+
          v2 = TMath::Cos(2.0 * (phi - fEP[0])) / res2[icent];
        }
        if (eta[itrk] < 0)
        { // eta-
          v2 = TMath::Cos(2.0 * (phi - fEP[1])) / res2[icent];
        }
        // if(fabs(eta[itrk])<1.0){ // eliminate spectators
        if (v2 != -999.0)
        {
          hv2EPpt[icent]->Fill(pT, v2);
          hv2EP->Fill(dCent, v2);
        }
      }
    
      for (int thetabin = 0; thetabin < thetabins; ++thetabin)
      {
        double dCosTerm = TMath::Cos(2.0 * (phi - theta[thetabin]));
        TComplex cNumeratorPOI = dCosTerm*(TComplex::Exp(cExponent[thetabin]));    
        prReNumer[thetabin][icent]->Fill(pT, cNumeratorPOI.Re());
        prImNumer[thetabin][icent]->Fill(pT, cNumeratorPOI.Im());
        if (bUseProduct){
          TComplex cCosTermComplex(1., r02Pro[icent][thetabin] * dCosTerm);
          TComplex cNumeratorPOIPro = genfunPr0[thetabin] * dCosTerm / cCosTermComplex;   
          prReNumerPro[thetabin][icent]->Fill(pT, cNumeratorPOIPro.Re());
          prImNumerPro[thetabin][icent]->Fill(pT, cNumeratorPOIPro.Im());          
        }
      }

      if (bTemporaryFlagForLYZEP){
        double v2LYZEP = dWR * TMath::Cos(2*(phi-dPsiR));
          hv2LYZEPpt[icent]->Fill(pT, v2LYZEP);
          hv2LYZEP->Fill(dCent, v2LYZEP);
      }
    } // end of the track loop
    }
  }   // end of event loop

  // cout << "double v2MC[9] = {";
  // for (int ic = 0 ; ic < ncent-1; ic++)
  // {
  //   cout << hv2MC->GetBinContent(ic+1) << ", ";
  // }
  // cout << hv2MC->GetBinContent(ncent) << "};" << endl;

  // cout << "double v2eMC[9] = {";
  // for (int ic = 0 ; ic < ncent-1; ic++)
  // {
  //   cout << hv2MC->GetBinError(ic+1) << ", ";
  // }
  // cout << hv2MC->GetBinError(ncent) << "};" << endl;

  // //============================================================================================================
  // cout << inputFileName.Data() << " file processed" << endl;
  // cout << "Resolution:" << endl;
  // GetRes(HRes);
  // GetMultMean(prRefMult);
  // double dChi2[ncent];
  // float v2int[ncent]={0.}, v2e[ncent]={0.};
  // double dVtheta[ncent][thetabins] = {0.};
  // cout << "const double r02[ncent][thetabins] = {";
  // for (int ic = 0; ic < ncent; ic++)
  // {
  //   float refmult = prRefMult->GetBinContent(ic+1);
  //   int thetacount = 0;
  //   cout <<"{";
  //   for (int it = 0; it < thetabins; it++)
  //   {
  //     TH1F *hGtheta = NULL;
  //     if (bUseProduct) {hGtheta = FillHistGtheta(prReGthetaProduct[ic][it], prImGthetaProduct[ic][it]);}
  //     else {hGtheta = FillHistGtheta(prReGthetaSum[ic][it], prImGthetaSum[ic][it]);}
  //     float r0theta = GetR0(hGtheta);
  //     // if (ic == 3 && it == 0) cout << "r0theta = " << r0theta << endl;
  //     // cout << "cent:" << ic <<", theta =" << it << ", r0theta = " << r0theta << endl;
  //     // if (it == 0) cout << rootJ0 <<"/"<< r0theta <<"/"<< refmult << " ";
  //     cout << r0theta << ", ";
  //     if (r0theta!=0) 
  //     {
  //       v2int[ic] += rootJ0 / r0theta;
  //       dVtheta[ic][it] = rootJ0 / r0theta;
  //       thetacount++;
  //     }
  //     // if (ic == 2) cout << rootJ0 / r0theta / refmult<< ", ";
  //   }
  //   cout << "}," << endl;
  //   if (thetacount!=0) v2int[ic] /= (float)thetacount*refmult; // refmult
  //   else {v2int[ic]=0.;}
    


  d_outfile->cd();
  for (int icent = 0; icent < ncent; icent++)
  { // loop over centrality classes
    hv22[icent]->Write();
    hv24[icent]->Write();
    hcov24[icent] ->Write();
    for (int kpt = 0; kpt < npt; kpt++)
    { // loop over pt bin
      hv22pt[icent][kpt] ->Write();
      hv24pt[icent][kpt] ->Write();
      hcov22prime[icent][kpt]->Write();
      hcov24prime[icent][kpt]->Write();
      hcov42prime[icent][kpt]->Write();
      hcov44prime[icent][kpt]->Write();
      hcov2prime4prime[icent][kpt]->Write();
    } // end of loop over pt bin
  } // end of loop over centrality classes

  if (bFirstRun)
  {
    hMult->Write();
    hBimp->Write();
    hPt->Write();
    hPhi->Write();
    hEta->Write();
    hv2MC->Write();
    HRes->Write();
    prRefMult->Write();
    prQ2x->Write();
    prQ2y->Write();
    prQ2ModSq->Write();
    for (int i = 0; i < ncent; ++i)
    {
      for (int j = 0; j < thetabins; ++j)
      {
        prReGthetaSum[i][j]->Write();
        prImGthetaSum[i][j]->Write();
        if (bUseProduct)
        {
          prReGthetaProduct[i][j]->Write();
          prImGthetaProduct[i][j]->Write();
        }
      }
    }
  }
  else
  {
    hv2EP->Write();
    for (int j = 0; j < thetabins; ++j)
    {
      prReDenom[j]->Write();
      prImDenom[j]->Write();
      for (int i = 0; i < ncent; i++)
      {
        prReNumer[j][i]->Write();
        prImNumer[j][i]->Write();
      }
      if (bUseProduct){
        prReDenomPro[j]->Write();
        prImDenomPro[j]->Write();
        for (int i = 0; i < ncent; i++)
        {
          prReNumerPro[j][i]->Write();
          prImNumerPro[j][i]->Write();
        }
      }
    }
    for (int ic = 0; ic < ncent; ic++)
    {
      prMultPOI[ic]->Write();
      hv2EPpt[ic]->Write();
      hv2MCpt[ic]->Write();
    }
    for (int it = 0; it < thetabins; it++)
    {
      prReDtheta[it]->Write();
      prImDtheta[it]->Write();
    }
    if (bTemporaryFlagForLYZEP){
    hv2LYZEP->Write();
    for (int ic = 0; ic < ncent; ic++)
    {
      hv2LYZEPpt[ic]->Write();
    }
    }
  }
  d_outfile->Close();
  cout << "Histfile has been written" << endl;
}

int main(int argc, char **argv)
{
  TString iFileName, oFileName, inputFileNameFromFirstRun = "", inputFileNameFromSecondRun = "";

  if (argc < 5)
  {
    std::cerr << "./FlowQCumulant -i INPUT -o OUTPUT [Second Run: -inHist FirstRun.root] [Third Run: -inHist2 SecondRun.root]" << std::endl;
    return 1;
  }
  for (Int_t i = 1; i < argc; i++)
  {
    if (std::string(argv[i]) != "-i" &&
        std::string(argv[i]) != "-o" &&
        std::string(argv[i]) != "-inHist" &&
        std::string(argv[i]) != "-inHist2")
    {
      std::cerr << "\n[ERROR]: Unknown parameter " << i << ": " << argv[i] << std::endl;
      return 2;
    }
    else
    {
      if (std::string(argv[i]) == "-i" && i != argc - 1)
      {
        iFileName = argv[++i];
        continue;
      }
      if (std::string(argv[i]) == "-i" && i == argc - 1)
      {
        std::cerr << "\n[ERROR]: Input file name was not specified " << std::endl;
        return 3;
      }
      if (std::string(argv[i]) == "-o" && i != argc - 1)
      {
        oFileName = argv[++i];
        continue;
      }
      if (std::string(argv[i]) == "-o" && i == argc - 1)
      {
        std::cerr << "\n[ERROR]: Output file name was not specified " << std::endl;
        return 4;
      }
      if (std::string(argv[i]) == "-inHist" && i != argc - 1)
      {
        inputFileNameFromFirstRun = argv[++i];
        continue;
      }
      if (std::string(argv[i]) == "-inHist" && i == argc - 1)
      {
        std::cerr << "\n[ERROR]: Input file name with histograms from 1-st run was not specified " << std::endl;
        return 5;
      }
      if (std::string(argv[i]) == "-inHist2" && i != argc - 1)
      {
        inputFileNameFromSecondRun = argv[++i];
        continue;
      }
      if (std::string(argv[i]) == "-inHist2" && i == argc - 1)
      {
        std::cerr << "\n[ERROR]: Input file name with histograms from 2-st run was not specified " << std::endl;
        return 1;
      }

    }
  }
  if (argc == 5) ToyModelTreeReader(iFileName, oFileName);
  else if (argc == 7) ToyModelTreeReader(iFileName, oFileName, inputFileNameFromFirstRun, 0);
  else if (argc == 9) ToyModelTreeReader(iFileName, oFileName, inputFileNameFromFirstRun, 0, inputFileNameFromSecondRun, 1);

  return 0;
}
