#include <TFile.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TSystem.h>
#include <TROOT.h>
#include <iostream>
#include <vector>
#include <string>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uzycie: " << argv[0] << " plik1.root plik2.root ... [-o wyjscie.png]" << std::endl;
        return 1;
    }

    std::vector<std::string> files;
    std::string outName = "EnergyVetoed_overlay.png";
    TString histname("EnergyVetoed");

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-o" && i + 1 < argc) {
            outName = argv[++i];
        } else if (arg == "-h" && i + 1 < argc) {
            histname = TString(argv[++i]);
        } else {
            files.push_back(arg);
        }
    }

    if (files.empty()) {
        std::cerr << "Nie podano zadnych plikow .root!" << std::endl;
        return 1;
    }

    TCanvas *c = new TCanvas("c_" + histname, histname, 900, 700);
    TLegend *leg = new TLegend(0.35, 0.65, 0.80, 0.89);
    leg->SetBorderSize(0);

    int colors[] = {
        kRed+1, kBlue+1, kGreen+2, kMagenta+1, kOrange+1, kCyan+2,
        kBlack, kViolet+1, kSpring+3, kAzure+2, kPink+1, kTeal+2
    };
    int nColors = sizeof(colors) / sizeof(colors[0]);

    double maxY = 0;
    std::vector<TH1*> hists;

    //Full spectrum histogram
    TFile *f = TFile::Open(files[0].c_str());
    if (!f || f->IsZombie()) {
        std::cerr << "Nie mozna otworzyc pliku: " << files[0] << std::endl;
    }

    TH1 *h = dynamic_cast<TH1*>(f->Get("Energy"));
    if (!h) {
        std::cerr << "Brak histogramu 'Energy' w pliku: " << files[0] << std::endl;
        f->Close();
    }

    h->SetDirectory(0);
    f->Close();

    h->SetLineColor(kBlack);
    h->SetLineWidth(2);
    h->SetStats(0);
    h->SetTitle("Energy");
    leg->AddEntry(h, "Full spectrum 6mm", "l");
    hists.push_back(h);
    maxY = h->GetMaximum();
    



    for (size_t i = 0; i < files.size(); ++i) {
        TFile *f = TFile::Open(files[i].c_str());
        if (!f || f->IsZombie()) {
            std::cerr << "Nie mozna otworzyc pliku: " << files[i] << std::endl;
            continue;
        }

        TH1 *h = dynamic_cast<TH1*>(f->Get(histname));
        if (!h) {
            std::cerr << "Brak histogramu "+ histname +"  w pliku: " << files[i] << std::endl;
            f->Close();
            continue;
        }

        h->SetDirectory(0);
        f->Close();

        int color = colors[hists.size() % nColors];
        h->SetLineColor(color);
        h->SetLineWidth(2);
        h->SetStats(0);
        h->SetTitle("histname");

        if (h->GetMaximum() > maxY) maxY = h->GetMaximum();

        leg->AddEntry(h, gSystem->BaseName(files[i].c_str()), "l");
        hists.push_back(h);
    }

    if (hists.empty()) {
        std::cerr << "Nie udalo sie wczytac zadnego histogramu." << std::endl;
        return 1;
    }

    hists[0]->SetMaximum(maxY * 1.15);
    hists[0]->SetMinimum(1);
    hists[0]->Draw("HIST");
    for (size_t i = 1; i < hists.size(); ++i) {
        hists[i]->Draw("HIST SAME");
    }

    c->SetLogy();
    leg->Draw();
    c->SaveAs(outName.c_str());

    std::cout << "Zapisano: " << outName << std::endl;

    return 0;
}