void Drawv2ptMC(){
  TFile *f = new TFile("TGraphError_pure_10mil.root","read");
  gROOT->SetStyle("Pub");
  gStyle->SetPalette(kDarkRainBow);
  TGraphErrors *gr = (TGraphErrors*) f -> Get("gr_3_0");
  gr -> SetMarkerStyle(20);
  gr -> SetMarkerSize(1.8);
  double xmin=0;
  double xmax=3.5;
  double ymin=0;
  double ymax=0.255;
  TCanvas *c = new TCanvas("c","canvas",200,10,800,600);

  c->SetBottomMargin(0.15);
  c->SetLeftMargin(0.15);

	gROOT->ForceStyle();
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
	gStyle->SetErrorX(0);
  gStyle -> SetOptStat(0);  


  TH2F *h = new TH2F("h","",5,xmin,xmax,5,ymin,ymax);
  h -> Draw();
  h -> SetXTitle("p_{T}, GeV/c");
  h -> SetYTitle("v_{2}");
  gr-> Draw("P");

  TPaveText *pt = new TPaveText(0.55,0.72,0.55+0.33,0.89,"NDC NB"); // right corner 0.56,0.72,0.89,0.89
  pt->SetBorderSize(0);
  pt->SetFillColor(0);
  pt->AddText("MC toy model");
  pt->AddText("Centrality: 30-40%");
  pt->Draw();
  c->SaveAs("v2ptpure.png");
  TCanvas *c2 = new TCanvas("c2","canvas",200,10,800,600);
  TFile *f2 = new TFile("pure_10mil.root","read");
  TH1F *hPt = (TH1F*)f2->Get("hPt");
  c2->SetLogy();
  hPt ->GetXaxis()->SetRangeUser(0.,4.);
  hPt -> Draw();
  c2 -> SaveAs("ptdis.png");
}