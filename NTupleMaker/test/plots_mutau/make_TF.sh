
#systematics="Nominal JetEnUp JetEnDown TauEnUp TauEnDown ElEnUp ElEnDown MuEnUp MuEnDown"
#systematics="Nominal JetEnUp JetEnDown TopPtUp TopPtDown ZPtUp ZPtDown TauEnUp TauEnDown ElEnUp ElEnDown MuEnUp MuEnDown UnclEnUp UnclEnDown ScalesDown ScalesUp PDFUp PDFDown TFRJetEnUp TFRJetEnDown TFRMuEnUp TFRMuEnDown TFRTauEnUp TFRTauEnDown METUp METDown BTagUp BTagDown EWKUp EWKDown"

#systematics="Nominal JetEnUp JetEnDown TopPtUp TopPtDown ZPtUp ZPtDown TauEnUp TauEnDown ElEnUp ElEnDown MuEnUp MuEnDown UnclEnUp UnclEnDown ScalesDown ScalesUp PDFUp PDFDown METUp METDown BTagUp BTagDown EWKUp EWKDown METRecoilUp METRecoilDown"
systematics="Nominal JetEnUp JetEnDown TauEnUp TauEnDown ElEnUp ElEnDown MuEnUp MuEnDown UnclEnUp UnclEnDown BTagUp BTagDown"



#systematics="Nominal"




for syst in $systematics
do

		root -l -q -b 'OverlapTF.C("'$syst'")' 
done

