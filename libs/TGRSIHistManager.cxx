


#include <TGRSIHistManager.h>

ClassImp(TGRSIHistManager)

TGRSIHistManager::TGRSIHistManager(){ }

TGRSIHistManager::~TGRSIHistManager() { }

std::map<TH1*,TGRSIHistManager*> TGRSIHistManager::MasterHistManagerMap;

TGRSIHistManager* TGRSIHistManager::GetHistManagerFromHist(TH1* hist){
   TGRSIHistManager* ghm = 0;
   
   printf("\tgetting ghm for = 08%p\n",hist);
   if(MasterHistManagerMap.count(hist) == 1) {
      ghm = MasterHistManagerMap.at(hist);
   }

   return ghm;
}



void TGRSIHistManager::InsertHist(TObject *obj,Option_t *opt) {
   if(!obj || !obj->InheritsFrom("TH1"))
      return;

   obj = obj->Clone();

   TH1 *hist = (TH1*)obj;
   GRSIHistInfo *info = 0;
   if(info = FindHistInfoByName(hist->GetName())) {
      ReplaceHist(info,hist);
   } else if (info = FindHistInfoByTitle(hist->GetTitle())) {
      ReplaceHist(info,hist);
   }
   
   info = new GRSIHistInfo(hist->GetName(),hist->GetTitle());
   if(hist->InheritsFrom("TH3"))
      info->dimension = 3;
   else if(hist->InheritsFrom("TH2"))
      info->dimension = 2;
  
   printf("\tobj = 08%p\n",hist);
   MasterHistManagerMap.insert(std::make_pair(hist,this));

   fGRSIHistMap.insert(std::make_pair(info,hist));
   return;
}


void TGRSIHistManager::RemoveHist(TObject *obj, Option_t *opt) {
   if(!obj || !obj->InheritsFrom("TH1"))
      return;
   GRSIHistInfo *info = 0;
   TH1 *hist = (TH1*)obj;
   if(info = FindHistInfoByName(hist->GetName())) {  } 
   else if (info = FindHistInfoByTitle(hist->GetTitle())) {   }
   
   if(info) {
      fGRSIHistMap.erase(info);
      delete info;
      hist->Delete();
   };

   return;
}

void TGRSIHistManager::RemoveAll(Option_t *opt) {
   GRSIHistMap::iterator it;   
   for(it = fGRSIHistMap.begin();it != fGRSIHistMap.end(); it++) {
      TObject *obj = it->second;
      if(obj) obj->Delete();
      if(it->first)  delete it->first;
   }
   fGRSIHistMap.clear();
   return;
}

void TGRSIHistManager::ReplaceHist(GRSIHistInfo *info,TObject *obj) {
   if(!info || !obj)
      return;
   TObject *toremove = fGRSIHistMap.at(info);
   if(toremove)
      toremove->Delete();
   fGRSIHistMap.at(info) = obj;
   return;
}

GRSIHistInfo *TGRSIHistManager::FindHistInfoByName(const char *cc_name) {
   std::string name = cc_name;
   if(name.length()==0)
      return 0;
   GRSIHistMap::iterator it;   
   GRSIHistInfo *found = 0;
   for(it = fGRSIHistMap.begin();it != fGRSIHistMap.end(); it++) {
     if(name.compare(it->first->name)==0) {
        found = it->first;  
        break;
     }
   }
   return found;
}

GRSIHistInfo *TGRSIHistManager::FindHistInfoByTitle(const char *cc_title) {
   std::string title = cc_title;
   if(title.length()==0)
      return 0;
   GRSIHistMap::iterator it;   
   GRSIHistInfo *found = 0;
   for(it = fGRSIHistMap.begin();it != fGRSIHistMap.end(); it++) {
     if(title.compare(it->first->title)==0) {
        found = it->first;  
        break;
     }
   }
   return found;
}


TH1 *TGRSIHistManager::FindHistByName(const char *cc_name) {
   std::string name = cc_name;
   if(name.length()==0)
      return 0;
   GRSIHistMap::iterator it;   
   TObject *found = 0;
   for(it = fGRSIHistMap.begin();it != fGRSIHistMap.end(); it++) {
     if(name.compare(it->first->name)==0) {
        found = it->second;  
        break;
     }
   }
   return (TH1*)found;
}

TH1 *TGRSIHistManager::FindHistByTitle(const char *cc_title) {
   std::string title = cc_title;
   if(title.length()==0)
      return 0;
   GRSIHistMap::iterator it;   
   TObject *found = 0;
   for(it = fGRSIHistMap.begin();it != fGRSIHistMap.end(); it++) {
     if(title.compare(it->first->title)==0) {
        found = it->second;  
        break;
     }
   }
   return (TH1*)found;
}

void TGRSIHistManager::AddXMarker(const char* name, Int_t bin){
   TH1 *hist = FindHistByName(name);
   if(!hist){ return;}

   GRSIHistInfo *info = FindHistInfoByName(name);
   printf("info = %p\n",info);
   if(info->xmarkers.size() == 6){
      UndrawXMarker(info->xmarkers.front(),hist);
      info->xmarkers.pop_front();
   }

   info->xmarkers.push_back(bin);
   DrawXMarker(bin,hist);
}

void TGRSIHistManager::UndrawXMarker(Int_t bin, TH1* hist){
    printf("Removing the marker at bin %d\n",bin);
}

void TGRSIHistManager::DrawXMarker(Int_t bin, TH1* hist){
    printf("Drawing a line on bin %d\n",bin);

   //need to do these lines for weird gPad pointer issues.
   TCanvas *canvas = (TCanvas*)gTQSender;
   canvas->cd();
   //Int_t ymax = hist->GetNbinsY();
   Int_t ymin, ymax;

   TPaveStats *statsbox = (TPaveStats*)(hist->FindObject("stats"));

   TFrame *frame = (TFrame*)canvas->GetPrimitive("TFrame");
   ymin = frame->GetY1();
   ymax = frame->GetY2();

/*
   gVirtualX->SetDrawMode(TVirtualX::kXor);
   gVirtualX->SetOpacity(100);
gPad->GetCanvas()->FeedbackMode(kTRUE);
      int px = gPad->GetEventX();
         int py = gPad->GetEventY();
            float uxmin = gPad->GetUxmin();
               float uxmax = gPad->GetUxmax();
                  int pxmin = gPad->XtoAbsPixel(uxmin);
                     int pxmax = gPad->XtoAbsPixel(uxmax);
                           gVirtualX->DrawLine(pxmin,py,pxmax,py);
                           */
                           
   TLine *line = new TLine(bin,ymin,bin,ymax);
   line->SetLineColor(kRed);
   line->Draw();

   if(statsbox){
      statsbox->Pop();
      statsbox->Draw();
   }
   canvas->Update();

/*
gPad->GetCanvas()->FeedbackMode(kTRUE);

   int px = bin;
   float uymin = canvas->GetUymin();
   float uymax = canvas->GetUymax();
   int pymin   = canvas->YtoAbsPixel(uymin);
   int pymax =   canvas->YtoAbsPixel(uymax);
    gVirtualX->DrawLine(bin,pymin,bin,pymax);
   gPad->SetUniqueID(px);
   Float_t upx = canvas->AbsPixeltoX(px);
   Float_t y = canvas->PadtoX(upx);
    canvas->Update();
*/
                        //line->DrawLine(bin,pymin,bin,pymax);
}








