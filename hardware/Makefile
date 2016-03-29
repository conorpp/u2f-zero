
name=u2f-zero
gers=gerbers
files=$(wildcard $(gers)/*)

compress:
	zip $(name)-gerbers.zip $(files)

rename:
	mv $(gers)/$(name)-F.Cu.gbr $(gers)/$(name).GTL
	mv $(gers)/$(name)-B.Cu.gbr $(gers)/$(name).GBL
	mv $(gers)/$(name)-F.Mask.gbr $(gers)/$(name).GTS
	mv $(gers)/$(name)-B.Mask.gbr $(gers)/$(name).GBS
	mv $(gers)/$(name)-F.SilkS.gbr $(gers)/$(name).GTO
	mv $(gers)/$(name)-B.SilkS.gbr $(gers)/$(name).GBO
	mv $(gers)/$(name)-Edge.Cuts.gbr $(gers)/$(name).GKO
	mv $(gers)/$(name).drl $(gers)/$(name).TXT

clean:
	rm -rf $(gers)/* *.zip
